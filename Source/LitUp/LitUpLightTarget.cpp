// Fill out your copyright notice in the Description page of Project Settings.


#include "LitUpLightTarget.h"
#include <Misc/OutputDeviceNull.h>

// Sets default values
ALitUpLightTarget::ALitUpLightTarget()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Target = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LightTarget"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh>CubeMeshAsset(TEXT("StaticMesh'/Game/CustomActors/Intermediates/Target_Mesh.Target_Mesh'"));
	Target->SetStaticMesh(CubeMeshAsset.Object);
	SetRootComponent(Target);

	static ConstructorHelpers::FObjectFinder<UMaterial>targetMaterial(TEXT("Material'/Game/CustomActors/Materials/M_TargetColor.M_TargetColor'"));
	dynamicTargetMaterialInstanceDynamic = UMaterialInstanceDynamic::Create(targetMaterial.Object, Target);
	Target->SetMaterial(1, dynamicTargetMaterialInstanceDynamic);
}

// Called when the game starts or when spawned
void ALitUpLightTarget::BeginPlay()
{
	Super::BeginPlay();
	Target->SetStaticMesh(TargetMeshAsset);

	FVector color = calculateColorFromWaveLength();

	GEngine->AddOnScreenDebugMessage(-14, 1.f, FColor::Yellow, FString::Printf(TEXT("x: %f y: %f x: %f"), color.X, color.Y, color.Z));

	//dynamicTargetMaterialInstanceDynamic->SetVectorParameterValue(FName("TargetColor"), color);
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

void ALitUpLightTarget::exec(float &waveLength)
{
	if (!UseWaveLength || (UseWaveLength && waveLength == WaveLength))
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("Hit!"));

		FOutputDeviceNull outputNull;
		const FString GoToNextLevelCommand = FString::Printf(TEXT("GoToNextLevel"));
		this->CallFunctionByNameWithArguments(*GoToNextLevelCommand, outputNull, NULL, true);
	}

	return;
}

inline FVector ALitUpLightTarget::calculateColorFromWaveLength()
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

	FVector result = { pow(red * factor, gamma), pow(green * factor, gamma), pow(blue * factor, gamma) };

	return result;
}