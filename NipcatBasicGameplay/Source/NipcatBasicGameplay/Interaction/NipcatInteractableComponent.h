// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NipcatInteractTypes.h"

#include "NipcatInteractableComponent.generated.h"


class UNipcatInteractData;
class UNipcatIndicatorComponent;

UCLASS(ClassGroup=(Nipcat), meta=(BlueprintSpawnableComponent), Blueprintable, BlueprintType)
class NIPCATBASICGAMEPLAY_API UNipcatInteractableComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UNipcatInteractableComponent();

	UFUNCTION(BlueprintPure)
	virtual const FNipcatInteractConfig& GetInteractConfig() const;
	
	virtual void Activate(bool bReset) override;
	virtual void Deactivate() override;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="- Config -", SaveGame)
	bool Activated = true;

protected:
	UPROPERTY(EditAnywhere, Category="- Config -")
	TObjectPtr<UNipcatInteractData> InteractData;

	UPROPERTY(EditAnywhere, Category="- Config -", meta=(EditCondition="InteractData==nullptr", EditConditionHides))
	FNipcatInteractConfig InteractConfig;

#if WITH_EDITORONLY_DATA
	// 这是一个“伪按钮”，点击它其实就是把 checkbox 勾选上
	UPROPERTY(EditAnywhere, Category = "- Config -", meta=(DisplayName=">>> Convert To Data Asset <<<", EditCondition="InteractData==nullptr", EditConditionHides))
	bool bActionConvert = false;
#endif

#if WITH_EDITOR
	// 监听属性变化
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	// 具体的执行逻辑
	void ConvertInteractConfigToInteractData();
#endif
	
public:
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="- Config -", meta=(Categories="InteractState"), SaveGame)
	FGameplayTagContainer DisabledStateTags;

	UFUNCTION(BlueprintCallable, meta=(GameplayTagFilter="InteractState", AutoCreateRefTerm="StateTag"))
	void EnableState(const FGameplayTagContainer& StateTag);

	UFUNCTION(BlueprintCallable, meta=(GameplayTagFilter="InteractState", AutoCreateRefTerm="StateTag"))
	void DisableState(const FGameplayTagContainer& StateTag);

	UFUNCTION(BlueprintPure, meta=(GameplayTagFilter="InteractState", AutoCreateRefTerm="StateTag"))
	bool IsStateEnabled(const FGameplayTag& StateTag);

	UFUNCTION(BlueprintPure, meta=(GameplayTagFilter="InteractState", AutoCreateRefTerm="StateTag"))
	FText GetInteractName(const FGameplayTag& StateTag);

	UFUNCTION(BlueprintPure)
	UNipcatIndicatorComponent* GetIndicator();
	
	UFUNCTION(BlueprintPure)
	FVector GetInteractPointLocation();

	virtual void Serialize(FArchive& Ar) override;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnInteractStateUpdatedDelegate, UNipcatInteractComponent*, InteractComponent, const FNipcatInteractableState&, OldState, const FNipcatInteractableState&, NewState);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInteractSucceedDelegate, UNipcatInteractComponent*, InteractComponent, FGameplayTag, StateTag);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnInteractFailedDelegate, UNipcatInteractComponent*, InteractComponent, FGameplayTag, StateTag, FText, FailedReason, UNipcatInteractCondition*, FailedCondition);
	
	UPROPERTY(BlueprintAssignable)
	FOnInteractStateUpdatedDelegate OnInteractStateUpdated;
	
	UPROPERTY(BlueprintAssignable)
	FOnInteractSucceedDelegate OnInteractSucceed;
	
	UPROPERTY(BlueprintAssignable)
	FOnInteractFailedDelegate OnInteractFailed;
};
