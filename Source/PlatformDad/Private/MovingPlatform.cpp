// Fill out your copyright notice in the Description page of Project Settings.


#include "MovingPlatform.h"

AMovingPlatform::AMovingPlatform()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = Root;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(Root);

	bReplicates = true;
	SetReplicatingMovement(true);
}

void AMovingPlatform::BeginPlay()
{
	Super::BeginPlay();

#if WITH_SERVER_CODE
	if (HasAuthority())
	{
		StartLocation = GetActorLocation();
	}
#endif
}

void AMovingPlatform::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

#if WITH_SERVER_CODE
	if (HasAuthority())
	{
		MovePlatform(DeltaTime);
	}
#endif
}

void AMovingPlatform::MovePlatform(const float DeltaTime)
{
	ElapsedTime += DeltaTime * Speed;
	const float SinValue = FMath::Sin(ElapsedTime);

	FVector NewPosition = StartLocation;
	if (MovingAxis == EMovingAxis::X) NewPosition.X += SinValue * Amplitude;
	else if (MovingAxis == EMovingAxis::Y) NewPosition.Y += SinValue * Amplitude;
	else if (MovingAxis == EMovingAxis::Z) NewPosition.Z += SinValue * Amplitude;

	SetActorLocation(NewPosition);
}
