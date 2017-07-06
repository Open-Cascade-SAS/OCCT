// Created on: 1995-02-13
// Created by: Mister rmi
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

#ifndef _SelectMgr_SelectionManager_HeaderFile
#define _SelectMgr_SelectionManager_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TColStd_MapOfTransient.hxx>
#include <SelectMgr_DataMapOfObjectSelectors.hxx>
#include <Standard_Transient.hxx>
#include <SelectMgr_ViewerSelector.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <SelectMgr_TypeOfUpdate.hxx>
class SelectMgr_SelectableObject;


class SelectMgr_SelectionManager;
DEFINE_STANDARD_HANDLE(SelectMgr_SelectionManager, Standard_Transient)

//! A framework to manage selection from the point of
//! view of viewer selectors. These can be added and
//! removed, and selection modes can be activated and
//! deactivated. In addition, objects may be known to all
//! selectors or only to some.
class SelectMgr_SelectionManager : public Standard_Transient
{

public:

  
  //! Constructs an empty selection manager object.
  Standard_EXPORT SelectMgr_SelectionManager();
  
  //! Adds the viewer selector theSelector to the list of known items.
  Standard_EXPORT void Add (const Handle(SelectMgr_ViewerSelector)& theSelector);
  
  //! Removes viewer selector theSelector from the list of known items.
  Standard_EXPORT void Remove (const Handle(SelectMgr_ViewerSelector)& theSelector);
  
  //! Returns true if the manager contains the viewer selector theSelector in a list of known items.
  Standard_EXPORT Standard_Boolean Contains (const Handle(SelectMgr_ViewerSelector)& theSelector) const;
  
  //! Returns true if the manager contains the selectable object theObject.
  Standard_EXPORT Standard_Boolean Contains (const Handle(SelectMgr_SelectableObject)& theObject) const;
  
  //! Loads and computes selection mode theMode (if it is not equal to -1) in global context and adds selectable
  //! object to BVH tree. If the object theObject has an already calculated selection with mode theMode and it was removed,
  //! the selection will be recalculated.
  Standard_EXPORT void Load (const Handle(SelectMgr_SelectableObject)& theObject, const Standard_Integer theMode = -1);
  
  //! Loads and computes selection mode theMode (if it is not equal to -1) and adds selectable object to BVH tree.
  //! Does not perform check of existance of theObject in global context before addition, but adds theSelector to local context.
  Standard_EXPORT void Load (const Handle(SelectMgr_SelectableObject)& theObject, const Handle(SelectMgr_ViewerSelector)& theSelector, const Standard_Integer theMode = -1);
  
  //! Removes selectable object theObject from all viewer selectors it was added to previously, removes it from all contexts
  //! and clears all computed selections of theObject.
  Standard_EXPORT void Remove (const Handle(SelectMgr_SelectableObject)& theObject);
  
  //! Removes theObject from theSelector, does not clear selections and unbind theObject from context maps.
  Standard_EXPORT void Remove (const Handle(SelectMgr_SelectableObject)& theObject, const Handle(SelectMgr_ViewerSelector)& theSelector);
  
  //! Activates the selection mode theMode in the selector theSelector for the selectable object anObject.
  //! By default, theMode is equal to 0. If theSelector is set to default (NULL), the selection with the mode theMode
  //! will be activated in all the viewers available.
  Standard_EXPORT void Activate (const Handle(SelectMgr_SelectableObject)& theObject, const Standard_Integer theMode = 0, const Handle(SelectMgr_ViewerSelector)& theSelector = NULL);
  
  //! Deactivates mode theMode of theObject in theSelector. If theMode value is set to default (-1), all
  //! avtive selection modes will be deactivated. Likewise, if theSelector value is set to default (NULL), theMode
  //! will be deactivated in all viewer selectors.
  Standard_EXPORT void Deactivate (const Handle(SelectMgr_SelectableObject)& theObject, const Standard_Integer theMode = -1, const Handle(SelectMgr_ViewerSelector)& theSelector = NULL);
  
  //! Returns true if the selection with theMode is active for the selectable object theObject and selector theSelector.
  //! If all parameters are set to default values, it returns it there is any active selection in any known viewer selector for
  //! object theObject.
  Standard_EXPORT Standard_Boolean IsActivated (const Handle(SelectMgr_SelectableObject)& theObject, const Standard_Integer theMode = -1, const Handle(SelectMgr_ViewerSelector)& theSelector = NULL) const;
  
