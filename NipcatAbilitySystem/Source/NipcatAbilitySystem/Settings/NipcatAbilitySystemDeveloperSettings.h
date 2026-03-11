// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "Engine/DeveloperSettings.h"
#include "GameplayTags.h"
#include "NipcatAbilitySystem/Attributes/NipcatAttributeSet.h"
#include "NipcatAbilitySystemDeveloperSettings.generated.h"

class UProxyTable;
class UGameplayEffect;
class UNipcatResourceConfig;
/**
 * 
 */
UCLASS(Config=NipcatAbilitySystem, DisplayName="NipcatAbilitySystem", DefaultConfig, BlueprintType)
class NIPCATABILITYSYSTEM_API UNipcatAbilitySystemDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UNipcatAbilitySystemDeveloperSettings();

	UPROPERTY(Config, BlueprintReadOnly, EditDefaultsOnly, Category="Attribute")
	TSoftObjectPtr<UNipcatResourceConfig> ResourceConfigPath;

	UPROPERTY(Config, BlueprintReadOnly, EditDefaultsOnly, Category="Attribute")
	TArray<TSoftClassPtr<UNipcatAttributeSet>> AttributeSets;

	UPROPERTY(Config, BlueprintReadOnly, EditDefaultsOnly, Category="Attribute")
	TArray<TSoftClassPtr<UGameplayEffect>> PostAttributeInitAppliedEffects;

protected:
	UPROPERTY(Transient)
	mutable TSubclassOf<UGameplayEffect> DamageGameplayEffect;
	
	UPROPERTY(Transient)
	mutable UProxyTable* DamageResultProxyTable;

public:
	UPROPERTY(Config, EditDefaultsOnly, Category="Damage", meta=(AllowedClasses="/Script/GameplayAbilities.GameplayEffect"))
	FSoftClassPath DamageGameplayEffectPath;
	
	UPROPERTY(Config, EditDefaultsOnly, Category="Damage", meta=(AllowedClasses="/Script/ProxyTable.ProxyTable"))
	TSoftObjectPtr<UProxyTable> DamageResultProxyTablePath;
	
	UPROPERTY(Config, BlueprintReadOnly, EditDefaultsOnly, Category="Tags")
	FGameplayTag WalkStateTag;

	UPROPERTY(Config, BlueprintReadOnly, EditDefaultsOnly, Category="Tags")
	FGameplayTag ForceWalkStateTag;

	UPROPERTY(Config, BlueprintReadOnly, EditDefaultsOnly, Category="Tags")
	FGameplayTag RunStateTag;

	UPROPERTY(Config, BlueprintReadOnly, EditDefaultsOnly, Category="Tags")
	FGameplayTag SprintStateTag;

	UPROPERTY(Config, BlueprintReadOnly, EditDefaultsOnly, Category="Tags")
	FGameplayTag StandingStateTag;

	UPROPERTY(Config, BlueprintReadOnly, EditDefaultsOnly, Category="Tags")
	FGameplayTag CrouchingStateTag;

	UPROPERTY(Config, BlueprintReadOnly, EditDefaultsOnly, Category="Tags")
	FGameplayTag JumpStateTag;

	UPROPERTY(Config, BlueprintReadOnly, EditDefaultsOnly, Category="Tags")
	FGameplayTag InAirStateTag;

	UPROPERTY(Config, BlueprintReadOnly, EditDefaultsOnly, Category="Tags")
	FGameplayTag ForwardFacingStateTag;

	UPROPERTY(Config, BlueprintReadOnly, EditDefaultsOnly, Category="Tags")
	FGameplayTag StrafeStateTag;

	UPROPERTY(Config, BlueprintReadOnly, EditDefaultsOnly, Category="Tags")
	FGameplayTag FocusStateTag;


	/** Gets the settings container name for the settings, either Project or Editor */
	virtual FName GetContainerName() const override { return FName("Project"); }
	/** Gets the category for the settings, some high level grouping like, Editor, Engine, Game...etc. */
	virtual FName GetCategoryName() const override { return FName("NipcatStudio"); }
	/** The unique name for your section of settings, uses the class's FName. */
	virtual FName GetSectionName() const override { return FName("Attribute"); }

	UFUNCTION(BlueprintPure, Category="NipcatAbilitySystem|DeveloperSettings")
	static TSubclassOf<UGameplayEffect> GetDamageGameplayEffect();
	
	UFUNCTION(BlueprintPure, Category="NipcatAbilitySystem|DeveloperSettings", meta=(BlueprintThreadSafe))
	UProxyTable* GetDamageResultProxyTable() const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="NipcatAbilitySystem|DeveloperSettings", DisplayName="GetNipcatAbilitySystemGlobalConfig")
	static const UNipcatAbilitySystemDeveloperSettings* Get()
	{
		return GetDefault<UNipcatAbilitySystemDeveloperSettings>();
	}
};
