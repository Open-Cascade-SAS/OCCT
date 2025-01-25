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

#ifndef _BRepToIGES_BRShell_HeaderFile
#define _BRepToIGES_BRShell_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepToIGES_BREntity.hxx>
#include <Message_ProgressRange.hxx>

class IGESData_IGESEntity;
class TopoDS_Shape;
class TopoDS_Shell;
class TopoDS_Face;

//! This class implements the transfer of Shape Entities from Geom
//! To IGES. These can be :
//! . Vertex
//! . Edge
//! . Wire
class BRepToIGES_BRShell : public BRepToIGES_BREntity
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT BRepToIGES_BRShell();

  Standard_EXPORT BRepToIGES_BRShell(const BRepToIGES_BREntity& BR);

  //! Transfert an Shape entity from TopoDS to IGES
  //! This entity must be a Face or a Shell.
  //! If this Entity could not be converted, this member returns a NullEntity.
  Standard_EXPORT Handle(IGESData_IGESEntity) TransferShell(
    const TopoDS_Shape&          start,
    const Message_ProgressRange& theProgress = Message_ProgressRange());

  //! Transfert an Shell entity from TopoDS to IGES
  //! If this Entity could not be converted, this member returns a NullEntity.
  Standard_EXPORT Handle(IGESData_IGESEntity) TransferShell(
    const TopoDS_Shell&          start,
    const Message_ProgressRange& theProgress = Message_ProgressRange());

  //! Transfert a Face entity from TopoDS to IGES
  //! If this Entity could not be converted, this member returns a NullEntity.
  Standard_EXPORT Handle(IGESData_IGESEntity) TransferFace(
    const TopoDS_Face&           start,
    const Message_ProgressRange& theProgress = Message_ProgressRange());

protected:
private:
};

#endif // _BRepToIGES_BRShell_HeaderFile
