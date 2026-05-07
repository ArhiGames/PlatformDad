// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "PlatformDadCharacter.generated.h"

class UEffectUserWidget;
class ACheckpoint;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UNiagaraSystem;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

USTRUCT()
struct FPowerUpData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	UTexture2D* Thumbnail;
	UPROPERTY(EditAnywhere)
	FText DescriptionText;
	UPROPERTY(EditAnywhere)
	int32 Seconds = 10;
	UPROPERTY(EditAnywhere)
	FString LocalId;
};

UCLASS(config=Game)
class APlatformDadCharacter : public ACharacter
{
	GENERATED_BODY()
	
public:
	APlatformDadCharacter();

public:
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	void SetLastCheckpoint(ACheckpoint* NewCheckpoint);
	bool IsCheckpointActivated(ACheckpoint* Checkpoint) const;

public:
	UFUNCTION(NetMulticast, Reliable)
	void NetMulticastSetWalkSpeed(const float NewSpeed);
	UFUNCTION(NetMulticast, Reliable)
	void NetMulticastSetJumpZVelocity(const float NewVelocity);
	UFUNCTION(NetMulticast, Reliable)
	void NetMulticastSetAllowJump(const bool AllowJump);
	UFUNCTION(NetMulticast, Reliable)
	void NetMulticastSetFriction(const float GroundFriction, const int32 BrakingDecelerationWalking, const int32 BrakingDecelerationFalling);
	UFUNCTION(NetMulticast, Reliable)
	void NetMulticastSpawnParticleSystem(UNiagaraSystem* NiagaraSystem, const FVector& Location);
	UFUNCTION(Client, Reliable)
	void ClientSetPlayerUserInterface(const FPowerUpData& PowerUpData);
	UFUNCTION(Server, Reliable)
	void ServerOnRandomPickupTakenUp(APlatformDadCharacter* TriggeringCharacter);
	UFUNCTION(Server, Reliable)
	void ServerResetToCheckpoint(const FVector& OldLocation);

public:
	UPROPERTY(Replicated)
	float DefaultMaxWalkSpeed;
	
protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Tick(float DeltaSeconds) override;

protected:
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

protected:
	UPROPERTY(Replicated)
	TArray<ACheckpoint*> Checkpoints;
	UPROPERTY(Replicated)
	FVector StartPosition;
	UPROPERTY(Replicated)
	float StartJumpZVelocity;
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

private:
	UPROPERTY(EditAnywhere)
	TArray<FPowerUpData> AllPowerUpsData;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UEffectUserWidget> EffectUserWidgetClass;
	UPROPERTY(EditAnywhere)
	UNiagaraSystem* NiagaraKillEffect;

	UPROPERTY(EditAnywhere)
	UEffectUserWidget* CreatedEffectUserWidget;
	
	TMap<APlatformDadCharacter*, FTimerHandle> TimerHandleResetSpeed;
	TMap<APlatformDadCharacter*, FTimerHandle> TimerHandleJumpBoost;
	TMap<APlatformDadCharacter*, FTimerHandle> TimerHandleJumpBlock;
	TMap<APlatformDadCharacter*, FTimerHandle> TimerHandleIcyGround;
};

