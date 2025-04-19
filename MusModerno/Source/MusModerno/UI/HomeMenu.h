// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HomeMenu.generated.h"

class UButton;
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
	TObjectPtr<UButton> StartGameButton;
};
