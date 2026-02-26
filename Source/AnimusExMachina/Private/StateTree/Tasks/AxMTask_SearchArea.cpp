// Copyright ArconicLabs. All Rights Reserved.

#include "StateTree/Tasks/AxMTask_SearchArea.h"
#include "AnimusExMachina.h"
#include "AIController.h"
#include "StateTreeExecutionContext.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/EnvQuery.h"

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

	InstanceData.ElapsedTime = 0.0f;
	InstanceData.bIsMoving = false;

	// kick off the first search point
	if (InstanceData.QueryAsset)
	{
		RunEQSQuery(InstanceData, Context);
	}
	else
	{
		MoveToRandomPoint(InstanceData, Context);
	}

	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FAxMTask_SearchArea::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	InstanceData.ElapsedTime += DeltaTime;

	if (InstanceData.ElapsedTime >= InstanceData.SearchDuration)
	{
		return EStateTreeRunStatus::Succeeded;
	}

	// when the current move finishes, pick the next search point
	if (InstanceData.bIsMoving &&
		InstanceData.Controller->GetMoveStatus() == EPathFollowingStatus::Idle)
	{
		InstanceData.bIsMoving = false;

		if (InstanceData.QueryAsset)
		{
			RunEQSQuery(InstanceData, Context);
		}
		else
		{
			MoveToRandomPoint(InstanceData, Context);
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

	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	if (InstanceData.Controller)
	{
		InstanceData.Controller->StopMovement();
	}
}

bool FAxMTask_SearchArea::MoveToRandomPoint(
	FInstanceDataType& InstanceData,
	FStateTreeExecutionContext& Context) const
{
	APawn* Pawn = InstanceData.Controller->GetPawn();
	if (!Pawn)
	{
		return false;
	}

	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(Pawn->GetWorld());
	if (!NavSys)
	{
		return false;
	}

	FNavLocation ResultLocation;
	if (!NavSys->GetRandomReachablePointInRadius(
		InstanceData.SearchCenter, InstanceData.SearchRadius, ResultLocation))
	{
		return false;
	}

	EPathFollowingRequestResult::Type MoveResult =
		InstanceData.Controller->MoveToLocation(
			ResultLocation.Location, InstanceData.AcceptanceRadius);

	if (MoveResult == EPathFollowingRequestResult::Failed)
	{
		return false;
	}

	InstanceData.bIsMoving =
		(MoveResult == EPathFollowingRequestResult::RequestSuccessful);

	return true;
}

void FAxMTask_SearchArea::RunEQSQuery(
	FInstanceDataType& InstanceData,
	FStateTreeExecutionContext& Context) const
{
	APawn* Pawn = InstanceData.Controller->GetPawn();
	if (!Pawn || !InstanceData.QueryAsset)
	{
		MoveToRandomPoint(InstanceData, Context);
		return;
	}

	UEnvQueryManager* EQSManager = UEnvQueryManager::GetCurrent(Pawn->GetWorld());
	if (!EQSManager)
	{
		MoveToRandomPoint(InstanceData, Context);
		return;
	}

	// capture raw controller pointer for the async callback — safe because
	// ExitState stops movement and the query is non-owning
	AAIController* Controller = InstanceData.Controller;
	float Radius = InstanceData.AcceptanceRadius;

	FEnvQueryRequest Request(InstanceData.QueryAsset, Pawn);

	EQSManager->RunQuery(Request,
		EEnvQueryRunMode::SingleResult,
		FQueryFinishedSignature::CreateLambda(
			[Controller, Radius](TSharedPtr<FEnvQueryResult> Result)
			{
				if (!IsValid(Controller) || !Result.IsValid() || !Result->IsSuccessful())
				{
					return;
				}

				FVector BestLocation = Result->GetItemAsLocation(0);
				Controller->MoveToLocation(BestLocation, Radius);
			}
		)
	);

	// assume the EQS callback will issue a move
	InstanceData.bIsMoving = true;
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
