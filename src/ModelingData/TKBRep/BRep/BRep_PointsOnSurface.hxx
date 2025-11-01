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

#ifndef _BRep_PointsOnSurface_HeaderFile
#define _BRep_PointsOnSurface_HeaderFile

#include <Standard.hxx>

#include <BRep_PointRepresentation.hxx>
#include <Standard_Real.hxx>
class Geom_Surface;
class TopLoc_Location;

class BRep_PointsOnSurface;
DEFINE_STANDARD_HANDLE(BRep_PointsOnSurface, BRep_PointRepresentation)

//! Root for points on surface.
class BRep_PointsOnSurface : public BRep_PointRepresentation
{

public:
  Standard_EXPORT virtual const Handle(Geom_Surface)& Surface() const Standard_OVERRIDE;

  Standard_EXPORT virtual void Surface(const Handle(Geom_Surface)& S) Standard_OVERRIDE;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream,
                                        Standard_Integer  theDepth = -1) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(BRep_PointsOnSurface, BRep_PointRepresentation)

protected:
  Standard_EXPORT BRep_PointsOnSurface(const Standard_Real         P,
                                       const Handle(Geom_Surface)& S,
                                       const TopLoc_Location&      L);

private:
  Handle(Geom_Surface) mySurface;
};

#endif // _BRep_PointsOnSurface_HeaderFile
