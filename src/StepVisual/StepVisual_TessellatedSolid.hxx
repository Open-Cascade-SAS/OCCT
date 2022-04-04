// Created on : Thu Mar 24 18:30:12 2022 
// Created by: snn
// Generator: Express (EXPRESS -> CASCADE/XSTEP Translator) V2.0
// Copyright (c) Open CASCADE 2022
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

#ifndef _StepVisual_TessellatedSolid_HeaderFile_
#define _StepVisual_TessellatedSolid_HeaderFile_

#include <Standard.hxx>
#include <Standard_Type.hxx>
#include <StepVisual_TessellatedItem.hxx>

#include <StepVisual_HArray1OfTessellatedStructuredItem.hxx>
#include <StepShape_ManifoldSolidBrep.hxx>

DEFINE_STANDARD_HANDLE(StepVisual_TessellatedSolid, StepVisual_TessellatedItem)

//! Representation of STEP entity TessellatedSolid
class StepVisual_TessellatedSolid : public StepVisual_TessellatedItem
{

public :

  //! default constructor
  Standard_EXPORT StepVisual_TessellatedSolid();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& theRepresentationItem_Name,
                            const Handle(StepVisual_HArray1OfTessellatedStructuredItem)& theItems,
                            const Standard_Boolean theHasGeometricLink,
                            const Handle(StepShape_ManifoldSolidBrep)& theGeometricLink);

  //! Returns field Items
  Standard_EXPORT Handle(StepVisual_HArray1OfTessellatedStructuredItem) Items() const;

  //! Sets field Items
  Standard_EXPORT void SetItems (const Handle(StepVisual_HArray1OfTessellatedStructuredItem)& theItems);

  //! Returns number of Items
  Standard_EXPORT Standard_Integer NbItems() const;

  //! Returns value of Items by its num
  Standard_EXPORT Handle(StepVisual_TessellatedStructuredItem) ItemsValue(const Standard_Integer theNum) const;

  //! Returns field GeometricLink
  Standard_EXPORT Handle(StepShape_ManifoldSolidBrep) GeometricLink() const;

  //! Sets field GeometricLink
  Standard_EXPORT void SetGeometricLink (const Handle(StepShape_ManifoldSolidBrep)& theGeometricLink);

  //! Returns True if optional field GeometricLink is defined
  Standard_EXPORT Standard_Boolean HasGeometricLink() const;

  DEFINE_STANDARD_RTTIEXT(StepVisual_TessellatedSolid, StepVisual_TessellatedItem)

private:

  Handle(StepVisual_HArray1OfTessellatedStructuredItem) myItems;
  Handle(StepShape_ManifoldSolidBrep) myGeometricLink; //!< optional
  Standard_Boolean myHasGeometricLink; //!< flag "is GeometricLink defined"

};

#endif // _StepVisual_TessellatedSolid_HeaderFile_
