// Fill out your copyright notice in the Description page of Project Settings.


#include "MusTable.h"

#include "CardWidget.h"
#include "CommonButtonBase.h"
#include "CommonTextBlock.h"
#include "EnvidoWidget.h"
#include "MusButton.h"
#include "Components/CanvasPanel.h"
#include "Components/HorizontalBox.h"
#include "MusModerno/Data/MusManager.h"

void UMusTable::NativeConstruct()
{
	Super::NativeConstruct();

	MusManager = Cast<UMusManager>(GetGameInstance());
	//TODO FOR NOW THERE IS NO DISCARD, CHANGE IN FUTURE
	// LeftActionButton.Get()->OnClicked().AddUObject(this, &UMusTable::PlayerCallsMus);
	// LeftActionButton.Get()->SetText(FText::FromString("MUS"));
	LeftActionButton.Get()->OnClicked().AddUObject(this, &UMusTable::PlayersCallsNoMus);
	LeftActionButton.Get()->SetTextOfButton(FText::FromString("MUS"));
	RightActionButton.Get()->OnClicked().AddUObject(this, &UMusTable::PlayersCallsNoMus);
	RightActionButton.Get()->SetTextOfButton(FText::FromString("NO MUS"));
	EnvidoActionButton.Get()->SetVisibility(ESlateVisibility::Hidden);
	EnvidoActionButton.Get()->SetTextOfButton(FText::FromString("ENVIDO"));
	EnvidoActionButton.Get()->OnClicked().AddUObject(this, &UMusTable::PlayersCallsEnvido);
	EnvidoWidget.Get()->SetVisibility(ESlateVisibility::Hidden);
	OrdagoActionButton.Get()->SetVisibility(ESlateVisibility::Hidden);
	OrdagoActionButton.Get()->SetTextOfButton(FText::FromString(TEXT("Órdago")));
	OrdagoActionButton.Get()->OnClicked().AddUObject(this, &UMusTable::PlayersCallsOrdago);

	//Hide Plays
	Bot1PlayText.Get()->SetText(FText::GetEmpty());
	Bot2PlayText.Get()->SetText(FText::GetEmpty());
	Bot3PlayText.Get()->SetText(FText::GetEmpty());
	PlayerPlayText.Get()->SetText(FText::GetEmpty());

	//Obtain Access to all cards of participants
	SetCardsWidgets(PlayerCardsWidget, PLAYER);
	SetCardsWidgets(Bot1CardsWidget, BOT1);
	SetCardsWidgets(Bot2CardsWidget, BOT2);
	SetCardsWidgets(Bot3CardsWidget, BOT3);
}

void UMusTable::NativeDestruct()
{
	Super::NativeDestruct();

	LeftActionButton.Get()->OnClicked().RemoveAll(this);
	RightActionButton.Get()->OnClicked().RemoveAll(this);
	EnvidoActionButton.Get()->OnClicked().RemoveAll(this);
	OrdagoActionButton.Get()->OnClicked().RemoveAll(this);
}

void UMusTable::SetParticipantsCards(EParticipant Participant, TArray<FCards_Struct*> NewCards)
{
	switch (Participant) {
	case PLAYER:
			PlayerCurrentCards[0]->SetCard(NewCards[0], false);
			PlayerCurrentCards[1]->SetCard(NewCards[1], false);
			PlayerCurrentCards[2]->SetCard(NewCards[2], false);
			PlayerCurrentCards[3]->SetCard(NewCards[3], false);
		break;
	case BOT1:
		//TODO Should change to UpsideDown True
			Bot1CurrentCards[0]->SetCard(NewCards[0], false);
			Bot1CurrentCards[1]->SetCard(NewCards[1], false);
			Bot1CurrentCards[2]->SetCard(NewCards[2], false);
			Bot1CurrentCards[3]->SetCard(NewCards[3], false);
		break;
	case BOT2:
			Bot2CurrentCards[0]->SetCard(NewCards[0], false);
			Bot2CurrentCards[1]->SetCard(NewCards[1], false);
			Bot2CurrentCards[2]->SetCard(NewCards[2], false);
			Bot2CurrentCards[3]->SetCard(NewCards[3], false);
		break;
	case BOT3:
			Bot3CurrentCards[0]->SetCard(NewCards[0], false);
			Bot3CurrentCards[1]->SetCard(NewCards[1], false);
			Bot3CurrentCards[2]->SetCard(NewCards[2], false);
			Bot3CurrentCards[3]->SetCard(NewCards[3], false);
		break;
	}
}

