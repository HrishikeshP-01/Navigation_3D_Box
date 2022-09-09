// Copyright 2021, Hrishikesh P, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NavBoxVolume.generated.h"

class UProceduralMeshComponent;
class NavNode;

UCLASS()
class NAVIGATION_3D_BOX_API ANavBoxVolume : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ANavBoxVolume();

private:
	// Root scene component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NavBoxVolume", meta = (AllowPrivateAccess = "true"))
		USceneComponent* DefaultSceneComponent = nullptr;
	// The procedural mesh responsible for rendering the grid
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NavBoxVolume", meta = (AllowPrivateAccess = "true"))
		UProceduralMeshComponent* ProceduralMesh = nullptr;

	// The number of divisions in the grid along the X axis
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NavBoxVolume|Pathfinding", meta = (AllowPrivateAccess = "true", ClampMin = 1))
		int32 DivisionsX = 10;
	// The number of divisions in the grid along the Y axis
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NavBoxVolume|Pathfinding", meta = (AllowPrivateAccess = "true", ClampMin = 1))
		int32 DivisionsY = 10;
	// The number of divisions in the grid along the Z axis
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NavBoxVolume|Pathfinding", meta = (AllowPrivateAccess = "true", ClampMin = 1))
		int32 DivisionsZ = 10;

	// The size of each division
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NavBoxVolume|Pathfinding", meta = (AllowPrivateAccess = "true", ClampMin = 1))
		float DivisionSize = 100.0f;

	// The min number of axes that must be shared with a neighboring node for it to be considered as a neighbor
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NavBoxVolume|Pathfinding", meta = (AllowPrivateAccess = "true", ClampMin = 0, ClampMax = 2))
		int32 MinSharedNeighborAxes = 0;

	// Aesthetics variables
	// Thickness of grid lines
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NavBoxVolume|Aesthetics", meta = (AllowPrivateAccess = "true", ClampMin = 0))
		float LineThickness = 2.0f;
	// The color of the grid
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NavBoxVolume|Aesthetics", meta = (AllowPrivateAccess = "true"))
		FLinearColor GridColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.5f);

public:
	/**
	*	Called when an instance is placed in editor or spawned
	*	@param Transform	The transform the actor was constructed at.
	*/
	virtual void OnConstruction(const FTransform& Transform) override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Gets the node at the specified coordinates
	NavNode* GetNode(FIntVector coordinates) const;

	// Finds a path from the starting location to the destination
	UFUNCTION(BlueprintCallable, Category = "NavBoxVolume")
		bool FindPath(const FVector& start, const FVector& destination, const TArray<TEnumAsByte<EObjectTypeQuery>>& object_types, UClass* actor_class_filter, TArray<FVector>& out_path);
	/*
	*	Converts a world space location to a coordinate in the grid.
	*	If the location is not located within the grid, the coordinate will be clamped to the closest coordinate
	*	@param location		The location to convert
	*	@return The converted coordinates
	*/
	UFUNCTION(BlueprintCallable, Category = "NavBoxVolume")
		UPARAM(DisplayName = "Coordinates") FIntVector ConvertLocationToCoordinates(const FVector& location);
	/*
	*	Converts a coordinate into world space location.
	*	If the coordinate is not within the bounds the coordinate will be clamped to the closest coordinate
	*	@param coordinates	 The coordinates to convert into world space location
	*	@return The converted location in world space
	*/
	UFUNCTION(BlueprintCallable, Category = "NavBoxVolume")
		UPARAM(DisplayName = "WorldLocation")FVector ConvertCoordinateToLocation(const FIntVector& coordinates);
	
	// Gets the total number of divisions in the grid
	UFUNCTION(BlueprintPure, Category = "NavBoxVolume")
		FORCEINLINE int32 GetTotalDivisions() const { return DivisionsX * DivisionsY * DivisionsZ; }
	// Gets the number of x divisions in the grid
	UFUNCTION(BlueprintPure, Category="NavBoxVolume")
		FORCEINLINE int32 GetDivisionsX() const { return DivisionsX; }
	// Gets the number of y divisions in the grid
	UFUNCTION(BlueprintPure, Category="NavBoxVolume")
		FORCEINLINE int32 GetDivisionsY() const { return DivisionsY; }
	// Gets the number of z divisions in the grid
	UFUNCTION(BlueprintPure, Category="NavBoxVolume")
		FORCEINLINE int32 GetDivisionsZ() const { return DivisionsZ; }
	// Gets the size of each division in the grid
	UFUNCTION(BlueprintPure, Category="NavBoxVolume")
		FORCEINLINE float GetDivisionsSize() const { return DivisionSize; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Overridable function called whenever this actor is being removed from a level
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// Gets the size of the grid along the X axis
	inline float GetGridSizeX() const { return DivisionsX * DivisionSize; }
	// Gets the size of the grid along the Y axis
	inline float GetGridSizeY() const { return DivisionsY * DivisionSize; }
	// Gets the size of the grid along the Z axis
	inline float GetGridSizeZ() const { return DivisionsZ * DivisionSize; }

private:
	// Helper function for creating the geometry for a single line of the grid
	void CreateLine(const FVector& start, FVector& end, const FVector& normal, TArray<FVector>& vertices, TArray<int32>& triangles);
	// Helper function to check if a coordinate is valid
	bool AreCoordinatesValid(const FIntVector& coordinates) const;
	// Helper function to clamp the coordinate to a valid one inside the grid
	void ClampCoordinates(FIntVector& coordinates) const;

	// Function to create Grid
	void CreateGrid();

	// The nodes used for pathfinding
	NavNode* Nodes = nullptr;
};
