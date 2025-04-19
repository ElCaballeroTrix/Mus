// Fill out your copyright notice in the Description page of Project Settings.


#include "MusManager.h"
#include "GroupOfCardsTable.h"
#include "Algo/RandomShuffle.h"
#include "Blueprint/UserWidget.h"
#include "MusModerno/UI/MusTable.h"

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
				ParticipantsInTheCurrentBet.Add(Participant);
			break;
		case ENVIDO:
				ParticipantsInTheCurrentBet.Add(Participant);
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
			TMap<EParticipant, TMap<int32, int32>> participantsAllCards;
			participantsAllCards.Add(BOT1, ParticipantsInfo[BOT1].ParticipantCardFrequency);
			participantsAllCards.Add(BOT2, ParticipantsInfo[BOT2].ParticipantCardFrequency);
			participantsAllCards.Add(BOT3, ParticipantsInfo[BOT3].ParticipantCardFrequency);
			participantsAllCards.Add(PLAYER, ParticipantsInfo[PLAYER].ParticipantCardFrequency);
			
			switch (CurrentBettingPhase) {
				case GRANDE:
						WinnerOfGrande = CheckGrande(participantsAllCards);
						if(noOneMadeABet || ParticipantsInTheCurrentBet.Num() == 1)
						{
							GrandeAmountWon = 1;
						}
						else
						{
							GrandeAmountWon = CurrentBetOnTable;
						}
						CurrentBettingPhase = CHICA;
					break;
				case CHICA:
						WinnerOfChica = CheckChica(participantsAllCards);
						if(noOneMadeABet || ParticipantsInTheCurrentBet.Num() == 1)
						{
							ChicaAmountWon = 1;
						}
						else
						{
							ChicaAmountWon = CurrentBetOnTable;
						}
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
						!noOneMadeABet ? CurrentBettingPhase = PARES : CurrentBettingPhase = TIENEJUEGO;
					break;
				case PARES:
						WinnerOfPares = CheckPares(participantsAllCards);
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
					break;
				case TIENEJUEGO:
						//If no one has a "JUEGO", pass go to "PUNTO" round
						!noOneMadeABet ? CurrentBettingPhase = JUEGO : CurrentBettingPhase = PUNTO;
					break;
				case JUEGO:
						if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::White, FString::Printf(TEXT("Winner Of GRANDE: %d"), WinnerOfGrande));}
						if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Orange, FString::Printf(TEXT("Winner Of CHICA: %d"), WinnerOfChica));}
						WinnerOfJuego = CheckJuego();
						JuegoAmountWon += CurrentBetOnTable;
						if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("Winner Of JUEGO: %d"), WinnerOfJuego));}
						CurrentBettingPhase = RESUMEN;
						CurrentParticipantIDInTurn = 0;
						ParticipantsThatHaveJuego.Empty();
						ShowWinners();
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
						ShowWinners();
					break;
			}
			ParticipantThatRaisedTheBet = 0;
			CurrentBetOnTable = 0;
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
				StarParticipantAction();
			}
		}
		else
		{
			PassTurn();
		}
	}
	//If "No MUS", start the Betting Phase
	else if(Move == NOMUS)
	{
		BetsStarted = true;
		CurrentParticipantIDInTurn = 0;
		CurrentBettingPhase = GRANDE;
		MusTable.Get()->UpdatePhase(CurrentBettingPhase);
		CheckFrequencyOfCardsInHand();
		if(OrderOfParticipantsInGame[CurrentParticipantIDInTurn] == PLAYER)
		{
			TArray<EMoves> playerPossibleMoves;
			playerPossibleMoves.Add(PASO),
			playerPossibleMoves.Add(ENVIDO);
			playerPossibleMoves.Add(ORDAGO);
			MusTable.Get()->ShowPlayerPossibleActions(playerPossibleMoves);
		}
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
	if(Card->Number == 11 && Card->Number == 12)
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
	Bot1.HasAmarrakos = false;
	Bot1.Partner = Bot3;
	ParticipantsInfo.Add(BOT1, Bot1);

	Bot2.ParticipantName = BOT2;
	Bot2.HasAmarrakos = true;
	Bot2.Partner = Player;
	ParticipantsInfo.Add(BOT2, Bot2);

	Bot3.ParticipantName = BOT3;
	Bot3.HasAmarrakos = true;
	Bot3.Partner = Bot1;
	ParticipantsInfo.Add(BOT3, Bot3);

	Player.ParticipantName = PLAYER;
	Player.HasAmarrakos = false;
	Player.Partner = Bot2;
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
}

