// Copyright ArconicLabs. All Rights Reserved.

#include "StateTree/Tasks/AxMTask_DebugState.h"
#include "AIController.h"
#include "StateTreeExecutionContext.h"
#include "DrawDebugHelpers.h"

EStateTreeRunStatus FAxMTask_DebugState::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FAxMTask_DebugState::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime) const
{
#if ENABLE_DRAW_DEBUG
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	if (!InstanceData.Controller)
	{
		return EStateTreeRunStatus::Running;
	}

	APawn* Pawn = InstanceData.Controller->GetPawn();
	if (!Pawn)
	{
		return EStateTreeRunStatus::Running;
	}

	const FVector Location = Pawn->GetActorLocation()
		+ FVector(0.0f, 0.0f, InstanceData.HeightOffset);

	DrawDebugString(
		Pawn->GetWorld(),
		Location,
		InstanceData.StateLabel,
		nullptr,
		InstanceData.LabelColor,
		0.0f, // duration — 0 means one frame
		true  // shadow
	);
#endif

	return EStateTreeRunStatus::Running;
}

#if WITH_EDITOR
FText FAxMTask_DebugState::GetDescription(
	const FGuid& ID,
	FStateTreeDataView InstanceDataView,
	const IStateTreeBindingLookup& BindingLookup,
	EStateTreeNodeFormatting Formatting) const
{
	const FAxMTask_DebugStateInstanceData* Data = InstanceDataView.GetPtr<FAxMTask_DebugStateInstanceData>();
	if (Data)
	{
		return FText::FromString(FString::Printf(TEXT("<b>Debug: %s</b>"), *Data->StateLabel));
	}
	return FText::FromString(TEXT("<b>AxM Debug State</b>"));
}
#endif
