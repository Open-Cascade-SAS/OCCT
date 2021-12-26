// Created on: 1994-03-18
// Created by: Yves FRICAUD
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _Bisector_Curve_HeaderFile
#define _Bisector_Curve_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Geom2d_Curve.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
class gp_Pnt2d;


class Bisector_Curve;
DEFINE_STANDARD_HANDLE(Bisector_Curve, Geom2d_Curve)


class Bisector_Curve : public Geom2d_Curve
{

public:

  
  Standard_EXPORT virtual Standard_Real Parameter (const gp_Pnt2d& P) const = 0;
  
  Standard_EXPORT virtual Standard_Boolean IsExtendAtStart() const = 0;
  
  Standard_EXPORT virtual Standard_Boolean IsExtendAtEnd() const = 0;
  
  //! If necessary,  breaks the  curve in  intervals  of
  //! continuity  <C1>.    And  returns   the number   of
  //! intervals.
  Standard_EXPORT virtual Standard_Integer NbIntervals() const = 0;
  
  //! Returns  the  first  parameter    of  the  current
  //! interval.
  Standard_EXPORT virtual Standard_Real IntervalFirst (const Standard_Integer Index) const = 0;
  
  //! Returns  the  last  parameter    of  the  current
  //! interval.
  Standard_EXPORT virtual Standard_Real IntervalLast (const Standard_Integer Index) const = 0;




  DEFINE_STANDARD_RTTIEXT(Bisector_Curve,Geom2d_Curve)

protected:




private:




};







#endif // _Bisector_Curve_HeaderFile
