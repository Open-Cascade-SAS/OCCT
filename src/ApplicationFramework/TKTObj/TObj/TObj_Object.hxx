// Created on: 2004-11-22
// Created by: Pavel TELKOV
// Copyright (c) 2004-2014 OPEN CASCADE SAS
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

// The original implementation Copyright: (C) RINA S.p.A

#ifndef TObj_Object_HeaderFile
#define TObj_Object_HeaderFile

#include <TObj_DeletingMode.hxx>
#include <TObj_SequenceOfObject.hxx>

#include <TDF_Label.hxx>
#include <gp_XYZ.hxx>

class TObj_Model;
class TObj_Persistence;
class TObj_ObjectIterator;
class TObj_TNameContainer;
class TCollection_HExtendedString;
class TCollection_HAsciiString;

#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <TCollection_ExtendedString.hxx>

//! Basis class for transient objects in OCAF-based models

class TObj_Object : public Standard_Transient
{
public:
  enum TypeFlags //!< Flags that define type-specific behaviour of objects
  {
    Visible = 0x0001 //!< Is visible in DataViewer
  };

  //! enumeration describing various object state bit flags (see Set/GetFlags())
  enum ObjectState
  {
    ObjectState_Hidden = 0x0001,         //!< object is hidden in tree browser
    ObjectState_Saved  = 0x0002,         //!< object has (or should have)
                                         //!<   corresponding saved file on disk
    ObjectState_Imported       = 0x0004, //!< object's data are just imported from somewhere
    ObjectState_ImportedByFile = 0x0008, //!< a model imported by file may need a
                                         //!<   sophisticated update of external references
    ObjectState_Ordered = 0x0010         //!< flag that partition contains ordered objects
  };

protected:
  //! enumeration for the ranks of label under Data section.
  enum DataTag
  {
    DataTag_First = 0,
    DataTag_Flags,                     //!< stores flags of object
    DataTag_Order,                     //!< stores order of object
    DataTag_Last = DataTag_First + 100 //!< Reserved for possible future use
  };

  //! enumeration for the ranks of label under Child section.
  enum ChildTag
  {
    ChildTag_First = 0,
    ChildTag_Last  = ChildTag_First //!< No children
  };

  //! enumeration for the ranks of label under Reference section.
  enum RefTag
  {
    RefTag_First = 0,
    RefTag_Last  = RefTag_First //!< No children
  };

protected:
  /**
   * Constructors
   */

  //! Constructor of a new object interface: requires label,
  //! which should be one of the labels in model's data structure.
  //! This creates a new object and attaches it to a given label.
  //! The initialisation of the object's data and their integrity is
  //! to be ensured by implementations and by persistence mechanism.
  //! If the flag theSetName is true (default) the object is assign the default name
  //! that is generated using the father partition object if any.
  Standard_EXPORT TObj_Object(const TDF_Label& theLabel, const bool theSetName = true);

  //! The special constructor intended for implementation of persistence
  //! mechanism. See class TObj_Persistence for more details.
  //! The first argument is used only to avoid confusion with
  //! other constructors.
  TObj_Object(const TObj_Persistence*, const TDF_Label& theLabel)
      : myLabel(theLabel)
  {
  }

  //! This method is called from persistence to initialize the object fields,
  //! so successors that have transient fields must initialize them in initFields(),
  //! and call the same method of parent.
  void initFields() {}

public:
  /**
   * Virtual methods
   */

  //! Returns the model to which the object belongs
  virtual Standard_EXPORT occ::handle<TObj_Model> GetModel() const;

  //! Returns iterator for the child objects.
  //! This method provides tree-like view of the objects hierarchy.
  //! The references to other objects are not considered as children.
  //! theType narrows a variety of iterated objects
  //! The default implementation search for children on 1 sublavel
  //! of the children sub label
  virtual Standard_EXPORT occ::handle<TObj_ObjectIterator> GetChildren(
    const occ::handle<Standard_Type>& theType = NULL) const;

