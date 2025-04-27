// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SettingEntry.h"
#include "SettingOptions.generated.h"

class UMusButton;
/**
 * 
 */
UCLASS()
class MUSMODERNO_API USettingOptions : public USettingEntry
{
	GENERATED_BODY()

public:
	virtual void SetSetting(FText ContentText, float _CurrentSettingValue, int _SettingMaxValue, TArray<FText> _Options) override;
	
protected:
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> OptionTextBlock;
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UMusButton> ButtonDecrease;
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UMusButton> ButtonIncrease;

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	void OnDecreasedSetting();
	void OnIncreasedSetting();
};
