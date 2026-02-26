// Copyright ArconicLabs. All Rights Reserved.

#include "StateTree/Tasks/AxMTask_LookAround.h"
#include "AIController.h"
#include "StateTreeExecutionContext.h"

EStateTreeRunStatus FAxMTask_LookAround::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	if (Transition.ChangeType != EStateTreeStateChangeType::Changed)
	{
		return EStateTreeRunStatus::Running;
	}

	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	if (!InstanceData.Controller)
	{
		return EStateTreeRunStatus::Failed;
	}

	InstanceData.ElapsedTime = 0.0f;
	InstanceData.LookPhase = 0;

	// compute focal points relative to pawn's current position and orientation
	APawn* Pawn = InstanceData.Controller->GetPawn();
	if (!Pawn)
	{
		return EStateTreeRunStatus::Failed;
	}

	const FVector Origin = Pawn->GetActorLocation();
	const FVector Forward = Pawn->GetActorForwardVector();
	const FVector Right = Pawn->GetActorRightVector();
	constexpr float LookDistance = 500.0f;

	InstanceData.FocalPoints[0] = Origin + (-Right * LookDistance); // left
	InstanceData.FocalPoints[1] = Origin + (Right * LookDistance);  // right
	InstanceData.FocalPoints[2] = Origin + (Forward * LookDistance); // forward

	// start looking left
	InstanceData.Controller->SetFocalPoint(InstanceData.FocalPoints[0]);

	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FAxMTask_LookAround::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	InstanceData.ElapsedTime += DeltaTime;

	if (InstanceData.ElapsedTime >= InstanceData.LookDuration)
	{
		return EStateTreeRunStatus::Succeeded;
	}

	// step through focal points at equal intervals
	const float PhaseLength = InstanceData.LookDuration / 3.0f;
	const int32 NewPhase = FMath::Clamp(
		static_cast<int32>(InstanceData.ElapsedTime / PhaseLength), 0, 2);

	if (NewPhase != InstanceData.LookPhase)
	{
		InstanceData.LookPhase = NewPhase;
		if (InstanceData.Controller)
		{
			InstanceData.Controller->SetFocalPoint(
				InstanceData.FocalPoints[NewPhase]);
		}
	}

	return EStateTreeRunStatus::Running;
}

void FAxMTask_LookAround::ExitState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	if (Transition.ChangeType != EStateTreeStateChangeType::Changed)
	{
		return;
	}

	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	if (InstanceData.Controller)
	{
		InstanceData.Controller->ClearFocus(EAIFocusPriority::Gameplay);
	}
}

#if WITH_EDITOR
FText FAxMTask_LookAround::GetDescription(
	const FGuid& ID,
	FStateTreeDataView InstanceDataView,
	const IStateTreeBindingLookup& BindingLookup,
	EStateTreeNodeFormatting Formatting) const
{
	return FText::FromString(TEXT("<b>AxM Look Around</b>"));
}
#endif
