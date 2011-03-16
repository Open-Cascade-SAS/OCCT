// File:	BOP_FaceAreaBuilder.cxx
// Created:	Thu Dec 21 17:07:40 1995
// Author:	Jean Yves LEBEY
//		<jyl@meteox>
//Copyright:	 Matra Datavision 1995
//modified by NIZNHY-PKV Tue Apr  3 15:55:58 2001

#include <BOP_FaceAreaBuilder.ixx>

//=======================================================================
//function : BOP_FaceAreaBuilder
//purpose  : 
//=======================================================================
  BOP_FaceAreaBuilder::BOP_FaceAreaBuilder()
{
}

//=======================================================================
//function : BOP_FaceAreaBuilder
//purpose  : 
//=======================================================================
  BOP_FaceAreaBuilder::BOP_FaceAreaBuilder(BOP_LoopSet& LS, 
					   BOP_LoopClassifier& LC,
					   const Standard_Boolean ForceClass)
{
  InitFaceAreaBuilder(LS,LC,ForceClass);
}

//=======================================================================
//function : InitFaceAreaBuilder
//purpose  : 
//=======================================================================
  void BOP_FaceAreaBuilder::InitFaceAreaBuilder(BOP_LoopSet& LS, 
						BOP_LoopClassifier& LC,
						const Standard_Boolean ForceClass)
{
  InitAreaBuilder(LS,LC,ForceClass);
}
