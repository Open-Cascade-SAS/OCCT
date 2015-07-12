// Created on: 1999-02-12
// Created by: Andrey BETENEV
// Copyright (c) 1999 Matra Datavision
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

#ifndef _StepToTopoDS_TranslateCurveBoundedSurface_HeaderFile
#define _StepToTopoDS_TranslateCurveBoundedSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Face.hxx>
#include <StepToTopoDS_Root.hxx>
#include <Standard_Boolean.hxx>
class StepGeom_CurveBoundedSurface;
class Transfer_TransientProcess;
class TopoDS_Face;


//! Translate curve_bounded_surface into TopoDS_Face
class StepToTopoDS_TranslateCurveBoundedSurface  : public StepToTopoDS_Root
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Create empty tool
  Standard_EXPORT StepToTopoDS_TranslateCurveBoundedSurface();
  
  //! Translate surface
  Standard_EXPORT StepToTopoDS_TranslateCurveBoundedSurface(const Handle(StepGeom_CurveBoundedSurface)& CBS, const Handle(Transfer_TransientProcess)& TP);
  
  //! Translate surface
  Standard_EXPORT Standard_Boolean Init (const Handle(StepGeom_CurveBoundedSurface)& CBS, const Handle(Transfer_TransientProcess)& TP);
  
  //! Returns result of last translation or null wire if failed.
  Standard_EXPORT const TopoDS_Face& Value() const;




protected:





private:



  TopoDS_Face myFace;


};







#endif // _StepToTopoDS_TranslateCurveBoundedSurface_HeaderFile
