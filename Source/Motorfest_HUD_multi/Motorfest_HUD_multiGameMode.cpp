// Copyright Epic Games, Inc. All Rights Reserved.

#include "Motorfest_HUD_multiGameMode.h"

#include "ARaceGameState.h"
#include "ARacePlayerState.h"
#include "Motorfest_HUD_multiCharacter.h"
#include "Motorfest_HUD_multiPlayerController.h"

AMotorfest_HUD_multiGameMode::AMotorfest_HUD_multiGameMode()
{
	PrimaryActorTick.bCanEverTick = true;

	// stub
	DefaultPawnClass = AMotorfest_HUD_multiCharacter::StaticClass();
	PlayerStateClass = ARacePlayerState::StaticClass();
	GameStateClass = ARaceGameState::StaticClass();
	PlayerControllerClass = AMotorfest_HUD_multiPlayerController::StaticClass();
}

void AMotorfest_HUD_multiGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	LastTimeSinceRanking += DeltaSeconds;

	//tous les 500ms on update le ranking de nos joueurs
	if (LastTimeSinceRanking >= RankUpdateInterval)
	{
		LastTimeSinceRanking = 0.0f;
		UpdateRanking();
	}
}

void AMotorfest_HUD_multiGameMode::UpdateRanking_Implementation()
{
	//UE_LOG(LogTemp, Display, TEXT("UpdateRanking"));

	ARaceGameState* raceGameState = GetGameState<ARaceGameState>();
	if (!raceGameState) return;

	TArray<ARacePlayerState*> racePlayerStates;
	for (APlayerState* playerState : raceGameState->PlayerArray)
	{
		//Cast les players state dans notre player state custom pour le jeu
		if (ARacePlayerState* RacePlayerState = Cast<ARacePlayerState>(playerState))
		{
			racePlayerStates.Add(RacePlayerState);
		}
	}

	//On classe les joeurs en fonction de leur vitesse -> A remplacer plus tard par  
	racePlayerStates.Sort([](const ARacePlayerState& A, const ARacePlayerState& B)
	{
		//Finisher en premier 
		if (A.bFinishedRace != B.bFinishedRace)
			return A.bFinishedRace > B.bFinishedRace;
		
		//Sinon on se base sur la distance de l'arrivée
		return A.DistanceToFinish < B.DistanceToFinish;
	});

	raceGameState->RankedPlayerIds.Empty();
	for (int i = 0; i < racePlayerStates.Num(); ++i)
	{
		racePlayerStates[i]->RacePosition = i + 1;
		raceGameState->RankedPlayerIds.Add(racePlayerStates[i]->GetPlayerId());
	}

	raceGameState->RaceTime += RankUpdateInterval;
}
