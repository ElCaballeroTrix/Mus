// Fill out your copyright notice in the Description page of Project Settings.


#include "SettingBar.h"

#include "Components/ProgressBar.h"
#include "Components/Slider.h"

void USettingBar::NativeConstruct()
{
	Super::NativeConstruct();
	SettingSlider.Get()->OnValueChanged.AddDynamic(this, &USettingBar::SliderChanged);
	SettingsNormalProgressBarFillStyle = SettingProgressBar.Get()->GetWidgetStyle();
	SettingsHoverProgressBarFillStyle = SettingProgressBar.Get()->GetWidgetStyle();
	SettingsHoverProgressBarFillStyle.BackgroundImage = HoveredSliderImage;
}

void USettingBar::NativeDestruct()
{
	Super::NativeDestruct();
	SettingSlider.Get()->OnValueChanged.RemoveAll(this);
}

void USettingBar::SetSetting(FText ContentText, float _CurrentSettingValue, int _SettingMaxValue, TArray<FText> _Options)
{
	Super::SetSetting(ContentText, _CurrentSettingValue, _SettingMaxValue, _Options);
	SettingSlider.Get()->SetMaxValue(SettingMaxValue);
	SettingSlider.Get()->SetValue(CurrentSettingValue);
	SettingProgressBar.Get()->SetPercent(CurrentSettingValue / SettingSlider.Get()->GetMaxValue());
}

void USettingBar::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	SettingProgressBar.Get()->SetWidgetStyle(SettingsHoverProgressBarFillStyle);
}

void USettingBar::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	SettingProgressBar.Get()->SetWidgetStyle(SettingsNormalProgressBarFillStyle);
}

void USettingBar::SliderChanged(float Value)
{
	SettingProgressBar.Get()->SetPercent(Value / SettingSlider.Get()->GetMaxValue());
	OnChangeSetting.Broadcast(Value);
}
