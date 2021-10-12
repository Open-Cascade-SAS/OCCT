// Created on: 1995-03-09
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

#ifndef _BRep_Polygon3D_HeaderFile
#define _BRep_Polygon3D_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <BRep_CurveRepresentation.hxx>
#include <Standard_Boolean.hxx>
class Poly_Polygon3D;
class TopLoc_Location;


class BRep_Polygon3D;
DEFINE_STANDARD_HANDLE(BRep_Polygon3D, BRep_CurveRepresentation)

//! Representation by a 3D polygon.
class BRep_Polygon3D : public BRep_CurveRepresentation
{

public:

  
  Standard_EXPORT BRep_Polygon3D(const Handle(Poly_Polygon3D)& P, const TopLoc_Location& L);
  
  //! Returns True.
  Standard_EXPORT virtual Standard_Boolean IsPolygon3D() const Standard_OVERRIDE;
  
  Standard_EXPORT virtual const Handle(Poly_Polygon3D)& Polygon3D() const Standard_OVERRIDE;
  
  Standard_EXPORT virtual void Polygon3D (const Handle(Poly_Polygon3D)& P) Standard_OVERRIDE;
  
  //! Return a copy of this representation.
  Standard_EXPORT Handle(BRep_CurveRepresentation) Copy() const Standard_OVERRIDE;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(BRep_Polygon3D,BRep_CurveRepresentation)

protected:




private:


  Handle(Poly_Polygon3D) myPolygon3D;


};







#endif // _BRep_Polygon3D_HeaderFile
