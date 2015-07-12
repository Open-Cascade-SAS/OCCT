// Created on: 1999-12-17
// Created by: Christian CAILLET
// Copyright (c) 1999 Matra Datavision
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

#ifndef _MoniTool_OptValue_HeaderFile
#define _MoniTool_OptValue_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TCollection_AsciiString.hxx>
#include <Standard_CString.hxx>
#include <Standard_Boolean.hxx>
class Standard_Transient;
class MoniTool_Profile;


//! This class allows two kinds of use
//!
//! As an object, a OptValue can be put in any operator or
//! algorithm ... to use an Option of a Profile, by recording
//! its value, hence avoiding to query the Profile eachtime
//!
//! This object brings a value which can be set as coming from a
//! Profile, with a configuration name and for an Option name
//! This value is evaluated then returned immediately
//!
//! As a class, it can be redefined to work on a dedicated
//! Profile, provided by such or such specific way (as static
//! context for instance)
//!
//! To change configuration, etc... can be done by querying and
//! editing the Profile
class MoniTool_OptValue 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates an OptValue on a given Option
  //! This allows to use "shortcut" method to set the value
  //!
  //! WARNING : loading is not done at creation time. It must be
  //! done explicitly by call to Load
  //!
  //! The reason comes from C++ : the Profile being virtual, and
  //! intended to be redefined in sub-classes, must not be used in
  //! the constructor. A separate method, called on the object
  //! already created with its true type, must be called after
  Standard_EXPORT MoniTool_OptValue(const Standard_CString opt = "");
  
  //! Clears the Value of the OptValue
  Standard_EXPORT void Clear();
  
  //! Sets the value as coming from the Profile, according to an
  //! Option name. Access as Fast or regular
  //! If no value is available, the former one remains : can be
  //! cleared by call to Clear
  Standard_EXPORT void SetValue (const Handle(MoniTool_Profile)& prof, const Standard_CString opt, const Standard_Boolean fast = Standard_True);
  
  //! Returns the Profile which can be used by Short Cut methods
  //! Defaults returns a Null Handle, can be redefined
  //! For instance, to return a static used as dictionary or context
  Standard_EXPORT virtual Handle(MoniTool_Profile) Prof() const;
  
  //! Sets the value from the Profile returned by method Prof,
  //! and Option Name given at creation time.
  //! FastValue by default, else Value
  //!
  //! Does not check if already loaded : reloads anyway
  //! IsLoaded allows to test
  Standard_EXPORT void Load (const Standard_Boolean fast = Standard_True);
  
  //! Says if the OptValue is already loaded (i.e. Value defined)
  Standard_EXPORT Standard_Boolean IsLoaded() const;
  
  //! Returns the Value set by, either SetConf or SetValue
  //! Can be Null ... (if not set or not properly set)
  //!
  //! Returned as Argument, hence avoiding DownCast
  //! Warning : type is not controlled
  Standard_EXPORT void Value (Handle(Standard_Transient)& val) const;
  
  Standard_EXPORT virtual void Delete();
Standard_EXPORT virtual ~MoniTool_OptValue() { Delete(); }




protected:





private:



  TCollection_AsciiString theopt;
  Handle(Standard_Transient) theval;


};







#endif // _MoniTool_OptValue_HeaderFile
