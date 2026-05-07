// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PowerUp.generated.h"

UCLASS()
class PLATFORMDAD_API APowerUp : public AActor
{
	GENERATED_BODY()
	
public:	
	APowerUp();

	UPROPERTY(EditAnywhere)
	UShapeComponent* Root;
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Mesh;
	
public:	
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "Animation")
	float Speed = 1.f;
	UPROPERTY(EditAnywhere, Category = "Animation")
	float Amplitude = 20.f;
	UPROPERTY(EditAnywhere, Category = "Animation")
	float RotationSpeed = 60.f;

	float ElapsedTime = 0.f;
	
private:
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappingComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void AnimateMesh(const float DeltaTime) const;
};
