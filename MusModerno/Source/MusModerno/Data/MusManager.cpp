// Fill out your copyright notice in the Description page of Project Settings.


#include "MusManager.h"

#include "BotAI.h"
#include "GroupOfCardsTable.h"
#include "Algo/RandomShuffle.h"
#include "Blueprint/UserWidget.h"
#include "MusModerno/UI/MusTable.h"

#define NUMBER_OF_MUS_CARDS 40

void UMusManager::OnStart()
{
	Super::OnStart();

	//TODO The order will depend on first card given to player.
	OrderOfParticipantsInGame.Add(EParticipant::PLAYER);
	OrderOfParticipantsInGame.Add(EParticipant::BOT1);
	OrderOfParticipantsInGame.Add(EParticipant::BOT2);
	OrderOfParticipantsInGame.Add(EParticipant::BOT3);
	//TODO REMOVE
	SetValueOfCards();
	StartGame();
}

void UMusManager::ParticipantCallsAMove(EParticipant Participant, EMoves Move, int32 EnvidoRocks)
{
	//First set the envido rocks the participant has bet
	ParticipantsInfo[Participant].LastMove = Move;
	ParticipantsInfo[Participant].EnvidoValue = EnvidoRocks;
	//Add the participant or remove it from the player betting list
	switch (Move)
	{
		case SITENGO:
				if(!ParticipantsInTheCurrentBet.Contains(Participant))
				{
					ParticipantsInTheCurrentBet.Add(Participant);
				}
			break;
		case ENVIDO:
				if(!ParticipantsInTheCurrentBet.Contains(Participant))
				{
					ParticipantsInTheCurrentBet.Add(Participant);
				}
				CurrentBetOnTable = EnvidoRocks;
				MusTable.Get()->UpdateTableBet(CurrentBetOnTable);
				ParticipantThatRaisedTheBet = CurrentParticipantIDInTurn;
			break;
		case NOTENGO:
				ParticipantsInfo[Participant].LastMove = PASO;
		case PASO:
				if(ParticipantsInTheCurrentBet.Contains(Participant))
				{
					ParticipantsInTheCurrentBet.Remove(Participant);
				}
			break;
	}
	
	//If in betting phase
	if(BetsStarted)
	{
		int32 nextParticipantID = CurrentParticipantIDInTurn + 1;
		if(nextParticipantID == OrderOfParticipantsInGame.Num())
		{ 
			nextParticipantID = 0;	
		}
		//If the next player is the "Hand" and no one raised the bet, check who won that round
		if(nextParticipantID == ParticipantThatRaisedTheBet)
		{
			bool someoneBetDifferent = false;
			bool noOneMadeABet = false;
			//Did someone make a bet?
			if(!ParticipantsInTheCurrentBet.IsEmpty())
			{
				int32 highestBet = 0;
				highestBet = ParticipantsInfo[ParticipantsInTheCurrentBet[0]].EnvidoValue;
				for ( int32 i = 1; i < ParticipantsInTheCurrentBet.Num(); i++)
				{
					if( ParticipantsInfo[ParticipantsInTheCurrentBet[i]].EnvidoValue != highestBet)
					{
						someoneBetDifferent = true;
					}
				}
				if(someoneBetDifferent)
				{
					PassTurn();
					return;
				}
			}
			//In all betting round except "Pares" there is always winner, so add all participants into the battle
			//if there were no bets.
			else if(CurrentBettingPhase != TIENEPARES)
			{
				noOneMadeABet = true;
				for (EParticipant participant : OrderOfParticipantsInGame)
				{
					ParticipantsInTheCurrentBet.Add(participant);
				}
			}
			//Check the plays
			switch (CurrentBettingPhase) {
				case GRANDE:
						WinnerOfGrande = CheckGrande();
						if(noOneMadeABet || ParticipantsInTheCurrentBet.Num() == 1)
						{
							GrandeAmountWon = 1;
						}
						else
						{
							GrandeAmountWon = CurrentBetOnTable;
						}
						ResetBotEnvidos();
						CurrentBettingPhase = CHICA;
					break;
				case CHICA:
						WinnerOfChica = CheckChica();
						if(noOneMadeABet || ParticipantsInTheCurrentBet.Num() == 1)
						{
							ChicaAmountWon = 1;
						}
						else
						{
							ChicaAmountWon = CurrentBetOnTable;
						}
						ResetBotEnvidos();
						CurrentBettingPhase = TIENEPARES;
						//Look who has a pair or higher
						for (EParticipant participant : OrderOfParticipantsInGame)
						{
							for (int32 i = 1; i < 13; i++)
							{
								//Check if participant has a pair or higher
								if(ParticipantsInfo[participant].ParticipantCardFrequency[i] >= 2)
								{
									ParticipantsThatHavePares.Add(participant);
									break;
								}
								//Jump from 7 to 10
								if(i == 7)
								{
									i = 9;
								}
							}
						}
					break;
				case TIENEPARES:
						//If no one has a pair or better, pass round, otherwise detect the "PARES" winner
						if(!noOneMadeABet && ParticipantsInTheCurrentBet.Num() > 1)
						{
							CurrentBettingPhase = PARES;
						}
						else if(ParticipantsInTheCurrentBet.Num() == 1)
						{
							//If only one participant has "PARES", let the game detect the winner and how
							//many "piedras" it won. After that, go to "TIENEJUEGO" round
							DetectWinnerOfParesAndPrepareForJuego();
						}
						else
						{
							CurrentBettingPhase = TIENEJUEGO;
						}
					break;
				case PARES:
						DetectWinnerOfParesAndPrepareForJuego();
					break;
				case TIENEJUEGO:
						//If no one has a "JUEGO", go to "PUNTO" round. If only one participant has "JUEGO",
						//let the game detect the winner and how many "piedras it won"
						if(!noOneMadeABet && ParticipantsInTheCurrentBet.Num() > 1)
						{
							CurrentBettingPhase = JUEGO;
						}
						else if(ParticipantsInTheCurrentBet.Num() == 1)
						{
							DetectWinnerOfJuego();
						}
						else
						{
							CurrentBettingPhase = PUNTO;
						}
					break;
				case JUEGO:
						DetectWinnerOfJuego();
					break;
				case PUNTO:
						if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::White, FString::Printf(TEXT("Winner Of GRANDE: %d"), WinnerOfGrande));}
						if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Orange, FString::Printf(TEXT("Winner Of CHICA: %d"), WinnerOfChica));}
						JuegoWentToPunto = true;		
						WinnerOfJuego = CheckPunto();
						JuegoAmountWon += CurrentBetOnTable;
						if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("Winner Of JUEGO: %d"), WinnerOfJuego));}
						CurrentBettingPhase = RESUMEN;
						CurrentParticipantIDInTurn = 0;
						ParticipantsThatHaveJuego.Empty();
						ResetBotEnvidos();
						ShowWinners();
					break;
			}
			ParticipantThatRaisedTheBet = 0;
			CurrentBetOnTable = 0;
			ParticipantsInTheCurrentBet.Empty();
			MusTable.Get()->UpdateTableBet(CurrentBetOnTable);
			MusTable.Get()->UpdatePhase(CurrentBettingPhase);
			//Reset participants last move if not "PARES" or "JUEGO" phase, because, previously, some participants
			//might have passed because they didn't have a pair or a juego
			if(CurrentBettingPhase != PARES && CurrentBettingPhase != JUEGO)
			{
				for(TTuple<EParticipant, FParticipantStruct> participant : ParticipantsInfo)
				{
					ParticipantsInfo[participant.Key].EnvidoValue = 0;
					ParticipantsInfo[participant.Key].LastMove = NOMOVE;
				}
			}
			if(CurrentBettingPhase != RESUMEN)
			{
				//Start next betting round with the Hand
				CurrentParticipantIDInTurn = 0;	
				StartParticipantAction();
			}
		}
		else
		{
			PassTurn();
		}
	}
	//If "MUS", add number of participants that want "MUS".
	//If all participants say "MUS", start "DISCARD" phase
	else if(Move == MUS)
	{
		++AmountOfParticipantsThatWantMus;
		if(AmountOfParticipantsThatWantMus == OrderOfParticipantsInGame.Num())
		{
			//Start "DISCARD" phase
			AmountOfParticipantsThatWantMus = 0;
			InDiscardPhase = true;
		}
		PassTurn();
	}
	//If "DISCARD", pass turn
	else if(Move == DISCARD)
	{
		++AmountOfParticipantsThatDiscarded;
		if(AmountOfParticipantsThatDiscarded == 4)
		{
			//Give new cards to participants
			AmountOfParticipantsThatDiscarded = 0;
			InDiscardPhase = false;
			GiveNewCardsToParticipants();
		}
		else
		{
			PassTurn();
		}
	}
	//If "No MUS", start the Betting Phase
	else if(Move == NOMUS)
	{
		AmountOfParticipantsThatWantMus = 0;
		BetsStarted = true;
		CurrentParticipantIDInTurn = 0;
		CurrentBettingPhase = GRANDE;
		MusTable.Get()->UpdatePhase(CurrentBettingPhase);
		//TODO Check Frequency should only be called when new cards given and after discard
		CheckFrequencyOfCardsInHand();
		ParticipantsAllCards.Empty();
		ParticipantsAllCards.Add(BOT1, ParticipantsInfo[BOT1].ParticipantCardFrequency);
		ParticipantsAllCards.Add(BOT2, ParticipantsInfo[BOT2].ParticipantCardFrequency);
		ParticipantsAllCards.Add(BOT3, ParticipantsInfo[BOT3].ParticipantCardFrequency);
		ParticipantsAllCards.Add(PLAYER, ParticipantsInfo[PLAYER].ParticipantCardFrequency);
		//When "MUS, NOMUS" round is over, all bots should examine their cards
		for (EParticipant participant : OrderOfParticipantsInGame)
		{
			if(ParticipantsInfo[participant].BotAI)
			{
				ParticipantsInfo[participant].BotAI->ExamineCards();
			}
		}
		StartParticipantAction();
	}
	else
	{
		PassTurn();	
	}
}

