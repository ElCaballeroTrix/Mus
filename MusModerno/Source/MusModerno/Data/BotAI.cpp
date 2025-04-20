#include "BotAI.h"

#include "GroupOfCardsTable.h"
#include "MusManager.h"
#include "Algo/RandomShuffle.h"

BotAI::BotAI(TObjectPtr<UMusManager> _MusManager, EParticipant _Participant)
{
	MusManager = _MusManager;
	Participant = _Participant;
	TArray<bool> buffScenario = {true, false};
	Algo::RandomShuffle(buffScenario);
	CanBluff = buffScenario[0];
}

void BotAI::ExamineCards()
{
	//Obtain current information of bot
	FParticipantStruct botInfo = MusManager.Get()->GetParticipantStruct(Participant);
	//Detect how many kings does bot have
	BotCardsInfo.NumberOfKings = botInfo.ParticipantCardFrequency[12];
	//Detect how many aces does bot have
	BotCardsInfo.NumberOfAces = botInfo.ParticipantCardFrequency[1];
	//Detect if bot has a poker, trio, double pair or pair. Save the number that is repeated
	for (TTuple<int32, int32> botCard : botInfo.ParticipantCardFrequency)
	{
		if(botCard.Value == 4)
		{
			BotCardsInfo.PokerNumber = botCard.Key;
			break;
		}
		if(botCard.Value == 3)
		{
			BotCardsInfo.TrioNumber = botCard.Key;
			break;
		}
		if(botCard.Value == 2)
		{
			if(BotCardsInfo.Pair1Number == 0)
			{
				BotCardsInfo.Pair1Number = botCard.Key;
			}
			else
			{
				BotCardsInfo.Pair2Number = botCard.Key;
				break;
			}
		}
	}
	//Detect the sum of all cards to see if bot has "JUEGO"
	for (FCards_Struct* card : botInfo.ParticipantCards)
	{
		BotCardsInfo.JuegoNumber += card->CardValue;
	}
}

void BotAI::ResetCardInformation()
{
	BotCardsInfo.NumberOfKings = 0;
	BotCardsInfo.NumberOfAces = 0;
	BotCardsInfo.PokerNumber = 0;
	BotCardsInfo.TrioNumber = 0;
	BotCardsInfo.Pair1Number = 0;
	BotCardsInfo.Pair2Number = 0;
	BotCardsInfo.JuegoNumber = 0;
	AmountOfEnvidos = 0;
}

void BotAI::MakeAMove(EBettingPhase CurrentBettingPhase)
{
	//Check if the game is in "MUS" phase, decide if it should discard any cards or not
	if(CurrentBettingPhase == NONE)
	{
		//TODO When discard is finish, change this
		MoveToMake = NOMUS;
		EnvidoRocksMade = 0;
		return;
	}
	//Else decide whether to bet or not
	//First check if it has not done 3 "envidos" in the same round
	//if it did, then do "QUIERO" move
	if(AmountOfEnvidos == 3)
	{
		MoveToMake = QUIERO;
		EnvidoRocksMade = MusManager.Get()->GetCurrentBetOnTable();
		return;
	}
	bool canPlay = false;
	//If previous move was "ENVIDO" or "QUIERO", it means bot already made a decision to play
	//so it will play it
	if(MusManager.Get()->GetParticipantStruct(Participant).LastMove == ENVIDO ||
		MusManager.Get()->GetParticipantStruct(Participant).LastMove == QUIERO)
	{
		canPlay = true;
	}
	//If bot can bluff, there is a possibility that he will, either "ENVIDO" more rocks, "QUIERO",
	//or "ÓRDAGO" if rivals or him are close to winning
	if(!canPlay && CanBluff)
	{
		float randomPercentageOfBluff = FMath::RandRange(0, 1);
		if(randomPercentageOfBluff >= 0.5f)
		{
			canPlay = true; 
		}
	}
	
	//If it did not want to bluff, check if it has a nice move
	if(!canPlay)
	{
		//Here, the bot detects the percentage it has of winning, there is a possibility of passing or not
		float possibilityOfPlaying = 0.0f;
		float randomPercentageOfPlaying = FMath::RandRange(0, 1);
		switch (CurrentBettingPhase) {
			case GRANDE:
					possibilityOfPlaying += BotCardsInfo.NumberOfKings / 4;
				break;
			case CHICA:
					possibilityOfPlaying += BotCardsInfo.NumberOfAces / 4;
				break;
			case PARES:
					if(BotCardsInfo.PokerNumber > 0)
					{
						possibilityOfPlaying = 1.0f;
					}
					else if(BotCardsInfo.TrioNumber > 0)
					{
						possibilityOfPlaying = 0.8f;
					}
					else if(BotCardsInfo.Pair1Number > 0 && BotCardsInfo.Pair2Number)
					{
						possibilityOfPlaying = 0.6f;
					}
					else if(BotCardsInfo.Pair1Number > 0 )
					{
						possibilityOfPlaying = 0.4f;
					}
				break;
			case JUEGO:
					if(BotCardsInfo.JuegoNumber == 31)
					{
						possibilityOfPlaying = 1.0f;
					}
					else if(BotCardsInfo.JuegoNumber == 32)
					{
						possibilityOfPlaying = 0.8f;
					}
					else if(BotCardsInfo.JuegoNumber == 40)
					{
						possibilityOfPlaying = 0.6f;
					}
					else if(BotCardsInfo.JuegoNumber > 32)
					{
						possibilityOfPlaying = 0.4f;
					}
				break;
			case PUNTO:
					//Best move
					if(BotCardsInfo.JuegoNumber == 30)
					{
						possibilityOfPlaying = 1.0f;
					}
					else if(BotCardsInfo.JuegoNumber >= 25)
					{
						possibilityOfPlaying = 0.7f;
					}
					else if(BotCardsInfo.JuegoNumber >= 20)
					{
						possibilityOfPlaying = 0.5f;
					}
				break;
		}
		if(randomPercentageOfPlaying > possibilityOfPlaying)
		{
			canPlay = true;
		}
	}
	
	//If bot has a nice move or is going to bluff, make or accept the bet
	if(canPlay)
	{
		//If someone made a bet, there is a possibility of incrementing the bet, or playing what is on table
		if(MusManager.Get()->GetCurrentBetOnTable() > 0)
		{
			float randomPercentage = FMath::RandRange(0, 1);
			if(randomPercentage >= 0.7f)
			{
				//If someone is close to winning, through "ÓRDAGO"
				if(MusManager.Get()->AreAnyParticipantCloseToWinning())
				{
					MoveToMake = ORDAGO;
					int32 allPiedras;
					MusManager.Get()->GetCurrentMusRules().ObjectiveIs8Amarrakos ? allPiedras = 40 : allPiedras = 30;
					EnvidoRocksMade = allPiedras;
				}
				//Else "ENVIDO"
				else
				{
					MoveToMake = ENVIDO;
					EnvidoRocksMade = MusManager.Get()->GetCurrentBetOnTable() + FMath::RandRange( 2, 5);
					++AmountOfEnvidos;
				}
			}
			else
			{
				MoveToMake = QUIERO;
				EnvidoRocksMade = MusManager.Get()->GetCurrentBetOnTable();
			}
		}
		else
		{
			MoveToMake = ENVIDO;
			EnvidoRocksMade = MusManager.Get()->GetCurrentBetOnTable() + FMath::RandRange( 2, 5);
			++AmountOfEnvidos;
		}
	}
	else
	{
		MoveToMake = PASO;
		EnvidoRocksMade = 0;
	}
}
