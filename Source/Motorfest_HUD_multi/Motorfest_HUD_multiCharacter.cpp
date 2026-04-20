// Copyright Epic Games, Inc. All Rights Reserved.

#include "Motorfest_HUD_multiCharacter.h"

#include "ARacePlayerState.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Motorfest_HUD_multi.h"
#include "Kismet/GameplayStatics.h"

AMotorfest_HUD_multiCharacter::AMotorfest_HUD_multiCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	//OVERRIDE : On essaie de supprimer le mesh 
	GetMesh()->SetVisibility(false);
	GetMesh()->DestroyComponent();
}

void AMotorfest_HUD_multiCharacter::BeginPlay()
{
	Super::BeginPlay();
	// Trouve la finish line dans le level
	TArray<AActor*> Found;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),
	                                      AFinishLine::StaticClass(), Found);
	if (Found.Num() > 0)
		FinishLineRef = Cast<AFinishLine>(Found[0]);
}

void AMotorfest_HUD_multiCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this,
		                                   &AMotorfest_HUD_multiCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this,
		                                   &AMotorfest_HUD_multiCharacter::Look);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this,
		                                   &AMotorfest_HUD_multiCharacter::Look);
	}
	else
	{
		UE_LOG(LogMotorfest_HUD_multi, Error,
		       TEXT(
			       "'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."
		       ), *GetNameSafe(this));
	}
}

void AMotorfest_HUD_multiCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void AMotorfest_HUD_multiCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void AMotorfest_HUD_multiCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void AMotorfest_HUD_multiCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AMotorfest_HUD_multiCharacter::DoJumpStart()
{
	// signal the character to jump
	Jump();
}

void AMotorfest_HUD_multiCharacter::DoJumpEnd()
{
	// signal the character to stop jumping
	StopJumping();
}

void AMotorfest_HUD_multiCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (IsLocallyControlled() && FinishLineRef)
	{
		//Nouvelle vitesse en cm/s
		float CurrentSpeed = GetVelocity().Size();

		if (FMath::Abs(CurrentSpeed - LastSentSpeed) > 5.0f)
		{
			ServerUpdateSpeed(CurrentSpeed);
			LastSentSpeed = CurrentSpeed;
		}

		float Dist = FVector::Dist(GetActorLocation(), FinishLineRef->GetActorLocation());
		ServerUpdateDistance(Dist);
	}
}

void AMotorfest_HUD_multiCharacter::ServerUpdateDistance_Implementation(float NewDistance)
{
	if (ARacePlayerState* PS = GetPlayerState<ARacePlayerState>())
		PS->DistanceToFinish = NewDistance;
}


void AMotorfest_HUD_multiCharacter::ServerUpdateSpeed_Implementation(float NewSpeed)
{
	if (ARacePlayerState* ps = GetPlayerState<ARacePlayerState>())
	{
		ps->Speed = NewSpeed;
	}
}
