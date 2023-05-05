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

	static ConstructorHelpers::FObjectFinder<UMaterial>dMaterial(TEXT("Material'/Game/Materials/M_Diffraction.M_Diffraction'"));
	diffractionMaterial = dMaterial.Object;
}

// Called when the game starts or when spawned
void ALitUpDiffractor::BeginPlay()
{
	Super::BeginPlay();
	Diffractor->SetStaticMesh(DiffractorMeshAsset);

	decal = GetWorld()->SpawnActor<ADecalActor>(Origin->GetComponentLocation(), FRotator(0.f,0.f,0.f));
	if (decal)
	{
		decal->GetDecal()->DecalSize = FVector(50.0f, 50.0f, 50.0f);
		decal->SetActorHiddenInGame(true);

		decal->SetDecalMaterial(diffractionMaterial);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No decal spawned"));
	}

	CalculerPatronDiffraction();

	if (decal)
	{
		UMaterialInstanceDynamic* dynamicDiffractionMaterialInstanceDynamic = decal->CreateDynamicMaterialInstance();
		dynamicDiffractionMaterialInstanceDynamic->SetTextureParameterValue("Patron", patronDiffraction);
		decal->SetDecalMaterial(dynamicDiffractionMaterialInstanceDynamic);
	}
}

// Called every frame
void ALitUpDiffractor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (decal) decal->SetActorHiddenInGame(!tick);
	tick = false;
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
		//DrawDebugLine(GetWorld(), Start, OutHit.Location, FColor::Green, true, 0.04, 0, 10);

		Start += ForwardVector * 100.f;

		decal->SetActorTransform(FTransform(FRotator(0, 0, 0) + ForwardVector.Rotation(), (OutHit.Location - Start) / 2.f + Start + ForwardVector * 0.01, FVector(OutHit.Distance / 100.f, 1, 1)));

		float distanceMurM = OutHit.Distance / 100.f;
		float taillePixelM = (distanceMurM) / (float)size;
		int sizeX = Fente->GetSizeX();
		int sizeY = Fente->GetSizeY();
		int nearestX = sizeX;
		int nearestY = sizeY;
		int furthestX = 0;
		int furthestY = 0;

		FTexture2DMipMap MyMipMap = Fente->GetPlatformData()->Mips[0];
		FByteBulkData RawImageData = MyMipMap.BulkData;

		FVector colorPatron = calculateColorFromWaveLength();
		
		pixelsPatron.SetNumZeroed(size * size);

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
		int coorX = ((largeurFente / 2.0 + nearestX) * (size/sizeX));
		int coorY = ((hauteurFente / 2.0 + nearestY) * (size/sizeY));
		GEngine->AddOnScreenDebugMessage(-17, 10.f, FColor::Yellow, FString::Printf(TEXT("Milieu X: %i"), coorX));
		GEngine->AddOnScreenDebugMessage(-18, 10.f, FColor::Yellow, FString::Printf(TEXT("Milieu Y: %i"), coorY));
		int testX = 560;
		int testY = 560;
		GEngine->AddOnScreenDebugMessage(-19, 10.f, FColor::Yellow, FString::Printf(TEXT("test X : %i"), testX));
		GEngine->AddOnScreenDebugMessage(-20, 10.f, FColor::Yellow, FString::Printf(TEXT("test Y: %i"), testY));
		for (int x = 0; x < size; x++)
		{
			for (int y = coorY - ((size/(2*sizeY)) * (hauteurFente)); y < coorY + ((size / (2 * sizeY)) * (hauteurFente)); y++)
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
					pixelsPatron[x + y * size] = FColor(colorPatron.X, colorPatron.Y, colorPatron.Z, pixelsPatron[x + y * size].A + ((cos(PI * m) / 2 + 0.5) / 2));
				}
				else
				{
					pixelsPatron[x + y * size] = FColor(colorPatron.X, colorPatron.Y, colorPatron.Z, pixelsPatron[x + y * size].A + ((cos(2 * PI * m + PI) / 2 + 0.5) * (1 / abs(m))) / 2);
				}
			}
		}

		for (int x = coorX - ((size / (2 * sizeX)) * (furthestX - nearestX)); x < coorX + ((size / (2 * sizeX)) * (furthestX - nearestX)); x++)
		{
			for (int y = 0; y < size; y++)
			{
				float dPointPatron = sqrt((x - coorX) * taillePixelM * (x - coorX) * taillePixelM + (y - coorY) * taillePixelM * (y - coorY) * taillePixelM);
				float hypotenuse = sqrt(distanceMurM * distanceMurM + dPointPatron * dPointPatron);

				float m = (((hauteurFente * pixelFente) / 1000000000.f) * dPointPatron) / (hypotenuse * WaveLength / 1000000000.f);
				if (m >= 0 && m < 1)
				{
					pixelsPatron[x + y * size] = FColor(colorPatron.X, colorPatron.Y, colorPatron.Z, pixelsPatron[x + y * size].A + ((cos(PI * m) / 2 + 0.5) / 2));
				}
				else
				{
					pixelsPatron[x + y * size] = FColor(colorPatron.X, colorPatron.Y, colorPatron.Z, pixelsPatron[x + y * size].A + ((cos(2 * PI * m + PI) / 2 + 0.5) * (1 / abs(m))) / 2);
				}
			}
		}
		//GEngine->AddOnScreenDebugMessage(-24, 10.f, FColor::Yellow, FString::Printf(TEXT("%f"), pixelsAlpha[testX + testY * size]));
		RawImageData.Unlock();

		

		FCreateTexture2DParameters params;	
		patronDiffraction = FImageUtils::CreateTexture2D(size, size, pixelsPatron, Diffractor, "Patron", EObjectFlags::RF_Transient, params);
	}
}

