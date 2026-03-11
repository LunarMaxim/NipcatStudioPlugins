// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatAbilitySystemDeveloperSettings.h"

#include "ProxyTable.h"

UNipcatAbilitySystemDeveloperSettings::UNipcatAbilitySystemDeveloperSettings()
{
	ResourceConfigPath = TSoftObjectPtr<UNipcatResourceConfig>(FSoftObjectPath("/NipcatAbilitySystem/Config/DefaultResourceConfig.DefaultResourceConfig"));
	DamageGameplayEffectPath = FSoftClassPath("/NipcatAbilitySystem/GameplayEffect/GE_NipcatDamageBase.GE_NipcatDamageBase_C");
	DamageResultProxyTablePath = TSoftObjectPtr<UProxyTable>(FSoftObjectPath("/NipcatAbilitySystem/Config/CPT_DefaultDamageResult.CPT_DefaultDamageResult"));
	
	WalkStateTag = FGameplayTag::RequestGameplayTag(TEXT("State.Locomotion.Gait.Walk"));
	ForceWalkStateTag = FGameplayTag::RequestGameplayTag(TEXT("Input.State.System.ForceWalk"));
	RunStateTag = FGameplayTag::RequestGameplayTag(TEXT("State.Locomotion.Gait.Run"));
	SprintStateTag = FGameplayTag::RequestGameplayTag(TEXT("State.Locomotion.Gait.Sprint"));
	StandingStateTag = FGameplayTag::RequestGameplayTag(TEXT("State.Locomotion.Stance.Standing"));
	CrouchingStateTag = FGameplayTag::RequestGameplayTag(TEXT("State.Locomotion.Stance.Crouching"));
	JumpStateTag = FGameplayTag::RequestGameplayTag(TEXT("State.Jump"));
	InAirStateTag = FGameplayTag::RequestGameplayTag(TEXT("State.InAir"));
	ForwardFacingStateTag = FGameplayTag::RequestGameplayTag(TEXT("State.Locomotion.RotationMode.ForwardFacing"));
	StrafeStateTag = FGameplayTag::RequestGameplayTag(TEXT("State.Locomotion.RotationMode.Strafe"));
	FocusStateTag = FGameplayTag::RequestGameplayTag(TEXT("State.Focus"));
}

TSubclassOf<UGameplayEffect> UNipcatAbilitySystemDeveloperSettings::GetDamageGameplayEffect()
{
	if (const auto Settings = Get())
	{
		TSubclassOf<UGameplayEffect> LoadedGameplayEffect = Settings->DamageGameplayEffect;
		if (!LoadedGameplayEffect)
		{
			LoadedGameplayEffect = Settings->DamageGameplayEffect = Settings->DamageGameplayEffectPath.TryLoadClass<UGameplayEffect>();
		}
		Settings->DamageGameplayEffect->AddToRoot();
		return LoadedGameplayEffect;
	}
	return nullptr;
}

UProxyTable* UNipcatAbilitySystemDeveloperSettings::GetDamageResultProxyTable() const
{
	UProxyTable* LoadedProxyTable = DamageResultProxyTable;
	if (!LoadedProxyTable)
	{
		LoadedProxyTable = DamageResultProxyTable = DamageResultProxyTablePath.LoadSynchronous();
	}
	DamageResultProxyTable->AddToRoot();
	return LoadedProxyTable;
}
