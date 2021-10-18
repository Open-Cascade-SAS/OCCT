// Created on: 1994-11-18
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

#ifndef _GeomToIGES_GeomVector_HeaderFile
#define _GeomToIGES_GeomVector_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GeomToIGES_GeomEntity.hxx>
class IGESGeom_Direction;
class Geom_Vector;
class Geom_VectorWithMagnitude;
class Geom_Direction;


//! This class implements the transfer of the Vector from Geom
//! to IGES . These can be :
//! . Vector
//! * Direction
//! * VectorWithMagnitude
class GeomToIGES_GeomVector  : public GeomToIGES_GeomEntity
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT GeomToIGES_GeomVector();
  
  //! Creates a tool GeomVector ready to run and sets its
  //! fields as GE's.
  Standard_EXPORT GeomToIGES_GeomVector(const GeomToIGES_GeomEntity& GE);
  
  //! Transfert  a  GeometryEntity which  answer True  to  the
  //! member : BRepToIGES::IsGeomVector(Geometry).  If this
  //! Entity could not be converted, this member returns a NullEntity.
  Standard_EXPORT Handle(IGESGeom_Direction) TransferVector (const Handle(Geom_Vector)& start);
  
  Standard_EXPORT Handle(IGESGeom_Direction) TransferVector (const Handle(Geom_VectorWithMagnitude)& start);
  
  Standard_EXPORT Handle(IGESGeom_Direction) TransferVector (const Handle(Geom_Direction)& start);




protected:





private:





};







#endif // _GeomToIGES_GeomVector_HeaderFile
