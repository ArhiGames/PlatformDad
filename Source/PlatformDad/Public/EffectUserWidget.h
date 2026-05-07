// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlatformDad/PlatformDadCharacter.h"
#include "EffectUserWidget.generated.h"

struct FPowerUpData;
class UTextBlock;
class UImage;

UCLASS()
class PLATFORMDAD_API UEffectUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetEffectUserData(const FPowerUpData& EffectData);

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta= (BindWidget))
	UImage* Thumbnail;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta= (BindWidget))
	UTextBlock* DescriptionTextBlock;

private:
	bool HasSetData = false;
	float Time = 0.f;
	FPowerUpData CurrentEffectData;
};
