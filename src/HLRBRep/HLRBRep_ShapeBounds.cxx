// File:	HLRBRep_ShapeBounds.cxx
// Created:	Thu Apr 17 18:35:42 1997
// Author:	Christophe MARION
//		<cma@partox.paris1.matra-dtv.fr>


#include <HLRBRep_ShapeBounds.ixx>

//=======================================================================
//function : HLRBRep_ShapeBounds
//purpose  : 
//=======================================================================

HLRBRep_ShapeBounds::
HLRBRep_ShapeBounds (const Handle(HLRTopoBRep_OutLiner)& S,
		     const Handle(MMgt_TShared)& SData,
		     const Standard_Integer nbIso,
		     const Standard_Integer V1,
		     const Standard_Integer V2,
		     const Standard_Integer E1,
		     const Standard_Integer E2,
		     const Standard_Integer F1,
		     const Standard_Integer F2) :
		     myShape(S),
		     myShapeData(SData),
		     myNbIso(nbIso),
		     myVertStart(V1),myVertEnd(V2),
		     myEdgeStart(E1),myEdgeEnd(E2),
		     myFaceStart(F1),myFaceEnd(F2)
{}

//=======================================================================
//function : HLRBRep_ShapeBounds
//purpose  : 
//=======================================================================

HLRBRep_ShapeBounds::
HLRBRep_ShapeBounds (const Handle(HLRTopoBRep_OutLiner)& S,
		     const Standard_Integer nbIso,
		     const Standard_Integer V1,
		     const Standard_Integer V2,
		     const Standard_Integer E1,
		     const Standard_Integer E2,
		     const Standard_Integer F1,
		     const Standard_Integer F2) :
		     myShape(S),
		     myNbIso(nbIso),
		     myVertStart(V1),myVertEnd(V2),
		     myEdgeStart(E1),myEdgeEnd(E2),
		     myFaceStart(F1),myFaceEnd(F2)
{}

//=======================================================================
//function : Translate
//purpose  : 
//=======================================================================

void HLRBRep_ShapeBounds::Translate (const Standard_Integer NV,
				     const Standard_Integer NE,
				     const Standard_Integer NF)
{
  myVertStart += NV;
  myVertEnd   += NV;
  myEdgeStart += NE;
  myEdgeEnd   += NE;
  myFaceStart += NF;
  myFaceEnd   += NF;
}

//=======================================================================
//function : Sizes
//purpose  : 
//=======================================================================

void HLRBRep_ShapeBounds::Sizes (Standard_Integer& NV,
				 Standard_Integer& NE,
				 Standard_Integer& NF) const
{
  NV = myVertEnd + 1 - myVertStart;
  NE = myEdgeEnd + 1 - myEdgeStart;
  NF = myFaceEnd + 1 - myFaceStart;
}

//=======================================================================
//function : Bounds
//purpose  : 
//=======================================================================

void HLRBRep_ShapeBounds::Bounds (Standard_Integer& V1,
				  Standard_Integer& V2,
				  Standard_Integer& E1,
				  Standard_Integer& E2,
				  Standard_Integer& F1,
				  Standard_Integer& F2) const
{
  V1 = myVertStart;
  V2 = myVertEnd;
  E1 = myEdgeStart;
  E2 = myEdgeEnd;
  F1 = myFaceStart;
  F2 = myFaceEnd;
}

//=======================================================================
//function : UpdateMinMax
//purpose  : 
//=======================================================================

void HLRBRep_ShapeBounds::UpdateMinMax (const Standard_Address TotMinMax)
{
  for (Standard_Integer i = 0; i <= 15; i++)
    myMinMax[i] = ((Standard_Integer*)TotMinMax)[i];
}

