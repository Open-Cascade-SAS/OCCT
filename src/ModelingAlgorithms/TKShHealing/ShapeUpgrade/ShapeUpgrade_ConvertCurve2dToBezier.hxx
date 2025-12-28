// Created on: 1999-05-13
// Created by: data exchange team
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

#ifndef _ShapeUpgrade_ConvertCurve2dToBezier_HeaderFile
#define _ShapeUpgrade_ConvertCurve2dToBezier_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Geom2d_Curve.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <ShapeUpgrade_SplitCurve2d.hxx>

//! converts/splits a 2d curve to a list of beziers
class ShapeUpgrade_ConvertCurve2dToBezier : public ShapeUpgrade_SplitCurve2d
{

public:
  //! Empty constructor.
  Standard_EXPORT ShapeUpgrade_ConvertCurve2dToBezier();

  //! Converts curve into a list of beziers, and stores the
  //! splitting parameters on original curve.
  Standard_EXPORT virtual void Compute() override;

  //! Splits a list of beziers computed by Compute method according
  //! the split values and splitting parameters.
  Standard_EXPORT virtual void Build(const bool Segment) override;

  //! Returns the list of split parameters in original curve parametrisation.
  Standard_EXPORT occ::handle<NCollection_HSequence<double>> SplitParams() const;

  DEFINE_STANDARD_RTTIEXT(ShapeUpgrade_ConvertCurve2dToBezier, ShapeUpgrade_SplitCurve2d)

private:
  //! Returns the list of bezier curves correspondent to original
  //! curve.
  Standard_EXPORT occ::handle<NCollection_HSequence<occ::handle<Geom2d_Curve>>> Segments() const;

  occ::handle<NCollection_HSequence<occ::handle<Geom2d_Curve>>> mySegments;
  occ::handle<NCollection_HSequence<double>>     mySplitParams;
};

#endif // _ShapeUpgrade_ConvertCurve2dToBezier_HeaderFile
