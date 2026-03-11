// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Subsystems/WorldSubsystem.h"
#include "NipcatActorTagSubsystem.generated.h"

USTRUCT(BlueprintType)
struct NIPCATBASICGAMEPLAY_API FNipcatWeakActorSet
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSet<TWeakObjectPtr<AActor>> Set;
};

/**
 * 
 */
UCLASS()
class NIPCATBASICGAMEPLAY_API UNipcatActorTagSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable)
	void RegisterActor(AActor* Actor, const FGameplayTagContainer& Tags);

	UFUNCTION(BlueprintCallable)
	void UnregisterActor(AActor* Actor, const FGameplayTagContainer& Tags);
	
	UFUNCTION(BlueprintCallable, meta=(Categories="Actor", AutoCreateRefTerm="Tag"))
	TArray<AActor*> GetAllActorsOfTag(const FGameplayTag& Tag);

	FNipcatWeakActorSet* GetWeakActorSetOfTag(const FGameplayTag& Tag);
	
	UFUNCTION(BlueprintCallable, meta=(Categories="Actor", AutoCreateRefTerm="Tags"))
	TArray<AActor*> GetAllActorsOfTags(const FGameplayTagContainer& Tags);

	TArray<FNipcatWeakActorSet*> GetWeakActorSetOfTags(const FGameplayTagContainer& Tags);
	
	UFUNCTION(BlueprintCallable, meta=(Categories="Actor", AutoCreateRefTerm="TagQuery"))
	TArray<AActor*> GetAllActorsByTagQuery(const FGameplayTagQuery& TagQuery);

	TArray<FNipcatWeakActorSet*> GetWeakActorSetOfTagsByTagQuery(const FGameplayTagQuery& TagQuery);
	
	UFUNCTION(BlueprintCallable, meta=(Categories="Actor", AutoCreateRefTerm="Tag"))
	TArray<AActor*> GetActorsOfTagInRange(const FGameplayTag& Tag, FVector Center, double Radius);
	
	TArray<TWeakObjectPtr<AActor>> GetWeakActorsOfTagInRange(const FGameplayTag& Tag, FVector Center, double Radius);

protected:
	
	UPROPERTY(VisibleAnywhere, meta=(ForceInlineRow))
	TMap<FGameplayTag, FNipcatWeakActorSet> TaggedActors;
	
};
