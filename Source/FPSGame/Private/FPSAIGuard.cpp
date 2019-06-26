// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSAIGuard.h"
#include "Perception/PawnSensingComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Actor.h"
#include "Public/TimerManager.h"
#include "FPSGameMode.h"
#include "Engine/World.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AFPSAIGuard::AFPSAIGuard()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	

	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
	
	GuardState = EAIState::Idle;

}

// Called when the game starts or when spawned
void AFPSAIGuard::BeginPlay()
{
	Super::BeginPlay();
	if (!HasAuthority()) { return; }

	// Bind the delegate here in the beginplay, not in the constructor!
	PawnSensingComp->OnSeePawn.AddDynamic(this, &AFPSAIGuard::MyOnSeePawn);
	PawnSensingComp->OnHearNoise.AddDynamic(this, &AFPSAIGuard::MyOnHearNoise);

	OriginalRotation = GetActorRotation();

	
	if (bPatrol)
	{
		UE_LOG(LogTemp, Warning, TEXT("bPatrol = True"));
		MoveToNextPatrolPoint();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("bPatrol = False"));
	}
}

// Only called on server side
void AFPSAIGuard::ResetOrientation()
{
	// Will fail one time if already alerted
	if (GuardState == EAIState::Alert)
	{
		return;
	}

	SetActorRotation(OriginalRotation);

	SetGuardState(EAIState::Idle);

	// Stopped investigating...if we are a patrolling pawn, pick a new patrol point to move to
	if (bPatrol)
	{
		MoveToNextPatrolPoint();
	}
}

// Only called on server side
void AFPSAIGuard::MyOnSeePawn(APawn* Pawn)
{
	if (!Pawn) { return; }

	RotateGuard(Pawn->GetActorLocation());

	UE_LOG(LogTemp, Warning, TEXT("OnSeePawn Triggered!!"));

	UWorld* CurWorld = GetWorld();

	DrawDebugSphere(CurWorld, Pawn->GetActorLocation(), 32.f, 12, FColor::Red, false, 10.f);

	if (AFPSGameMode* GM = Cast<AFPSGameMode>(CurWorld->GetAuthGameMode()))
	{
		GM->CompleteMission(Pawn, false);
		//PawnSensingComp->SetSensingInterval(5.f);
		//PawnSensingComp->SetSensingUpdatesEnabled(false);
		// Destroy the PawnSensingComponent upon final sense.
		//PawnSensingComp->DestroyComponent();

		// This solves guard still sensing after CompleteMission
		// For some reason, SetSensingUpdatesEnabled(false) does not work at runtime...
		PawnSensingComp->bSeePawns = false;

	}

	SetGuardState(EAIState::Alert);

	// Stop Movement if Patrolling
	AController* TempController = GetController();
	if (TempController)
	{
		TempController->StopMovement();
	}
}

// Only called on server side
void AFPSAIGuard::MyOnHearNoise(APawn* NoiseInstigator, const FVector& Location, float Volume)
{
	if (GuardState == EAIState::Alert)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("MyOnHearNoise Triggered!!"));

	DrawDebugSphere(GetWorld(), Location, 32.f, 12, FColor::Green, false, 10.f);
	//FVector ActorLocation = GetActorLocation();

	RotateGuard(Location);

	// Stops original timer and refreshes it if new sound is heard before old timer is finished
	GetWorldTimerManager().ClearTimer(TimerHandle_ResetOrientation);
	GetWorldTimerManager().SetTimer(TimerHandle_ResetOrientation, this, &AFPSAIGuard::ResetOrientation, 3.0f);

	SetGuardState(EAIState::Suspicious);

	// Stop Movement if Patrolling
	AController* TempController = GetController();
	if (TempController)
	{
		TempController->StopMovement();
	}
}

// Only called on server side
void AFPSAIGuard::MoveToNextPatrolPoint()
{
	// Assign next patrol point.
	if (CurrentPatrolPoint == nullptr || CurrentPatrolPoint == SecondPatrolPoint)
	{
		CurrentPatrolPoint = FirstPatrolPoint;
	}
	else
	{
		CurrentPatrolPoint = SecondPatrolPoint;
	}
	UAIBlueprintHelperLibrary::SimpleMoveToActor(GetController(), CurrentPatrolPoint);
}

// If variable GuardState changes, this function will get called IF you are a CLIENT ONLY
void AFPSAIGuard::OnRep_GuardState()
{
	OnStateChanged(GuardState);
}

// Only called on server side
void AFPSAIGuard::SetGuardState(EAIState NewState)
{
	if (GuardState == NewState)
	{
		return;
	}

	GuardState = NewState;

	// This calls OnStateChanged for SERVER, meanwhile it is already called for client using OnRep_GuardState() automatically
	// The fact that we use the OnRep function here guarantees the same behavior for server and client upon the GuardState property changing above
	OnRep_GuardState();
}

// Only called on server side
void AFPSAIGuard::RotateGuard(const FVector & Location)
{
	// Get Direction to look at which is the difference of new location and current location
	FVector Direction = Location - GetActorLocation();
	Direction.Normalize();

	// Set new rotation for guard, only changing yaw value
	FRotator NewLookAt;
	NewLookAt.Yaw = FRotationMatrix::MakeFromX(Direction).Rotator().Yaw;
	NewLookAt.Pitch = GetActorRotation().Pitch;
	NewLookAt.Roll = GetActorRotation().Roll;

	// Make guard rotate to face location of sound
	SetActorRotation(NewLookAt);
}

// Called every frame
void AFPSAIGuard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!HasAuthority()) { return; }
	// Patrol Goal Checks
	if (CurrentPatrolPoint)
	{
		FVector Delta = GetActorLocation() - CurrentPatrolPoint->GetActorLocation();
		float DistanceToGoal = Delta.Size();
		// Check if we are within 50 units of our goal, if so - pick a new patrol point
		if (DistanceToGoal < 50)
		{
			MoveToNextPatrolPoint();
		}
	}

}

// This function needed to add replicated properties such as "GuardState" to tell Unreal the replication rules
void AFPSAIGuard::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSAIGuard, GuardState);
}

