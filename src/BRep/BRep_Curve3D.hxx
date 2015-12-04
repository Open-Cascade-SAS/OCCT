// Created on: 1993-07-06
// Created by: Remi LEQUETTE
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

#ifndef _BRep_Curve3D_HeaderFile
#define _BRep_Curve3D_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <BRep_GCurve.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
class Geom_Curve;
class TopLoc_Location;
class gp_Pnt;
class BRep_CurveRepresentation;


class BRep_Curve3D;
DEFINE_STANDARD_HANDLE(BRep_Curve3D, BRep_GCurve)

//! Representation of a curve by a 3D curve.
class BRep_Curve3D : public BRep_GCurve
{

public:

  
  Standard_EXPORT BRep_Curve3D(const Handle(Geom_Curve)& C, const TopLoc_Location& L);
  
  //! Computes the point at parameter U.
  Standard_EXPORT void D0 (const Standard_Real U, gp_Pnt& P) const Standard_OVERRIDE;
  
  //! Returns True.
  Standard_EXPORT virtual Standard_Boolean IsCurve3D() const Standard_OVERRIDE;
  
  Standard_EXPORT virtual const Handle(Geom_Curve)& Curve3D() const Standard_OVERRIDE;
  
  Standard_EXPORT virtual void Curve3D (const Handle(Geom_Curve)& C) Standard_OVERRIDE;
  
  //! Return a copy of this representation.
  Standard_EXPORT Handle(BRep_CurveRepresentation) Copy() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(BRep_Curve3D,BRep_GCurve)

protected:




private:


  Handle(Geom_Curve) myCurve;


};







#endif // _BRep_Curve3D_HeaderFile
