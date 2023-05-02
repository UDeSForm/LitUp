// Fill out your copyright notice in the Description page of Project Settings.


#include "LitUpDiffractor.h"

// Sets default values
ALitUpDiffractor::ALitUpDiffractor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Origin = CreateDefaultSubobject<USceneComponent>(TEXT("Origin"));
	SetRootComponent(Origin);

	Diffractor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Diffractor"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh>CubeMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	Diffractor->SetStaticMesh(CubeMeshAsset.Object);
	SetRootComponent(Diffractor);
}

// Called when the game starts or when spawned
void ALitUpDiffractor::BeginPlay()
{
	Super::BeginPlay();
	Diffractor->SetStaticMesh(DiffractorMeshAsset);
	CalculerPatronDiffraction();
}

// Called every frame
void ALitUpDiffractor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// This ultimately is what controls whether or not it can even tick at all in the editor view port. 
//But, it is EVERY view port so it still needs to be blocked from preview windows and junk.
bool ALitUpDiffractor::ShouldTickIfViewportsOnly() const
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

inline void ALitUpDiffractor::CalculerPatronDiffraction()
{
	FVector Start = Origin->GetComponentLocation();
	FVector ForwardVector = Origin->GetForwardVector();
	FHitResult OutHit;
	FVector End = ((ForwardVector * 10000.f) + Start);

	FCollisionQueryParams CollisionParams;
	CollisionParams.bTraceComplex = true;

	//DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 0.04, 0, 10);

	if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_WorldStatic, CollisionParams))
	{
		float distanceMur = OutHit.Distance;
		float taillePixelNM = (distanceMur * 10000000) / 1024.f;
		int sizeX = Fente->GetSizeX();
		int sizeY = Fente->GetSizeY();

		FTexture2DMipMap MyMipMap = Fente->GetPlatformData()->Mips[0];
		FByteBulkData RawImageData = MyMipMap.BulkData;
		FColor* FormatedImageData = static_cast<FColor*>(RawImageData.Lock(LOCK_READ_ONLY));

		for (int i = 0; i < sizeX; i++)
		{
			for (int j = 0; j < sizeY; j++)
			{

				FColor PixelColor = FormatedImageData[j * sizeX + i];
				if (PixelColor.R > 0)
				{
					for (int x = 0; x < 1024; x++)
					{
						for (int y = 0; y < 1024; y++)
						{
							
						}
					}
				}

			}
		}
		RawImageData.Unlock();
	}

	
	
}

void ALitUpDiffractor::exec()
{
	int sizeX = Fente->GetSizeX();
	
}