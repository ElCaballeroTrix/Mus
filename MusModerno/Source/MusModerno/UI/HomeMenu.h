// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HomeMenu.generated.h"

class UOptions;
class UMusManager;
class UCommonActivatableWidgetStack;
class UMusButton;
/**
 *
 * Is this menu the player will decide the rules of the game as well as configure
 * the music and brightness of game
 */
UCLASS()
class MUSMODERNO_API UHomeMenu : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UCommonActivatableWidgetStack> HomeMenuStack;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UMusButton> StartGameButton;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UMusButton> OptionsButton;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UMusButton> ExitButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MUS| Home Menu Classes")
	TSubclassOf<UOptions> OptionsClass;

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
private:
	TObjectPtr<UMusManager> MusManager;
	
	UFUNCTION()
	void StartGame();
	UFUNCTION()
	void ShowOptions();
	UFUNCTION()
	void ExitGame();
};
