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

#ifndef DFBrowser_ThreadItemSearch_H
#define DFBrowser_ThreadItemSearch_H

#include <inspector/DFBrowser_ThreadItem.hxx>
#include <inspector/DFBrowser_SearchLineModel.hxx> // to include DFBrowser_SearchItemInfo

#include <Standard.hxx>
#include <TDF_Label.hxx>

#include <QAbstractItemModel>
#include <QMap>
#include <QStringList>

class DFBrowser_Module;
class DFBrowser_SearchLine;
class DFBrowser_SearchLineModel;

//! \class DFBrowser_ThreadItemSearch
//! Iterates by OCAF application to fill internal containers with information necessary for search
class DFBrowser_ThreadItemSearch : public DFBrowser_ThreadItem
{
public:

  //! Constructor
  DFBrowser_ThreadItemSearch (DFBrowser_SearchLine* theSearchLine) : mySearchLine (theSearchLine) {}

  //! Destructor
  virtual ~DFBrowser_ThreadItemSearch() {}

  //! Obtains the curent OCAF application from the current module, iterates by all documents to fill
  //! internal containers for search functionality
  Standard_EXPORT virtual void Run() Standard_OVERRIDE;

  //! Set filled containers into search line
  Standard_EXPORT virtual void ApplyValues() Standard_OVERRIDE;

  //! Clear search line values
  Standard_EXPORT static void ClearValues (DFBrowser_SearchLine* theSearchLine);

private:

  //! Fills information containers by iterating through sub-labels and attributes
  //! The method is recursive by labels.
  //! \parm theLabel a current label, the label information is got and children and attributes information
  //! \param theCurrentPath it contains the current path to the label (stores in container)
  //! \param theValues container of document item values
  //! \param theInfoValues container of document item values
  void getLabelLines (const TDF_Label& theLabel, QStringList& theCurrentPath,
                      QMap<QString, DFBrowser_SearchItemInfo >& theValues, QStringList& theInfoValues);

  //! Adds label information into container: search will be performed by label entry
  //! \parm theLabel a current label
  //! \param theCurrentPath it contains the current path to the label (stores in container)
  //! \param theValues container of document item values
  //! \param theInfoValues container of document item values
  void addLabel (const TDF_Label& theLabel, const QStringList& theCurrentPath,
                 QMap<QString, DFBrowser_SearchItemInfo>& theValues, QStringList& theInfoValues);

  //! Add attribute information, it is either attribute kind or attribure value for TDataStd_Name or TDataStd_Comment
  //! \parm theAttribute a current attribute
  //! \param theCurrentPath it contains the current path to the label (stores in container)
  //! \param theValues container of document item values
  //! \param theInfoValues container of document item values
  void addAttribute (const Handle(TDF_Attribute)& theAttribute, const QStringList& theCurrentPath,
                     QMap<QString, DFBrowser_SearchItemInfo>& theValues, QStringList& theInfoValues);

private:

  DFBrowser_SearchLine* mySearchLine; //!< class that should be filled by apply of the thread item
  //!< a document index to container of entry/attribute name to item information
  QMap<int, QMap<QString, DFBrowser_SearchItemInfo > > myDocumentValues;
  QMap<int, QStringList> myDocumentInfoValues; //!< a document index to entry/attribute name
};


#endif
