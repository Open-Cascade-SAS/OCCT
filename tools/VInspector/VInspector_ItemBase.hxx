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

#ifndef VInspector_ItemBase_H
#define VInspector_ItemBase_H

#include <AIS_InteractiveContext.hxx>
#include <Standard.hxx>
#include <TopoDS_Shape.hxx>

#include <inspector/TreeModel_ColumnType.hxx>
#include <inspector/TreeModel_ItemBase.hxx>

class VInspector_ItemBase;
typedef QExplicitlySharedDataPointer<VInspector_ItemBase> VInspector_ItemBasePtr;

//! \class VInspector_ItemBase
//! Parent item for all ShapeView items
class VInspector_ItemBase : public TreeModel_ItemBase
{
public:
  //! Resets cached values
  Standard_EXPORT virtual void Reset() Standard_OVERRIDE;

  //! Sets the context 
  //! \param theLabel an object where the child items structure is found
  void SetContext (const Handle(AIS_InteractiveContext)& theContext) { myContext = theContext; }

  //! Returns true if the current context is not null
  //! \return a boolean value
  bool HasContext() const { return !GetContext().IsNull(); }

  //! Returns the current contex. It iterates up by list of parents to found context item and return context
  //! \return a context
  Standard_EXPORT Handle(AIS_InteractiveContext) GetContext() const;

  //! Returns item information for the given role. Fills internal container if it was not filled yet
  //! \param theItemRole a value role
  //! \return the value
  Standard_EXPORT virtual QVariant initValue (const int theItemRole) const Standard_OVERRIDE;

  //! Returns presentation of the attribute to be visualized in the view
  //! \thePresentations [out] container of presentation handles to be visualized
  virtual void Presentations (NCollection_List<Handle(Standard_Transient)>& thePresentations)
  { (void)thePresentations; }

  //! Returns transform persistent of the item or NULL
  Handle(Graphic3d_TransformPers) TransformPersistence() const { return myTransformPersistence; }

  //! Returns shape of the item parameters
  //! \return generated shape of the item parameters
  Standard_EXPORT virtual TopoDS_Shape GetPresentationShape() const;

  //! Rebuild presentation shape if the item use it
  //! \return generated shape of the item parameters
  void UpdatePresentationShape() { myPresentationShape = buildPresentationShape(); }

protected:

  //! Build presentation shape
  //! \return generated shape of the item parameters
  virtual TopoDS_Shape buildPresentationShape() { return TopoDS_Shape(); }

protected:

  //! Constructor
  //! \param theParent a parent item
  //! \param theRow the item row position in the parent item
  //! \param theColumn the item column position in the parent item
  VInspector_ItemBase (TreeModel_ItemBasePtr theParent, const int theRow, const int theColumn)
    : TreeModel_ItemBase (theParent, theRow, theColumn), myContext (0) {}

protected:

  Handle(AIS_InteractiveContext) myContext; //!< the current context
  TopoDS_Shape myPresentationShape; //!< item presentation shape
  Handle(Graphic3d_TransformPers) myTransformPersistence; //!< item cached persistent
};

#endif