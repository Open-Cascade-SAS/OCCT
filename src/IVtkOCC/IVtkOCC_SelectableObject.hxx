// Created on: 2011-10-20 
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

#ifndef __IVTKOCC_SELECTABLEOBJECT_H__
#define __IVTKOCC_SELECTABLEOBJECT_H__

#include <Bnd_Box.hxx>
#include <IVtkOCC_Shape.hxx>
#include <Prs3d_Drawer.hxx>
#include <SelectMgr_SelectableObject.hxx>
#include <SelectMgr_Selection.hxx>

// -----------------------------------------------------------------------------
//! @class IVtkOCC_SelectableObject
//! @brief Class with selection primitives used by OCCT selection algorithm.
class IVtkOCC_SelectableObject : public SelectMgr_SelectableObject
{
public:

  //! Constructs a selectable object initialized by the given shape
  //! @param [in] theShape Selectable shape
  IVtkOCC_SelectableObject (const IVtkOCC_Shape::Handle& theShape);

  //! Constructs uninitialized selectable object.
  //! setShape() should be called later.
  IVtkOCC_SelectableObject();

  //! Sets the selectable shape
  //! @param [in] theShape Selectable shape
  void SetShape (const IVtkOCC_Shape::Handle& theShape);

  const IVtkOCC_Shape::Handle&  GetShape() const { return myShape; };

  DEFINE_STANDARD_RTTI( IVtkOCC_SelectableObject )

private: 

  //! Internal method, computes selection data for viewer selector
  //! Inspired by AIS_Shape::ComputeSelection() from OCCT 6.5.1
  //! @param [in] selection container for sensitive primitives
  //! @param [in] mode Selection mode
  void ComputeSelection (const Handle(SelectMgr_Selection)& theSelection,
                         const Standard_Integer theMode);

  const Bnd_Box& BoundingBox();

  IVtkOCC_Shape::Handle myShape;
  Bnd_Box               myBndBox;
  Handle(Prs3d_Drawer)  myOCCTDrawer;
};

DEFINE_STANDARD_HANDLE( IVtkOCC_SelectableObject, SelectMgr_SelectableObject )

#endif // __IVTKOCC_SELECTABLEOBJECT_H__