void UMusTable::ShowPlayerPossibleActions(TArray<EMoves> PossiblePlayerMoves)
{
	ActionButtonsBox.Get()->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	for (EMoves possibleMove : PossiblePlayerMoves)
	{
		switch (possibleMove)
		{
			case MUS:
					LeftActionButton.Get()->SetTextOfButton(FText::FromString("MUS"));
					LeftActionButton.Get()->SetVisibility(ESlateVisibility::Visible);
					LeftActionButton.Get()->OnClicked().AddUObject(this, &UMusTable::PlayerCallsMus);
				break;
			case NOMUS:
					RightActionButton.Get()->SetTextOfButton(FText::FromString("NO MUS"));
					RightActionButton.Get()->SetVisibility(ESlateVisibility::Visible);
					RightActionButton.Get()->OnClicked().AddUObject(this, &UMusTable::PlayersCallsNoMus);
				break;
			case PASO:
					RightActionButton.Get()->SetTextOfButton(FText::FromString("PASO"));
					RightActionButton.Get()->SetVisibility(ESlateVisibility::Visible);
					RightActionButton.Get()->OnClicked().AddUObject(this, &UMusTable::PlayersPasses);
				break;
			case ENVIDO:
					EnvidoWidget.Get()->SetVisibility(ESlateVisibility::Visible);
					EnvidoActionButton.Get()->SetVisibility(ESlateVisibility::Visible);
				break;
			case QUIERO:
					LeftActionButton.Get()->SetTextOfButton(FText::FromString("QUIERO"));
					LeftActionButton.Get()->SetVisibility(ESlateVisibility::Visible);
					LeftActionButton.Get()->OnClicked().AddUObject(this, &UMusTable::PlayersAcceptsEnvido);
				break;
			case ORDAGO:
					OrdagoActionButton.Get()->SetVisibility(ESlateVisibility::Visible);
				break;
			case SITENGO:
					RightActionButton.Get()->SetTextOfButton(FText::FromString(TEXT("SÍ")));
					RightActionButton.Get()->SetVisibility(ESlateVisibility::Visible);
					RightActionButton.Get()->OnClicked().AddUObject(this, &UMusTable::PlayerHasPairOrJuego);
				break;
			case NOTENGO:
					RightActionButton.Get()->SetTextOfButton(FText::FromString("NO"));
					RightActionButton.Get()->SetVisibility(ESlateVisibility::Visible);
					RightActionButton.Get()->OnClicked().AddUObject(this, &UMusTable::PlayerDoesNotHavePairOrJuego);
				break;
		}
	}
}

void UMusTable::UpdateTableBet(int32 BetValue)
{
	TableBet.Get()->SetText(FText::FromString(FString::Printf(TEXT("%d"), BetValue)));
}

void UMusTable::UpdatePhase(EBettingPhase Phase, bool Winner, EParticipant Participant)
{
	FString phaseText = "";
	FString winner = "";
	if(Winner)
	{
		switch (Participant) {
			case NOONE:
					phaseText = "NADIE ";
				break;
			case BOT1:
					phaseText = "BOT1 ";
				break;
			case BOT2:
					phaseText = "BOT2 ";
				break;
			case BOT3:
					phaseText = "BOT3 ";
				break;
			case PLAYER:
					phaseText = "USTED ";
				break;
		}
		phaseText.Append("gano ");
	}
	switch (Phase) {
		case NONE:
				phaseText.Append(TEXT("¿MUS?"));
			break;
		case GRANDE:
				phaseText.Append("GRANDE");
			break;
		case CHICA:
				phaseText.Append("CHICA");
			break;
		case TIENEPARES:
				phaseText.Append(TEXT("¿PARES?"));
			break;
		case PARES:
				phaseText.Append("PARES");
			break;
		case TIENEJUEGO:
				phaseText.Append(TEXT("¿JUEGO?"));
			break;
		case JUEGO:
				phaseText.Append("JUEGO");
			break;
		case PUNTO:
				phaseText.Append("PUNTO");
			break;
		case RESUMEN:
				phaseText.Append("RESUMEN");
			break;
	}
	PhaseTextBlock.Get()->SetText(FText::FromString(phaseText));
}

void UMusTable::UpdatePiedras(EParticipant Participant, int32 Amarrakos, int32 Piedras)
{
	switch (Participant) {
		case NOONE:
			break;
		case BOT1:
				Bot1Piedras.Get()->SetText(FText::FromString(FString::Printf(TEXT("A: %d, P: %d"), Amarrakos, Piedras)));
			break;
		case BOT2:
				Bot2Piedras.Get()->SetText(FText::FromString(FString::Printf(TEXT("A: %d, P: %d"), Amarrakos, Piedras)));
			break;
		case BOT3:
				Bot3Piedras.Get()->SetText(FText::FromString(FString::Printf(TEXT("A: %d, P: %d"), Amarrakos, Piedras)));
			break;
		case PLAYER:
				PlayerPiedras.Get()->SetText(FText::FromString(FString::Printf(TEXT("A: %d, P: %d"), Amarrakos, Piedras)));
			break;
	}
}

