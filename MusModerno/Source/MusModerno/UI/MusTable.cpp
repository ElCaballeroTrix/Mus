// Fill out your copyright notice in the Description page of Project Settings.


#include "MusTable.h"

#include "CardWidget.h"
#include "CommonButtonBase.h"
#include "CommonTextBlock.h"
#include "EnvidoWidget.h"
#include "GameOver.h"
#include "HomeMenu.h"
#include "MusButton.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "Components/VerticalBox.h"
#include "MusModerno/Data/MusManager.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

void UMusTable::NativeConstruct()
{
	Super::NativeConstruct();

	MusManager = Cast<UMusManager>(GetGameInstance());
	PhaseTextBlock.Get()->SetText(FText::GetEmpty());
	TableBet.Get()->SetText(FText::GetEmpty());
	MusActionButton.Get()->SetTextOfButton(FText::FromString("MUS"));
	MusActionButton.Get()->OnClicked().AddUObject(this, &UMusTable::PlayerCallsMus);
	NoMusActionButton.Get()->SetTextOfButton(FText::FromString("NO MUS"));
	NoMusActionButton.Get()->OnClicked().AddUObject(this, &UMusTable::PlayerCallsNoMus);
	EnvidoActionButton.Get()->SetTextOfButton(FText::FromString("ENVIDO"));
	EnvidoActionButton.Get()->OnClicked().AddUObject(this, &UMusTable::PlayerCallsEnvido);
	EnvidoWidget.Get()->SetVisibility(ESlateVisibility::Hidden);
	OrdagoActionButton.Get()->SetTextOfButton(FText::FromString(TEXT("Órdago")));
	OrdagoActionButton.Get()->OnClicked().AddUObject(this, &UMusTable::PlayersCallsOrdago);
	HideAllButtons();

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

	MusActionButton.Get()->OnClicked().RemoveAll(this);
	NoMusActionButton.Get()->OnClicked().RemoveAll(this);
	UpperActionButton.Get()->OnClicked().RemoveAll(this);
	LowerActionButton.Get()->OnClicked().RemoveAll(this);
	EnvidoActionButton.Get()->OnClicked().RemoveAll(this);
	OrdagoActionButton.Get()->OnClicked().RemoveAll(this);
}