void UMusManager::SetValueOfCards()
{
	TArray<FCards_Struct*> oroCards;
	OrosTable.Get()->GetAllRows(TEXT(""), oroCards);
	for (FCards_Struct* oroCard : oroCards)
	{
		AddCardToDeck(oroCard);
	}
	oroCards.Empty();
	
	TArray<FCards_Struct*> copasCards;
	CopasTable.Get()->GetAllRows(TEXT(""), copasCards);
	for (FCards_Struct* copasCard : copasCards)
	{
		AddCardToDeck(copasCard);
	}
	copasCards.Empty();
	
	TArray<FCards_Struct*> espadasCards;
	EspadasTable.Get()->GetAllRows(TEXT(""), espadasCards);
	for (FCards_Struct* espadasCard : espadasCards)
	{
		AddCardToDeck(espadasCard);
	}
	espadasCards.Empty();
	
	TArray<FCards_Struct*> bastosCards;
	BastosTable.Get()->GetAllRows(TEXT(""), bastosCards);
	for (FCards_Struct* bastosCard : bastosCards)
	{
		AddCardToDeck(bastosCard);
	}
	bastosCards.Empty();
}

void UMusManager::AddCardToDeck(FCards_Struct* Card)
{
	//Kings, horses and jacks count as 10
	if(Card->Number == 11 || Card->Number == 12)
	{
		Card->CardValue = 10;
	}
	else
	{
		Card->CardValue = Card->Number;
	}
	//Depends on rules
	if(Card->Number == 3 && CurrentMusRules.KingsAndAces8)
	{
		Card->CardValue = 10;
	}
	else if(Card->Number == 2 && CurrentMusRules.KingsAndAces8)
	{
		Card->CardValue = 1;
	}
	GameCards.Add(Card);
}

