// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "Mus_UserSettings.generated.h"

/**
 * 
 */
UCLASS()
class MUSMODERNO_API UMus_UserSettings : public UGameUserSettings
{
	GENERATED_BODY()
	
public:
	UMus_UserSettings(const FObjectInitializer& ObjectInitializer);
	virtual void LoadSettings(bool bForceReload = false) override;
	virtual void ApplySettings(bool bCheckForCommandLineOverrides) override;
	
	EColorVisionDeficiency GetColorBlindMode(){return ColorBlindMode;}
	void SetColorBlindMode(EColorVisionDeficiency Value){ColorBlindMode = Value;}
	
	int GetColorBlindStrength(){return ColorBlindStrength;}
	void SetColorBlindStrength(int Value){ColorBlindStrength = Value;}

	float GetBrightnessValue(){return BrightnessValue;}
	void SetBrightness(float Value){BrightnessValue = Value;}
	
	float GetMusicVolume(){return MusicVolume;}
	void SetMusicVolume(float Value){MusicVolume = Value;}
	
	float GetSFXVolume(){return SFXVolume;}
	void SetSFXVolume(float Value){SFXVolume = Value;}

	int32 GetMaxAmarrakos(){return MaxAmarrakos;}
	void SetMaxAmarrakos(float Value){MaxAmarrakos = Value;}

	int32 GetNumberOfKings(){return NumberOfKings;}
	void SetNumberOfKings(float Value){NumberOfKings = Value;}

protected:
	UPROPERTY(Config)
	EColorVisionDeficiency ColorBlindMode = EColorVisionDeficiency::NormalVision;
	UPROPERTY(Config)
	int ColorBlindStrength = 0;
	UPROPERTY(Config)
	float BrightnessValue = 0.5f;
	UPROPERTY(Config)
	float MusicVolume = 0.5f;
	UPROPERTY(Config)
	float SFXVolume = 0.5f;
	UPROPERTY(Config)
	int32 MaxAmarrakos = 1.0f;
	UPROPERTY(Config)
	int32 NumberOfKings = 1.0f;
};
