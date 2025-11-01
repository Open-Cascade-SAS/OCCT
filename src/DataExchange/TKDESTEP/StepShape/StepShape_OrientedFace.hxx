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

#ifndef _StepShape_OrientedFace_HeaderFile
#define _StepShape_OrientedFace_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Boolean.hxx>
#include <StepShape_Face.hxx>
#include <StepShape_HArray1OfFaceBound.hxx>
#include <Standard_Integer.hxx>
class TCollection_HAsciiString;
class StepShape_FaceBound;

class StepShape_OrientedFace;
DEFINE_STANDARD_HANDLE(StepShape_OrientedFace, StepShape_Face)

class StepShape_OrientedFace : public StepShape_Face
{

public:
  //! Returns a OrientedFace
  Standard_EXPORT StepShape_OrientedFace();

  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& aName,
                            const Handle(StepShape_Face)&           aFaceElement,
                            const Standard_Boolean                  aOrientation);

  Standard_EXPORT void SetFaceElement(const Handle(StepShape_Face)& aFaceElement);

  Standard_EXPORT Handle(StepShape_Face) FaceElement() const;

  Standard_EXPORT void SetOrientation(const Standard_Boolean aOrientation);

  Standard_EXPORT Standard_Boolean Orientation() const;

  Standard_EXPORT virtual void SetBounds(const Handle(StepShape_HArray1OfFaceBound)& aBounds)
    Standard_OVERRIDE;

  Standard_EXPORT virtual Handle(StepShape_HArray1OfFaceBound) Bounds() const Standard_OVERRIDE;

  Standard_EXPORT virtual Handle(StepShape_FaceBound) BoundsValue(const Standard_Integer num) const
    Standard_OVERRIDE;

  Standard_EXPORT virtual Standard_Integer NbBounds() const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(StepShape_OrientedFace, StepShape_Face)

protected:
private:
  Handle(StepShape_Face) faceElement;
  Standard_Boolean       orientation;
};

#endif // _StepShape_OrientedFace_HeaderFile
