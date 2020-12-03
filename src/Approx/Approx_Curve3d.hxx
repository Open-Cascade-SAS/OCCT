// Created on: 1998-08-20
// Created by: Philippe MANGIN
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

#ifndef _Approx_Curve3d_HeaderFile
#define _Approx_Curve3d_HeaderFile

#include <Adaptor3d_Curve.hxx>
#include <GeomAbs_Shape.hxx>
#include <Geom_BSplineCurve.hxx>

class Approx_Curve3d 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Approximation  of  a  curve  with respect of the
  //! required tolerance Tol3D.
  Standard_EXPORT Approx_Curve3d(const Handle(Adaptor3d_Curve)& Curve, const Standard_Real Tol3d, const GeomAbs_Shape Order, const Standard_Integer MaxSegments, const Standard_Integer MaxDegree);
  
  Standard_EXPORT Handle(Geom_BSplineCurve) Curve() const;
  
  //! returns  Standard_True  if  the  approximation  has
  //! been  done  within  required tolerance
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  //! returns  Standard_True if the approximation did come out
  //! with a result that  is not NECESSARELY within the required
  //! tolerance
  Standard_EXPORT Standard_Boolean HasResult() const;
  
  //! returns  the  Maximum  Error  (>0 when an approximation
  //! has  been  done, 0  if  no  approximation)
  Standard_EXPORT Standard_Real MaxError() const;
  
  //! Print on the stream  o  information about the object
  Standard_EXPORT void Dump (Standard_OStream& o) const;

private:

  Standard_Boolean myIsDone;
  Standard_Boolean myHasResult;
  Handle(Geom_BSplineCurve) myBSplCurve;
  Standard_Real myMaxError;

};

#endif // _Approx_Curve3d_HeaderFile
