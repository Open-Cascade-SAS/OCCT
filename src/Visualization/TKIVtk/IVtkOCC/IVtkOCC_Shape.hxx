// Created on: 2011-10-14
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

#ifndef __IVTKOCC_SHAPE_H__
#define __IVTKOCC_SHAPE_H__

#include <IVtk_IShape.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedMap.hxx>
#include <SelectMgr_SelectableObject.hxx>

//! @class IVtkOCC_Shape
//! @brief OCC implementation of IShape interface.
class IVtkOCC_Shape : public IVtk_IShape
{
public:
  typedef occ::handle<IVtkOCC_Shape> Handle;

  //! Constructor for OCC IShape implementation.
  //! @param[in] theShape  shape to display
  //! @param[in] theDrawerLink  default attributes to link
  Standard_EXPORT IVtkOCC_Shape(
    const TopoDS_Shape&              theShape,
    const occ::handle<Prs3d_Drawer>& theDrawerLink = occ::handle<Prs3d_Drawer>());

  //! Destructor
  Standard_EXPORT ~IVtkOCC_Shape() override;

  //! Returns unique ID of the given sub-shape within the top-level shape.
  Standard_EXPORT IVtk_IdType GetSubShapeId(const IVtk_IShape::Handle&) const;

  DEFINE_STANDARD_RTTIEXT(IVtkOCC_Shape, IVtk_IShape)

  //! Get the wrapped original OCCT shape
  //! @return TopoDS_Shape the wrapped original OCCT shape
  const TopoDS_Shape& GetShape() const { return myTopoDSShape; }

  //! @brief Get local ID of a sub-shape.
  //!
  //! Returns unique ID of the given sub-shape within the top-level shape.
  //! Note that the sub-shape ID remains unchanged until the top-level is
  //! modified by some operation.
  //! @param[in]  subShape sub-shape whose ID is returned
  //! @return local ID of the sub-shape.
  Standard_EXPORT IVtk_IdType GetSubShapeId(const TopoDS_Shape& theSubShape) const;

  //! Get ids of sub-shapes composing a sub-shape with the given id
  Standard_EXPORT NCollection_List<IVtk_IdType> GetSubIds(const IVtk_IdType) const override;

  //! @brief Get a sub-shape by its local ID.
  //!
  //! @param[in]  id local ID of a sub-shape
  //! @return TopoDS_Shape& a sub-shape
  Standard_EXPORT const TopoDS_Shape& GetSubShape(const IVtk_IdType theId) const;

  //! Stores a handle to selectable object used by OCCT selection algorithm
  //! in a data field. This object internally caches selection data
  //! so it should be stored until the shape is no longer selectable.
  //! Note that the selectable object keeps a pointer to OccShape.
  //! @param[in]  selObj Handle to the selectable object
  void SetSelectableObject(const occ::handle<SelectMgr_SelectableObject>& theSelObj)
  {
    mySelectable = theSelObj;
  }

  //! @return Handle to the selectable object for this shape.
  const occ::handle<SelectMgr_SelectableObject>& GetSelectableObject() const
  {
    return mySelectable;
  }

  //! Return presentation attributes.
  const occ::handle<Prs3d_Drawer>& Attributes() const { return myOCCTDrawer; }

  //! Set presentation attributes.
  void SetAttributes(const occ::handle<Prs3d_Drawer>& theDrawer) { myOCCTDrawer = theDrawer; }

private:
  //! @brief Build a map of sub-shapes by their IDs
  //!
  //! Private method, assigns IDs to all sub-shapes of the
  //! top-level shape.
  //! @see IVtkOCC_Shape::GetSubShapeId
  void buildSubShapeIdMap();

private:
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>
                            mySubShapeIds; //!< Map of sub-shapes by their IDs
  TopoDS_Shape              myTopoDSShape; //!< The wrapped main OCCT shape
  occ::handle<Prs3d_Drawer> myOCCTDrawer;  //!< presentation attributes
  occ::handle<SelectMgr_SelectableObject>
    mySelectable; //!< Link to a holder of selection primitives
};

#endif // __IVTKOCC_SHAPE_H__
