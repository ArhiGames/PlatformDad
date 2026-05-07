// Fill out your copyright notice in the Description page of Project Settings.


#include "InstantKillZone.h"

#include "Components/BoxComponent.h"
#include "PlatformDad/PlatformDadCharacter.h"

AInstantKillZone::AInstantKillZone()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<UBoxComponent>("Root");
	RootComponent = Root;

	bReplicates = true;
	SetReplicatingMovement(true);

	Root->OnComponentBeginOverlap.AddDynamic(this, &AInstantKillZone::OnOverlap);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(Root);
}

void AInstantKillZone::BeginPlay()
{
	Super::BeginPlay();
}

void AInstantKillZone::OnOverlap(UPrimitiveComponent* OverlappingComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlatformDadCharacter* Character = Cast<APlatformDadCharacter>(OtherActor);
	if (Character && Character->HasAuthority())
	{
		Character->ServerResetToCheckpoint(Character->GetActorLocation());
	}
}

