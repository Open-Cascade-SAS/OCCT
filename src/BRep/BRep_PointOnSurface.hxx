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

#ifndef _BRep_PointOnSurface_HeaderFile
#define _BRep_PointOnSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Real.hxx>
#include <BRep_PointsOnSurface.hxx>
#include <Standard_Boolean.hxx>
class Geom_Surface;
class TopLoc_Location;


class BRep_PointOnSurface;
DEFINE_STANDARD_HANDLE(BRep_PointOnSurface, BRep_PointsOnSurface)

//! Representation by two parameters on a surface.
class BRep_PointOnSurface : public BRep_PointsOnSurface
{

public:

  
  Standard_EXPORT BRep_PointOnSurface(const Standard_Real P1, const Standard_Real P2, const Handle(Geom_Surface)& S, const TopLoc_Location& L);
  
  Standard_EXPORT virtual Standard_Boolean IsPointOnSurface() const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_Boolean IsPointOnSurface (const Handle(Geom_Surface)& S, const TopLoc_Location& L) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_Real Parameter2() const Standard_OVERRIDE;
  
  Standard_EXPORT virtual void Parameter2 (const Standard_Real P) Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(BRep_PointOnSurface,BRep_PointsOnSurface)

protected:




private:


  Standard_Real myParameter2;


};







#endif // _BRep_PointOnSurface_HeaderFile