void UMusTable::SetParticipantsCards(EParticipant Participant, TArray<FCards_Struct*> NewCards)
{
	switch (Participant) {
		case PLAYER:
				PlayerCurrentCards[0]->ResetDissolve();
				PlayerCurrentCards[0]->SetCard(NewCards[0], false);
				PlayerCurrentCards[1]->ResetDissolve();
				PlayerCurrentCards[1]->SetCard(NewCards[1], false);
				PlayerCurrentCards[2]->ResetDissolve();
				PlayerCurrentCards[2]->SetCard(NewCards[2], false);
				PlayerCurrentCards[3]->ResetDissolve();
				PlayerCurrentCards[3]->SetCard(NewCards[3], false);
			break;
		case BOT1:
			//TODO Should change to UpsideDown True
				Bot1CurrentCards[0]->ResetDissolve();
				Bot1CurrentCards[0]->SetCard(NewCards[0], MusManager.Get()->ShouldBotCardsBeInvisible);
				Bot1CurrentCards[1]->ResetDissolve();
				Bot1CurrentCards[1]->SetCard(NewCards[1], MusManager.Get()->ShouldBotCardsBeInvisible);
				Bot1CurrentCards[2]->ResetDissolve();
				Bot1CurrentCards[2]->SetCard(NewCards[2], MusManager.Get()->ShouldBotCardsBeInvisible);
				Bot1CurrentCards[3]->ResetDissolve();
				Bot1CurrentCards[3]->SetCard(NewCards[3], MusManager.Get()->ShouldBotCardsBeInvisible);
			break;
		case BOT2:
				Bot2CurrentCards[0]->ResetDissolve();
				Bot2CurrentCards[0]->SetCard(NewCards[0], MusManager.Get()->ShouldBotCardsBeInvisible);
				Bot2CurrentCards[1]->ResetDissolve();
				Bot2CurrentCards[1]->SetCard(NewCards[1], MusManager.Get()->ShouldBotCardsBeInvisible);
				Bot2CurrentCards[2]->ResetDissolve();
				Bot2CurrentCards[2]->SetCard(NewCards[2], MusManager.Get()->ShouldBotCardsBeInvisible);
				Bot2CurrentCards[3]->ResetDissolve();
				Bot2CurrentCards[3]->SetCard(NewCards[3], MusManager.Get()->ShouldBotCardsBeInvisible);
			break;
		case BOT3:
				Bot3CurrentCards[0]->ResetDissolve();
				Bot3CurrentCards[0]->SetCard(NewCards[0], MusManager.Get()->ShouldBotCardsBeInvisible);
				Bot3CurrentCards[1]->ResetDissolve();
				Bot3CurrentCards[1]->SetCard(NewCards[1], MusManager.Get()->ShouldBotCardsBeInvisible);
				Bot3CurrentCards[2]->ResetDissolve();
				Bot3CurrentCards[2]->SetCard(NewCards[2], MusManager.Get()->ShouldBotCardsBeInvisible);
				Bot3CurrentCards[3]->ResetDissolve();
				Bot3CurrentCards[3]->SetCard(NewCards[3], MusManager.Get()->ShouldBotCardsBeInvisible);
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
					MusActionButton.Get()->SetVisibility(ESlateVisibility::Visible);
				break;
			case NOMUS:
					NoMusActionButton.Get()->SetVisibility(ESlateVisibility::Visible);
				break;
			case DISCARD:
					LowerActionButton.Get()->SetTextOfButton(FText::FromString("DESCARTE"));
					LowerActionButton.Get()->SetVisibility(ESlateVisibility::Visible);
					LowerActionButton.Get()->OnClicked().AddUObject(this, &UMusTable::PlayersCallsDiscard);
				break;
			case PASO:
					LowerActionButton.Get()->SetTextOfButton(FText::FromString("PASO"));
					LowerActionButton.Get()->SetVisibility(ESlateVisibility::Visible);
					LowerActionButton.Get()->OnClicked().AddUObject(this, &UMusTable::PlayerPasses);
				break;
			case ENVIDO:
					EnvidoWidget.Get()->SetVisibility(ESlateVisibility::Visible);
					EnvidoActionButton.Get()->SetVisibility(ESlateVisibility::Visible);
				break;
			case QUIERO:
					UpperActionButton.Get()->SetTextOfButton(FText::FromString("QUIERO"));
					UpperActionButton.Get()->SetVisibility(ESlateVisibility::Visible);
					UpperActionButton.Get()->OnClicked().AddUObject(this, &UMusTable::PlayerAcceptsEnvido);
				break;
			case NOQUIERO:
					LowerActionButton.Get()->SetTextOfButton(FText::FromString("NO QUIERO"));
					LowerActionButton.Get()->SetVisibility(ESlateVisibility::Visible);
					LowerActionButton.Get()->OnClicked().AddUObject(this, &UMusTable::PlayerPasses);
				break;
			case ORDAGO:
					OrdagoActionButton.Get()->SetVisibility(ESlateVisibility::Visible);
				break;
			case SITENGO:
					LowerActionButton.Get()->SetTextOfButton(FText::FromString(TEXT("SÍ")));
					LowerActionButton.Get()->SetVisibility(ESlateVisibility::Visible);
					LowerActionButton.Get()->OnClicked().AddUObject(this, &UMusTable::PlayerHasPairOrJuego);
				break;
			case NOTENGO:
					LowerActionButton.Get()->SetTextOfButton(FText::FromString("NO"));
					LowerActionButton.Get()->SetVisibility(ESlateVisibility::Visible);
					LowerActionButton.Get()->OnClicked().AddUObject(this, &UMusTable::PlayerDoesNotHavePairOrJuego);
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
		case ORDAGOWINNER:
			phaseText.Append(TEXT("ÓRDAGO"));
		break;
	}
	PhaseTextBlock.Get()->SetText(FText::FromString(phaseText));
}

