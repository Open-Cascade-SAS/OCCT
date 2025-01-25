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

#include <inspector/TreeModel_ItemBase.hxx>
#include <Standard.hxx>
#include <TopoDS_Shape.hxx>
#include <inspector/TreeModel_ModelBase.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QMap>
#include <QObject>
#include <Standard_WarningsRestore.hxx>

class ShapeView_TreeModel;

//! \class ShapeView_TreeModel
//! View model to visualize content of TopoDS_Shape
class ShapeView_TreeModel : public TreeModel_ModelBase
{
public:
  //! Constructor
  Standard_EXPORT ShapeView_TreeModel(QObject* theParent);

  //! Destructor
  virtual ~ShapeView_TreeModel() {}

  //! Adds shape, append it to the model root item
  //! \param theShape a shape instance
  Standard_EXPORT void AddShape(const TopoDS_Shape& theShape);

  //! Removes all shapes in the model root item
  Standard_EXPORT void RemoveAllShapes();

  //! Returns model index of the shape.
  //! \param theShape a shape object
  //! \return the model index
  QModelIndex FindIndex(const TopoDS_Shape& theShape) const;

protected:
  //! Creates root item
  //! \param theColumnId index of a column
  Standard_EXPORT virtual TreeModel_ItemBasePtr createRootItem(const int theColumnId)
    Standard_OVERRIDE;
};

#endif
