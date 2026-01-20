// Created on: 1998-02-23
// Created by: Christian CAILLET
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _IFSelect_EditForm_HeaderFile
#define _IFSelect_EditForm_HeaderFile

#include <Standard.hxx>

#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_Sequence.hxx>
#include <TCollection_HAsciiString.hxx>
#include <NCollection_HSequence.hxx>
class IFSelect_Editor;
class Interface_InterfaceModel;
class IFSelect_ListEditor;
class TCollection_HAsciiString;

//! An EditForm is the way to apply an Editor on an Entity or on
//! the Model
//! It gives read-only or read-write access, with or without undo
//!
//! It can be complete (all the values of the Editor are present)
//! or partial (a sub-list of these value are present)
//! Anyway, all references to Number (argument <num>) refer to
//! Number of Value for the Editor
//! While references to Rank are for rank in the EditForm, which
//! may differ if it is not Complete
//! Two methods give the correspondence between this Number and
//! the Rank in the EditForm : RankFromNumber and NumberFromRank
class IFSelect_EditForm : public Standard_Transient
{
public:
  //! Creates a complete EditForm from an Editor
  //! A specific Label can be given
  Standard_EXPORT IFSelect_EditForm(const occ::handle<IFSelect_Editor>& editor,
                                    const bool                          readonly,
                                    const bool                          undoable,
                                    const char* const                   label = "");

  //! Creates an extracted EditForm from an Editor, limited to
  //! the values identified in <nums>
  //! A specific Label can be given
  Standard_EXPORT IFSelect_EditForm(const occ::handle<IFSelect_Editor>& editor,
                                    const NCollection_Sequence<int>&    nums,
                                    const bool                          readonly,
                                    const bool                          undoable,
                                    const char* const                   label = "");

  //! Returns and may change the keep status on modif
  //! It starts as False
  //! If it is True, Apply does not clear modification status
  //! and the EditForm can be loaded again, modified value remain
  //! and may be applied again
  //! Remark that ApplyData does not clear the modification status,
  //! a call to ClearEdit does
  Standard_EXPORT bool& EditKeepStatus();

  Standard_EXPORT const char* Label() const;

  //! Tells if the EditForm is loaded now
  Standard_EXPORT bool IsLoaded() const;

  Standard_EXPORT void ClearData();

  Standard_EXPORT void SetData(const occ::handle<Standard_Transient>&       ent,
                               const occ::handle<Interface_InterfaceModel>& model);

  Standard_EXPORT void SetEntity(const occ::handle<Standard_Transient>& ent);

  Standard_EXPORT void SetModel(const occ::handle<Interface_InterfaceModel>& model);

  Standard_EXPORT occ::handle<Standard_Transient> Entity() const;

  Standard_EXPORT occ::handle<Interface_InterfaceModel> Model() const;

  Standard_EXPORT occ::handle<IFSelect_Editor> Editor() const;

  //! Tells if an EditForm is complete or is an extract from Editor
  Standard_EXPORT bool IsComplete() const;

  //! Returns the count of values
  //! <editable> True : count of editable values, i.e.
  //! For a complete EditForm, it is given by the Editor
  //! Else, it is the length of the extraction map
  //! <editable> False : all the values from the Editor
  Standard_EXPORT int NbValues(const bool editable) const;

  //! Returns the Value Number in the Editor from a given Rank in
  //! the EditForm
  //! For a complete EditForm, both are equal
  //! Else, it is given by the extraction map
  //! Returns 0 if <rank> exceeds the count of editable values,
  Standard_EXPORT int NumberFromRank(const int rank) const;

  //! Returns the Rank in the EditForm from a given Number of Value
  //! for the Editor
  //! For a complete EditForm, both are equal
  //! Else, it is given by the extraction map
  //! Returns 0 if <number> is not forecast to be edited, or is
  //! out of range
  Standard_EXPORT int RankFromNumber(const int number) const;

  //! Returns the Value Number in the Editor for a given Name
  //! i.e. the true ValueNumber which can be used in various methods
  //! of EditForm
  //! If it is not complete, for a recorded (in the Editor) but
  //! non-loaded name, returns negative value (- number)
  Standard_EXPORT int NameNumber(const char* const name) const;

