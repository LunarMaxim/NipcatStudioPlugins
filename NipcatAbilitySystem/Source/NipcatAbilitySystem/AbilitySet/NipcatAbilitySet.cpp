// Copyright Epic Games, Inc. All Rights Reserved.

#include "NipcatAbilitySet.h"
#include "NipcatAbilitySystem/NipcatAbilitySystemLogChannels.h"
#include "NipcatAbilitySystem/NipcatAbilitySystemComponent.h"
#include "NipcatAbilitySystem/Abilities/NipcatGameplayAbility.h"
#include "NipcatBasicGameplay/FunctionLibrary/NipcatBasicGameplayFunctionLibrary.h"
#include "Misc/EngineVersionComparison.h"


void FNipcatAbilitySet_GrantedHandles::AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle)
{
	if (Handle.IsValid())
	{
		AbilitySpecHandles.Add(Handle);
	}
}

void FNipcatAbilitySet_GrantedHandles::AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle)
{
	if (Handle.IsValid())
	{
		GameplayEffectHandles.Add(Handle);
	}
}

void FNipcatAbilitySet_GrantedHandles::AddAttributeSet(UAttributeSet* Set)
{
	GrantedAttributeSets.Add(Set);
}

void FNipcatAbilitySet_GrantedHandles::AddGameplayTags(FGameplayTagContainer Tags)
{
	GrantedTags = Tags;
}

void FNipcatAbilitySet_GrantedHandles::TakeFromAbilitySystem(UAbilitySystemComponent* ASC)
{
	check(ASC);

	if (!ASC->IsOwnerActorAuthoritative())
	{
		// Must be authoritative to give or take ability sets.
		return;
	}

	for (const FGameplayAbilitySpecHandle& Handle : AbilitySpecHandles)
	{
		if (Handle.IsValid())
		{
			if (const FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromHandle(Handle); Spec && Spec->IsActive())
			{
				ASC->CancelAbilityHandle(Handle);
			}
			ASC->ClearAbility(Handle);
		}
	}

	for (UAttributeSet* Set : GrantedAttributeSets)
	{
		ASC->RemoveSpawnedAttribute(Set);
	}
	
	for (const FActiveGameplayEffectHandle& Handle : GameplayEffectHandles)
	{
		if (Handle.IsValid())
		{
			ASC->RemoveActiveGameplayEffect(Handle);
		}
	}
	
#if UE_VERSION_OLDER_THAN(5,7,0)
	ASC->RemoveLooseGameplayTags(GrantedTags);
	ASC->RemoveReplicatedLooseGameplayTags(GrantedTags);
#else
	ASC->RemoveLooseGameplayTags(GrantedTags, 1, EGameplayTagReplicationState::CountToOwner);
#endif

	AbilitySpecHandles.Reset();
	GrantedAttributeSets.Reset();
	GameplayEffectHandles.Reset();
}

TArray<FGameplayAbilitySpecHandle> FNipcatAbilitySet_GrantedHandles::GetAbilitySpecHandles(UAbilitySystemComponent* ASC)
{
	check(ASC);
	TArray<FGameplayAbilitySpecHandle> NewAbilitySpecHandles;
	if (!ASC->IsOwnerActorAuthoritative())
	{
		// Must be authoritative to give or take ability sets.
		return NewAbilitySpecHandles;
	}

	return AbilitySpecHandles;
}

