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

#include <NCollection_Array1.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <NCollection_List.hxx>
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
  //! @param[in] theOwner element to change selection state
  //! @param[in] theFilter context filter
  //! @param[in] theSelScheme selection scheme
  //! @param[in] theIsDetected flag of object detection
  //! @return result of selection
  Standard_EXPORT virtual AIS_SelectStatus Select(
    const occ::handle<SelectMgr_EntityOwner>& theOwner,
    const occ::handle<SelectMgr_Filter>&      theFilter,
    const AIS_SelectionScheme                 theSelScheme,
    const bool                                theIsDetected);

  //! the object is always add int the selection.
  //! faster when the number of objects selected is great.
  Standard_EXPORT virtual AIS_SelectStatus AddSelect(
    const occ::handle<SelectMgr_EntityOwner>& theObject);

  //! clears the selection and adds the object in the selection.
  //! @param[in] theObject element to change selection state
  //! @param[in] theFilter context filter
  //! @param[in] theIsDetected flag of object detection
  virtual void ClearAndSelect(const occ::handle<SelectMgr_EntityOwner>& theObject,
                              const occ::handle<SelectMgr_Filter>&      theFilter,
                              const bool                                theIsDetected)
  {
    Clear();
    Select(theObject, theFilter, AIS_SelectionScheme_Add, theIsDetected);
  }

  //! checks if the object is in the selection.
  bool IsSelected(const occ::handle<SelectMgr_EntityOwner>& theObject) const
  {
    return myResultMap.IsBound(theObject);
  }

  //! Return the list of selected objects.
  const NCollection_List<occ::handle<SelectMgr_EntityOwner>>& Objects() const { return myresult; }

  //! Return the number of selected objects.
  int Extent() const { return myresult.Size(); }

  //! Return true if list of selected objects is empty.
  bool IsEmpty() const { return myresult.IsEmpty(); }

public:
  //! Start iteration through selected objects.
  void Init()
  {
    myIterator = NCollection_List<occ::handle<SelectMgr_EntityOwner>>::Iterator(myresult);
  }

  //! Return true if iterator points to selected object.
  bool More() const { return myIterator.More(); }

  //! Continue iteration through selected objects.
  void Next() { myIterator.Next(); }

  //! Return selected object at iterator position.
  const occ::handle<SelectMgr_EntityOwner>& Value() const { return myIterator.Value(); }

  //! Select or deselect owners depending on the selection scheme.
  //! @param[in] thePickedOwners elements to change selection state
  //! @param[in] theSelScheme selection scheme, defines how owner is selected
  //! @param[in] theToAllowSelOverlap selection flag, if true - overlapped entities are allowed
  //! @param[in] theFilter context filter to skip not acceptable owners
  Standard_EXPORT virtual void SelectOwners(
    const NCollection_Array1<occ::handle<SelectMgr_EntityOwner>>& thePickedOwners,
    const AIS_SelectionScheme                                     theSelScheme,
    const bool                                                    theToAllowSelOverlap,
    const occ::handle<SelectMgr_Filter>&                          theFilter);

protected:
  //! Append the owner into the current selection if filter is Ok.
  //! @param[in] theOwner  element to change selection state
  //! @param[in] theFilter  context filter to skip not acceptable owners
  //! @return result of selection
  Standard_EXPORT virtual AIS_SelectStatus appendOwner(
    const occ::handle<SelectMgr_EntityOwner>& theOwner,
    const occ::handle<SelectMgr_Filter>&      theFilter);

protected:
  NCollection_List<occ::handle<SelectMgr_EntityOwner>>           myresult;
  NCollection_List<occ::handle<SelectMgr_EntityOwner>>::Iterator myIterator;
  NCollection_DataMap<occ::handle<SelectMgr_EntityOwner>,
                      NCollection_List<occ::handle<SelectMgr_EntityOwner>>::Iterator>
    myResultMap;
};

#endif // _AIS_Selection_HeaderFile
