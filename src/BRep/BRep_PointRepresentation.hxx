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

#ifndef _BRep_PointRepresentation_HeaderFile
#define _BRep_PointRepresentation_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TopLoc_Location.hxx>
#include <Standard_Real.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Boolean.hxx>
class Geom_Curve;
class Geom2d_Curve;
class Geom_Surface;


class BRep_PointRepresentation;
DEFINE_STANDARD_HANDLE(BRep_PointRepresentation, Standard_Transient)

//! Root  class     for   the points  representations.
//! Contains a location and a parameter.
class BRep_PointRepresentation : public Standard_Transient
{

public:

  
  //! A point on a 3d curve.
  Standard_EXPORT virtual Standard_Boolean IsPointOnCurve() const;
  
  //! A point on a 2d curve on a surface.
  Standard_EXPORT virtual Standard_Boolean IsPointOnCurveOnSurface() const;
  
  //! A point on a surface.
  Standard_EXPORT virtual Standard_Boolean IsPointOnSurface() const;
  
  //! A point on the curve <C>.
  Standard_EXPORT virtual Standard_Boolean IsPointOnCurve (const Handle(Geom_Curve)& C, const TopLoc_Location& L) const;
  
  //! A point on the 2d curve <PC> on the surface <S>.
  Standard_EXPORT virtual Standard_Boolean IsPointOnCurveOnSurface (const Handle(Geom2d_Curve)& PC, const Handle(Geom_Surface)& S, const TopLoc_Location& L) const;
  
  //! A point on the surface <S>.
  Standard_EXPORT virtual Standard_Boolean IsPointOnSurface (const Handle(Geom_Surface)& S, const TopLoc_Location& L) const;
  
    const TopLoc_Location& Location() const;
  
    void Location (const TopLoc_Location& L);
  
    Standard_Real Parameter() const;
  
    void Parameter (const Standard_Real P);
  
  Standard_EXPORT virtual Standard_Real Parameter2() const;
  
  Standard_EXPORT virtual void Parameter2 (const Standard_Real P);
  
  Standard_EXPORT virtual const Handle(Geom_Curve)& Curve() const;
  
  Standard_EXPORT virtual void Curve (const Handle(Geom_Curve)& C);
  
  Standard_EXPORT virtual const Handle(Geom2d_Curve)& PCurve() const;
  
  Standard_EXPORT virtual void PCurve (const Handle(Geom2d_Curve)& C);
  
  Standard_EXPORT virtual const Handle(Geom_Surface)& Surface() const;
  
  Standard_EXPORT virtual void Surface (const Handle(Geom_Surface)& S);

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;




  DEFINE_STANDARD_RTTIEXT(BRep_PointRepresentation,Standard_Transient)

protected:

  
  Standard_EXPORT BRep_PointRepresentation(const Standard_Real P, const TopLoc_Location& L);



private:


  TopLoc_Location myLocation;
  Standard_Real myParameter;


};


#include <BRep_PointRepresentation.lxx>





#endif // _BRep_PointRepresentation_HeaderFile
