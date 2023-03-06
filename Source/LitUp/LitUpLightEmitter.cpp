// Fill out your copyright notice in the Description page of Project Settings.


#include "LitUpLightEmitter.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"

// Sets default values
ALitUpLightEmitter::ALitUpLightEmitter()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Emitter = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LightEmitter"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh>CubeMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	Emitter->SetStaticMesh(CubeMeshAsset.Object);
	SetRootComponent(Emitter);

	LightRay = CreateDefaultSubobject<UChildActorComponent>(TEXT("LightRay"));
	LightRay->SetChildActorClass(ALitUpLightRay::StaticClass());
	LightRay->SetupAttachment(Emitter);
}

// Called when the game starts or when spawned
void ALitUpLightEmitter::BeginPlay()
{
	Super::BeginPlay();
	Emitter->SetStaticMesh(EmitterMeshAsset);
}

// Called every frame
void ALitUpLightEmitter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// This ultimately is what controls whether or not it can even tick at all in the editor view port. 
//But, it is EVERY view port so it still needs to be blocked from preview windows and junk.
bool ALitUpLightEmitter::ShouldTickIfViewportsOnly() const
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