void UMusManager::StartGame()
{
	Bot1.ParticipantName = BOT1;
	Bot1.BotAI = new BotAI(this, BOT1);
	ParticipantsInfo.Add(BOT1, Bot1);

	Bot2.ParticipantName = BOT2;
	Bot2.BotAI = new BotAI(this, BOT2);
	ParticipantsInfo.Add(BOT2, Bot2);

	Bot3.ParticipantName = BOT3;
	Bot3.BotAI = new BotAI(this, BOT3);
	ParticipantsInfo.Add(BOT3, Bot3);

	Player.ParticipantName = PLAYER;
	ParticipantsInfo.Add(PLAYER, Player);
	
	MusTable = Cast<UMusTable>(CreateWidget(GetWorld(), MusTableClass));
	MusTable->AddToViewport();
	
	ShuffleAndGiveCards();
	//TODO Remove comment
	// Algo::RandomShuffle(OrderOfParticipantsInGame);
}

void UMusManager::ShuffleAndGiveCards()
{
	//TODO Add animation
	Algo::RandomShuffle(GameCards);
	ParticipantsInfo[BOT1].ParticipantCards.Add(GameCards[0]);
	ParticipantsInfo[BOT1].ParticipantCards.Add(GameCards[1]);
	ParticipantsInfo[BOT1].ParticipantCards.Add(GameCards[2]);
	ParticipantsInfo[BOT1].ParticipantCards.Add(GameCards[3]);
	
	ParticipantsInfo[BOT2].ParticipantCards.Add(GameCards[4]);
	ParticipantsInfo[BOT2].ParticipantCards.Add(GameCards[5]);
	ParticipantsInfo[BOT2].ParticipantCards.Add(GameCards[6]);
	ParticipantsInfo[BOT2].ParticipantCards.Add(GameCards[7]);
	
	ParticipantsInfo[BOT3].ParticipantCards.Add(GameCards[8]);
	ParticipantsInfo[BOT3].ParticipantCards.Add(GameCards[9]);
	ParticipantsInfo[BOT3].ParticipantCards.Add(GameCards[10]);
	ParticipantsInfo[BOT3].ParticipantCards.Add(GameCards[11]);
	
	ParticipantsInfo[PLAYER].ParticipantCards.Add(GameCards[12]);
	ParticipantsInfo[PLAYER].ParticipantCards.Add(GameCards[13]);
	ParticipantsInfo[PLAYER].ParticipantCards.Add(GameCards[14]);
	ParticipantsInfo[PLAYER].ParticipantCards.Add(GameCards[15]);

	CurrentGameCardIndexInDeck = 16;

	MusTable.Get()->SetParticipantsCards(BOT1, ParticipantsInfo[BOT1].ParticipantCards);
	MusTable.Get()->SetParticipantsCards(BOT2, ParticipantsInfo[BOT2].ParticipantCards);
	MusTable.Get()->SetParticipantsCards(BOT3, ParticipantsInfo[BOT3].ParticipantCards);
	MusTable.Get()->SetParticipantsCards(PLAYER, ParticipantsInfo[PLAYER].ParticipantCards);
	
	CheckFrequencyOfCardsInHand();
	GetWorld()->GetTimerManager().SetTimer(
				StartGameHandle, // handle to cancel timer at a later time
				this, // the owning object
				&UMusManager::EndStartingGame, // function to call on elapsed
				CardsShowingAnimationTime, // float delay until elapsed
			false);
}

void UMusManager::EndStartingGame()
{
	MusTable.Get()->UpdatePhase(NONE);
	//TODO this should not be PLAYER, it depende on who is the hand
	TArray<EMoves> playerPossibleMoves;
	playerPossibleMoves.Add(MUS);
	playerPossibleMoves.Add(NOMUS);	
	MusTable.Get()->ShowPlayerPossibleActions(playerPossibleMoves);
	MusTable.Get()->UpdateHand(OrderOfParticipantsInGame[0]);
}

