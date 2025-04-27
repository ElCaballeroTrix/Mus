// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MusManager.generated.h"

class UHomeMenu;
class UMus_UserSettings;
class BotAI;
class UMusTable;
struct FCards_Struct;
/**
 * 
 */
UENUM()
enum EParticipant
{
	NOONE = 0,
	BOT1 = 1,
	BOT2 = 2,
	BOT3 = 3,
	PLAYER = 4
};
UENUM()
enum EMoves
{
	NOMOVE = 0,
	MUS = 1,
	NOMUS = 2,
	DISCARD = 3,
	PASO = 4,
	ENVIDO = 5,
	QUIERO = 6,
	NOQUIERO = 7,
	ORDAGO = 8,
	SITENGO = 9,
	NOTENGO = 10
};
UENUM()
enum EBettingPhase
{
	NONE = -1,
	GRANDE = 0,
	CHICA = 1,
	TIENEPARES = 2,
	PARES = 3,
	TIENEJUEGO = 4,
	JUEGO = 5,
	PUNTO = 6,
	RESUMEN = 7,
	ORDAGOWINNER = 8
};

USTRUCT(BlueprintType, Blueprintable)
struct FParticipantStruct
{
	GENERATED_BODY()
	
	EParticipant ParticipantName;
	EMoves LastMove;
	TArray<FCards_Struct*> ParticipantCards;
	TMap<int32, int32> ParticipantCardFrequency;
	int32 EnvidoValue = 0;
	int32 Amarrakos = 0;
	int32 Piedras = 0;
	BotAI* BotAI;
	FParticipantStruct()
	{
		InitializeCardFrequency();
	}

	void InitializeCardFrequency()
	{
		//Fill the map of frequency. In this way, the system can access how many cards
		//of the same number does the participant have in hand
		ParticipantCardFrequency.Add(1,0);  ParticipantCardFrequency.Add(2,0);
		ParticipantCardFrequency.Add(3,0);  ParticipantCardFrequency.Add(4,0);
		ParticipantCardFrequency.Add(5,0);  ParticipantCardFrequency.Add(6,0);
		ParticipantCardFrequency.Add(7,0);	ParticipantCardFrequency.Add(10,0);
		ParticipantCardFrequency.Add(11,0);	ParticipantCardFrequency.Add(12,0);
	}

	void AddPiedras(int32 NewPiedras)
	{
		Piedras += NewPiedras;
		while(Piedras >= 5)
		{
			Piedras -= 5;
			Amarrakos += 1;
		}
	}
};

USTRUCT(BlueprintType, Blueprintable)
struct FMusRules 
{
	GENERATED_BODY()

	//The objectives are either obtain 8 amarrakos or 6
	bool ObjectiveIs8Amarrakos = true;
	//Either there are 8 kings and 8 aces(the original 4 plus threes(count as kings) and twos(count as aces))
	//or only the original 4
	bool KingsAndAces8 = true;
};

UCLASS()
class MUSMODERNO_API UMusManager : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MUS| Datatable")
	TObjectPtr<UDataTable> OrosTable;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MUS| Datatable")
	TObjectPtr<UDataTable> CopasTable;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MUS| Datatable")
	TObjectPtr<UDataTable> EspadasTable;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MUS| Datatable")
	TObjectPtr<UDataTable> BastosTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MUS| Class")
	TSubclassOf<UMusTable> MusTableClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MUS| Class")
	TSubclassOf<UHomeMenu> HomeMenuClass;

	//The time it takes to reveal a new cards when given
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MUS| Table Info")
	float CardsShowingAnimationTime = 2.5f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MUS| Bot", meta = (UIMin = 1, UIMax = 15), meta = (ClampMin = 1, ClampMax = 15))
	int64 MinBotThinkTime = 2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MUS| Bot", meta = (UIMin = 1, UIMax = 15), meta = (ClampMin = 1, ClampMax = 15))
	int64 MaxBotThinkTime = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MUS| Options")
	TObjectPtr<USoundMix> SoundMix;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MUS| Options")
	TObjectPtr<USoundClass> MusicClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MUS| Options")
	TObjectPtr<USoundClass> SFXClass;

	void ShowTable();
	void StartGame();
	UFUNCTION(BlueprintCallable)
	void ApplySettings();
	void SetMusRules(FMusRules NewMusRules){ CurrentMusRules = NewMusRules;}
	UFUNCTION(BlueprintCallable)
	void SetMasterPostProcess(APostProcessVolume* _MasterPostProcess){ MasterPostProcess = _MasterPostProcess; }
	FMusRules GetCurrentMusRules(){ return CurrentMusRules; }
	//---------------Participants Functions----------------------------------//
	void ParticipantCallsAMove(EParticipant Participant, EMoves Move, int32 EnvidoRocks = 0);
	int32 GetCurrentBetOnTable(){ return CurrentBetOnTable; }
	FParticipantStruct GetParticipantStruct(EParticipant Participant){ return ParticipantsInfo[Participant]; }
	bool AreAnyParticipantCloseToWinning();
	void DiscardParticipantCards(EParticipant Participant, TArray<int32> CardsToBeDiscarded);
	bool IsThereOrdago(){ return SomeonePlayedOrdago; }
	
