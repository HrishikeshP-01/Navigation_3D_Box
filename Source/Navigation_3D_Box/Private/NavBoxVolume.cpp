// Copyright 2021, Hrishikesh P, All rights reserved


#include "NavBoxVolume.h"
#include "ProceduralMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "NavNode.h"
#include <set>
#include <unordered_map>

static UMaterial* GridMaterial = nullptr;

// Sets the default values
ANavBoxVolume::ANavBoxVolume()
{
	// Set this actor call to Tick() every frame. TURN THIS OFF if you don't need it to imporve performance
	PrimaryActorTick.bCanEverTick = true;

	// Create the default scene component
	DefaultSceneComponent = CreateDefaultSubobject<USceneComponent>("DefaultSceneComponent");
	SetRootComponent(DefaultSceneComponent);

	// Create the procedural mesh component & optimize it
	ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>("ProceduralMesh");
	ProceduralMesh->SetupAttachment(GetRootComponent());
	ProceduralMesh->CastShadow = false;
	ProceduralMesh->SetEnableGravity(false);
	ProceduralMesh->bApplyImpulseOnDamage = false;
	ProceduralMesh->SetGenerateOverlapEvents(false);
	ProceduralMesh->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	ProceduralMesh->SetCollisionProfileName("NoCollision");
	ProceduralMesh->bHiddenInGame = false;

	// By default, hide the volume while game is running
	SetActorHiddenInGame(true);

	// Find and save the grid material
	static ConstructorHelpers::FObjectFinder<UMaterial> materialFinder(TEXT("Material'/Navigation_3D_Box/M_Grid.M_Grid'"));
	GridMaterial = materialFinder.Object;
}

void ANavBoxVolume::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	CreateGrid();
}

void ANavBoxVolume::CreateGrid()
{
	// Create arrays to store the vertices and the triangles
	TArray<FVector> vertices;
	TArray<int32> triangles;

	// Define variables for the start and end of line
	FVector start = FVector::ZeroVector;
	FVector end = FVector::ZeroVector;

	// Create the X lines geometry
	for (int32 z = 0;z <= DivisionsZ;++z)
	{
		start.Z = end.Z = z * DivisionSize;
		for (int32 x = 0;x <= DivisionsX;++x)
		{
			start.X = end.X = (x * DivisionSize);
			end.Y = GetGridSizeY();

			CreateLine(start, end, FVector::UpVector, vertices, triangles);
		}
	}

	// Reset the start and end variables

	// Unused variables that are required to be passed to CreateMeshSection
	TArray<FVector> Normals;
	TArray<FVector2D> UVs;
	TArray<FColor> Colors;
	TArray<FProcMeshTangent> Tangents;

	// Add the geometry to the procedural mesh so it'll render
	ProceduralMesh->CreateMeshSection(0, vertices, triangles, Normals, UVs, Colors, Tangents, false);

	// Set the material on the procedural mesh so its color/opacity can be configurable
	UMaterialInstanceDynamic* dynamicMaterialInstance = UMaterialInstanceDynamic::Create(GridMaterial, this);
	dynamicMaterialInstance->SetVectorParameterValue("Color", GridColor);
	dynamicMaterialInstance->SetScalarParameterValue("Opacity", GridColor.A);
	ProceduralMesh->SetMaterial(0, dynamicMaterialInstance);
	UE_LOG(LogTemp, Warning, TEXT("This is working"));
}

void ANavBoxVolume::CreateLine(const FVector& start, FVector& end, const FVector& normal, TArray<FVector>& vertices, TArray<int32>& triangles)
{
	// Calculate half line thickness and thickness direction
	float halfLineThickness = LineThickness / 2.0f;
	FVector line = end - start;
	line.Normalize();

	// 0------1
	// | line |
	// 2------3
	/*
	* The [] is the capture-clause which is used to capture variables
	* Eg:
	* [=]	Capture variables within scope by value
	* [&]	Capture variables within scope by reference
	* [&var] Capture var by reference
	* [&, var]	Default way of capturing variables in scope is by reference, we want to capture var
	* [=, &var]		Default way of capturing variables in scope is by value but capture var by reference instead
	*/
	// auto createFlatLine is a lambda function
	// [&] means every variable in scope of the lambda fn is captured by reference
	auto createFlatLine = [&](const FVector& thicknessDirection)
	{
		// Top triangle
		triangles.Add(vertices.Num() + 2);
		triangles.Add(vertices.Num() + 1);
		triangles.Add(vertices.Num() + 0);

		// Bottom triangle
		triangles.Add(vertices.Num() + 2);
		triangles.Add(vertices.Num() + 3);
		triangles.Add(vertices.Num() + 1);

		// Vertex 0
		vertices.Add(start + (thicknessDirection * halfLineThickness));
		// Vertex 1
		vertices.Add(end + (thicknessDirection * halfLineThickness));
		//Vertex 2
		vertices.Add(start - (thicknessDirection * halfLineThickness));
		// Vertex 3
		vertices.Add(end - (thicknessDirection * halfLineThickness));
	};

	FVector direction1 = UKismetMathLibrary::Cross_VectorVector(line, normal);
	FVector direction2 = UKismetMathLibrary::Cross_VectorVector(line, direction1);

	createFlatLine(direction1);
	createFlatLine(direction2);
}

// Called when the game starts or when spawned
void ANavBoxVolume::BeginPlay()
{
	Super::BeginPlay();
}

void ANavBoxVolume::EndPlay(const EEndPlayReason::Type EndPlayReason)
{

	Super::EndPlay(EndPlayReason);
}

// Called every frame
void ANavBoxVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool ANavBoxVolume::FindPath(const FVector& start, const FVector& destination, const TArray<TEnumAsByte<EObjectTypeQuery>>& object_types, UClass* actor_class_filter, TArray<FVector>& out_path)
{
	return true; // placeholder
}

FIntVector ANavBoxVolume::ConvertLocationToCoordinates(const FVector& location)
{
	FIntVector coordinates;

	return coordinates;
}

FVector ANavBoxVolume::ConvertCoordinateToLocation(const FIntVector& coordinates)
{
	FVector gridSpaceLocation = FVector::ZeroVector;

	return UKismetMathLibrary::TransformLocation(GetActorTransform(), gridSpaceLocation);
}

bool ANavBoxVolume::AreCoordinatesValid(const FIntVector& coordinates)const
{
	return coordinates.X >= 0 && coordinates.X < DivisionsX&&
		coordinates.Y >= 0 && coordinates.Y < DivisionsY&&
		coordinates.Z >= 0 && coordinates.Z < DivisionsZ;
}

void ANavBoxVolume::ClampCoordinates(FIntVector& coordinaes) const
{

}

NavNode* ANavBoxVolume::GetNode(FIntVector coordinates) const
{
	int32 index = 0;//Placeholder
	return &Nodes[index];
}