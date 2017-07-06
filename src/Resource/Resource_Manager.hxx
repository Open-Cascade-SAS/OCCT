// Created on: 1995-04-20
// Created by: Tony GEORGIADES
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

#ifndef _Resource_Manager_HeaderFile
#define _Resource_Manager_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TCollection_AsciiString.hxx>
#include <Resource_DataMapOfAsciiStringAsciiString.hxx>
#include <Resource_DataMapOfAsciiStringExtendedString.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Transient.hxx>
#include <Standard_CString.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <Standard_ExtString.hxx>
class Standard_TypeMismatch;
class Resource_NoSuchResource;
class Standard_OutOfRange;
class TCollection_AsciiString;


class Resource_Manager;
DEFINE_STANDARD_HANDLE(Resource_Manager, Standard_Transient)

//! Defines a resource structure and its management methods.
class Resource_Manager : public Standard_Transient
{

public:

  
  //! Create a Resource manager.
  //! Attempts to find the two following files:
  //! $CSF_`aName`Defaults/aName
  //! $CSF_`aName`UserDefaults/aName
  //! and load them respectively into a reference and a user resource structure.
  //!
  //! If CSF_ResourceVerbose defined, seeked files will be printed.
  //!
  //! FILE SYNTAX
  //! The syntax of a resource file is a sequence of resource
  //! lines terminated by newline characters or end of file.  The
  //! syntax of an individual resource line is:
  Standard_EXPORT Resource_Manager(const Standard_CString aName, const Standard_Boolean Verbose = Standard_False);
  
  Standard_EXPORT Resource_Manager(const Standard_CString aName, TCollection_AsciiString& aDefaultsDirectory, TCollection_AsciiString& anUserDefaultsDirectory, const Standard_Boolean Verbose = Standard_False);
  
  //! Save the user resource structure in the specified file.
  //! Creates the file if it does not exist.
  Standard_EXPORT Standard_Boolean Save() const;
  
  //! returns True if the Resource does exist.
  Standard_EXPORT Standard_Boolean Find (const Standard_CString aResource) const;
  
  //! Gets the value of an integer resource according to its
  //! instance and its type.
  Standard_EXPORT virtual Standard_Integer Integer (const Standard_CString aResourceName) const;
  
  //! Gets the value of a real resource according to its instance
  //! and its type.
  Standard_EXPORT virtual Standard_Real Real (const Standard_CString aResourceName) const;
  
  //! Gets the value of a CString resource according to its instance
  //! and its type.
  Standard_EXPORT virtual Standard_CString Value (const Standard_CString aResourceName) const;
  
  //! Gets the value of an ExtString resource according to its instance
  //! and its type.
  Standard_EXPORT virtual Standard_ExtString ExtValue (const Standard_CString aResourceName);
  
  //! Sets the new value of an integer resource.
  //! If the resource does not exist, it is created.
  Standard_EXPORT virtual void SetResource (const Standard_CString aResourceName, const Standard_Integer aValue);
  
  //! Sets the new value of a real resource.
  //! If the resource does not exist, it is created.
  Standard_EXPORT virtual void SetResource (const Standard_CString aResourceName, const Standard_Real aValue);
  
  //! Sets the new value of an CString resource.
  //! If the resource does not exist, it is created.
  Standard_EXPORT virtual void SetResource (const Standard_CString aResourceName, const Standard_CString aValue);
  
  //! Sets the new value of an ExtString resource.
  //! If the resource does not exist, it is created.
  Standard_EXPORT virtual void SetResource (const Standard_CString aResourceName, const Standard_ExtString aValue);
  
  //! Gets the resource file full path by its name.
  //! If corresponding environment variable is not set
  //! or file doesn't exist returns empty string.
  Standard_EXPORT static void GetResourcePath (TCollection_AsciiString& aPath, const Standard_CString aName, const Standard_Boolean isUserDefaults);




  DEFINE_STANDARD_RTTIEXT(Resource_Manager,Standard_Transient)

protected:




private:

  
  Standard_EXPORT void Load (TCollection_AsciiString& aPath, Resource_DataMapOfAsciiStringAsciiString& aMap);

  TCollection_AsciiString myName;
  Resource_DataMapOfAsciiStringAsciiString myRefMap;
  Resource_DataMapOfAsciiStringAsciiString myUserMap;
  Resource_DataMapOfAsciiStringExtendedString myExtStrMap;
  Standard_Boolean myVerbose;


};







#endif // _Resource_Manager_HeaderFile
