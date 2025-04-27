// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "SettingEntry.generated.h"

class UCommonTextBlock;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FChangeSetting, float, Value);
/**
 * 
 */
UCLASS()
class MUSMODERNO_API USettingEntry : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	FChangeSetting OnChangeSetting;
	virtual void SetSetting(FText ContentText, float _CurrentSettingValue, int _SettingMaxValue, TArray<FText> _Options);

protected:
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> ContentTextBlock;
	float CurrentSettingValue = 0;
	int SettingMaxValue = 0;
	TArray<FText> Options;
};
