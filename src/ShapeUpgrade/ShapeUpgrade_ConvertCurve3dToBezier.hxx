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

#ifndef _ShapeUpgrade_ConvertCurve3dToBezier_HeaderFile
#define _ShapeUpgrade_ConvertCurve3dToBezier_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TColGeom_HSequenceOfCurve.hxx>
#include <ShapeUpgrade_SplitCurve3d.hxx>


class ShapeUpgrade_ConvertCurve3dToBezier;
DEFINE_STANDARD_HANDLE(ShapeUpgrade_ConvertCurve3dToBezier, ShapeUpgrade_SplitCurve3d)

//! converts/splits a 3d curve of any type to a list of beziers
class ShapeUpgrade_ConvertCurve3dToBezier : public ShapeUpgrade_SplitCurve3d
{

public:

  //! Empty constructor
  Standard_EXPORT ShapeUpgrade_ConvertCurve3dToBezier();
  
  //! Sets mode for conversion Geom_Line to bezier.
    void SetLineMode (const Standard_Boolean mode);
  
  //! Returns the Geom_Line conversion mode.
    Standard_Boolean GetLineMode() const;
  
  //! Sets mode for conversion Geom_Circle to bezier.
    void SetCircleMode (const Standard_Boolean mode);
  
  //! Returns the Geom_Circle conversion mode.
    Standard_Boolean GetCircleMode() const;
  
  //! Returns the Geom_Conic conversion mode.
    void SetConicMode (const Standard_Boolean mode);
  
  //! Performs converting and computes the resulting shape.
    Standard_Boolean GetConicMode() const;
  
  //! Converts curve into a list of beziers, and stores the
  //! splitting parameters on original curve.
  Standard_EXPORT virtual void Compute() Standard_OVERRIDE;
  
  //! Splits a list of beziers computed by Compute method according
  //! the split values and splitting parameters.
  Standard_EXPORT virtual void Build (const Standard_Boolean Segment) Standard_OVERRIDE;
  
  //! Returns the list of split parameters in original curve parametrisation.
  Standard_EXPORT Handle(TColStd_HSequenceOfReal) SplitParams() const;

  DEFINE_STANDARD_RTTIEXT(ShapeUpgrade_ConvertCurve3dToBezier,ShapeUpgrade_SplitCurve3d)

private:

  //! Returns the list of bezier curves correspondent to original
  //! curve.
  Standard_EXPORT Handle(TColGeom_HSequenceOfCurve) Segments() const;

  Handle(TColGeom_HSequenceOfCurve) mySegments;
  Handle(TColStd_HSequenceOfReal) mySplitParams;
  Standard_Boolean myLineMode;
  Standard_Boolean myCircleMode;
  Standard_Boolean myConicMode;

};

#include <ShapeUpgrade_ConvertCurve3dToBezier.lxx>

#endif // _ShapeUpgrade_ConvertCurve3dToBezier_HeaderFile
