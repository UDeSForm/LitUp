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
	Diffractor->SetupAttachment(Origin);

	static ConstructorHelpers::FObjectFinder<UStaticMesh>CubeMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	Diffractor->SetStaticMesh(CubeMeshAsset.Object);
	
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
	previousTick = currentTick;
	if (previousTick)
	{
		//show decal
	}
	currentTick = false;
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


	if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_WorldStatic, CollisionParams))
	{
		float distanceMurM = OutHit.Distance / 100.f;
		float taillePixelM = (distanceMurM) / 1024.f;
		int sizeX = Fente->GetSizeX();
		int sizeY = Fente->GetSizeY();
		int nearestX = sizeX;
		int nearestY = sizeY;
		int furthestX = 0;
		int furthestY = 0;

		FTexture2DMipMap MyMipMap = Fente->GetPlatformData()->Mips[0];
		FByteBulkData RawImageData = MyMipMap.BulkData;

		TArray<FColor> pixelsPatron;
		FVector colorPatron = calculateColorFromWaveLength(WaveLength);
		pixelsPatron.SetNumZeroed(1024 * 1024);

		FColor* FormatedImageData = static_cast<FColor*>(RawImageData.Lock(LOCK_READ_ONLY));
		for (int i = 0; i < sizeX; i++)
		{
			for (int j = 0; j < sizeY; j++)
			{
				FColor PixelColor = FormatedImageData[j * sizeX + i];
				if (PixelColor.R > 0)
				{
					
					if (i < nearestX) nearestX = i;
					if (j < nearestY) nearestY = j;

					if (i > furthestX) furthestX = i;
					if (j > furthestY) furthestY = j;	
				}
			}
		}
		int largeurFente = furthestX - nearestX + 1;
		int hauteurFente = furthestY - nearestY + 1;
		int coorX = ((largeurFente / 2.0 + nearestX) * 64);
		int coorY = ((hauteurFente / 2.0 + nearestY) * 64);
		GEngine->AddOnScreenDebugMessage(-17, 10.f, FColor::Yellow, FString::Printf(TEXT("Milieu X: %i"), coorX));
		GEngine->AddOnScreenDebugMessage(-18, 10.f, FColor::Yellow, FString::Printf(TEXT("Milieu Y: %i"), coorY));
		int testX = 560;
		int testY = 560;
		GEngine->AddOnScreenDebugMessage(-19, 10.f, FColor::Yellow, FString::Printf(TEXT("test X : %i"), testX));
		GEngine->AddOnScreenDebugMessage(-20, 10.f, FColor::Yellow, FString::Printf(TEXT("test Y: %i"), testY));
		for (int x = 0; x < 1024; x++)
		{
			for (int y = coorY - (32 * (hauteurFente)); y < coorY + (32 * (hauteurFente)); y++)
			{
				float dPointPatron = sqrt((x - coorX) * taillePixelM * (x - coorX) * taillePixelM + (y - coorY) * taillePixelM * (y - coorY) * taillePixelM);
				
				float hypotenuse = sqrt(distanceMurM * distanceMurM + dPointPatron * dPointPatron);

				float m = (((largeurFente * pixelFente)/1000000000.f) * dPointPatron) / (hypotenuse * (WaveLength / 1000000000.f));
				if (x == testX && y == testY)
				{
					GEngine->AddOnScreenDebugMessage(-21, 10.f, FColor::Yellow, FString::Printf(TEXT("Distance point-Milieu: %f"), dPointPatron));
					GEngine->AddOnScreenDebugMessage(-22, 10.f, FColor::Yellow, FString::Printf(TEXT("Hyp: %f"), hypotenuse));
					GEngine->AddOnScreenDebugMessage(-23, 10.f, FColor::Yellow, FString::Printf(TEXT("m: %f"), m));

				}
					
				if (m >= 0 && m < 1)
				{
					pixels[x][y] += ((cos(PI * m) / 2 + 0.5)/2);
				}
				else
				{
					pixels[x][y] += (((cos(2 * PI * m + PI) / 2 + 0.5) * (1 / abs(m)))/2);
				}
			}
		}

		for (int x = coorX - (32 * (furthestX - nearestX)); x < coorX + (32 * (furthestX - nearestX)); x++)
		{
			for (int y = 0; y < 1024; y++)
			{
				float dPointPatron = sqrt((x - coorX) * taillePixelM * (x - coorX) * taillePixelM + (y - coorY) * taillePixelM * (y - coorY) * taillePixelM);
				float hypotenuse = sqrt(distanceMurM * distanceMurM + dPointPatron * dPointPatron);

				float m = (((hauteurFente * pixelFente)/1000000000.f) * dPointPatron) / (hypotenuse * WaveLength / 1000000000.f);
				if (m >= 0 && m < 1)
				{
					pixels[x][y] += ((cos(PI * m) / 2 + 0.5)/2);
				}
				else
				{
					pixels[x][y] += (((cos(2 * PI * m + PI) / 2 + 0.5) * (1 / abs(m)))/2);
				}
			}
		}
		GEngine->AddOnScreenDebugMessage(-24, 10.f, FColor::Yellow, FString::Printf(TEXT("%f"), pixels[testX][testY]));
		RawImageData.Unlock();

		

		FCreateTexture2DParameters params;	
		patronDiffraction = FImageUtils::CreateTexture2D(1024, 1024, pixelsPatron, Diffractor, "Patron", EObjectFlags::RF_Transient, params);
	}
}

void ALitUpDiffractor::exec(float rayWaveLength)
{

	if (rayWaveLength == WaveLength)
	{
		currentTick = true;
	}
}