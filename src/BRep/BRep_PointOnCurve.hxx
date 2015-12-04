// Created on: 1993-08-10
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

#ifndef _BRep_PointOnCurve_HeaderFile
#define _BRep_PointOnCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <BRep_PointRepresentation.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
class Geom_Curve;
class TopLoc_Location;


class BRep_PointOnCurve;
DEFINE_STANDARD_HANDLE(BRep_PointOnCurve, BRep_PointRepresentation)

//! Representation by a parameter on a 3D curve.
class BRep_PointOnCurve : public BRep_PointRepresentation
{

public:

  
  Standard_EXPORT BRep_PointOnCurve(const Standard_Real P, const Handle(Geom_Curve)& C, const TopLoc_Location& L);
  
  //! Returns True
  Standard_EXPORT virtual Standard_Boolean IsPointOnCurve() const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_Boolean IsPointOnCurve (const Handle(Geom_Curve)& C, const TopLoc_Location& L) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual const Handle(Geom_Curve)& Curve() const Standard_OVERRIDE;
  
  Standard_EXPORT virtual void Curve (const Handle(Geom_Curve)& C) Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(BRep_PointOnCurve,BRep_PointRepresentation)

protected:




private:


  Handle(Geom_Curve) myCurve;


};







#endif // _BRep_PointOnCurve_HeaderFile
