#include <BOP_SolidAreaBuilder.ixx>

// =====================================================================
// function: BOP_SolidAreaBuilder::BOP_SolidAreaBuilder
// purpose: 
// =====================================================================
  BOP_SolidAreaBuilder::BOP_SolidAreaBuilder()
{
}

// =====================================================================
// function: BOP_SolidAreaBuilder
// purpose: 
// =====================================================================
  BOP_SolidAreaBuilder::BOP_SolidAreaBuilder(BOP_LoopSet&           theLS,
					     BOP_LoopClassifier&    theLC,
					     const Standard_Boolean theForceClassFlag)
{
  InitSolidAreaBuilder(theLS, theLC, theForceClassFlag);
}

// =====================================================================
// function: InitSolidAreaBuilder
// purpose: 
// =====================================================================
  void BOP_SolidAreaBuilder::InitSolidAreaBuilder(BOP_LoopSet&           theLS,
						  BOP_LoopClassifier&    theLC,
						  const Standard_Boolean theForceClassFlag) 
{
  InitAreaBuilder(theLS, theLC, theForceClassFlag);
}

