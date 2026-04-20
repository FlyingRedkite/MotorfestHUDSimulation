// Fill out your copyright notice in the Description page of Project Settings.


#include "URaceHUDWidget.h"

#include "ARacePlayerState.h"
#include "LocalizationDescriptor.h"
#include "Motorfest_HUD_multiPlayerController.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

void URaceHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateFromGameState();
}

void URaceHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (APlayerController* PC = GetOwningPlayer())
	{
		PlayerState = PC->GetPlayerState<ARacePlayerState>();
	}

	RaceGameState = Cast<ARaceGameState>(GetWorld()->GetGameState());
}

void URaceHUDWidget::UpdateFromGameState()
{
	if (AMotorfest_HUD_multiPlayerController* pc = GetOwningPlayer<AMotorfest_HUD_multiPlayerController>())
	{
		if (!PlayerState)
			PlayerState = pc->GetPlayerState<ARacePlayerState>();
		
		//We use the local speed to show the speed instead of the player state speed (replicated 
		if (APawn* LocalPawn = pc->GetPawn())
		{
			LocalSpeedKm  = LocalPawn->GetVelocity().Size() / 100.f * 3.6f;
		}
		
		
	}
	if (!RaceGameState)
		RaceGameState = Cast<ARaceGameState>(GetWorld()->GetGameState());
	if (RaceGameState)
	{
		RaceTime = RaceGameState->RaceTime;

		LeaderboardEntries.Empty();
		for (APlayerState* ps : RaceGameState->PlayerArray)
		{
			if (ARacePlayerState* RacePlayerState = Cast<ARacePlayerState>(ps))
			{
				FLeaderboardEntry LeaderboardEntry;
				LeaderboardEntry.PlayerName = RacePlayerState->GetPlayerName();
				LeaderboardEntry.Position = RacePlayerState->RacePosition;
				LeaderboardEntry.PingMs = FMath::RoundToInt(RacePlayerState->GetPingInMilliseconds());
				LeaderboardEntry.bIsLocal = (RacePlayerState == PlayerState);
				LeaderboardEntries.Add(LeaderboardEntry);
			}
		}
		LeaderboardEntries.Sort([](const FLeaderboardEntry& A, const FLeaderboardEntry& B)
		{
			return A.Position < B.Position;
		});

		if (LeaderboardBox)
		{
			LeaderboardBox->ClearChildren();
			for (const FLeaderboardEntry& Entry : LeaderboardEntries)
			{
				UTextBlock* Line = NewObject<UTextBlock>(this, UTextBlock::StaticClass());

				FString LineText = FString::Printf(
					TEXT("%d. %s  [%dms]"),
					Entry.Position,
					*Entry.PlayerName,
					Entry.PingMs
				);
				
				Line->SetText(FText::FromString(LineText));

				// Couleur nom
				FSlateColor Color = Entry.bIsLocal
					? FSlateColor(FLinearColor::Yellow)
					: FSlateColor(FLinearColor::White);
				Line->SetColorAndOpacity(Color);

				// Taille de la font
				FSlateFontInfo FontInfo = Line->GetFont();
				FontInfo.Size = 14;
				Line->SetFont(FontInfo);

				// AddChildToVerticalBox au lieu de AddChild
				// Ça crée le slot avec les bonnes propriétés de layout
				if (UVerticalBoxSlot* slot = LeaderboardBox->AddChildToVerticalBox(Line))
				{
					slot->SetPadding(FMargin(2.f, 2.f)); 
					slot->SetHorizontalAlignment(HAlign_Fill);
					slot->SetSize(ESlateSizeRule::Automatic);
					slot->SetHorizontalAlignment(HAlign_Fill);
					slot->SetVerticalAlignment(VAlign_Center);
				}
				
			}
		}
	}
}