  //! Returns the label under which children are stored
  Standard_EXPORT TDF_Label GetChildLabel() const;

  //! Returns the label for child with rank
  Standard_EXPORT TDF_Label getChildLabel(const int theRank) const;

public:
  /**
   * Access to the OCAF-specific data
   */

  //! Returns the OCAF label on which object`s data are stored
  Standard_EXPORT TDF_Label GetLabel() const;

  //! Returns the label which is the root for data OCAF sub-tree
  Standard_EXPORT TDF_Label GetDataLabel() const;

  //! Returns the label which is the root for reference OCAF sub-tree
  Standard_EXPORT TDF_Label GetReferenceLabel() const;

public:
  /**
   * Methods handling name of the object
   */

  //! Returns the map of names of the objects
  //! Default implementation returns global Dictionary of the model
  virtual Standard_EXPORT occ::handle<TObj_TNameContainer> GetDictionary() const;

  //! Returns the name of the object (empty string if object has no name)
  virtual Standard_EXPORT occ::handle<TCollection_HExtendedString> GetName() const;

  //! Returns the true is object has name and returns name to theName
  Standard_EXPORT bool GetName(TCollection_ExtendedString& theName) const;

  //! Returns the true is object has name and returns name to theName
  Standard_EXPORT bool GetName(TCollection_AsciiString& theName) const;

  //! Sets name of the object. Returns False if theName is not unique.
  virtual Standard_EXPORT bool SetName(
    const occ::handle<TCollection_HExtendedString>& theName) const;

  //! Sets name of the object. Returns False if theName is not unique.
  Standard_EXPORT bool SetName(const occ::handle<TCollection_HAsciiString>& theName) const;

  //! Sets name of the object. Returns False if theName is not unique.
  Standard_EXPORT bool SetName(const char* name) const;

  //! Returns name for copy
  //! default implementation returns the same name
  virtual occ::handle<TCollection_HExtendedString> GetNameForClone(
    const occ::handle<TObj_Object>&) const
  {
    return GetName();
  }

public:
  /**
   * Analysis of dependencies on other objects
   */

  //! Returns True if object has reference to indicated object
  virtual Standard_EXPORT bool HasReference(const occ::handle<TObj_Object>& theObject) const;

  //! Returns an Iterator containing objects that compose the this one
  //! theType narrows a variety of iterated objects
  virtual Standard_EXPORT occ::handle<TObj_ObjectIterator> GetReferences(
    const occ::handle<Standard_Type>& theType = NULL) const;

  //! Remove all references to other objects, by removing all reference attributes
  virtual Standard_EXPORT void RemoveAllReferences();

  //! Returns iterator for the objects which depend on this one.
  //! These referring objects may belong to other models.
  //! theType narrows a variety of iterated objects
  virtual Standard_EXPORT occ::handle<TObj_ObjectIterator> GetBackReferences(
    const occ::handle<Standard_Type>& theType = NULL) const;

  //! Registers another object as being dependent on this one.
  //! Stores back references under sublabel 2 (purely transient data,
  //! not subject to persistency).
  virtual Standard_EXPORT void AddBackReference(const occ::handle<TObj_Object>& theObject);

  //! Removes information on dependent object (back reference).
  //! If theSingleOnly is true only the first back reference is removed in the
  //! case of duplicate items.
  virtual Standard_EXPORT void RemoveBackReference(const occ::handle<TObj_Object>& theObject,
                                                   const bool theSingleOnly = true);

  //! Removes all back reference by removing references from other to me.
  virtual Standard_EXPORT bool RemoveBackReferences(
    const TObj_DeletingMode theMode = TObj_FreeOnly);

  //! The default implementation just clear the back references container
  virtual Standard_EXPORT void ClearBackReferences();

  //! Returns TRUE if object has 1 or more back references
  Standard_EXPORT bool HasBackReferences() const;