void UMusTable::UpdatePlay(EParticipant Participant, EMoves Move)
{
	//Transform the EMoves to String
	FString playText;
	switch (Move) {
		case MUS:
				playText = "MUS";
			break;
		case NOMUS:
				playText = "NO MUS";
			break;
		case PASO:
				playText = "PASO";
			break;
		case ENVIDO:
				playText = "ENVIDO";
			break;
		case QUIERO:
				playText = "QUIERO";
			break;
		case ORDAGO:
				playText = TEXT("ÓRDAGO");
			break;
		case SITENGO:
				playText = TEXT("SÍ");
			break;
		case NOTENGO:
				playText = "NO";
			break;
	}
	switch (Participant) {
		case BOT1:
				Bot1PlayText.Get()->SetText(FText::FromString(playText));
			break;
		case BOT2:
				Bot2PlayText.Get()->SetText(FText::FromString(playText));
			break;
		case BOT3:
				Bot3PlayText.Get()->SetText(FText::FromString(playText));
			break;
		case PLAYER:
				PlayerPlayText.Get()->SetText(FText::FromString(playText));
			break;
	}
}

void UMusTable::ResetPlays()
{
	Bot1PlayText.Get()->SetText(FText::GetEmpty());
	Bot2PlayText.Get()->SetText(FText::GetEmpty());
	Bot3PlayText.Get()->SetText(FText::GetEmpty());
	// PlayerPlayText.Get()->SetText(FText::GetEmpty());
}

void UMusTable::SetCardsWidgets(TObjectPtr<UCanvasPanel> ParticipantCardsWidget, EParticipant Participant)
{
	for (TObjectPtr<UWidget> cardWidget : ParticipantCardsWidget.Get()->GetAllChildren())
	{
		TObjectPtr<UCardWidget> card = Cast<UCardWidget>(cardWidget);
		if(card)
		{
			switch (Participant)
			{
			case PLAYER:
				PlayerCurrentCards.Add(card);
				break;
			case BOT1:
				Bot1CurrentCards.Add(card);
				break;
			case BOT2:
				Bot2CurrentCards.Add(card);
				break;
			case BOT3:
				Bot3CurrentCards.Add(card);
				break;
			}
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------Mus Actions----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------

void UMusTable::PlayerCallsMus()
{
	HideAllButtons();
	MusManager.Get()->ParticipantCallsAMove(EParticipant::PLAYER, EMoves::MUS);
	UpdatePlay(PLAYER, MUS);
}

void UMusTable::PlayersCallsNoMus()
{
	HideAllButtons();
	MusManager.Get()->ParticipantCallsAMove(EParticipant::PLAYER, EMoves::NOMUS);
	UpdatePlay(PLAYER, NOMUS);
}

void UMusTable::PlayersCallsEnvido()
{
	HideAllButtons();
	MusManager.Get()->ParticipantCallsAMove(EParticipant::PLAYER, EMoves::ENVIDO,  MusManager.Get()->GetCurrentBetOnTable() + EnvidoWidget.Get()->GetEnvidio());
	UpdatePlay(PLAYER, ENVIDO);
}

void UMusTable::PlayersAcceptsEnvido()
{
	HideAllButtons();
	MusManager.Get()->ParticipantCallsAMove(EParticipant::PLAYER, EMoves::QUIERO,  MusManager.Get()->GetCurrentBetOnTable());
	UpdatePlay(PLAYER, QUIERO);
}

void UMusTable::PlayersPasses()
{
	HideAllButtons();
	MusManager.Get()->ParticipantCallsAMove(EParticipant::PLAYER, EMoves::PASO,  0);
	UpdatePlay(PLAYER, PASO);
}

void UMusTable::PlayerHasPairOrJuego()
{
	HideAllButtons();
	MusManager.Get()->ParticipantCallsAMove(EParticipant::PLAYER, EMoves::SITENGO,  0);
	UpdatePlay(PLAYER, SITENGO);
}

void UMusTable::PlayerDoesNotHavePairOrJuego()
{
	HideAllButtons();
	MusManager.Get()->ParticipantCallsAMove(EParticipant::PLAYER, EMoves::NOTENGO,  0);
	UpdatePlay(PLAYER, NOTENGO);
}

void UMusTable::PlayersCallsOrdago()
{
	HideAllButtons();
	int32 allPiedras;
	MusManager.Get()->GetCurrentMusRules().ObjectiveIs8Amarrakos ? allPiedras = 40 : allPiedras = 30; 
	MusManager.Get()->ParticipantCallsAMove(EParticipant::PLAYER, EMoves::ORDAGO,  allPiedras);
	UpdatePlay(PLAYER, ORDAGO);
}

void UMusTable::HideAllButtons()
{
	ActionButtonsBox.Get()->SetVisibility(ESlateVisibility::Hidden);
	LeftActionButton.Get()->SetVisibility(ESlateVisibility::Collapsed);
	RightActionButton.Get()->SetVisibility(ESlateVisibility::Collapsed);
	EnvidoActionButton.Get()->SetVisibility(ESlateVisibility::Hidden);
	EnvidoWidget.Get()->SetVisibility(ESlateVisibility::Hidden);
	OrdagoActionButton.Get()->SetVisibility(ESlateVisibility::Hidden);
	LeftActionButton.Get()->OnClicked().RemoveAll(this);
	RightActionButton.Get()->OnClicked().RemoveAll(this);
	// ResetPlays();
}

