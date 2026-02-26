// Copyright ArconicLabs. All Rights Reserved.

// StateTree task that searches an area around a center point by navigating
// to successive EQS-driven (or random NavMesh) points. Succeeds when the
// search duration expires.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "AxMTask_SearchArea.generated.h"

class AAIController;
class UEnvQuery;

USTRUCT()
struct FAxMTask_SearchAreaInstanceData
{
	GENERATED_BODY()

	/** AI Controller for navigation and EQS queries */
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AAIController> Controller;

	/** Center point for the search pattern (bind to LastKnownLocation) */
	UPROPERTY(EditAnywhere, Category = "Input")
	FVector SearchCenter = FVector::ZeroVector;

	/** Radius around the center to generate search points */
	UPROPERTY(EditAnywhere, Category = "Parameter", meta = (ClampMin = "100", Units = "cm"))
	float SearchRadius = 500.0f;

	/** Total time to spend searching before giving up */
	UPROPERTY(EditAnywhere, Category = "Parameter", meta = (ClampMin = "1", Units = "s"))
	float SearchDuration = 10.0f;

	/** EQS query asset for generating search points (null = use random NavMesh points) */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	TObjectPtr<UEnvQuery> QueryAsset;

	/** Acceptance radius for reaching each search point */
	UPROPERTY(EditAnywhere, Category = "Parameter", meta = (ClampMin = "0", Units = "cm"))
	float AcceptanceRadius = 50.0f;

	// --- Internal state ---
	float ElapsedTime = 0.0f;
	bool bIsMoving = false;
};

USTRUCT(meta = (DisplayName = "AxM Search Area", Category = "AxM|Tasks"))
struct ANIMUSEXMACHINA_API FAxMTask_SearchArea : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FAxMTask_SearchAreaInstanceData;

	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition) const override;

	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context,
		const float DeltaTime) const override;

	virtual void ExitState(FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition) const override;

#if WITH_EDITOR
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView,
		const IStateTreeBindingLookup& BindingLookup,
		EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;
#endif

private:

	/** Pick a random reachable point and issue a MoveTo. Returns true if successful. */
	bool MoveToRandomPoint(FInstanceDataType& InstanceData,
		FStateTreeExecutionContext& Context) const;

	/** Run the EQS query and navigate to the best result. */
	void RunEQSQuery(FInstanceDataType& InstanceData,
		FStateTreeExecutionContext& Context) const;
};
