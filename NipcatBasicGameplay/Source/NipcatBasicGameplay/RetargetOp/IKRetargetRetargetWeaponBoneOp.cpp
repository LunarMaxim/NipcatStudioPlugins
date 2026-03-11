// Copyright Nipcat Studio All Rights Reserved.


#include "IKRetargetRetargetWeaponBoneOp.h"
#include "Retargeter/IKRetargetProcessor.h"

#define LOCTEXT_NAMESPACE "URetargetWeaponBoneOp"

const UClass* FIKRetargetRetargetWeaponBoneOpSettings::GetControllerType() const
{
	return UIKRetargetRetargetWeaponBoneController::StaticClass();
}

void FIKRetargetRetargetWeaponBoneOpSettings::CopySettingsAtRuntime(
	const FIKRetargetOpSettingsBase* InSettingsToCopyFrom)
{
	const TArray<FName> PropertiesToIgnore = {"BonesToRetarget"};
	FIKRetargetOpBase::CopyStructProperties(
		FIKRetargetRetargetWeaponBoneOpSettings::StaticStruct(),
		InSettingsToCopyFrom,
		this,
		PropertiesToIgnore);
}

bool FIKRetargetRetargetWeaponBoneOp::Initialize(const FIKRetargetProcessor& InProcessor,
                                       const FRetargetSkeleton& InSourceSkeleton, const FTargetSkeleton& InTargetSkeleton,
                                       const FIKRetargetOpBase* InParentOp, FIKRigLogger& Log)
{
	bIsInitialized = true;
	bool bFullyInitialized = true;
	for (FRetargetWeaponBoneData& BoneToRetarget : Settings.BonesToRetarget)
	{
		BoneToRetarget.WeaponBone.BoneIndex = InTargetSkeleton.FindBoneIndexByName(BoneToRetarget.WeaponBone.BoneName);
		BoneToRetarget.HandBone.BoneIndex = InTargetSkeleton.GetParentIndex(BoneToRetarget.WeaponBone.BoneIndex);
		BoneToRetarget.SourceWeaponBone.BoneIndex = InSourceSkeleton.FindBoneIndexByName(BoneToRetarget.SourceWeaponBone.BoneName);
		BoneToRetarget.SourceHandBone.BoneIndex = InSourceSkeleton.FindBoneIndexByName(BoneToRetarget.SourceHandBone.BoneName);

		const bool bFoundWeaponBone = BoneToRetarget.WeaponBone.BoneIndex != INDEX_NONE;
		const bool bFoundSourceWeaponBone = BoneToRetarget.SourceWeaponBone.BoneIndex != INDEX_NONE;
		const bool bFoundSourceHandBone = BoneToRetarget.SourceHandBone.BoneIndex != INDEX_NONE;
		if (!bFoundWeaponBone)
		{
			bFullyInitialized = false;
			Log.LogWarning(FText::Format(
				LOCTEXT("MissingSourceBone", "Retarget Weapon Bone op refers to non-existant weapon bone, {0}."),
				FText::FromName(BoneToRetarget.WeaponBone.BoneName)));
		}

		if (!bFoundSourceWeaponBone)
		{
			bFullyInitialized = false;
			Log.LogWarning(FText::Format(
				LOCTEXT("MissingTargetBone", "Retarget Weapon Bone op refers to non-existant source weapon bone, {0}."),
				FText::FromName(BoneToRetarget.SourceWeaponBone.BoneName)));
		}

		if (!bFoundSourceHandBone)
		{
			bFullyInitialized = false;
			Log.LogWarning(FText::Format(
				LOCTEXT("MissingTargetBone", "Retarget Weapon Bone op refers to non-existant source hand bone, {0}."),
				FText::FromName(BoneToRetarget.SourceWeaponBone.BoneName)));
		}
		
	}

#if WITH_EDITOR
	if (bFullyInitialized)
	{
		Message = FText::Format(LOCTEXT("ReadyToRun", "Retargeting {0} weapon bones."), FText::AsNumber(Settings.BonesToRetarget.Num()));
	}
	else
	{
		Message = FText(LOCTEXT("MissingBonesWarning", "Bone(s) not found. See output log."));
	}
#endif

	// always treat this op as "initialized", individual pins will only execute if their prerequisites are met
	return true;
}

