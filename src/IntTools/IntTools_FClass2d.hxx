// Created on: 1995-03-22
// Created by: Laurent BUCHARD
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

#ifndef _IntTools_FClass2d_HeaderFile
#define _IntTools_FClass2d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepTopAdaptor_SeqOfPtr.hxx>
#include <TColStd_SequenceOfInteger.hxx>
#include <Standard_Real.hxx>
#include <TopoDS_Face.hxx>
#include <Standard_Boolean.hxx>
#include <TopAbs_State.hxx>
class TopoDS_Face;
class gp_Pnt2d;


//! Class provides an algorithm to classify a 2d Point
//! in 2d space of face using boundaries of the face.
class IntTools_FClass2d 
{
public:

  DEFINE_STANDARD_ALLOC

  

  //! Empty constructor
  Standard_EXPORT IntTools_FClass2d();
  

  //! Initializes algorithm by the face F
  //! and tolerance Tol
  Standard_EXPORT IntTools_FClass2d(const TopoDS_Face& F, const Standard_Real Tol);
  

  //! Initializes algorithm by the face F
  //! and tolerance Tol
  Standard_EXPORT void Init (const TopoDS_Face& F, const Standard_Real Tol);
  

  //! Returns state of infinite 2d point relatively to (0, 0)
  Standard_EXPORT TopAbs_State PerformInfinitePoint() const;
  

  //! Returns state of the 2d point Puv.
  //! If RecadreOnPeriodic is true (defalut value),
  //! for the periodic surface 2d point, adjusted to period, is
  //! classified.
  Standard_EXPORT TopAbs_State Perform (const gp_Pnt2d& Puv, const Standard_Boolean RecadreOnPeriodic = Standard_True) const;
  

  //! Destructor
  Standard_EXPORT void Destroy();
~IntTools_FClass2d()
{
  Destroy();
}
  

  //! Test a point with +- an offset (Tol) and returns
  //! On if some points are OUT an some are IN
  //! (Caution: Internal use . see the code for more details)
  Standard_EXPORT TopAbs_State TestOnRestriction (const gp_Pnt2d& Puv, const Standard_Real Tol, const Standard_Boolean RecadreOnPeriodic = Standard_True) const;
  
  Standard_EXPORT Standard_Boolean IsHole() const;




protected:





private:



  BRepTopAdaptor_SeqOfPtr TabClass;
  TColStd_SequenceOfInteger TabOrien;
  Standard_Real Toluv;
  TopoDS_Face Face;
  Standard_Real U1;
  Standard_Real V1;
  Standard_Real U2;
  Standard_Real V2;
  Standard_Real Umin;
  Standard_Real Umax;
  Standard_Real Vmin;
  Standard_Real Vmax;
  Standard_Boolean myIsHole;


};







#endif // _IntTools_FClass2d_HeaderFile