protected:
	TObjectPtr<APostProcessVolume> MasterPostProcess;
	
	void OnStart() override;

private:
	TObjectPtr<UMus_UserSettings> UserSettings;
	FParticipantStruct Bot1;
	FParticipantStruct Bot2;
	FParticipantStruct Bot3;
	FParticipantStruct Player;
	TArray<FCards_Struct*> GameCards;
	FMusRules CurrentMusRules;
	FTimerHandle StartGameHandle;
	TMap<EParticipant , FParticipantStruct> ParticipantsInfo;
	TArray<EParticipant> OrderOfParticipantsInGame;
	int32 CurrentParticipantIDInTurn = 0;
	int32 CurrentGameCardIndexInDeck = 0;
	int32 AmountOfParticipantsThatWantMus = 0;
	bool InDiscardPhase = false;
	int32 AmountOfParticipantsThatDiscarded = 0;
	FTimerHandle GiveCardsTimer;
	bool BetsStarted = false;
	EBettingPhase CurrentBettingPhase = NONE;
	int32 CurrentBetOnTable = 0;
	bool SomeonePlayedOrdago = false;
	int32 ParticipantThatRaisedTheBet = 0;
	TArray<EParticipant> ParticipantsInTheCurrentBet;
	TMap<EParticipant, TMap<int32, int32>> ParticipantsAllCards;
	TObjectPtr<UMusTable> MusTable;
	FTimerHandle BotActionTimer;
	TArray<EParticipant> ParticipantsThatHavePares;
	TArray<EParticipant> ParticipantsThatHaveJuego;
	//---------------WINNERS----------------------------------//
	FTimerHandle WinnerTimer;
	EParticipant WinnerOfGrande = NOONE;
	EParticipant WinnerOfChica = NOONE;
	EParticipant WinnerOfPares = NOONE;
	EParticipant WinnerOfJuego = NOONE;
	EParticipant WinnerOfOrdago = NOONE;
	bool JuegoWentToPunto = false;
	int32 GrandeAmountWon = 0;
	int32 ChicaAmountWon = 0;
	int32 ParesAmountWon = 0;
	int32 JuegoAmountWon = 0;

	//---------------FUNCTIONS----------------------------------//
	void SetValueOfCards();
	void AddCardToDeck(FCards_Struct* Card);
	void ShuffleAndGiveCards();
	UFUNCTION()
	void EndStartingGame();
	//Give new cards to participants that discarded, wait a while and then, continue the game
	void GiveNewCardsToParticipants();
	UFUNCTION()
	void EndDiscardPhase();
	//Sets a random time for bots to think their next move
	void StartParticipantAction();
	void ResetBotEnvidos();
	UFUNCTION()
	void FinishBotAction();

	//---------------PLAYS----------------------------------//
	//Check the play a participant has
	void DetectWinnerOfParesAndPrepareForJuego();
	void DetectWinnerOfJuego();
	void CheckFrequencyOfCardsInHand();
	EParticipant CheckGrande();
	EParticipant CheckChica();
	EParticipant CheckPares();
	EParticipant CheckPokerOrTrio(TMap<EParticipant, int32> ParticipantsWithPokerOrTrio);
	EParticipant CheckPairsIndividually(TMap<EParticipant, int32> ParticipantsWithADuo, TMap<EParticipant, int32> ParticipantsWithAnotherDuo);
	EParticipant CheckJuego();
	EParticipant CheckPunto();

	int32 CheckMusRulesFor3sAnd2s(FCards_Struct* Card);
	void ChangeHand();
	void PassTurn();
	void UpdateParticipantPiedras(EParticipant Participant);
	void OrdagoInPlay();
	void ShowWinners();
	UFUNCTION()
	void ShowGrandeWinner();
	UFUNCTION()
	void ShowChicaWinner();
	UFUNCTION()
	void ShowPairWinner();
	UFUNCTION()
	void ShowJuegoWinner();
	UFUNCTION()
	void EndShowingWinners();
	UFUNCTION()
	void EndShowingOrdagoWinner();
	UFUNCTION()
	void NextRound();
};
