// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AFinishLine.generated.h"

UCLASS()
class MOTORFEST_HUD_MULTI_API AFinishLine : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFinishLine();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UBoxComponent* FinishLine;
	
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
						AActor* OtherActor,
						UPrimitiveComponent* OtherComp,
						int32 OtherBodyIndex,
						bool bFromSweep,
						const FHitResult& SweepResult);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
