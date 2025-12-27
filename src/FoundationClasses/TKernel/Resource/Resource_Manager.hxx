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
#include <TCollection_AsciiString.hxx>
#include <NCollection_DataMap.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <NCollection_DataMap.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Transient.hxx>
#include <Standard_CString.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>

//! Defines a resource structure and its management methods.
class Resource_Manager : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Resource_Manager, Standard_Transient)
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
  //! lines terminated by newline characters or end of file. The
  //! syntax of an individual resource line is:
  Standard_EXPORT Resource_Manager(const char* aName,
                                   const bool Verbose = false);

  //! Create an empty Resource manager
  Standard_EXPORT Resource_Manager();

  //! Create a Resource manager.
  //! @param[in] theName  description file name
  //! @param[in] theDefaultsDirectory   default folder for looking description file
  //! @param[in] theUserDefaultsDirectory  user folder for looking description file
  //! @param[in] theIsVerbose  print verbose messages
  Standard_EXPORT Resource_Manager(const TCollection_AsciiString& theName,
                                   const TCollection_AsciiString& theDefaultsDirectory,
                                   const TCollection_AsciiString& theUserDefaultsDirectory,
                                   const bool         theIsVerbose = false);

  //! Save the user resource structure in the specified file.
  //! Creates the file if it does not exist.
  Standard_EXPORT bool Save() const;

  //! returns True if the Resource does exist.
  Standard_EXPORT bool Find(const char* aResource) const;

  //! returns True if the Resource does exist.
  Standard_EXPORT bool Find(const TCollection_AsciiString& theResource,
                                        TCollection_AsciiString&       theValue) const;

  //! Gets the value of an integer resource according to its
  //! instance and its type.
  Standard_EXPORT virtual int Integer(const char* aResourceName) const;

  //! Gets the value of a real resource according to its instance
  //! and its type.
  Standard_EXPORT virtual double Real(const char* aResourceName) const;

  //! Gets the value of a CString resource according to its instance
  //! and its type.
  Standard_EXPORT virtual const char* Value(const char* aResourceName) const;

  //! Gets the value of an ExtString resource according to its instance
  //! and its type.
  Standard_EXPORT virtual const char16_t* ExtValue(const char* aResourceName);

  //! Sets the new value of an integer resource.
  //! If the resource does not exist, it is created.
  Standard_EXPORT virtual void SetResource(const char* aResourceName,
                                           const int aValue);

  //! Sets the new value of a real resource.
  //! If the resource does not exist, it is created.
  Standard_EXPORT virtual void SetResource(const char* aResourceName,
                                           const double    aValue);

  //! Sets the new value of an CString resource.
  //! If the resource does not exist, it is created.
  Standard_EXPORT virtual void SetResource(const char* aResourceName,
                                           const char* aValue);

  //! Sets the new value of an ExtString resource.
  //! If the resource does not exist, it is created.
  Standard_EXPORT virtual void SetResource(const char*   aResourceName,
                                           const char16_t* aValue);

  //! Gets the resource file full path by its name.
  //! If corresponding environment variable is not set
  //! or file doesn't exist returns empty string.
  Standard_EXPORT static void GetResourcePath(TCollection_AsciiString& aPath,
                                              const char*   aName,
                                              const bool   isUserDefaults);

  //! Returns internal Ref or User map with parameters
  Standard_EXPORT NCollection_DataMap<TCollection_AsciiString, TCollection_AsciiString>& GetMap(
    bool theRefMap = true);

  //! Returns true if Resource have been found
  bool IsInitialized() const { return myInitialized; }

private:
  Standard_EXPORT void Load(const TCollection_AsciiString&            thePath,
                            NCollection_DataMap<TCollection_AsciiString, TCollection_AsciiString>& aMap);

private:
  TCollection_AsciiString                     myName;
  NCollection_DataMap<TCollection_AsciiString, TCollection_AsciiString>    myRefMap;
  NCollection_DataMap<TCollection_AsciiString, TCollection_AsciiString>    myUserMap;
  NCollection_DataMap<TCollection_AsciiString, TCollection_ExtendedString> myExtStrMap;
  bool                            myVerbose;
  bool                            myInitialized;
};

#endif // _Resource_Manager_HeaderFile
