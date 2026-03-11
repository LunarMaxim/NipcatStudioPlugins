// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Retargeter/IKRetargetOps.h"
#include "Retargeter/RetargetOps/PinBoneOp.h"

#include "IKRetargetRetargetWeaponBoneOp.generated.h"

#define LOCTEXT_NAMESPACE "RetargetOps"

USTRUCT(BlueprintType)
struct FRetargetWeaponBoneData
{
	GENERATED_BODY()

	FRetargetWeaponBoneData() = default;

	UPROPERTY(EditAnywhere, Category=Settings, meta=(ReinitializeOnEdit))
	FBoneReference WeaponBone;

	UPROPERTY(meta=(ReinitializeOnEdit))
	FBoneReference HandBone;

	UPROPERTY(EditAnywhere, Category=Settings, meta=(ReinitializeOnEdit))
	FBoneReference SourceWeaponBone;

	UPROPERTY(EditAnywhere, Category=Settings, meta=(ReinitializeOnEdit))
	FBoneReference SourceHandBone;

	UPROPERTY(EditAnywhere, Category=Settings, meta=(ReinitializeOnEdit))
	bool EnableWeaponLocationIK;

	UPROPERTY(EditAnywhere, Category=Settings, meta=(ReinitializeOnEdit))
	bool EnableWeaponRotationIK;

	UPROPERTY(EditAnywhere, Category=Settings, meta=(ReinitializeOnEdit))
	double EnableWeaponIKMinDistance;

	UPROPERTY(EditAnywhere, Category=Settings, meta=(ReinitializeOnEdit))
	double EnableWeaponIKMaxDistance;

};

USTRUCT(BlueprintType, meta = (DisplayName = "Retarget Weapon Bone Settings"))
struct FIKRetargetRetargetWeaponBoneOpSettings : public FIKRetargetOpSettingsBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category="OpSettings", meta=(ReinitializeOnEdit))
	TArray<FRetargetWeaponBoneData> BonesToRetarget;

	// A manual offset to apply in global space
	UPROPERTY(EditAnywhere, Category=Settings)
	FTransform GlobalOffset;

	// A manual offset to apply in local space
	UPROPERTY(EditAnywhere, Category=Settings)
	FTransform LocalOffset;

	virtual const UClass* GetControllerType() const override;

	virtual void CopySettingsAtRuntime(const FIKRetargetOpSettingsBase* InSettingsToCopyFrom) override;
};

/**
 * 
 */
USTRUCT(BlueprintType, meta=(DisplayName="Retarget Weapon Bone"))
struct  NIPCATBASICGAMEPLAY_API FIKRetargetRetargetWeaponBoneOp : public FIKRetargetOpBase
{
	GENERATED_BODY()

public:
	virtual bool Initialize(const FIKRetargetProcessor& InProcessor, const FRetargetSkeleton& InSourceSkeleton,
	                        const FTargetSkeleton& InTargetSkeleton, const FIKRetargetOpBase* InParentOp,
	                        FIKRigLogger& Log) override;

	virtual void Run(FIKRetargetProcessor& InProcessor, const double InDeltaTime,
	                 const TArray<FTransform>& InSourceGlobalPose, TArray<FTransform>& OutTargetGlobalPose) override;

	virtual const UScriptStruct* GetType() const override;
	virtual FIKRetargetOpSettingsBase* GetSettings() override;
	virtual const UScriptStruct* GetSettingsType() const override;
	
	UPROPERTY()
	FIKRetargetRetargetWeaponBoneOpSettings Settings;

#if WITH_EDITOR
	virtual FText GetWarningMessage() const override { return Message; };
	FText Message;
#endif
};

/* The blueprint/python API for editing a Copy Base Pose Op */
UCLASS(MinimalAPI, BlueprintType)
class UIKRetargetRetargetWeaponBoneController : public UIKRetargetOpControllerBase
{
	GENERATED_BODY()
	
public:
	/* Get the current op settings as a struct.
	 * @return FIKRetargetCopyBasePoseOpSettings struct with the current settings used by the op. */
	UFUNCTION(BlueprintCallable, Category = Settings)
	FIKRetargetRetargetWeaponBoneOpSettings GetSettings();

	/* Set the op settings. Input is a custom struct type for this op.
	 * @param InSettings a FIKRetargetCopyBasePoseOpSettings struct containing all the settings to apply to this op */
	UFUNCTION(BlueprintCallable, Category = Settings)
	void SetSettings(FIKRetargetRetargetWeaponBoneOpSettings InSettings);
};

UCLASS()
class NIPCATBASICGAMEPLAY_API URetargetWeaponBoneOp : public URetargetOpBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category="OpSettings")
	TArray<FRetargetWeaponBoneData> BonesToRetarget;

	// A manual offset to apply in global space
	UPROPERTY(EditAnywhere, Category=Settings)
	FTransform GlobalOffset;

	// A manual offset to apply in local space
	UPROPERTY(EditAnywhere, Category=Settings)
	FTransform LocalOffset;

	virtual void ConvertToInstancedStruct(FInstancedStruct& OutInstancedStruct) override
	{
		OutInstancedStruct.InitializeAs(FIKRetargetRetargetWeaponBoneOp::StaticStruct());
		FIKRetargetRetargetWeaponBoneOp& NewOp = OutInstancedStruct.GetMutable<FIKRetargetRetargetWeaponBoneOp>();
		NewOp.SetEnabled(bIsEnabled);
		NewOp.Settings.BonesToRetarget = BonesToRetarget;
		NewOp.Settings.GlobalOffset = GlobalOffset;
		NewOp.Settings.LocalOffset = LocalOffset;
	}
};

#undef LOCTEXT_NAMESPACE
