// Created on: 1998-07-30
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

#ifndef _IFSelect_ParamEditor_HeaderFile
#define _IFSelect_ParamEditor_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TCollection_AsciiString.hxx>
#include <IFSelect_Editor.hxx>
#include <Standard_Integer.hxx>
#include <Standard_CString.hxx>
#include <Standard_Boolean.hxx>
#include <TColStd_HSequenceOfHAsciiString.hxx>
class Interface_TypedValue;
class TCollection_AsciiString;
class IFSelect_EditForm;
class TCollection_HAsciiString;
class Standard_Transient;
class Interface_InterfaceModel;


class IFSelect_ParamEditor;
DEFINE_STANDARD_HANDLE(IFSelect_ParamEditor, IFSelect_Editor)

//! A ParamEditor gives access for edition to a list of TypedValue
//! (i.e. of Static too)
//! Its definition is made of the TypedValue to edit themselves,
//! and can add some constants, which can then be displayed but
//! not changed (for instance, system name, processor version ...)
//!
//! I.E. it gives a way of editing or at least displaying
//! parameters as global
class IFSelect_ParamEditor : public IFSelect_Editor
{

public:

  
  //! Creates a ParamEditor, empty, with a maximum count of params
  //! (default is 100)
  //! And a label, by default it will be "Param Editor"
  Standard_EXPORT IFSelect_ParamEditor(const Standard_Integer nbmax = 100, const Standard_CString label = "");
  
  //! Adds a TypedValue
  //! By default, its short name equates its complete name, it can
  //! be explicited
  Standard_EXPORT void AddValue (const Handle(Interface_TypedValue)& val, const Standard_CString shortname = "");
  
  //! Adds a Constant Text, it will be Read Only
  //! By default, its long name equates its shortname
  Standard_EXPORT void AddConstantText (const Standard_CString val, const Standard_CString shortname, const Standard_CString completename = "");
  
  Standard_EXPORT TCollection_AsciiString Label() const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Boolean Recognize (const Handle(IFSelect_EditForm)& form) const Standard_OVERRIDE;
  
  Standard_EXPORT Handle(TCollection_HAsciiString) StringValue (const Handle(IFSelect_EditForm)& form, const Standard_Integer num) const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Boolean Load (const Handle(IFSelect_EditForm)& form, const Handle(Standard_Transient)& ent, const Handle(Interface_InterfaceModel)& model) const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Boolean Apply (const Handle(IFSelect_EditForm)& form, const Handle(Standard_Transient)& ent, const Handle(Interface_InterfaceModel)& model) const Standard_OVERRIDE;
  
  //! Returns a ParamEditor to work on the Static Parameters of
  //! which names are listed in <list>
  //! Null Handle if <list> is null or empty
  Standard_EXPORT static Handle(IFSelect_ParamEditor) StaticEditor (const Handle(TColStd_HSequenceOfHAsciiString)& list, const Standard_CString label = "");




  DEFINE_STANDARD_RTTIEXT(IFSelect_ParamEditor,IFSelect_Editor)

protected:




private:


  TCollection_AsciiString thelabel;


};







#endif // _IFSelect_ParamEditor_HeaderFile
