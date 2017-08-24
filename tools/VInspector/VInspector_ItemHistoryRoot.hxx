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

#ifndef VInspector_ItemHistoryRoot_H
#define VInspector_ItemHistoryRoot_H

#include <Standard.hxx>
#include <inspector/VInspector_ItemBase.hxx>
#include <inspector/VInspector_ItemHistoryTypeInfo.hxx>
#include <inspector/VInspector_CallBackMode.hxx>

class VInspector_ItemHistoryRoot;
typedef QExplicitlySharedDataPointer<VInspector_ItemHistoryRoot> VInspector_ItemHistoryRootPtr;

//! \class VInspector_ItemHistoryRoot
//! Item in history tree model. Has container of VInspector_ItemHistoryTypeInfo. Each element is a child item.
//! Parent item is NULL, children are ItemHistoryType items 
class VInspector_ItemHistoryRoot : public VInspector_ItemBase
{

public:

  //! Creates an item wrapped by a shared pointer
  static VInspector_ItemHistoryRootPtr CreateItem (TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn)
  { return VInspector_ItemHistoryRootPtr (new VInspector_ItemHistoryRoot (theParent, theRow, theColumn)); }

  //! Destructor
  virtual ~VInspector_ItemHistoryRoot() Standard_OVERRIDE {};

  //! Sets maximum count of history elements in internal container.
  //! \param theSize a size value
  void SetHistoryTypesMaxAmount (const int theSize) { myInfoMaxSize = theSize; }

  //! Appends new history element into internal container. If the container already has maximum number
  //! of values, the first container is removed.
  //! \param theMode a type of the element
  //! \param theInfo a history information element
  Standard_EXPORT void AddElement (const VInspector_CallBackMode& theMode, const QList<QVariant>& theInfo);

  //! Returns history item information for the child item
  //! \param theChildRowIndex a child index 
  Standard_EXPORT const VInspector_ItemHistoryTypeInfo& GetTypeInfo (const int theChildRowIndex);

protected:

  //! Return data value for the role. It should be reimplemented in child
  //! \param theRole a value role
  //! \return the value
  virtual QVariant initValue(const int theRole) const;

  //! \return number of children. It should be reimplemented in child
  virtual int initRowCount() const Standard_OVERRIDE { return myInfoMap.size(); }

  //! Creates a child item in the given position.
  //! \param theRow the child row position
  //! \param theColumn the child column position
  //! \return the created item
  virtual TreeModel_ItemBasePtr createChild (int theRow, int theColumn) Standard_OVERRIDE;

private:

  //! Constructor
  //! param theParent a parent item
  VInspector_ItemHistoryRoot(TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn);

private:

  QMap<int, VInspector_ItemHistoryTypeInfo> myInfoMap; //!< container of whole callback info, bounded by maximum size value
  int myFirstIndex; //!< the first index in the information map, the first index is incremented if max value of container is achieved 
  int myLastIndex; //!< the last index in the information map, is incremented when new element is added
  int myInfoMaxSize; //!< maximum number of elements in information map
};

#endif
