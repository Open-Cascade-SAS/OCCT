// Created on: 2016-07-04
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

#ifndef _BRepMesh_ModelPreProcessor_HeaderFile
#define _BRepMesh_ModelPreProcessor_HeaderFile

#include <IMeshTools_ModelAlgo.hxx>
#include <IMeshTools_Parameters.hxx>
#include <IMeshData_Types.hxx>

//! Class implements functionality of model pre-processing tool.
//! Nullifies existing polygonal data in case if model elements
//! have IMeshData_Outdated status.
class BRepMesh_ModelPreProcessor : public IMeshTools_ModelAlgo
{
public:

  //! Constructor.
  Standard_EXPORT BRepMesh_ModelPreProcessor();

  //! Destructor.
  Standard_EXPORT virtual ~BRepMesh_ModelPreProcessor();

  DEFINE_STANDARD_RTTIEXT(BRepMesh_ModelPreProcessor, IMeshTools_ModelAlgo)

protected:

  //! Performs processing of edges of the given model.
  Standard_EXPORT virtual Standard_Boolean performInternal (
    const Handle(IMeshData_Model)& theModel,
    const IMeshTools_Parameters&   theParameters,
    const Message_ProgressRange&   theRange) Standard_OVERRIDE;
};

#endif