  //! Replace reference from old object to new object.
  //! If it is not possible, may raise exception.
  //! If new object is null then simple remove reference to old object.
  virtual Standard_EXPORT void ReplaceReference(const occ::handle<TObj_Object>& theOldObject,
                                                const occ::handle<TObj_Object>& theNewObject);

  //! Return True if this refers to the model theRoot belongs
  //! to and a referred label is not a descendant of theRoot.
  //! In this case theBadReference returns the currently referred label.
  virtual Standard_EXPORT bool GetBadReference(const TDF_Label& theRoot,
                                               TDF_Label&       theBadReference) const;

  //! Make that each reference pointing to a descendant label of
  //! theFromRoot to point to an equivalent label under theToRoot.
  //! Return False if a resulting reference does not point to
  //! an TObj_Object
  //! Example:
  //! a referred object label = 0:3:24:7:2:7
  //! theFromRoot             = 0:3:24
  //! theToRoot               = 0:2
  //! a new referred label    = 0:2:7:2:7
  virtual Standard_EXPORT bool RelocateReferences(const TDF_Label& theFromRoot,
                                                  const TDF_Label& theToRoot,
                                                  const bool       theUpdateBackRefs = true);

  //! Returns True if the referred object theObject can be deleted
  //! without deletion of this object.
  //! Default implementation does nothing and returns False.
  virtual Standard_EXPORT bool CanRemoveReference(const occ::handle<TObj_Object>& theObject) const;

  //! Removes reference to the object by replace reference to NULL object
  virtual Standard_EXPORT void RemoveReference(const occ::handle<TObj_Object>& theObject);

  //! Invokes from TObj_TReference::BeforeForget().
  //! theLabel - label on that reference become removed
  //! Default implementation is empty
  virtual void BeforeForgetReference(const TDF_Label& /*theLabel*/) {}

public:
  /**
   * Methods for deleting the object
   */

  //! Checks if object can be detached with specified mode
  virtual Standard_EXPORT bool CanDetach(const TObj_DeletingMode theMode = TObj_FreeOnly);

  //! Deletes the object from the model.
  //! The dependent objects are either deleted or modified when possible
  //! (see description of TObj_DeletingMode enumeration for more details)
  //! Returns True if deletion was successful.
  //! Checks if object can be deleted.
  //! Should be redefined for each specific kind of object
  virtual Standard_EXPORT bool Detach(const TObj_DeletingMode theMode = TObj_FreeOnly);

  //! Deletes the object from the label. Checks if object can be deleted.
  //! Finds object on the label and detaches it by calling previous method.
  //! Returns true if there is no object on the label after detaching
  static Standard_EXPORT bool Detach(const TDF_Label&        theLabel,
                                     const TObj_DeletingMode theMode = TObj_FreeOnly);

public:
  /**
   * methods for object retrieval
   */

  //! Returns the Object attached to a given label.
  //! Returns False if no object of type TObj_Object is stored on the
  //! specified label.
  //! If isSuper is true tries to find on the super labels.
  static Standard_EXPORT bool GetObj(const TDF_Label&          theLabel,
                                     occ::handle<TObj_Object>& theResult,
                                     const bool                isSuper = false);

  //! Returns the father object, which may be NULL
  //! theType gives type of father object to search
  Standard_EXPORT occ::handle<TObj_Object> GetFatherObject(
    const occ::handle<Standard_Type>& theType = NULL) const;

public:
  /**
   * Methods for checking and Updating object
   */

  //! Checks that object alive in model
  //! Default implementation checks that object has TObject attribute at own label.
  virtual Standard_EXPORT bool IsAlive() const;

public:
  /**
   * Cloning related methods
   */

  //! Copy me to other label theTargetLabel
  //! New object will not have all the reference that has me.
  //! Coping object with data and childs, but change name by adding string "_copy"
  //! As result return handle of new object (null handle is something wrong)
  //! NOTE: BackReferences not coping.
  //! After cloning all objects it is necessary to call copy references
  //! with the same relocation table
  virtual Standard_EXPORT occ::handle<TObj_Object> Clone(
    const TDF_Label&                 theTargetLabel,
    occ::handle<TDF_RelocationTable> theRelocTable = 0);

