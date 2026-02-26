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

	const bool bHasTargetActor = IsValid(InstanceData.TargetActor);

	// issue the move request
	EPathFollowingRequestResult::Type Result;

	if (bHasTargetActor)
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
		// following a moving actor — stay alive for the tick transition to handle
		if (bHasTargetActor)
		{
			return EStateTreeRunStatus::Running;
		}
		return EStateTreeRunStatus::Succeeded;
	}

	if (Result == EPathFollowingRequestResult::Failed)
	{
		return EStateTreeRunStatus::Failed;
	}

	// bind to the path following component's delegate for async completion
	UPathFollowingComponent* PathComp = InstanceData.Controller->GetPathFollowingComponent();
	if (PathComp)
	{
		TWeakObjectPtr<AAIController> WeakController = InstanceData.Controller;
		TWeakObjectPtr<AActor> WeakTarget = InstanceData.TargetActor;
		float AcceptanceRadius = InstanceData.AcceptanceRadius;

		InstanceData.MoveFinishedHandle = PathComp->OnRequestFinished.AddLambda(
			[WeakContext = Context.MakeWeakExecutionContext(), WeakController, WeakTarget, AcceptanceRadius](
				FAIRequestID RequestID, const FPathFollowingResult& MoveResult)
			{
				if (!MoveResult.IsSuccess())
				{
					WeakContext.FinishTask(EStateTreeFinishTaskType::Failed);
					return;
				}

				// continuous follow: target is still valid, re-issue MoveToActor
				if (WeakTarget.IsValid() && WeakController.IsValid())
				{
					WeakController->MoveToActor(WeakTarget.Get(), AcceptanceRadius);
					return;
				}

				// one-shot: reached a fixed location
				WeakContext.FinishTask(EStateTreeFinishTaskType::Succeeded);
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

	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	if (InstanceData.Controller)
	{
		InstanceData.Controller->StopMovement();

		// clean up the delegate binding using the stored handle
		UPathFollowingComponent* PathComp = InstanceData.Controller->GetPathFollowingComponent();
		if (PathComp && InstanceData.MoveFinishedHandle.IsValid())
		{
			PathComp->OnRequestFinished.Remove(InstanceData.MoveFinishedHandle);
			InstanceData.MoveFinishedHandle.Reset();
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
