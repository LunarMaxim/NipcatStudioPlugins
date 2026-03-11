// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "Animation/AnimInstance.h"
#include "NipcatAnimInstance.generated.h"

class UCharacterMovementComponent;

USTRUCT(BlueprintType)
struct FNipcatAnimCurveValuePropertyMapping
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName CurveName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (GetOptions = "GetFloatPropertyNames"))
	FName TargetFloatPropertyName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	double DefaultValue;

	FDoubleProperty* CachedProperty = nullptr;
};

USTRUCT(BlueprintType)
struct FNipcatSocketTransformPropertyMapping
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (GetOptions = "GetVectorPropertyNames"))
	FName TargetVectorPropertyName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (GetOptions = "GetRotatorPropertyNames"))
	FName TargetRotatorPropertyName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<ERelativeTransformSpace> TransformSpace = RTS_World;

	FStructProperty* CachedVectorProperty = nullptr;
	FStructProperty* CachedRotatorProperty = nullptr;
	
};

USTRUCT(BlueprintType)
struct FNipcatAnimPropertyMap
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(TitleProperty="CurveName"))
	TArray<FNipcatAnimCurveValuePropertyMapping> CurveMappings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(TitleProperty="SocketName"))
	TArray<FNipcatSocketTransformPropertyMapping> SocketTransformMappings;

	void Initialize(UAnimInstance* InAnimInstance);

	void Update(UAnimInstance* InAnimInstance, const USkeletalMeshComponent* Mesh);

private:
	bool bIsInitialized = false;
};

/**
 * 
 */
UCLASS()
class NIPCATLOCOMOTIONSYSTEM_API UNipcatAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "- Runtime -", Transient)
	TObjectPtr<ACharacter> Character;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "- Runtime -", Transient)
	TObjectPtr<UAbilitySystemComponent> ASC;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "- Runtime -", Transient)
	TObjectPtr<USkeletalMeshComponent> MeshComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "- Runtime -", Transient)
	TObjectPtr<USkeletalMeshComponent> AttachParentMeshComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "- Runtime -", Transient)
	TObjectPtr<UCharacterMovementComponent> CharacterMovement;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "- Runtime -", Transient)
	TObjectPtr<UPrimitiveComponent> MovementBase;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "- Runtime -", Transient)
	TObjectPtr<AActor> MovementBaseActor;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "- Runtime -", Transient)
	FVector MovementBaseLocation;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "- Runtime -", Transient)
	FVector LastMovementBaseLocation;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "- Runtime -", Transient)
	FVector MovementBaseDisplacement;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "- Runtime -", Transient)
	bool MovementBaseIsMoving;
	
	UPROPERTY(EditDefaultsOnly, Category = "- Config -", meta=(ForceInlineRow))
	FNipcatAnimPropertyMap AnimPropertyMap;
	
	UPROPERTY(EditDefaultsOnly, Category = "- Config -", meta=(ForceInlineRow))
	FGameplayTagBlueprintPropertyMap GameplayTagPropertyMap;
	
	/*
	UPROPERTY(BlueprintReadOnly, Category = "Character State Data")
	float GroundDistance = -1.0f;*/
	
public:

	UNipcatAnimInstance(const FObjectInitializer& ObjectInitializer);

	virtual void NativeInitializeAnimation() override;
	
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;
	
	virtual void InitializeWithAbilitySystem(UAbilitySystemComponent* InASC);
	
protected:

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
	
	UFUNCTION()
	TArray<FName> GetFloatPropertyNames() const;

	UFUNCTION()
	TArray<FName> GetVectorPropertyNames() const;

	UFUNCTION()
	TArray<FName> GetRotatorPropertyNames() const;

#endif // WITH_EDITOR
	
};
