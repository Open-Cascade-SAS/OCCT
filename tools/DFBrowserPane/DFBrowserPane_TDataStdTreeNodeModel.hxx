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

#ifndef DFBrowserPane_TDataStdTreeNodeMode_H
#define DFBrowserPane_TDataStdTreeNodeMode_H

#include <inspector/TreeModel_ModelBase.hxx>

#include <Standard.hxx>
#include <TDF_Attribute.hxx>

#include <QObject>
#include <QHash>
#include <QByteArray>

//! \class DFBrowserPane_TDataStdTreeNodeModel
//! \brief It builds a tree of items for the given tree node attribute.
class DFBrowserPane_TDataStdTreeNodeModel : public TreeModel_ModelBase
{
  Q_OBJECT
public:

  //! Constructor
  Standard_EXPORT DFBrowserPane_TDataStdTreeNodeModel (QObject* theParent);

  //! Destructor
  virtual ~DFBrowserPane_TDataStdTreeNodeModel() Standard_OVERRIDE {};

  //! Initializes the tree model by the attribute
  //! \param theAttribute a current attribute
  Standard_EXPORT void SetAttribute (const Handle(TDF_Attribute)& theAttribute);

  //! Returns model index by the attribute. The method is recursive.
  //! \param theAttribute an attribute that is searched
  //! \param theParentIndex an index of the parent item
  //! \return the model index
  QModelIndex FindIndex (const Handle(TDF_Attribute)& theAttribute, const QModelIndex theParentIndex);

  //! Returns count of columns in the model.
  //! \param theParent an index of the parent item
  //! \return integer value
  virtual int columnCount (const QModelIndex& theParent = QModelIndex()) const Standard_OVERRIDE
  { (void)theParent; return 1; }

private:

  Handle(TDF_Attribute) myAttribute; //! the parent attribute
};

#endif