void UMusManager::GiveNewCardsToParticipants()
{
	bool didAParticipantDiscard = false;
	for (EParticipant participant : OrderOfParticipantsInGame)
	{
		if(ParticipantsInfo[participant].ParticipantCards.Num() != 4)
		{
			didAParticipantDiscard = true;
			while(ParticipantsInfo[participant].ParticipantCards.Num() != 4)
			{
				//If for some reason, all players called "MUS" and the deck of 40 cards emptied,
				//restart the deck and check if the top one is owned by a participant or not, if it is pass
				//to the next card
				for (int32 i = 0; i < OrderOfParticipantsInGame.Num(); i++)
				{
					if(ParticipantsInfo[OrderOfParticipantsInGame[i]].ParticipantCards.Contains(GameCards[CurrentGameCardIndexInDeck]))
					{
						i = 0;
						++CurrentGameCardIndexInDeck;
						if(CurrentGameCardIndexInDeck == NUMBER_OF_MUS_CARDS)
						{
							CurrentGameCardIndexInDeck = 0;
						}
					}
				}
				ParticipantsInfo[participant].ParticipantCards.Add(GameCards[CurrentGameCardIndexInDeck]);
				++CurrentGameCardIndexInDeck;
				if(CurrentGameCardIndexInDeck == NUMBER_OF_MUS_CARDS)
				{
					CurrentGameCardIndexInDeck = 0;
				}	
			}
			MusTable.Get()->SetParticipantsCards(participant, ParticipantsInfo[participant].ParticipantCards);
		}
	}
	//If no one discarded any cards, continue game
	if(!didAParticipantDiscard)
	{
		PassTurn();
	}
	//Else execute give cards animations
	else
	{
		GetWorld()->GetTimerManager().SetTimer(
				GiveCardsTimer, // handle to cancel timer at a later time
				this, // the owning object
				&UMusManager::EndDiscardPhase, // function to call on elapsed
				CardsShowingAnimationTime, // float delay until elapsed
			false);
	}
}

void UMusManager::EndDiscardPhase()
{
	PassTurn();
}

void UMusManager::StartParticipantAction()
{
	EParticipant currentParticipant = OrderOfParticipantsInGame[CurrentParticipantIDInTurn];
	//First detect if the participant in turn passed. If it did, continue with the next one
	if(ParticipantsInfo[currentParticipant].LastMove == PASO)
	{
		ParticipantCallsAMove(currentParticipant, PASO);
		MusTable.Get()->UpdatePlay(currentParticipant, PASO);
	}
	//Next, check if current participant is PLAYER
	else if(currentParticipant == PLAYER)
	{
		TArray<EMoves> playerPossibleMoves;
		//If not in betting phase, player can only "MUS" or "NO MUS"
		if(!BetsStarted)
        {
			if(InDiscardPhase)
			{
				playerPossibleMoves.Add(DISCARD);
				MusTable.Get()->ActivePlayerCardsForDiscard();
			}
			else
			{
				playerPossibleMoves.Add(MUS);
				playerPossibleMoves.Add(NOMUS);	
			}
        }
		else
		{
			//Before "PARES" and "JUEGO" phase, ask the player if they have,
			if(CurrentBettingPhase == TIENEPARES)
			{
				ParticipantsThatHavePares.Contains(PLAYER) ?
					playerPossibleMoves.Add(EMoves::SITENGO) :
					playerPossibleMoves.Add(EMoves::NOTENGO); 
			}
			else if(CurrentBettingPhase	== TIENEJUEGO)
			{
				ParticipantsThatHaveJuego.Contains(PLAYER) ?
					playerPossibleMoves.Add(EMoves::SITENGO) :
					playerPossibleMoves.Add(EMoves::NOTENGO); 
			}
			//Else, let them "Pass", "Envido", "Quiero" or "Órdago"
			else
			{
				playerPossibleMoves.Add(PASO),
				playerPossibleMoves.Add(ENVIDO);
				if(ParticipantsInfo[PLAYER].EnvidoValue != CurrentBetOnTable)
				{
					playerPossibleMoves.Add(QUIERO);
				}
				playerPossibleMoves.Add(ORDAGO);
			}
		}
		MusTable.Get()->ShowPlayerPossibleActions(playerPossibleMoves);
	}
	//Else, bot action
	else
	{
		int64 time = FMath::RandRange(MinBotThinkTime, MaxBotThinkTime);
		GetWorld()->GetTimerManager().SetTimer(
					BotActionTimer, // handle to cancel timer at a later time
					this, // the owning object
					&UMusManager::FinishBotAction, // function to call on elapsed
					time, // float delay until elapsed
				false);
	}
}

void UMusManager::ResetBotEnvidos()
{
	for (EParticipant participant : OrderOfParticipantsInGame)
	{
		if(ParticipantsInfo[participant].BotAI)
		{
			ParticipantsInfo[participant].BotAI->ResetAmountOfEnvidos();
		}
	}
}

