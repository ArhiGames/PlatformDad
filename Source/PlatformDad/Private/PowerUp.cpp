// Fill out your copyright notice in the Description page of Project Settings.


#include "PowerUp.h"

#include "EngineUtils.h"
#include "Components/BoxComponent.h"
#include "PlatformDad/PlatformDadCharacter.h"

APowerUp::APowerUp()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<UBoxComponent>(TEXT("Root"));
	RootComponent = Root;

	bReplicates = true;
	SetReplicatingMovement(true);
	
	Root->OnComponentBeginOverlap.AddDynamic(this, &APowerUp::OnOverlap);
	Root->SetGenerateOverlapEvents(true);
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);
}

void APowerUp::BeginPlay()
{
	Super::BeginPlay();
}

void APowerUp::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	ElapsedTime += DeltaTime * Speed;
	AnimateMesh(DeltaTime);
}

void APowerUp::OnOverlap(UPrimitiveComponent* OverlappingComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlatformDadCharacter* Character = Cast<APlatformDadCharacter>(OtherActor);
	if (Character && Character->HasAuthority())
	{
		Character->ServerOnRandomPickupTakenUp(Character);
		Destroy();
	}
}

void APowerUp::AnimateMesh(const float DeltaTime) const
{
	const float SinValue = FMath::Sin(ElapsedTime);

	FVector NewPosition = FVector::ZeroVector;
	NewPosition.Z += SinValue * Amplitude;

	Mesh->SetRelativeLocation(NewPosition);

	FRotator NewRotation = Mesh->GetRelativeRotation();
	NewRotation.Yaw += RotationSpeed * DeltaTime;
	Mesh->SetRelativeRotation(NewRotation);
}

