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

#ifndef _StepShape_PolyLoop_HeaderFile
#define _StepShape_PolyLoop_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepGeom_CartesianPoint.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepShape_Loop.hxx>
#include <Standard_Integer.hxx>
class TCollection_HAsciiString;
class StepGeom_CartesianPoint;

class StepShape_PolyLoop : public StepShape_Loop
{

public:
  //! Returns a PolyLoop
  Standard_EXPORT StepShape_PolyLoop();

  Standard_EXPORT void Init(const occ::handle<TCollection_HAsciiString>&         aName,
                            const occ::handle<NCollection_HArray1<occ::handle<StepGeom_CartesianPoint>>>& aPolygon);

  Standard_EXPORT void SetPolygon(const occ::handle<NCollection_HArray1<occ::handle<StepGeom_CartesianPoint>>>& aPolygon);

  Standard_EXPORT occ::handle<NCollection_HArray1<occ::handle<StepGeom_CartesianPoint>>> Polygon() const;

  Standard_EXPORT occ::handle<StepGeom_CartesianPoint> PolygonValue(const int num) const;

  Standard_EXPORT int NbPolygon() const;

  DEFINE_STANDARD_RTTIEXT(StepShape_PolyLoop, StepShape_Loop)

private:
  occ::handle<NCollection_HArray1<occ::handle<StepGeom_CartesianPoint>>> polygon;
};

#endif // _StepShape_PolyLoop_HeaderFile
