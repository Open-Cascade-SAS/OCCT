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

#ifndef _BRep_CurveOn2Surfaces_HeaderFile
#define _BRep_CurveOn2Surfaces_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TopLoc_Location.hxx>
#include <GeomAbs_Shape.hxx>
#include <BRep_CurveRepresentation.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
class Geom_Surface;
class Standard_NullObject;
class TopLoc_Location;
class gp_Pnt;
class BRep_CurveRepresentation;


class BRep_CurveOn2Surfaces;
DEFINE_STANDARD_HANDLE(BRep_CurveOn2Surfaces, BRep_CurveRepresentation)

//! Defines a continuity between two surfaces.
class BRep_CurveOn2Surfaces : public BRep_CurveRepresentation
{

public:

  
  Standard_EXPORT BRep_CurveOn2Surfaces(const Handle(Geom_Surface)& S1, const Handle(Geom_Surface)& S2, const TopLoc_Location& L1, const TopLoc_Location& L2, const GeomAbs_Shape C);
  
  //! Returns True.
  Standard_EXPORT virtual Standard_Boolean IsRegularity() const Standard_OVERRIDE;
  
  //! A curve on two surfaces (continuity).
  Standard_EXPORT virtual Standard_Boolean IsRegularity (const Handle(Geom_Surface)& S1, const Handle(Geom_Surface)& S2, const TopLoc_Location& L1, const TopLoc_Location& L2) const Standard_OVERRIDE;
  
  //! Raises an error.
  Standard_EXPORT void D0 (const Standard_Real U, gp_Pnt& P) const;
  
  Standard_EXPORT virtual const Handle(Geom_Surface)& Surface() const Standard_OVERRIDE;
  
  Standard_EXPORT virtual const Handle(Geom_Surface)& Surface2() const Standard_OVERRIDE;
  
  Standard_EXPORT virtual const TopLoc_Location& Location2() const Standard_OVERRIDE;
  
  Standard_EXPORT virtual const GeomAbs_Shape& Continuity() const Standard_OVERRIDE;
  
  Standard_EXPORT virtual void Continuity (const GeomAbs_Shape C) Standard_OVERRIDE;
  
  //! Return a copy of this representation.
  Standard_EXPORT Handle(BRep_CurveRepresentation) Copy() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(BRep_CurveOn2Surfaces,BRep_CurveRepresentation)

protected:




private:


  Handle(Geom_Surface) mySurface;
  Handle(Geom_Surface) mySurface2;
  TopLoc_Location myLocation2;
  GeomAbs_Shape myContinuity;


};







#endif // _BRep_CurveOn2Surfaces_HeaderFile
