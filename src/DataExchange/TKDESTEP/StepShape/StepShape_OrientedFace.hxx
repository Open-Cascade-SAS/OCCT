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
#include <StepShape_FaceBound.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Standard_Integer.hxx>
class TCollection_HAsciiString;
class StepShape_FaceBound;

class StepShape_OrientedFace : public StepShape_Face
{

public:
  //! Returns a OrientedFace
  Standard_EXPORT StepShape_OrientedFace();

  Standard_EXPORT void Init(const occ::handle<TCollection_HAsciiString>& aName,
                            const occ::handle<StepShape_Face>&           aFaceElement,
                            const bool                  aOrientation);

  Standard_EXPORT void SetFaceElement(const occ::handle<StepShape_Face>& aFaceElement);

  Standard_EXPORT occ::handle<StepShape_Face> FaceElement() const;

  Standard_EXPORT void SetOrientation(const bool aOrientation);

  Standard_EXPORT bool Orientation() const;

  Standard_EXPORT virtual void SetBounds(const occ::handle<NCollection_HArray1<occ::handle<StepShape_FaceBound>>>& aBounds)
    override;

  Standard_EXPORT virtual occ::handle<NCollection_HArray1<occ::handle<StepShape_FaceBound>>> Bounds() const override;

  Standard_EXPORT virtual occ::handle<StepShape_FaceBound> BoundsValue(const int num) const
    override;

  Standard_EXPORT virtual int NbBounds() const override;

  DEFINE_STANDARD_RTTIEXT(StepShape_OrientedFace, StepShape_Face)

private:
  occ::handle<StepShape_Face> faceElement;
  bool       orientation;
};

#endif // _StepShape_OrientedFace_HeaderFile
