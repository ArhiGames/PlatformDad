// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MovingPlatform.generated.h"

UENUM()
enum class EMovingAxis : uint8
{
	X UMETA(DisplayName = "X"),
	Y UMETA(DisplayName = "Y"),
	Z UMETA(DisplayName = "Z")
};

UCLASS()
class PLATFORMDAD_API AMovingPlatform : public AActor
{
	GENERATED_BODY()
	
public:	
	AMovingPlatform();

	UPROPERTY(EditAnywhere)
	USceneComponent* Root;
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Mesh;
	
public:	
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "Movement")
	float Speed = 1.f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float Amplitude = 500.f;
	
	UPROPERTY(EditAnywhere, Category = "Movement")
	EMovingAxis MovingAxis = EMovingAxis::Y;
	
	FVector StartLocation;
	float ElapsedTime = 0.f;
	
private:
	void MovePlatform(float DeltaTime);
};
