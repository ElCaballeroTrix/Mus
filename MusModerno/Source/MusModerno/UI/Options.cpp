// Fill out your copyright notice in the Description page of Project Settings.


#include "Options.h"

#include "MusButton.h"
#include "SettingBar.h"
#include "SettingOptions.h"
#include "Components/VerticalBox.h"
#include "MusModerno/Data/MusManager.h"
#include "MusModerno/Data/Mus_UserSettings.h"

void UOptions::NativeConstruct()
{
	Super::NativeConstruct();
	MusManager = Cast<UMusManager>(GetGameInstance());
	UserSettings = Cast<UMus_UserSettings>(GEngine->GameUserSettings);
	RetreatButton.Get()->OnClicked().AddUObject(this, &UOptions::ReturnToMenu);
	//--Music--
	TObjectPtr<USettingBar> musicSetting = Cast<USettingBar>(CreateWidget(GetWorld(), SettingBarClass));
	musicSetting.Get()->SetSetting(FText::FromString(TEXT("Música")),  UserSettings.Get()->GetMusicVolume(),1,
									{
									});
	musicSetting.Get()->OnChangeSetting.AddDynamic(this, &UOptions::ChangeMusicVolume);
	SettingsBox.Get()->AddChildToVerticalBox(musicSetting);
	SettingsList.Add(musicSetting);
	//--SFX--
	TObjectPtr<USettingBar> sfxSetting = Cast<USettingBar>(CreateWidget(GetWorld(), SettingBarClass));
	sfxSetting.Get()->SetSetting(FText::FromString("Sonidos"),  UserSettings.Get()->GetSFXVolume(),1,
									{
									});
	sfxSetting.Get()->OnChangeSetting.AddDynamic(this, &UOptions::ChangeSFXVolume);
	SettingsBox.Get()->AddChildToVerticalBox(sfxSetting);
	SettingsList.Add(sfxSetting);
	//--Brightness--
	TObjectPtr<USettingBar> brightnessSetting = Cast<USettingBar>(CreateWidget(GetWorld(), SettingBarClass));
	brightnessSetting.Get()->SetSetting(FText::FromString("Brillo"),  UserSettings.Get()->GetBrightnessValue(),1,
									{
									});
	brightnessSetting.Get()->OnChangeSetting.AddDynamic(this, &UOptions::ChangeBrightness);
	SettingsBox.Get()->AddChildToVerticalBox(brightnessSetting);
	SettingsList.Add(brightnessSetting);
	
	//--Color Blind Mode--
	TObjectPtr<USettingOptions> colorBlindModeSetting = Cast<USettingOptions>(CreateWidget(GetWorld(), SettingOptionsClass));
	colorBlindModeSetting.Get()->SetSetting(FText::FromString(TEXT("Modo Daltónico")), static_cast<int>(UserSettings.Get()->GetColorBlindMode()), 3,
									{
												FText::FromString("Normal Vision"),
												FText::FromString("Deuteranope"),
												FText::FromString("Protanope"),
												FText::FromString("Tritanope")
									});
	colorBlindModeSetting.Get()->OnChangeSetting.AddDynamic(this, &UOptions::ChangeColorBlindMode);
	SettingsBox.Get()->AddChildToVerticalBox(colorBlindModeSetting);
	SettingsList.Add(colorBlindModeSetting);
	//--Color Blind Strength--
	TObjectPtr<USettingOptions> colorBlindStrengthSetting = Cast<USettingOptions>(CreateWidget(GetWorld(), SettingOptionsClass));
	colorBlindStrengthSetting.Get()->SetSetting(FText::FromString(TEXT("Intensidad Daltónica")), static_cast<int>(UserSettings.Get()->GetColorBlindStrength()),4,
									{
											FText::FromString("1"),
											FText::FromString("2"),
											FText::FromString("3"),
											FText::FromString("4"),
											FText::FromString("5")
									});
	colorBlindStrengthSetting.Get()->OnChangeSetting.AddDynamic(this, &UOptions::ChangeColorBlindStrength);
	SettingsBox.Get()->AddChildToVerticalBox(colorBlindStrengthSetting);
	SettingsList.Add(colorBlindStrengthSetting);
	//--Max Amarrakos--
	TObjectPtr<USettingOptions> maxAmarrakosSetting = Cast<USettingOptions>(CreateWidget(GetWorld(), SettingOptionsClass));
	maxAmarrakosSetting.Get()->SetSetting(FText::FromString(TEXT("Victoria por:")), UserSettings.Get()->GetMaxAmarrakos(),1,
									{
											FText::FromString("6 Amarrakos"),
											FText::FromString("8 Amarrakos")
									});
	maxAmarrakosSetting.Get()->OnChangeSetting.AddDynamic(this, &UOptions::ChangeMusRulesAmarrakos);
	SettingsBox.Get()->AddChildToVerticalBox(maxAmarrakosSetting);
	SettingsList.Add(maxAmarrakosSetting);
	//--Number Of Kings--
	TObjectPtr<USettingOptions> numberOfKingsSetting = Cast<USettingOptions>(CreateWidget(GetWorld(), SettingOptionsClass));
	numberOfKingsSetting.Get()->SetSetting(FText::FromString(TEXT("Los 3 cuentan como reyes")), UserSettings.Get()->GetNumberOfKings(),1,
									{
											FText::FromString("NO"),
											FText::FromString(TEXT("SÍ"))
									});
	numberOfKingsSetting.Get()->OnChangeSetting.AddDynamic(this, &UOptions::ChangeMusRulesNumberOfKings);
	SettingsBox.Get()->AddChildToVerticalBox(numberOfKingsSetting);
	SettingsList.Add(numberOfKingsSetting);
}

