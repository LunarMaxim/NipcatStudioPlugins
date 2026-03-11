// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "NipcatComboDefinition.generated.h"

struct FGameplayAbilitySpecHandle;
class UAbilitySystemComponent;
class UGameplayAbility;


USTRUCT(BlueprintType)
struct NIPCATABILITYSYSTEM_API FNipcatComboDefinition
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSoftClassPtr<UGameplayAbility> Ability = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(MultiLine))
	FString Comment;

	UPROPERTY()
	bool bCommentModified;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bConsumeInput = false;

};

/**
 * 
 */
UCLASS(BlueprintType)
class NIPCATABILITYSYSTEM_API UNipcatComboDefinitionConfig : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Priority = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(Categories="Combo", ForceInlineRow, TitleProperty="Comment"))
	TMap<FGameplayTag, FNipcatComboDefinition> ComboDefinitions;

#if WITH_EDITOR
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
	virtual void PostLoad() override;
#endif
};

USTRUCT(BlueprintType)
struct NIPCATABILITYSYSTEM_API FNipcatComboDefinitionHandle
{
	GENERATED_BODY()

	FNipcatComboDefinitionHandle(){}

	FNipcatComboDefinitionHandle(const UNipcatComboDefinitionConfig* InComboDefinitionConfig)
	{
		ComboDefinitionConfig = InComboDefinitionConfig;
	}

	UPROPERTY(BlueprintReadOnly)
	const UNipcatComboDefinitionConfig* ComboDefinitionConfig = nullptr;

	UPROPERTY(BlueprintReadOnly)
	TArray<FGameplayAbilitySpecHandle> AbilityHandles;

	bool IsValid() const
	{
		return ComboDefinitionConfig != nullptr;
	}
	
	bool operator==(const FNipcatComboDefinitionHandle& Other) const
	{
		return ComboDefinitionConfig == Other.ComboDefinitionConfig && AbilityHandles == Other.AbilityHandles;
	}
};
