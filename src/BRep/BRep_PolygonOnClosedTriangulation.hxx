// Created on: 1995-03-15
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

#ifndef _BRep_PolygonOnClosedTriangulation_HeaderFile
#define _BRep_PolygonOnClosedTriangulation_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <BRep_PolygonOnTriangulation.hxx>
#include <Standard_Boolean.hxx>
class Poly_PolygonOnTriangulation;
class Poly_Triangulation;
class TopLoc_Location;
class BRep_CurveRepresentation;


class BRep_PolygonOnClosedTriangulation;
DEFINE_STANDARD_HANDLE(BRep_PolygonOnClosedTriangulation, BRep_PolygonOnTriangulation)

//! A representation by two arrays of nodes on a
//! triangulation.
class BRep_PolygonOnClosedTriangulation : public BRep_PolygonOnTriangulation
{

public:

  
  Standard_EXPORT BRep_PolygonOnClosedTriangulation(const Handle(Poly_PolygonOnTriangulation)& P1, const Handle(Poly_PolygonOnTriangulation)& P2, const Handle(Poly_Triangulation)& Tr, const TopLoc_Location& L);
  
  //! Returns True.
  Standard_EXPORT virtual Standard_Boolean IsPolygonOnClosedTriangulation() const Standard_OVERRIDE;
  
  Standard_EXPORT virtual void PolygonOnTriangulation2 (const Handle(Poly_PolygonOnTriangulation)& P2) Standard_OVERRIDE;
  
  Standard_EXPORT virtual const Handle(Poly_PolygonOnTriangulation)& PolygonOnTriangulation2() const Standard_OVERRIDE;
  
  //! Return a copy of this representation.
  Standard_EXPORT virtual Handle(BRep_CurveRepresentation) Copy() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(BRep_PolygonOnClosedTriangulation,BRep_PolygonOnTriangulation)

protected:




private:


  Handle(Poly_PolygonOnTriangulation) myPolygon2;


};







#endif // _BRep_PolygonOnClosedTriangulation_HeaderFile
