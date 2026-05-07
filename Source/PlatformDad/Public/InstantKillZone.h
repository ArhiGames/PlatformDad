// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InstantKillZone.generated.h"

UCLASS()
class PLATFORMDAD_API AInstantKillZone : public AActor
{
	GENERATED_BODY()
	
public:	
	AInstantKillZone();

	UPROPERTY(EditAnywhere)
	UShapeComponent* Root;
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Mesh;
	
protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappingComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
