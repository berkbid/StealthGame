// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSAIGuard.h"
#include "Perception/PawnSensingComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Actor.h"

// Sets default values
AFPSAIGuard::AFPSAIGuard()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));


}

// Called when the game starts or when spawned
void AFPSAIGuard::BeginPlay()
{
	Super::BeginPlay();

	// Bind the delegate here in the beginplay, not in the constructor!
	PawnSensingComp->OnSeePawn.AddDynamic(this, &AFPSAIGuard::MyOnSeePawn);

}

// If we want to call the event from blueprint instead of bind delegate
void AFPSAIGuard::OnPawnSeen(APawn* SeenPawn)
{
	UE_LOG(LogTemp, Warning, TEXT("OnSeePawn Triggered!!"));

	if (SeenPawn) 
	{
		UE_LOG(LogTemp, Warning, TEXT("I SEE YOU"));
		DrawDebugSphere(GetWorld(), SeenPawn->GetActorLocation(), 32.f, 12, FColor::Yellow, false, 10.f);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("????"));
	}
}

void AFPSAIGuard::MyOnSeePawn(APawn* Pawn)
{
	UE_LOG(LogTemp, Warning, TEXT("OnSeePawn Triggered!!"));
	if (Pawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("I SEE YOU"));
		DrawDebugSphere(GetWorld(), Pawn->GetActorLocation(), 32.f, 12, FColor::Yellow, false, 10.f);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("????"));
	}
}

// Called every frame
void AFPSAIGuard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


