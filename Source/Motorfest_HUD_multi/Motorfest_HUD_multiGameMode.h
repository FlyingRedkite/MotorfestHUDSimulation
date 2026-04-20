// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Motorfest_HUD_multiGameMode.generated.h"

/**
 *  Simple GameMode for a third person game
 */
UCLASS(abstract)
class AMotorfest_HUD_multiGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	/** Constructor */
	AMotorfest_HUD_multiGameMode();

	virtual void Tick(float DeltaSeconds) override;

private:
	UFUNCTION(Server, Reliable)
	void UpdateRanking();

	float LastTimeSinceRanking = 0.f;
	float RankUpdateInterval = 0.5f;
};
