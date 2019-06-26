// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FPSAIGuard.generated.h"



UENUM(BlueprintType)
enum class EAIState : uint8
{
	Idle,
	Suspicious,
	Alert
};



UCLASS()
class FPSGAME_API AFPSAIGuard : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFPSAIGuard();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FRotator OriginalRotation;

	// Must be set as UFUNCTION to be properly used by the timer
	UFUNCTION()
	void ResetOrientation();

	FTimerHandle TimerHandle_ResetOrientation;

	// Marks this property as replicated
	UPROPERTY(ReplicatedUsing=OnRep_GuardState)
	EAIState GuardState;

	UFUNCTION()
	void OnRep_GuardState();

	void SetGuardState(EAIState NewState);

	UFUNCTION(BlueprintImplementableEvent, Category = "AI")
	void OnStateChanged(EAIState NewState);

	void RotateGuard(const FVector& Location);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class UPawnSensingComponent* PawnSensingComp;

	UFUNCTION()
	void MyOnSeePawn(APawn* Pawn);

	UFUNCTION()
	void MyOnHearNoise(APawn* NoiseInstigator, const FVector& Location, float Volume);

protected:

	// CHALLENGE CODE	

	/* Let the guard go on patrol */
	UPROPERTY(EditInstanceOnly, Category = "AI")
	bool bPatrol;

	/* First of two patrol points to patrol between */
	UPROPERTY(EditInstanceOnly, Category = "AI", meta = (EditCondition = "bPatrol"))
	AActor* FirstPatrolPoint;

	/* Second of two patrol points to patrol between */
	UPROPERTY(EditInstanceOnly, Category = "AI", meta = (EditCondition = "bPatrol"))
	AActor* SecondPatrolPoint;

	// The current point the actor is either moving to or standing at
	AActor* CurrentPatrolPoint;

	void MoveToNextPatrolPoint();

};
