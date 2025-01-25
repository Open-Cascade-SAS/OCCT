// Created on: 1995-01-27
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

#ifndef _BRepToIGES_BRSolid_HeaderFile
#define _BRepToIGES_BRSolid_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepToIGES_BREntity.hxx>
class IGESData_IGESEntity;
class TopoDS_Shape;
class TopoDS_Solid;
class TopoDS_CompSolid;
class TopoDS_Compound;

//! This class implements the transfer of Shape Entities from Geom
//! To IGES. These can be :
//! . Vertex
//! . Edge
//! . Wire
class BRepToIGES_BRSolid : public BRepToIGES_BREntity
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT BRepToIGES_BRSolid();

  Standard_EXPORT BRepToIGES_BRSolid(const BRepToIGES_BREntity& BR);

  //! Transfert a Shape entity from TopoDS to IGES
  //! this entity must be a Solid or a CompSolid or a Compound.
  //! If this Entity could not be converted, this member returns a NullEntity.
  Standard_EXPORT Handle(IGESData_IGESEntity) TransferSolid(
    const TopoDS_Shape&          start,
    const Message_ProgressRange& theProgress = Message_ProgressRange());

  //! Transfert a Solid entity from TopoDS to IGES
  //! If this Entity could not be converted, this member returns a NullEntity.
  Standard_EXPORT Handle(IGESData_IGESEntity) TransferSolid(
    const TopoDS_Solid&          start,
    const Message_ProgressRange& theProgress = Message_ProgressRange());

  //! Transfert an CompSolid entity from TopoDS to IGES
  //! If this Entity could not be converted, this member returns a NullEntity.
  Standard_EXPORT Handle(IGESData_IGESEntity) TransferCompSolid(
    const TopoDS_CompSolid&      start,
    const Message_ProgressRange& theProgress = Message_ProgressRange());

  //! Transfert a Compound entity from TopoDS to IGES
  //! If this Entity could not be converted, this member returns a NullEntity.
  Standard_EXPORT Handle(IGESData_IGESEntity) TransferCompound(
    const TopoDS_Compound&       start,
    const Message_ProgressRange& theProgress = Message_ProgressRange());

protected:
private:
};

#endif // _BRepToIGES_BRSolid_HeaderFile
