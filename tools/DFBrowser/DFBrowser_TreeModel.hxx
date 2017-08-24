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

#ifndef DFBrowser_TreeModel_H
#define DFBrowser_TreeModel_H

#include <inspector/DFBrowser_ItemBase.hxx>
#include <inspector/TreeModel_ModelBase.hxx>

#include <NCollection_List.hxx>
#include <Standard.hxx>
#include <TDF_Label.hxx>
#include <TDocStd_Application.hxx>

#include <QByteArray>
#include <QHash>
#include <QObject>

class DFBrowser_Module;
class DFBrowser_TreeModel;

//! \class DFBrowser_TreeModel
//! Tree model that has items described TDocStd_Application. The structure of items is the following:
//! -  <DFBrowser_ItemApplication>: for TDocStd_Application
//! -    <DFBrowser_ItemDocument>: for TDocStd_Document
//!-       <DFBrowser_Item>: for either TDF_Label or TDF_Attribute
//! It is possible to visualize some items as highlighted.
class DFBrowser_TreeModel : public TreeModel_ModelBase
{
  Q_OBJECT
public:

  //! Constructor
  Standard_EXPORT DFBrowser_TreeModel (QObject* theParent, DFBrowser_Module* theModule);

  //! Destructor
  virtual ~DFBrowser_TreeModel() Standard_OVERRIDE {};

  //! Fills the root item by the application
  Standard_EXPORT void Init (const Handle(TDocStd_Application)& theApplication);

  //! Returns an OCAF application or NULL
  //! \return an application instance
  Standard_EXPORT Handle(TDocStd_Application) GetTDocStdApplication() const;

  //! Returns true if the tree view model contains highlighted items. This highlight is set manually.
  bool HasHighlighted() { return !myHighlightedIndices.isEmpty(); }

  //! Sets items of the indices highlighted in the model.
  //! \param theIndices a list of tree model indices
  void SetHighlighted (const QModelIndexList& theIndices = QModelIndexList()) { myHighlightedIndices = theIndices; }

  //! Returns tree model index of the label item. It creates container of the label fathers and
  //! starting from the rools label it descends by the found labels till the parameter label.
  //! \param theLabel an OCAF label
  //! \return model index if the value is found or Null model index
  Standard_EXPORT QModelIndex FindIndex (const TDF_Label& theLabel) const;

  //! Returns tree model index by list of label entries and (possible) attribute name.
  //! \param theLabelEntries a container of label entries starting from root till searched label
  //! \param theValue a label entry or attribute name
  //! \return model index if the value is found or Null model index
  Standard_EXPORT QModelIndex FindIndexByPath (const QStringList& theLabelEntries, const QString& theValue) const;

  //! Returns tree model index of the attribute item. It gets the attributes label, find index of the label
  //! and after find under this item attribute child item.
  //! \param theAttribute an OCAF attribute
  //! \return model index if the value is found or Null model index
  Standard_EXPORT QModelIndex FindIndexByAttribute (Handle(TDF_Attribute) theAttribute) const;

  //! Returns tree model indices for the labels.
  Standard_EXPORT void ConvertToIndices (const NCollection_List<TDF_Label>& theReferences, QModelIndexList& theIndices);

  //! Returns tree model indices of references 
  Standard_EXPORT void ConvertToIndices (const NCollection_List<Handle(TDF_Attribute)>& theReferences,
                                         QModelIndexList& theIndices);

  //! Returns the data stored under the given role for the current item
  //! \param theIndex the item model index
  //! \param theRole the item model role
  Standard_EXPORT virtual QVariant data (const QModelIndex& theIndex,
                                         int theRole = Qt::DisplayRole) const Standard_OVERRIDE;

  //! Returns count of columns in the model
  //! \param theParent an index of the parent item
  //! \return integer value
  virtual int columnCount (const QModelIndex& theParent = QModelIndex()) const Standard_OVERRIDE
  { (void)theParent; return 1; }

private:

  QModelIndexList myHighlightedIndices; //!< tree model indices that should be visualized as highlighted
};

#endif
