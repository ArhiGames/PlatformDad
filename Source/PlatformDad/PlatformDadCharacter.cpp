// Copyright Epic Games, Inc. All Rights Reserved.

#include "PlatformDadCharacter.h"

#include "EffectUserWidget.h"
#include "EngineUtils.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Blueprint/UserWidget.h"
#include "Net/UnrealNetwork.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Public/Checkpoint.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

APlatformDadCharacter::APlatformDadCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 500.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	bReplicates = true;
}

void APlatformDadCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		StartPosition = GetActorLocation();
		DefaultMaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
		StartJumpZVelocity = GetCharacterMovement()->JumpZVelocity;
	}

	if (IsLocallyControlled())
	{
		CreatedEffectUserWidget = CreateWidget<UEffectUserWidget>(GetWorld(), EffectUserWidgetClass);
		if (CreatedEffectUserWidget)
		{
			CreatedEffectUserWidget->AddToViewport();
		}
	}
}

void APlatformDadCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlatformDadCharacter, Checkpoints)
	DOREPLIFETIME(APlatformDadCharacter, StartPosition);
	DOREPLIFETIME(APlatformDadCharacter, DefaultMaxWalkSpeed);
	DOREPLIFETIME(APlatformDadCharacter, StartJumpZVelocity);
}

void APlatformDadCharacter::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

#if WITH_SERVER_CODE
	if (HasAuthority() && GetActorLocation().Z < -50.f)
	{
		ServerResetToCheckpoint(GetActorLocation());
	}
#endif
}

bool APlatformDadCharacter::IsCheckpointActivated(ACheckpoint* Checkpoint) const
{
	return Checkpoints.Contains(Checkpoint);
}

void APlatformDadCharacter::NetMulticastSetFriction_Implementation(const float GroundFriction,
	const int32 BrakingDecelerationWalking, const int32 BrakingDecelerationFalling)
{
	GetCharacterMovement()->GroundFriction = GroundFriction;
	GetCharacterMovement()->BrakingDecelerationWalking = BrakingDecelerationWalking;
	GetCharacterMovement()->BrakingDecelerationFalling = BrakingDecelerationFalling;
}

void APlatformDadCharacter::NetMulticastSetAllowJump_Implementation(const bool AllowJump)
{
	GetCharacterMovement()->SetJumpAllowed(AllowJump);
}

void APlatformDadCharacter::NetMulticastSpawnParticleSystem_Implementation(UNiagaraSystem* NiagaraSystem, const FVector& Location)
{
	if (NiagaraSystem)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NiagaraSystem, Location);
	}
}

void APlatformDadCharacter::NetMulticastSetJumpZVelocity_Implementation(const float NewVelocity)
{
	GetCharacterMovement()->JumpZVelocity = NewVelocity;
}

void APlatformDadCharacter::ClientSetPlayerUserInterface_Implementation(const FPowerUpData& PowerUpData)
{
	CreatedEffectUserWidget->SetEffectUserData(PowerUpData);
}

