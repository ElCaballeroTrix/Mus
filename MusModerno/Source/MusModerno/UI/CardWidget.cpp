// Fill out your copyright notice in the Description page of Project Settings.


#include "CardWidget.h"

#include "Components/Image.h"
#include "Kismet/KismetMaterialLibrary.h"

void UCardWidget::NativeConstruct()
{
	Super::NativeConstruct();
	DynamicMat = UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(), DissolveMaterial);
	DynamicMat.Get()->SetScalarParameterValue("Dissolve", DissolvedCompletely);
	DissolveBlend.SetValueRange(DissolvedCompletely, DissolvedNothing);
	DissolveBlend.SetBlendTime(DissolveTime);
	SetIsEnabled(false);
}

void UCardWidget::SetCard(FCards_Struct* _CardInfo, bool UpsideDown)
{
	//If it is hidden because of new round or discard, execute dissolve animation
	if(IsDissolvedCompletely)
	{
		DissolveCard();
	}
	//Set the information and modify the dissolve material with the proper texture
	CardInfo = _CardInfo;
	TObjectPtr<UTexture> cardTexture;
	if(UpsideDown)
	{
		cardTexture = Cast<UTexture>(BackOfCardBrush.GetResourceObject());
	}
	else
	{
		cardTexture = Cast<UTexture>(CardInfo->CardBrush.GetResourceObject());
	}
	DynamicMat.Get()->SetTextureParameterValue("MaterialTexture", cardTexture);
	FSlateBrush cardBrush = CardImage.Get()->GetBrush();
	cardBrush.SetResourceObject(DynamicMat);
	CardImage.Get()->SetBrush(cardBrush);
}

void UCardWidget::SetCardActive(bool CardActive)
{	
	SetIsEnabled(CardActive);
	if(CardActive)
	{
		PlayAnimation(IncreaseCardSize);
	}
	else
	{
		PlayAnimationReverse(IncreaseCardSize);
	}
}

void UCardWidget::DissolveCard()
{
	SetCardActive(false);
	DiscardCard = false;
	CardImageGlow.Get()->SetVisibility(ESlateVisibility::Hidden);
	if(IsDissolvedCompletely)
	{
		DissolveBlend.SetValueRange(DissolvedCompletely, DissolvedNothing);
	}
	else
	{
		DissolveBlend.SetValueRange(DissolvedNothing, DissolvedCompletely);
	}
	DissolveBlend.Reset();
	ShouldDissolve = true;
}

void UCardWidget::ResetDissolve()
{
	IsDissolvedCompletely = true;
}

void UCardWidget::NativeOnClicked()
{
	Super::NativeOnClicked();
	DiscardCard = !DiscardCard;
	DiscardCard ? CardImageGlow.Get()->SetVisibility(ESlateVisibility::SelfHitTestInvisible) : CardImageGlow.Get()->SetVisibility(ESlateVisibility::Hidden);
}

void UCardWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if(ShouldDissolve)
	{
		DissolveBlend.Update(InDeltaTime);
		DynamicMat.Get()->SetScalarParameterValue("Dissolve", DissolveBlend.GetBlendedValue());
		if(DissolveBlend.IsComplete())
		{
			ShouldDissolve = false;
			IsDissolvedCompletely = !IsDissolvedCompletely;
			DissolveBlend.Reset();
		}
	}
}

void UCardWidget::NativeOnHovered()
{
	Super::NativeOnHovered();
	SetColorAndOpacity(CardHoveredColor);
}

void UCardWidget::NativeOnUnhovered()
{
	Super::NativeOnUnhovered();
	SetColorAndOpacity(FLinearColor::White);
}

void UCardWidget::NativeOnPressed()
{
	Super::NativeOnPressed();
	SetColorAndOpacity(CardPressedColor);
}
