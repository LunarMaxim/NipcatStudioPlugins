// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameplayAbilitySpec.h"
#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"
#include "Engine/DataAsset.h"

#include "NipcatAbilitySet.generated.h"


class UNipcatAbilitySystemComponent;
class UNipcatGameplayAbility;
class UGameplayEffect;


USTRUCT(BlueprintType)
struct NIPCATABILITYSYSTEM_API FNipcatAbilitySet_GameplayAbility
{
	GENERATED_BODY()

public:

	// Gameplay ability to grant.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSoftClassPtr<UNipcatGameplayAbility> Ability = nullptr;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, meta=(MultiLine))
	FString Comment;

	UPROPERTY()
	bool bCommentModified;
#endif
	
	// Level of ability to grant.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 AbilityLevel = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool InitialActivate = false;

};


/**
 * FNipcatAbilitySet_GameplayEffect
 *
 *	Data used by the ability set to grant gameplay effects.
 */
USTRUCT(BlueprintType)
struct NIPCATABILITYSYSTEM_API FNipcatAbilitySet_GameplayEffect
{
	GENERATED_BODY()

public:

	// Gameplay effect to grant.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftClassPtr<UGameplayEffect> GameplayEffect = nullptr;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, meta=(MultiLine))
	FString Comment;

	UPROPERTY()
	bool bCommentModified;
#endif
	
	// Level of gameplay effect to grant.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float EffectLevel = 1.0f;
	
};

/**
 * FNipcatAbilitySet_AttributeSet
 *
 *	Data used by the ability set to grant attribute sets.
 */
USTRUCT(BlueprintType)
struct NIPCATABILITYSYSTEM_API FNipcatAbilitySet_AttributeSet
{
	GENERATED_BODY()

public:
	// Gameplay effect to grant.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UAttributeSet> AttributeSet;

};

/**
 * FNipcatAbilitySet_GrantedHandles
 *
 *	Data used to store handles to what has been granted by the ability set.
 */
USTRUCT(BlueprintType)
struct NIPCATABILITYSYSTEM_API FNipcatAbilitySet_GrantedHandles
{
	GENERATED_BODY()

public:

	void AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle);
	void AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle);
	void AddAttributeSet(UAttributeSet* Set);
	void AddGameplayTags(FGameplayTagContainer Tags);

	void TakeFromAbilitySystem(UAbilitySystemComponent* ASC);
	
	TArray<FGameplayAbilitySpecHandle> GetAbilitySpecHandles(UAbilitySystemComponent* ASC);
	

protected:

	// Handles to the granted abilities.
	UPROPERTY(VisibleAnywhere)
	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;

	// Handles to the granted gameplay effects.
	UPROPERTY(VisibleAnywhere)
	TArray<FActiveGameplayEffectHandle> GameplayEffectHandles;

	// Pointers to the granted attribute sets
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UAttributeSet>> GrantedAttributeSets;
	
	UPROPERTY(VisibleAnywhere)
	FGameplayTagContainer GrantedTags;
};


/**
 * UNipcatAbilitySet
 *
 *	Non-mutable data asset used to grant gameplay abilities and gameplay effects.
 */
UCLASS(BlueprintType)
class NIPCATABILITYSYSTEM_API UNipcatAbilitySet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UNipcatAbilitySet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// Grants the ability set to the specified ability system component.
	// The returned handles can be used later to take away anything that was granted.
	UFUNCTION(BlueprintCallable)
	FNipcatAbilitySet_GrantedHandles GiveToAbilitySystem(UAbilitySystemComponent* ASC, UObject* SourceObject = nullptr);

	UFUNCTION(BlueprintCallable)
	static void RemoveGrantedHandles(UAbilitySystemComponent* ASC, FNipcatAbilitySet_GrantedHandles GrantedHandles);
	
	// Gameplay abilities to grant when this ability set is granted.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay Abilities", meta=(TitleProperty=Comment))
	TArray<FNipcatAbilitySet_GameplayAbility> GrantedGameplayAbilities;

	// Gameplay effects to grant when this ability set is granted.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay Effects", meta=(TitleProperty=Comment))
	TArray<FNipcatAbilitySet_GameplayEffect> GrantedGameplayEffects;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay Tags")
	FGameplayTagContainer GrantedGameplayTags;
	
	// Attribute sets to grant when this ability set is granted.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attribute Sets", meta=(TitleProperty=AttributeSet))
	TArray<FNipcatAbilitySet_AttributeSet> GrantedAttributes;

	
#if WITH_EDITOR
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
	virtual void PostLoad() override;
#endif
	
};
