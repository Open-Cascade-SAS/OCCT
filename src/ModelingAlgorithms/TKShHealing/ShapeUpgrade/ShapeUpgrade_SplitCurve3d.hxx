// Created on: 1998-03-12
// Created by: Roman LYGIN
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _ShapeUpgrade_SplitCurve3d_HeaderFile
#define _ShapeUpgrade_SplitCurve3d_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Geom_Curve.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <ShapeUpgrade_SplitCurve.hxx>
class Geom_Curve;

//! Splits a 3d curve with a criterion.
class ShapeUpgrade_SplitCurve3d : public ShapeUpgrade_SplitCurve
{

public:
  //! Empty constructor.
  Standard_EXPORT ShapeUpgrade_SplitCurve3d();

  //! Initializes with curve with its first and last parameters.
  Standard_EXPORT void Init(const occ::handle<Geom_Curve>& C);

  //! Initializes with curve with its parameters.
  Standard_EXPORT void Init(const occ::handle<Geom_Curve>& C,
                            const double       First,
                            const double       Last);

  //! If Segment is True, the result is composed with
  //! segments of the curve bounded by the SplitValues. If
  //! Segment is False, the result is composed with trimmed
  //! Curves all based on the same complete curve.
  Standard_EXPORT virtual void Build(const bool Segment) override;

  Standard_EXPORT const occ::handle<NCollection_HArray1<occ::handle<Geom_Curve>>>& GetCurves() const;

  DEFINE_STANDARD_RTTIEXT(ShapeUpgrade_SplitCurve3d, ShapeUpgrade_SplitCurve)

protected:
  occ::handle<Geom_Curve>              myCurve;
  occ::handle<NCollection_HArray1<occ::handle<Geom_Curve>>> myResultingCurves;

};

#endif // _ShapeUpgrade_SplitCurve3d_HeaderFile
