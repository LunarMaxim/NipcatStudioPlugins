// Fill out your copyright notice in the Description page of Project Settings.


#include "NipcatAnimInstance.h"

#include "AbilitySystemGlobals.h"
#include "GameFramework/Character.h"
#include "Misc/DataValidation.h"



void FNipcatAnimPropertyMap::Initialize(UAnimInstance* InAnimInstance)
{
	if (!InAnimInstance) return;

	UClass* AnimClass = InAnimInstance->GetClass();

	// 1. 初始化曲线映射 (Float)
	for (FNipcatAnimCurveValuePropertyMapping& Map : CurveMappings)
	{
		if (Map.TargetFloatPropertyName.IsNone()) continue;

		// 通过反射查找 Float 属性
		FProperty* Prop = AnimClass->FindPropertyByName(Map.TargetFloatPropertyName);
		Map.CachedProperty = CastField<FDoubleProperty>(Prop);

		if (!Map.CachedProperty)
		{
			UE_LOG(LogAnimation, Warning, TEXT("Mapper: Could not find Float Property '%s' for Curve '%s'"), 
				*Map.TargetFloatPropertyName.ToString(), *Map.CurveName.ToString());
		}
	}

	// 2. 初始化 Socket 映射 (Vector)
	for (FNipcatSocketTransformPropertyMapping& Map : SocketTransformMappings)
	{
		if (!Map.TargetVectorPropertyName.IsNone())
		{
			// 通过反射查找 Struct 属性 (FVector 是一个 Struct)
			FProperty* Prop = AnimClass->FindPropertyByName(Map.TargetVectorPropertyName);
			FStructProperty* StructProp = CastField<FStructProperty>(Prop);

			// 确保它真的是一个 Vector
			if (StructProp && StructProp->Struct->GetFName() == NAME_Vector)
			{
				Map.CachedVectorProperty = StructProp;
			}
			else
			{
				UE_LOG(LogAnimation, Warning, TEXT("Mapper: Property '%s' is not a valid Vector for Socket '%s'"), 
					*Map.TargetVectorPropertyName.ToString(), *Map.SocketName.ToString());
				Map.CachedVectorProperty = nullptr;
			}
		}
		if (!Map.TargetRotatorPropertyName.IsNone())
		{
			FProperty* Prop = AnimClass->FindPropertyByName(Map.TargetRotatorPropertyName);
			FStructProperty* StructProp = CastField<FStructProperty>(Prop);

			// 确保它真的是一个 Vector
			if (StructProp && StructProp->Struct->GetFName() == NAME_Rotator)
			{
				Map.CachedRotatorProperty = StructProp;
			}
			else
			{
				UE_LOG(LogAnimation, Warning, TEXT("Mapper: Property '%s' is not a valid Rotator for Socket '%s'"), 
					*Map.TargetRotatorPropertyName.ToString(), *Map.SocketName.ToString());
				Map.CachedRotatorProperty = nullptr;
			}
		}
	}

	bIsInitialized = true;
}

void FNipcatAnimPropertyMap::Update(UAnimInstance* InAnimInstance, const USkeletalMeshComponent* Mesh)
{
	if (!bIsInitialized || !InAnimInstance) return;

	for (const FNipcatAnimCurveValuePropertyMapping& Map : CurveMappings)
	{
		if (Map.CachedProperty)
		{
			float Val;
			InAnimInstance->GetCurveValueWithDefault(Map.CurveName, Map.DefaultValue, Val);
            
			// 直接写入内存 (极快)
			Map.CachedProperty->SetPropertyValue_InContainer(InAnimInstance, Val);
		}
	}

	if (Mesh)
	{
		for (const FNipcatSocketTransformPropertyMapping& Map : SocketTransformMappings)
		{
			if (Map.CachedVectorProperty || Map.CachedRotatorProperty)
			{
				FTransform Transform = Mesh->GetSocketTransform(Map.SocketName, Map.TransformSpace);
				if (Map.CachedVectorProperty)
				{
					FVector Loc = Transform.GetTranslation();

					// 通过反射写入 Vector
					// ContainerPtrToValuePtr 返回变量在内存中的实际地址
					void* ValuePtr = Map.CachedVectorProperty->ContainerPtrToValuePtr<void>(InAnimInstance);
                
					// 将 FVector 数据拷贝到该地址
					Map.CachedVectorProperty->CopyCompleteValue(ValuePtr, &Loc);
				}
				if (Map.CachedRotatorProperty)
				{
					FRotator Rot = Transform.Rotator();
					void* ValuePtr = Map.CachedRotatorProperty->ContainerPtrToValuePtr<void>(InAnimInstance);
					Map.CachedRotatorProperty->CopyCompleteValue(ValuePtr, &Rot);
				}
			}
		}
	}
}

