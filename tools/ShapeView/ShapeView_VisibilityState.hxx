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

#ifndef ShapeView_VisibilityState_H
#define ShapeView_VisibilityState_H

#include <inspector/TreeModel_VisibilityState.hxx>

#include <inspector/View_Displayer.hxx>

#include <Standard.hxx>
#include <Standard_Macro.hxx>
#include <TopoDS_Shape.hxx>

class TreeModel_ModelBase;

//! \class ShapeView_VisibilityState
//! \brief Class provides connection between model and visualization control
class ShapeView_VisibilityState : public QObject, public TreeModel_VisibilityState
{
  Q_OBJECT
public:
  //! Constructor
  ShapeView_VisibilityState (TreeModel_ModelBase* theModel) : TreeModel_VisibilityState (theModel),
  myPresentationType (View_PresentationType_Main) {}

  //! Destructor
  ~ShapeView_VisibilityState() {}

  //! Sets current displayer
  //! \theDisplayer class that provides connection to visualized objects
  void SetDisplayer (View_Displayer* theDisplayer) { myDisplayer = theDisplayer; }

  //! Sets presentation type for displayer
  //! \param theType type value
  void SetPresentationType (const View_PresentationType theType) { myPresentationType = theType; }

  //! Returns true if visibility of the item can be changed
  //! \param theIndex tree model index
  //! \return boolean value
  virtual bool CanBeVisible (const QModelIndex& theIndex) const Standard_OVERRIDE { return !Shape (theIndex).IsNull(); }

  //! Sets visibility state
  //! \theIndex tree model index
  //! \param theState visibility state
  //! \param toEmitDataChanged boolean flag whether emit of the model should be done immediately
  //! \return true if state is changed
  Standard_EXPORT virtual bool SetVisible (const QModelIndex& theIndex, const bool theState, const bool toEmitDataChanged) Standard_OVERRIDE;

  //! Returns visibility state value
  virtual bool IsVisible (const QModelIndex& theIndex) const Standard_OVERRIDE
  { return myDisplayer->IsVisible (Shape (theIndex), myPresentationType); }

public slots:
  //! Processes the mouse clicked on the index.
  //! It changes the item visibility if model allows to change it.
  //! \theIndex tree model index
  void OnClicked (const QModelIndex& theIndex);

signals:
  //! Signal after OnClicked is performed
  //! \theIndex tree model index
  void itemClicked (const QModelIndex& theIndex);

protected:
  //! Gets shape of the view model by the parameter index if it has a shape
  //! \param theIndex tree model index
  //! \return shape instance
  TopoDS_Shape Shape (const QModelIndex& theIndex) const;

private:
  View_Displayer* myDisplayer; //!< view displayer
  View_PresentationType myPresentationType; //!< presentation type
};

#endif
