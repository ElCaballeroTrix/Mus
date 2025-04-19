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
	void SetCardActive(bool _CardActive);
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UImage> CardImage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mus| Images")
	FSlateBrush BackOfCardBrush;

	virtual void NativeOnClicked() override;
	
private:
	FCards_Struct* CardInfo;
	bool CardActive = false;
};