UNipcatAnimInstance::UNipcatAnimInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


void UNipcatAnimInstance::NativeInitializeAnimation()
{
	Character = Cast<ACharacter>(GetOwningActor());
	MeshComp = GetOwningComponent();
	if (MeshComp->GetAttachParent())
	{
		AttachParentMeshComp = Cast<USkeletalMeshComponent>(MeshComp->GetAttachParent());
	}

#if WITH_EDITOR
	if (const UWorld* World = GetWorld())
	{
		if (!World->IsGameWorld() && !IsValid(Character))
		{
			// Use default objects for editor preview.

			Character = GetMutableDefault<ACharacter>();
		}
	}
#endif
	
	if (Character)
	{
		CharacterMovement = Character->GetCharacterMovement();
		if (UAbilitySystemComponent* FoundASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Character))
		{
			InitializeWithAbilitySystem(FoundASC);
		}
	}
	
	AnimPropertyMap.Initialize(this);
}

void UNipcatAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if (!IsValid(Character))
	{
		return;
	}
	
	MovementBase = Character->GetMovementBase();
	MovementBaseActor = MovementBase ? MovementBase->GetOwner() : nullptr;
	MovementBaseLocation = MovementBase ? MovementBase->GetComponentLocation() : FVector::ZeroVector;
	
	AnimPropertyMap.Update(this, MeshComp);
	
	/*
	UNipcatCharacterMovementComponent* CharMoveComp = CastChecked<UNipcatCharacterMovementComponent>(Character->GetCharacterMovement());
	const FNipcatCharacterGroundInfo& GroundInfo = CharMoveComp->GetGroundInfo();
	GroundDistance = GroundInfo.GroundDistance;
	*/
}

void UNipcatAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	if (!IsValid(Character))
	{
		return;
	}

	if (!MovementBase)
	{
		LastMovementBaseLocation = FVector::ZeroVector;
		MovementBaseDisplacement = FVector::ZeroVector;
		MovementBaseIsMoving = false;
	}
	else if (LastMovementBaseLocation == FVector::ZeroVector)
	{
		LastMovementBaseLocation = MovementBaseLocation;
	}
	
	MovementBaseDisplacement = MovementBaseLocation - LastMovementBaseLocation;
	MovementBaseIsMoving = !(MovementBaseDisplacement / DeltaSeconds).IsNearlyZero();
	LastMovementBaseLocation = MovementBaseLocation;
}

void UNipcatAnimInstance::InitializeWithAbilitySystem(UAbilitySystemComponent* InASC)
{
	check(InASC);

	ASC = InASC;
	
	GameplayTagPropertyMap.Initialize(this, InASC);
}

#if WITH_EDITOR
EDataValidationResult UNipcatAnimInstance::IsDataValid(class FDataValidationContext& Context) const
{
	Super::IsDataValid(Context);
	
	GameplayTagPropertyMap.IsDataValid(this, Context);

	return ((Context.GetNumErrors() > 0) ? EDataValidationResult::Invalid : EDataValidationResult::Valid);
}

TArray<FName> UNipcatAnimInstance::GetFloatPropertyNames() const
{
	TArray<FName> Names;
	for (TFieldIterator<FDoubleProperty> PropIt(GetClass()); PropIt; ++PropIt)
	{
		FDoubleProperty* Prop = *PropIt;
		Names.Add(Prop->GetFName());
	}
	Names.Sort([](const FName& A, const FName& B)
	{
		return A.ToString() < B.ToString();
	});
	Names.Insert(NAME_None, 0);
	return Names;
}

TArray<FName> UNipcatAnimInstance::GetVectorPropertyNames() const
{
	TArray<FName> Names;
	for (TFieldIterator<FStructProperty> PropIt(GetClass()); PropIt; ++PropIt)
	{
		FStructProperty* Prop = *PropIt;
		if (Prop->Struct->GetFName() == NAME_Vector)
		{
			Names.Add(Prop->GetFName());
		}
	}
	Names.Sort([](const FName& A, const FName& B)
	{
		return A.ToString() < B.ToString();
	});
	Names.Insert(NAME_None, 0);
	return Names;
}

TArray<FName> UNipcatAnimInstance::GetRotatorPropertyNames() const
{
	TArray<FName> Names;
	for (TFieldIterator<FStructProperty> PropIt(GetClass()); PropIt; ++PropIt)
	{
		FStructProperty* Prop = *PropIt;
		if (Prop->Struct->GetFName() == NAME_Rotator)
		{
			Names.Add(Prop->GetFName());
		}
	}
	Names.Sort([](const FName& A, const FName& B)
	{
		return A.ToString() < B.ToString();
	});
	Names.Insert(NAME_None, 0);
	return Names;
}
#endif // WITH_EDITOR
