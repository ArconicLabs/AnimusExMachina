// Copyright ArconicLabs. All Rights Reserved.

// Navigates the AI pawn to a target actor or location. Completes with
// Succeeded on arrival, Failed on path failure. Fully delegate-driven
// via OnRequestFinished — no Tick override.

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

	// Try actor first, fall back to location if unreachable
	EPathFollowingRequestResult::Type Result = EPathFollowingRequestResult::Failed;

	if (bHasTargetActor)
	{
		Result = InstanceData.Controller->MoveToActor(
			InstanceData.TargetActor,
			InstanceData.AcceptanceRadius);
	}

	if (Result == EPathFollowingRequestResult::Failed
		&& InstanceData.TargetLocation != FVector::ZeroVector)
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
		InstanceData.MoveFinishedHandle = PathComp->OnRequestFinished.AddLambda(
			[WeakContext = Context.MakeWeakExecutionContext()](
				FAIRequestID RequestID, const FPathFollowingResult& MoveResult)
			{
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
