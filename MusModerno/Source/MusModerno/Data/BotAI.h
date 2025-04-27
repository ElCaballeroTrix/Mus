#pragma once

enum EBettingPhase : int;
enum EParticipant : int;
class UMusManager;
enum EMoves : int;

struct BotCardsInfo
{
	int32 NumberOfKings = 0;
	int32 NumberOfAces = 0;
	int32 PokerNumber = 0;
	int32 TrioNumber = 0;
	int32 Pair1Number = 0;
	int32 Pair2Number = 0;
	int32 JuegoNumber = 0;
};

class BotAI
{
public:
	BotAI(TObjectPtr<UMusManager> _MusManager, EParticipant _Participant);
	void ExamineCards();
	void ResetCardInformation();
	void MakeAMove(EBettingPhase CurrentBettingPhase);
	EMoves GetBotMove() { return MoveToMake; }
	int32 GetEnvidoRocks(){ return EnvidoRocksMade; }
	void ResetAmountOfEnvidos(){ AmountOfEnvidos = 0; }
	TArray<int32> GetCardsToBeDiscarded(){ return CardsToBeDiscarded; }
	
private:
	BotCardsInfo BotCardsInfo;
	TObjectPtr<UMusManager> MusManager;
	EParticipant Participant;
	EMoves MoveToMake;
	int32 EnvidoRocksMade = 0;
	bool CanBluff = false;
	//This variable is to limit the amount of "envidos" in a round to 3.
	//This is done so that the bot don't send "envidos" infinitely
	int32 AmountOfEnvidos = 0;
	//Cards to be discarded
	TArray<int32> CardsToBeDiscarded;

	//Detects if it should "MUS" or not.
	//If "MUS", it decides which cards to discard
	void MusOrNoMus();
};
