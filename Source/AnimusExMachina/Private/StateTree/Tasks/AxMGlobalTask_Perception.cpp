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

	if (!InstanceData.Controller)
	{
		return EStateTreeRunStatus::Running;
	}

	// read cached perception data from the controller's delegate results
	InstanceData.TargetActor = InstanceData.Controller->GetCachedTargetActor();
	InstanceData.LastKnownLocation = InstanceData.Controller->GetCachedLastKnownLocation();

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
