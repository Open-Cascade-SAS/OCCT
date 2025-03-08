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

#ifndef _StepShape_VertexLoop_HeaderFile
#define _StepShape_VertexLoop_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepShape_Loop.hxx>
class StepShape_Vertex;
class TCollection_HAsciiString;

class StepShape_VertexLoop;
DEFINE_STANDARD_HANDLE(StepShape_VertexLoop, StepShape_Loop)

class StepShape_VertexLoop : public StepShape_Loop
{

public:
  //! Returns a VertexLoop
  Standard_EXPORT StepShape_VertexLoop();

  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& aName,
                            const Handle(StepShape_Vertex)&         aLoopVertex);

  Standard_EXPORT void SetLoopVertex(const Handle(StepShape_Vertex)& aLoopVertex);

  Standard_EXPORT Handle(StepShape_Vertex) LoopVertex() const;

  DEFINE_STANDARD_RTTIEXT(StepShape_VertexLoop, StepShape_Loop)

protected:
private:
  Handle(StepShape_Vertex) loopVertex;
};

#endif // _StepShape_VertexLoop_HeaderFile
