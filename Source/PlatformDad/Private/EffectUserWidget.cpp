// Fill out your copyright notice in the Description page of Project Settings.

#include "EffectUserWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

void UEffectUserWidget::SetEffectUserData(const FPowerUpData& EffectData)
{
	SetVisibility(ESlateVisibility::Visible);
	
	Thumbnail->SetBrushFromTexture(EffectData.Thumbnail);
	const FText NewText = FText::Format(FText::FromString("{0} ({1}s)"), CurrentEffectData.DescriptionText, EffectData.Seconds);
	DescriptionTextBlock->SetText(NewText);

	Time = EffectData.Seconds;
	
	HasSetData = true;
	CurrentEffectData = EffectData;
}

void UEffectUserWidget::NativeTick(const FGeometry& MyGeometry, const float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (HasSetData)
	{
		Time -= InDeltaTime;
		const FText NewText = FText::Format(FText::FromString("{0} ({1}s)"), CurrentEffectData.DescriptionText, static_cast<int32>(Time));
		DescriptionTextBlock->SetText(NewText);
		
		if (Time < 0.0f)
		{
			CurrentEffectData = FPowerUpData();
			SetVisibility(ESlateVisibility::Collapsed);
			HasSetData = false;
		}
	}
}
