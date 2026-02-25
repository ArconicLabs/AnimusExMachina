// Copyright ArconicLabs. All Rights Reserved.

#include "StateTree/Tasks/AxMTask_MoveTo.h"
#include "AIController.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeExecutionTypes.h"
#include "StateTreeAsyncExecutionContext.h"
#include "Navigation/PathFollowingComponent.h"

EStateTreeRunStatus FAxMTask_MoveTo::EnterState(
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

	// issue the move request
	EPathFollowingRequestResult::Type Result;

	if (IsValid(InstanceData.TargetActor))
	{
		Result = InstanceData.Controller->MoveToActor(
			InstanceData.TargetActor,
			InstanceData.AcceptanceRadius);
	}
	else
	{
		Result = InstanceData.Controller->MoveToLocation(
			InstanceData.TargetLocation,
			InstanceData.AcceptanceRadius);
	}

	if (Result == EPathFollowingRequestResult::AlreadyAtGoal)
	{
		return EStateTreeRunStatus::Succeeded;
	}

	if (Result == EPathFollowingRequestResult::Failed)
	{
		return EStateTreeRunStatus::Failed;
	}

	// bind to the path following component's non-dynamic delegate for async completion
	UPathFollowingComponent* PathComp = InstanceData.Controller->GetPathFollowingComponent();
	if (PathComp)
	{
		PathComp->OnRequestFinished.AddLambda(
			[WeakContext = Context.MakeWeakExecutionContext()](
				FAIRequestID RequestID, const FPathFollowingResult& MoveResult)
			{
				if (MoveResult.IsSuccess())
				{
					WeakContext.FinishTask(EStateTreeFinishTaskType::Succeeded);
				}
				else
				{
					WeakContext.FinishTask(EStateTreeFinishTaskType::Failed);
				}
			}
		);
	}

	return EStateTreeRunStatus::Running;
}

void FAxMTask_MoveTo::ExitState(
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
		InstanceData.Controller->StopMovement();

		// clean up the delegate binding
		UPathFollowingComponent* PathComp = InstanceData.Controller->GetPathFollowingComponent();
		if (PathComp)
		{
			PathComp->OnRequestFinished.RemoveAll(this);
		}
	}
}

#if WITH_EDITOR
FText FAxMTask_MoveTo::GetDescription(
	const FGuid& ID,
	FStateTreeDataView InstanceDataView,
	const IStateTreeBindingLookup& BindingLookup,
	EStateTreeNodeFormatting Formatting) const
{
	return FText::FromString(TEXT("<b>AxM Move To</b>"));
}
#endif
