// Fill out your copyright notice in the Description page of Project Settings.


#include "GroupOfCardsTable.h"

void FCards_Struct::OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName)
{
	FTableRowBase::OnDataTableChanged(InDataTable, InRowName);
	
	uint64_t newID = 0;
	TArray<FCards_Struct*> cards;
	InDataTable->GetAllRows(TEXT(""), cards);
	
	for (FCards_Struct* card : cards)
	{
		//Set different id
		card->ID = newID;	
		++newID;
	}

	cards.Empty();
}
