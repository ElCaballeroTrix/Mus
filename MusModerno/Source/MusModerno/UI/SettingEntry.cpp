// Fill out your copyright notice in the Description page of Project Settings.


#include "SettingEntry.h"

#include "CommonTextBlock.h"

void USettingEntry::SetSetting(FText ContentText, float _CurrentSettingValue, int _SettingMaxValue,
                               TArray<FText> _Options)
{
	ContentTextBlock.Get()->SetText(ContentText);
	CurrentSettingValue = _CurrentSettingValue;
	SettingMaxValue = _SettingMaxValue;
	Options = _Options;
}
