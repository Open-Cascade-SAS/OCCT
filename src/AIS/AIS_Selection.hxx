// Created on: 1995-03-21
// Created by: Jean-Louis Frenkel
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _AIS_Selection_HeaderFile
#define _AIS_Selection_HeaderFile

#include <AIS_NArray1OfEntityOwner.hxx>
#include <AIS_NListOfEntityOwner.hxx>
#include <AIS_SelectionScheme.hxx>
#include <AIS_SelectStatus.hxx>
#include <Standard.hxx>
#include <Standard_Type.hxx>

class SelectMgr_Filter;

//! Class holding the list of selected owners.
class AIS_Selection : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(AIS_Selection, Standard_Transient)
public:

  //! creates a new selection.
  Standard_EXPORT AIS_Selection();
  
  //! removes all the object of the selection.
  Standard_EXPORT virtual void Clear();
  
  //! if the object is not yet in the selection, it will be added.
  //! if the object is already in the selection, it will be removed.
  Standard_EXPORT virtual AIS_SelectStatus Select (const Handle(SelectMgr_EntityOwner)& theObject);
  
  //! the object is always add int the selection.
  //! faster when the number of objects selected is great.
  Standard_EXPORT virtual AIS_SelectStatus AddSelect (const Handle(SelectMgr_EntityOwner)& theObject);

  //! clears the selection and adds the object in the selection.
  virtual void ClearAndSelect (const Handle(SelectMgr_EntityOwner)& theObject)
  {
    Clear();
    Select (theObject);
  }

  //! checks if the object is in the selection.
  Standard_Boolean IsSelected (const Handle(SelectMgr_EntityOwner)& theObject) const { return myResultMap.IsBound (theObject); }

  //! Return the list of selected objects.
  const AIS_NListOfEntityOwner& Objects() const { return myresult; }

  //! Return the number of selected objects.
  Standard_Integer Extent() const { return myresult.Size(); }

  //! Return true if list of selected objects is empty.
  Standard_Boolean IsEmpty() const { return myresult.IsEmpty(); }

public:

  //! Start iteration through selected objects.
  void Init() { myIterator = AIS_NListOfEntityOwner::Iterator(myresult); }

  //! Return true if iterator points to selected object.
  Standard_Boolean More() const { return myIterator.More(); }

  //! Continue iteration through selected objects.
  void Next() { myIterator.Next(); }

  //! Return selected object at iterator position.
  const Handle(SelectMgr_EntityOwner)& Value() const { return myIterator.Value(); }

  //! Select or deselect owners depending on the selection scheme.
  //! @param theOwners [in] elements to change selection state
  //! @param theSelScheme [in] selection scheme, defines how owner is selected
  //! @param theToAllowSelOverlap [in] selection flag, if true - overlapped entities are allowed
  //! @param theFilter [in] context filter to skip not acceptable owners
  Standard_EXPORT virtual void SelectOwners (const AIS_NArray1OfEntityOwner& thePickedOwners,
                                             const AIS_SelectionScheme theSelScheme,
                                             const Standard_Boolean theToAllowSelOverlap,
                                             const Handle(SelectMgr_Filter)& theFilter);

protected:

  //! Append the owner into the current selection if filter is Ok.
  //! @param theOwner [in] element to change selection state
  //! @param theFilter [in] context filter to skip not acceptable owners
  //! @return result of selection
  Standard_EXPORT virtual AIS_SelectStatus appendOwner (const Handle(SelectMgr_EntityOwner)& theOwner,
                                                        const Handle(SelectMgr_Filter)& theFilter);

protected:

  AIS_NListOfEntityOwner myresult;
  AIS_NListOfEntityOwner::Iterator myIterator;
  NCollection_DataMap<Handle(SelectMgr_EntityOwner), AIS_NListOfEntityOwner::Iterator> myResultMap;

};

DEFINE_STANDARD_HANDLE(AIS_Selection, Standard_Transient)

#endif // _AIS_Selection_HeaderFile