void UOptions::NativeDestruct()
{
	Super::NativeDestruct();
	SettingsBox.Get()->ClearChildren();
	RetreatButton.Get()->OnClicked().RemoveAll(this);
	for (TObjectPtr<USettingEntry> setting : SettingsList)
	{
		setting.Get()->OnChangeSetting.RemoveAll(this);
	}
	SettingsList.Empty();
}

void UOptions::ReturnToMenu()
{
	DeactivateWidget();
}

void UOptions::ChangeMusicVolume(float Value)
{
	UserSettings.Get()->SetMusicVolume(Value);
	ApplySettings();
}

void UOptions::ChangeSFXVolume(float Value)
{
	UserSettings.Get()->SetSFXVolume(Value);
	ApplySettings();
}

void UOptions::ChangeColorBlindMode(float Value)
{
	EColorVisionDeficiency colorMode = EColorVisionDeficiency::NormalVision;
	switch (FMath::FloorToInt(Value))
	{
	case 0:
		colorMode = EColorVisionDeficiency::NormalVision;
		break;
	case 1:
		colorMode = EColorVisionDeficiency::Deuteranope;
		break;
	case 2:
		colorMode = EColorVisionDeficiency::Protanope;
		break;
	case 3:
		colorMode = EColorVisionDeficiency::Tritanope;
		break;
	}
	UserSettings.Get()->SetColorBlindMode(colorMode);
	ApplySettings();
}

void UOptions::ChangeColorBlindStrength(float Value)
{
	UserSettings.Get()->SetColorBlindStrength(Value);
	ApplySettings();
}

void UOptions::ChangeBrightness(float Value)
{
	UserSettings.Get()->SetBrightness(Value);
	ApplySettings();
}

void UOptions::ChangeMusRulesAmarrakos(float Value)
{
	UserSettings.Get()->SetMaxAmarrakos(Value);
	ApplySettings();
}

void UOptions::ChangeMusRulesNumberOfKings(float Value)
{
	UserSettings.Get()->SetNumberOfKings(Value);
	ApplySettings();
}

void UOptions::ApplySettings()
{
	UserSettings.Get()->SaveSettings();
	MusManager.Get()->ApplySettings();
}
