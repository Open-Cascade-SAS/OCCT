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

#ifndef ShapeView_ItemShape_H
#define ShapeView_ItemShape_H

#include <inspector/ShapeView_ItemBase.hxx>
#include <Standard.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopoDS_Shape.hxx>

#include <QMap>
#include <QVariant>

class ShapeView_ItemShape;
typedef QExplicitlySharedDataPointer<ShapeView_ItemShape> ShapeView_ItemShapePtr;

//! \class ShapeView_ItemShape
//! This item is connected to TopoDS_Shape.
//! Parent is either ShapeView_ItemRoot or ShapeView_ItemShape, children are ShapeView_ItemShape or no children
class ShapeView_ItemShape : public ShapeView_ItemBase
{
public:

  //! Creates an item wrapped by a shared pointer
  //! \param theRow the item row positition in the parent item
  //! \param theColumn the item column positition in the parent item
  //! \return the pointer to the created item
  static ShapeView_ItemShapePtr CreateItem (TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn)
  { return ShapeView_ItemShapePtr (new ShapeView_ItemShape (theParent, theRow, theColumn)); }

  //! Destructor
  virtual ~ShapeView_ItemShape() Standard_OVERRIDE {};

  //! Returns the current shape
  const TopoDS_Shape& GetItemShape() const { initItem(); return myShape; }

  //! Returns child(extracted) shape for the current shape by the index
  //! \param theRowId an index of child shape
  //! \returns shape instance or NULL
  Standard_EXPORT TopoDS_Shape GetShape (const int theRowId) const;

  //! Returns name of BREP file for the shape if exists
  //! \return string valuie
  QString GetFileName() const { return myFileName; }

  //! Sets name of BREP file for the shape if exists
  //! \return string valuie
  void SetFileName (const QString& theFileName) { myFileName = theFileName; }

  //! Returns TShape pointer info of the current TopoDS Shape
  //! \return string value
  TCollection_AsciiString TShapePointer() const { return getPointerInfo (myShape.TShape()); }

  //! Inits the item, fills internal containers
  Standard_EXPORT virtual void Init() Standard_OVERRIDE;

  //! Resets cached values
  Standard_EXPORT virtual void Reset() Standard_OVERRIDE;

  //! Return data value for the role.
  //! \param theRole a value role
  //! \return the value
  Standard_EXPORT virtual QVariant initValue(const int theRole) const;

  //! \return number of children.
  Standard_EXPORT virtual int initRowCount() const Standard_OVERRIDE;

protected:

  //! Initialize the current item. It is empty because Reset() is also empty.
  virtual void initItem() const Standard_OVERRIDE;

  //! Creates a child item in the given position.
  //! \param theRow the child row position
  //! \param theColumn the child column position
  //! \return the created item
  virtual TreeModel_ItemBasePtr createChild (int theRow, int theColumn) Standard_OVERRIDE;

  //! Returns number of child shapes. Init item if it is not initialized
  //! \return integer value
  int getRowCount() const;

  //! Returns current shape, initialized item if it has not been initialized yet
  //! \return shape value
  TopoDS_Shape getShape() const;

  //! Convert pointer to string value
  //! \param thePointer a pointer
  //! \param isShortInfo if true, all '0' symbols in the beginning of the pointer are skipped
  //! \return the string value
  static TCollection_AsciiString getPointerInfo (const Handle(Standard_Transient)& thePointer, const bool isShortInfo = true);

private:

  //! Constructor
  ShapeView_ItemShape(TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn)
  : ShapeView_ItemBase(theParent, theRow, theColumn) {}

private:

  TopoDS_Shape myShape; //!< current shape
  QString myFileName; //!< BREP file name
};

#endif
