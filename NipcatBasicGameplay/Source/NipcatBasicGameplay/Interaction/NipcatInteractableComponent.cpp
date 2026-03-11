// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatInteractableComponent.h"

#include "NipcatInteractableInterface.h"
#include "NipcatInteractComponent.h"
#include "NipcatInteractData.h"
#include "NipcatBasicGameplay/ActorTag/NipcatActorTagSubsystem.h"
#include "NipcatBasicGameplay/FunctionLibrary/NipcatBasicGameplayFunctionLibrary.h"
#include "NipcatBasicGameplay/UI/IndicatorSystem/NipcatIndicatorComponent.h"

#if WITH_EDITOR
#include "AssetToolsModule.h"
#include "IAssetTools.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#endif

UNipcatInteractableComponent::UNipcatInteractableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bAutoActivate = Activated;
}

const FNipcatInteractConfig& UNipcatInteractableComponent::GetInteractConfig() const
{
	return InteractData ? InteractData->InteractConfig : InteractConfig;
}

void UNipcatInteractableComponent::Activate(bool bReset)
{
	Super::Activate(bReset);
	if (GetWorld() && GetWorld()->IsGameWorld())
	{
		GetWorld()->GetSubsystem<UNipcatActorTagSubsystem>()->RegisterActor(GetOwner(), FGameplayTagContainer(NipcatInteractTags::Actor_Interactable));
	}
	Activated = true;
}

void UNipcatInteractableComponent::Deactivate()
{
	if (GetWorld() && GetWorld()->IsGameWorld())
	{
		GetWorld()->GetSubsystem<UNipcatActorTagSubsystem>()->UnregisterActor(GetOwner(), FGameplayTagContainer(NipcatInteractTags::Actor_Interactable));
	}
	Activated = false;
	Super::Deactivate();
}

#if WITH_EDITOR
void UNipcatInteractableComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if (PropertyName == GET_MEMBER_NAME_CHECKED(UNipcatInteractableComponent, bActionConvert))
	{
		if (bActionConvert)
		{
			if (HasAnyFlags(RF_ArchetypeObject))
			{
				bool bShouldExecute = true;
				
				if (const auto* ParentCDO = Cast<UNipcatInteractableComponent>(GetArchetype()))
				{
					if (ParentCDO->bActionConvert)
					{
						bShouldExecute = false;
					}
				}

				if (bShouldExecute)
				{
					ConvertInteractConfigToInteractData();
				}
			}
			
			bActionConvert = false; 
		}
	}
}

void UNipcatInteractableComponent::ConvertInteractConfigToInteractData()
{
    // 1. 安全检查
    if (InteractData != nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("InteractData is already set. Operation aborted."));
        return;
    }

    // 2. 获取当前上下文路径
    // GetPackage() 在蓝图编辑器中会返回蓝图资产的包，例如 "/Game/Blueprints/BP_MyActor"
    UPackage* CurrentPackage = GetPackage();
    FString PackagePath = FPackageName::GetLongPackagePath(CurrentPackage->GetName());
    
    // 获取蓝图名字作为前缀 (去除路径)
    FString BlueprintName = FPaths::GetBaseFilename(CurrentPackage->GetName());

    // 3. 构造新资产名称
    FString NewAssetName = FString::Printf(TEXT("%s"), *BlueprintName.Replace(TEXT("BP_"), TEXT("InteractData_")));
    
    // 4. 准备创建资产
    IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

    // 检查文件是否已存在，避免覆盖
    // 如果存在，尝试加个后缀 _1, _2 等
    FString NewPackageName = PackagePath + "/" + NewAssetName;
    AssetTools.CreateUniqueAssetName(NewPackageName, TEXT(""), NewPackageName, NewAssetName);

    // 5. 创建资产
    UObject* NewAsset = AssetTools.CreateAsset(
        NewAssetName,
        PackagePath,
        UNipcatInteractData::StaticClass(),
        nullptr
    );

    UNipcatInteractData* NewInteractData = Cast<UNipcatInteractData>(NewAsset);

    if (NewInteractData)
    {
        // 6. 开启事务 (Undo/Redo 支持)
        this->Modify();
        NewInteractData->Modify();

        // 7. 数据迁移
        NewInteractData->InteractConfig = this->InteractConfig;
    	
    	UNipcatBasicGameplayFunctionLibrary::DeepCopyInstancedObjects(
					FNipcatInteractConfig::StaticStruct(), 
					&NewInteractData->InteractConfig, 
					NewInteractData // 新的 Outer
				);
    	
        // 8. 设置引用并清空本地配置
        this->InteractData = NewInteractData;
        
        // 可选：清空本地结构体，恢复默认值
        this->InteractConfig = FNipcatInteractConfig();

        // 9. 标记脏数据 (保存提示)
        NewInteractData->MarkPackageDirty();
        this->MarkPackageDirty();

        UE_LOG(LogTemp, Log, TEXT("Successfully created %s and assigned to component."), *NewAssetName);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create InteractData asset."));
    }
#endif
}

void UNipcatInteractableComponent::EnableState(const FGameplayTagContainer& StateTag)
{
	DisabledStateTags.RemoveTags(StateTag);
}

void UNipcatInteractableComponent::DisableState(const FGameplayTagContainer& StateTag)
{
	DisabledStateTags.AppendTags(StateTag);
}

bool UNipcatInteractableComponent::IsStateEnabled(const FGameplayTag& StateTag)
{
	return !DisabledStateTags.HasTag(StateTag);
}

FText UNipcatInteractableComponent::GetInteractName(const FGameplayTag& StateTag)
{
	for (auto& State : GetInteractConfig().States)
	{
		if (State.StateTag == StateTag)
		{
			return State.InteractName;
		}
	}
	return FText();
}

UNipcatIndicatorComponent* UNipcatInteractableComponent::GetIndicator()
{
	if (GetOwner()->GetClass()->ImplementsInterface(UNipcatInteractableInterface::StaticClass()))
	{
		return INipcatInteractableInterface::Execute_GetInteractIndicator(GetOwner(), this);
	}
	return nullptr;
}

FVector UNipcatInteractableComponent::GetInteractPointLocation()
{
	if (const auto Indicator = GetIndicator())
	{
		return Indicator->GetComponentLocation();
	}
	return GetOwner()->GetActorLocation();
}

void UNipcatInteractableComponent::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
	if (Ar.IsSaveGame())
	{
		DisabledStateTags.Serialize(FStructuredArchiveFromArchive(Ar).GetSlot());
		if (Ar.IsLoading())
		{
			if (Activated && !IsActive())
			{
				Activate(false);
			}
			else if (!Activated && IsActive())
			{
				Deactivate();
			}
		}
	}
}