  //! Returns the Rank of Value in the EditForm for a given Name
  //! i.e. if it is not complete, for a recorded (in the Editor) but
  //! non-loaded name, returns 0
  Standard_EXPORT int NameRank(const char* const name) const;

  //! For a read-write undoable EditForm, loads original values
  //! from defaults stored in the Editor
  Standard_EXPORT void LoadDefault();

  //! Loads modifications to data
  //! Default uses Editor. Can be redefined
  //! Remark that <ent> and/or <model> may be null, according to the
  //! kind of Editor. Shortcuts are available for these cases, but
  //! they finally call LoadData (hence, just ignore non-used args)
  Standard_EXPORT virtual bool LoadData(const occ::handle<Standard_Transient>&       ent,
                                        const occ::handle<Interface_InterfaceModel>& model);

  //! Shortcut for LoadData when <model> is not used
  Standard_EXPORT bool LoadEntity(const occ::handle<Standard_Transient>& ent);

  //! Shortcut for LoadData when only the model is concerned
  Standard_EXPORT bool LoadModel(const occ::handle<Interface_InterfaceModel>& model);

  //! Shortcut when both <ent> and <model> are not used
  //! (when the Editor works on fully static or global data)
  Standard_EXPORT bool LoadData();

  //! Returns a ListEditor to edit the parameter <num> of the
  //! EditForm, if it is a List
  //! The Editor created it (by ListEditor) then loads it (by
  //! ListValue)
  //! For a single parameter, returns a Null Handle ...
  Standard_EXPORT occ::handle<IFSelect_ListEditor> ListEditor(const int num) const;

  //! Loads an original value (single). Called by the Editor only
  Standard_EXPORT void LoadValue(const int num, const occ::handle<TCollection_HAsciiString>& val);

  //! Loads an original value as a list. Called by the Editor only
  Standard_EXPORT void LoadList(
    const int                                                                        num,
    const occ::handle<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>>& list);

  //! From an edited value, returns its ... value (original one)
  //! Null means that this value is not defined
  //! <num> is for the EditForm, not the Editor
  //! It is for a single parameter. For a list, gives a Null Handle
  Standard_EXPORT occ::handle<TCollection_HAsciiString> OriginalValue(const int num) const;

  //! Returns an original value, as a list
  //! <num> is for the EditForm, not the Editor
  //! For a single parameter, gives a Null Handle
  Standard_EXPORT occ::handle<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>>
                  OriginalList(const int num) const;

  //! Returns the Edited (i.e. Modified) Value (string for single)
  //! <num> reports to the EditForm
  //! If IsModified is False, returns OriginalValue
  //! Null with IsModified True : means that this value is not
  //! defined or has been removed
  //! It is for a single parameter. For a list, gives a Null Handle
  Standard_EXPORT occ::handle<TCollection_HAsciiString> EditedValue(const int num) const;

  //! Returns the Edited Value as a list
  //! If IsModified is False, returns OriginalValue
  //! Null with IsModified True : means that this value is not
  //! defined or has been removed
  //! For a single parameter, gives a Null Handle
  Standard_EXPORT occ::handle<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>>
                  EditedList(const int num) const;

  //! Tells if a Value (of the EditForm) is modified (directly or
  //! through touching by Update)
  Standard_EXPORT bool IsModified(const int num) const;

  //! Tells if a Value (of the EditForm) has been touched, i.e.
  //! not modified directly but by the modification of another one
  //! (by method Update from the Editor)
  Standard_EXPORT bool IsTouched(const int num) const;

  //! Gives a new value for the item <num> of the EditForm, if
  //! it is a single parameter (for a list, just returns False)
  //! Null means to Remove it
  //! <enforce> True to overpass Protected or Computed Access Mode
  //! Calls the method Update from the Editor, which can touch other
  //! parameters (see NbTouched)
  //! Returns True if well recorded, False if this value is not
  //! allowed
  //! Warning : Does not apply immediately : will be applied by the method
  //! Apply
  Standard_EXPORT bool Modify(const int                                    num,
                              const occ::handle<TCollection_HAsciiString>& newval,
                              const bool                                   enforce = false);

