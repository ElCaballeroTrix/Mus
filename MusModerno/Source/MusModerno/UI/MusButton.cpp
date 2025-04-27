// Fill out your copyright notice in the Description page of Project Settings.


#include "MusButton.h"

#include "CommonTextBlock.h"

void UMusButton::NativePreConstruct()
{
	Super::NativePreConstruct();
	SetTextOfButton(TextOfButton);
}

void UMusButton::SetTextOfButton(FText NewText)
{
	if(MusButtonTextBlock)
	{
		if(!NewText.IsEmpty())
		{
			MusButtonTextBlock.Get()->SetText(NewText);
		}
		else
		{
			MusButtonTextBlock.Get()->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

