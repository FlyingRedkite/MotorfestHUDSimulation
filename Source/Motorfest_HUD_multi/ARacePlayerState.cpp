// Fill out your copyright notice in the Description page of Project Settings.


#include "ARacePlayerState.h"
#include "Net/UnrealNetwork.h"

void ARacePlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ARacePlayerState, Speed);
	DOREPLIFETIME(ARacePlayerState, RacePosition);
	DOREPLIFETIME(ARacePlayerState, DistanceToFinish);
	DOREPLIFETIME(ARacePlayerState, bFinishedRace);
}