void FIKRetargetRetargetWeaponBoneOp::Run(FIKRetargetProcessor& InProcessor, const double InDeltaTime,
	const TArray<FTransform>& InSourceGlobalPose, TArray<FTransform>& OutTargetGlobalPose)
{
	for (const FRetargetWeaponBoneData& BoneToRetarget : Settings.BonesToRetarget)
	{
		if (BoneToRetarget.WeaponBone.BoneIndex == INDEX_NONE || BoneToRetarget.SourceWeaponBone.BoneIndex == INDEX_NONE || BoneToRetarget.SourceHandBone.BoneIndex == INDEX_NONE || BoneToRetarget.HandBone.BoneIndex == INDEX_NONE)
		{
			continue; // disabled or not successfully initialized
		}

		// apply result
		
		const FTransform& HandBoneTransform = OutTargetGlobalPose[BoneToRetarget.HandBone.BoneIndex];
		const FTransform& SourceHandBoneTransform = InSourceGlobalPose[BoneToRetarget.SourceHandBone.BoneIndex];
		const FTransform& SourceWeaponBoneTransform = InSourceGlobalPose[BoneToRetarget.SourceWeaponBone.BoneIndex];

		FTransform WeaponTransform = SourceWeaponBoneTransform.GetRelativeTransform(SourceHandBoneTransform) * HandBoneTransform;

		if (BoneToRetarget.EnableWeaponLocationIK || BoneToRetarget.EnableWeaponRotationIK)
		{
			if (BoneToRetarget.EnableWeaponIKMinDistance > 0 || BoneToRetarget.EnableWeaponIKMaxDistance > 0)
			{
				const double Distance = FVector::Dist(SourceWeaponBoneTransform.GetLocation(), SourceHandBoneTransform.GetLocation());
				double IKAlpha = FMath::GetMappedRangeValueClamped(FVector2d(BoneToRetarget.EnableWeaponIKMinDistance, BoneToRetarget.EnableWeaponIKMaxDistance), FVector2d(0, 1), Distance);
				if (BoneToRetarget.EnableWeaponLocationIK)
				{
					WeaponTransform.SetLocation(FMath::Lerp(WeaponTransform.GetLocation(), SourceWeaponBoneTransform.GetLocation(), IKAlpha));
				}
				if (BoneToRetarget.EnableWeaponRotationIK)
				{
					WeaponTransform.SetRotation(FMath::Lerp(WeaponTransform.GetRotation(), SourceWeaponBoneTransform.GetRotation(), IKAlpha));
				}
			}
		}
		
		FTransform& WeaponBoneTransform = OutTargetGlobalPose[BoneToRetarget.WeaponBone.BoneIndex];
		WeaponBoneTransform = Settings.LocalOffset * WeaponTransform * Settings.GlobalOffset;
	}
}

const UScriptStruct* FIKRetargetRetargetWeaponBoneOp::GetType() const
{
	return FIKRetargetRetargetWeaponBoneOp::StaticStruct();
}

FIKRetargetOpSettingsBase* FIKRetargetRetargetWeaponBoneOp::GetSettings()
{
	return &Settings;
}

const UScriptStruct* FIKRetargetRetargetWeaponBoneOp::GetSettingsType() const
{
	return FIKRetargetRetargetWeaponBoneOpSettings::StaticStruct();
}

FIKRetargetRetargetWeaponBoneOpSettings UIKRetargetRetargetWeaponBoneController::GetSettings()
{
	return *reinterpret_cast<FIKRetargetRetargetWeaponBoneOpSettings*>(OpSettingsToControl);
}

void UIKRetargetRetargetWeaponBoneController::SetSettings(FIKRetargetRetargetWeaponBoneOpSettings InSettings)
{
	OpSettingsToControl->CopySettingsAtRuntime(&InSettings);
}


#undef LOCTEXT_NAMESPACE