  //! Coping the references.
  //! return false is Target object is different type
  virtual Standard_EXPORT void CopyReferences(
    const occ::handle<TObj_Object>&         theTargetObject,
    const occ::handle<TDF_RelocationTable>& theRelocTable);

  //! Coping the children from source label to the target.
  virtual Standard_EXPORT void CopyChildren(TDF_Label&                              theTargetLabel,
                                            const occ::handle<TDF_RelocationTable>& theRelocTable);

public:
  /**
   * Public methods to access order of object
   */

  //! returns order of object (or tag of their label if order is not initialised)
  virtual Standard_EXPORT int GetOrder() const;

  //! sets order of object
  virtual Standard_EXPORT bool SetOrder(const int& theIndx);

public:
  /**
   * Public methods to check modifications of the object since last commit
   */
  //! Returns true if object attributes or its children were modified in the current open
  //! transaction
  Standard_EXPORT bool HasModifications() const;

protected:
  /**
   * Protected Methods copy data of object to other object
   */

  //! Coping the data of me to Target object.
  //! return false is Target object is different type
  virtual Standard_EXPORT bool copyData(const occ::handle<TObj_Object>& theTargetObject);

  //! Coping the references from source label to the target.
  Standard_EXPORT void copyReferences(const TDF_Label&                        theSourceLabel,
                                      TDF_Label&                              theTargetLabel,
                                      const occ::handle<TDF_RelocationTable>& theRelocTable);

public:
  /**
   * Access to object flags
   */

  //! Returns flags (bitmask) that define properties of objects of that type
  //! By default returns flag Visible
  virtual Standard_EXPORT int GetTypeFlags() const;

  //! Returns mask of seted flags
  Standard_EXPORT int GetFlags() const;

  //! Sets flags with defined mask.
  Standard_EXPORT void SetFlags(const int theMask);

  //! tests flags by the mask.
  Standard_EXPORT bool TestFlags(const int theMask) const;

  //! clears flags by the mask.
  Standard_EXPORT void ClearFlags(const int theMask = ~0);

public:
  /**
   * Method for updating object after restoring
   */

  //! Performs updating the links and dependences of the object which are not
  //! stored in persistence. Should be redefined if necessary.
  virtual Standard_EXPORT void AfterRetrieval();

  //! Performs storing the objects transient fields in OCAF document
  //! which were outside transaction mechanism.
  //! Default implementation does nothing
  virtual Standard_EXPORT void BeforeStoring();

protected:
  /**
   * Internal tools for work with OCAF data
   */

  //! Returns the theRank2-th sub label of the theRank1-th sublabel of the
  //! Data label of the object.
  //! If theRank2 is 0 (default), sub label theRank1 of Data label is returned.
  //! If requested label does not exist, it is created.
  Standard_EXPORT TDF_Label getDataLabel(const int theRank1, const int theRank2 = 0) const;

  //! Returns the theRank2-th sub label of the theRank1-th sublabel of the
  //! Reference label of the object.
  //! If theRank2 is 0 (default), sub label theRank1 of Reference label is returned.
  //! If requested label does not exist, it is created.
  Standard_EXPORT TDF_Label getReferenceLabel(const int theRank1, const int theRank2 = 0) const;

  //! Returns True if there is an attribute having theGUID on the
  //! theRank2-th sublabel of theRank1-th sublabel of the Data
  //! label of the object.
  //! If theRank2 is 0 (default), label theRank1 is supposed, not
  //! its sublabel
  Standard_EXPORT bool isDataAttribute(const Standard_GUID& theGUID,
                                       const int            theRank1,
                                       const int            theRank2 = 0) const;

