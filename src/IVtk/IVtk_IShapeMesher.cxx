// Created on: 2011-10-11 
// Created by: Roman KOZLOV
// Copyright (c) 2011-2014 OPEN CASCADE SAS 
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

#include <IVtk_IShapeMesher.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IVtk_IShapeMesher,IVtk_Interface)

// Handle implementation


//! Excutes the mesh generation algorithms. To be defined in implementation class.
void IVtk_IShapeMesher::initialize (const IVtk_IShape::Handle&     theShape,
                                    const IVtk_IShapeData::Handle& theData)
{
  myShapeObj = theShape;
  myShapeData = theData;
}

//! Main entry point for building shape representation
//! @param [in] shape IShape to be meshed
//! @param [in] data IShapeData interface visualization data is passed to.
void IVtk_IShapeMesher::Build (const IVtk_IShape::Handle&     theShape,
                               const IVtk_IShapeData::Handle& theData)
{
  if (!theShape.IsNull())
  {
    initialize (theShape, theData);
    internalBuild();
  }
}
