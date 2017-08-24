// Created on: 2017-06-16
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2017 OPEN CASCADE SAS
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

#ifndef ShapeView_ItemRoot_H
#define ShapeView_ItemRoot_H

#include <NCollection_List.hxx>
#include <inspector/ShapeView_ItemBase.hxx>
#include <Standard.hxx>
#include <TopoDS_Shape.hxx>

class ShapeView_ItemRoot;
typedef QExplicitlySharedDataPointer<ShapeView_ItemRoot> ShapeView_ItemRootPtr;

//! \class ShapeView_ItemRoot
//! Collects shapes that should be visualized in tree view. Shapes are cached and if shapes are not needed,
//! cache should be cleared using RemoveAllShapes.
//! Parent is NULL, children are ShapeView_ItemShape items.
class ShapeView_ItemRoot : public ShapeView_ItemBase
{
public:

  //! Creates an item wrapped by a shared pointer
  static ShapeView_ItemRootPtr CreateItem (TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn)
    { return ShapeView_ItemRootPtr (new ShapeView_ItemRoot (theParent, theRow, theColumn)); }

  //! Destructor
  virtual ~ShapeView_ItemRoot() Standard_OVERRIDE {};

  //! Appends new shape
  //! \param theShape a shape instance
  void AddShape (const TopoDS_Shape& theShape) { myShapes.Append (theShape); }

  //! Clears internal container of added shapes
  void RemoveAllShapes() { myShapes.Clear(); }

  //! Returns shape by the number
  //! \param theRowId an index of the shape in the internal container.
  Standard_EXPORT const TopoDS_Shape& GetShape (const int theRowId);

protected:

  //! Return data value for the role.
  //! \param theItemRole a value role
  //! \return the value
  virtual QVariant initValue(const int theItemRole) const;

  //! \return number of children.
  virtual int initRowCount() const Standard_OVERRIDE { return myShapes.Size(); }

  //! Creates a child item in the given position.
  //! \param theRow the child row position
  //! \param theColumn the child column position
  //! \return the created item
  virtual TreeModel_ItemBasePtr createChild (int theRow, int theColumn) Standard_OVERRIDE;

private:

  //! Constructor
  //! param theParent a parent item
  ShapeView_ItemRoot(TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn)
  : ShapeView_ItemBase (theParent, theRow, theColumn) {}

private:

  NCollection_List<TopoDS_Shape> myShapes; //!< shapes presented in tree view
};

#endif
