// Copyright (c) 1995-1999 Matra Datavision
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

#include <Prs3d_Presentation.hxx>

#include <Geom_Transformation.hxx>
#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Graphic3d_DataStructureManager.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_NameOfMaterial.hxx>
#include <Graphic3d_Structure.hxx>
#include <Graphic3d_StructureManager.hxx>
#include <Prs3d_Root.hxx>
#include <Standard_Real.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Prs3d_Presentation,Graphic3d_Structure)

//=======================================================================
//function : Prs3d_Presentation
//purpose  :
//=======================================================================
Prs3d_Presentation::Prs3d_Presentation (const Handle(Graphic3d_StructureManager)& theViewer,
                                        const Standard_Boolean                    theToInit)
: Graphic3d_Structure (theViewer)
{
  if (!theToInit)
  {
    return;
  }
}

//=======================================================================
//function : Prs3d_Presentation
//purpose  :
//=======================================================================
Prs3d_Presentation::Prs3d_Presentation (const Handle(Graphic3d_StructureManager)& theViewer,
                                        const Handle(Prs3d_Presentation)&         thePrs)
: Graphic3d_Structure (theViewer, thePrs)
{
  //
}


//=======================================================================
//function : Connect
//purpose  : 
//=======================================================================
void Prs3d_Presentation::Connect
  ( const Handle(Prs3d_Presentation)& aPresentation) 
{
  Graphic3d_Structure::Connect(aPresentation, Graphic3d_TOC_DESCENDANT);
}


//=======================================================================
//function : Remove
//purpose  : 
//=======================================================================
void Prs3d_Presentation::Remove (const Handle(Prs3d_Presentation)& aPresentation) 
{
  Disconnect(aPresentation);
}

//=======================================================================
//function : RemoveAll
//purpose  : 
//=======================================================================
void Prs3d_Presentation::RemoveAll () 
{
  DisconnectAll(Graphic3d_TOC_DESCENDANT);
}


//=======================================================================
//function : CurrentGroup
//purpose  : 
//=======================================================================
Handle(Graphic3d_Group) Prs3d_Presentation::CurrentGroup () const 
{
  if (Groups().IsEmpty())
  {
    return const_cast<Prs3d_Presentation* >(this)->NewGroup();
  }
  return Groups().Last();
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================

Handle(Graphic3d_Structure) Prs3d_Presentation::
       Compute(const Handle(Graphic3d_DataStructureManager)& /*aProjector*/) 
{
  return this;
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================

void Prs3d_Presentation::Compute(const Handle(Graphic3d_DataStructureManager)& aDataStruct, 
                                 Handle(Graphic3d_Structure)& aStruct)
{
 Graphic3d_Structure::Compute(aDataStruct,aStruct );
}

//=======================================================================
//function : Compute
//purpose  :
//=======================================================================

Handle(Graphic3d_Structure) Prs3d_Presentation::Compute (const Handle(Graphic3d_DataStructureManager)& theDataStruc,
                                                         const Handle(Geom_Transformation)& theTrsf)
{
  return Graphic3d_Structure::Compute (theDataStruc, theTrsf);
}

//=======================================================================
//function : Compute
//purpose  :
//=======================================================================

void Prs3d_Presentation::Compute (const Handle(Graphic3d_DataStructureManager)& theDataStruc,
                                  const Handle(Geom_Transformation)& theTrsf,
                                  Handle(Graphic3d_Structure)& theStruc)
{
  Graphic3d_Structure::Compute (theDataStruc, theTrsf, theStruc);
}
