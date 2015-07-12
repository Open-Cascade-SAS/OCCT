// Created on: 1994-11-16
// Created by: Marie Jose MARTZ
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _GeomToIGES_GeomPoint_HeaderFile
#define _GeomToIGES_GeomPoint_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GeomToIGES_GeomEntity.hxx>
class GeomToIGES_GeomEntity;
class IGESGeom_Point;
class Geom_Point;
class Geom_CartesianPoint;


//! This class implements the transfer of the Point Entity from Geom
//! to IGES . These are :
//! . Point
//! * CartesianPoint
class GeomToIGES_GeomPoint  : public GeomToIGES_GeomEntity
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT GeomToIGES_GeomPoint();
  
  //! Creates a tool GeomPoint ready to run and sets its
  //! fields as GE's.
  Standard_EXPORT GeomToIGES_GeomPoint(const GeomToIGES_GeomEntity& GE);
  
  //! Transfert  a  Point from Geom to IGES. If this
  //! Entity could not be converted, this member returns a NullEntity.
  Standard_EXPORT Handle(IGESGeom_Point) TransferPoint (const Handle(Geom_Point)& start);
  
  //! Transfert  a  CartesianPoint from Geom to IGES. If this
  //! Entity could not be converted, this member returns a NullEntity.
  Standard_EXPORT Handle(IGESGeom_Point) TransferPoint (const Handle(Geom_CartesianPoint)& start);




protected:





private:





};







#endif // _GeomToIGES_GeomPoint_HeaderFile
