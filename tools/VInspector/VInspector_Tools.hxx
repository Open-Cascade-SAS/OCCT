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
#include <SelectMgr_EntityOwner.hxx>
#include <Standard.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Shape.hxx>

#include <inspector/VInspector_CallBackMode.hxx>
#include <inspector/VInspector_SelectionType.hxx>

#ifdef _MSC_VER
#pragma warning(disable : 4127) // conditional expression is constant
#endif

#include <QList>
#include <QVariant>

//! \class VInspector_Tools
//! The class that gives auxiliary methods for Visualization elements manipulation
class VInspector_Tools
{

public:

  //! Returns string value of enumeration value
  //! \param theType a shape type
  //! \return text value
  Standard_EXPORT static TCollection_AsciiString GetShapeTypeInfo (const TopAbs_ShapeEnum& theType);

  //! Convert pointer to string value
  //! \param thePointer a pointer
  //! \param isShortInfo if true, all '0' symbols in the beginning of the pointer are skipped
  //! \return the string value 
  Standard_EXPORT static TCollection_AsciiString GetPointerInfo (const Handle(Standard_Transient)& thePointer,
                                                                 const bool isShortInfo);

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
                                               const Handle(SelectBasics_EntityOwner)& theOwner);

  //! Returns all owners present in the context
  //! \param theContext an interactive context
  //! \return container of owners
  Standard_EXPORT static NCollection_List<Handle(SelectBasics_EntityOwner)> ContextOwners (
                                                 const Handle(AIS_InteractiveContext)& theContext);

  //! Returns active owners in main selector of context
  //! \param theContext an interactive context
  //! \param theEmptySelectableOwners container of owners with NULL presentation or not displayed presentation
  //! \return container of owners
  Standard_EXPORT static NCollection_List<Handle(SelectBasics_EntityOwner)> ActiveOwners (
                            const Handle(AIS_InteractiveContext)& theContext,
                            NCollection_List<Handle(SelectBasics_EntityOwner)>& theEmptySelectableOwners);

  //! Unhighlight selected, set selected the owners
  //! \param theContext an interactive context
  //! \param theOwners a container of owners
  Standard_EXPORT static void AddOrRemoveSelectedShapes (const Handle(AIS_InteractiveContext)& theContext,
                                         const NCollection_List<Handle(SelectBasics_EntityOwner)>& theOwners);

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

  //! Returns string information of call back mode
  //! \param theMode type of selection
  //! \param theValue a value in selection enumeration
  //! \return information text
  Standard_EXPORT static TCollection_AsciiString ToName (const VInspector_SelectionType theType, const int theValue);

  //! Returns selection information 
  //! \param theMode a selection mode
  //! \param thePresentation a presentation
  //! \return text value
  Standard_EXPORT static TCollection_AsciiString SelectionModeToName (int theMode, const Handle(AIS_InteractiveObject)& thePresentation);

  //! Returns text of orientation
  //! \param theOrientation an orientation value
  //! \return text value
  Standard_EXPORT static TCollection_AsciiString OrientationToName (const TopAbs_Orientation& theOrientation);

  //! Returns text of orientation
  //! \param theLocation a location value
  //! \return text value
  Standard_EXPORT static TCollection_AsciiString LocationToName (const TopLoc_Location& theLocation);

  //! Read Shape using BREP reader
  //! \param theFileName a file name
  //! \return shape or NULL
  Standard_EXPORT static TopoDS_Shape ReadShape (const TCollection_AsciiString& theFileName);
};

#endif