  //! Returns the real value from theRank2-th sublabel of theRank1-th
  //! sublabel of the Data label of the object.
  //! If theRank2 is 0 (default), label theRank1 is supposed (not its sublabel).
  //! Returns 0.0 if no real value is stored on that label.
  Standard_EXPORT double getReal(const int theRank1, const int theRank2 = 0) const;

  //! Sets the real value for theRank2-th sublabel of theRank1-th
  //! sublabel of the Data label of the object.
  //! If theRank2 is 0 (default), label theRank1 is supposed (not its sublabel).
  //! Returns True if new value is actually different from previous one
  //! (with specified tolerance)
  Standard_EXPORT bool setReal(const double theValue,
                               const int    theRank1,
                               const int    theRank2     = 0,
                               const double theTolerance = 0.) const;

  //! Returns the integer value from theRank2-th sublabel of theRank1-th
  //! sublabel of the Data label of the object.
  //! If theRank2 is 0 (default), label theRank1 is supposed (not its sublabel).
  //! Returns 0 if no integer value is stored on that label.
  Standard_EXPORT int getInteger(const int theRank1, const int theRank2 = 0) const;

  //! Sets the integer value for theRank2-th sublabel of theRank1-th
  //! sublabel of the Data label of the object.
  //! If theRank2 is 0 (default), label theRank1 is supposed (not its sublabel).
  //! Returns True if new value is actually different from previous one
  Standard_EXPORT bool setInteger(const int theValue,
                                  const int theRank1,
                                  const int theRank2 = 0) const;

  //! Returns an existing or create a new real array on theRank2-th
  //! sublabel of theRank1-th sublabel of the Data label of the object.
  //! If theRank2 is 0 (default), label theRank1 is supposed (not its sublabel).
  //! A newly created array has 1 and theLength bounds and is initialized
  //! with theInitialValue
  //!
  //! NOTE: do not create new array and returns NULL handle if no array on label
  //!       and theLength less than
  //!
  //! WARNING: call setArray() after array contents modification
  //! in order to assure Undo work
  Standard_EXPORT occ::handle<NCollection_HArray1<double>> getRealArray(
    const int    theLength,
    const int    theRank1,
    const int    theRank2        = 0,
    const double theInitialValue = 0.0) const;

  //! Returns an existing or create a new integer array on theRank2-th
  //! sublabel of theRank1-th sublabel of the Data label of the object.
  //! If theRank2 is 0 (default), label theRank1 is supposed (not its sublabel).
  //! A newly created array has 1 and theLength bounds and is initialized
  //! with theInitialValue
  //!
  //! NOTE: do not create new array and returns NULL handle if no array on label
  //!       and theLength less than
  //!
  //! WARNING: call setArray() after array contents modification
  //! in order to assure Undo work
  Standard_EXPORT occ::handle<NCollection_HArray1<int>> getIntegerArray(
    const int theLength,
    const int theRank1,
    const int theRank2        = 0,
    const int theInitialValue = 0) const;

  //! Returns an existing or create a new string array on theRank2-th
  //! sublabel of theRank1-th sublabel of the Data label of the object.
  //! If theRank2 is 0 (default), label theRank1 is supposed (not its sublabel).
  //! A newly created array has 1 and theLength bounds
  //! NOTE: new created array is NOT initialized.
  //!
  //! NOTE: do not create new array and returns NULL handle if no array on label
  //!       and theLength less than
  //!
  //! WARNING: call setArray() after array contents modification
  //! in order to assure Undo work
  Standard_EXPORT occ::handle<NCollection_HArray1<TCollection_ExtendedString>> getExtStringArray(
    const int theLength,
    const int theRank1,
    const int theRank2 = 0) const;

  //! Store theArray on theRank2-th sublabel of theRank1-th sublabel
  //! of the Data label of the object.
  //! If theRank2 is 0 (default), label theRank1 is supposed (not its sublabel).
  //! If theArray is null then array attribute if any is removed.
  Standard_EXPORT void setArray(const occ::handle<NCollection_HArray1<double>>& theArray,
                                const int                                       theRank1,
                                const int                                       theRank2 = 0);

