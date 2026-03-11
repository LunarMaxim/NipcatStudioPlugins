// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatAbilitySystemTypes.h"

bool FNipcatNameRequirements::RequirementsMet(const FName& Name) const
{
	const bool bHasAnyRequired = RequireNames_Any.IsEmpty() || RequireNames_Any.Contains(Name);
	const bool bHasAllRequired = RequireNames_All.Num() <= 1 && (RequireNames_All.IsEmpty() || RequireNames_All.Contains(Name));
	const bool bHasIgnored = IgnoreNames.IsEmpty() ? false : IgnoreNames.Contains(Name);

	return bHasAnyRequired && bHasAllRequired && !bHasIgnored;
}

bool FNipcatNameRequirements::RequirementsMet(const TArray<FName>& Names) const
{
	bool bHasAnyRequired = RequireNames_Any.IsEmpty();
	if (!bHasAnyRequired)
	{
		for (const FName& Name : Names)
		{
			if (RequireNames_Any.Contains(Name))
			{
				bHasAnyRequired = true;
				break;
			}
		}
	}
	
	bool bHasAllRequired = true;
	if (!RequireNames_All.IsEmpty())
	{
		for (const FName& Name : RequireNames_All)
		{
			if (!Names.Contains(Name))
			{
				bHasAllRequired = false;
				break;
			}
		}
	}
	
	bool bHasIgnored = false;
	if (!IgnoreNames.IsEmpty())
	{
		for (const FName& Name : IgnoreNames)
		{
			if (Names.Contains(Name))
			{
				bHasIgnored = true;
				break;
			}
		}
	}
	
	return bHasAnyRequired && bHasAllRequired && !bHasIgnored;
}

bool FNipcatNameRequirements::IsEmpty() const
{
	return RequireNames_All.IsEmpty() && RequireNames_Any.IsEmpty() && IgnoreNames.IsEmpty();
}

bool FNipcatNameRequirements::operator==(const FNipcatNameRequirements& Other) const
{
	return RequireNames_All == Other.RequireNames_All && RequireNames_Any == Other.RequireNames_Any && IgnoreNames == Other.IgnoreNames;
}

bool FNipcatNameRequirements::operator!=(const FNipcatNameRequirements& Other) const
{
	return !(*this == Other);
}
