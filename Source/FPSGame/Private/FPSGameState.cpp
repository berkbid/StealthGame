// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSGameState.h"
#include "Engine/World.h"
#include "MyPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

void AFPSGameState::MulticastOnMissionComplete_Implementation(APawn* InstigatorPawn, bool bMissionSuccess)
{
	UGameplayStatics::PlaySound2D(this, WinSound);

	if (SpectatingViewpointClass)
	{
		TArray<AActor*> ReturnedActors;
		UGameplayStatics::GetAllActorsOfClass(this, SpectatingViewpointClass, ReturnedActors);

		// Change viewtarget if any valid actor found
		if (ReturnedActors.Num() > 0)
		{
			AActor* NewViewTarget = ReturnedActors[0];

			for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
			{
				UE_LOG(LogTemp, Warning, TEXT("Found pawn: %s"), *InstigatorPawn->GetName());
				AMyPlayerController* PC = Cast<AMyPlayerController>(It->Get());
				// Only if owner of this player owns this controller should call this function
				if (PC && PC->IsLocalController())
				{
					PC->SetViewTargetWithBlend(NewViewTarget, 0.5f, EViewTargetBlendFunction::VTBlend_Cubic);

					PC->OnMissionCompleted(InstigatorPawn, bMissionSuccess);

					// Disable input on pawn of local controller
					APawn* MyPawn = PC->GetPawn();
					if (MyPawn)
					{
						MyPawn->DisableInput(PC);
					}
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No SpectatingView Found"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SpectatingViewpoint not set in BP_GameState"));
	}

}
