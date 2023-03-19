// Fill out your copyright notice in the Description page of Project Settings.


#include "LitUpPrism.h"

// Sets default values
ALitUpPrism::ALitUpPrism()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Prism = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Prism"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh>CubeMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	Prism->SetStaticMesh(CubeMeshAsset.Object);
	SetRootComponent(Prism);
}

// Called when the game starts or when spawned
void ALitUpPrism::BeginPlay()
{
	Super::BeginPlay();
	Prism->SetStaticMesh(PrismMeshAsset);
}

// Called every frame
void ALitUpPrism::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// This ultimately is what controls whether or not it can even tick at all in the editor view port. 
//But, it is EVERY view port so it still needs to be blocked from preview windows and junk.
bool ALitUpPrism::ShouldTickIfViewportsOnly() const
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