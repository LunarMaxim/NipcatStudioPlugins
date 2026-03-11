// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatStatFunctionLibrary.h"

void UNipcatStatFunctionLibrary::SetBoolStat(TScriptInterface<INipcatStatInterface> StatOwner, FGameplayTag Tag,
	bool NewValue)
{
	if (!Tag.IsValid() || !StatOwner.GetObject()) { return; }
	FNipcatStat& Stat = StatOwner->GetStat();
	Stat.BoolStat.Add(Tag, NewValue);
}

void UNipcatStatFunctionLibrary::SetIntStat(TScriptInterface<INipcatStatInterface> StatOwner, FGameplayTag Tag,
	int32 NewValue)
{
	if (!Tag.IsValid() || !StatOwner.GetObject()) { return; }
	FNipcatStat& Stat = StatOwner->GetStat();
	Stat.IntStat.Add(Tag, NewValue);
}

void UNipcatStatFunctionLibrary::SetFloatStat(TScriptInterface<INipcatStatInterface> StatOwner, FGameplayTag Tag,
	float NewValue)
{
	if (!Tag.IsValid() || !StatOwner.GetObject()) { return; }
	FNipcatStat& Stat = StatOwner->GetStat();
	Stat.FloatStat.Add(Tag, NewValue);
}

void UNipcatStatFunctionLibrary::SetEnumStat(TScriptInterface<INipcatStatInterface> StatOwner, FGameplayTag Tag,
	uint8 NewValue)
{
	if (!Tag.IsValid() || !StatOwner.GetObject()) { return; }
	FNipcatStat& Stat = StatOwner->GetStat();
	Stat.EnumStat.Add(Tag, NewValue);
}

bool UNipcatStatFunctionLibrary::GetBoolStat(const TScriptInterface<INipcatStatInterface>& StatOwner, FGameplayTag Tag)
{
	if (!Tag.IsValid() || !StatOwner.GetObject()) { return false; }
	FNipcatStat& Stat = StatOwner->GetStat();
	if (const auto FoundResult = Stat.BoolStat.Find(Tag))
	{
		return *FoundResult;
	}
	return false;
}

int32 UNipcatStatFunctionLibrary::GetIntStat(const TScriptInterface<INipcatStatInterface>& StatOwner, FGameplayTag Tag)
{
	if (!Tag.IsValid() || !StatOwner.GetObject()) { return 0; }
	FNipcatStat& Stat = StatOwner->GetStat();
	if (const auto FoundResult = Stat.IntStat.Find(Tag))
	{
		return *FoundResult;
	}
	return 0;
}

float UNipcatStatFunctionLibrary::GetFloatStat(const TScriptInterface<INipcatStatInterface>& StatOwner, FGameplayTag Tag)
{
	if (!Tag.IsValid() || !StatOwner.GetObject()) { return 0.f; }
	FNipcatStat& Stat = StatOwner->GetStat();
	if (const auto FoundResult = Stat.FloatStat.Find(Tag))
	{
		return *FoundResult;
	}
	return 0.f;
}

uint8 UNipcatStatFunctionLibrary::GetEnumStat(const TScriptInterface<INipcatStatInterface>& StatOwner, FGameplayTag Tag)
{
	if (!Tag.IsValid() || !StatOwner.GetObject()) { return 0; }
	FNipcatStat& Stat = StatOwner->GetStat();
	if (const auto FoundResult = Stat.EnumStat.Find(Tag))
	{
		return *FoundResult;
	}
	return 0;
}

void UNipcatStatFunctionLibrary::AddIntStat(TScriptInterface<INipcatStatInterface> StatOwner, FGameplayTag Tag,
	int32 AddValue)
{
	if (!Tag.IsValid() || !StatOwner.GetObject()) { return; }
	FNipcatStat& Stat = StatOwner->GetStat();
	const auto Value = GetIntStat(StatOwner, Tag);
	Stat.IntStat.Add(Tag, Value + AddValue);
}

void UNipcatStatFunctionLibrary::AddFloatStat(TScriptInterface<INipcatStatInterface> StatOwner, FGameplayTag Tag,
	float AddValue)
{
	if (!Tag.IsValid() || !StatOwner.GetObject()) { return; }
	FNipcatStat& Stat = StatOwner->GetStat();
	const auto Value = GetFloatStat(StatOwner, Tag);
	Stat.FloatStat.Add(Tag, Value + AddValue);
}
