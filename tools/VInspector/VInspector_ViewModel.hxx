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

#ifndef VInspector_ViewModel_H
#define VInspector_ViewModel_H

#include <AIS_InteractiveContext.hxx>
#include <NCollection_List.hxx>
#include <SelectBasics_EntityOwner.hxx>
#include <Standard.hxx>
#include <inspector/TreeModel_ModelBase.hxx>
#include <inspector/VInspector_ItemBase.hxx>

#include <QByteArray>
#include <QHash>
#include <QMap>
#include <QObject>

class OCAFSampleAPI_Module;
class OCAFSampleModel_DocumentMgr;

class QItemSelectionModel;

//! \class VInspector_ViewModel
//! The class that visualizes the AIS context content
class VInspector_ViewModel : public TreeModel_ModelBase
{
  Q_OBJECT

public:

  //! Constructor
  Standard_EXPORT VInspector_ViewModel (QObject* theParent);

  //! Destructor
  virtual ~VInspector_ViewModel() Standard_OVERRIDE {};

  //! Initialize the model by the given context
  //! \param theContext viewer context
  Standard_EXPORT const Handle(AIS_InteractiveContext)& GetContext() const;

  //! Initialize the model by the given context
  //! \param theContext viewer context
  Standard_EXPORT void SetContext (const Handle(AIS_InteractiveContext)& theContext);

  //! Returns tree view indices for the given pointers of presentable object
  //! \param thePointers a list of presentation pointers
  //! \return container of indices
  Standard_EXPORT QModelIndexList FindPointers (const QStringList& thePointers);

  //! Returns tree model index of the presentation item in the tree view.
  //! \param thePresentation a presentation
  //! \return model index if the value is found or Null model index
  Standard_EXPORT QModelIndex FindIndex (const Handle(AIS_InteractiveObject)& thePresentation) const;

  //! Returns root item by column
  //! \param theColumn an index of the column
  //! \return root item instance
  Standard_EXPORT virtual TreeModel_ItemBasePtr RootItem (const int theColumn) const Standard_OVERRIDE
  { return myRootItems[theColumn]; }

  //! Returns count of columns in the model
  //! \param theParent an index of the parent item
  //! \return integer value
  Standard_EXPORT virtual int columnCount (const QModelIndex& theParent = QModelIndex()) const Standard_OVERRIDE
  { (void)theParent; return 9; }

  //! Returns the header data for the given role and section in the header with the specified orientation.
  //! \param theSection the header section. For horizontal headers - column number, for vertical headers - row number.
  //! \param theOrientation a header orientation
  //! \param theRole a data role
  //! \return the header data
  Standard_EXPORT virtual QVariant headerData (int theSection, Qt::Orientation theOrientation,
                                               int theRole = Qt::DisplayRole ) const Standard_OVERRIDE;

  //! Returns select owners for tree view selected items
  //! \param theSelectionModel a selection model
  //! \param theOwners an output list of owners
  Standard_EXPORT static void GetSelectedOwners (QItemSelectionModel* theSelectionModel,
                                                 NCollection_List<Handle(SelectBasics_EntityOwner)>& theOwners);

private:

  QMap<int, TreeModel_ItemBasePtr> myRootItems; //!< container of root items, for each column own root item
};

#endif
