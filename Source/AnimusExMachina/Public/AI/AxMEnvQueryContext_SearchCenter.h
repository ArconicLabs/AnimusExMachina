// Copyright ArconicLabs. All Rights Reserved.

// EQS context that provides the AI Controller's cached last known location
// as a point for Environment Queries. Used by the SearchArea task to
// generate search points around where the target was last seen.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "AxMEnvQueryContext_SearchCenter.generated.h"

UCLASS()
class ANIMUSEXMACHINA_API UAxMEnvQueryContext_SearchCenter : public UEnvQueryContext
{
	GENERATED_BODY()

public:

	virtual void ProvideContext(FEnvQueryInstance& QueryInstance,
		FEnvQueryContextData& ContextData) const override;
};
