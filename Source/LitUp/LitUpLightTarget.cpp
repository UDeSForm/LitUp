// Fill out your copyright notice in the Description page of Project Settings.


#include "LitUpLightTarget.h"
#include <Misc/OutputDeviceNull.h>

// Sets default values
ALitUpLightTarget::ALitUpLightTarget()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Créer et appliquer un mesh sur la cible
	Target = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LightTarget"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh>CubeMeshAsset(TEXT("StaticMesh'/Game/CustomActors/Intermediates/Target_Mesh.Target_Mesh'"));
	Target->SetStaticMesh(CubeMeshAsset.Object);
	SetRootComponent(Target);
}

// Called when the game starts or when spawned
void ALitUpLightTarget::BeginPlay()
{
	Super::BeginPlay();
	Target->SetStaticMesh(TargetMeshAsset); //Appliquer le mesh choisi sur l'actor
}

// Called every frame
void ALitUpLightTarget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (isHit && lastTime == 0) lastTime = FPlatformTime::Seconds(); //Si le temps n'a pas été défini et que la cible est touchée le temps est celui actuel
	
	if (!isHit) lastTime = 0; //Si la cible n'est pas touchée, le temps est nul

	if (isHit && FPlatformTime::Seconds() - lastTime > 5) //Si la cible est touchée et que 5 secondes ont passé depuis qu'elle a été touchée, on passe au prochain niveau
	{
		FOutputDeviceNull outputNull;
		const FString GoToNextLevelCommand = FString::Printf(TEXT("GoToNextLevel"));
		this->CallFunctionByNameWithArguments(*GoToNextLevelCommand, outputNull, NULL, true);
	}

	isHit = false; //Fait en dernier afin de détecter lorsque la cible arrête de se faire toucher, au prochain tick, si la cible est toujours touchée, la valeur va être vraie avant de faire les vérifications
}

// This ultimately is what controls whether or not it can even tick at all in the editor view port. 
//But, it is EVERY view port so it still needs to be blocked from preview windows and junk.
bool ALitUpLightTarget::ShouldTickIfViewportsOnly() const
{
	if (GetWorld() != nullptr && GetWorld()->WorldType == EWorldType::Editor)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void ALitUpLightTarget::exec(float &waveLength)
{
	if (!UseWaveLength || (UseWaveLength && waveLength == WaveLength)) //Si on utilise la longeur d'onde et qu'elle est bonne ou qu'on utilise pas la longeur d'onde la cible est touchée alors la cible est touchée
	{
		//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("Hit!"));
		isHit = true;
	}
}