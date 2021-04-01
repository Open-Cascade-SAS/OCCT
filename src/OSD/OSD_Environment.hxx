// Created on: 2018-03-15
// Created by: Stephan GARNAUD (ARM)
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

#ifndef _OSD_Environment_HeaderFile
#define _OSD_Environment_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TCollection_AsciiString.hxx>
#include <OSD_Error.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
class Standard_ConstructionError;
class Standard_NullObject;
class OSD_OSDError;
class TCollection_AsciiString;


//! Management of system environment variables
//! An environment variable is composed of a variable name
//! and its value.
//!
//! To be portable among various systems, environment variables
//! are local to a process.
class OSD_Environment 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates the object Environment.
  Standard_EXPORT OSD_Environment();
  
  //! Creates an Environment variable initialized with value
  //! set to an empty AsciiString.
  Standard_EXPORT OSD_Environment(const TCollection_AsciiString& Name);
  
  //! Creates an Environment variable initialized with Value.
  Standard_EXPORT OSD_Environment(const TCollection_AsciiString& Name, const TCollection_AsciiString& Value);
  
  //! Changes environment variable value.
  //! Raises ConstructionError either if the string contains
  //! characters not in range of ' '...'~' or if the string
  //! contains the character '$' which is forbidden.
  Standard_EXPORT void SetValue (const TCollection_AsciiString& Value);
  
  //! Gets the value of an environment variable
  Standard_EXPORT TCollection_AsciiString Value();
  
  //! Changes environment variable name.
  //! Raises ConstructionError either if the string contains
  //! characters not in range of ' '...'~' or if the string
  //! contains the character '$' which is forbidden.
  Standard_EXPORT void SetName (const TCollection_AsciiString& name);
  
  //! Gets the name of <me>.
  Standard_EXPORT TCollection_AsciiString Name() const;
  
  //! Sets the value of an environment variable
  //! into system (physically).
  Standard_EXPORT void Build();
  
  //! Removes (physically) an environment variable
  Standard_EXPORT void Remove();
  
  //! Returns TRUE if an error occurs
  Standard_EXPORT Standard_Boolean Failed() const;
  
  //! Resets error counter to zero
  Standard_EXPORT void Reset();
  
  //! Raises OSD_Error
  Standard_EXPORT void Perror();
  
  //! Returns error number if 'Failed' is TRUE.
  Standard_EXPORT Standard_Integer Error() const;




protected:





private:



  TCollection_AsciiString myName;
  TCollection_AsciiString myValue;
  OSD_Error myError;


};







#endif // _OSD_Environment_HeaderFile
