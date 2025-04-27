// Fill out your copyright notice in the Description page of Project Settings.


#include "Mus_UserSettings.h"

UMus_UserSettings::UMus_UserSettings(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	ColorBlindMode = EColorVisionDeficiency::NormalVision;
	ColorBlindStrength = 0;
	BrightnessValue = 0.5f;
	MusicVolume =0.5f;
	SFXVolume =0.5f;
	MaxAmarrakos = 1;
	NumberOfKings = 1;
}

void UMus_UserSettings::LoadSettings(bool bForceReload)
{
	Super::LoadSettings(bForceReload);
}

void UMus_UserSettings::ApplySettings(bool bCheckForCommandLineOverrides)
{
	Super::ApplySettings(bCheckForCommandLineOverrides);
}
