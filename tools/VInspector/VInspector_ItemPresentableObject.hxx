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

#ifndef VInspector_ItemPresentableObject_H
#define VInspector_ItemPresentableObject_H

#include <Standard.hxx>
#include <inspector/VInspector_ItemBase.hxx>

#include <AIS_InteractiveObject.hxx>
#include <NCollection_List.hxx>

class QItemSelectionModel;

class VInspector_ItemPresentableObject;
typedef QExplicitlySharedDataPointer<VInspector_ItemPresentableObject> VInspector_ItemPresentableObjectPtr;

//! \class VInspector_ItemPresentableObject
//! Item presents information about AIS_InteractiveObject.
//! Parent is item context, children are item selections.
class VInspector_ItemPresentableObject : public VInspector_ItemBase
{

public:

  //! Creates an item wrapped by a shared pointer
  static VInspector_ItemPresentableObjectPtr CreateItem (TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn)
  { return VInspector_ItemPresentableObjectPtr (new VInspector_ItemPresentableObject (theParent, theRow, theColumn)); }
  //! Destructor
  virtual ~VInspector_ItemPresentableObject() Standard_OVERRIDE {};

  //! Returns the current interactive object, init item if it was not initialized yet
  //! \return interactive object
  Standard_EXPORT Handle(AIS_InteractiveObject) GetInteractiveObject() const;

  //! Returns pointer information for the current interactive object, init item if it was not initialized yet
  //! \return string value
  Standard_EXPORT QString PointerInfo() const;

  //! Inits the item, fills internal containers
  Standard_EXPORT virtual void Init() Standard_OVERRIDE;

  //! Resets cached values
  Standard_EXPORT virtual void Reset() Standard_OVERRIDE;

  //! Returns presentations, which items are selected in tree view
  //! \param theSelectionModel a selection model
  //! \return container of presentations
  Standard_EXPORT static NCollection_List<Handle(AIS_InteractiveObject)> GetSelectedPresentations
    (QItemSelectionModel* theSelectionModel);

protected:

  //! Initialize the current item. It is empty because Reset() is also empty.
  virtual void initItem() const Standard_OVERRIDE;

  //! Returns number of item selected
  //! \return rows count
  virtual int initRowCount() const Standard_OVERRIDE;

  //! Returns item information for the given role. Fills internal container if it was not filled yet
  //! \param theItemRole a value role
  //! \return the value
  virtual QVariant initValue (const int theItemRole) const Standard_OVERRIDE;

  //! Creates a child item in the given position.
  //! \param theRow the child row position
  //! \param theColumn the child column position
  //! \return the created item
  virtual TreeModel_ItemBasePtr createChild (int theRow, int theColumn) Standard_OVERRIDE;

private:

  //! Set interactive object into the current field
  //! \param theIO a presentation
  void setInteractiveObject (Handle(AIS_InteractiveObject) theIO) { myIO = theIO; }

private:

  //! Constructor
  //! param theParent a parent item
  VInspector_ItemPresentableObject(TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn)
  : VInspector_ItemBase(theParent, theRow, theColumn) {}

protected:

  Handle(AIS_InteractiveObject) myIO; //!< the current interactive context
};

#endif
