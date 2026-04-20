// Fill out your copyright notice in the Description page of Project Settings.


#include "ARaceGameState.h"

#include "Net/UnrealNetwork.h"

void ARaceGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ARaceGameState, RaceTime);
	DOREPLIFETIME(ARaceGameState, RankedPlayerIds);
	DOREPLIFETIME(ARaceGameState, bIsFinished);
	
}
