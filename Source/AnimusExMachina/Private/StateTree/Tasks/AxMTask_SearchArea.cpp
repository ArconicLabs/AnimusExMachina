// Copyright ArconicLabs. All Rights Reserved.

#include "StateTree/Tasks/AxMTask_SearchArea.h"
#include "AnimusExMachina.h"
#include "AIController.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeAsyncExecutionContext.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/EnvQuery.h"
#include "TimerManager.h"

EStateTreeRunStatus FAxMTask_SearchArea::EnterState(
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

	// start the search duration timer
	UWorld* World = InstanceData.Controller->GetWorld();
	if (World)
	{
		World->GetTimerManager().SetTimer(
			InstanceData.SearchTimerHandle,
			FTimerDelegate::CreateLambda([WeakContext = Context.MakeWeakExecutionContext()]()
			{
				WeakContext.FinishTask(EStateTreeFinishTaskType::Succeeded);
			}),
			InstanceData.SearchDuration,
			false
		);
	}

	// bind move completion delegate to chain search points
	UPathFollowingComponent* PathComp = InstanceData.Controller->GetPathFollowingComponent();
	if (PathComp)
	{
		TWeakObjectPtr<AAIController> WeakController = InstanceData.Controller;
		TWeakObjectPtr<UEnvQuery> WeakQueryAsset = InstanceData.QueryAsset;
		FVector SearchCenter = InstanceData.SearchCenter;
		float SearchRadius = InstanceData.SearchRadius;
		float AcceptanceRadius = InstanceData.AcceptanceRadius;

		InstanceData.MoveFinishedHandle = PathComp->OnRequestFinished.AddLambda(
			[WeakController, WeakQueryAsset, SearchCenter, SearchRadius, AcceptanceRadius](
				FAIRequestID RequestID, const FPathFollowingResult& MoveResult)
			{
				if (!WeakController.IsValid())
				{
					return;
				}

				AAIController* Controller = WeakController.Get();

				// EQS path: run query, move to result in callback
				if (WeakQueryAsset.IsValid())
				{
					APawn* Pawn = Controller->GetPawn();
					if (!Pawn)
					{
						return;
					}

					UEnvQueryManager* EQSManager = UEnvQueryManager::GetCurrent(Pawn->GetWorld());
					if (!EQSManager)
					{
						return;
					}

					FEnvQueryRequest Request(WeakQueryAsset.Get(), Pawn);
					TWeakObjectPtr<AAIController> InnerWeakController = Controller;

					EQSManager->RunQuery(Request,
						EEnvQueryRunMode::SingleResult,
						FQueryFinishedSignature::CreateLambda(
							[InnerWeakController, AcceptanceRadius](TSharedPtr<FEnvQueryResult> Result)
							{
								if (!InnerWeakController.IsValid() || !Result.IsValid() || !Result->IsSuccessful())
								{
									return;
								}

								FVector BestLocation = Result->GetItemAsLocation(0);
								InnerWeakController->MoveToLocation(BestLocation, AcceptanceRadius);
							}
						)
					);
					return;
				}

				// random NavMesh path: pick a random reachable point
				APawn* Pawn = Controller->GetPawn();
				if (!Pawn)
				{
					return;
				}

				UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(Pawn->GetWorld());
				if (!NavSys)
				{
					return;
				}

				FNavLocation ResultLocation;
				if (NavSys->GetRandomReachablePointInRadius(SearchCenter, SearchRadius, ResultLocation))
				{
					Controller->MoveToLocation(ResultLocation.Location, AcceptanceRadius);
				}
			}
		);
	}

	// kick off the first search point
	if (InstanceData.QueryAsset)
	{
		APawn* Pawn = InstanceData.Controller->GetPawn();
		if (Pawn)
		{
			UEnvQueryManager* EQSManager = UEnvQueryManager::GetCurrent(Pawn->GetWorld());
			if (EQSManager)
			{
				TWeakObjectPtr<AAIController> WeakController = InstanceData.Controller;
				float AcceptanceRadius = InstanceData.AcceptanceRadius;

				FEnvQueryRequest Request(InstanceData.QueryAsset, Pawn);
				EQSManager->RunQuery(Request,
					EEnvQueryRunMode::SingleResult,
					FQueryFinishedSignature::CreateLambda(
						[WeakController, AcceptanceRadius](TSharedPtr<FEnvQueryResult> Result)
						{
							if (!WeakController.IsValid() || !Result.IsValid() || !Result->IsSuccessful())
							{
								return;
							}

							FVector BestLocation = Result->GetItemAsLocation(0);
							WeakController->MoveToLocation(BestLocation, AcceptanceRadius);
						}
					)
				);
			}
		}
	}
	else
	{
		APawn* Pawn = InstanceData.Controller->GetPawn();
		if (Pawn)
		{
			UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(Pawn->GetWorld());
			if (NavSys)
			{
				FNavLocation ResultLocation;
				if (NavSys->GetRandomReachablePointInRadius(
					InstanceData.SearchCenter, InstanceData.SearchRadius, ResultLocation))
				{
					InstanceData.Controller->MoveToLocation(
						ResultLocation.Location, InstanceData.AcceptanceRadius);
				}
			}
		}
	}

	return EStateTreeRunStatus::Running;
}

void FAxMTask_SearchArea::ExitState(
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
		// clear the search duration timer
		UWorld* World = InstanceData.Controller->GetWorld();
		if (World)
		{
			World->GetTimerManager().ClearTimer(InstanceData.SearchTimerHandle);
		}

		// remove move completion delegate
		UPathFollowingComponent* PathComp = InstanceData.Controller->GetPathFollowingComponent();
		if (PathComp && InstanceData.MoveFinishedHandle.IsValid())
		{
			PathComp->OnRequestFinished.Remove(InstanceData.MoveFinishedHandle);
			InstanceData.MoveFinishedHandle.Reset();
		}

		// stop any in-progress movement
		InstanceData.Controller->StopMovement();
	}
}

#if WITH_EDITOR
FText FAxMTask_SearchArea::GetDescription(
	const FGuid& ID,
	FStateTreeDataView InstanceDataView,
	const IStateTreeBindingLookup& BindingLookup,
	EStateTreeNodeFormatting Formatting) const
{
	return FText::FromString(TEXT("<b>AxM Search Area</b>"));
}
#endif
