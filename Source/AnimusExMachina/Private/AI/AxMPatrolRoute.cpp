// Copyright ArconicLabs. All Rights Reserved.

#include "AI/AxMPatrolRoute.h"
#include "Components/SplineComponent.h"

AAxMPatrolRoute::AAxMPatrolRoute()
{
	PrimaryActorTick.bCanEverTick = false;

	SplineComp = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComp"));
	RootComponent = SplineComp;
}

TArray<FVector> AAxMPatrolRoute::GetPatrolPoints() const
{
	TArray<FVector> Points;
	if (!SplineComp)
	{
		return Points;
	}

	const int32 NumPoints = SplineComp->GetNumberOfSplinePoints();
	Points.Reserve(NumPoints);
	for (int32 i = 0; i < NumPoints; ++i)
	{
		Points.Add(SplineComp->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World));
	}

	return Points;
}

int32 AAxMPatrolRoute::GetNumPatrolPoints() const
{
	return SplineComp ? SplineComp->GetNumberOfSplinePoints() : 0;
}

FVector AAxMPatrolRoute::GetPatrolPoint(int32 Index) const
{
	if (!SplineComp || Index < 0 || Index >= SplineComp->GetNumberOfSplinePoints())
	{
		return FVector::ZeroVector;
	}

	return SplineComp->GetLocationAtSplinePoint(Index, ESplineCoordinateSpace::World);
}
