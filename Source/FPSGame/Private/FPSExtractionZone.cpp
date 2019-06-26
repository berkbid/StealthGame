// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSExtractionZone.h"
#include "Components/BoxComponent.h"
#include "Components/DecalComponent.h"
#include "FPSCharacter.h"
#include "FPSGameMode.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AFPSExtractionZone::AFPSExtractionZone()
{
	OverlapComp = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapComp"));
	OverlapComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	OverlapComp->SetBoxExtent(FVector(200.0f));
	RootComponent = OverlapComp;

	// We would see the UBoxComponent lines in the game for overlapping purposes
	OverlapComp->SetHiddenInGame(false);

	// One way to bind a function and override it
	OverlapComp->OnComponentBeginOverlap.AddDynamic(this, &AFPSExtractionZone::HandleOverlap);

	DecalComp = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComp"));
	DecalComp->SetupAttachment(RootComponent);
	DecalComp->DecalSize = FVector(200.0f);
	//DecalComp->SetRelativeLocation(RootComponent->GetComponentLocation());
	

}



void AFPSExtractionZone::HandleOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, 
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlapped with extraction zone!"));

	if (AFPSCharacter* MyPawn = Cast<AFPSCharacter>(OtherActor))
	{
		if (MyPawn->bIsCarryingObjective)
		{
			// GetAuthGameMode() will return NULL on clients
			if (AFPSGameMode* GM = Cast<AFPSGameMode>(GetWorld()->GetAuthGameMode()))
			{
				GM->CompleteMission(MyPawn, true);
			}
		}
		else
		{
			UGameplayStatics::PlaySound2D(this, ObjectiveMissingSound);
		}
	}

}


