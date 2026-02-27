// Copyright ArconicLabs. All Rights Reserved.

#include "StateTree/Tasks/AxMGlobalTask_TargetTracking.h"
#include "AIController.h"
#include "StateTreeExecutionContext.h"

EStateTreeRunStatus FAxMGlobalTask_TargetTracking::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	if (!InstanceData.Controller || !IsValid(InstanceData.TargetActor))
	{
		InstanceData.DistanceToTarget = 0.0f;
		InstanceData.HasLineOfSight = false;
		return EStateTreeRunStatus::Running;
	}

	const APawn* Pawn = InstanceData.Controller->GetPawn();
	if (!Pawn)
	{
		return EStateTreeRunStatus::Running;
	}

	InstanceData.DistanceToTarget = FVector::Distance(
		Pawn->GetActorLocation(),
		InstanceData.TargetActor->GetActorLocation());

	InstanceData.HasLineOfSight = InstanceData.Controller->LineOfSightTo(
		InstanceData.TargetActor);

	return EStateTreeRunStatus::Running;
}

#if WITH_EDITOR
FText FAxMGlobalTask_TargetTracking::GetDescription(
	const FGuid& ID,
	FStateTreeDataView InstanceDataView,
	const IStateTreeBindingLookup& BindingLookup,
	EStateTreeNodeFormatting Formatting) const
{
	return FText::FromString(TEXT("<b>AxM Target</b>"));
}
#endif
