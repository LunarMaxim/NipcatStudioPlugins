// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WorldConditionBase.h"
#include "WorldConditionSchema.h"
#include "NipcatAbilitySystemWorldConditionSchema.generated.h"

USTRUCT(meta=(Hidden))
struct NIPCATABILITYSYSTEM_API FNipcatAbilitySystemWorldConditionBase : public FWorldConditionBase
{
	GENERATED_BODY()

	FNipcatAbilitySystemWorldConditionBase() = default;

	virtual bool Initialize(const UWorldConditionSchema& Schema) override;
	
	FWorldConditionContextDataRef ASCRef;
};

/**
 * 
 */
UCLASS()
class NIPCATABILITYSYSTEM_API UNipcatAbilitySystemWorldConditionSchema : public UWorldConditionSchema
{
	GENERATED_BODY()

public:
	explicit UNipcatAbilitySystemWorldConditionSchema(const FObjectInitializer& ObjectInitializer);

	FWorldConditionContextDataRef GetASCRef() const {return ASCRef; }
	
protected:
	virtual bool IsStructAllowed(const UScriptStruct* InScriptStruct) const override
	{
		return Super::IsStructAllowed(InScriptStruct) || InScriptStruct->IsChildOf(TBaseStructure<FNipcatAbilitySystemWorldConditionBase>::Get());
	}
	
	FWorldConditionContextDataRef ASCRef;
};
