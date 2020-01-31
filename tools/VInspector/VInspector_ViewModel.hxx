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

#include <Standard_WarningsDisable.hxx>
#include <QByteArray>
#include <QHash>
#include <QMap>
#include <QObject>
#include <Standard_WarningsRestore.hxx>

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
  virtual ~VInspector_ViewModel() {}

  //! Creates model columns and root items.
  Standard_EXPORT virtual void InitColumns() Standard_OVERRIDE;

  //! Initialize the model by the given context
  //! \param theContext viewer context
  Standard_EXPORT Handle(AIS_InteractiveContext) GetContext() const;
  
  //! Initialize the model by the given context
  //! \param theContext viewer context
  Standard_EXPORT void SetContext (const Handle(AIS_InteractiveContext)& theContext);

  //! Returns tree view indices for the given pointers of presentable object
  //! \param thePointers a list of presentation pointers
  //! \param theParent an index of the parent item
  //! \param [out] container of indices
  Standard_EXPORT void FindPointers (const QStringList& thePointers,
                                     const QModelIndex& theParent,
                                     QModelIndexList& theFoundIndices);

  //! Returns tree model index of the presentation item in the tree view.
  //! \param thePresentation a presentation
  //! \return model index if the value is found or Null model index
  Standard_EXPORT QModelIndex FindIndex (const Handle(AIS_InteractiveObject)& thePresentation) const;

  //! Updates tree model
  Standard_EXPORT void UpdateTreeModel();

protected:
  //! Creates root item
  //! \param theColumnId index of a column
  Standard_EXPORT virtual TreeModel_ItemBasePtr createRootItem (const int theColumnId) Standard_OVERRIDE;

};

#endif
