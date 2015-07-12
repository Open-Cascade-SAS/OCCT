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


#include <Geom_Transformation.hxx>
#include <gp_Trsf.hxx>
#include <Graphic3d_DataStructureManager.hxx>
#include <Graphic3d_Structure.hxx>
#include <Graphic3d_StructureManager.hxx>
#include <Precision.hxx>
#include <PrsMgr_Presentation.hxx>
#include <PrsMgr_Prs.hxx>
#include <Standard_Type.hxx>

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

Handle(Graphic3d_Structure) PrsMgr_Prs::Compute(const Handle(Graphic3d_DataStructureManager)& aProjector,
						const TColStd_Array2OfReal& AMatrix) 
{
  gp_Trsf TheTrsf;
  Standard_Integer LC(AMatrix.LowerCol()),LR(AMatrix.LowerRow());
  TheTrsf.SetValues(AMatrix(LR,LC),AMatrix(LR,LC+1),AMatrix(LR,LC+2),AMatrix(LR,LC+3),
		    AMatrix(LR+1,LC),AMatrix(LR+1,LC+1),AMatrix(LR+1,LC+2),AMatrix(LR+1,LC+3),
		    AMatrix(LR+2,LC),AMatrix(LR+2,LC+1),AMatrix(LR+2,LC+2),AMatrix(LR+2,LC+3));
   Handle(Geom_Transformation) G = new Geom_Transformation(TheTrsf);
  
  return myPresentation3d->Compute(aProjector,G);
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

void PrsMgr_Prs::Compute(const Handle(Graphic3d_DataStructureManager)& aProjector,
			 const TColStd_Array2OfReal& AMatrix,
			  Handle(Graphic3d_Structure)& aGivenStruct) 
{
  gp_Trsf TheTrsf;
  Standard_Integer LC(AMatrix.LowerCol()),LR(AMatrix.LowerRow());
  TheTrsf.SetValues(AMatrix(LR,LC),AMatrix(LR,LC+1),AMatrix(LR,LC+2),AMatrix(LR,LC+3),
		    AMatrix(LR+1,LC),AMatrix(LR+1,LC+1),AMatrix(LR+1,LC+2),AMatrix(LR+1,LC+3),
		    AMatrix(LR+2,LC),AMatrix(LR+2,LC+1),AMatrix(LR+2,LC+2),AMatrix(LR+2,LC+3));
  Handle(Geom_Transformation) G = new Geom_Transformation(TheTrsf);


  myPresentation3d->Compute(aProjector,G,aGivenStruct);
}
