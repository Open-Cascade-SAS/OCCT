// Created on: 1995-03-14
// Created by: Laurent PAINNOT
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

#ifndef _BRep_PolygonOnSurface_HeaderFile
#define _BRep_PolygonOnSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <BRep_CurveRepresentation.hxx>
class Poly_Polygon2D;
class Geom_Surface;
class TopLoc_Location;

class BRep_PolygonOnSurface;
DEFINE_STANDARD_HANDLE(BRep_PolygonOnSurface, BRep_CurveRepresentation)

//! Representation of a 2D polygon in the parametric
//! space of a surface.
class BRep_PolygonOnSurface : public BRep_CurveRepresentation
{

public:
  Standard_EXPORT BRep_PolygonOnSurface(const Handle(Poly_Polygon2D)& P,
                                        const Handle(Geom_Surface)&   S,
                                        const TopLoc_Location&        L);

  //! A   2D polygon  representation  in the  parametric
  //! space of a surface.
  Standard_EXPORT virtual Standard_Boolean IsPolygonOnSurface() const Standard_OVERRIDE;

  //! A   2D polygon  representation  in the  parametric
  //! space of a surface.
  Standard_EXPORT virtual Standard_Boolean IsPolygonOnSurface(const Handle(Geom_Surface)& S,
                                                              const TopLoc_Location&      L) const
    Standard_OVERRIDE;

  Standard_EXPORT virtual const Handle(Geom_Surface)& Surface() const Standard_OVERRIDE;

  Standard_EXPORT virtual const Handle(Poly_Polygon2D)& Polygon() const Standard_OVERRIDE;

  Standard_EXPORT virtual void Polygon(const Handle(Poly_Polygon2D)& P) Standard_OVERRIDE;

  //! Return a copy of this representation.
  Standard_EXPORT virtual Handle(BRep_CurveRepresentation) Copy() const Standard_OVERRIDE;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream,
                                        Standard_Integer  theDepth = -1) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(BRep_PolygonOnSurface, BRep_CurveRepresentation)

protected:
private:
  Handle(Poly_Polygon2D) myPolygon2D;
  Handle(Geom_Surface)   mySurface;
};

#endif // _BRep_PolygonOnSurface_HeaderFile
