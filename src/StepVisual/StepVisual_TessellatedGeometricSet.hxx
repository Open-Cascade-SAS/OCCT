// Created on: 2015-10-29
// Created by: Galina Kulikova
// Copyright (c) 2015 OPEN CASCADE SAS
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

#ifndef _StepVisual_TessellatedGeometricSet_HeaderFile
#define  _StepVisual_TessellatedGeometricSet_HeaderFile
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <StepVisual_TessellatedItem.hxx>
#include <NCollection_Array1.hxx>
//#include <NCollection_DefineHArray1.hxx>
#include <NCollection_Handle.hxx>

class Standard_Transient;

typedef NCollection_Array1<Handle(StepVisual_TessellatedItem)> StepVisual_Array1OfTessellatedItem;

DEFINE_STANDARD_HANDLE(StepVisual_TessellatedGeometricSet, StepVisual_TessellatedItem)
class StepVisual_TessellatedGeometricSet  : public StepVisual_TessellatedItem
{
public:

  DEFINE_STANDARD_ALLOC
  
  //! Returns a DraughtingCalloutElement select type
  Standard_EXPORT StepVisual_TessellatedGeometricSet();

   Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& theName, const NCollection_Handle<StepVisual_Array1OfTessellatedItem>& theItems);
  
   Standard_EXPORT NCollection_Handle<StepVisual_Array1OfTessellatedItem> Items() const;

private:
  NCollection_Handle<StepVisual_Array1OfTessellatedItem> myItems;

public:

  DEFINE_STANDARD_RTTIEXT(StepVisual_TessellatedGeometricSet,StepVisual_TessellatedItem)
};
#endif // StepVisual_TessellatedGeometricSet
