// Copyright 2021, Hrishikesh P, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include <vector>

/**
 *  Each node will have a variable to store its co-ordinates
 *	A vecor array to store its neighbors
 *	An FScore for A* scoring purposes
 */
class NAVIGATION_3D_BOX_API NavNode
{
public:
	NavNode();
	~NavNode();
	FIntVector Coordinates;
	std::vector<NavNode*> Neighbors;
	float FScore = FLT_MAX;
};

/*
To check if the left node's FScore is less than the right node's FScore.
*/
struct NodeCompare
{
	bool operator()(const NavNode* lhs, const NavNode* rhs)const
	{
		return(lhs->FScore < rhs->FScore);
	}
};