void UMusManager::FinishBotAction()
{
	EParticipant currentParticipant = OrderOfParticipantsInGame[CurrentParticipantIDInTurn];
	if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::White, FString::Printf(TEXT("Turn of: %d"), currentParticipant));}
	//If checking a participant has "PARES" or "JUEGO", the participant can only respond with "YES" or "NO". Else, decide their action
	if(CurrentBettingPhase == TIENEPARES)
	{
		if(ParticipantsThatHavePares.Contains(currentParticipant))
		{
			MusTable.Get()->UpdatePlay(currentParticipant, SITENGO);
			ParticipantCallsAMove(currentParticipant, SITENGO, 0);
		}
		else
		{
			MusTable.Get()->UpdatePlay(currentParticipant, NOTENGO);
			ParticipantCallsAMove(currentParticipant, NOTENGO, 0);	
		}
	}
	else if (CurrentBettingPhase == TIENEJUEGO)
	{
		if(ParticipantsThatHaveJuego.Contains(currentParticipant))
		{
			MusTable.Get()->UpdatePlay(currentParticipant, SITENGO);
			ParticipantCallsAMove(currentParticipant, SITENGO, 0);
		}
		else
		{
			MusTable.Get()->UpdatePlay(currentParticipant, NOTENGO);
			ParticipantCallsAMove(currentParticipant, NOTENGO, 0);	
		}
	}
	else if (InDiscardPhase)
	{
		//TODO Bot should decide which cards to discard
		TArray<int32> cardsToDiscard = {1,3};
		MusTable.Get()->BotCardsDiscard(currentParticipant, cardsToDiscard);
		DiscardParticipantCards(currentParticipant, cardsToDiscard);
	}
	else
	{
		ParticipantsInfo[currentParticipant].BotAI->MakeAMove(CurrentBettingPhase);
		MusTable.Get()->UpdatePlay(currentParticipant, ParticipantsInfo[currentParticipant].BotAI->GetBotMove());
		ParticipantCallsAMove(currentParticipant, ParticipantsInfo[currentParticipant].BotAI->GetBotMove(), ParticipantsInfo[currentParticipant].BotAI->GetEnvidoRocks());
	}
}

void UMusManager::DetectWinnerOfParesAndPrepareForJuego()
{
	WinnerOfPares = CheckPares();
	ParesAmountWon += CurrentBetOnTable;
	ParticipantsThatHavePares.Empty();
	CurrentBettingPhase = TIENEJUEGO;
	//Look who has a "JUEGO"
	for (EParticipant participant : OrderOfParticipantsInGame)
	{
		int32 sumOfParticipantCard = 0;
		for (FCards_Struct* participantCard : ParticipantsInfo[participant].ParticipantCards)
		{
			sumOfParticipantCard += participantCard->CardValue;
		}
		if(sumOfParticipantCard >= 31 && sumOfParticipantCard <= 40)
		{
			ParticipantsThatHaveJuego.Add(participant);
		}
	}
	ResetBotEnvidos();
}

void UMusManager::DetectWinnerOfJuego()
{
	if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::White, FString::Printf(TEXT("Winner Of GRANDE: %d"), WinnerOfGrande));}
	if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Orange, FString::Printf(TEXT("Winner Of CHICA: %d"), WinnerOfChica));}
	WinnerOfJuego = CheckJuego();
	JuegoAmountWon += CurrentBetOnTable;
	if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("Winner Of JUEGO: %d"), WinnerOfJuego));}
	CurrentBettingPhase = RESUMEN;
	CurrentParticipantIDInTurn = 0;
	ParticipantsThatHaveJuego.Empty();
	ResetBotEnvidos();
	ShowWinners();
}

bool UMusManager::AreAnyParticipantCloseToWinning()
{
	int32 numberOfRocksCloseToWinning = 30;
	if(!CurrentMusRules.ObjectiveIs8Amarrakos)
	{
		numberOfRocksCloseToWinning = 20;
	}
	for (TTuple<EParticipant, FParticipantStruct> participantInfo : ParticipantsInfo)
	{
		if(participantInfo.Value.Piedras * participantInfo.Value.Amarrakos >= numberOfRocksCloseToWinning)
		{
			return true;	
		}
	}
	return false;
}

void UMusManager::DiscardParticipantCards(EParticipant Participant, TArray<int32> CardsToBeDiscarded)
{
	TArray<FCards_Struct*> cardsToRemove;
	for (int32 i = 0; i < CardsToBeDiscarded.Num(); i++)
	{
		cardsToRemove.Add(ParticipantsInfo[Participant].ParticipantCards[CardsToBeDiscarded[i]]);
	}
	
	for (int32 i = 0; i < cardsToRemove.Num(); i++)
	{
		ParticipantsInfo[Participant].ParticipantCards.Remove(cardsToRemove[i]);
	}
	cardsToRemove.Empty();
}

//----------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------Mus Plays------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------
void UMusManager::CheckFrequencyOfCardsInHand()
{
	//Detect frequency of cards each player has
	for (EParticipant participant : OrderOfParticipantsInGame)
	{
		ParticipantsInfo[participant].ParticipantCardFrequency.Empty();
		ParticipantsInfo[participant].InitializeCardFrequency();
		for (FCards_Struct* participantCard : ParticipantsInfo[participant].ParticipantCards)
		{
			ParticipantsInfo[participant].ParticipantCardFrequency[CheckMusRulesFor3sAnd2s(participantCard)]++;
		}
	}
}

