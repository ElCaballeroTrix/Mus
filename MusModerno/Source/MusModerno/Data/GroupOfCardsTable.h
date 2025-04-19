// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GroupOfCardsTable.generated.h"

/**
 * 
 */
//Weapons
UENUM(BlueprintType)
enum ECardType
{
	OROS = 0,
	COPAS = 1,
	ESPADAS = 2,
	BASTOS = 3
};

USTRUCT(BlueprintType, Blueprintable)
struct FCards_Struct : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	int64 ID = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int64 Number;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<ECardType> CardType = OROS;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlateBrush CardBrush;

	//This "value" depends on the rules set and the number of the card
	int64 CardValue = 1;

	virtual void OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName) override;
};

UCLASS()
class MUSMODERNO_API UGroupOfCardsTable : public UDataTable
{
	GENERATED_BODY()
};