void UMusTable::EmptyPhaseText()
{
	PhaseTextBlock.Get()->SetText(FText::GetEmpty());
	TableBet.Get()->SetText(FText::GetEmpty());
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
		case NOMOVE:
				playText = "";
			break;
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
		case NOQUIERO:
				playText = "NO QUIERO";
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
	PlayerPlayText.Get()->SetText(FText::GetEmpty());
}

void UMusTable::ParticipantsCardsDiscard(EParticipant Participant, TArray<int32> CardsToDiscard,
	bool IsInDiscardingPhase)
{
	ParticipantDiscarting = Participant;
	for (int32 i = 0; i < CardsToDiscard.Num(); i++)
	{
		switch (Participant) {
			case PLAYER:
					PlayerCurrentCards[CardsToDiscard[i]].Get()->DissolveCard();
				break;
			case BOT1:
					Bot1CurrentCards[CardsToDiscard[i]].Get()->DissolveCard();
				break;
			case BOT2:
					Bot2CurrentCards[CardsToDiscard[i]].Get()->DissolveCard();
				break;
			case BOT3:
					Bot3CurrentCards[CardsToDiscard[i]].Get()->DissolveCard();
				break;
		}
	}
	if(IsInDiscardingPhase)
	{
		WaitUntilDiscardAnimationIsComplete();
	}
}

void UMusTable::ActivePlayerCardsForDiscard()
{
	for ( int32 i = 0 ; i < PlayerCurrentCards.Num(); i++)
	{
		PlayerCurrentCards[i].Get()->SetCardActive(true);
	}
}

void UMusTable::UpdateHand(EParticipant ParticipantHand)
{
	Bot1Hand.Get()->SetVisibility(ESlateVisibility::Hidden);
	Bot2Hand.Get()->SetVisibility(ESlateVisibility::Hidden);
	Bot3Hand.Get()->SetVisibility(ESlateVisibility::Hidden);
	PlayerHand.Get()->SetVisibility(ESlateVisibility::Hidden);
	
	switch (ParticipantHand)
	{
		case BOT1:
				Bot1Hand.Get()->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			break;
		case BOT2:
				Bot2Hand.Get()->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			break;
		case BOT3:
				Bot3Hand.Get()->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			break;
		case PLAYER:
				PlayerHand.Get()->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			break;
	}
}

void UMusTable::StandOutACard(EParticipant Participant, int32 IDOfCard, bool ShouldStandOut)
{
	switch (Participant) {
		case BOT1:
				Bot1CurrentCards[IDOfCard].Get()->GlowCard(ShouldStandOut);
			break;
		case BOT2:
				Bot2CurrentCards[IDOfCard].Get()->GlowCard(ShouldStandOut);
			break;
		case BOT3:
				Bot3CurrentCards[IDOfCard].Get()->GlowCard(ShouldStandOut);
			break;
		case PLAYER:
				PlayerCurrentCards[IDOfCard].Get()->GlowCard(ShouldStandOut);
			break;
	}

}

void UMusTable::ResetGlowOfAllCards()
{
	for (int32 i = 0; i < 4; i++)
	{
		Bot1CurrentCards[i].Get()->GlowCard(false);
		Bot2CurrentCards[i].Get()->GlowCard(false);
		Bot3CurrentCards[i].Get()->GlowCard(false);
		PlayerCurrentCards[i].Get()->GlowCard(false);
	}
}

void UMusTable::ShowAllCards()
{
	for (int32 i = 0; i < 4; i++)
	{
		Bot1CurrentCards[i].Get()->FlipCard();
		Bot2CurrentCards[i].Get()->FlipCard();
		Bot3CurrentCards[i].Get()->FlipCard();
	}
}

