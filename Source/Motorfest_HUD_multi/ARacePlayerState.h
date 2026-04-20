// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ARacePlayerState.generated.h"

/**
 * 
 */
UCLASS()
class MOTORFEST_HUD_MULTI_API ARacePlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, Replicated)
	float Speed = 0.0f;
	
	UPROPERTY(BlueprintReadOnly, Replicated)
	int32 RacePosition = 1;
	
	UPROPERTY(BlueprintReadOnly, Replicated)
	float DistanceToFinish = 0.0f;
	
	UPROPERTY(BlueprintReadOnly, Replicated)
	bool bFinishedRace = false;

	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

};
