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

#ifndef DFBROWSER_ThreadItemUsedShapesMap_H
#define DFBROWSER_ThreadItemUsedShapesMap_H

#include <inspector/DFBrowser_ThreadItem.hxx>

#include <NCollection_DataMap.hxx>
#include <Standard.hxx>
#include <TCollection_AsciiString.hxx>
#include <TDF_Attribute.hxx>

#ifdef _MSC_VER
#pragma warning(disable : 4127) // conditional expression is constant
#endif
#include <QStringList>
#include <QPair>

class DFBrowser_Module;

//! \class DFBrowser_ThreadItemUsedShapesMap
//! Collects and sorts map values of TNaming_UsedShape attributes. Applies values to the OCAF tree view model
class DFBrowser_ThreadItemUsedShapesMap : public DFBrowser_ThreadItem
{
public:

  //! Constructor
  DFBrowser_ThreadItemUsedShapesMap() : myModule (0) {}

  //! Destructor
  virtual ~DFBrowser_ThreadItemUsedShapesMap() {}

  //! Sets the current module
  void SetModule (DFBrowser_Module* theModule) { myModule = theModule; }

  //! Iterates by application documents and for TNaming_UsedShape attribute collects container of sorted values.
  Standard_EXPORT virtual void Run() Standard_OVERRIDE;

  //! Initialize tree view model of OCAF by internal container of sorted values.
  Standard_EXPORT virtual void ApplyValues() Standard_OVERRIDE;

  //! Set empty cache into tree view model of OCAF. Calls Init of these item by usual values (not sorted)
  Standard_EXPORT static void ClearSortedReferences (DFBrowser_Module* theModule);

private:

  //! Collects sorted values of the attribute if the attribute has TNaming_UsedShape type
  //! \param theAttribute a processed attribute
  //! \param theReferences a output container of sorted references
  void findReferences (Handle(TDF_Attribute) theAttribute, std::list<TCollection_AsciiString>& theReferences);

  //! Checks whether the left path is less than the right path
  //! \param theLeft path to label
  //! \param theRight path to label
  //! \returns true if the left path is less(not equal) than right path
  static bool isLessThan (const QStringList& theLeft, const QStringList& theRight);

  //! Insert entry into container of entries using sorting. It is split by ":" separator to perform soring
  //! \param theEntry a label entry
  //! \param theEntries a result container
  void addValue (const TCollection_AsciiString& theEntry, QList<QPair<TCollection_AsciiString, QStringList > >& theEntries);

private:

  DFBrowser_Module* myModule; //!< the current module
  NCollection_DataMap<Handle(TDF_Attribute), std::list<TCollection_AsciiString> > myAttributeRefs; //!< sorted references
};

#endif
