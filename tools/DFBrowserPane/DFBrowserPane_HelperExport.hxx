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

#ifndef DFBrowserPane_HelperExport_H
#define DFBrowserPane_HelperExport_H

#include <TopoDS_Shape.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QObject>
#include <QModelIndex>
#include <Standard_WarningsRestore.hxx>

//! \class DFBrowserPane_HelperExport
//! \brief It performs export to BREP of a shape by button is pressed
//! It contains a container of shapes for model indices. If button is pressed for index where the
//! shape exists, this shape is exported to BREP file.
//! It contains a container of shapes, it is important to clear this helper after using.
class DFBrowserPane_HelperExport : public QObject
{
  Q_OBJECT
public:
  //! Constructor
  DFBrowserPane_HelperExport (QObject* theParent) { (void)theParent; }

  //! Destructor
  virtual ~DFBrowserPane_HelperExport() Standard_OVERRIDE {}

  //! Clears current shapes
  void Clear() { myShapes.clear(); }

  //! Append a shape to be exported if pressed button on item from the given list
  //! \param theShape a shape
  //! \param theIndicies a list of indices for this shape
  void AddShape (const TopoDS_Shape& theShape, const QModelIndexList& theIndices);

  //! Returns whether the map of shapes contains a shape for the index
  //! \param theIndex a model index
  //! \return true if the map contains shape
  bool HasShape (const QModelIndex& theIndex) const { return myShapes.contains (theIndex); }

  //! Returns shape for the index
  //! \param theIndex a model view index
  //! \return a cached shape
  const TopoDS_Shape& Shape (const QModelIndex& theIndex) { return myShapes[theIndex]; }

public slots:

  //! Slot that processing button press for the model index
  //! \param theIndex a model index
  void OnButtonPressed (const QModelIndex& theIndex);

private:
#ifdef _MSC_VER
#pragma warning(push, 0) // 4251: class 'QMap<QModelIndex,TopoDS_Shape>' needs to have dll-interface...
#endif
  QMap<QModelIndex, TopoDS_Shape> myShapes; //!< a container of shapes
#ifdef _MSC_VER
#pragma warning(pop)
#endif
};

#endif
