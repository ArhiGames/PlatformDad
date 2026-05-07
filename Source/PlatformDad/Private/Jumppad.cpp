// Fill out your copyright notice in the Description page of Project Settings.


#include "Jumppad.h"

#include "Components/BoxComponent.h"
#include "PlatformDad/PlatformDadCharacter.h"

AJumppad::AJumppad()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<UBoxComponent>(TEXT("Root"));
	RootComponent = Root;
	
	Root->OnComponentBeginOverlap.AddDynamic(this, &AJumppad::OnOverlap);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);
}

void AJumppad::BeginPlay()
{
	Super::BeginPlay();
}

void AJumppad::OnOverlap(UPrimitiveComponent* OverlappingComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlatformDadCharacter* Character = Cast<APlatformDadCharacter>(OtherActor);
	if (Character && Character->HasAuthority())
	{
		Character->LaunchCharacter(LaunchStrength, true, true);
	}
}

