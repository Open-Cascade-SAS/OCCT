// Created on: 2016-04-07
// Copyright (c) 2016 OPEN CASCADE SAS
// Created by: Oleg AGASHIN
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

#ifndef _BRepMesh_ModelBuilder_HeaderFile
#define _BRepMesh_ModelBuilder_HeaderFile

#include <IMeshTools_ModelBuilder.hxx>
#include <Standard_Type.hxx>
#include <TopoDS_Shape.hxx>

//! Class implements interface representing tool for discrete model building.
//! 
//! The following statuses should be used by default:
//! Message_Done1 - model has been successfully built.
//! Message_Fail1 - empty shape.
//! Message_Fail2 - model has not been build due to unexpected reason.
class BRepMesh_ModelBuilder : public IMeshTools_ModelBuilder
{
public:

  //! Constructor.
  Standard_EXPORT BRepMesh_ModelBuilder ();

  //! Destructor.
  Standard_EXPORT virtual ~BRepMesh_ModelBuilder ();

  DEFINE_STANDARD_RTTIEXT(BRepMesh_ModelBuilder, IMeshTools_ModelBuilder)

protected:

  //! Creates discrete model for the given shape.
  //! Returns nullptr in case of failure.
  Standard_EXPORT virtual Handle (IMeshData_Model) performInternal (
    const TopoDS_Shape&          theShape,
    const IMeshTools_Parameters& theParameters) Standard_OVERRIDE;
};

#endif