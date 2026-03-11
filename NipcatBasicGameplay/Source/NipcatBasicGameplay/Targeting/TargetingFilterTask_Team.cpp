// Copyright Nipcat Studio All Rights Reserved.


#include "TargetingFilterTask_Team.h"

#include "NipcatBasicGameplay/Team/NipcatTeamUtils.h"

bool UTargetingFilterTask_Team::ShouldFilterTarget(const FTargetingRequestHandle& TargetingHandle,
                                                   const FTargetingDefaultResultData& TargetData) const
{
	if (const FTargetingSourceContext* SourceContext = FTargetingSourceContext::Find(TargetingHandle))
	{
		if (const auto Instigator = SourceContext->InstigatorActor)
		{
			if (AActor* TargetActor = TargetData.HitResult.GetActor())
			{
				switch (UNipcatTeamUtils::GetTeamAttitudeTowards(Instigator, TargetActor))
				{
				case ETeamAttitude::Type::Friendly:
					{
						return bDetectFriendlies ? false : true;
					}
				case ETeamAttitude::Type::Neutral:
					{
						return bDetectNeutrals ? false : true;
					}
				case ETeamAttitude::Type::Hostile:
					{
						return bDetectEnemies ? false : true;
					}
				}
			}
		}
	}
	return Super::ShouldFilterTarget(TargetingHandle, TargetData);
}
