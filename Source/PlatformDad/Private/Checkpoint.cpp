// Fill out your copyright notice in the Description page of Project Settings.

#include "PlatformDad/Public/Checkpoint.h"

#include "Components/BoxComponent.h"
#include "PlatformDad/PlatformDadCharacter.h"

ACheckpoint::ACheckpoint()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<UBoxComponent>("Root");
	RootComponent = Root;

	Root->OnComponentBeginOverlap.AddDynamic(this, &ACheckpoint::OnOverlap);
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(Root);
}

void ACheckpoint::BeginPlay()
{
	Super::BeginPlay();
}

void ACheckpoint::OnOverlap(UPrimitiveComponent* OverlappingComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlatformDadCharacter* Character = Cast<APlatformDadCharacter>(OtherActor);
	if (Character)
	{
		if (!Character->IsCheckpointActivated(this))
		{
			if (Character->HasAuthority())
			{
				Character->SetLastCheckpoint(this);
			}

			if (Character->IsLocallyControlled() && !Character->IsCheckpointActivated(this))
			{
				UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(Mesh->GetMaterial(0), this);
				if (DynamicMaterial)
				{
					DynamicMaterial->SetVectorParameterValue(FName("Color"), FLinearColor::Green);
				}
				Mesh->SetMaterial(0, DynamicMaterial);
			}
		}
	}
}