UNipcatAbilitySet::UNipcatAbilitySet(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FNipcatAbilitySet_GrantedHandles UNipcatAbilitySet::GiveToAbilitySystem(UAbilitySystemComponent* ASC, UObject* SourceObject)
{
	FNipcatAbilitySet_GrantedHandles GrantedHandles;
	if (!ASC || !ASC->IsOwnerActorAuthoritative())
	{
		// Must be authoritative to give or take ability sets.
		UE_LOG(LogNipcatAbilitySystem, Error, TEXT("UNipcatAbilitySet::GiveToAbilitySystem: Try give %s to an Invalid ASC."), *GetName());
		return GrantedHandles;
	}

	// Grant the gameplay abilities.
	for (int32 AbilityIndex = 0; AbilityIndex < GrantedGameplayAbilities.Num(); ++AbilityIndex)
	{
		const FNipcatAbilitySet_GameplayAbility& AbilityToGrant = GrantedGameplayAbilities[AbilityIndex];

		if (!IsValid(AbilityToGrant.Ability.LoadSynchronous()))
		{
			UE_LOG(LogNipcatAbilitySystem, Error, TEXT("GrantedGameplayAbilities[%d] on ability set [%s] is not valid."), AbilityIndex, *GetNameSafe(this));
			continue;
		}

		UNipcatGameplayAbility* AbilityCDO = AbilityToGrant.Ability->GetDefaultObject<UNipcatGameplayAbility>();

		FGameplayAbilitySpec AbilitySpec(AbilityCDO, AbilityToGrant.AbilityLevel);
		AbilitySpec.SourceObject = SourceObject;

		const FGameplayAbilitySpecHandle AbilitySpecHandle = ASC->GiveAbility(AbilitySpec);
		if (AbilityToGrant.InitialActivate)
		{
			ASC->TryActivateAbility(AbilitySpecHandle);
		}

		GrantedHandles.AddAbilitySpecHandle(AbilitySpecHandle);
	}

	// Grant the attribute sets.
	for (int32 SetIndex = 0; SetIndex < GrantedAttributes.Num(); ++SetIndex)
	{
		const FNipcatAbilitySet_AttributeSet& SetToGrant = GrantedAttributes[SetIndex];

		if (!IsValid(SetToGrant.AttributeSet))
		{
			UE_LOG(LogNipcatAbilitySystem, Error, TEXT("GrantedAttributes[%d] on ability set [%s] is not valid"), SetIndex, *GetNameSafe(this));
			continue;
		}

		UAttributeSet* NewSet = NewObject<UAttributeSet>(ASC->GetOwner(), SetToGrant.AttributeSet);
		ASC->AddAttributeSetSubobject(NewSet);

		GrantedHandles.AddAttributeSet(NewSet);
	}
	
	// Grant the gameplay effects.
	for (int32 EffectIndex = 0; EffectIndex < GrantedGameplayEffects.Num(); ++EffectIndex)
	{
		const FNipcatAbilitySet_GameplayEffect& EffectToGrant = GrantedGameplayEffects[EffectIndex];

		if (!IsValid(EffectToGrant.GameplayEffect.LoadSynchronous()))
		{
			UE_LOG(LogNipcatAbilitySystem, Error, TEXT("GrantedGameplayEffects[%d] on ability set [%s] is not valid"), EffectIndex, *GetNameSafe(this));
			continue;
		}

		const UGameplayEffect* GameplayEffect = EffectToGrant.GameplayEffect->GetDefaultObject<UGameplayEffect>();
		const FActiveGameplayEffectHandle GameplayEffectHandle = ASC->ApplyGameplayEffectToSelf(GameplayEffect, EffectToGrant.EffectLevel, ASC->MakeEffectContext());

		GrantedHandles.AddGameplayEffectHandle(GameplayEffectHandle);
	}

#if UE_VERSION_OLDER_THAN(5,7,0)
	ASC->AddLooseGameplayTags(GrantedGameplayTags);
	ASC->AddReplicatedLooseGameplayTags(GrantedGameplayTags);
#else
	ASC->AddLooseGameplayTags(GrantedGameplayTags, 1, EGameplayTagReplicationState::CountToOwner);
#endif
	GrantedHandles.AddGameplayTags(GrantedGameplayTags);
	
	return GrantedHandles;
}

void UNipcatAbilitySet::RemoveGrantedHandles(UAbilitySystemComponent* ASC, FNipcatAbilitySet_GrantedHandles GrantedHandles)
{
	GrantedHandles.TakeFromAbilitySystem(ASC);
}

#if WITH_EDITOR

void UNipcatAbilitySet::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);
#if WITH_EDITORONLY_DATA
	if (PropertyChangedEvent.Property)
	{
		if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_STRING_VIEW_CHECKED(FNipcatAbilitySet_GameplayAbility, Comment))
		{
			const auto Index = PropertyChangedEvent.GetArrayIndex(GET_MEMBER_NAME_STRING_CHECKED(UNipcatAbilitySet, GrantedGameplayAbilities));
			if (GrantedGameplayAbilities.IsValidIndex(Index))
			{
				auto& GrantedGameplayAbility = GrantedGameplayAbilities[Index];
				if (GrantedGameplayAbility.Comment.IsEmpty())
				{
					GrantedGameplayAbility.bCommentModified = false;
					UNipcatBasicGameplayFunctionLibrary::TryUpdateComment(GrantedGameplayAbility.Comment, GrantedGameplayAbility.Ability, GrantedGameplayAbility.bCommentModified);
				}
				else if (GrantedGameplayAbility.Comment != UNipcatBasicGameplayFunctionLibrary::GetClassObjectName(GrantedGameplayAbility.Ability))
				{
					GrantedGameplayAbility.bCommentModified = true;
				}
			}
		}
		if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_STRING_VIEW_CHECKED(FNipcatAbilitySet_GameplayAbility, Ability))
		{
			const auto Index = PropertyChangedEvent.GetArrayIndex(GET_MEMBER_NAME_STRING_CHECKED(UNipcatAbilitySet, GrantedGameplayAbilities));
			if (GrantedGameplayAbilities.IsValidIndex(Index))
			{
				auto& GrantedGameplayAbility = GrantedGameplayAbilities[Index];
				UNipcatBasicGameplayFunctionLibrary::TryUpdateComment(GrantedGameplayAbility.Comment, GrantedGameplayAbility.Ability, GrantedGameplayAbility.bCommentModified);
			}
		}
		if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_STRING_VIEW_CHECKED(FNipcatAbilitySet_GameplayEffect, Comment))
		{
			const auto Index = PropertyChangedEvent.GetArrayIndex(GET_MEMBER_NAME_STRING_CHECKED(UNipcatAbilitySet, GrantedGameplayEffects));
			if (GrantedGameplayEffects.IsValidIndex(Index))
			{
				auto& GrantedGameplayEffect = GrantedGameplayEffects[Index];
				if (GrantedGameplayEffect.Comment.IsEmpty())
				{
					GrantedGameplayEffect.bCommentModified = false;
					UNipcatBasicGameplayFunctionLibrary::TryUpdateComment(GrantedGameplayEffect.Comment, GrantedGameplayEffect.GameplayEffect, GrantedGameplayEffect.bCommentModified);
				}
				else if (GrantedGameplayEffect.Comment != UNipcatBasicGameplayFunctionLibrary::GetClassObjectName(GrantedGameplayEffect.GameplayEffect))
				{
					GrantedGameplayEffect.bCommentModified = true;
				}
			}
		}
		if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_STRING_VIEW_CHECKED(FNipcatAbilitySet_GameplayEffect, GameplayEffect))
		{
			const auto Index = PropertyChangedEvent.GetArrayIndex(GET_MEMBER_NAME_STRING_CHECKED(UNipcatAbilitySet, GrantedGameplayEffects));
			if (GrantedGameplayEffects.IsValidIndex(Index))
			{
				auto& GrantedGameplayEffect = GrantedGameplayEffects[Index];
				UNipcatBasicGameplayFunctionLibrary::TryUpdateComment(GrantedGameplayEffect.Comment, GrantedGameplayEffect.GameplayEffect, GrantedGameplayEffect.bCommentModified);
			}
		}
	}
#endif
}

void UNipcatAbilitySet::PostLoad()
{
	Super::PostLoad();
	for (auto& GrantedGameplayAbility : GrantedGameplayAbilities)
	{
		UNipcatBasicGameplayFunctionLibrary::TryUpdateComment(GrantedGameplayAbility.Comment, GrantedGameplayAbility.Ability, GrantedGameplayAbility.bCommentModified);
	}
	for (auto& GrantedGameplayEffect : GrantedGameplayEffects)
	{
		UNipcatBasicGameplayFunctionLibrary::TryUpdateComment(GrantedGameplayEffect.Comment, GrantedGameplayEffect.GameplayEffect, GrantedGameplayEffect.bCommentModified);
	}
}


#endif

