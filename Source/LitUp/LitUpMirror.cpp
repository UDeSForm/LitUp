// Fill out your copyright notice in the Description page of Project Settings.


#include "LitUpMirror.h"

// Sets default values
ALitUpMirror::ALitUpMirror()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mirror = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mirror"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh>CubeMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	Mirror->SetStaticMesh(CubeMeshAsset.Object);
	SetRootComponent(Mirror);
}

// Called when the game starts or when spawned
void ALitUpMirror::BeginPlay()
{
	Super::BeginPlay();
	Mirror->SetStaticMesh(MirrorMeshAsset);
}

// Called every frame
void ALitUpMirror::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// This ultimately is what controls whether or not it can even tick at all in the editor view port. 
//But, it is EVERY view port so it still needs to be blocked from preview windows and junk.
bool ALitUpMirror::ShouldTickIfViewportsOnly() const
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