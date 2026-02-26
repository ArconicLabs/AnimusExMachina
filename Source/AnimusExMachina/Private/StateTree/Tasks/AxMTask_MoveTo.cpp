// Copyright ArconicLabs. All Rights Reserved.

// Navigates the AI pawn to a target actor (continuous follow) or location
// (one-shot). Completion is fully delegate-driven via OnRequestFinished —
// no Tick override. See header comment for mode details.

#include "StateTree/Tasks/AxMTask_MoveTo.h"
#include "AnimusExMachina.h"
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
		UE_LOG(LogAxM, Warning, TEXT("MoveTo: no controller bound"));
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
		UE_LOG(LogAxM, Log, TEXT("MoveTo: AlreadyAtGoal — bHasTargetActor=%d"),
			bHasTargetActor);
		// continuous follow: at target, state transitions handle next step
		// one-shot: arrived, complete immediately
		return bHasTargetActor
			? EStateTreeRunStatus::Running
			: EStateTreeRunStatus::Succeeded;
	}

	if (Result == EPathFollowingRequestResult::Failed)
	{
		UE_LOG(LogAxM, Warning, TEXT("MoveTo: path request failed — TargetActor=%s, Location=%s, Radius=%.1f"),
			bHasTargetActor ? *GetNameSafe(InstanceData.TargetActor) : TEXT("null"),
			*InstanceData.TargetLocation.ToString(),
			InstanceData.AcceptanceRadius);
		return EStateTreeRunStatus::Failed;
	}

	// RequestSuccessful — bind delegate for async completion
	UPathFollowingComponent* PathComp =
		InstanceData.Controller->GetPathFollowingComponent();

	if (PathComp)
	{
		TWeakObjectPtr<AAIController> WeakController = InstanceData.Controller;
		TWeakObjectPtr<AActor> WeakTarget = InstanceData.TargetActor;
		float Radius = InstanceData.AcceptanceRadius;

		InstanceData.MoveFinishedHandle = PathComp->OnRequestFinished.AddLambda(
			[WeakContext = Context.MakeWeakExecutionContext(),
			 WeakController, WeakTarget, Radius](
				FAIRequestID RequestID, const FPathFollowingResult& MoveResult)
			{
				if (WeakTarget.IsValid() && WeakController.IsValid())
				{
					// Continuous follow — re-issue only on non-success
					// (aborted path, blocked, etc.). On success the NPC
					// arrived; state transitions handle the next step.
					//
					// Re-entrancy safe: if re-issue triggers AlreadyAtGoal,
					// UE fires this delegate synchronously with Success,
					// which we don't re-issue on.
					if (!MoveResult.IsSuccess())
					{
						WeakController->MoveToActor(
							WeakTarget.Get(), Radius);
					}
					return;
				}

				// One-shot — complete the task
				if (MoveResult.IsSuccess())
				{
					WeakContext.FinishTask(
						EStateTreeFinishTaskType::Succeeded);
				}
				else
				{
					WeakContext.FinishTask(
						EStateTreeFinishTaskType::Failed);
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

	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	if (InstanceData.Controller)
	{
		// remove delegate before StopMovement — StopMovement aborts the
		// active request which fires OnRequestFinished synchronously
		UPathFollowingComponent* PathComp =
			InstanceData.Controller->GetPathFollowingComponent();
		if (PathComp && InstanceData.MoveFinishedHandle.IsValid())
		{
			PathComp->OnRequestFinished.Remove(
				InstanceData.MoveFinishedHandle);
			InstanceData.MoveFinishedHandle.Reset();
		}

		InstanceData.Controller->StopMovement();
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
