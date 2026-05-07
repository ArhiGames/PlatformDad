// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Jumppad.generated.h"

UCLASS()
class PLATFORMDAD_API AJumppad : public AActor
{
	GENERATED_BODY()
	
public:	
	AJumppad();

	UPROPERTY(EditAnywhere)
	UShapeComponent* Root;
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere)
	FVector LaunchStrength = FVector(0.f, 0.f, 1000.f);

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappingComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
