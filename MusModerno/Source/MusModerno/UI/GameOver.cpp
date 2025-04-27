// Fill out your copyright notice in the Description page of Project Settings.


#include "GameOver.h"

#include "CommonTextBlock.h"
#include "MusButton.h"
#include "MusModerno/Data/MusManager.h"

void UGameOver::NativeConstruct()
{
	Super::NativeConstruct();

	MusManager = Cast<UMusManager>(GetGameInstance());
	PlayAnimation(FadeInAnimation);
}

void UGameOver::NativeDestruct()
{
	Super::NativeDestruct();
	PlayAgainButton.Get()->OnClicked().RemoveAll(this);
	ExitButton.Get()->OnClicked().RemoveAll(this);
}

void UGameOver::OnAnimationFinished_Implementation(const UWidgetAnimation* Animation)
{
	Super::OnAnimationFinished_Implementation(Animation);
	if(FadeIn)
	{
		FadeIn = false;
		PlayAgainButton.Get()->OnClicked().AddUObject(this, &UGameOver::PlayAgain);
		ExitButton.Get()->OnClicked().AddUObject(this, &UGameOver::ReturnToHomeMenu);
	}
	else
	{
		MusManager.Get()->StartGame();
		DeactivateWidget();
	}
}

void UGameOver::PlayerWon(bool Value)
{
	if(Value)
	{
		FinalResultTextBlock.Get()->SetText(FText::FromString(TEXT("¡¡HAS GANADO!!")));
		FinalResultTextBlock.Get()->SetColorAndOpacity(WinnerColor);
	}
	else
	{
		FinalResultTextBlock.Get()->SetText(FText::FromString(TEXT("¡¡HAS PERDIDO!!")));
		FinalResultTextBlock.Get()->SetColorAndOpacity(LoserColor);
	}
}

void UGameOver::PlayAgain()
{
	PlayAgainButton.Get()->OnClicked().RemoveAll(this);
	ExitButton.Get()->OnClicked().RemoveAll(this);
	PlayAnimationReverse(FadeInAnimation);
}

void UGameOver::ReturnToHomeMenu()
{
	OnExitSignature.Broadcast();
	DeactivateWidget();
}
