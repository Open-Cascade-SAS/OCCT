// File:	StepFEA_FeaParametricPoint.cxx
// Created:	Thu Dec 12 17:51:06 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <StepFEA_FeaParametricPoint.ixx>

//=======================================================================
//function : StepFEA_FeaParametricPoint
//purpose  : 
//=======================================================================

StepFEA_FeaParametricPoint::StepFEA_FeaParametricPoint ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepFEA_FeaParametricPoint::Init (const Handle(TCollection_HAsciiString) &aRepresentationItem_Name,
                                       const Handle(TColStd_HArray1OfReal) &aCoordinates)
{
  StepGeom_Point::Init(aRepresentationItem_Name);

  theCoordinates = aCoordinates;
}

//=======================================================================
//function : Coordinates
//purpose  : 
//=======================================================================

Handle(TColStd_HArray1OfReal) StepFEA_FeaParametricPoint::Coordinates () const
{
  return theCoordinates;
}

//=======================================================================
//function : SetCoordinates
//purpose  : 
//=======================================================================

void StepFEA_FeaParametricPoint::SetCoordinates (const Handle(TColStd_HArray1OfReal) &aCoordinates)
{
  theCoordinates = aCoordinates;
}
