// Copyright ArconicLabs. All Rights Reserved.

// StateTree task that navigates the NPC along an assigned patrol route.
// Delegate-driven — moves to each waypoint, waits, then advances to the
// next. Succeeds immediately if no route is assigned.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "AxMTask_Patrol.generated.h"

class AAIController;

/**
 *  Instance data for the patrol task.
 */
USTRUCT()
struct FAxMTask_PatrolInstanceData
{
	GENERATED_BODY()

	/** AI Controller for navigation and route access */
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AAIController> Controller;

	/** How long to wait at each waypoint before moving to the next */
	UPROPERTY(EditAnywhere, Category = "Parameter", meta = (ClampMin = "0", Units = "s"))
	float WaitDuration = 2.0f;

	/** Acceptance radius for reaching each waypoint */
	UPROPERTY(EditAnywhere, Category = "Parameter", meta = (ClampMin = "0", Units = "cm"))
	float AcceptanceRadius = 50.0f;

	/** If true, loop from last point back to first; if false, ping-pong */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	bool bLoop = true;

	// --- Internal state ---
	int32 CurrentPointIndex = 0;
	int32 Direction = 1;
	FDelegateHandle MoveFinishedHandle;
	FTimerHandle WaitTimerHandle;
};

/**
 *  Patrol task — navigates between waypoints on an assigned patrol route.
 */
USTRUCT(meta = (DisplayName = "AxM Patrol", Category = "Animus Ex Machina|Tasks"))
struct ANIMUSEXMACHINA_API FAxMTask_Patrol : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FAxMTask_PatrolInstanceData;

	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition) const override;

	virtual void ExitState(FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition) const override;

#if WITH_EDITOR
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView,
		const IStateTreeBindingLookup& BindingLookup,
		EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;
#endif
};
