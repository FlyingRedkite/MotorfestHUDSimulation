
#pragma once

#include "CoreMinimal.h"
#include "ARaceGameState.h"
#include "ARacePlayerState.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "URaceHUDWidget.generated.h"

/**
 * Structure utilisée pour dans le classement 
 */
USTRUCT(BlueprintType)
struct FLeaderboardEntry
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly)
	FString PlayerName;
	UPROPERTY(BlueprintReadOnly)
	int32 Position;
	UPROPERTY(BlueprintReadOnly)
	int32 PingMs = 0;
	UPROPERTY(BlueprintReadOnly)
	bool bIsLocal = false;
};


UCLASS()
class MOTORFEST_HUD_MULTI_API URaceHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	//Local player speeed in km/h
	UPROPERTY(BlueprintReadOnly)
	float LocalSpeedKm = 0.0f;

	//Local List of all players used in the leaderboard
	UPROPERTY(BlueprintReadOnly)
	TArray<FLeaderboardEntry> LeaderboardEntries;

	//Temps de course
	UPROPERTY(BlueprintReadOnly)
	float RaceTime = 0.0f;
	
	UPROPERTY(meta=(BindWidget))
	class UVerticalBox* LeaderboardBox;

protected:
	//utilisé pour mettre à jour les données
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	virtual void NativeConstruct() override;
	
private:
	UPROPERTY()
	ARacePlayerState* PlayerState;
	
	UPROPERTY()
	ARaceGameState* RaceGameState;
	
	void UpdateFromGameState();
	
	UPROPERTY()
	TArray<UTextBlock*> LeaderboardLines;

	static constexpr int32 MaxPlayers = 4;
	
};
