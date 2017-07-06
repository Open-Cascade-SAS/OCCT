// Created on: 1991-12-02
// Created by: Laurent PAINNOT
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _AppParCurves_MultiPoint_HeaderFile
#define _AppParCurves_MultiPoint_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <Standard_Real.hxx>
#include <Standard_OStream.hxx>
class Standard_Transient;
class Standard_OutOfRange;
class Standard_DimensionError;
class gp_Pnt;
class gp_Pnt2d;


//! This class describes Points composing a MultiPoint.
//! These points can be 2D or 3D. The user must first give the
//! 3D Points and then the 2D Points.
//! They are Poles of a Bezier Curve.
//! This class is used either to define data input or
//! results when performing the approximation of several lines in parallel.
class AppParCurves_MultiPoint 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! creates an indefinite MultiPoint.
  Standard_EXPORT AppParCurves_MultiPoint();
  
  //! constructs a set of Points used to approximate a
  //! Multiline.
  //! These Points can be of 2 or 3 dimensions.
  //! Points will be initialized with SetPoint and SetPoint2d.
  //! NbPoints is the number of 3D Points.
  //! NbPoints2d is the number of 2D Points.
  Standard_EXPORT AppParCurves_MultiPoint(const Standard_Integer NbPoints, const Standard_Integer NbPoints2d);
  
  //! creates a MultiPoint only composed of 3D points.
  Standard_EXPORT AppParCurves_MultiPoint(const TColgp_Array1OfPnt& tabP);
  
  //! creates a MultiPoint only composed of 2D points.
  Standard_EXPORT AppParCurves_MultiPoint(const TColgp_Array1OfPnt2d& tabP2d);
  
  //! constructs a set of Points used to approximate a
  //! Multiline.
  //! These Points can be of 2 or 3 dimensions.
  //! Points will be initialized with SetPoint and SetPoint2d.
  //! NbPoints is the total number of Points.
  Standard_EXPORT AppParCurves_MultiPoint(const TColgp_Array1OfPnt& tabP, const TColgp_Array1OfPnt2d& tabP2d);
  Standard_EXPORT virtual ~AppParCurves_MultiPoint();
  
  //! the 3d Point of range Index of this MultiPoint is
  //! set to <Point>.
  //! An exception is raised if Index < 0 or
  //! Index > number of 3d Points.
  Standard_EXPORT void SetPoint (const Standard_Integer Index, const gp_Pnt& Point);
  
  //! returns the 3d Point of range Index.
  //! An exception is raised if Index < 0 or
  //! Index < number of 3d Points.
  Standard_EXPORT const gp_Pnt& Point (const Standard_Integer Index) const;
  
  //! The 2d Point of range Index is set to <Point>.
  //! An exception is raised if Index > 3d Points or
  //! Index > total number of Points.
  Standard_EXPORT void SetPoint2d (const Standard_Integer Index, const gp_Pnt2d& Point);
  
  //! returns the 2d Point of range Index.
  //! An exception is raised if index <= number of
  //! 3d Points or Index > total number of Points.
  Standard_EXPORT const gp_Pnt2d& Point2d (const Standard_Integer Index) const;
  
  //! returns the dimension of the point of range Index.
  //! An exception is raised if Index <0 or Index > NbCurves.
    Standard_Integer Dimension (const Standard_Integer Index) const;
  
  //! returns the number of points of dimension 3D.
    Standard_Integer NbPoints() const;
  
  //! returns the number of points of dimension 2D.
    Standard_Integer NbPoints2d() const;
  
  //! Applies a transformation to the curve of range
  //! <CuIndex>.
  //! newx = x + dx*oldx
  //! newy = y + dy*oldy    for all points of the curve.
  //! newz = z + dz*oldz
  Standard_EXPORT void Transform (const Standard_Integer CuIndex, const Standard_Real x, const Standard_Real dx, const Standard_Real y, const Standard_Real dy, const Standard_Real z, const Standard_Real dz);
  
  //! Applies a transformation to the Curve of range
  //! <CuIndex>.
  //! newx = x + dx*oldx
  //! newy = y + dy*oldy    for all points of the curve.
  Standard_EXPORT void Transform2d (const Standard_Integer CuIndex, const Standard_Real x, const Standard_Real dx, const Standard_Real y, const Standard_Real dy);
  
  //! Prints on the stream o information on the current
  //! state of the object.
  //! Is used to redefine the operator <<.
  Standard_EXPORT virtual void Dump (Standard_OStream& o) const;




protected:



  Handle(Standard_Transient) ttabPoint;
  Handle(Standard_Transient) ttabPoint2d;
  Standard_Integer nbP;
  Standard_Integer nbP2d;


private:





};


#include <AppParCurves_MultiPoint.lxx>





#endif // _AppParCurves_MultiPoint_HeaderFile
