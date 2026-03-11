// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"
#include "NativeGameplayTags.h"
#include "NipcatPlayerMontageParameter.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "NipcatAbilitySystem/FX/NipcatFXDefinition_BurstGameplayCue.h"
#include "NipcatGameplayEffectContext.generated.h"

class UNipcatAbilitySystemComponent;
class UAbilitySystemComponent;

UE_DECLARE_GAMEPLAY_TAG_EXTERN(DamageHitResult_None)

USTRUCT(BlueprintType)
struct NIPCATABILITYSYSTEM_API FDamageDefinition : public FTableRowBase
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta=(EditCondition = "UseWeight"), SaveGame)
	FGuid GUID = FGuid();
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool UseWeight = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(EditCondition = "UseWeight"))
	int32 WeightGroup = 0;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(EditCondition = "UseWeight"))
	float Weight = 1.f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(Categories="SetByCaller", ForceInlineRow, EditCondition = "!UseWeight"))
	TMap<FGameplayTag, float> SetByCallerMagnitudes;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(Categories="DamageLevel"))
	FGameplayTag DamageLevelTag;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(Categories="EffectLevel"))
	FGameplayTag EffectLevelTag;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(Categories="DamageDirection"))
	FGameplayTag DamageDirectionTag;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(Categories="DamageKnockBackDirection"))
	FGameplayTag DamageKnockBackDirectionTag;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(Categories="DamageOriginType"))
	FGameplayTag DamageOriginTypeTag;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(Categories="DamageTag"))
	FGameplayTagContainer DamageTags;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ForceInlineRow))
	TMap<TSubclassOf<UGameplayEffect>, int32> AdditionalEffectsApplyToOwner;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ForceInlineRow))
	TMap<TSubclassOf<UGameplayEffect>, int32> AdditionalEffectsApplyToTarget;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(InlineEditConditionToggle))
	bool bUseOverrideBurstEffects;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(EditCondition="bUseOverrideBurstEffects"))
	FNipcatFXDefinition_BurstGameplayCue OverrideBurstEffects;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(InlineEditConditionToggle))
	bool bUseAdditionalBurstEffects;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(EditCondition="bUseAdditionalBurstEffects"))
	FNipcatFXDefinition_BurstGameplayCue AdditionalBurstEffects;

	void PostSerialize(const FArchive& Ar)
	{
		GUID = FGuid::NewGuid();
	}
};

template<>
struct TStructOpsTypeTraits<FDamageDefinition> : public TStructOpsTypeTraitsBase2<FDamageDefinition>
{
	enum
	{
		WithPostSerialize = true,
	};
};
USTRUCT(BlueprintType)
struct NIPCATABILITYSYSTEM_API FDamageDefinitionHandle
{
	GENERATED_BODY()

private:
	FDamageDefinition* Data;

public:
	
	FDamageDefinitionHandle()
	{
	}

	virtual ~FDamageDefinitionHandle()
	{
	}
	
	/** Constructs from an existing definition, should be allocated by new */
	explicit FDamageDefinitionHandle(FDamageDefinition* DataPtr)
	{
		Data = DataPtr;
	}

	/** Sets from an existing definition, should be allocated by new */
	void operator=(FDamageDefinition* DataPtr)
	{
		Data = DataPtr;
	}

	void Clear()
	{
		Data = nullptr;
	}

	bool IsValid() const
	{
		return Data != nullptr;
	}

	/** Returns Raw damage definition, may be null */
	FDamageDefinition* Get()
	{
		return Data;
	}
	const FDamageDefinition* Get() const
	{
		return Data;
	}
};

UCLASS(Blueprintable, BlueprintType)
class NIPCATABILITYSYSTEM_API UDamageInstance : public UObject
{
	GENERATED_BODY()

public:

	virtual void BeginDestroy() override;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ExposeOnSpawn))
	FDamageDefinition DamageDefinition;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(Categories="DamageHitResult"))
	FGameplayTag DamageHitResultTag = DamageHitResult_None;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(Categories="DamageApplyResult"))
	FGameplayTagContainer DamageApplyResultTags;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(Categories="WeaponMaterial"))
	FGameplayTag WeaponMaterialTag;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector KnockBackDirection;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	double KnockBackDirectionAngle;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayEffectSpecHandle EffectSpec;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ExposeOnSpawn))
	TObjectPtr<UDamageInstance> SourceDamageInstance;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	AActor* TargetActor;
	
	UFUNCTION(BlueprintCallable)
	void SetTargetActor(AActor* InTargetActor);
	
	UFUNCTION(BlueprintCallable)
	void SetHitResult(FHitResult InHitResult);
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FHitResult HitResult;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TEnumAsByte<EPhysicalSurface> HitResultSurfaceType = SurfaceType_Default;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UAbilitySystemComponent* InstigatorAbilitySystemComponent;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UAbilitySystemComponent* TargetAbilitySystemComponent;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FGameplayTagContainer InstigatorOwnedTags;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FGameplayTagContainer TargetOwnedTags;

	FDelegateHandle InstigatorGameplayTagCountChangedDelegate;
	
	FDelegateHandle TargetGameplayTagCountChangedDelegate;
	
	UFUNCTION()
	void OnInstigatorGameplayTagCountChanged(const FGameplayTag Tag, int32 Count);
	
	UFUNCTION()
	void OnTargetGameplayTagCountChanged(const FGameplayTag Tag, int32 Count);
};

USTRUCT()
struct FNipcatGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()

	FNipcatGameplayEffectContext()
		: FGameplayEffectContext()
	{
	}

	FNipcatGameplayEffectContext(AActor* InInstigator, AActor* InEffectCauser)
		: FGameplayEffectContext(InInstigator, InEffectCauser)
	{
	}

	UPROPERTY()
	TWeakObjectPtr<UDamageInstance> DamageInstance;
	
	/** Returns the wrapped FNipcatGameplayEffectContext from the handle, or nullptr if it doesn't exist or is the wrong type */
	static NIPCATABILITYSYSTEM_API FNipcatGameplayEffectContext* ExtractEffectContext(struct FGameplayEffectContextHandle Handle);

	virtual FGameplayEffectContext* Duplicate() const override
	{
		FNipcatGameplayEffectContext* NewContext = new FNipcatGameplayEffectContext();
		*NewContext = *this;
		if (GetHitResult())
		{
			const FVector Origin = GetOrigin();
			// Does a deep copy of the hit result
			NewContext->AddHitResult(*GetHitResult(), true);
			// Avoid WorldOrigin Overriden by AddHitResult
			NewContext->AddOrigin(Origin);
		}
		NewContext->DamageInstance = DamageInstance;
		return NewContext;
	}

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FNipcatGameplayEffectContext::StaticStruct();
	}

	/** Overridden to serialize new fields */
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;
	
};

template<>
struct TStructOpsTypeTraits<FNipcatGameplayEffectContext> : public TStructOpsTypeTraitsBase2<FNipcatGameplayEffectContext>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true
	};
};

USTRUCT()
struct FNipcatGameplayAbilityActorInfo : public FGameplayAbilityActorInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<TSubclassOf<UGameplayAbility>, FNipcatMontagePlaySettingMap> MontagePlaySettingOverride;
	
	static NIPCATABILITYSYSTEM_API FNipcatGameplayAbilityActorInfo* ExtractActorInfo(struct FGameplayAbilityActorInfo* ActorInfo);

};