void ALitUpDiffractor::exec(float rayWaveLength)
{
	if (rayWaveLength == WaveLength)
	{
		tick = true;
	}
}

inline FVector ALitUpDiffractor::calculateColorFromWaveLength()
{
	// Credits: Dan Bruton http://www.physics.sfasu.edu/astro/color.html
	double red = 0.0;
	double green = 0.0;
	double blue = 0.0;

	if ((380.0 <= WaveLength) && (WaveLength <= 439.0))
	{
		red = -(WaveLength - 440.0) / (440.0 - 380.0);
		green = 0.0;
		blue = 1.0;
	}
	else if ((440.0 <= WaveLength) && (WaveLength <= 489.0))
	{
		red = 0.0;
		green = (WaveLength - 440.0) / (490.0 - 440.0);
		blue = 1.0;
	}
	else if ((490.0 <= WaveLength) && (WaveLength <= 509.0))
	{
		red = 0.0;
		green = 1.0;
		blue = -(WaveLength - 510.0) / (510.0 - 490.0);
	}
	else if ((510.0 <= WaveLength) && (WaveLength <= 579.0))
	{
		red = (WaveLength - 510.0) / (580.0 - 510.0);
		green = 1.0;
		blue = 0.0;
	}
	else if ((580.0 <= WaveLength) && (WaveLength <= 644.0))
	{
		red = 1.0;
		green = -(WaveLength - 645.0) / (645.0 - 580.0);
		blue = 0.0;
	}
	else if ((645.0 <= WaveLength) && (WaveLength <= 780.0))
	{
		red = 1.0;
		green = 0.0;
		blue = 0.0;
	}

	double factor = 0.0;

	if ((380.0 <= WaveLength) && (WaveLength <= 419.0))
		factor = 0.3 + 0.7 * (WaveLength - 380.0) / (420.0 - 380.0);
	else if ((420.0 <= WaveLength) && (WaveLength <= 700.0))
		factor = 1.0;
	else if ((701.0 <= WaveLength) && (WaveLength <= 780.0))
		factor = 0.3 + 0.7 * (780.0 - WaveLength) / (780.0 - 700.0);
	else
		factor = 0.0;

	const double gamma = 0.8;
	const double intensity_max = 255;

	FVector result = { intensity_max * pow(red * factor, gamma), intensity_max * pow(green * factor, gamma), intensity_max * pow(blue * factor, gamma) };

	return result;
}