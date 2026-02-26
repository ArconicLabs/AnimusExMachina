// Copyright ArconicLabs. All Rights Reserved.

#include "AI/AxMAIController.h"
#include "AnimusExMachina.h"
#include "Components/StateTreeAIComponent.h"
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

	// Configure only the enabled senses using the (potentially Blueprint-overridden) parameters
	if (bEnableSight && SightConfig)
	{
		SightConfig->SightRadius = SightRadius;
		SightConfig->LoseSightRadius = LoseSightRadius;
		SightConfig->PeripheralVisionAngleDegrees = PeripheralVisionAngle;
		SightConfig->DetectionByAffiliation.bDetectEnemies = true;
		SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
		SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
		SightConfig->SetMaxAge(SightMaxAge);

		PerceptionComp->ConfigureSense(*SightConfig);
		PerceptionComp->SetDominantSense(UAISenseConfig_Sight::StaticClass());
	}

	if (bEnableHearing && HearingConfig)
	{
		HearingConfig->HearingRange = HearingRange;
		HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
		HearingConfig->DetectionByAffiliation.bDetectFriendlies = false;
		HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
		HearingConfig->SetMaxAge(HearingMaxAge);

		PerceptionComp->ConfigureSense(*HearingConfig);
	}

	if (bEnableDamage && DamageConfig)
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
			CachedTargetActor = Actor;
			CachedLastKnownLocation = Actor->GetActorLocation();
		}
		else
		{
			if (CachedTargetActor == Actor)
			{
				CachedLastKnownLocation = Actor->GetActorLocation();
				CachedTargetActor = nullptr;
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