EParticipant UMusManager::CheckGrande()
{
	EParticipant winner = ParticipantsInTheCurrentBet[0];
	TArray<EParticipant> participantsInTheBattle = ParticipantsInTheCurrentBet;
	participantsInTheBattle.Remove(winner);
	
	for (int32 i = 12; i > 0; i--)
	{
		bool winnerIsSuperior = true;
		bool foundANewWinner = false;
		for (int k = 0; k < participantsInTheBattle.Num(); k++)
		{
			//Compare the supposed winner versus the rest
			if(ParticipantsAllCards[winner][i] < ParticipantsAllCards[participantsInTheBattle[k]][i])
			{
				winnerIsSuperior = false;
				foundANewWinner = true;
				winner = participantsInTheBattle[k];
				participantsInTheBattle.Remove(winner);
				--k;
			}
			else if(ParticipantsAllCards[winner][i] == ParticipantsAllCards[participantsInTheBattle[k]][i])
			{
				winnerIsSuperior = false;
			}
			else
			{
				EParticipant participantOutOfBattle = participantsInTheBattle[k];
				participantsInTheBattle.Remove(participantOutOfBattle);
				--k;
			}
		}
		//If the supposed winner has a more frequency of a higher card, it automatically wins
		//Ex: if participant 1 has 2 Kings and the rest only 1 King, participant automatically wins
		if(winnerIsSuperior)
		{
			break;
		}
		if(foundANewWinner)
		{
			//Check again for the same number
			++i;
		}
		//Jump from 10 to 7
		if(i == 10)
		{
			i = 8;
		}
	}

	return winner;
}

EParticipant UMusManager::CheckChica()
{
	EParticipant winner = ParticipantsInTheCurrentBet[0];
	TArray<EParticipant> participantsInTheBattle = ParticipantsInTheCurrentBet;
	participantsInTheBattle.Remove(winner);
	
	for (int32 i = 1; i < 13; i++)
	{
		bool winnerIsSuperior = true;
		bool foundANewWinner = false;
		for (int k = 0; k < participantsInTheBattle.Num(); k++)
		{
			//Compare the supposed winner versus the rest
			if(ParticipantsAllCards[winner][i] < ParticipantsAllCards[participantsInTheBattle[k]][i])
			{
				winnerIsSuperior = false;
				foundANewWinner = true;
				winner = participantsInTheBattle[k];
				participantsInTheBattle.Remove(winner);
				
				--k;
			}
			else if(ParticipantsAllCards[winner][i] == ParticipantsAllCards[participantsInTheBattle[k]][i])
			{
				winnerIsSuperior = false;
			}
			else
			{
				EParticipant participantOutOfBattle = participantsInTheBattle[k];
				participantsInTheBattle.Remove(participantOutOfBattle);
				--k;
			}
		}
		//If the supposed winner has a more frequency of a lower card, it automatically wins
		//Ex: if participant 1 has 2 As and the rest only 1 As, participant automatically wins
		if(winnerIsSuperior)
		{
			break;
		}
		if(foundANewWinner)
		{
			//Check again for the same number
			--i;
		}
		//Jump from 7 to 10
		if(i == 7)
		{
			i = 9;
		}
	}

	return winner;
}

EParticipant UMusManager::CheckPares()
{
	//Detect how many double duos, trios and duos a participant has
	EParticipant winner = NOONE;
	//Save the number of the 4 equal cards a participant has
	//Ex: PLAYER has a 4 Kings: (PLAYER, K)
	TMap<EParticipant, int32> participantsWithPoker;
	//Save the number of the trio a participant has
	TMap<EParticipant, int32> participantsWithATrio;
	//Save a duo of a participant
	TMap<EParticipant, int32> participantsWithADuo;
	//Save another duo of a participant. This map saves the highest pair of a participant
	TMap<EParticipant, int32> participantsWithAnotherDuo;
	
	for (EParticipant participant : ParticipantsInTheCurrentBet)
	{
		for (int32 i = 1; i < 13; i++)
		{
			//Check 4 equal cards
			if(ParticipantsAllCards[participant][i] == 4)
			{
				participantsWithPoker.Add(participant, i);
				break;
			}
			//Check for trios
			if(ParticipantsAllCards[participant][i] == 3)
			{
				participantsWithATrio.Add(participant, i);
				break;
			}
			//Checks for duos
			if(ParticipantsAllCards[participant][i] == 2)
			{
				//Check if the player already has one pair
				if(participantsWithADuo.Contains(participant))
				{
					participantsWithAnotherDuo.Add(participant, i);
				}
				else
				{
					participantsWithADuo.Add(participant, i);
				}
			}

			//Jump from 7 to 10
			if(i == 7)
			{
				i = 9;
			}
		}
	}

	//Examine the results. If only 1 participant has a poker, it wins
	//If only one has a trio, it wins
	//If only one has a duo, it wins
	//If only 2 have a poker, or only 2 a trio, compare the number of the card
	if(participantsWithPoker.IsEmpty())
	{
		//If there is no pokers, check trios
		if(participantsWithATrio.IsEmpty())
		{
			//If there is no trios or pokers, check pairs
			if(!participantsWithADuo.IsEmpty())
			{
				winner = CheckPairsIndividually(participantsWithADuo, participantsWithAnotherDuo);
				if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, FString::Printf(TEXT("Winner Of Par: %d"), winner));}
				ParesAmountWon = 1;
			}
		}
		else
		{
			winner = CheckPokerOrTrio(participantsWithATrio);
			if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, FString::Printf(TEXT("Winner Of Medias: %d"), winner));}
			ParesAmountWon = 2;
		}
	}
	else
	{
		winner = CheckPokerOrTrio(participantsWithPoker);
		if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Blue, FString::Printf(TEXT("Winner Of Duples: %d"), winner));}
		ParesAmountWon = 3;
	}
	return winner;
}

