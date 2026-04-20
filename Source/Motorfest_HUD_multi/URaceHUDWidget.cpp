// Fill out your copyright notice in the Description page of Project Settings.


#include "URaceHUDWidget.h"

#include "ARacePlayerState.h"
#include "LocalizationDescriptor.h"
#include "Motorfest_HUD_multiPlayerController.h"
#include "Blueprint/WidgetTree.h"
#include "Components/HorizontalBox.h"
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
	
	// Pré-crée les TextBlocks une seule fois
	if (LeaderboardBox)
	{
		for (int32 i = 0; i < MaxPlayers; i++)
		{
			UTextBlock* Line = WidgetTree->ConstructWidget<UTextBlock>(
				UTextBlock::StaticClass(),
				FName(*FString::Printf(TEXT("LeaderboardLine_%d"), i))
			);

			FSlateFontInfo Font = Line->GetFont();
			Font.Size = 14;
			Line->SetFont(Font);

			UVerticalBoxSlot* uvslot = LeaderboardBox->AddChildToVerticalBox(Line);
			if (uvslot)
			{
				uvslot->SetPadding(FMargin(2.f, 3.f));
				uvslot->SetHorizontalAlignment(HAlign_Fill);
				uvslot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
			}

			Line->SetVisibility(ESlateVisibility::Collapsed); // caché par défaut
			LeaderboardLines.Add(Line);
		}
	}
	
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
			LocalSpeedKm = LocalPawn->GetVelocity().Size() / 100.f * 3.6f;
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
		
		for (int32 i = 0; i < LeaderboardLines.Num(); i++)
		{
			UTextBlock* Line = LeaderboardLines[i];
			if (!Line) continue;

			if (i < LeaderboardEntries.Num())
			{
				const FLeaderboardEntry& Entry = LeaderboardEntries[i];

				Line->SetText(FText::FromString(FString::Printf(
					TEXT("%d. %s  [%dms]"),
					Entry.Position,
					*Entry.PlayerName,
					Entry.PingMs
				)));

				Line->SetColorAndOpacity(Entry.bIsLocal
					? FSlateColor(FLinearColor::Yellow)
					: FSlateColor(FLinearColor::White));

				Line->SetVisibility(ESlateVisibility::Visible);
			}
			else
			{
				// Pas de joueur pour cette ligne — on cache
				Line->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
		
		/*if (LeaderboardBox)
		{
			LeaderboardBox->ClearChildren();
			for (const FLeaderboardEntry& Entry : LeaderboardEntries)
			{
				APlayerController* PC = GetOwningPlayer();
				if (!PC) continue;

				// CreateWidget avec le PC comme outer — c'est la façon correcte
				UTextBlock* Line = NewObject<UTextBlock>(PC);
				if (!Line) continue;
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
				
				UVerticalBoxSlot* vslot = LeaderboardBox->AddChildToVerticalBox(Line);
				if (vslot)
				{
					vslot->SetPadding(FMargin(0.f, 2.f));
					vslot->SetHorizontalAlignment(HAlign_Fill);
					vslot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
				}
			}
		}*/
	}
}
