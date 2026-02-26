// Copyright ArconicLabs. All Rights Reserved.

#include "StateTree/Tasks/AxMGlobalTask_Perception.h"
#include "AI/AxMAIController.h"
#include "StateTreeExecutionContext.h"

EStateTreeRunStatus FAxMGlobalTask_Perception::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	// record the NPC's spawn location as home
	if (InstanceData.Controller)
	{
		if (APawn* Pawn = InstanceData.Controller->GetPawn())
		{
			InstanceData.HomeLocation = Pawn->GetActorLocation();
		}
	}

	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FAxMGlobalTask_Perception::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	AAxMAIController* AxMController = Cast<AAxMAIController>(InstanceData.Controller);
	if (!AxMController)
	{
		return EStateTreeRunStatus::Running;
	}

	// --- Sight (always read) ---
	InstanceData.TargetActor = AxMController->GetCachedTargetActor();
	InstanceData.LastKnownLocation = AxMController->GetCachedLastKnownLocation();

	// --- Hearing (consume pending event, reset strength when idle) ---
	if (AxMController->HasPendingHearingEvent())
	{
		float Strength;
		FVector Location;
		AxMController->ConsumeHearingEvent(Strength, Location);
		InstanceData.StimulusLocation = Location;
		InstanceData.HearingStrength = Strength;
	}
	else
	{
		InstanceData.HearingStrength = 0.0f;
	}

	// --- Damage (known instigator → immediate target, unknown → investigate) ---
	if (AxMController->HasPendingDamageEvent())
	{
		AActor* Instigator = nullptr;
		FVector Location;
		AxMController->ConsumeDamageEvent(Instigator, Location);

		if (IsValid(Instigator))
		{
			InstanceData.TargetActor = Instigator;
			InstanceData.LastKnownLocation = Instigator->GetActorLocation();
		}
		else
		{
			InstanceData.StimulusLocation = Location;
			InstanceData.HearingStrength = 1.0f;
		}
	}

	return EStateTreeRunStatus::Running;
}

#if WITH_EDITOR
FText FAxMGlobalTask_Perception::GetDescription(
	const FGuid& ID,
	FStateTreeDataView InstanceDataView,
	const IStateTreeBindingLookup& BindingLookup,
	EStateTreeNodeFormatting Formatting) const
{
	return FText::FromString(TEXT("<b>AxM Perception</b>"));
}
#endif
