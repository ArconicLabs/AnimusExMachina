// Copyright ArconicLabs. All Rights Reserved.

// Global StateTree task that computes derived targeting data (distance,
// line of sight, attack range) from a TargetActor reference.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "AxMGlobalTask_TargetTracking.generated.h"

class AAIController;

/**
 *  Instance data for the target tracking global task.
 */
USTRUCT()
struct FAxMGlobalTask_TargetTrackingInstanceData
{
	GENERATED_BODY()

	/** AI Controller for line-of-sight traces */
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AAIController> Controller;

	/** [Input] Current target actor (bound from Perception output) */
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<AActor> TargetActor;

	/** Attack range threshold */
	UPROPERTY(EditAnywhere, Category = "Parameter", meta = (ClampMin = "0", Units = "cm"))
	float AttackRange = 200.0f;

	/** [Output] Distance from the NPC to TargetActor */
	UPROPERTY(EditAnywhere, Category = "Output")
	float DistanceToTarget = 0.0f;

	/** [Output] Whether the NPC has unobstructed line of sight to TargetActor */
	UPROPERTY(EditAnywhere, Category = "Output")
	bool HasLineOfSight = false;

	/** [Output] Whether TargetActor is within AttackRange */
	UPROPERTY(EditAnywhere, Category = "Output")
	bool IsInAttackRange = false;
};

/**
 *  Global StateTree task that computes DistanceToTarget, HasLineOfSight,
 *  and IsInAttackRange from TargetActor. Runs every tick on the root state.
 */
USTRUCT(meta = (DisplayName = "AxM Target Tracking (Global)", Category = "AxM|Global"))
struct ANIMUSEXMACHINA_API FAxMGlobalTask_TargetTracking : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FAxMGlobalTask_TargetTrackingInstanceData;

	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}

	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context,
		const float DeltaTime) const override;

#if WITH_EDITOR
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView,
		const IStateTreeBindingLookup& BindingLookup,
		EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;
#endif
};
