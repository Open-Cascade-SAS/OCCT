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

#include <inspector/TreeModel_ItemBase.hxx>

#include <TopTools_IndexedMapOfShape.hxx>

#include <Standard.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS_Shape.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QMap>
#include <QVariant>
#include <Standard_WarningsRestore.hxx>

class ShapeView_ItemShape;
typedef QExplicitlySharedDataPointer<ShapeView_ItemShape> ShapeView_ItemShapePtr;

//! \class ShapeView_ItemShape
//! This item is connected to TopoDS_Shape.
//! Parent is either ShapeView_ItemRoot or ShapeView_ItemShape, children are ShapeView_ItemShape or no children
class ShapeView_ItemShape : public TreeModel_ItemBase
{
public:

  //! Creates an item wrapped by a shared pointer
  //! \param theRow the item row position in the parent item
  //! \param theColumn the item column position in the parent item
  //! \return the pointer to the created item
  static ShapeView_ItemShapePtr CreateItem (TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn)
  { return ShapeView_ItemShapePtr (new ShapeView_ItemShape (theParent, theRow, theColumn)); }

  //! Destructor
  virtual ~ShapeView_ItemShape() {}

  //! Returns explode type of the item
  TopAbs_ShapeEnum ExplodeType() const { return myExplodeType; }

  //! Sets explore type
  //! \param theType type of item explode. If TopAbs_SHAPE, no explode, only iteration by shape
  void SetExplodeType (const TopAbs_ShapeEnum theType) { myExplodeType  = theType; }

  //! Returns the current shape
  const TopoDS_Shape& GetItemShape() const { initItem(); return myShape; }

  //! Returns child(extracted) shape for the current shape by the index
  //! \param theRowId an index of child shape
  //! \returns shape instance or NULL
  Standard_EXPORT TopoDS_Shape Shape (const int theRowId) const;

  //! Returns name of BREP file for the shape if exists
  //! \return string value
  QString GetFileName() const { return myFileName; }

  //! Sets name of BREP file for the shape if exists
  //! \return string value
  void SetFileName (const QString& theFileName) { myFileName = theFileName; }

  //! Inits the item, fills internal containers
  Standard_EXPORT virtual void Init() Standard_OVERRIDE;

  //! Resets cached values
  Standard_EXPORT virtual void Reset() Standard_OVERRIDE;

  //! Returns data value for the role.
  //! \param theRole a value role
  //! \return the value
  Standard_EXPORT virtual QVariant initValue(const int theRole) const Standard_OVERRIDE;

  //! \return number of children.
  Standard_EXPORT virtual int initRowCount() const Standard_OVERRIDE;

  //! Returns stream value of the item to fulfill property panel.
  //! \return stream value or dummy
  Standard_EXPORT virtual void initStream (Standard_OStream& theOStream) const Standard_OVERRIDE;

protected:

  //! Initializes the current item. It is empty because Reset() is also empty.
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

private:

  //! Constructor
  ShapeView_ItemShape (TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn)
    : TreeModel_ItemBase (theParent, theRow, theColumn), myExplodeType (TopAbs_SHAPE) {}

private:
  TopAbs_ShapeEnum myExplodeType; //!< type of explore own shape and get children

  TopoDS_Shape myShape; //!< current shape
  QString myFileName; //!< BREP file name

  TopTools_IndexedMapOfShape myChildShapes; //!< cached container of child shapes
};

#endif
