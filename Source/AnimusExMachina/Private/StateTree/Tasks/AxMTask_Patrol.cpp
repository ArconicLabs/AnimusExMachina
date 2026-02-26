// Copyright ArconicLabs. All Rights Reserved.

#include "StateTree/Tasks/AxMTask_Patrol.h"
#include "AI/AxMAIController.h"
#include "AI/AxMPatrolRoute.h"
#include "AnimusExMachina.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeExecutionTypes.h"
#include "Navigation/PathFollowingComponent.h"
#include "TimerManager.h"

EStateTreeRunStatus FAxMTask_Patrol::EnterState(
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

	// Get patrol route from the AxM AI Controller
	const AAxMAIController* AxMController = Cast<AAxMAIController>(InstanceData.Controller);
	AAxMPatrolRoute* Route = AxMController ? AxMController->GetPatrolRoute() : nullptr;

	if (!Route || Route->GetNumPatrolPoints() < 2)
	{
		// No route or only one point — nothing to patrol
		return EStateTreeRunStatus::Succeeded;
	}

	InstanceData.CurrentPointIndex = 0;
	InstanceData.Direction = 1;

	// Bind move completion delegate to chain: arrive → wait → next point
	UPathFollowingComponent* PathComp = InstanceData.Controller->GetPathFollowingComponent();
	if (!PathComp)
	{
		return EStateTreeRunStatus::Failed;
	}

	TWeakObjectPtr<AAIController> WeakController = InstanceData.Controller;
	TWeakObjectPtr<AAxMPatrolRoute> WeakRoute = Route;
	float WaitDuration = InstanceData.WaitDuration;
	float AcceptanceRadius = InstanceData.AcceptanceRadius;
	bool bLoop = InstanceData.bLoop;

	// Shared mutable state for the delegate chain — allocated once, captured by lambda
	struct FPatrolState
	{
		int32 CurrentIndex = 0;
		int32 Direction = 1;
	};
	TSharedPtr<FPatrolState> PatrolState = MakeShared<FPatrolState>();

	InstanceData.MoveFinishedHandle = PathComp->OnRequestFinished.AddLambda(
		[WeakController, WeakRoute, WaitDuration, AcceptanceRadius, bLoop, PatrolState](
			FAIRequestID /*RequestID*/, const FPathFollowingResult& /*MoveResult*/)
		{
			if (!WeakController.IsValid() || !WeakRoute.IsValid())
			{
				return;
			}

			AAIController* Controller = WeakController.Get();
			AAxMPatrolRoute* Route = WeakRoute.Get();
			const int32 NumPoints = Route->GetNumPatrolPoints();
			if (NumPoints < 2)
			{
				return;
			}

			// Advance to next point
			if (bLoop)
			{
				PatrolState->CurrentIndex = (PatrolState->CurrentIndex + 1) % NumPoints;
			}
			else
			{
				// Ping-pong
				int32 NextIndex = PatrolState->CurrentIndex + PatrolState->Direction;
				if (NextIndex >= NumPoints || NextIndex < 0)
				{
					PatrolState->Direction *= -1;
					NextIndex = PatrolState->CurrentIndex + PatrolState->Direction;
				}
				PatrolState->CurrentIndex = NextIndex;
			}

			const FVector NextPoint = Route->GetPatrolPoint(PatrolState->CurrentIndex);

			// Wait at waypoint, then move
			UWorld* World = Controller->GetWorld();
			if (World && WaitDuration > 0.0f)
			{
				FTimerHandle WaitHandle;
				TWeakObjectPtr<AAIController> InnerWeakController = Controller;
				World->GetTimerManager().SetTimer(
					WaitHandle,
					FTimerDelegate::CreateLambda(
						[InnerWeakController, NextPoint, AcceptanceRadius]()
						{
							if (InnerWeakController.IsValid())
							{
								InnerWeakController->MoveToLocation(
									NextPoint, AcceptanceRadius);
							}
						}
					),
					WaitDuration,
					false
				);
			}
			else
			{
				Controller->MoveToLocation(NextPoint, AcceptanceRadius);
			}
		}
	);

	// Kick off the first move
	const FVector FirstPoint = Route->GetPatrolPoint(0);
	InstanceData.Controller->MoveToLocation(FirstPoint, AcceptanceRadius);

	UE_LOG(LogAxM, Verbose, TEXT("Patrol: starting route with %d points"),
		Route->GetNumPatrolPoints());

	return EStateTreeRunStatus::Running;
}

void FAxMTask_Patrol::ExitState(
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
		// Clear wait timer
		UWorld* World = InstanceData.Controller->GetWorld();
		if (World)
		{
			World->GetTimerManager().ClearTimer(InstanceData.WaitTimerHandle);
		}

		// Remove delegate before stopping movement
		UPathFollowingComponent* PathComp = InstanceData.Controller->GetPathFollowingComponent();
		if (PathComp && InstanceData.MoveFinishedHandle.IsValid())
		{
			PathComp->OnRequestFinished.Remove(InstanceData.MoveFinishedHandle);
			InstanceData.MoveFinishedHandle.Reset();
		}

		InstanceData.Controller->StopMovement();
	}
}

#if WITH_EDITOR
FText FAxMTask_Patrol::GetDescription(
	const FGuid& ID,
	FStateTreeDataView InstanceDataView,
	const IStateTreeBindingLookup& BindingLookup,
	EStateTreeNodeFormatting Formatting) const
{
	return FText::FromString(TEXT("<b>AxM Patrol</b>"));
}
#endif
