// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameplayEffectTypes.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NipcatBasicGameplay/Tags/NipcatBasicGameplayTags.h"
#include "NipcatCharacterMovementComponent.generated.h"


class UNipcatCustomMovementMode;
class UCharacterRotationModifier;

UCLASS(ClassGroup=(Nipcat), meta=(BlueprintSpawnableComponent))
class NIPCATBASICGAMEPLAY_API UNipcatCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UNipcatCharacterMovementComponent(const FObjectInitializer& ObjectInitializer);
	
	virtual void BeginPlay() override;
	
	UPROPERTY()
	float DeltaT;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> ASC;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CustomMovement", Instanced, meta=(ForceInlineRow, Categories="MovementMode.Custom", GetOptions=GetCustomMovementModeNames))
	TMap<FGameplayTag, UNipcatCustomMovementMode*> CustomMovementModes;

	UFUNCTION(BlueprintPure)
	static FGameplayTag FindCustomMovementModeTag(uint8 InCustomMovementMode);
	
	UFUNCTION(BlueprintPure)
	UNipcatCustomMovementMode* FindCustomMovementMode(uint8 InCustomMovementMode) const;
	
	UFUNCTION(BlueprintPure)
	UNipcatCustomMovementMode* FindCustomMovementModeByTag(UPARAM(meta=(GameplayTagFilter="MovementMode.Custom")) FGameplayTag InCustomMovementModeTag) const;
	
	virtual void InitializeComponent() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Crouch Jump */
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bCanJumpOnCrouching = true;
	virtual bool CanAttemptJump() const override;
	
	/** Rotation Modifier */
	
	UPROPERTY(Transient)
	TArray<UCharacterRotationModifier*> RotationModeModifierStack;

	bool bOldAllowPhysicsRotationDuringAnimRootMotion;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Character Movement: Jumping / Falling")
	bool bAirControlBoostVelocityThresholdIncludeZ = false;
	virtual float BoostAirControl(float DeltaTime, float TickAirControl, const FVector& FallAcceleration) override;
	
	virtual void PhysicsRotation(float DeltaTime) override;

	bool GetModifiedRotator(float DeltaTime, FRotator& OutRotation) const;

	virtual FRotator GetDeltaRotation(float DeltaTime) const override;

	float NipcatGetAxisDeltaRotation(float InAxisRotationRate, float DeltaTime) const;
	
	UCharacterRotationModifier* GetRotationModifier() const;
	
	UFUNCTION(BlueprintCallable, Category = "NipcatCharacterMovementComponent")
	void AddRotationModifier(UCharacterRotationModifier* RotationModifier);

	UFUNCTION(BlueprintCallable, Category = "NipcatCharacterMovementComponent")
	void RemoveRotationModifier(UCharacterRotationModifier* RotationModifier);

	/** Edge Protection */
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bEdgeProtectionDebug = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayTag DisableEdgeProtectionTag = NipcatBasicGameplayTags::DisableEdgeProtection;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float EdgeProtectionFirstTraceDistance = 15.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float EdgeProtectionFirstTraceHeight = 20.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float EdgeProtectionSecondTraceDistance = 65.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float EdgeProtectionSecondTraceHeight = 20.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float EdgeProtectionSecondTraceSphereRadius = 15.0f;

	FTransform OnProcessRootMotionPostConvertToWorld(const FTransform& InRootMotion, class UCharacterMovementComponent* CharacterMovementComponent, float DeltaSeconds) const;

	/** Knock Back RootMotion Source*/

	virtual FVector ConstrainAnimRootMotionVelocity(const FVector& RootMotionVelocity, const FVector& CurrentVelocity) const override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTeleportedDelegate);
	UPROPERTY(BlueprintAssignable, Category="NipcatCharacterMovementComponent")
	FOnTeleportedDelegate OnTeleport;
	virtual void OnTeleported() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHasVelocity);
	UPROPERTY(BlueprintAssignable, Category="NipcatCharacterMovementComponent")
	FOnHasVelocity OnHasVelocity;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNoVelocity);
	UPROPERTY(BlueprintAssignable, Category="NipcatCharacterMovementComponent")
	FOnNoVelocity OnNoVelocity;
	
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Sliding")
	bool bSlidingHoldSpeedMagnitude;
	
	virtual FVector ComputeSlideVector(const FVector& Delta, const float Time, const FVector& Normal, const FHitResult& Hit) const override;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHasAcceleration);
	UPROPERTY(BlueprintAssignable, Category="NipcatCharacterMovementComponent")
	FOnHasAcceleration OnHasAcceleration;
	virtual FVector ConsumeInputVector() override;

	virtual void CalcVelocity(float DeltaTime, float Friction, bool bFluid, float BrakingDeceleration) override;

	/* Based Movement Rotation **/
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	bool BasedMovementUpdated;
	
	UPROPERTY(VisibleAnywhere)
	FRotator DeltaBaseRotation;
	
	virtual void UpdateBasedRotation(FRotator& FinalRotation, const FRotator& ReducedRotation) override;
	
	UFUNCTION(BlueprintCallable, Category = "NipcatCharacterMovementComponent")
	bool TryGetMovementBaseDeltaRotation(FRotator& DeltaRotation);

	/* Disable Root Motion Scale **/
	
	UPROPERTY(EditAnywhere, Category = "RootMotionScale", meta=(InlineEditConditionToggle))
	bool bDisableRootMotionScale;
	
	UPROPERTY(EditAnywhere, Category = "RootMotionScale", meta=(EditCondition=bDisableRootMotionScale))
	FGameplayTagRequirements DisableRootMotionScaleRequirements;

	/* Penetration Adjustment **/
	
	// If checked, improves the response to interaction from moving kinematic physical
	// bodies, but may cause some issues when interacting with simulated physical bodies.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Penetration")
	uint8 bAllowImprovedPenetrationAdjustment : 1 {true};
	
	virtual void ComputeFloorDist(const FVector& CapsuleLocation, float LineDistance, float SweepDistance, FFindFloorResult& OutFloorResult, float SweepRadius, const FHitResult* DownwardSweepResult) const override;

	virtual void PhysWalking(float deltaTime, int32 Iterations) override;
	
	/* Custom Movement Mode **/
	
	virtual void PhysCustom(float deltaTime, int32 Iterations) override;
	
	virtual void SetMovementMode(EMovementMode NewMovementMode, uint8 NewCustomMode = 0) override;

	UFUNCTION(BlueprintCallable)
	virtual void AddCustomMovementInput(FVector InputValue, float DeltaTime);

	virtual FRotator ComputeOrientToMovementRotation(const FRotator& CurrentRotation, float DeltaTime, FRotator& DeltaRotation) const override;
	
	/** Push Collision */
	
	UPROPERTY(EditAnywhere, Category = "Push", meta=(InlineEditConditionToggle))
	bool bEnablePushOthers = true;
	
	UPROPERTY(EditAnywhere, Category = "Push", meta=(EditCondition="bEnablePushOthers"))
	FGameplayTagRequirements CanPushOthersRequirements;
	
	UPROPERTY(EditAnywhere, Category = "Push", meta=(InlineEditConditionToggle))
	bool bEnableBePushed = true;
	
	UPROPERTY(EditAnywhere, Category = "Push", meta=(EditCondition="bEnableBePushed"))
	FGameplayTagRequirements CanBePushedRequirements;
	
	virtual void HandleImpact(const FHitResult& Hit, float TimeSlice = 0, const FVector& MoveDelta = FVector::ZeroVector) override;
	
	UFUNCTION(BlueprintPure, Category = "NipcatCharacterMovementComponent")
	bool CanPushOthers() const;
	
	UFUNCTION(BlueprintPure, Category = "NipcatCharacterMovementComponent")
	bool CanBePushed() const;
};
