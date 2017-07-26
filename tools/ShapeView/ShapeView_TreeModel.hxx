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

#ifndef ShapeView_TreeModel_H
#define ShapeView_TreeModel_H

#include <ShapeView_ItemBase.hxx>
#include <Standard.hxx>
#include <TopoDS_Shape.hxx>
#include <TreeModel_ModelBase.hxx>

#include <QMap>
#include <QObject>

class ShapeView_TreeModel;

//! \class ShapeView_TreeModel
//! View model to visualize content of TopoDS_Shape
class ShapeView_TreeModel : public TreeModel_ModelBase
{
public:

  //! Constructor
  Standard_EXPORT ShapeView_TreeModel (QObject* theParent);

  //! Destructor
  virtual ~ShapeView_TreeModel() Standard_OVERRIDE {};

  //! Add shape, append it to the model root item
  //! \param theShape a shape instance
  Standard_EXPORT void AddShape (const TopoDS_Shape& theShape);

  //! Remove all shapes in the model root item
  Standard_EXPORT void RemoveAllShapes();

  //! Returns root item by column
  //! \param theColumn an index of the column
  //! \return root item instance
  virtual TreeModel_ItemBasePtr RootItem(const int theColumn) const Standard_OVERRIDE
  { return myRootItems[theColumn]; }

  //! Returns count of columns in the model.
  //! \param theParent an index of the parent item
  //! \return integer value
  virtual int columnCount (const QModelIndex& theParent = QModelIndex()) const Standard_OVERRIDE
  { (void)theParent; return 7; }

  //! Returns the header data for the given role and section in the header with the specified orientation.
  //! \param theSection the header section. For horizontal headers - column number, for vertical headers - row number.
  //! \param theOrientation a header orientation
  //! \param theRole a data role
  //! \return the header data
  Standard_EXPORT virtual QVariant headerData (int theSection, Qt::Orientation theOrientation,
                                               int theRole = Qt::DisplayRole ) const Standard_OVERRIDE;

private:
  QMap<int, TreeModel_ItemBasePtr> myRootItems; //!< container of root items, for each column own root item
};

#endif
