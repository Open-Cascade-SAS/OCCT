// Created on: 1995-01-31
// Created by: Mister rmi
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

#ifndef _PrsMgr_Prs_HeaderFile
#define _PrsMgr_Prs_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <PrsMgr_PresentationPointer.hxx>
#include <Prs3d_Presentation.hxx>
#include <PrsMgr_TypeOfPresentation3d.hxx>
#include <TColStd_Array2OfReal.hxx>
class Graphic3d_StructureManager;
class Graphic3d_Structure;
class Graphic3d_DataStructureManager;

class PrsMgr_Prs;
DEFINE_STANDARD_HANDLE(PrsMgr_Prs, Prs3d_Presentation)

class PrsMgr_Prs : public Prs3d_Presentation
{

public:
  
  Standard_EXPORT PrsMgr_Prs(const Handle(Graphic3d_StructureManager)& theStructManager, const PrsMgr_PresentationPointer& thePresentation, const PrsMgr_TypeOfPresentation3d theTypeOfPresentation3d);
  
  Standard_EXPORT void Compute() Standard_OVERRIDE;
  
  Standard_EXPORT Handle(Graphic3d_Structure) Compute (const Handle(Graphic3d_DataStructureManager)& aProjector) Standard_OVERRIDE;
  
  //! the "degenerated" Structure is displayed with
  //! a transformation defined by <AMatrix>
  //! which is not a Pure Translation.
  //! We have to take in account this Transformation
  //! in the computation of hidden line removal...
  //! returns a filled Graphic Structure.
  Standard_EXPORT Handle(Graphic3d_Structure) Compute (const Handle(Graphic3d_DataStructureManager)& theProjector,
                                                       const Handle(Geom_Transformation)& theTrsf) Standard_OVERRIDE;

  //! No need to return a structure, just to fill
  //! <ComputedStruct> ....
  Standard_EXPORT void Compute (const Handle(Graphic3d_DataStructureManager)& aProjector, Handle(Graphic3d_Structure)& ComputedStruct) Standard_OVERRIDE;
  
  //! No Need to return a Structure, just to
  //! Fill <aStructure>. The Trsf has to be taken in account
  //! in the computation (Rotation Part....)
  Standard_EXPORT void Compute (const Handle(Graphic3d_DataStructureManager)& theProjector,
                                const Handle(Geom_Transformation)& theTrsf,
                                Handle(Graphic3d_Structure)& theStructure) Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(PrsMgr_Prs,Prs3d_Presentation)

private:

  PrsMgr_PresentationPointer myPresentation3d;

};

#endif // _PrsMgr_Prs_HeaderFile
