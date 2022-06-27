// Created on: 1995-12-01
// Created by: EXPRESS->CDL V0.2 Translator
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

#ifndef _StepShape_FaceSurface_HeaderFile
#define _StepShape_FaceSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepShape_Face.hxx>
#include <StepShape_HArray1OfFaceBound.hxx>
class StepGeom_Surface;
class TCollection_HAsciiString;


class StepShape_FaceSurface;
DEFINE_STANDARD_HANDLE(StepShape_FaceSurface, StepShape_Face)


class StepShape_FaceSurface : public StepShape_Face
{

public:

  
  //! Returns a FaceSurface
  Standard_EXPORT StepShape_FaceSurface();
  
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aName, const Handle(StepShape_HArray1OfFaceBound)& aBounds, const Handle(StepGeom_Surface)& aFaceGeometry, const Standard_Boolean aSameSense);
  
  Standard_EXPORT void SetFaceGeometry (const Handle(StepGeom_Surface)& aFaceGeometry);
  
  Standard_EXPORT Handle(StepGeom_Surface) FaceGeometry() const;
  
  Standard_EXPORT void SetSameSense (const Standard_Boolean aSameSense);
  
  Standard_EXPORT Standard_Boolean SameSense() const;




  DEFINE_STANDARD_RTTIEXT(StepShape_FaceSurface,StepShape_Face)

protected:




private:


  Handle(StepGeom_Surface) faceGeometry;
  Standard_Boolean sameSense;


};







#endif // _StepShape_FaceSurface_HeaderFile
