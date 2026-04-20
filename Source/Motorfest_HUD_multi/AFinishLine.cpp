// Fill out your copyright notice in the Description page of Project Settings.


#include "AFinishLine.h"

#include "ARacePlayerState.h"
#include "Motorfest_HUD_multiCharacter.h"
#include "Motorfest_HUD_multiPlayerController.h"
#include "Components/BoxComponent.h"


// Sets default values
AFinishLine::AFinishLine()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	FinishLine = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = FinishLine;
	
	FinishLine->SetCollisionProfileName(TEXT("Trigger"));

	FinishLine->OnComponentBeginOverlap.AddDynamic(
		this, &AFinishLine::OnOverlapBegin);
}

// Called when the game starts or when spawned
void AFinishLine::BeginPlay()
{
	Super::BeginPlay();
}

void AFinishLine::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority()) return;

	if (AMotorfest_HUD_multiCharacter* Car = Cast<AMotorfest_HUD_multiCharacter>(OtherActor))
	{
		if (ARacePlayerState* PS = Car->GetPlayerState<ARacePlayerState>())
		{
			if (PS->bFinishedRace) return;
			
			PS->bFinishedRace = true;
			// Tu peux logger le temps d'arrivée ici aussi
			UE_LOG(LogTemp, Warning, TEXT("%s a terminé !"), *PS->GetPlayerName());
			
			// Récupère le PlayerController de CE joueur et envoie le RPC
			if (AMotorfest_HUD_multiPlayerController* PC =
				Cast<AMotorfest_HUD_multiPlayerController>(Car->GetController()))	
			{
				PC->ClientShowFinished(); // RPC → s'exécute sur le client 
				PC->DisableInput(PC); ///make player enable to move
			}
			
		}
	}
	
}

// Called every frame
void AFinishLine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

