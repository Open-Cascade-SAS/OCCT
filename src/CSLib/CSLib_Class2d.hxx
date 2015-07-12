// Created on: 1995-03-08
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

#ifndef _CSLib_Class2d_HeaderFile
#define _CSLib_Class2d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Address.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
class gp_Pnt2d;



//! *** Class2d    : Low level algorithm for 2d classification
//! this class was moved from package BRepTopAdaptor
class CSLib_Class2d 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT CSLib_Class2d(const TColgp_Array1OfPnt2d& TP, const Standard_Real aTolu, const Standard_Real aTolv, const Standard_Real umin, const Standard_Real vmin, const Standard_Real umax, const Standard_Real vmax);
  
  Standard_EXPORT Standard_Integer SiDans (const gp_Pnt2d& P) const;
  
  Standard_EXPORT Standard_Integer SiDans_OnMode (const gp_Pnt2d& P, const Standard_Real Tol) const;
  
  Standard_EXPORT Standard_Integer InternalSiDans (const Standard_Real X, const Standard_Real Y) const;
  
  Standard_EXPORT Standard_Integer InternalSiDansOuOn (const Standard_Real X, const Standard_Real Y) const;
  
  Standard_EXPORT const CSLib_Class2d& Copy (const CSLib_Class2d& Other) const;
const CSLib_Class2d& operator= (const CSLib_Class2d& Other) const
{
  return Copy(Other);
}
  
  Standard_EXPORT void Destroy();
~CSLib_Class2d()
{
  Destroy();
}




protected:





private:



  Standard_Address MyPnts2dX;
  Standard_Address MyPnts2dY;
  Standard_Real Tolu;
  Standard_Real Tolv;
  Standard_Integer N;
  Standard_Real Umin;
  Standard_Real Vmin;
  Standard_Real Umax;
  Standard_Real Vmax;


};







#endif // _CSLib_Class2d_HeaderFile
