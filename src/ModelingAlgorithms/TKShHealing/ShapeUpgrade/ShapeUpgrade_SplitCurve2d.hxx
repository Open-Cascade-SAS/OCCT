// Created on: 1998-03-12
// Created by: Pierre BARRAS
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

#ifndef _ShapeUpgrade_SplitCurve2d_HeaderFile
#define _ShapeUpgrade_SplitCurve2d_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Geom2d_Curve.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <ShapeUpgrade_SplitCurve.hxx>
class Geom2d_Curve;

//! Splits a 2d curve with a criterion.
class ShapeUpgrade_SplitCurve2d : public ShapeUpgrade_SplitCurve
{

public:
  //! Empty constructor.
  Standard_EXPORT ShapeUpgrade_SplitCurve2d();

  //! Initializes with pcurve with its first and last parameters.
  Standard_EXPORT void Init(const occ::handle<Geom2d_Curve>& C);

  //! Initializes with pcurve with its parameters.
  Standard_EXPORT void Init(const occ::handle<Geom2d_Curve>& C,
                            const double                     First,
                            const double                     Last);

  //! If Segment is True, the result is composed with
  //! segments of the curve bounded by the SplitValues. If
  //! Segment is False, the result is composed with trimmed
  //! Curves all based on the same complete curve.
  Standard_EXPORT virtual void Build(const bool Segment) override;

  Standard_EXPORT const occ::handle<NCollection_HArray1<occ::handle<Geom2d_Curve>>>& GetCurves()
    const;

  DEFINE_STANDARD_RTTIEXT(ShapeUpgrade_SplitCurve2d, ShapeUpgrade_SplitCurve)

protected:
  occ::handle<Geom2d_Curve>                                   myCurve;
  occ::handle<NCollection_HArray1<occ::handle<Geom2d_Curve>>> myResultingCurves;
};

#endif // _ShapeUpgrade_SplitCurve2d_HeaderFile
