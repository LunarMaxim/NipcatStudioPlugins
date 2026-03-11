// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatQuestData.h"

const FPrimaryAssetType UNipcatQuestData::QuestType = TEXT("NipcatQuestData");

FPrimaryAssetId UNipcatQuestData::GetPrimaryAssetId() const
{
    return FPrimaryAssetId(QuestType, GetFName());
}