void APlatformDadCharacter::ServerOnRandomPickupTakenUp_Implementation(APlatformDadCharacter* TriggeringCharacter)
{
	if (HasAuthority())
	{
		const FPowerUpData PowerUpData = AllPowerUpsData[FMath::RandRange(0, AllPowerUpsData.Num() - 1)];
		UE_LOG(LogTemp, Warning, TEXT("PowerUpData: %s"), *PowerUpData.LocalId);
		TArray<APlatformDadCharacter*> OtherCharacters{};

		for (TActorIterator<APlatformDadCharacter> It(GetWorld()); It; ++It)
		{
			APlatformDadCharacter* Character = *It;
			if (Character != TriggeringCharacter)
			{
				OtherCharacters.Add(Character);
			}
		}

		TriggeringCharacter->ClientSetPlayerUserInterface(PowerUpData);
		for (APlatformDadCharacter* Character : OtherCharacters)
		{
			Character->ClientSetPlayerUserInterface(PowerUpData);
		}
		
		if (PowerUpData.LocalId == "Slowness")
		{
			FTimerHandle& TriggeringCharacterTimerHandle = TimerHandleResetSpeed.FindOrAdd(TriggeringCharacter);
			TriggeringCharacter->NetMulticastSetWalkSpeed(TriggeringCharacter->DefaultMaxWalkSpeed * 1.25f);
			GetWorldTimerManager().SetTimer(TriggeringCharacterTimerHandle, [this, TriggeringCharacter]()
			{
				TriggeringCharacter->NetMulticastSetWalkSpeed(TriggeringCharacter->DefaultMaxWalkSpeed);
			}, PowerUpData.Seconds, false);
			
			for (APlatformDadCharacter* Character : OtherCharacters)
			{
				Character->NetMulticastSetWalkSpeed(Character->DefaultMaxWalkSpeed / 2.f);

				FTimerHandle& TimerHandle = TimerHandleResetSpeed.FindOrAdd(Character);
				GetWorldTimerManager().SetTimer(TimerHandle, [this, Character]()
				{
					Character->NetMulticastSetWalkSpeed(Character->DefaultMaxWalkSpeed);
				}, PowerUpData.Seconds, false);
			}
		}
		else if (PowerUpData.LocalId == "SlownessEverybody")
		{
			for (APlatformDadCharacter* Character : OtherCharacters)
			{
				Character->NetMulticastSetWalkSpeed(Character->DefaultMaxWalkSpeed / 5);

				FTimerHandle& TimerHandle = TimerHandleResetSpeed.FindOrAdd(Character);
				GetWorldTimerManager().SetTimer(TimerHandle, [this, Character]()
				{
					Character->NetMulticastSetWalkSpeed(Character->DefaultMaxWalkSpeed);
				}, PowerUpData.Seconds, false);
			}
		}
		else if (PowerUpData.LocalId == "JumpBoost")
		{
			TriggeringCharacter->NetMulticastSetJumpZVelocity(StartJumpZVelocity * 1.5f);

			FTimerHandle& TimerHandle = TimerHandleJumpBoost.FindOrAdd(TriggeringCharacter);
			GetWorldTimerManager().SetTimer(TimerHandle, [this, TriggeringCharacter]()
			{
				TriggeringCharacter->NetMulticastSetJumpZVelocity(StartJumpZVelocity);
			}, PowerUpData.Seconds, false);
		}
		else if (PowerUpData.LocalId == "BlockJump")
		{
			for (APlatformDadCharacter* Character : OtherCharacters)
			{
				FTimerHandle& TimerHandle = TimerHandleJumpBlock.FindOrAdd(Character);
				Character->NetMulticastSetAllowJump(false);
				
				GetWorldTimerManager().SetTimer(TimerHandle, [this, Character]()
				{
					Character->NetMulticastSetAllowJump(true);
				}, PowerUpData.Seconds, false);
			}
		}
		else if (PowerUpData.LocalId == "IcyGround")
		{
			FTimerHandle& TriggeringCharacterTimerHandle = TimerHandleIcyGround.FindOrAdd(TriggeringCharacter);
			TriggeringCharacter->NetMulticastSetFriction(0.2f, 0, 0);
			GetWorldTimerManager().SetTimer(TriggeringCharacterTimerHandle, [this, TriggeringCharacter]()
			{
				TriggeringCharacter->NetMulticastSetFriction(8.f, 2000, 1500);
			}, PowerUpData.Seconds, false);
			
			for (APlatformDadCharacter* Character : OtherCharacters)
			{
				FTimerHandle& TimerHandle = TimerHandleIcyGround.FindOrAdd(Character);
				Character->NetMulticastSetFriction(0.2f, 0, 0);

				GetWorldTimerManager().SetTimer(TimerHandle, [this, Character]()
				{
					Character->NetMulticastSetFriction(8.f, 2000, 1500);	
				}, PowerUpData.Seconds, false);
			}
		}
	}
}

void APlatformDadCharacter::NetMulticastSetWalkSpeed_Implementation(const float NewSpeed)
{
	GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
}

void APlatformDadCharacter::ServerResetToCheckpoint_Implementation(const FVector& OldLocation)
{
	if (Checkpoints.Num() > 0)
	{
		SetActorLocation(Checkpoints[Checkpoints.Num() - 1]->GetActorLocation());
	}
	else
	{
		SetActorLocation(StartPosition);
	}
	NetMulticastSpawnParticleSystem(NiagaraKillEffect, GetActorLocation());
}

void APlatformDadCharacter::SetLastCheckpoint(ACheckpoint* NewCheckpoint)
{
	Checkpoints.Add(NewCheckpoint);
}

void APlatformDadCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (const APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlatformDadCharacter::Move);

		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlatformDadCharacter::Look);
	}
}

void APlatformDadCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void APlatformDadCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerPitchInput(LookAxisVector.Y);
	}
}
