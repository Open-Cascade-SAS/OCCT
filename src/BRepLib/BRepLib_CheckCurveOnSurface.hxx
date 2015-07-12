// Created by: Eugeny MALTCHIKOV
// Copyright (c) 2014 OPEN CASCADE SAS
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

#ifndef _BRepLib_CheckCurveOnSurface_HeaderFile
#define _BRepLib_CheckCurveOnSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class Geom_Curve;
class Geom2d_Curve;
class Geom_Surface;
class TopoDS_Edge;
class TopoDS_Face;



//! Computes the max distance between edge and its
//! 2d representation on the face.
//!
//! The algorithm can be initialized in the following ways:
//! 1. Input args are Edge and Face;
//! 2. Input args are 3D curve, 2d curve, Surface and
//! parametric range of the curve (first and last values).
class BRepLib_CheckCurveOnSurface 
{
public:

  DEFINE_STANDARD_ALLOC

  

  //! Empty contructor
  Standard_EXPORT BRepLib_CheckCurveOnSurface();
  

  //! Contructor
  Standard_EXPORT BRepLib_CheckCurveOnSurface(const TopoDS_Edge& theEdge, const TopoDS_Face& theFace);
  

  //! Contructor
  Standard_EXPORT BRepLib_CheckCurveOnSurface(const Handle(Geom_Curve)& theCurve, const Handle(Geom2d_Curve)& thePCurve, const Handle(Geom_Surface)& theSurface, const Standard_Real theFirst, const Standard_Real theLast);
  

  //! Sets the data for the algorithm
  Standard_EXPORT void Init (const TopoDS_Edge& theEdge, const TopoDS_Face& theFace);
  

  //! Sets the data for the algorithm
  Standard_EXPORT void Init (const Handle(Geom_Curve)& theCurve, const Handle(Geom2d_Curve)& thePCurve, const Handle(Geom_Surface)& theSurface, const Standard_Real theFirst, const Standard_Real theLast);
  

  //! Returns my3DCurve
    const Handle(Geom_Curve)& Curve() const;
  

  //! Returns my2DCurve
    const Handle(Geom2d_Curve)& PCurve() const;
  

  //! Returns my2DCurve
    const Handle(Geom2d_Curve)& PCurve2() const;
  

  //! Returns mySurface
    const Handle(Geom_Surface)& Surface() const;
  

  //! Returns the range
    void Range (Standard_Real& theFirst, Standard_Real& theLast);
  

  //! Performs the calculation
  //! If isTheMultyTheadDisabled == TRUE then computation will be made
  //! without any parallelization.
  Standard_EXPORT void Perform (const Standard_Boolean isTheMultyTheradDisabled = Standard_False);
  

  //! Returns true if the max distance has been found
    Standard_Boolean IsDone() const;
  

  //! Returns error status
  //! The possible values are:
  //! 0 - OK;
  //! 1 - null curve or surface or 2d curve;
  //! 2 - invalid parametric range;
  //! 3 - error in calculations.
    Standard_Integer ErrorStatus() const;
  

  //! Returns max distance
    Standard_Real MaxDistance() const;
  

  //! Returns parameter in which the distance is maximal
    Standard_Real MaxParameter() const;




protected:

  

  //! Checks the data
  Standard_EXPORT void CheckData();
  

  //! Computes the max distance for the 3d curve <myCurve>
  //! and 2d curve <thePCurve>
  //! If isTheMultyTheadDisabled == TRUE then computation will be made
  //! without any parallelization.
  Standard_EXPORT void Compute (const Handle(Geom2d_Curve)& thePCurve, const Standard_Boolean isTheMultyTheradDisabled);




private:



  Handle(Geom_Curve) myCurve;
  Handle(Geom2d_Curve) myPCurve;
  Handle(Geom2d_Curve) myPCurve2;
  Handle(Geom_Surface) mySurface;
  Standard_Real myFirst;
  Standard_Real myLast;
  Standard_Integer myErrorStatus;
  Standard_Real myMaxDistance;
  Standard_Real myMaxParameter;


};


#include <BRepLib_CheckCurveOnSurface.lxx>





#endif // _BRepLib_CheckCurveOnSurface_HeaderFile
