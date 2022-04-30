// Created on: 1998-03-16
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

#ifndef _ShapeUpgrade_SplitSurface_HeaderFile
#define _ShapeUpgrade_SplitSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TColStd_HSequenceOfReal.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Transient.hxx>
#include <ShapeExtend_Status.hxx>
class Geom_Surface;
class ShapeExtend_CompositeSurface;

// resolve name collisions with X11 headers
#ifdef Status
  #undef Status
#endif

class ShapeUpgrade_SplitSurface;
DEFINE_STANDARD_HANDLE(ShapeUpgrade_SplitSurface, Standard_Transient)

//! Splits a Surface with a criterion.
class ShapeUpgrade_SplitSurface : public Standard_Transient
{

public:

  
  //! Empty constructor.
  Standard_EXPORT ShapeUpgrade_SplitSurface();
  
  //! Initializes with single supporting surface.
  Standard_EXPORT void Init (const Handle(Geom_Surface)& S);
  
  //! Initializes with single supporting surface with bounding parameters.
  Standard_EXPORT void Init (const Handle(Geom_Surface)& S,
                             const Standard_Real UFirst, const Standard_Real ULast,
                             const Standard_Real VFirst, const Standard_Real VLast,
                             const Standard_Real theArea = 0.);
  
  //! Sets U parameters where splitting has to be done
  Standard_EXPORT void SetUSplitValues (const Handle(TColStd_HSequenceOfReal)& UValues);
  
  //! Sets V parameters where splitting has to be done
  Standard_EXPORT void SetVSplitValues (const Handle(TColStd_HSequenceOfReal)& VValues);
  
  //! Performs splitting of the supporting surface.
  //! If resulting surface is B-Spline and Segment is True,
  //! the result is composed with segments of the surface bounded
  //! by the U and V SplitValues (method Geom_BSplineSurface::Segment
  //! is used).
  //! If Segment is False, the result is composed with
  //! Geom_RectangularTrimmedSurface all based on the same complete
  //! surface.
  //! Fields myNbResultingRow and myNbResultingCol must be set to
  //! specify the size of resulting grid of surfaces.
  Standard_EXPORT virtual void Build (const Standard_Boolean Segment);
  
  //! Calculates points for correction/splitting of the surface.
  Standard_EXPORT virtual void Compute (const Standard_Boolean Segment = Standard_True);
  
  //! Performs correction/splitting of the surface.
  //! First defines splitting values by method Compute(), then calls method Build().
  Standard_EXPORT void Perform (const Standard_Boolean Segment = Standard_True);
  
  //! returns all the U splitting values including the
  //! First and Last parameters of the input surface
  Standard_EXPORT const Handle(TColStd_HSequenceOfReal)& USplitValues() const;
  
  //! returns all the splitting V values including the
  //! First and Last parameters of the input surface
  Standard_EXPORT const Handle(TColStd_HSequenceOfReal)& VSplitValues() const;
  
  //! Returns the status
  //! OK    - no splitting is needed
  //! DONE1 - splitting required and gives more than one patch
  //! DONE2 - splitting is required, but gives only single patch (initial)
  //! DONE3 - geometric form of the surface or parametrisation is modified
  Standard_EXPORT Standard_Boolean Status (const ShapeExtend_Status status) const;
  
  //! Returns obtained surfaces after splitting as CompositeSurface
  Standard_EXPORT const Handle(ShapeExtend_CompositeSurface)& ResSurfaces() const;




  DEFINE_STANDARD_RTTIEXT(ShapeUpgrade_SplitSurface,Standard_Transient)

protected:


  Handle(TColStd_HSequenceOfReal) myUSplitValues;
  Handle(TColStd_HSequenceOfReal) myVSplitValues;
  Standard_Integer myNbResultingRow;
  Standard_Integer myNbResultingCol;
  Handle(Geom_Surface) mySurface;
  Standard_Integer myStatus;
  Handle(ShapeExtend_CompositeSurface) myResSurfaces;
  Standard_Real myArea;
  Standard_Real myUsize;
  Standard_Real myVsize;


private:




};







#endif // _ShapeUpgrade_SplitSurface_HeaderFile
