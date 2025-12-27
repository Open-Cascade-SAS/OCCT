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

#ifndef _StepShape_OrientedEdge_HeaderFile
#define _StepShape_OrientedEdge_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Boolean.hxx>
#include <StepShape_Edge.hxx>
class TCollection_HAsciiString;
class StepShape_Vertex;

class StepShape_OrientedEdge : public StepShape_Edge
{

public:
  //! Returns a OrientedEdge
  Standard_EXPORT StepShape_OrientedEdge();

  Standard_EXPORT void Init(const occ::handle<TCollection_HAsciiString>& aName,
                            const occ::handle<StepShape_Edge>&           aEdgeElement,
                            const bool                  aOrientation);

  Standard_EXPORT void SetEdgeElement(const occ::handle<StepShape_Edge>& aEdgeElement);

  Standard_EXPORT occ::handle<StepShape_Edge> EdgeElement() const;

  Standard_EXPORT void SetOrientation(const bool aOrientation);

  Standard_EXPORT bool Orientation() const;

  Standard_EXPORT virtual void SetEdgeStart(const occ::handle<StepShape_Vertex>& aEdgeStart)
    override;

  Standard_EXPORT virtual occ::handle<StepShape_Vertex> EdgeStart() const override;

  Standard_EXPORT virtual void SetEdgeEnd(const occ::handle<StepShape_Vertex>& aEdgeEnd)
    override;

  Standard_EXPORT virtual occ::handle<StepShape_Vertex> EdgeEnd() const override;

  DEFINE_STANDARD_RTTIEXT(StepShape_OrientedEdge, StepShape_Edge)

private:
  occ::handle<StepShape_Edge> edgeElement;
  bool       orientation;
};

#endif // _StepShape_OrientedEdge_HeaderFile
