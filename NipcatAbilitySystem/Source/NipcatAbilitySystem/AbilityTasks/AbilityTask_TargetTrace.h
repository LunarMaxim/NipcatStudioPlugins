// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "CollisionShape.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AbilityTask_TargetTrace.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWTargetTraceAbilityTaskDelegate, FHitResult, Hit, int32, TraceKey);


/**
 * 
 */
UCLASS()
class NIPCATABILITYSYSTEM_API UAbilityTask_TargetTrace : public UAbilityTask
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FWTargetTraceAbilityTaskDelegate OnFirstHit;

protected:
	UPROPERTY()
	int32 TraceKey;
	UPROPERTY()
	UMeshComponent* MeshComponent;
	UPROPERTY()
	FName SocketName;
	UPROPERTY()
	FVector PositionOffset;
	UPROPERTY()
	FRotator RotationOffset;
	// UPROPERTY()
	// float MaxDuration;
	UPROPERTY()
	FTimerHandle MaxDurationTimerHandle;
	FCollisionShape Shape;
	UPROPERTY()
	TEnumAsByte<ECollisionChannel> TraceChannel;
	UPROPERTY()
	bool bIgnoreOwner;
	UPROPERTY()
	bool bIgnoreMeshOwner;
	FCollisionQueryParams QueryParams;

	UPROPERTY()
	TEnumAsByte<EDrawDebugTrace::Type> DrawDebugTrace;
	UPROPERTY()
	FLinearColor TraceColor;
	UPROPERTY()
	FLinearColor TraceHitColor;
	UPROPERTY()
	float DrawTime;
	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> HitActors;

public:
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_TargetTrace* CreateTargetBoxTraceTask
	(
		UGameplayAbility* OwningAbility,
		FName TaskName,
		int32 InTraceKey,
		UMeshComponent* InMeshComponent,
		FName InSocketName,
		FVector InPositionOffset,
		FRotator InRotationOffset,
		FVector InBoxExtent,
		ECollisionChannel InTraceChannel,
		TArray<AActor*> InIgnoreActors,
		bool IgnoreOwner = true,
		bool IgnoreMeshOwner = true
	);

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_TargetTrace* CreateTargetSphereTraceTask
	(
		UGameplayAbility* OwningAbility,
		FName TaskName,
		int32 InTraceKey,
		UMeshComponent* InMeshComponent,
		FName InSocketName,
		FVector InPositionOffset,
		FRotator InRotationOffset,
		float InRadius,
		ECollisionChannel InTraceChannel,
		TArray<AActor*> InIgnoreActors,
		bool IgnoreOwner = true,
		bool IgnoreMeshOwner = true
	);

	static UAbilityTask_TargetTrace* CreateTargetTraceTask
	(
		UGameplayAbility* OwningAbility,
		FName TaskName,
		int32 InTraceKey,
		UMeshComponent* InMeshComponent,
		FName InSocketName,
		FVector InPositionOffset,
		FRotator InRotationOffset,
		FCollisionShape InShape,
		ECollisionChannel InTraceChannel,
		TArray<AActor*> InIgnoreActors = TArray<AActor*>(),
		bool IgnoreOwner = true,
		bool IgnoreMeshOwner = true
	);


	UFUNCTION(BlueprintCallable, Category="Ability|Tasks")
	void SetDebug(EDrawDebugTrace::Type InDrawDebugTrace, FLinearColor InTraceColor, FLinearColor InTraceHitColor, float InDrawTime);
	virtual void TickTask(float DeltaTime) override;
	virtual void OnDestroy(bool bInOwnerFinished) override;

protected:
	virtual void Activate() override;

	void OnGameplayAbilityEnded(UGameplayAbility* GameplayAbility);
};
