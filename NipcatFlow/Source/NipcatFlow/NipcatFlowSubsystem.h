// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FlowSubsystem.h"
#include "NipcatFlowSubsystem.generated.h"

struct FNipcatFlowIdentityInfoModifier;
class UNipcatFlowComponent;
struct FNipcatFlowIdentityInfoModifierArray;
struct FNipcatFlowIdentityInfo;
/**
 * 
 */
UCLASS()
class NIPCATFLOW_API UNipcatFlowSubsystem : public UFlowSubsystem
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, meta=(ForceInlineRow))
	TMap<FGameplayTag, FNipcatFlowIdentityInfoModifierArray> IdentityInfoModifiersFromTags;

	UPROPERTY(VisibleAnywhere, meta=(ForceInlineRow))
	TMap<FGuid, FNipcatFlowIdentityInfoModifierArray> IdentityInfoModifiersFromGuid;

	UFUNCTION()
	virtual void RegisterComponentByGuid(UNipcatFlowComponent* Component);
	
	UFUNCTION()
	virtual void UnregisterComponentByGuid(UNipcatFlowComponent* Component);
	
	UFUNCTION(BlueprintCallable, meta=(AutoCreateRefTerm="Modifier"))
	void AddIdentityInfoModifier(const FNipcatFlowIdentityInfoModifier& Modifier);
    
	UFUNCTION(BlueprintCallable, meta=(DefaultToSelf="SourceObject"))
	void RemoveAllIdentityModifiers(UObject* SourceObject);

	UFUNCTION()
	void GetCombinedIdentityInfoByTags(UPARAM(meta=(GameplayTagFilter="Flow.Identity")) const FGameplayTagContainer& IdentityTags, FNipcatFlowIdentityInfo& IdentityInfo);

	UFUNCTION()
	void GetCombinedIdentityInfoByGuid(FGuid FlowComponentGuid, FNipcatFlowIdentityInfo& IdentityInfo);

	UFUNCTION(BlueprintCallable)
	FNipcatFlowIdentityInfo GetCombinedIdentityInfo(UNipcatFlowComponent* FlowComponent);

	UFUNCTION(BlueprintCallable)
	void RefreshIdentityInfoByTagsImmediately(UPARAM(meta=(GameplayTagFilter="Flow.Identity")) FGameplayTag ChangedIdentity, bool bExactMatch);
	
	UFUNCTION(BlueprintCallable)
	void RefreshIdentityInfoByGuidImmediately(FGuid ChangedGuid);
	
protected:
	UPROPERTY(VisibleAnywhere, meta=(ForceInlineRow))
	TMap<FGuid, TWeakObjectPtr<UNipcatFlowComponent>> FlowComponentGuidRegistry; 
	
};
