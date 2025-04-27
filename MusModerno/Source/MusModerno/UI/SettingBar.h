// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SettingEntry.h"
#include "SettingBar.generated.h"

class UCommonTextBlock;
class USlider;
class UProgressBar;

/**
 * 
 */
UCLASS()
class MUSMODERNO_API USettingBar : public USettingEntry
{
	GENERATED_BODY()

public:
	virtual void SetSetting(FText ContentText, float _CurrentSettingValue, int _SettingMaxValue, TArray<FText> _Options) override;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UProgressBar> SettingProgressBar;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<USlider> SettingSlider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MUS| Settings")
	FSlateBrush HoveredSliderImage;
	
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	
private:
	FProgressBarStyle SettingsNormalProgressBarFillStyle;
	FProgressBarStyle SettingsHoverProgressBarFillStyle;
	
	UFUNCTION()
	void SliderChanged(float Value);
};
