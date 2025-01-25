// Created on: 1999-09-20
// Created by: Peter KURNEV
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

#ifndef _TopOpeBRepDS_ShapeWithState_HeaderFile
#define _TopOpeBRepDS_ShapeWithState_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopTools_ListOfShape.hxx>
#include <TopAbs_State.hxx>
#include <Standard_Boolean.hxx>
class TopoDS_Shape;

class TopOpeBRepDS_ShapeWithState
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT TopOpeBRepDS_ShapeWithState();

  Standard_EXPORT const TopTools_ListOfShape& Part(const TopAbs_State aState) const;

  Standard_EXPORT void AddPart(const TopoDS_Shape& aShape, const TopAbs_State aState);

  Standard_EXPORT void AddParts(const TopTools_ListOfShape& aListOfShape,
                                const TopAbs_State          aState);

  Standard_EXPORT void SetState(const TopAbs_State aState);

  Standard_EXPORT TopAbs_State State() const;

  Standard_EXPORT void SetIsSplitted(const Standard_Boolean anIsSplitted);

  Standard_EXPORT Standard_Boolean IsSplitted() const;

protected:
private:
  TopTools_ListOfShape myPartIn;
  TopTools_ListOfShape myPartOut;
  TopTools_ListOfShape myPartOn;
  TopAbs_State         myState;
  Standard_Boolean     myIsSplitted;
};

#endif // _TopOpeBRepDS_ShapeWithState_HeaderFile