void UMusTable::GameEnded(bool PlayerWon)
{
	GameOver = Cast<UGameOver>(TableStack.Get()->AddWidget(GameOverClass));
	GameOver.Get()->OnExitSignature.AddDynamic(this, &UMusTable::ReturnToHomeMenu);
	GameOver.Get()->PlayerWon(PlayerWon);
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

void UMusTable::PlayerCallsNoMus()
{
	HideAllButtons();
	MusManager.Get()->ParticipantCallsAMove(EParticipant::PLAYER, EMoves::NOMUS);
	UpdatePlay(PLAYER, NOMUS);
}

void UMusTable::PlayerCallsEnvido()
{
	HideAllButtons();
	MusManager.Get()->ParticipantCallsAMove(EParticipant::PLAYER, EMoves::ENVIDO,  MusManager.Get()->GetCurrentBetOnTable() + EnvidoWidget.Get()->GetEnvidio());
	UpdatePlay(PLAYER, ENVIDO);
}

void UMusTable::PlayerAcceptsEnvido()
{
	HideAllButtons();
	MusManager.Get()->ParticipantCallsAMove(EParticipant::PLAYER, EMoves::QUIERO,  MusManager.Get()->GetCurrentBetOnTable());
	UpdatePlay(PLAYER, QUIERO);
}

void UMusTable::PlayerDeniesEnvido()
{
	HideAllButtons();
	MusManager.Get()->ParticipantCallsAMove(EParticipant::PLAYER, EMoves::NOQUIERO,  0);
	UpdatePlay(PLAYER, NOQUIERO);
}

void UMusTable::PlayerPasses()
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

void UMusTable::PlayersCallsDiscard()
{
	ParticipantDiscarting = PLAYER;
	HideAllButtons();
	TArray<int32> playerCardsToDiscard;
	for ( int32 i = 0 ; i < PlayerCurrentCards.Num(); i++)
	{
		if(PlayerCurrentCards[i].Get()->IsCardToBeDiscarded())
		{
			playerCardsToDiscard.Add(i);
			PlayerCurrentCards[i].Get()->DissolveCard();
		}
		else
		{
			PlayerCurrentCards[i].Get()->SetCardActive(false);
		}
	}
	MusManager.Get()->DiscardParticipantCards(ParticipantDiscarting, playerCardsToDiscard);
	WaitUntilDiscardAnimationIsComplete();
}

void UMusTable::HideAllButtons()
{
	MusActionButton.Get()->SetVisibility(ESlateVisibility::Hidden);
	NoMusActionButton.Get()->SetVisibility(ESlateVisibility::Hidden);
	ActionButtonsBox.Get()->SetVisibility(ESlateVisibility::Hidden);
	UpperActionButton.Get()->SetVisibility(ESlateVisibility::Collapsed);
	LowerActionButton.Get()->SetVisibility(ESlateVisibility::Collapsed);
	EnvidoActionButton.Get()->SetVisibility(ESlateVisibility::Hidden);
	EnvidoWidget.Get()->SetVisibility(ESlateVisibility::Hidden);
	OrdagoActionButton.Get()->SetVisibility(ESlateVisibility::Hidden);
	UpperActionButton.Get()->OnClicked().RemoveAll(this);
	LowerActionButton.Get()->OnClicked().RemoveAll(this);
	// ResetPlays();
}


void UMusTable::WaitUntilDiscardAnimationIsComplete()
{
	GetWorld()->GetTimerManager().SetTimer(
				DiscardTimerHandle, // handle to cancel timer at a later time
				this, // the owning object
				&UMusTable::DiscardOfParticipantFinished, // function to call on elapsed
				MusManager.Get()->CardsShowingAnimationTime, // float delay until elapsed
			false);
}

void UMusTable::DiscardOfParticipantFinished()
{
	MusManager.Get()->ParticipantCallsAMove(ParticipantDiscarting, DISCARD,  0);
}

void UMusTable::ReturnToHomeMenu()
{
	GameOver.Get()->OnExitSignature.RemoveAll(this);
	CreateWidget(GetWorld(), HomeMenuClass)->AddToViewport();
	RemoveFromParent();
}

