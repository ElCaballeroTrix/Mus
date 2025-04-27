// Fill out your copyright notice in the Description page of Project Settings.


#include "SettingOptions.h"

#include "CommonTextBlock.h"
#include "MusButton.h"

void USettingOptions::NativeConstruct()
{
	Super::NativeConstruct();
	//Bindings
	ButtonDecrease.Get()->OnClicked().AddUObject(this, &USettingOptions::OnDecreasedSetting);
	ButtonIncrease.Get()->OnClicked().AddUObject(this, &USettingOptions::OnIncreasedSetting);
}

void USettingOptions::NativeDestruct()
{
	Super::NativeDestruct();
	ButtonDecrease.Get()->OnClicked().RemoveAll(this);
	ButtonIncrease.Get()->OnClicked().RemoveAll(this);
}

void USettingOptions::SetSetting(FText ContentText, float _CurrentSettingValue, int _SettingMaxValue,
	TArray<FText> _Options)
{
	Super::SetSetting(ContentText, _CurrentSettingValue, _SettingMaxValue, _Options);
	OptionTextBlock.Get()->SetText(Options[CurrentSettingValue]);
}

void USettingOptions::OnDecreasedSetting()
{
	--CurrentSettingValue;
	if(CurrentSettingValue < 0)
	{
		CurrentSettingValue = SettingMaxValue;
	}

	CurrentSettingValue = FMath::Clamp(CurrentSettingValue, 0, SettingMaxValue);
	OptionTextBlock.Get()->SetText(Options[CurrentSettingValue]);
	OnChangeSetting.Broadcast(CurrentSettingValue);
}

void USettingOptions::OnIncreasedSetting()
{
	++CurrentSettingValue;
	if(CurrentSettingValue > SettingMaxValue)
	{
		CurrentSettingValue = 0;
	}
	CurrentSettingValue = FMath::Clamp(CurrentSettingValue, 0, SettingMaxValue);
	OptionTextBlock.Get()->SetText(Options[CurrentSettingValue]);
	OnChangeSetting.Broadcast(CurrentSettingValue);
}
