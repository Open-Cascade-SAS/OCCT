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

#ifndef __IVTK_ISHAPE_H__
#define __IVTK_ISHAPE_H__

#include <IVtk_Interface.hxx>
#include <IVtk_Types.hxx>

class IVtk_IShape;
DEFINE_STANDARD_HANDLE(IVtk_IShape, IVtk_Interface)

//! @class IVtk_IShape
//! @brief Interface for working with a shape and its sub-shapes ids.
class IVtk_IShape : public IVtk_Interface
{
public:
  typedef Handle(IVtk_IShape) Handle;

  virtual ~IVtk_IShape() {}

  IVtk_IdType GetId() const { return myId; }

  void SetId(const IVtk_IdType theId) { myId = theId; }

  //! Get ids of sub-shapes composing a sub-shape with the given id
  virtual IVtk_ShapeIdList GetSubIds(const IVtk_IdType theId) const = 0;

  DEFINE_STANDARD_RTTIEXT(IVtk_IShape, IVtk_Interface)

private:
  IVtk_IdType myId;
};

typedef NCollection_List<IVtk_IShape::Handle> IVtk_ShapePtrList;

#endif // __IVTK_ISHAPE_H__
