// Fill out your copyright notice in the Description page of Project Settings.


#include "LitUpLightTarget.h"
#include <Misc/OutputDeviceNull.h>

// Sets default values
ALitUpLightTarget::ALitUpLightTarget()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Target = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LightTarget"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh>CubeMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	Target->SetStaticMesh(CubeMeshAsset.Object);
	SetRootComponent(Target);
}

// Called when the game starts or when spawned
void ALitUpLightTarget::BeginPlay()
{
	Super::BeginPlay();
	Target->SetStaticMesh(TargetMeshAsset);
}

// Called every frame
void ALitUpLightTarget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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

void ALitUpLightTarget::exec()
{
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("Hit!"));

	FOutputDeviceNull outputNull;
	const FString RotateObjectCommand = FString::Printf(TEXT("GoToNextLevel"));
	this->CallFunctionByNameWithArguments(*RotateObjectCommand, outputNull, NULL, true);
	return;
}