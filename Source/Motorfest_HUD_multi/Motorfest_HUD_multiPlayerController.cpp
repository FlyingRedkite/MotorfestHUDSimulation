// Copyright Epic Games, Inc. All Rights Reserved.


#include "Motorfest_HUD_multiPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
#include "Motorfest_HUD_multi.h"
#include "Widgets/Input/SVirtualJoystick.h"

void AMotorfest_HUD_multiPlayerController::ClientShowFinished_Implementation()
{
	// S'exécute sur le client uniquement
	if (FinishedWidgetClass)
	{
		FinishedWidget = CreateWidget<UUserWidget>(this, FinishedWidgetClass);
		if (FinishedWidget)
			FinishedWidget->AddToViewport(10); // ZOrder 10 = au-dessus du HUD
	}
}

void AMotorfest_HUD_multiPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// only spawn touch controls on local player controllers
	if (ShouldUseTouchControls() && IsLocalPlayerController())
	{
		// spawn the mobile controls widget
		MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);

		if (MobileControlsWidget)
		{
			// add the controls to the player screen
			MobileControlsWidget->AddToPlayerScreen(0);
		}
		else
		{
			UE_LOG(LogMotorfest_HUD_multi, Error, TEXT("Could not spawn mobile controls widget."));
		}
	}
	if (IsLocalController() && HUDWidgetClass)
	{
		HUDWidget = CreateWidget<URaceHUDWidget>(this, HUDWidgetClass);
		if (HUDWidget)
		{
			HUDWidget->AddToViewport();
		}
	}
}

void AMotorfest_HUD_multiPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// only add IMCs for local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Contexts
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}

			// only add these IMCs if we're not using mobile touch input
			if (!ShouldUseTouchControls())
			{
				for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}
		}
	}
}

bool AMotorfest_HUD_multiPlayerController::ShouldUseTouchControls() const
{
	// are we on a mobile platform? Should we force touch?
	return SVirtualJoystick::ShouldDisplayTouchInterface() || bForceTouchControls;
}
