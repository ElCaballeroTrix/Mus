// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "GameOver.generated.h"

class UCommonTextBlock;
class UMusManager;
class UMusButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnExitSignature);
/**
 * 
 */
UCLASS()
class MUSMODERNO_API UGameOver : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnExitSignature OnExitSignature;
	void PlayerWon(bool Value);
	
protected:
	//---------------Widgets----------------------------------//
	UPROPERTY(Transient,meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> FadeInAnimation;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> FinalResultTextBlock;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UMusButton> PlayAgainButton;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UMusButton> ExitButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MUS| Game Over Info")
	FSlateColor WinnerColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MUS| Game Over Info")
	FSlateColor LoserColor;

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void OnAnimationFinished_Implementation(const UWidgetAnimation* Animation) override;
	
private:
	TObjectPtr<UMusManager> MusManager;
	bool FadeIn = true;
	
	UFUNCTION()
	void PlayAgain();
	UFUNCTION()
	void ReturnToHomeMenu();
	
};
