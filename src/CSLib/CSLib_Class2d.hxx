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
#include <NCollection_Handle.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColgp_SequenceOfPnt2d.hxx>

class gp_Pnt2d;



//! *** Class2d    : Low level algorithm for 2d classification
//! this class was moved from package BRepTopAdaptor
class CSLib_Class2d 
{
public:

  DEFINE_STANDARD_ALLOC

  
    //! Constructs the 2D-polygon.
    //! thePnts2d is the set of the vertices (closed polygon
    //! will always be created inside of this constructor;
    //! consequently, there is no point in repeating first and
    //! last point in thePnts2d).
    //! theTolu and theTolv are tolerances.
    //! theUmin, theVmin, theUmax, theVmax are
    //! UV-bounds of the polygon.
    Standard_EXPORT CSLib_Class2d(const TColgp_Array1OfPnt2d& thePnts2d,
                                  const Standard_Real theTolU,
                                  const Standard_Real theTolV,
                                  const Standard_Real theUMin,
                                  const Standard_Real theVMin,
                                  const Standard_Real theUMax,
                                  const Standard_Real theVMax);

  //! Constructs the 2D-polygon.
  //! thePnts2d is the set of the vertices (closed polygon
  //! will always be created inside of this constructor;
  //! consequently, there is no point in repeating first and
  //! last point in thePnts2d).
  //! theTolu and theTolv are tolerances.
  //! theUmin, theVmin, theUmax, theVmax are
  //! UV-bounds of the polygon.
  Standard_EXPORT CSLib_Class2d(const TColgp_SequenceOfPnt2d& thePnts2d,
                                const Standard_Real theTolU,
                                const Standard_Real theTolV,
                                const Standard_Real theUMin,
                                const Standard_Real theVMin,
                                const Standard_Real theUMax,
                                const Standard_Real theVMax);

  Standard_EXPORT Standard_Integer SiDans (const gp_Pnt2d& P) const;
  
  Standard_EXPORT Standard_Integer SiDans_OnMode (const gp_Pnt2d& P, const Standard_Real Tol) const;
  
  Standard_EXPORT Standard_Integer InternalSiDans (const Standard_Real X, const Standard_Real Y) const;
  
  Standard_EXPORT Standard_Integer InternalSiDansOuOn (const Standard_Real X, const Standard_Real Y) const;
  
protected:


private:

  //! Initializes theObj
  template <class TCol_Containers2d>
  void Init(const TCol_Containers2d& TP2d,
                          const Standard_Real aTolu,
                          const Standard_Real aTolv,
                          const Standard_Real umin,
                          const Standard_Real vmin,
                          const Standard_Real umax,
                          const Standard_Real vmax);

  //! Assign operator is forbidden
  const CSLib_Class2d& operator= (const CSLib_Class2d& Other) const;

  NCollection_Handle <TColStd_Array1OfReal> MyPnts2dX, MyPnts2dY;
  Standard_Real Tolu;
  Standard_Real Tolv;
  Standard_Integer N;
  Standard_Real Umin;
  Standard_Real Vmin;
  Standard_Real Umax;
  Standard_Real Vmax;


};







#endif // _CSLib_Class2d_HeaderFile
