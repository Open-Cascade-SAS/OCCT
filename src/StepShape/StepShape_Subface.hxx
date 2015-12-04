// Created on: 2002-01-04
// Created by: data exchange team
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#ifndef _StepShape_Subface_HeaderFile
#define _StepShape_Subface_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepShape_Face.hxx>
#include <StepShape_HArray1OfFaceBound.hxx>
class StepShape_Face;
class TCollection_HAsciiString;


class StepShape_Subface;
DEFINE_STANDARD_HANDLE(StepShape_Subface, StepShape_Face)

//! Representation of STEP entity Subface
class StepShape_Subface : public StepShape_Face
{

public:

  
  //! Empty constructor
  Standard_EXPORT StepShape_Subface();
  
  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& aRepresentationItem_Name, const Handle(StepShape_HArray1OfFaceBound)& aFace_Bounds, const Handle(StepShape_Face)& aParentFace);
  
  //! Returns field ParentFace
  Standard_EXPORT Handle(StepShape_Face) ParentFace() const;
  
  //! Set field ParentFace
  Standard_EXPORT void SetParentFace (const Handle(StepShape_Face)& ParentFace);




  DEFINE_STANDARD_RTTIEXT(StepShape_Subface,StepShape_Face)

protected:




private:


  Handle(StepShape_Face) theParentFace;


};







#endif // _StepShape_Subface_HeaderFile
