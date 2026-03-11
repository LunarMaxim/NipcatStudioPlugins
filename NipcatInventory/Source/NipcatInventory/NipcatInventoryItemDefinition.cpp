// Copyright Epic Games, Inc. All Rights Reserved.

#include "NipcatInventoryItemDefinition.h"

#include "NipcatInventoryComponent.h"
#include "NipcatInventoryItemInstance.h"
#include "Interface/NipcatInventoryComponentOwnerInterface.h"
#include "Templates/SubclassOf.h"
#include "UObject/AssetRegistryTagsContext.h"
#include "UObject/ObjectPtr.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(NipcatInventoryItemDefinition)

//////////////////////////////////////////////////////////////////////
// UNipcatInventoryItemFragment

void UNipcatInventoryItemFragment::OnInstanceCreated_Implementation()
{
}

void UNipcatInventoryItemFragment::PreInstanceRemoved_Implementation()
{
}

void UNipcatInventoryItemFragment::Init()
{
	ItemInstance = Cast<UNipcatInventoryItemInstance>(GetOuter());
	InventoryComponent = ItemInstance->GetInventoryComponent();
	NotifyAvatarActorChanged();
	ItemDef = ItemInstance->GetItemDef();
	OnInitialize();
}

void UNipcatInventoryItemFragment::OnPickUpItem(int32 Count, bool IsNewItem)
{
	BP_OnPickUpItem(Count, IsNewItem);
}

void UNipcatInventoryItemFragment::NotifyAvatarActorChanged()
{
	const auto& OldAvatarActor = AvatarActor;
	const auto& NewAvatarActor = INipcatInventoryComponentOwnerInterface::Execute_GetAvatarActor(InventoryComponent->GetOwner());
	PreAvatarActorChanged(OldAvatarActor, NewAvatarActor);
	AvatarActor = NewAvatarActor;
	PostAvatarActorChanged(OldAvatarActor, NewAvatarActor);
}

void UNipcatInventoryItemFragment::PreAvatarActorChanged(AActor* OldAvatarActor, AActor* NewAvatarActor)
{
	K2_PreAvatarActorChanged(OldAvatarActor, NewAvatarActor);
}

void UNipcatInventoryItemFragment::PostAvatarActorChanged(AActor* OldAvatarActor, AActor* NewAvatarActor)
{
	K2_PostAvatarActorChanged(OldAvatarActor, NewAvatarActor);
}

void UNipcatInventoryItemFragment::OnItemCountChanged(int32 OldCount, int32 NewCount, int32 DeltaCount)
{
	K2_OnItemCountChanged(OldCount, NewCount, DeltaCount);
}

void UNipcatInventoryItemFragment::OnSaveGameLoaded()
{
	K2_OnSaveGameLoaded();
}

UWorld* UNipcatInventoryItemFragment::GetWorld() const
{
	const TIndirectArray<FWorldContext>& WorldContexts = GEngine->GetWorldContexts();
	for (const FWorldContext& Context : WorldContexts)
	{
		if (((Context.WorldType == EWorldType::PIE) || (Context.WorldType == EWorldType::Game)) && (Context.World() != NULL))
		{
			return Context.World();
		}
	}
	return nullptr;
}

//////////////////////////////////////////////////////////////////////
// UNipcatInventoryItemDefinition

void UNipcatInventoryItemDefinition::GetAssetRegistryTags(FAssetRegistryTagsContext Context) const
{
	Super::GetAssetRegistryTags(Context);
	
	Context.AddTag(FAssetRegistryTag(
		FName("DisplayName"),
		BaseData.DisplayName.ToString(),
		FAssetRegistryTag::TT_Alphabetical
	));
	
	Context.AddTag(FAssetRegistryTag(
		FName("Type"),
		BaseData.Type.ToString(),
		FAssetRegistryTag::TT_Alphabetical
	));
	
	Context.AddTag(FAssetRegistryTag(
			FName("Rare"),
			BaseData.Rare.ToString(),
			FAssetRegistryTag::TT_Alphabetical
	));
	
	Context.AddTag(FAssetRegistryTag(
			FName("HasIcon"),
			BaseData.Icon.IsNull() ? TEXT("false") : TEXT("true"),
			FAssetRegistryTag::TT_Alphabetical
	));
	
	Context.AddTag(FAssetRegistryTag(
			FName("HasThumbnail"),
			BaseData.Thumbnail.IsNull() ? TEXT("false") : TEXT("true"),
			FAssetRegistryTag::TT_Alphabetical
	));
	
	for (auto& [Name, String] : BP_GetAssetRegistryTags())
	{
		Context.AddTag(FAssetRegistryTag(
		Name,
		String,
		FAssetRegistryTag::TT_Alphabetical
	));
	}
}

FPrimaryAssetId UNipcatInventoryItemDefinition::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(FPrimaryAssetType(TEXT("NipcatInventoryItemDefinition")), GetFName());
}

const UNipcatInventoryItemFragment* UNipcatInventoryItemDefinition::FindFragmentByClass(const TSubclassOf<UNipcatInventoryItemFragment> FragmentClass) const
{
	if (FragmentClass != nullptr)
	{
		for (const UNipcatInventoryItemFragment* Fragment : Fragments)
		{
			if (Fragment && Fragment->IsA(FragmentClass))
			{
				return Fragment;
			}
		}
	}

	return nullptr;
}

TSubclassOf<UNipcatInventoryItemInstance> UNipcatInventoryItemDefinition::GetInstanceClass() const
{
	if (InstanceClass)
	{
		return InstanceClass;
	}
	return UNipcatInventoryItemInstance::StaticClass();
}

UTexture2D* UNipcatInventoryItemDefinition::GetIcon() const
{
	if (BaseData.Icon)
	{
		return BaseData.Icon.Get();
	}
	return nullptr;
}