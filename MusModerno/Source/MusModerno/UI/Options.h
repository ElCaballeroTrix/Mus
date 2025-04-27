// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "Options.generated.h"

class UMusButton;
class UMusManager;
class USettingEntry;
class UMus_UserSettings;
class USettingOptions;
class USettingBar;
class UVerticalBox;
/**
 * 
 */
UCLASS()
class MUSMODERNO_API UOptions : public UCommonActivatableWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UVerticalBox> SettingsBox;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UMusButton> RetreatButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MUS| Options")
	TSubclassOf<USettingOptions> SettingOptionsClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MUS| Options")
	TSubclassOf<USettingBar> SettingBarClass;

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	TObjectPtr<UMus_UserSettings> UserSettings;
	TObjectPtr<UMusManager> MusManager;
	TArray<TObjectPtr<USettingEntry>> SettingsList;

	UFUNCTION()
	void ReturnToMenu();
	UFUNCTION()
	void ChangeMusicVolume(float Value);
	UFUNCTION()
	void ChangeSFXVolume(float Value);
	UFUNCTION()
	void ChangeColorBlindMode(float Value);
	UFUNCTION()
	void ChangeColorBlindStrength(float Value);
	UFUNCTION()
	void ChangeBrightness(float Value);
	UFUNCTION()
	void ChangeMusRulesAmarrakos(float Value);
	UFUNCTION()
	void ChangeMusRulesNumberOfKings(float Value);
	void ApplySettings();
};
