// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatComboDefinition.h"

#include "AbilitySystemComponent.h"
#include "GameplayAbilities/Public/Abilities/GameplayAbility.h"

#include "NipcatBasicGameplay/FunctionLibrary/NipcatBasicGameplayFunctionLibrary.h"


#if WITH_EDITOR
void UNipcatComboDefinitionConfig::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);
	
#if WITH_EDITORONLY_DATA
	if (PropertyChangedEvent.Property)
	{
		if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_STRING_VIEW_CHECKED(FNipcatComboDefinition, Comment))
		{
			const auto Index = PropertyChangedEvent.GetArrayIndex(GET_MEMBER_NAME_STRING_CHECKED(UNipcatComboDefinitionConfig, ComboDefinitions));

			const FMapProperty* MapProperty = CastField<FMapProperty>(PropertyChangedEvent.PropertyChain.GetActiveMemberNode()->GetValue());
			FScriptMapHelper MapHelper(MapProperty, MapProperty->ContainerPtrToValuePtr<void>(this));
                
			if (MapHelper.IsValidIndex(Index))
			{
				// 获取Value
				uint8* ValuePtr = MapHelper.GetValuePtr(Index);
                    
				// 转换为你的结构体类型
				FNipcatComboDefinition& ComboDefinition = *reinterpret_cast<FNipcatComboDefinition*>(ValuePtr);
				
				if (ComboDefinition.Comment.IsEmpty())
				{
					ComboDefinition.bCommentModified = false;
					UNipcatBasicGameplayFunctionLibrary::TryUpdateComment(ComboDefinition.Comment, ComboDefinition.Ability, ComboDefinition.bCommentModified);
				}
				else if (ComboDefinition.Comment != UNipcatBasicGameplayFunctionLibrary::GetClassObjectName(ComboDefinition.Ability))
				{
					ComboDefinition.bCommentModified = true;
				}
			}
		}
		if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_STRING_VIEW_CHECKED(FNipcatComboDefinition, Ability))
		{
			const auto Index = PropertyChangedEvent.GetArrayIndex(GET_MEMBER_NAME_STRING_CHECKED(UNipcatComboDefinitionConfig, ComboDefinitions));

			const FMapProperty* MapProperty = CastField<FMapProperty>(PropertyChangedEvent.PropertyChain.GetActiveMemberNode()->GetValue());
			FScriptMapHelper MapHelper(MapProperty, MapProperty->ContainerPtrToValuePtr<void>(this));
                
			if (MapHelper.IsValidIndex(Index))
			{
				// 获取Value
				uint8* ValuePtr = MapHelper.GetValuePtr(Index);
                    
				// 转换为你的结构体类型
				FNipcatComboDefinition& ComboDefinition = *reinterpret_cast<FNipcatComboDefinition*>(ValuePtr);
				
				UNipcatBasicGameplayFunctionLibrary::TryUpdateComment(ComboDefinition.Comment, ComboDefinition.Ability, ComboDefinition.bCommentModified);
			}
		}
	}
#endif
}

void UNipcatComboDefinitionConfig::PostLoad()
{
	Super::PostLoad();
	for (auto& [Tag, ComboDefinition] : ComboDefinitions)
	{
		UNipcatBasicGameplayFunctionLibrary::TryUpdateComment(ComboDefinition.Comment, ComboDefinition.Ability, ComboDefinition.bCommentModified);
	}
}

#endif

