// Created on: 1999-04-14
// Created by: Roman LYGIN
// Copyright (c) 1999 Matra Datavision
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

#ifndef _ShapeUpgrade_SplitCurve2dContinuity_HeaderFile
#define _ShapeUpgrade_SplitCurve2dContinuity_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <GeomAbs_Shape.hxx>
#include <Standard_Integer.hxx>
#include <ShapeUpgrade_SplitCurve2d.hxx>

//! Corrects/splits a 2d curve with a continuity criterion.
//! Tolerance is used to correct the curve at a knot that respects
//! geometrically the criterion, in order to reduce the
//! multiplicity of the knot.
class ShapeUpgrade_SplitCurve2dContinuity : public ShapeUpgrade_SplitCurve2d
{

public:
  //! Empty constructor.
  Standard_EXPORT ShapeUpgrade_SplitCurve2dContinuity();

  //! Sets criterion for splitting.
  Standard_EXPORT void SetCriterion(const GeomAbs_Shape Criterion);

  //! Sets tolerance.
  Standard_EXPORT void SetTolerance(const double Tol);

  //! Calculates points for correction/splitting of the curve
  Standard_EXPORT virtual void Compute() override;

  DEFINE_STANDARD_RTTIEXT(ShapeUpgrade_SplitCurve2dContinuity, ShapeUpgrade_SplitCurve2d)

private:
  GeomAbs_Shape    myCriterion;
  int myCont;
  double    myTolerance;
};

#endif // _ShapeUpgrade_SplitCurve2dContinuity_HeaderFile
