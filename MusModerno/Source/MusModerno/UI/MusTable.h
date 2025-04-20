// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "MusModerno/Data/MusManager.h"
#include "MusTable.generated.h"

class UCommonTextBlock;
class UEnvidoWidget;
struct FCards_Struct;
enum EParticipant : int;
class UCanvasPanel;
class UCardWidget;
class UHorizontalBox;
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
	void UpdatePiedras(EParticipant Participant, int32 Amarrakos, int32 Piedras);
	void UpdatePlay(EParticipant Participant, EMoves Move);
	void ResetPlays();
	
protected:
	//---------------Widgets----------------------------------//
	//Action Buttons
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UHorizontalBox> ActionButtonsBox;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UMusButton> LeftActionButton;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UMusButton> RightActionButton;
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
	
	//---------------Native Functions----------------------------------//
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
private:
	TObjectPtr<UMusManager> MusManager;
	TArray<TObjectPtr<UCardWidget>> PlayerCurrentCards;
	TArray<TObjectPtr<UCardWidget>> Bot1CurrentCards;
	TArray<TObjectPtr<UCardWidget>> Bot2CurrentCards;
	TArray<TObjectPtr<UCardWidget>> Bot3CurrentCards;
	
	UFUNCTION()
	void PlayerCallsMus();
	UFUNCTION()
	void PlayersCallsNoMus();
	UFUNCTION()
	void PlayersCallsEnvido();
	UFUNCTION()
	void PlayersAcceptsEnvido();
	UFUNCTION()
	void PlayersPasses();
	UFUNCTION()
	void PlayerHasPairOrJuego();
	UFUNCTION()
	void PlayerDoesNotHavePairOrJuego();
	UFUNCTION()
	void PlayersCallsOrdago();
	
	void HideAllButtons();
	void SetCardsWidgets(TObjectPtr<UCanvasPanel> ParticipantCardsWidget , EParticipant Participant);
};
