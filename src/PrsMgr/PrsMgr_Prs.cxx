// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <PrsMgr_Prs.hxx>

#include <Geom_Transformation.hxx>
#include <gp_Trsf.hxx>
#include <Graphic3d_DataStructureManager.hxx>
#include <Graphic3d_Structure.hxx>
#include <Graphic3d_StructureManager.hxx>
#include <Precision.hxx>
#include <PrsMgr_Presentation.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(PrsMgr_Prs,Prs3d_Presentation)

PrsMgr_Prs::PrsMgr_Prs (const Handle(Graphic3d_StructureManager)& theStructManager,
                        const PrsMgr_PresentationPointer&         thePrs,
                        const PrsMgr_TypeOfPresentation3d         theTypeOfPresentation)
: Prs3d_Presentation (theStructManager),
  myPresentation3d   (thePrs)
{
  if (theTypeOfPresentation == PrsMgr_TOP_ProjectorDependant)
    SetVisual(Graphic3d_TOS_COMPUTED);
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================

void PrsMgr_Prs::Compute()
{
  myPresentation3d->Compute (this);
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================

Handle(Graphic3d_Structure) PrsMgr_Prs::Compute(const Handle(Graphic3d_DataStructureManager)& aProjector) {
  return myPresentation3d->Compute(aProjector);
}
//=======================================================================
//function : Compute
//purpose  :
//=======================================================================

Handle(Graphic3d_Structure) PrsMgr_Prs::Compute (const Handle(Graphic3d_DataStructureManager)& theProjector,
						                                     const Handle(Geom_Transformation)& theTrsf)
{
  return myPresentation3d->Compute (theProjector, theTrsf);
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================

void PrsMgr_Prs::Compute(const Handle(Graphic3d_DataStructureManager)& aProjector,
			 Handle(Graphic3d_Structure)& aGivenStruct) 
{
  myPresentation3d->Compute(aProjector,aGivenStruct);
}


//=======================================================================
//function : Compute
//purpose  :
//=======================================================================

void PrsMgr_Prs::Compute (const Handle(Graphic3d_DataStructureManager)& theProjector,
                          const Handle(Geom_Transformation)& theTrsf,
                          Handle(Graphic3d_Structure)& theGivenStruct)
{
  myPresentation3d->Compute (theProjector, theTrsf, theGivenStruct);
}
