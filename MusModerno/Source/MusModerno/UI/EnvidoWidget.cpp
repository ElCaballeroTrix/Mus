// Fill out your copyright notice in the Description page of Project Settings.


#include "EnvidoWidget.h"

#include "CommonTextBlock.h"
#include "MusButton.h"

void UEnvidoWidget::NativeConstruct()
{
	Super::NativeConstruct();

	MinusButton.Get()->OnClicked().AddUObject(this, &UEnvidoWidget::ReduceEnvidio);
	PlusButton.Get()->OnClicked().AddUObject(this, &UEnvidoWidget::AddEnvidio);
}

void UEnvidoWidget::NativeDestruct()
{
	Super::NativeDestruct();
	
	MinusButton.Get()->OnClicked().RemoveAll(this);
	PlusButton.Get()->OnClicked().RemoveAll(this);
}

void UEnvidoWidget::ReduceEnvidio()
{
	--Envidio;
	Envidio = FMath::Clamp(Envidio, 2, 30);
	EnvidioTextBlock.Get()->SetText(FText::FromString(FString::Printf(TEXT("%d"), Envidio)));
}

void UEnvidoWidget::AddEnvidio()
{
	++Envidio;
	Envidio = FMath::Clamp(Envidio, 2, 30);
	EnvidioTextBlock.Get()->SetText(FText::FromString(FString::Printf(TEXT("%d"), Envidio)));
}
