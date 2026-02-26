// Copyright ArconicLabs. All Rights Reserved.

// Global StateTree task that manages a suspicion level driven by hearing
// events. Accumulates suspicion from HearingStrength input and decays it
// over time, outputting the current level and a threshold flag.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "AxMGlobalTask_Suspicion.generated.h"

USTRUCT()
struct FAxMGlobalTask_SuspicionInstanceData
{
	GENERATED_BODY()

	/** [Input] Hearing strength from Perception global task (0 when no event this tick) */
	UPROPERTY(EditAnywhere, Category = "Input")
	float HearingStrength = 0.0f;

	/** Per-second decay rate when no stimuli arrive */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	float SuspicionDecayRate = 0.15f;

	/** Suspicion level that triggers the Investigate state */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	float InvestigateThreshold = 0.4f;

	/** Multiplier applied to hearing strength when accumulating suspicion */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	float SuspicionGainMultiplier = 1.0f;

	/** [Output] Current suspicion level (0.0 = calm, 1.0 = fully alert) */
	UPROPERTY(EditAnywhere, Category = "Output")
	float SuspicionLevel = 0.0f;

	/** [Output] True when SuspicionLevel exceeds InvestigateThreshold */
	UPROPERTY(EditAnywhere, Category = "Output")
	bool bIsSuspicious = false;
};

USTRUCT(meta = (DisplayName = "AxM Suspicion (Global)", Category = "Animus Ex Machina|Global"))
struct ANIMUSEXMACHINA_API FAxMGlobalTask_Suspicion : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FAxMGlobalTask_SuspicionInstanceData;

	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	}

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition) const override;

	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context,
		const float DeltaTime) const override;

#if WITH_EDITOR
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView,
		const IStateTreeBindingLookup& BindingLookup,
		EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;
#endif
};
