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

#ifndef _BRepMesh_Context_HeaderFile
#define _BRepMesh_Context_HeaderFile

#include <IMeshTools_Context.hxx>

//! Class implementing default context of BRepMesh algorithm.
//! Initializes context by default algorithms.
class BRepMesh_Context : public IMeshTools_Context
{
public:
  //! Constructor.
  Standard_EXPORT BRepMesh_Context(
    IMeshTools_MeshAlgoType theMeshType = IMeshTools_MeshAlgoType_DEFAULT);

  //! Destructor.
  Standard_EXPORT virtual ~BRepMesh_Context();

  DEFINE_STANDARD_RTTIEXT(BRepMesh_Context, IMeshTools_Context)
};

#endif