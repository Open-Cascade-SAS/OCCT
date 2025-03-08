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

#ifndef _StepShape_LoopAndPath_HeaderFile
#define _StepShape_LoopAndPath_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepShape_TopologicalRepresentationItem.hxx>
#include <StepShape_HArray1OfOrientedEdge.hxx>
#include <Standard_Integer.hxx>
class StepShape_Loop;
class StepShape_Path;
class TCollection_HAsciiString;
class StepShape_OrientedEdge;

class StepShape_LoopAndPath;
DEFINE_STANDARD_HANDLE(StepShape_LoopAndPath, StepShape_TopologicalRepresentationItem)

class StepShape_LoopAndPath : public StepShape_TopologicalRepresentationItem
{

public:
  //! Returns a LoopAndPath
  Standard_EXPORT StepShape_LoopAndPath();

  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& aName,
                            const Handle(StepShape_Loop)&           aLoop,
                            const Handle(StepShape_Path)&           aPath);

  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)&        aName,
                            const Handle(StepShape_HArray1OfOrientedEdge)& aEdgeList);

  Standard_EXPORT void SetLoop(const Handle(StepShape_Loop)& aLoop);

  Standard_EXPORT Handle(StepShape_Loop) Loop() const;

  Standard_EXPORT void SetPath(const Handle(StepShape_Path)& aPath);

  Standard_EXPORT Handle(StepShape_Path) Path() const;

  Standard_EXPORT void SetEdgeList(const Handle(StepShape_HArray1OfOrientedEdge)& aEdgeList);

  Standard_EXPORT Handle(StepShape_HArray1OfOrientedEdge) EdgeList() const;

  Standard_EXPORT Handle(StepShape_OrientedEdge) EdgeListValue(const Standard_Integer num) const;

  Standard_EXPORT Standard_Integer NbEdgeList() const;

  DEFINE_STANDARD_RTTIEXT(StepShape_LoopAndPath, StepShape_TopologicalRepresentationItem)

protected:
private:
  Handle(StepShape_Loop) loop;
  Handle(StepShape_Path) path;
};

#endif // _StepShape_LoopAndPath_HeaderFile
