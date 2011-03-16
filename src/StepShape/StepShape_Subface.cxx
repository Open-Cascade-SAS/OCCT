// File:	StepShape_Subface.cxx
// Created:	Fri Jan  4 17:42:45 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1
// Copyright:	Matra Datavision 2000

#include <StepShape_Subface.ixx>

//=======================================================================
//function : StepShape_Subface
//purpose  : 
//=======================================================================

StepShape_Subface::StepShape_Subface ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepShape_Subface::Init (const Handle(TCollection_HAsciiString) &aRepresentationItem_Name,
                              const Handle(StepShape_HArray1OfFaceBound) &aFace_Bounds,
                              const Handle(StepShape_Face) &aParentFace)
{
  StepShape_Face::Init(aRepresentationItem_Name,
                       aFace_Bounds);

  theParentFace = aParentFace;
}

//=======================================================================
//function : ParentFace
//purpose  : 
//=======================================================================

Handle(StepShape_Face) StepShape_Subface::ParentFace () const
{
  return theParentFace;
}

//=======================================================================
//function : SetParentFace
//purpose  : 
//=======================================================================

void StepShape_Subface::SetParentFace (const Handle(StepShape_Face) &aParentFace)
{
  theParentFace = aParentFace;
}
