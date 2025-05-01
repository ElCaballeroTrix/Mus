// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "MusModerno/Data/MusManager.h"
#include "MusTable.generated.h"

class UHomeMenu;
class UGameOver;
class UCommonActivatableWidget;
class UCommonActivatableWidgetStack;
class UVerticalBox;
class UImage;
class UCommonTextBlock;
class UEnvidoWidget;
struct FCards_Struct;
enum EParticipant : int;
class UCanvasPanel;
class UCardWidget;
class UMusButton;
class UMusManager;
/**
 * 
 */
UCLASS()
class MUSMODERNO_API UMusTable : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	void SetParticipantsCards(EParticipant Participant, TArray<FCards_Struct*> NewCards);
	void ShowPlayerPossibleActions(TArray<EMoves> PossiblePlayerMoves);
	void UpdateTableBet(int32 BetValue);
	void UpdatePhase(EBettingPhase Phase, bool Winner = false, EParticipant Participant = PLAYER);
	void EmptyPhaseText();
	void UpdatePiedras(EParticipant Participant, int32 Amarrakos, int32 Piedras);
	void UpdatePlay(EParticipant Participant, EMoves Move);
	void ResetPlays();
	void ParticipantsCardsDiscard(EParticipant Participant, TArray<int32> CardsToDiscard, bool IsInDiscardingPhase = true);
	void ActivePlayerCardsForDiscard();
	void UpdateHand(EParticipant ParticipantHand);
	void StandOutACard(EParticipant Participant, int32 IDOfCard, bool ShouldStandOut);
	void ResetGlowOfAllCards();
	void ShowAllCards();
	void GameEnded(bool PlayerWon);
	
protected:
	//---------------Widgets----------------------------------//
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UCommonActivatableWidgetStack> TableStack;
	//Action Buttons
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UVerticalBox> ActionButtonsBox;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UMusButton> MusActionButton;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UMusButton> NoMusActionButton;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UMusButton> UpperActionButton;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UMusButton> LowerActionButton;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UMusButton> EnvidoActionButton;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UMusButton> OrdagoActionButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UEnvidoWidget> EnvidoWidget;

	//---------------TEXT----------------------------------------------//
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> TableBet;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> PhaseTextBlock;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> Bot1Piedras;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> Bot2Piedras;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> Bot3Piedras;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> PlayerPiedras;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> Bot1PlayText;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> Bot2PlayText;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> Bot3PlayText;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> PlayerPlayText;

	//---------------CARDS--------------------------------------------//
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UCanvasPanel> PlayerCardsWidget;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UCanvasPanel> Bot1CardsWidget;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UCanvasPanel> Bot2CardsWidget;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UCanvasPanel> Bot3CardsWidget;
	
	//---------------HAND---------------------------------------------//
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UImage> PlayerHand;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UImage> Bot1Hand;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UImage> Bot2Hand;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UImage> Bot3Hand;
	//---------------MUS TABLE VARIABLES--------------------------------//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MUS| Mus Table Info")
	float DiscardTime = 2.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MUS| Mus Table Info| Classes")
	TSubclassOf<UCommonActivatableWidget> GameOverClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MUS| Mus Table Info| Classes")
	TSubclassOf<UHomeMenu> HomeMenuClass;
	
	//---------------Native Functions----------------------------------//
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
private:
	TObjectPtr<UMusManager> MusManager;
	TArray<TObjectPtr<UCardWidget>> PlayerCurrentCards;
	TArray<TObjectPtr<UCardWidget>> Bot1CurrentCards;
	TArray<TObjectPtr<UCardWidget>> Bot2CurrentCards;
	TArray<TObjectPtr<UCardWidget>> Bot3CurrentCards;
	FTimerHandle DiscardTimerHandle;
	EParticipant ParticipantDiscarting;
	TObjectPtr<UGameOver> GameOver;
	
	UFUNCTION()
	void PlayerCallsMus();
	UFUNCTION()
	void PlayerCallsNoMus();
	UFUNCTION()
	void PlayerCallsEnvido();
	UFUNCTION()
	void PlayerAcceptsEnvido();
	UFUNCTION()
	void PlayerDeniesEnvido();
	UFUNCTION()
	void PlayerPasses();
	UFUNCTION()
	void PlayerHasPairOrJuego();
	UFUNCTION()
	void PlayerDoesNotHavePairOrJuego();
	UFUNCTION()
	void PlayersCallsOrdago();
	UFUNCTION()
	void PlayersCallsDiscard();
	
	void HideAllButtons();
	void SetCardsWidgets(TObjectPtr<UCanvasPanel> ParticipantCardsWidget , EParticipant Participant);
	void WaitUntilDiscardAnimationIsComplete();
	void DiscardOfParticipantFinished();
	UFUNCTION()
	void ReturnToHomeMenu();
};
