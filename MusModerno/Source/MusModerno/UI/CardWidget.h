// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "MusModerno/Data/GroupOfCardsTable.h"
#include "CardWidget.generated.h"

class UImage;
/**
 * 
 */
UCLASS()
class MUSMODERNO_API UCardWidget : public UCommonButtonBase
{
	GENERATED_BODY()

public:
	void SetCard(FCards_Struct* _CardInfo, bool UpsideDown = true);
	void SetCardActive(bool CardActive);
	void DissolveCard();
	bool IsCardToBeDiscarded(){ return DiscardCard; }
	void GiveNewCard(FCards_Struct* _CardInfo, bool UpsideDown = true);
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UImage> CardImage;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UImage> CardImageGlow;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mus| Card Info")
	FSlateBrush BackOfCardBrush;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MUS| Card Info")
	FLinearColor CardHoveredColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MUS| Card Info")
	FLinearColor CardPressedColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MUS| Card Info")
	TObjectPtr<UMaterialInstance> DissolveMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MUS| Card Info")
	float DissolvedCompletely = 0.7f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MUS| Card Info")
	float DissolvedNothing = -0.4f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MUS| Card Info")
	float DissolveTime = 2.0f;
	
	
	virtual void NativeOnClicked() override;
	void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeOnHovered() override;
	virtual void NativeOnUnhovered() override;
	virtual void NativeOnPressed() override;
	
private:
	FCards_Struct* CardInfo;
	//If the player has clicked the card to be discarded, it will be noted for future discard,
	//ones the player clicks the discard button
	bool DiscardCard = false;
	//This "dissolve" material will be used to make a card disappear
	TObjectPtr<UMaterialInstanceDynamic> DynamicMat;
	bool ShouldDissolve = false;
	bool IsDissolvedCompletely = true;
	FAlphaBlend DissolveBlend;
};
