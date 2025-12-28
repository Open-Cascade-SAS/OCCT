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

#ifndef _StepShape_GeometricSet_HeaderFile
#define _StepShape_GeometricSet_HeaderFile

#include <Standard.hxx>

#include <StepShape_GeometricSetSelect.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepGeom_GeometricRepresentationItem.hxx>
#include <Standard_Integer.hxx>
class TCollection_HAsciiString;
class StepShape_GeometricSetSelect;

class StepShape_GeometricSet : public StepGeom_GeometricRepresentationItem
{

public:
  //! Returns a GeometricSet
  Standard_EXPORT StepShape_GeometricSet();

  Standard_EXPORT void Init(const occ::handle<TCollection_HAsciiString>&              aName,
                            const occ::handle<NCollection_HArray1<StepShape_GeometricSetSelect>>& aElements);

  Standard_EXPORT void SetElements(const occ::handle<NCollection_HArray1<StepShape_GeometricSetSelect>>& aElements);

  Standard_EXPORT occ::handle<NCollection_HArray1<StepShape_GeometricSetSelect>> Elements() const;

  Standard_EXPORT StepShape_GeometricSetSelect ElementsValue(const int num) const;

  Standard_EXPORT int NbElements() const;

  DEFINE_STANDARD_RTTIEXT(StepShape_GeometricSet, StepGeom_GeometricRepresentationItem)

private:
  occ::handle<NCollection_HArray1<StepShape_GeometricSetSelect>> elements;
};

#endif // _StepShape_GeometricSet_HeaderFile
