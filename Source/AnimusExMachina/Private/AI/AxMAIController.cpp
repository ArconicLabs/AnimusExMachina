// Copyright ArconicLabs. All Rights Reserved.

#include "AI/AxMAIController.h"
#include "AnimusExMachina.h"
#include "Config/AxMConfig.h"
#include "Components/StateTreeAIComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Damage.h"

AAxMAIController::AAxMAIController()
{
	// StateTree component
	StateTreeAI = CreateDefaultSubobject<UStateTreeAIComponent>(TEXT("StateTreeAI"));
	check(StateTreeAI);

	bStartAILogicOnPossess = true;
	bAttachToPawn = true;

	// Perception component
	PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComp"));
	SetPerceptionComponent(*PerceptionComp);

	// Create sense configs as subobjects (configured in PostInitializeComponents
	// so Blueprint-overridden parameter values are applied)
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
	DamageConfig = CreateDefaultSubobject<UAISenseConfig_Damage>(TEXT("DamageConfig"));
}

void AAxMAIController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (!PerceptionComp)
	{
		return;
	}

	// Resolve sense parameters — Config Data Asset takes priority over loose properties
	const bool bUseSight = Config ? Config->bEnableSight : bEnableSight;
	const bool bUseHearing = Config ? Config->bEnableHearing : bEnableHearing;
	const bool bUseDamage = Config ? Config->bEnableDamage : bEnableDamage;

	if (bUseSight && SightConfig)
	{
		SightConfig->SightRadius = Config ? Config->SightRadius : SightRadius;
		SightConfig->LoseSightRadius = Config ? Config->LoseSightRadius : LoseSightRadius;
		SightConfig->PeripheralVisionAngleDegrees = Config ? Config->PeripheralVisionAngle : PeripheralVisionAngle;
		SightConfig->DetectionByAffiliation.bDetectEnemies = true;
		SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
		SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
		SightConfig->SetMaxAge(Config ? Config->SightMaxAge : SightMaxAge);

		PerceptionComp->ConfigureSense(*SightConfig);
		PerceptionComp->SetDominantSense(UAISenseConfig_Sight::StaticClass());
	}

	if (bUseHearing && HearingConfig)
	{
		HearingConfig->HearingRange = Config ? Config->HearingRange : HearingRange;
		HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
		HearingConfig->DetectionByAffiliation.bDetectFriendlies = false;
		HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
		HearingConfig->SetMaxAge(Config ? Config->HearingMaxAge : HearingMaxAge);

		PerceptionComp->ConfigureSense(*HearingConfig);
	}

	if (bUseDamage && DamageConfig)
	{
		PerceptionComp->ConfigureSense(*DamageConfig);
	}
}

void AAxMAIController::BeginPlay()
{
	Super::BeginPlay();

	if (PerceptionComp)
	{
		PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(
			this, &AAxMAIController::OnTargetPerceptionUpdated);
	}
}

void AAxMAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!Actor)
	{
		return;
	}

	// Dispatch by sense type
	if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
	{
		if (Stimulus.WasSuccessfullySensed())
		{
			const bool bWasNull = !IsValid(CachedTargetActor);
			CachedTargetActor = Actor;
			CachedLastKnownLocation = Actor->GetActorLocation();

			if (bWasNull)
			{
				OnTargetAcquired.Broadcast(Actor);
			}
		}
		else
		{
			if (CachedTargetActor == Actor)
			{
				CachedLastKnownLocation = Actor->GetActorLocation();
				CachedTargetActor = nullptr;
				OnTargetLost.Broadcast();
			}
		}
	}
	else if (Stimulus.Type == UAISense::GetSenseID<UAISense_Hearing>())
	{
		// keep the strongest hearing event if multiple arrive between ticks
		if (!bHearingEventPending || Stimulus.Strength > CachedHearingStrength)
		{
			CachedStimulusLocation = Stimulus.StimulusLocation;
			CachedHearingStrength = Stimulus.Strength;
		}
		bHearingEventPending = true;
	}
	else if (Stimulus.Type == UAISense::GetSenseID<UAISense_Damage>())
	{
		CachedDamageInstigator = Actor;
		CachedDamageLocation = Stimulus.StimulusLocation;
		bDamageEventPending = true;
	}
}

void AAxMAIController::SetAlertState(const FGameplayTag& NewState)
{
	if (CurrentAlertState == NewState)
	{
		return;
	}

	const FGameplayTag OldState = CurrentAlertState;
	CurrentAlertState = NewState;

	ApplyMovementSpeedForState(NewState);

	OnAlertStateChanged.Broadcast(OldState, NewState);
}

void AAxMAIController::ApplyMovementSpeedForState(const FGameplayTag& StateTag)
{
	if (!Config || !StateTag.IsValid())
	{
		return;
	}

	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn)
	{
		return;
	}

	ACharacter* PawnAsCharacter = Cast<ACharacter>(ControlledPawn);
	if (!PawnAsCharacter || !PawnAsCharacter->GetCharacterMovement())
	{
		return;
	}

	float Speed = 0.0f;
	if (StateTag == Config->PatrolTag)
	{
		Speed = Config->PatrolSpeed;
	}
	else if (StateTag == Config->InvestigateTag)
	{
		Speed = Config->InvestigateSpeed;
	}
	else if (StateTag == Config->CombatTag)
	{
		Speed = Config->CombatSpeed;
	}
	else if (StateTag == Config->SearchTag)
	{
		Speed = Config->SearchSpeed;
	}

	if (Speed > 0.0f)
	{
		PawnAsCharacter->GetCharacterMovement()->MaxWalkSpeed = Speed;
	}
}

void AAxMAIController::ConsumeHearingEvent(float& OutStrength, FVector& OutLocation)
{
	OutStrength = CachedHearingStrength;
	OutLocation = CachedStimulusLocation;
	bHearingEventPending = false;
	CachedHearingStrength = 0.0f;
}

void AAxMAIController::ConsumeDamageEvent(AActor*& OutInstigator, FVector& OutLocation)
{
	OutInstigator = CachedDamageInstigator;
	OutLocation = CachedDamageLocation;
	bDamageEventPending = false;
	CachedDamageInstigator = nullptr;
}