  //! Removes sensitive entities from all viewer selectors
  //! after method Clear() was called to the selection they belonged to
  //! or it was recomputed somehow.
  Standard_EXPORT void ClearSelectionStructures (const Handle(SelectMgr_SelectableObject)& theObj, const Standard_Integer theMode = -1, const Handle(SelectMgr_ViewerSelector)& theSelector = NULL);
  
  //! Re-adds newely calculated sensitive  entities of recomputed selection
  //! defined by mode theMode to all viewer selectors contained that selection.
  Standard_EXPORT void RestoreSelectionStructures (const Handle(SelectMgr_SelectableObject)& theObj, const Standard_Integer theMode = -1, const Handle(SelectMgr_ViewerSelector)& theSelector = NULL);
  
  //! Recomputes activated selections of theObject for all known viewer selectors according to theMode specified.
  //! If theMode is set to default (-1), then all activated selections will be recomputed. If theIsForce is set to true,
  //! then selection mode theMode for object theObject will be recomputed regardless of its activation status.
  Standard_EXPORT void RecomputeSelection (const Handle(SelectMgr_SelectableObject)& theObject, const Standard_Boolean theIsForce = Standard_False, const Standard_Integer theMode = -1);
  
  //! Updates all selections of theObject in all viewer selectors according to its current update status.
  //! If theIsForce is set to true, the call is equal to recomputation.
  Standard_EXPORT void Update (const Handle(SelectMgr_SelectableObject)& theObject, const Standard_Boolean theIsForce = Standard_True);
  
  //! Updates all selections of theObject in specified viewer selector according to its current update status.
  //! If theIsForce is set to true, the call is equal to recomputation.
  Standard_EXPORT void Update (const Handle(SelectMgr_SelectableObject)& theObject, const Handle(SelectMgr_ViewerSelector)& theSelector, const Standard_Boolean theIsForce = Standard_True);
  
  //! Sets type of update of all selections of theObject to the given theType.
  Standard_EXPORT void SetUpdateMode (const Handle(SelectMgr_SelectableObject)& theObject, const SelectMgr_TypeOfUpdate theType);
  
  //! Sets type of update of selection with theMode of theObject to the given theType.
  Standard_EXPORT void SetUpdateMode (const Handle(SelectMgr_SelectableObject)& theObject, const Standard_Integer theMode, const SelectMgr_TypeOfUpdate theType);

  //! Allows to manage sensitivity of a particular selection of interactive object theObject and
  //! changes previous sensitivity value of all sensitive entities in selection with theMode
  //! to the given theNewSensitivity.
  Standard_EXPORT void SetSelectionSensitivity (const Handle(SelectMgr_SelectableObject)& theObject,
                                                const Standard_Integer theMode,
                                                const Standard_Integer theNewSens);

  //! Re-adds selectable object in BVHs in all viewer selectors.
  Standard_EXPORT void UpdateSelection (const Handle(SelectMgr_SelectableObject)& theObj);

  DEFINE_STANDARD_RTTIEXT(SelectMgr_SelectionManager,Standard_Transient)

protected:

  //! Recomputes given selection mode and updates BVHs in all viewer selectors
  Standard_EXPORT void recomputeSelectionMode (const Handle(SelectMgr_SelectableObject)& theObject,
                                               const Handle(SelectMgr_Selection)& theSelection,
                                               const Standard_Integer theMode);


private:

  
  //! Loads and creates selection structures for object theObject with mode theMode in specified
  //! viewer selector theSelector. If theSelector is set to default value (NULL), the selection mode
  //! created will be added to all known viewer selectors.
  Standard_EXPORT void loadMode (const Handle(SelectMgr_SelectableObject)& theObject, const Standard_Integer theMode, const Handle(SelectMgr_ViewerSelector)& theSelector = NULL);
  
  //! Internal function that marks 1st level BVH of the object theObj as
  //! outdated.
  Standard_EXPORT void rebuildSelectionStructures (const Handle(SelectMgr_ViewerSelector)& theSelector = NULL);

  TColStd_MapOfTransient mySelectors;
  TColStd_MapOfTransient myGlobal;
  SelectMgr_DataMapOfObjectSelectors myLocal;


};







#endif // _SelectMgr_SelectionManager_HeaderFile
