// Created on: 1993-06-16
// Created by: Isabelle GRIGNON
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _GProp_PEquation_HeaderFile
#define _GProp_PEquation_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GProp_EquaType.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
class gp_Pln;
class gp_Lin;


//! A framework to analyze a collection - or cloud
//! - of points and to verify if they are coincident,
//! collinear or coplanar within a given precision. If
//! so, it also computes the mean point, the mean
//! line or the mean plane of the points. If not, it
//! computes the minimal box which includes all the points.
class GProp_PEquation 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Constructs a framework to analyze the
  //! collection of points Pnts and computes:
  //! -   the mean point if the points in question are
  //! considered to be coincident within the precision Tol, or
  //! -   the mean line if they are considered to be
  //! collinear within the precision Tol, or
  //! -   the mean plane if they are considered to be
  //! coplanar within the precision Tol, or
  //! -   the minimal box which contains all the points. Use :
  //! -   the functions IsPoint, IsLinear, IsPlanar
  //! and IsSpace to find the result of the analysis, and
  //! -   the function Point, Line, Plane or Box to
  //! access the computed result.
  Standard_EXPORT GProp_PEquation(const TColgp_Array1OfPnt& Pnts, const Standard_Real Tol);
  
  //! Returns true if, according to the given
  //! tolerance, the points analyzed by this framework are  coplanar.
  //! Use the function  Plane  to access the computed result.
  Standard_EXPORT Standard_Boolean IsPlanar() const;
  
  //! Returns true if, according to the given
  //! tolerance, the points analyzed by this framework are  colinear.
  //! Use the function  Line  to access the computed result.
  Standard_EXPORT Standard_Boolean IsLinear() const;
  
  //! Returns true if, according to the given
  //! tolerance, the points analyzed by this framework are  coincident.
  //! Use the function  Point  to access the computed result.
  Standard_EXPORT Standard_Boolean IsPoint() const;
  
  //! Returns true if, according to the given
  //! tolerance value, the points analyzed by this
  //! framework are neither coincident, nor collinear, nor coplanar.
  //! Use the function Box to query the smallest box
  //! that includes the collection of points.
  Standard_EXPORT Standard_Boolean IsSpace() const;
  
  //! Returns the mean plane passing near all the
  //! points analyzed by this framework if, according
  //! to the given precision, the points are considered to be coplanar.
  //! Exceptions
  //! Standard_NoSuchObject if, according to the
  //! given precision value, the points analyzed by
  //! this framework are considered to be:
  //! -   coincident, or
  //! -   collinear, or
  //! -   not coplanar.
  Standard_EXPORT gp_Pln Plane() const;
  
  //! Returns the mean line passing near all the
  //! points analyzed by this framework if, according
  //! to the given precision value, the points are considered to be collinear.
  //! Exceptions
  //! Standard_NoSuchObject if, according to the
  //! given precision, the points analyzed by this
  //! framework are considered to be:
  //! -   coincident, or
  //! -   not collinear.
  Standard_EXPORT gp_Lin Line() const;
  
  //! Returns the mean point of all the points
  //! analyzed by this framework if, according to the
  //! given precision, the points are considered to be coincident.
  //! Exceptions
  //! Standard_NoSuchObject if, according to the
  //! given precision, the points analyzed by this
  //! framework are not considered to be coincident.
  Standard_EXPORT gp_Pnt Point() const;
  
  //! Returns the definition of the smallest box which
  //! contains all the points analyzed by this
  //! framework if, according to the given precision
  //! value, the points are considered to be neither
  //! coincident, nor collinear and nor coplanar.
  //! This box is centered on the barycenter P of the
  //! collection of points. Its sides are parallel to the
  //! three vectors V1, V2 and V3, the length of
  //! which is the length of the box in the corresponding direction.
  //! Note: Vectors V1, V2 and V3 are parallel to
  //! the three axes of principal inertia of the system
  //! composed of the collection of points where each point is of equal mass.
  //! Exceptions
  //! Standard_NoSuchObject if, according to the given precision,
  //! the points analyzed by this framework are considered to be coincident, collinear or coplanar.
  Standard_EXPORT void Box (gp_Pnt& P, gp_Vec& V1, gp_Vec& V2, gp_Vec& V3) const;




protected:





private:



  GProp_EquaType type;
  gp_Pnt g;
  gp_Vec v1;
  gp_Vec v2;
  gp_Vec v3;


};







#endif // _GProp_PEquation_HeaderFile
