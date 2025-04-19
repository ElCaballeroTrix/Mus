// Fill out your copyright notice in the Description page of Project Settings.


#include "CardWidget.h"

#include "Components/Image.h"

void UCardWidget::SetCard(FCards_Struct* _CardInfo, bool UpsideDown)
{
	CardInfo = _CardInfo;
	if(UpsideDown)
	{
		CardImage.Get()->SetBrush(BackOfCardBrush);
	}
	else
	{
		CardImage.Get()->SetBrush(CardInfo->CardBrush);
	}
}

void UCardWidget::SetCardActive(bool _CardActive)
{
	CardActive = _CardActive;
	SetIsEnabled(CardActive);
}

void UCardWidget::NativeOnClicked()
{
	Super::NativeOnClicked();
	if(CardActive)
	{
		
	}
}
