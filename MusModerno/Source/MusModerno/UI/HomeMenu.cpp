// Fill out your copyright notice in the Description page of Project Settings.


#include "HomeMenu.h"

#include "MusButton.h"
#include "Options.h"
#include "Kismet/KismetSystemLibrary.h"
#include "MusModerno/Data/MusManager.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

void UHomeMenu::NativeConstruct()
{
	Super::NativeConstruct();

	MusManager = Cast<UMusManager>(GetGameInstance());
	StartGameButton.Get()->OnClicked().AddUObject(this, &UHomeMenu::StartGame);
	OptionsButton.Get()->OnClicked().AddUObject(this, &UHomeMenu::ShowOptions);
	ExitButton.Get()->OnClicked().AddUObject(this, &UHomeMenu::ExitGame);
}

void UHomeMenu::NativeDestruct()
{
	Super::NativeDestruct();
	
	StartGameButton.Get()->OnClicked().RemoveAll(this);
	OptionsButton.Get()->OnClicked().RemoveAll(this);
	ExitButton.Get()->OnClicked().RemoveAll(this);
}

void UHomeMenu::StartGame()
{
	MusManager.Get()->ShowTable();
	MusManager.Get()->StartGame();
	RemoveFromParent();
}

void UHomeMenu::ShowOptions()
{
	HomeMenuStack.Get()->AddWidget(OptionsClass);
}

void UHomeMenu::ExitGame()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, false);
}
