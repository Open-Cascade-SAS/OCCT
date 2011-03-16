// File:	StepShape_ConnectedFaceSubSet.cxx
// Created:	Fri Jan  4 17:42:43 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1
// Copyright:	Matra Datavision 2000

#include <StepShape_ConnectedFaceSubSet.ixx>

//=======================================================================
//function : StepShape_ConnectedFaceSubSet
//purpose  : 
//=======================================================================

StepShape_ConnectedFaceSubSet::StepShape_ConnectedFaceSubSet ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepShape_ConnectedFaceSubSet::Init (const Handle(TCollection_HAsciiString) &aRepresentationItem_Name,
                                          const Handle(StepShape_HArray1OfFace) &aConnectedFaceSet_CfsFaces,
                                          const Handle(StepShape_ConnectedFaceSet) &aParentFaceSet)
{
  StepShape_ConnectedFaceSet::Init(aRepresentationItem_Name,
                                   aConnectedFaceSet_CfsFaces);

  theParentFaceSet = aParentFaceSet;
}

//=======================================================================
//function : ParentFaceSet
//purpose  : 
//=======================================================================

Handle(StepShape_ConnectedFaceSet) StepShape_ConnectedFaceSubSet::ParentFaceSet () const
{
  return theParentFaceSet;
}

//=======================================================================
//function : SetParentFaceSet
//purpose  : 
//=======================================================================

void StepShape_ConnectedFaceSubSet::SetParentFaceSet (const Handle(StepShape_ConnectedFaceSet) &aParentFaceSet)
{
  theParentFaceSet = aParentFaceSet;
}
