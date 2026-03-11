// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameplayEffect.h"
#include "AnimNotify/LinkedAnimNotify/NipcatLinkedAnimNotify.h"
#include "EffectContext/NipcatGameplayEffectContext.h"
#include "EffectContext/NipcatPlayerMontageParameter.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "NipcatAbilitySystemBlueprintFunctionLibrary.generated.h"

struct FNipcatFloatComparator;
struct FNipcatNameRequirements;
struct FNipcatFloatParameter;
class UNipcatAbilityTreeDefine;


/**
 * 
 */
UCLASS()
class NIPCATABILITYSYSTEM_API UNipcatAbilitySystemBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Nipcat Ability System Function Library|Animation", meta=(ScriptMethod))
	static UObject* GetAnimNotifyObject(const FAnimNotifyEvent& AnimNotifyEvent);
	
	UFUNCTION(BlueprintPure, Category = "Nipcat Ability System Function Library|Animation", meta=(ScriptMethod))
	static bool DoesAnimNotifyEventImplementInterface(const FAnimNotifyEvent& AnimNotifyEvent, const TSubclassOf<UInterface> Interface);
	
	UFUNCTION(BlueprintCallable, Category = "Nipcat Ability System Function Library|Animation", meta=(ScriptMethod, AutoCreateRefTerm="MontagePlaySetting"))
	static void GetAnimNotifies(UAnimMontage* AnimMontage, const FNipcatMontagePlaySetting& MontagePlaySetting, TArray<FAnimNotifyData>& OutNotifies);

	UFUNCTION(BlueprintCallable, Category = "Nipcat Ability System Function Library|Animation", meta=(ScriptMethod, AutoCreateRefTerm="MontagePlaySetting"))
	static void GetAnimNotifiesInSection(UAnimMontage* AnimMontage, const FNipcatMontagePlaySetting& MontagePlaySetting, FName SectionName, TArray<FAnimNotifyData>& OutNotifies);

	UFUNCTION(BlueprintCallable, Category = "Nipcat Ability System Function Library|Animation")
	static void SortNotifies(UPARAM(ref) TArray<FAnimNotifyData>& Notifies);
	
	UFUNCTION(BlueprintCallable, Category = "Nipcat Ability System Function Library|Animation", meta=(ScriptMethod))
	static void GetAllAnimNotifiesByInterface(UAnimMontage* AnimMontage, const TSubclassOf<UInterface> Interface, TArray<FAnimNotifyData>& OutNotifies, bool IncludeLinkedNotify = false);

	UFUNCTION(BlueprintCallable, Category = "Nipcat Ability System Function Library|Animation", meta=(ScriptMethod))
	static void GetAnimNotifyStatesByClass(UAnimMontage* AnimMontage, const TSubclassOf<UAnimNotifyState> Class, TArray<FAnimNotifyData>& OutNotifies, bool IncludeLinkedNotify = false);

	UFUNCTION(BlueprintCallable, Category = "Nipcat Ability System Function Library|Animation", meta=(ScriptMethod))
	static void GetAnimNotifyStatesByInterface(UAnimMontage* AnimMontage, const TSubclassOf<UInterface> Interface, TArray<FAnimNotifyData>& OutNotifies, bool IncludeLinkedNotify = false);

	UFUNCTION(BlueprintCallable, Category = "Nipcat Ability System Function Library|Animation", meta=(ScriptMethod, CallableWithoutWorldContext), CallInEditor)
	static bool RemoveInvalidNotifies(UAnimSequenceBase* AnimationSequenceBase);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Nipcat Ability System Function Library|Animation")
	static float GetFloatCurveValue(UAnimSequenceBase* Animation, FName CurveName, float Time, float DefaultValue = 0.f);

	UFUNCTION(BlueprintCallable, Category = "Nipcat Ability System Function Library|Animation")
	static float EvaluateRuntimeCurve(float InEvaluatePos, const FRuntimeFloatCurve& InCurve);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Nipcat Ability System Function Library|Animation")
	static double CalculateCurveQuadrature(UCurveFloat* Curve, int SubSteps = 5, double StartTime = 0.f, double End = 0.f);

	UFUNCTION(BlueprintPure, Category = "Nipcat Ability System Function Library|Animation", meta=(ScriptMethod, CallableWithoutWorldContext, AutoCreateRefTerm="InMontagePlaySetting,InOverrideMap,MontageTag", Categories="Montage"))
	static FNipcatMontagePlaySetting GetOverriddenMontagePlaySetting(const FNipcatMontagePlaySetting& InMontagePlaySetting, const FNipcatMontagePlaySettingMap& InOverrideMap, const FGameplayTag& MontageTag);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Nipcat Ability System Function Library|Ability", meta=(DefaultToSelf="Ability"))
	static ACharacter* GetAvatarCharacterFromAbility(UGameplayAbility* Ability);

	UFUNCTION(BlueprintCallable, Category = "Nipcat Ability System Function Library|Ability", meta=(DefaultToSelf="Ability"))
	static bool SetCooldownDuration(UGameplayAbility* Ability, float NewDuration);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Nipcat Ability System Function Library|Ability", meta=(DefaultToSelf="Ability"))
	static FGameplayAbilitySpecHandle GetCurrentAbilitySpecHandle(UGameplayAbility* Ability);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Nipcat Ability System Function Library|Ability", meta=(DefaultToSelf="Ability"))
	static FGameplayEffectSpecHandle MakeOutgoingGameplayEffectSpecByDamageDefinition(UGameplayAbility* Ability, TSubclassOf<UGameplayEffect> GameplayEffectClass, const FDamageDefinition& DamageDefinition, float Level = 1.f);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Nipcat Ability System Function Library|AbilitySystemComponent", meta=(ScriptMethod, DefaultToSelf="GameplayAbility"))
	static FNipcatGameplayAbilityActorInfo GetNipcatAbilityActorInfoFromAbility(UGameplayAbility* GameplayAbility);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Nipcat Ability System Function Library|AbilitySystemComponent", meta=(ScriptMethod, DefaultToSelf="GameplayAbility"))
	static FNipcatMontagePlaySettingMap GetMontagePlaySettingOverrideMapFromActorInfo(UGameplayAbility* GameplayAbility);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Nipcat Ability System Function Library|EffectContext")
	static const UGameplayAbility* GetSourceAbilityInstanceNotReplicatedFromEffectContext(const FGameplayEffectContextHandle& EffectContext);

	UFUNCTION(BlueprintCallable, Category = "Nipcat Ability System Function Library|EffectContext")
	static void AddActor(FGameplayEffectContextHandle ContextHandle, AActor* Actor, bool bReset = false);

	/** Returns the physical material from the hit result if there is one */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Nipcat Ability System Function Library|EffectContext")
	static const UPhysicalMaterial* GetPhysicalMaterial(const FGameplayEffectContextHandle& ContextHandle);

	UFUNCTION(BlueprintCallable, Category = "Nipcat Ability System Function Library|EffectContext")
	static void SetDamageInstance(FGameplayEffectContextHandle ContextHandle, UDamageInstance* DamageInstance);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Nipcat Ability System Function Library|EffectContext")
	static UDamageInstance* GetDamageInstance(const FGameplayEffectContextHandle& ContextHandle);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Nipcat Ability System Function Library|EffectContext")
	static FDamageDefinition GetDamageDefinition(const FGameplayEffectContextHandle& ContextHandle);

	/** Gets the GameplayEffectSpec's effect context handle */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Nipcat Ability System Function Library|GameplayEffectSpec")
	static FGameplayEffectContextHandle GetEffectContextHandleFromEffectSpec(const FGameplayEffectSpec& Spec);

	/** Gets the GameplayEffectSpec's GameplayEffect CDO */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Nipcat Ability System Function Library|GameplayEffectSpec")
	static const UGameplayEffect* GetGameplayEffectDefFromEffectSpec(const FGameplayEffectSpecHandle& Spec);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Nipcat Ability System Function Library|GameplayEffectSpec", meta=(ScriptMethod, AutoCreateRefTerm="Tag", Categories="SetByCaller", CallableWithoutWorldContext))
	static float GetSetByCallerMagnitudeByTag(const FGameplayEffectSpecHandle& EffectSpec, const FGameplayTag& Tag, const bool TryParentTagIfNotFound = true, const bool WarnIfNotFound = false, const float DefaultIfNotFound = 0.f);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Nipcat Ability System Function Library|GameplayEffectSpec", meta=(ScriptMethod, AutoCreateRefTerm="MagnitudeName", CallableWithoutWorldContext))
	static float GetSetByCallerMagnitudeByName(const FGameplayEffectSpecHandle& EffectSpec, const FName& MagnitudeName, const bool WarnIfNotFound = false, const float DefaultIfNotFound = 0.f);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Nipcat Ability System Function Library|AbilitySystemComponent", meta=(ScriptMethod, DefaultToSelf="ASC"))
	static FNipcatGameplayAbilityActorInfo GetNipcatAbilityActorInfo(UAbilitySystemComponent* ASC);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Nipcat Ability System Function Library|AbilitySystemComponent")
	static FGameplayAbilitySpec GetHandleAbilitySpec(UAbilitySystemComponent* ASC, FGameplayAbilitySpecHandle AbilitySpecHandle);

	/** 激活GA并返回GA实例对象，可以用于绑定事件以监听Ability的结束 */
	UFUNCTION(BlueprintCallable, Category = "Nipcat Ability System Function Library|AbilitySystemComponent", meta=(DefaultToSelf="ASC"))
	static void TryActivateAbilityByClassAndReturnInstance(UAbilitySystemComponent* ASC, TSubclassOf<UGameplayAbility> InAbilityToActivate, bool& bSuccess, UGameplayAbility*& Instance, bool bAllowRemoteActivation = true);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Nipcat Ability System Function Library|AbilitySystemComponent", meta=(DefaultToSelf="ASC"))
	static bool GetActivatableAbilitySpecHandleByClass(UAbilitySystemComponent* ASC, TSubclassOf<UGameplayAbility> InAbilityClass, FGameplayAbilitySpecHandle& Handle);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Nipcat Ability System Function Library|AbilitySystemComponent", meta=(DefaultToSelf="ASC"))
	static bool GetCooldownRemainingByClass(UAbilitySystemComponent* ASC, TSubclassOf<UGameplayAbility> InAbilityClass, float& CooldownRemaining, float& CooldownDuration);

	/** Returns a list of currently active ability instances that match the tags */
	UFUNCTION(BlueprintCallable, Category = "Nipcat Ability System Function Library|AbilitySystemComponent", meta=(DefaultToSelf="ASC"))
	static void GetActiveAbilitiesWithTags(UAbilitySystemComponent* ASC, const FGameplayTagContainer& GameplayTagContainer, TArray<UGameplayAbility*>& ActiveAbilities);

	/** 修改已激活GE的持续时间 该操作包含一个const_cast, 这可能不是Epic希望的修改持续时间的方法, 但是迄今为止它看起来运行得很好。 */
	UFUNCTION(BlueprintCallable, Category = "Nipcat Ability System Function Library|AbilitySystemComponent", meta=(DefaultToSelf="ASC"))
	static bool SetGameplayEffectDurationHandle(UAbilitySystemComponent* ASC, FActiveGameplayEffectHandle Handle, float NewDuration);

	UFUNCTION(BlueprintPure, Category = "Nipcat Ability System Function Library|AbilitySystemComponent", meta = (CallableWithoutWorldContext, ScriptMethod))
	static FGameplayEffectContextHandle GetEffectContextFromActiveGEHandle(const FActiveGameplayEffectHandle& ActiveGameplayEffectHandle);
	
	UFUNCTION(BlueprintCallable, Category = "Nipcat Ability System Function Library|AbilitySystemComponent", meta=(DefaultToSelf="ASC"))
	static void CancelAbility(UAbilitySystemComponent* ASC, UGameplayAbility* Ability);

	UFUNCTION(BlueprintCallable, Category = "Nipcat Ability System Function Library|AbilitySystemComponent", meta=(DefaultToSelf="ASC"))
	static void CancelAbilityByClass(UAbilitySystemComponent* ASC, TSubclassOf<UGameplayAbility> AbilityClass);

	UFUNCTION(BlueprintCallable, Category = "Nipcat Ability System Function Library|AbilitySystemComponent", meta=(DefaultToSelf="ASC"))
	static void CancelAbilities(UAbilitySystemComponent* ASC, const FGameplayTagContainer WithTags, const FGameplayTagContainer WithoutTags, UGameplayAbility* Ignore);

	UFUNCTION(BlueprintCallable, Category = "Nipcat Ability System Function Library|AbilitySystemComponent", meta=(DefaultToSelf="ASC"))
	static void CancelAllAbilities(UAbilitySystemComponent* ASC, UGameplayAbility* Ignore);

	UFUNCTION(BlueprintPure, Category = "Nipcat Ability System Function Library|AbilitySystemComponent", meta=(DefaultToSelf="ASC", ScriptMethod))
	static FGameplayAbilityActorInfo GetAbilityActorInfo(const UAbilitySystemComponent* ASC);

	UFUNCTION(BlueprintPure, Category = "Nipcat Ability System Function Library|AbilitySystemComponent", meta=(DefaultToSelf="ASC", ScriptMethod))
	static AActor* GetAvatarActorFromAbilityActorInfo(const UAbilitySystemComponent* ASC);

	UFUNCTION(BlueprintPure, Category = "Nipcat Ability System Function Library|AbilitySystemComponent", meta=(DefaultToSelf="ASC", ScriptMethod))
	static FActiveGameplayEffect GetActiveGameplayEffect(const UAbilitySystemComponent* ASC, const FActiveGameplayEffectHandle Handle);

	UFUNCTION(BlueprintPure, Category = "Nipcat Ability System Function Library|AbilitySystemComponent", meta=(DefaultToSelf="ASC", ScriptMethod))
	static bool CanApplyAttributeModifiers(const UAbilitySystemComponent* ASC, const UGameplayEffect *GameplayEffect, float Level, const FGameplayEffectContextHandle& EffectContext);
	
	UFUNCTION(BlueprintCallable, Category = "Nipcat Ability System Function Library|Event", Meta = (Tooltip = "This function can be used to trigger an ability on the actor in question with useful payload data."))
	static void SendGameplayEventsToActor(AActor* Actor, FGameplayTagContainer EventTags, FGameplayEventData Payload);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Nipcat Ability System Function Library|GameplayModifierEvaluatedData")
	static void BreakGameplayModifierEvaluatedData(const FGameplayModifierEvaluatedData& Data,
	                                               FGameplayAttribute& Attribute, TEnumAsByte<EGameplayModOp::Type>& ModifierOp, float& Magnitude,
	                                               FActiveGameplayEffectHandle& Handle, bool& IsValid);

	UFUNCTION(BlueprintCallable, Category = "Nipcat Ability System Function Library|AnimNotify")
	static void SetAnimNotifyEventTriggerTime(UPARAM(ref) FAnimNotifyEvent& NotifyEvent, float NewTime);
	
	UFUNCTION(BlueprintCallable, Category = "Nipcat Ability System Function Library|AnimNotify")
	static void SetAnimNotifyEventDuration(UPARAM(ref) FAnimNotifyEvent& NotifyEvent, float NewTime);

	UFUNCTION(BlueprintPure, Category = "Nipcat Ability System Function Library|AnimNotify")
	static UAnimSequenceBase* GetLinkedAnimSequenceBase(const FAnimNotifyEvent& NotifyEvent);
	
	UFUNCTION(BlueprintPure, Category = "Nipcat Ability System Function Library|Damage", meta=(ScriptMethod))
	static FDamageDefinition GetDamageDefinitionByHandle(FDamageDefinitionHandle Handle);
	
	UFUNCTION(BlueprintCallable, Category = "Nipcat Ability System Function Library|Damage", meta=(ScriptMethod))
	static void SetDamageDefinitionByHandle(FDamageDefinitionHandle Handle, const FDamageDefinition& Value);

	UFUNCTION(BlueprintCallable, Category = "Nipcat Ability System Function Library|Damage", meta=(ScriptMethod))
	static FDamageDefinitionHandle MakeDamageDefinitionHandle(const FDamageDefinition& DamageDefinition);

	UFUNCTION(BlueprintPure, Category = "Nipcat Ability System Function Library|Montage", meta=(ScriptMethod))
	static TArray<float> GetMontageSettingRandomWeights(const FNipcatMontagePlaySettingArray& MontagePlaySettingArray);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Nipcat Ability System Function Library", meta=(ScriptMethod, CallableWithoutWorldContext))
	static bool RequirementsMet(const FGameplayTagRequirements TagRequirements, const FGameplayTagContainer Container);

	/** Returns the ability system component that created this handle */
	UFUNCTION(BlueprintPure, Category = "Nipcat Ability System Function Library|ActiveGameplayEffectHandle", meta=(ScriptMethod, CallableWithoutWorldContext))
	static UAbilitySystemComponent* GetOwningAbilitySystemComponent(const FActiveGameplayEffectHandle& ActiveGameplayEffectHandle);

	UFUNCTION(BlueprintPure, Category = "Nipcat Ability System Function Library|AbilitySystemComponent", meta=(DefaultToSelf="ASC", ScriptMethod))
	static FActiveGameplayEffect GetActiveGameplayEffectFromHandle(const FActiveGameplayEffectHandle& ActiveGameplayEffectHandle);
	
	/** Returns the ability system component that created this handle */
	UFUNCTION(BlueprintPure, Category = "Nipcat Ability System Function Library|ActiveGameplayEffectHandle", meta=(ScriptMethod, CallableWithoutWorldContext))
	static FActiveGameplayEffectHandle GetHandle(const FActiveGameplayEffect& ActiveGameplayEffect);

	/** Returns the ability system component that created this handle */
	UFUNCTION(BlueprintPure, Category = "Nipcat Ability System Function Library|ActiveGameplayEffectHandle", meta=(ScriptMethod, CallableWithoutWorldContext))
	static float GetTimeRemaining(const FActiveGameplayEffect& ActiveGameplayEffect, float WorldTime);

	/** Returns the ability system component that created this handle */
	UFUNCTION(BlueprintPure, Category = "Nipcat Ability System Function Library|ActiveGameplayEffectHandle", meta=(ScriptMethod, CallableWithoutWorldContext))
	static float GetDuration(const FActiveGameplayEffect& ActiveGameplayEffect);

	/** Returns the ability system component that created this handle */
	UFUNCTION(BlueprintPure, Category = "Nipcat Ability System Function Library|ActiveGameplayEffectHandle", meta=(ScriptMethod, CallableWithoutWorldContext))
	static float GetPeriod(const FActiveGameplayEffect& ActiveGameplayEffect);
	
	/** Returns the ability system component that created this handle */
	UFUNCTION(BlueprintPure, Category = "Nipcat Ability System Function Library|ActiveGameplayEffectHandle", meta=(ScriptMethod, CallableWithoutWorldContext))
	static float GetEndTime(const FActiveGameplayEffect& ActiveGameplayEffect);
	
	/** Returns the ability system component that created this handle */
	UFUNCTION(BlueprintPure, Category = "Nipcat Ability System Function Library|ActiveGameplayEffectHandle", meta=(ScriptMethod, CallableWithoutWorldContext))
	static FGameplayEffectSpec GetEffectSpec(const FActiveGameplayEffect& ActiveGameplayEffect);
	
	UFUNCTION(BlueprintPure, Category = "Nipcat Ability System Function Library|FloatValue", meta=(ScriptMethod, CallableWithoutWorldContext))
	static double GetFloatParameterValue(const FNipcatFloatParameter& FloatParameter, const UAbilitySystemComponent* ASC, const UGameplayAbility* GameplayAbility = nullptr);
	
	UFUNCTION(BlueprintPure, Category = "Nipcat Ability System Function Library|FloatValue", meta=(ScriptMethod, CallableWithoutWorldContext))
	static bool Compare(const FNipcatFloatComparator& FloatComparator, const UAbilitySystemComponent* ASC, const UGameplayAbility* GameplayAbility = nullptr);
	
	UFUNCTION(BlueprintPure, Category = "Nipcat Ability System Function Library|NameRequirements", meta=(ScriptMethod, CallableWithoutWorldContext, AutoCreateRefTerm="Name"))
	static bool NameRequirementsMet(const FNipcatNameRequirements& NameRequirements, const FName& Name);
	
	UFUNCTION(BlueprintPure, Category = "Nipcat Ability System Function Library|NameRequirements", meta=(ScriptMethod, CallableWithoutWorldContext))
	static bool NameRequirementsMets(const FNipcatNameRequirements& NameRequirements, const TArray<FName>& Names);

	UFUNCTION(BlueprintPure, Category = "Nipcat Ability System Function Library|NameRequirements", meta=(ScriptMethod, CallableWithoutWorldContext, AutoCreateRefTerm="Tag"))
	static FORCEINLINE FGameplayTagContainer GetSingleTagContainer(const FGameplayTag& Tag) { return Tag.GetSingleTagContainer(); }
	
	UFUNCTION(BlueprintPure, Category = "Nipcat Ability System Function Library|NameRequirements", meta=(ScriptMethod, CallableWithoutWorldContext, AutoCreateRefTerm="Tag"))
	static FORCEINLINE FGameplayTagContainer GetGameplayTagParents(const FGameplayTag& Tag) { return Tag.GetGameplayTagParents(); }

	UFUNCTION(BlueprintPure, Category = "Nipcat Ability System Function Library|Attribute")
	static TArray<FString> GetAttributeGroupNames();

	UFUNCTION(BlueprintCallable, Category = "Nipcat Ability System Function Library|GameplayTags", meta=(ScriptMethod, CallableWithoutWorldContext, AutoCreateRefTerm="Query"))
	static const TArray<FGameplayTag>& GetGameplayTagArrayFromQuery(const FGameplayTagQuery& Query);
};
