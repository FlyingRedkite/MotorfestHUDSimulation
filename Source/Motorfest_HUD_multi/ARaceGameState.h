// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ARacePlayerState.h"
#include "GameFramework/GameStateBase.h"
#include "ARaceGameState.generated.h"

/**
 * 
 */
UCLASS()
class MOTORFEST_HUD_MULTI_API ARaceGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	//Temps de course - répliqués vers les clients
	UPROPERTY(Replicated)
	float RaceTime = 0.0f;

	// IDs des joueurs dans l'ordre du classement (1er en index 0)
	// Mis à jour par le serveur, répliqué vers les clients
	UPROPERTY(Replicated)
	TArray<int32> RankedPlayerIds;

	UPROPERTY(Replicated)
	bool bIsFinished = false;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