void UMusManager::StarParticipantAction()
{
	EParticipant currentParticipant = OrderOfParticipantsInGame[CurrentParticipantIDInTurn];
	//First detect if the participant in turn passed. If it did, continue with the next one
	if(ParticipantsInfo[currentParticipant].LastMove == PASO)
	{
		ParticipantCallsAMove(currentParticipant, PASO);
	}
	//Next, check if current participant is PLAYER
	else if(currentParticipant == PLAYER)
	{
		TArray<EMoves> playerPossibleMoves;
		//If not in betting phase, player can only "MUS" or "NO MUS"
		if(!BetsStarted)
        {
        	playerPossibleMoves.Add(MUS);
        	playerPossibleMoves.Add(NOMUS);
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
	else
	{
		switch (currentParticipant)
		{
		case BOT1:
			//TODO REMOVE THIS, THIS IS FOR TESTING
				MusTable.Get()->UpdatePlay(currentParticipant, ENVIDO);
				ParticipantCallsAMove(BOT1, ENVIDO, CurrentBetOnTable + 2);
			break;
		case BOT2:
				MusTable.Get()->UpdatePlay(currentParticipant, PASO);
				ParticipantCallsAMove(BOT2, PASO, 0);
			break;
		case BOT3:
				MusTable.Get()->UpdatePlay(currentParticipant, QUIERO);
				ParticipantCallsAMove(BOT3, QUIERO, CurrentBetOnTable);
			break;
		}
	}
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

EParticipant UMusManager::CheckGrande(TMap<EParticipant, TMap<int32, int32>> ParticipantsAllCards)
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

EParticipant UMusManager::CheckChica(TMap<EParticipant, TMap<int32, int32>> ParticipantsAllCards)
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

EParticipant UMusManager::CheckPares(TMap<EParticipant, TMap<int32, int32>> ParticipantsAllCards)
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
	for (int32 i = 0; i < OrderOfParticipantsInGame.Num(); i++)
	{
		EParticipant saveParticipantForChange = OrderOfParticipantsInGame[i];
		if(i + 1 == OrderOfParticipantsInGame.Num())
		{
			OrderOfParticipantsInGame[i] = OrderOfParticipantsInGame[0];
			OrderOfParticipantsInGame[0] = saveParticipantForChange;
		}
		else
		{
			OrderOfParticipantsInGame[i] = OrderOfParticipantsInGame[i + 1];
			OrderOfParticipantsInGame[i + 1] = saveParticipantForChange;
		}
	}
}

void UMusManager::PassTurn()
{
	++CurrentParticipantIDInTurn;
	if(CurrentParticipantIDInTurn == OrderOfParticipantsInGame.Num())
	{ 
		CurrentParticipantIDInTurn = 0;	
	}
	StarParticipantAction();
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
	if(ParticipantsInfo[Participant].HasAmarrakos)
	{
		MusTable.Get()->UpdatePiedras(Participant, ParticipantsInfo[Participant].Amarrakos);
		MusTable.Get()->UpdatePiedras(ParticipantsInfo[Participant].Partner.Get()->ParticipantName, ParticipantsInfo[Participant].Partner.Get()->Piedras);
	}
	else
	{
		MusTable.Get()->UpdatePiedras(Participant, ParticipantsInfo[Participant].Piedras);
		MusTable.Get()->UpdatePiedras(ParticipantsInfo[Participant].Partner.Get()->ParticipantName, ParticipantsInfo[Participant].Partner.Get()->Amarrakos);
	}
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
	MusTable.Get()->UpdatePhase(CurrentBettingPhase);
	ChangeHand();
	//Update cards
	ShuffleAndGiveCards();
	StarParticipantAction();
}

