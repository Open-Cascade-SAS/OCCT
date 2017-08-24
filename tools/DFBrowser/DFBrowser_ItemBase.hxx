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

#ifndef DFBrowser_ItemBase_H
#define DFBrowser_ItemBase_H

#include <Standard.hxx>
#include <TDF_Label.hxx>

#include <inspector/TreeModel_ItemBase.hxx>

class DFBrowser_ItemBase;
class DFBrowser_Module;
typedef QExplicitlySharedDataPointer<DFBrowser_ItemBase> DFBrowser_ItemBasePtr;

//! \class DFBrowser_ItemBase
//! \brief Declaration of the tree model base item.
//! This item provide method to process a TDF label.
class DFBrowser_ItemBase : public TreeModel_ItemBase
{
public:

  //! Sets the module to have an access to attribute information
  //! \param theModule a current loaded application module
  void SetModule (DFBrowser_Module* theModule) { myModule = theModule; }

  //! Resets the cached item values
  Standard_EXPORT virtual void Reset() Standard_OVERRIDE;

  //! \return true if the current label is not null
  bool HasLabel() const { return !GetLabel().IsNull(); }

  //! \return the current label
  Standard_EXPORT virtual TDF_Label GetLabel() const;

  //! \return the current module
  DFBrowser_Module* GetModule() const { return myModule; }

  //! Change using of additional information in item. It it does not use additional info,
  //! it will not return extended text in initValue().
  //! \param theValue a new value
  //! \return the previous value
  Standard_EXPORT bool SetUseAdditionalInfo (const bool theValue);

  //! Returns the data stored under the given role for the current item
  //! \param theIndex the item model index
  //! \param theRole the item model role
  Standard_EXPORT virtual QVariant data (const QModelIndex& theIndex, int theRole) const Standard_OVERRIDE;

protected:

  //! Sets the item label
  //! \param theLabel an object where the child items structure is found
  void setLabel(TDF_Label theLabel) { myLabel = theLabel; }

  //! Returns if additional information is shown in item for Display and ToolTip values
  //! \return boolean value
  bool useAdditionalInfo() const { return myIsUseAdditionalInfo; }

  //! Returns sum of label children and attributes
  //! \return rows count
  virtual int initRowCount() const Standard_OVERRIDE;

  //! Returns label information like text, icon or background(if it contains TDataStd_Name attribute)
  //! \param theItemRole a value role
  //! \return the value
  virtual QVariant initValue (const int theItemRole) const Standard_OVERRIDE;

  //! Creates a child item in the given position.
  //! \param theRow the child row position
  //! \param theColumn the child column position
  //! \return the created item
  virtual TreeModel_ItemBasePtr createChild (int theRow, int theColumn) Standard_OVERRIDE;

  //! Initialize the current item. It creates a backup of the specific item information
  virtual void initItem() const {};

protected:

  //! Constructor
  //! param theParent a parent item
  //! \param theRow the item row positition in the parent item
  //! \param theColumn the item column positition in the parent item
  DFBrowser_ItemBase (TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn);

private:

  TDF_Label myLabel; //!< a label of the document, which contains child labels and attributes
  DFBrowser_Module* myModule; //!< the current module
  bool myIsUseAdditionalInfo; //!< if true, additional item info is shown in square brackets
};

#endif