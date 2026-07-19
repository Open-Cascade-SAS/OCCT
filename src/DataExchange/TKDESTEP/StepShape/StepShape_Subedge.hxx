// Created on: 2002-01-04
// Created by: data exchange team
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#ifndef _StepShape_Subedge_HeaderFile
#define _StepShape_Subedge_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepShape_Edge.hxx>
class TCollection_HAsciiString;
class StepShape_Vertex;

//! Representation of STEP entity Subedge
class StepShape_Subedge : public StepShape_Edge
{

public:
  //! Empty constructor
  Standard_EXPORT StepShape_Subedge();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(const occ::handle<TCollection_HAsciiString>& aRepresentationItem_Name,
                            const occ::handle<StepShape_Vertex>&         aEdge_EdgeStart,
                            const occ::handle<StepShape_Vertex>&         aEdge_EdgeEnd,
                            const occ::handle<StepShape_Edge>&           aParentEdge);

  //! Returns field ParentEdge
  Standard_EXPORT occ::handle<StepShape_Edge> ParentEdge() const;

  //! Set field ParentEdge
  Standard_EXPORT void SetParentEdge(const occ::handle<StepShape_Edge>& ParentEdge);

  DEFINE_STANDARD_RTTIEXT(StepShape_Subedge, StepShape_Edge)

private:
  occ::handle<StepShape_Edge> theParentEdge;
};

#endif // _StepShape_Subedge_HeaderFile
