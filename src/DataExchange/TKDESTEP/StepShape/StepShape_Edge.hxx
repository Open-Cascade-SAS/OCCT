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

#ifndef _StepShape_Edge_HeaderFile
#define _StepShape_Edge_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepShape_Vertex.hxx>

class TCollection_HAsciiString;

class StepShape_Edge;
DEFINE_STANDARD_HANDLE(StepShape_Edge, StepShape_TopologicalRepresentationItem)

class StepShape_Edge : public StepShape_TopologicalRepresentationItem
{

public:
  //! Returns a Edge
  Standard_EXPORT StepShape_Edge();

  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& aName,
                            const Handle(StepShape_Vertex)&         aEdgeStart,
                            const Handle(StepShape_Vertex)&         aEdgeEnd);

  Standard_EXPORT virtual void SetEdgeStart(const Handle(StepShape_Vertex)& aEdgeStart);

  Standard_EXPORT virtual Handle(StepShape_Vertex) EdgeStart() const;

  Standard_EXPORT virtual void SetEdgeEnd(const Handle(StepShape_Vertex)& aEdgeEnd);

  Standard_EXPORT virtual Handle(StepShape_Vertex) EdgeEnd() const;

  DEFINE_STANDARD_RTTIEXT(StepShape_Edge, StepShape_TopologicalRepresentationItem)

protected:
private:
  Handle(StepShape_Vertex) edgeStart;
  Handle(StepShape_Vertex) edgeEnd;
};

#endif // _StepShape_Edge_HeaderFile
