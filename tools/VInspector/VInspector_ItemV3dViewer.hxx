// Created on: 2020-02-10
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2020 OPEN CASCADE SAS
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

#ifndef VInspector_ItemV3dViewer_H
#define VInspector_ItemV3dViewer_H

#include <Standard.hxx>
#include <inspector/VInspector_ItemBase.hxx>

#include <V3d_Viewer.hxx>

class VInspector_ItemV3dViewer;
typedef QExplicitlySharedDataPointer<VInspector_ItemV3dViewer> VInspector_ItemV3dViewerPtr;

//! \class VInspector_ItemV3dViewer
//! Parent item is context properties, that corresponds to AIS_InteractiveContext
class VInspector_ItemV3dViewer : public VInspector_ItemBase
{
public:

  //! Creates an item wrapped by a shared pointer
  static VInspector_ItemV3dViewerPtr CreateItem (TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn)
  { return VInspector_ItemV3dViewerPtr (new VInspector_ItemV3dViewer (theParent, theRow, theColumn)); }

  //! Destructor
  virtual ~VInspector_ItemV3dViewer() Standard_OVERRIDE {};

  //! Inits the item, fills internal containers
  Standard_EXPORT virtual void Init() Standard_OVERRIDE;

  //! Resets cached values
  Standard_EXPORT virtual void Reset() Standard_OVERRIDE;

  //! Returns data object of the item.
  //! \return object
  virtual const Handle(Standard_Transient)& Object() const Standard_OVERRIDE { initItem(); return myViewer; }

  //! Returns the current viewer, init item if it was not initialized yet
  //! \return interactive object
  Handle(V3d_Viewer) GetViewer() const { return Handle(V3d_Viewer)::DownCast (Object()); }

protected:
  //! Initializes the current item. It is empty because Reset() is also empty.
  virtual void initItem() const Standard_OVERRIDE;

  //! Returns number of displayed presentations
  //! \return rows count
  Standard_EXPORT virtual int initRowCount() const Standard_OVERRIDE;

  //! Returns item information for the given role. Fills internal container if it was not filled yet
  //! \param theItemRole a value role
  //! \return the value
  Standard_EXPORT virtual QVariant initValue (const int theItemRole) const Standard_OVERRIDE;

  //! Returns stream value of the item to fulfill property panel.
  //! \return stream value or dummy
  Standard_EXPORT virtual void initStream (Standard_OStream& theOStream) const Standard_OVERRIDE;

private:

  //! Constructor
  //! param theParent a parent item
  //! \param theRow the item row positition in the parent item
  //! \param theColumn the item column positition in the parent item
  VInspector_ItemV3dViewer(TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn)
    : VInspector_ItemBase(theParent, theRow, theColumn) {}

protected:

  Handle(V3d_Viewer) myViewer; //!< the current viewer
};

#endif
