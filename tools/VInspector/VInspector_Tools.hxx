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

#ifndef VInspector_Tools_H
#define VInspector_Tools_H

#include <AIS_InteractiveContext.hxx>
#include <Bnd_Box.hxx>
#include <Bnd_OBB.hxx>
#include <Graphic3d_Buffer.hxx>
#include <Graphic3d_Mat4.hxx>
#include <Graphic3d_Mat4d.hxx>
#include <Select3D_BndBox3d.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <Standard.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Shape.hxx>

#include <inspector/View_DisplayActionType.hxx>

#include <inspector/TreeModel_ItemBase.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QList>
#include <QVariant>
#include <Standard_WarningsRestore.hxx>

class ViewControl_TableModelValues;

class Graphic3d_IndexBuffer;
class Graphic3d_Buffer;
class Graphic3d_BoundBuffer;

//! \class VInspector_Tools
//! The class that gives auxiliary methods for Visualization elements manipulation
class VInspector_Tools
{

public:

  //! Returns string value of enumeration value
  //! \param theType a shape type
  //! \return text value
  Standard_EXPORT static TCollection_AsciiString GetShapeTypeInfo (const TopAbs_ShapeEnum& theType);

  //! Returns number of selected owners for presentation
  //! \param theContext an interactive context
  //! \param theObject a presentation
  //! \param theShapeInfoOnly if true, only BRep owners are taken
  Standard_EXPORT static int SelectedOwners (const Handle(AIS_InteractiveContext)& theContext,
                                             const Handle(AIS_InteractiveObject)& theObject,
                                             const bool theShapeInfoOnly);

  //! Returns true if the owner is selected in the context
  //! \param theContext an interactive context
  //! \param theOwner a selectable owner
  //! \return boolean value
  Standard_EXPORT static bool IsOwnerSelected (const Handle(AIS_InteractiveContext)& theContext,
                                               const Handle(SelectMgr_EntityOwner)& theOwner);

  //! Returns all owners present in the context
  //! \param theContext an interactive context
  //! \return container of owners
  Standard_EXPORT static NCollection_List<Handle(SelectMgr_EntityOwner)> ContextOwners (
                                                 const Handle(AIS_InteractiveContext)& theContext);

  //! Returns active owners in main selector of context
  //! \param theContext an interactive context
  //! \param theEmptySelectableOwners container of owners with NULL presentation or not displayed presentation
  //! \return container of owners
  Standard_EXPORT static NCollection_List<Handle(SelectMgr_EntityOwner)> ActiveOwners (
                            const Handle(AIS_InteractiveContext)& theContext,
                            NCollection_List<Handle(SelectMgr_EntityOwner)>& theEmptySelectableOwners);

  //! Unhighlight selected, set selected the owners
  //! \param theContext an interactive context
  //! \param theOwners a container of owners
  Standard_EXPORT static void AddOrRemoveSelectedShapes (const Handle(AIS_InteractiveContext)& theContext,
                                         const NCollection_List<Handle(SelectMgr_EntityOwner)>& theOwners);

  //! Unhighlight selected, set selected presentations
  //! \param theContext an interactive context
  //! \param thePresentations a container of presentations
  Standard_EXPORT static void AddOrRemovePresentations (const Handle(AIS_InteractiveContext)& theContext,
                                        const NCollection_List<Handle(AIS_InteractiveObject)>& thePresentations);

  //! Returns information about presentation: Dynamic Type, Pointer info, Shape type info
  //! \param theObject a presentation
  //! \return container of values
  Standard_EXPORT static QList<QVariant> GetInfo (Handle(AIS_InteractiveObject)& theObject);

  //! Returns information about current highlight: Names, Owners, Pointers, Owners
  //! \param theContext an interactive context
  //! \return container of values
  Standard_EXPORT static QList<QVariant> GetHighlightInfo (const Handle(AIS_InteractiveContext)& theContext);

  //! Returns information about current selection: Names, Owners, Pointers, Owners
  //! \param theContext an interactive context
  //! \return container of values
  Standard_EXPORT static QList<QVariant> GetSelectedInfo (const Handle(AIS_InteractiveContext)& theContext);

  //! Returns the first pointer of selection in the context
  Standard_EXPORT static QString GetSelectedInfoPointers (const Handle(AIS_InteractiveContext)& theContext);

  //! Returns the string name for a given type.
  //! @param theType action type
  //! @return string identifier from the display action type
  Standard_EXPORT static Standard_CString DisplayActionTypeToString (View_DisplayActionType theType);

  //! Returns the enumeration type from the given string identifier (using case-insensitive comparison).
  //! @param theTypeString string identifier
  //! @return string identifier from the display action type
  static View_DisplayActionType DisplayActionTypeFromString (Standard_CString theTypeString)
  {
    View_DisplayActionType aType = View_DisplayActionType_NoneId;
    DisplayActionTypeFromString (theTypeString, aType);
    return aType;
  }

  //! Determines the enumeration type from the given string identifier (using case-insensitive comparison).
  //! @param theTypeString string identifier
  //! @param theType detected action type
  //! @return TRUE if string identifier is known
  Standard_EXPORT static Standard_Boolean DisplayActionTypeFromString (Standard_CString theTypeString,
                                                                       View_DisplayActionType& theType);
};

#endif