  //! Changes the value of an item of the EditForm, if it is a List
  //! (else, just returns False)
  //! The ListEditor contains the edited values of the list
  //! If no edition was recorded, just returns False
  //! Calls the method Update from the Editor, which can touch other
  //! parameters (see NbTouched)
  //! Returns True if well recorded, False if this value is not
  //! allowed
  //! Warning : Does not apply immediately : will be applied by the method
  //! Apply
  Standard_EXPORT bool ModifyList(const int                               num,
                                  const occ::handle<IFSelect_ListEditor>& edited,
                                  const bool                              enforce = false);

  //! As ModifyList but the new value is given as such
  //! Creates a ListEditor, Loads it, then calls ModifyList
  Standard_EXPORT bool ModifyListValue(
    const int                                                                        num,
    const occ::handle<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>>& list,
    const bool enforce = false);

  //! Gives a new value computed by the Editor, if another parameter
  //! commands the value of <num>
  //! It is generally the case for a Computed Parameter for instance
  //! Increments the counter of touched parameters
  //! Warning : it gives no protection for ReadOnly etc... while it is the
  //! internal way of touching parameters
  //! Does not work (returns False) if <num> is for a list
  Standard_EXPORT bool Touch(const int num, const occ::handle<TCollection_HAsciiString>& newval);

  //! Acts as Touch but for a list
  //! Does not work (returns False) if <num> is for a single param
  Standard_EXPORT bool TouchList(
    const int                                                                        num,
    const occ::handle<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>>& newlist);

  //! Returns the count of parameters touched by the last Modify
  //! (apart from the modified parameter itself)
  //! Normally it is zero
  Standard_EXPORT int NbTouched() const;

  //! Clears modification status : by default all, or one by its
  //! numbers (in the Editor)
  Standard_EXPORT void ClearEdit(const int num = 0);

  //! Prints Definitions, relative to the Editor
  Standard_EXPORT void PrintDefs(Standard_OStream& S) const;

  //! Prints Values, according to what and alsolist
  //! <names> True : prints Long Names; False : prints Short Names
  //! <what> < 0 : prints Original Values (+ flag Modified)
  //! <what> > 0 : prints Final Values (+flag Modified)
  //! <what> = 0 : prints Modified Values (Original + Edited)
  //! <alsolist> False (D) : lists are printed only as their count
  //! <alsolist> True : lists are printed for all their items
  Standard_EXPORT void PrintValues(Standard_OStream& S,
                                   const int         what,
                                   const bool        names,
                                   const bool        alsolist = false) const;

  //! Applies modifications to own data
  //! Calls ApplyData then Clears Status according EditKeepStatus
  Standard_EXPORT bool Apply();

  //! Tells if this EditForm can work with its Editor and its actual
  //! Data (Entity and Model)
  //! Default uses Editor. Can be redefined
  Standard_EXPORT virtual bool Recognize() const;

  //! Applies modifications to data
  //! Default uses Editor. Can be redefined
  Standard_EXPORT virtual bool ApplyData(const occ::handle<Standard_Transient>&       ent,
                                         const occ::handle<Interface_InterfaceModel>& model);

  //! For an undoable EditForm, Applies ... origibal values !
  //! and clears modified ones
  //! Can be run only once
  Standard_EXPORT bool Undo();

  DEFINE_STANDARD_RTTIEXT(IFSelect_EditForm, Standard_Transient)

private:
  bool                                                thecomplete;
  bool                                                theloaded;
  bool                                                thekeepst;
  TCollection_AsciiString                             thelabel;
  NCollection_Array1<int>                             thenums;
  NCollection_Array1<occ::handle<Standard_Transient>> theorigs;
  NCollection_Array1<occ::handle<Standard_Transient>> themodifs;
  NCollection_Array1<int>                             thestatus;
  occ::handle<IFSelect_Editor>                        theeditor;
  occ::handle<Standard_Transient>                     theent;
  occ::handle<Interface_InterfaceModel>               themodel;
  int                                                 thetouched;
};

#endif // _IFSelect_EditForm_HeaderFile
