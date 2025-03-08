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

#ifndef _IFSelect_Editor_HeaderFile
#define _IFSelect_Editor_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <TColStd_Array1OfTransient.hxx>
#include <TColStd_Array1OfAsciiString.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <Standard_Transient.hxx>
#include <IFSelect_EditValue.hxx>
#include <TColStd_HSequenceOfHAsciiString.hxx>
#include <TCollection_AsciiString.hxx>
#include <NCollection_DataMap.hxx>
class Interface_TypedValue;
class IFSelect_EditForm;
class TCollection_HAsciiString;
class IFSelect_ListEditor;
class Interface_InterfaceModel;

class IFSelect_Editor;
DEFINE_STANDARD_HANDLE(IFSelect_Editor, Standard_Transient)

//! An Editor defines a set of values and a way to edit them, on
//! an entity or on the model (e.g. on its header)
//!
//! Each Value is controlled by a TypedValue, with a number (it is
//! an Integer) and a name under two forms (complete and short)
//! and an edit mode
class IFSelect_Editor : public Standard_Transient
{

public:
  //! Sets a Typed Value for a given ident and short name, with an
  //! Edit Mode
  Standard_EXPORT void SetValue(const Standard_Integer              num,
                                const Handle(Interface_TypedValue)& typval,
                                const Standard_CString              shortname  = "",
                                const IFSelect_EditValue            accessmode = IFSelect_Editable);

  //! Sets a parameter to be a List
  //! max < 0 : not for a list (set when starting)
  //! max = 0 : list with no length limit (default for SetList)
  //! max > 0 : list limited to <max> items
  Standard_EXPORT void SetList(const Standard_Integer num, const Standard_Integer max = 0);

  //! Returns the count of Typed Values
  Standard_EXPORT Standard_Integer NbValues() const;

  //! Returns a Typed Value from its ident
  Standard_EXPORT Handle(Interface_TypedValue) TypedValue(const Standard_Integer num) const;

  //! Tells if a parameter is a list
  Standard_EXPORT Standard_Boolean IsList(const Standard_Integer num) const;

  //! Returns max length allowed for a list
  //! = 0 means : list with no limit
  //! < 0 means : not a list
  Standard_EXPORT Standard_Integer MaxList(const Standard_Integer num) const;

  //! Returns the name of a Value (complete or short) from its ident
  //! Short Name can be empty
  Standard_EXPORT Standard_CString Name(const Standard_Integer num,
                                        const Standard_Boolean isshort = Standard_False) const;

  //! Returns the edit mode of a Value
  Standard_EXPORT IFSelect_EditValue EditMode(const Standard_Integer num) const;

  //! Returns the number (ident) of a Value, from its name, short or
  //! complete. If not found, returns 0
  Standard_EXPORT Standard_Integer NameNumber(const Standard_CString name) const;

  Standard_EXPORT void PrintNames(Standard_OStream& S) const;

  Standard_EXPORT void PrintDefs(Standard_OStream&      S,
                                 const Standard_Boolean labels = Standard_False) const;

  //! Returns the MaxLength of, according to what :
  //! <what> = -1 : length of short names
  //! <what> =  0 : length of complete names
  //! <what> =  1 : length of values labels
  Standard_EXPORT Standard_Integer MaxNameLength(const Standard_Integer what) const;

  //! Returns the specific label
  Standard_EXPORT virtual TCollection_AsciiString Label() const = 0;

  //! Builds and Returns an EditForm, empty (no data yet)
  //! Can be redefined to return a specific type of EditForm
  Standard_EXPORT virtual Handle(IFSelect_EditForm) Form(
    const Standard_Boolean readonly,
    const Standard_Boolean undoable = Standard_True) const;

  //! Tells if this Editor can work on this EditForm and its content
  //! (model, entity ?)
  Standard_EXPORT virtual Standard_Boolean Recognize(
    const Handle(IFSelect_EditForm)& form) const = 0;

