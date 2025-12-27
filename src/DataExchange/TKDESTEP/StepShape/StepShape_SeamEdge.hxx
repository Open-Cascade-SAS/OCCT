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

#ifndef _StepShape_SeamEdge_HeaderFile
#define _StepShape_SeamEdge_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepShape_OrientedEdge.hxx>
class StepGeom_Pcurve;
class TCollection_HAsciiString;
class StepShape_Edge;

//! Representation of STEP entity SeamEdge
class StepShape_SeamEdge : public StepShape_OrientedEdge
{

public:
  //! Empty constructor
  Standard_EXPORT StepShape_SeamEdge();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(const occ::handle<TCollection_HAsciiString>& aRepresentationItem_Name,
                            const occ::handle<StepShape_Edge>&           aOrientedEdge_EdgeElement,
                            const bool                  aOrientedEdge_Orientation,
                            const occ::handle<StepGeom_Pcurve>&          aPcurveReference);

  //! Returns field PcurveReference
  Standard_EXPORT occ::handle<StepGeom_Pcurve> PcurveReference() const;

  //! Set field PcurveReference
  Standard_EXPORT void SetPcurveReference(const occ::handle<StepGeom_Pcurve>& PcurveReference);

  DEFINE_STANDARD_RTTIEXT(StepShape_SeamEdge, StepShape_OrientedEdge)

private:
  occ::handle<StepGeom_Pcurve> thePcurveReference;
};

#endif // _StepShape_SeamEdge_HeaderFile
