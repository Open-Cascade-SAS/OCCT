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

#ifndef _BRepTopAdaptor_FClass2d_HeaderFile
#define _BRepTopAdaptor_FClass2d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepTopAdaptor_SeqOfPtr.hxx>
#include <TColStd_SequenceOfInteger.hxx>
#include <Standard_Real.hxx>
#include <TopoDS_Face.hxx>
#include <TopAbs_State.hxx>
#include <Standard_Boolean.hxx>
class TopoDS_Face;
class gp_Pnt2d;



class BRepTopAdaptor_FClass2d 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT BRepTopAdaptor_FClass2d(const TopoDS_Face& F, const Standard_Real Tol);
  
  Standard_EXPORT TopAbs_State PerformInfinitePoint() const;
  
  Standard_EXPORT TopAbs_State Perform (const gp_Pnt2d& Puv, const Standard_Boolean RecadreOnPeriodic = Standard_True) const;
  
  Standard_EXPORT void Destroy();
~BRepTopAdaptor_FClass2d()
{
  Destroy();
}
  
  Standard_EXPORT const BRepTopAdaptor_FClass2d& Copy (const BRepTopAdaptor_FClass2d& Other) const;
const BRepTopAdaptor_FClass2d& operator= (const BRepTopAdaptor_FClass2d& Other) const
{
  return Copy(Other);
}
  
  //! Test a point with +- an offset (Tol) and returns
  //! On if some points are OUT an some are IN
  //! (Caution: Internal use . see the code for more details)
  Standard_EXPORT TopAbs_State TestOnRestriction (const gp_Pnt2d& Puv, const Standard_Real Tol, const Standard_Boolean RecadreOnPeriodic = Standard_True) const;




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


};







#endif // _BRepTopAdaptor_FClass2d_HeaderFile
