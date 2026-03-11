// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GenericTeamAgentInterface.h"
#include "Components/ActorComponent.h"
#include "NipcatTeamComponent.generated.h"


USTRUCT(BlueprintType)
struct NIPCATBASICGAMEPLAY_API FNipcatTeamDefinition : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	uint8 TeamId = 255;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(Categories = "Team"))
	FGameplayTag TeamTag;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(Categories = "Team", ForceInlineRow))
	TMap<FGameplayTag, TEnumAsByte<ETeamAttitude::Type>> AttitudeTowards;
};

UCLASS(BlueprintType)
class NIPCATBASICGAMEPLAY_API UNipcatTeamConfig : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(Categories = "Team", ForceInlineRow, TitleProperty="TeamTag"))
	TArray<FNipcatTeamDefinition> TeamDefinitions;

	UFUNCTION(BlueprintPure, Category="Nipcat Basic Gameplay|Team")
	FNipcatTeamDefinition FindTeamDefinitionByGenericTeamId(FGenericTeamId Id) const;
	
	UFUNCTION(BlueprintPure, Category="Nipcat Basic Gameplay|Team", meta=(GameplayTagFilter="Team"))
	FNipcatTeamDefinition FindTeamDefinitionByTag(FGameplayTag TeamTag) const;
	
	UFUNCTION(BlueprintPure, Category = "Nipcat Basic Gameplay|Team", meta=(GameplayTagFilter="Team"))
	ETeamAttitude::Type GetTeamAttitudeTowards(FGameplayTag SourceTeamTag, FGameplayTag TargetTeamTag) const;

};


UCLASS(ClassGroup=(Nipcat), DisplayName="NipcatTeamComponent", meta=(BlueprintSpawnableComponent))
class NIPCATBASICGAMEPLAY_API UNipcatTeamComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bShowDebug;
	
	virtual void BeginPlay() override;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta=(ForceInlineRow))
	TMap<AActor*, TEnumAsByte<ETeamAttitude::Type>> ActorAttitudeOverride;
	
	UFUNCTION(BlueprintCallable, Category="Nipcat Basic Gameplay|Team")
	void SetActorAttitudeOverride(AActor* Actor, ETeamAttitude::Type NewAttitude = ETeamAttitude::Type::Hostile, bool Bidirectional = true);
	
	UFUNCTION(BlueprintPure, Category="Nipcat Basic Gameplay|Team")
	FGameplayTag GetCurrentTeamTag() const;

	UFUNCTION(BlueprintCallable, Category="Nipcat Basic Gameplay|Team", meta=(GameplayTagFilter = "Team"))
	void OverrideCurrentTeamTag(FGameplayTag InTag);

	/** For IGenericTeamAgent interface */
	UFUNCTION(BlueprintCallable, Category="Nipcat Basic Gameplay|Team")
	void OverrideCurrentTeamTagByGenericTeamId(FGenericTeamId Id);

	UFUNCTION(BlueprintPure, Category="Nipcat Basic Gameplay|Team")
	FGenericTeamId GetCurrentGenericTeamId() const;
	
	UFUNCTION(BlueprintPure, Category="Nipcat Basic Gameplay|Team")
	ETeamAttitude::Type GetTeamAttitudeTowards(const AActor* Actor);
	/** End for IGenericTeamAgent interface */
	
protected:
	
	UPROPERTY(VisibleAnywhere)
	FGameplayTag OverrideTeamTag;

	UPROPERTY(VisibleAnywhere)
	FGameplayTag DefaultTeamTag;

private:
	void DebugLog(const FString& DebugMessage, bool EnableDebug, float TimeToDisplay = 10.f, FColor Color = FColor::Orange);
};