EParticipant UMusManager::CheckPokerOrTrio(TMap<EParticipant, int32> ParticipantsWithPokerOrTrio)
{
	TTuple<EParticipant, int32> winnerOfPokerOrTrio = TTuple<EParticipant, int32>(NOONE, 0);
	for(TTuple<EParticipant, int32> participantPokerOrTrio : ParticipantsWithPokerOrTrio)
	{
		if(winnerOfPokerOrTrio.Value < participantPokerOrTrio.Value)
		{
			winnerOfPokerOrTrio = participantPokerOrTrio;
		}
	}
	return winnerOfPokerOrTrio.Key;
}

EParticipant UMusManager::CheckPairsIndividually(TMap<EParticipant, int32> ParticipantsWithADuo,
	TMap<EParticipant, int32> ParticipantsWithAnotherDuo)
{
	TTuple<EParticipant, int32> winnerOfPairs = TTuple<EParticipant, int32>(NOONE, 0);
	//If the participants only have one pair
	if(ParticipantsWithAnotherDuo.IsEmpty())
	{
		for(TTuple<EParticipant, int32> participantPair : ParticipantsWithADuo)
		{
			if(winnerOfPairs.Value < participantPair.Value)
			{
				winnerOfPairs = participantPair;
			}
		}
	}
	else
	{
		bool tiedInHighestPair = true;
		TArray<EParticipant> participantsTied;
		//Check the value of highest pair. If there is a tie, check the other pair
		for(TTuple<EParticipant, int32> participantWithHighestDuo : ParticipantsWithAnotherDuo)
		{
			if(winnerOfPairs.Value < participantWithHighestDuo.Value)
			{
				participantsTied.Empty();
				tiedInHighestPair = false;
				winnerOfPairs = participantWithHighestDuo;
			}
			else if(winnerOfPairs.Value == participantWithHighestDuo.Value)
			{
				if(!participantsTied.Contains(winnerOfPairs.Key))
				{
					participantsTied.Add(winnerOfPairs.Key);	
				}
				participantsTied.Add(participantWithHighestDuo.Key);
				tiedInHighestPair = true;
			}
		}
		if(tiedInHighestPair)
		{
			int32 highestSecondPair = 0;
			for(EParticipant participantTied : participantsTied)
			{
				if(highestSecondPair < ParticipantsWithADuo[participantTied])
				{
					winnerOfPairs.Key =  participantTied;
					highestSecondPair = ParticipantsWithADuo[participantTied];
				}
			}
		}
	}
	
	return winnerOfPairs.Key;
}

EParticipant UMusManager::CheckJuego()
{
	TTuple<EParticipant, int32> winnerOfJuego = TTuple<EParticipant, int32>(NOONE, 0);

	TArray<EParticipant> participantsWithJuego;
	TMap<EParticipant, int32> participantsMapTotalSum;
	for (EParticipant participant : ParticipantsInTheCurrentBet)
	{
		int32 sumOfParticipantCard = 0;
		for (FCards_Struct* participantCard : ParticipantsInfo[participant].ParticipantCards)
		{
			sumOfParticipantCard += participantCard->CardValue;
		}
		participantsMapTotalSum.Add(participant, sumOfParticipantCard);
	}
	
	//Check who won "JUEGO"
	for (TTuple<EParticipant, int32> participantMapTotalSum : participantsMapTotalSum)
	{
		//Check best score
		if(participantMapTotalSum.Value == 31 && winnerOfJuego.Value != participantMapTotalSum.Value)
		{
			winnerOfJuego = participantMapTotalSum;
			JuegoAmountWon = 3;
		}
		//Check second best score
		else if(participantMapTotalSum.Value == 32 && winnerOfJuego.Value != 31 && winnerOfJuego.Value != participantMapTotalSum.Value)
		{
			winnerOfJuego = participantMapTotalSum;
			JuegoAmountWon = 2;
		}
		//Check third best score
		else if(participantMapTotalSum.Value == 40 && winnerOfJuego.Value != 31 && winnerOfJuego.Value != 32 && winnerOfJuego.Value != participantMapTotalSum.Value)
		{
			winnerOfJuego = participantMapTotalSum;
			JuegoAmountWon = 2;
		}
		//Check the rest of scores
		else if(winnerOfJuego.Value != 31 && winnerOfJuego.Value != 32 && winnerOfJuego.Value != 40
			&& winnerOfJuego.Value < participantMapTotalSum.Value)
		{
			winnerOfJuego = participantMapTotalSum;
			JuegoAmountWon = 2;
		}
	}

	return winnerOfJuego.Key;
}

EParticipant UMusManager::CheckPunto()
{
	TTuple<EParticipant, int32> winnerOfPunto = TTuple<EParticipant, int32>(NOONE, 0);
	TMap<EParticipant, int32> participantsMapTotalSum;
	for (EParticipant participant : ParticipantsInTheCurrentBet)
	{
		int32 sumOfParticipantCard = 0;
		for (FCards_Struct* participantCard : ParticipantsInfo[participant].ParticipantCards)
		{
			sumOfParticipantCard += participantCard->CardValue;
		}
		participantsMapTotalSum.Add(participant,sumOfParticipantCard);
	}
	for (TTuple<EParticipant, int32> participantMapTotalSum : participantsMapTotalSum)
	{
		if(winnerOfPunto.Value < participantMapTotalSum.Value)
		{
			winnerOfPunto = participantMapTotalSum;
		}
	}
	JuegoAmountWon = 1;
	
	return winnerOfPunto.Key;
}

