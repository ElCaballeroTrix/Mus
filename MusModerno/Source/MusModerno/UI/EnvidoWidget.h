// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "EnvidoWidget.generated.h"

class UCommonTextBlock;
class UMusButton;
/**
 * 
 */
UCLASS()
class MUSMODERNO_API UEnvidoWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	int32 GetEnvidio(){return Envidio;}
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UMusButton> MinusButton;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UMusButton> PlusButton;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> EnvidioTextBlock;

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
private:
	int32 Envidio = 2;
	UFUNCTION()
	void ReduceEnvidio();
	UFUNCTION()
	void AddEnvidio();
};
