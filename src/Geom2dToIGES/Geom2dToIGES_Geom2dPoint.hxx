// Created on: 1995-02-01
// Created by: Marie Jose MARTZ
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

#ifndef _Geom2dToIGES_Geom2dPoint_HeaderFile
#define _Geom2dToIGES_Geom2dPoint_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Geom2dToIGES_Geom2dEntity.hxx>
class IGESGeom_Point;
class Geom2d_Point;
class Geom2d_CartesianPoint;


//! This class implements the transfer of the Point Entity from Geom2d
//! to IGES . These are :
//! . 2dPoint
//! * 2dCartesianPoint
class Geom2dToIGES_Geom2dPoint  : public Geom2dToIGES_Geom2dEntity
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Geom2dToIGES_Geom2dPoint();
  
  //! Creates a tool Geom2dPoint ready to run and sets its
  //! fields as G2dE's.
  Standard_EXPORT Geom2dToIGES_Geom2dPoint(const Geom2dToIGES_Geom2dEntity& G2dE);
  
  //! Transfert  a  Point from Geom to IGES. If this
  //! Entity could not be converted, this member returns a NullEntity.
  Standard_EXPORT Handle(IGESGeom_Point) Transfer2dPoint (const Handle(Geom2d_Point)& start);
  
  //! Transfert  a  CartesianPoint from Geom to IGES. If this
  //! Entity could not be converted, this member returns a NullEntity.
  Standard_EXPORT Handle(IGESGeom_Point) Transfer2dPoint (const Handle(Geom2d_CartesianPoint)& start);




protected:





private:





};







#endif // _Geom2dToIGES_Geom2dPoint_HeaderFile
