// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "MusButton.generated.h"

class UCommonTextBlock;
/**
 * 
 */
UCLASS()
class MUSMODERNO_API UMusButton : public UCommonButtonBase
{
	GENERATED_BODY()

public:
	void SetTextOfButton(FText NewText);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> MusButtonTextBlock;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MUS| Info")
	FText TextOfButton;

	virtual void NativePreConstruct() override;
};
