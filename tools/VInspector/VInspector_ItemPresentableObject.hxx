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
typedef QExplicitlySharedDataPointer<VInspector_ItemPresentableObject>
  VInspector_ItemPresentableObjectPtr;

//! \class VInspector_ItemPresentableObject
//! Item presents information about AIS_InteractiveObject.
//! Parent is item context, children are item selections.
class VInspector_ItemPresentableObject : public VInspector_ItemBase
{

public:
  //! Creates an item wrapped by a shared pointer
  static VInspector_ItemPresentableObjectPtr CreateItem(TreeModel_ItemBasePtr theParent,
                                                        const int             theRow,
                                                        const int             theColumn)
  {
    return VInspector_ItemPresentableObjectPtr(
      new VInspector_ItemPresentableObject(theParent, theRow, theColumn));
  }

  //! Destructor
  virtual ~VInspector_ItemPresentableObject() {}

  //! Returns data object of the item.
  //! \return object
  virtual const Handle(Standard_Transient)& Object() const Standard_OVERRIDE
  {
    initItem();
    return myIO;
  }

  //! Returns the current interactive object, init item if it was not initialized yet
  //! \return interactive object
  Handle(AIS_InteractiveObject) GetInteractiveObject() const
  {
    return Handle(AIS_InteractiveObject)::DownCast(Object());
  }

  //! Returns pointer information for the current interactive object, init item if it was not
  //! initialized yet \return string value
  Standard_EXPORT QString PointerInfo() const;

  //! Inits the item, fills internal containers
  Standard_EXPORT virtual void Init() Standard_OVERRIDE;

  //! Resets cached values
  Standard_EXPORT virtual void Reset() Standard_OVERRIDE;

  //! Returns presentation of the attribute to be visualized in the view
  //! \thePresentations[out]  container of presentation handles to be visualized
  Standard_EXPORT virtual void Presentations(
    NCollection_List<Handle(Standard_Transient)>& thePresentations) Standard_OVERRIDE;

protected:
  //! Initialize the current item. It is empty because Reset() is also empty.
  virtual void initItem() const Standard_OVERRIDE;

  //! Returns number of item selected
  //! \return rows count
  virtual int initRowCount() const Standard_OVERRIDE;

  //! Returns item information for the given role. Fills internal container if it was not filled yet
  //! \param theItemRole a value role
  //! \return the value
  virtual QVariant initValue(const int theItemRole) const Standard_OVERRIDE;

  //! Returns stream value of the item to fulfill property panel.
  //! \return stream value or dummy
  Standard_EXPORT virtual void initStream(Standard_OStream& theOStream) const Standard_OVERRIDE;

protected:
  //! Build presentation shape
  //! \return generated shape of the item parameters
  virtual TopoDS_Shape buildPresentationShape() Standard_OVERRIDE;

  //! Set interactive object into the current field
  //! \param theIO a presentation
  void setInteractiveObject(Handle(AIS_InteractiveObject) theIO) { myIO = theIO; }

private:
  //! Constructor
  //! \param theParent a parent item
  VInspector_ItemPresentableObject(TreeModel_ItemBasePtr theParent,
                                   const int             theRow,
                                   const int             theColumn)
      : VInspector_ItemBase(theParent, theRow, theColumn)
  {
  }

protected:
  Handle(AIS_InteractiveObject) myIO; //!< the current interactive context
};

#endif
