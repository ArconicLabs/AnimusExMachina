// Copyright ArconicLabs. All Rights Reserved.

#include "AI/AxMEnvQueryContext_SearchCenter.h"
#include "AI/AxMAIController.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Point.h"

void UAxMEnvQueryContext_SearchCenter::ProvideContext(
	FEnvQueryInstance& QueryInstance,
	FEnvQueryContextData& ContextData) const
{
	APawn* QuerierPawn = Cast<APawn>(QueryInstance.Owner.Get());
	if (!QuerierPawn)
	{
		return;
	}

	AAxMAIController* Controller = Cast<AAxMAIController>(QuerierPawn->GetController());
	if (!Controller)
	{
		return;
	}

	UEnvQueryItemType_Point::SetContextHelper(
		ContextData, Controller->GetCachedLastKnownLocation());
}
