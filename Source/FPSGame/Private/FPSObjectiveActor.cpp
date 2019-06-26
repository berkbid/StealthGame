// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSObjectiveActor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "FPSCharacter.h"



// Sets default values
AFPSObjectiveActor::AFPSObjectiveActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	VolumeMultiplier = 2.f;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// RootComponent or SetRootComponent(MeshComp)?
	RootComponent = MeshComp;
	//SetRootComponent(MeshComp);

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SphereComp->SetupAttachment(MeshComp);

	SetReplicates(true);

}

// Called when the game starts or when spawned
void AFPSObjectiveActor::BeginPlay()
{
	Super::BeginPlay();

	//PlayEffects();
	
}

void AFPSObjectiveActor::PlayEffects()
{
	UGameplayStatics::SpawnEmitterAtLocation(this, PickupFX, GetActorLocation());
}


void AFPSObjectiveActor::NotifyActorBeginOverlap(AActor * OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	// Both clients and server see and hear these effects and sound
	PlayEffects();
	UGameplayStatics::PlaySound2D(this, ObjectivePickedUpSound, VolumeMultiplier);

	// Only run as server
	if (Role == ROLE_Authority)
	{
		if (AFPSCharacter* MyCharacter = Cast<AFPSCharacter>(OtherActor))
		{
			// Only set to true on server here, but we setup replication for clients in FPSCharacter.cpp
			MyCharacter->bIsCarryingObjective = true;

			Destroy();
		}
	}


}