  //! Returns the value of an EditForm, for a given item
  //! (if not a list. for a list, a Null String may be returned)
  Standard_EXPORT virtual Handle(TCollection_HAsciiString) StringValue(
    const Handle(IFSelect_EditForm)& form,
    const Standard_Integer           num) const = 0;

  //! Returns a ListEditor for a parameter which is a List
  //! Default returns a basic ListEditor for a List, a Null Handle
  //! if <num> is not for a List. Can be redefined
  Standard_EXPORT virtual Handle(IFSelect_ListEditor) ListEditor(const Standard_Integer num) const;

  //! Returns the value of an EditForm as a List, for a given item
  //! If not a list, a Null Handle should be returned
  //! Default returns a Null Handle, because many Editors have
  //! no list to edit. To be redefined as required
  Standard_EXPORT virtual Handle(TColStd_HSequenceOfHAsciiString) ListValue(
    const Handle(IFSelect_EditForm)& form,
    const Standard_Integer           num) const;

  //! Loads original values from some data, to an EditForm
  //! Remark: <ent> may be Null, this means all <model> is concerned
  //! Also <model> may be Null, if no context applies for <ent>
  //! And both <ent> and <model> may be Null, for a full static
  //! editor
  Standard_EXPORT virtual Standard_Boolean Load(
    const Handle(IFSelect_EditForm)&        form,
    const Handle(Standard_Transient)&       ent,
    const Handle(Interface_InterfaceModel)& model) const = 0;

  //! Updates the EditForm when a parameter is modified
  //! I.E.  default does nothing, can be redefined, as follows :
  //! Returns True when done (even if does nothing), False in case
  //! of refuse (for instance, if the new value is not suitable)
  //! <num> is the rank of the parameter for the EDITOR itself
  //! <enforce> True means that protected parameters can be touched
  //!
  //! If a parameter commands the value of other ones, when it is
  //! modified, it is necessary to touch them by Touch from EditForm
  Standard_EXPORT virtual Standard_Boolean Update(const Handle(IFSelect_EditForm)&        form,
                                                  const Standard_Integer                  num,
                                                  const Handle(TCollection_HAsciiString)& newval,
                                                  const Standard_Boolean enforce) const;

  //! Acts as Update, but when the value is a list
  Standard_EXPORT virtual Standard_Boolean UpdateList(
    const Handle(IFSelect_EditForm)&               form,
    const Standard_Integer                         num,
    const Handle(TColStd_HSequenceOfHAsciiString)& newlist,
    const Standard_Boolean                         enforce) const;

  //! Applies modified values of the EditForm with some data
  //! Remark: <ent> may be Null, this means all <model> is concerned
  //! Also <model> may be Null, if no context applies for <ent>
  //! And both <ent> and <model> may be Null, for a full static
  //! editor
  Standard_EXPORT virtual Standard_Boolean Apply(
    const Handle(IFSelect_EditForm)&        form,
    const Handle(Standard_Transient)&       ent,
    const Handle(Interface_InterfaceModel)& model) const = 0;

  DEFINE_STANDARD_RTTIEXT(IFSelect_Editor, Standard_Transient)

protected:
  //! Prepares the list of Typed Values (gives its count)
  //! This count can be tuned later, to a LOWER value, this allows
  //! to initialize with a "maximum reservation" then cut the extra
  Standard_EXPORT IFSelect_Editor(const Standard_Integer nbval);

  //! Adjusts the true count of values. It can be LOWER or equal to
  //! the initial size (which then acts as a reservation), but never
  //! greater
  Standard_EXPORT void SetNbValues(const Standard_Integer nbval);

private:
  Standard_Integer                                               thenbval;
  Standard_Integer                                               themaxsh;
  Standard_Integer                                               themaxco;
  Standard_Integer                                               themaxla;
  NCollection_DataMap<TCollection_AsciiString, Standard_Integer> thenames;
  TColStd_Array1OfTransient                                      thevalues;
  TColStd_Array1OfAsciiString                                    theshorts;
  TColStd_Array1OfInteger                                        themodes;
  TColStd_Array1OfInteger                                        thelists;
};

#endif // _IFSelect_Editor_HeaderFile