  //! Store theArray on theRank2-th sublabel of theRank1-th sublabel
  //! of the Data label of the object.
  //! If theRank2 is 0 (default), label theRank1 is supposed (not its sublabel).
  //! If theArray is null then array attribute if any is removed.
  Standard_EXPORT void setArray(const occ::handle<NCollection_HArray1<int>>& theArray,
                                const int                                    theRank1,
                                const int                                    theRank2 = 0);

  //! Store theArray on theRank2-th sublabel of theRank1-th sublabel
  //! of the Data label of the object.
  //! If theRank2 is 0 (default), label theRank1 is supposed (not its sublabel).
  //! If theArray is null then array attribute if any is removed.
  Standard_EXPORT void setArray(
    const occ::handle<NCollection_HArray1<TCollection_ExtendedString>>& theArray,
    const int                                                           theRank1,
    const int                                                           theRank2 = 0);

  //! Sets the string value for theRank2-th sublabel of theRank1-th
  //! sublabel of the Data label of the object.
  //! If theRank2 is 0 (default), label theRank1 is supposed (not its sublabel).
  Standard_EXPORT void setExtString(const occ::handle<TCollection_HExtendedString>& theValue,
                                    const int                                       theRank1,
                                    const int theRank2 = 0) const;

  //! Returns the string value from theRank2-th sublabel of theRank1-th
  //! sublabel of the Data label of the object.
  //! If theRank2 is 0 (default), label theRank1 is supposed (not its sublabel).
  Standard_EXPORT occ::handle<TCollection_HExtendedString> getExtString(
    const int theRank1,
    const int theRank2 = 0) const;

  //! Sets the ascii string value for theRank2-th sublabel of theRank1-th
  //! sublabel of the Data label of the object.
  //! If theRank2 is 0 (default), label theRank1 is supposed (not its sublabel).
  Standard_EXPORT void setAsciiString(const occ::handle<TCollection_HAsciiString>& theValue,
                                      const int                                    theRank1,
                                      const int theRank2 = 0) const;

  //! Returns the string value from theRank2-th sublabel of theRank1-th
  //! sublabel of the Data label of the object.
  //! If theRank2 is 0 (default), label theRank1 is supposed (not its sublabel).
  Standard_EXPORT occ::handle<TCollection_HAsciiString> getAsciiString(
    const int theRank1,
    const int theRank2 = 0) const;

  //! Returns the reference attribute from theRank2-th sublabel of theRank1-th
  //! sublabel of the References label of the object.
  //! If theRank2 is 0 (default), label theRank1 is supposed (not its sublabel).
  Standard_EXPORT occ::handle<TObj_Object> getReference(const int theRank1,
                                                        const int theRank2 = 0) const;

  //! Sets the reference to theObject on theRank2-th sublabel of theRank1-th
  //! sublabel of the References label of the object.
  //! If theRank2 is 0 (default), label theRank1 is supposed (not its sublabel).
  //! Returns True if new value is actually different from previous one
  //! If Object is NULL, reference is deleted
  Standard_EXPORT bool setReference(const occ::handle<TObj_Object>& theObject,
                                    const int                       theRank1,
                                    const int                       theRank2 = 0);

  //! Adds the reference to theObject on next free sublabel of theRank1-th
  //! sublabel of the References label of the object and returns the Label.
  Standard_EXPORT TDF_Label addReference(const int                       theRank1,
                                         const occ::handle<TObj_Object>& theObject);

private:
  /**
   * Fields
   */
  TDF_Label myLabel; //!< Label on which object`s data are stored

  occ::handle<NCollection_HSequence<occ::handle<TObj_Object>>>
    myHSeqBackRef; //!< hsequence of back references.

  friend class TObj_TObject;

public:
  //! CASCADE RTTI
  DEFINE_STANDARD_RTTIEXT(TObj_Object, Standard_Transient)
};

//! Define handle in separate file

#endif

#ifdef _MSC_VER
#pragma once
#endif
