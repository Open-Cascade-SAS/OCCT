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

#ifndef _StepVisual_TessellatedVertex_HeaderFile_
#define _StepVisual_TessellatedVertex_HeaderFile_

#include <Standard.hxx>
#include <Standard_Type.hxx>
#include <StepVisual_TessellatedStructuredItem.hxx>

#include <StepVisual_CoordinatesList.hxx>
#include <StepShape_VertexPoint.hxx>

//! Representation of STEP entity TessellatedVertex
class StepVisual_TessellatedVertex : public StepVisual_TessellatedStructuredItem
{

public:
  //! default constructor
  Standard_EXPORT StepVisual_TessellatedVertex();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(const occ::handle<TCollection_HAsciiString>& theRepresentationItem_Name,
                            const occ::handle<StepVisual_CoordinatesList>& theCoordinates,
                            const bool                                     theHasTopologicalLink,
                            const occ::handle<StepShape_VertexPoint>&      theTopologicalLink,
                            const int                                      thePointIndex);

  //! Returns field Coordinates
  Standard_EXPORT occ::handle<StepVisual_CoordinatesList> Coordinates() const;

  //! Sets field Coordinates
  Standard_EXPORT void SetCoordinates(
    const occ::handle<StepVisual_CoordinatesList>& theCoordinates);

  //! Returns field TopologicalLink
  Standard_EXPORT occ::handle<StepShape_VertexPoint> TopologicalLink() const;

  //! Sets field TopologicalLink
  Standard_EXPORT void SetTopologicalLink(
    const occ::handle<StepShape_VertexPoint>& theTopologicalLink);

  //! Returns True if optional field TopologicalLink is defined
  Standard_EXPORT bool HasTopologicalLink() const;

  //! Returns field PointIndex
  Standard_EXPORT int PointIndex() const;

  //! Sets field PointIndex
  Standard_EXPORT void SetPointIndex(const int thePointIndex);

  DEFINE_STANDARD_RTTIEXT(StepVisual_TessellatedVertex, StepVisual_TessellatedStructuredItem)

private:
  occ::handle<StepVisual_CoordinatesList> myCoordinates;
  occ::handle<StepShape_VertexPoint>      myTopologicalLink; //!< optional
  int                                     myPointIndex;
  bool myHasTopologicalLink; //!< flag "is TopologicalLink defined"
};

#endif // _StepVisual_TessellatedVertex_HeaderFile_