int32 UMusManager::CheckMusRulesFor3sAnd2s(FCards_Struct* Card)
{
	if(CurrentMusRules.KingsAndAces8)
	{
		if(Card->Number == 3)
		{
			return 12;
		}
		if(Card->Number == 2)
		{
			return 1;		
		}
			return Card->Number;		
	}
	
	return  Card->Number;
}

void UMusManager::ChangeHand()
{
	for (int32 i = 0; i < OrderOfParticipantsInGame.Num() - 1; i++)
	{
		EParticipant saveParticipantForChange = OrderOfParticipantsInGame[i];
		// if(i + 1 == OrderOfParticipantsInGame.Num())
		// {
		// 	OrderOfParticipantsInGame[i] = OrderOfParticipantsInGame[0];
		// 	OrderOfParticipantsInGame[0] = saveParticipantForChange;
		// }
		// else
		// {
			OrderOfParticipantsInGame[i] = OrderOfParticipantsInGame[i + 1];
			OrderOfParticipantsInGame[i + 1] = saveParticipantForChange;
		// }
	}
	MusTable.Get()->UpdateHand(OrderOfParticipantsInGame[0]);
}

void UMusManager::PassTurn()
{
	++CurrentParticipantIDInTurn;
	if(CurrentParticipantIDInTurn == OrderOfParticipantsInGame.Num())
	{ 
		CurrentParticipantIDInTurn = 0;	
	}
	StartParticipantAction();
}

//----------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------SHOW WINNERS---------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------

void UMusManager::ShowWinners()
{
	//TODO Add Animation where it shows how many "piedras" a participant won in the rounds
	GetWorld()->GetTimerManager().SetTimer(
				WinnerTimer, // handle to cancel timer at a later time
				this, // the owning object
				&UMusManager::ShowGrandeWinner, // function to call on elapsed
				3, // float delay until elapsed
			false);
}

void UMusManager::UpdateParticipantPiedras(EParticipant Participant)
{
	MusTable.Get()->UpdatePiedras(Participant, ParticipantsInfo[Participant].Amarrakos,ParticipantsInfo[Participant].Piedras);
}

void UMusManager::ShowGrandeWinner()
{
	MusTable.Get()->UpdatePhase(GRANDE, true, WinnerOfGrande);
	ParticipantsInfo[WinnerOfGrande].AddPiedras(GrandeAmountWon);
	UpdateParticipantPiedras(WinnerOfGrande);
	GrandeAmountWon = 0;
	WinnerOfGrande = NOONE;
	GetWorld()->GetTimerManager().SetTimer(
			WinnerTimer, // handle to cancel timer at a later time
			this, // the owning object
			&UMusManager::ShowChicaWinner, // function to call on elapsed
			3, // float delay until elapsed
		false);
}

void UMusManager::ShowChicaWinner()
{
	MusTable.Get()->UpdatePhase(CHICA, true,WinnerOfChica);
	ParticipantsInfo[WinnerOfChica].AddPiedras(ChicaAmountWon);
	UpdateParticipantPiedras(WinnerOfChica);
	ChicaAmountWon = 0;
	WinnerOfChica = NOONE;
	GetWorld()->GetTimerManager().SetTimer(
			WinnerTimer, // handle to cancel timer at a later time
			this, // the owning object
			&UMusManager::ShowPairWinner, // function to call on elapsed
			3, // float delay until elapsed
		false);
}

void UMusManager::ShowPairWinner()
{
	MusTable.Get()->UpdatePhase(PARES, true, WinnerOfPares);
	ParticipantsInfo[WinnerOfPares].AddPiedras(ParesAmountWon);
	UpdateParticipantPiedras(WinnerOfPares);
	ParesAmountWon = 0;
	WinnerOfPares = NOONE;
	GetWorld()->GetTimerManager().SetTimer(
			WinnerTimer, // handle to cancel timer at a later time
			this, // the owning object
			&UMusManager::ShowJuegoWinner, // function to call on elapsed
			3, // float delay until elapsed
		false);
}

void UMusManager::ShowJuegoWinner()
{
	//Check if there was "JUEGO" or "PUNTO"
	if(!JuegoWentToPunto)
	{
		MusTable.Get()->UpdatePhase(JUEGO, true, WinnerOfJuego);
	}
	else
	{
		MusTable.Get()->UpdatePhase(PUNTO, true, WinnerOfJuego);
		JuegoWentToPunto = false;
	}
	ParticipantsInfo[WinnerOfJuego].AddPiedras(JuegoAmountWon);
	UpdateParticipantPiedras(WinnerOfJuego);
	JuegoAmountWon = 0;
	WinnerOfJuego = NOONE;
	GetWorld()->GetTimerManager().SetTimer(
			WinnerTimer, // handle to cancel timer at a later time
			this, // the owning object
			&UMusManager::EndShowingWinners, // function to call on elapsed
			3, // float delay until elapsed
		false);
}

void UMusManager::EndShowingWinners()
{
	//When all round of winner have been done
	//Start next round
	CurrentBettingPhase = NONE;
	BetsStarted = false;
	MusTable.Get()->UpdatePhase(CurrentBettingPhase);
	ChangeHand();
	//Reset bot card info
	for (EParticipant participant : OrderOfParticipantsInGame)
	{
		if(ParticipantsInfo[participant].BotAI)
		{
			ParticipantsInfo[participant].BotAI->ResetCardInformation();
		}
	}
	//Update cards
	ShuffleAndGiveCards();
	StartParticipantAction();
}

