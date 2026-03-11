// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatAbilitySystemCharacter.h"


// Sets default values
ANipcatAbilitySystemCharacter::ANipcatAbilitySystemCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilitySystemComponent = CreateDefaultSubobject<UNipcatAbilitySystemComponent>(TEXT("AbilitySystem"));
}

void ANipcatAbilitySystemCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (Controller)
	{
		// 虽然ASC的InitializeComponent里有Init, 但ActorInfo没有Controller信息
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}
}

void ANipcatAbilitySystemCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (NewController)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}
}

UNipcatAbilitySystemComponent* ANipcatAbilitySystemCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
