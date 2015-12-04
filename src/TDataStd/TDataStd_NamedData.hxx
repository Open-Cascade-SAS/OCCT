// Created on: 2007-05-29
// Created by: Vlad Romashko
// Copyright (c) 2007-2014 OPEN CASCADE SAS
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

#ifndef _TDataStd_NamedData_HeaderFile
#define _TDataStd_NamedData_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TDF_Attribute.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_DataMapOfStringInteger.hxx>
#include <Standard_Real.hxx>
#include <TDataStd_DataMapOfStringReal.hxx>
#include <TDataStd_DataMapOfStringString.hxx>
#include <Standard_Byte.hxx>
#include <TDataStd_DataMapOfStringByte.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TDataStd_DataMapOfStringHArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TDataStd_DataMapOfStringHArray1OfReal.hxx>
#include <Standard_OStream.hxx>
class TDataStd_HDataMapOfStringInteger;
class TDataStd_HDataMapOfStringReal;
class TDataStd_HDataMapOfStringString;
class TDataStd_HDataMapOfStringByte;
class TDataStd_HDataMapOfStringHArray1OfInteger;
class TDataStd_HDataMapOfStringHArray1OfReal;
class Standard_GUID;
class TDF_Label;
class TCollection_ExtendedString;
class TDF_Attribute;
class TDF_RelocationTable;


class TDataStd_NamedData;
DEFINE_STANDARD_HANDLE(TDataStd_NamedData, TDF_Attribute)

//! Contains a named data.
class TDataStd_NamedData : public TDF_Attribute
{

public:

  
  //! Static methods
  //! ==============
  //! Returns the ID of the named data attribute.
  Standard_EXPORT static const Standard_GUID& GetID();
  
  //! Finds or creates a named data attribute.
  Standard_EXPORT static Handle(TDataStd_NamedData) Set (const TDF_Label& label);
  
  Standard_EXPORT TDataStd_NamedData();
  
  //! Returns true if at least one named integer value is
  //! kept in the attribute.
    Standard_Boolean HasIntegers() const;
  
  //! Returns true if the attribute contains specified by Name
  //! integer value.
  Standard_EXPORT Standard_Boolean HasInteger (const TCollection_ExtendedString& theName) const;
  
  //! Returns the integer value specified by the Name.
  //! It returns 0 if internal map doesn't contain the specified
  //! integer (use HasInteger() to check before).
  Standard_EXPORT Standard_Integer GetInteger (const TCollection_ExtendedString& theName);
  
  //! Defines a named integer.
  //! If the integer already exists, it changes its value to <theInteger>.
  Standard_EXPORT void SetInteger (const TCollection_ExtendedString& theName, const Standard_Integer theInteger);
  
  //! Returns the internal container of named integers.
  Standard_EXPORT const TColStd_DataMapOfStringInteger& GetIntegersContainer();
  
  //! Replace the container content by new content of the <theIntegers>.
  Standard_EXPORT void ChangeIntegers (const TColStd_DataMapOfStringInteger& theIntegers);
  
  //! Returns true if at least one named real value is
  //! kept in the attribute.
    Standard_Boolean HasReals() const;
  
  //! Returns true if the attribute contains a real specified by Name.
  Standard_EXPORT Standard_Boolean HasReal (const TCollection_ExtendedString& theName) const;
  
  //! Returns the named real.
  //! It returns 0.0 if there is no such a named real
  //! (use HasReal()).
  Standard_EXPORT Standard_Real GetReal (const TCollection_ExtendedString& theName);
  
  //! Defines a named real.
  //! If the real already exists, it changes its value to <theReal>.
  Standard_EXPORT void SetReal (const TCollection_ExtendedString& theName, const Standard_Real theReal);
  
  //! Returns the internal container of named reals.
  Standard_EXPORT const TDataStd_DataMapOfStringReal& GetRealsContainer();
  
  //! Replace the container content by new content of the <theReals>.
  Standard_EXPORT void ChangeReals (const TDataStd_DataMapOfStringReal& theReals);
  
  //! Returns true if there are some named strings in the attribute.
    Standard_Boolean HasStrings() const;
  
  //! Returns true if the attribute contains this named string.
  Standard_EXPORT Standard_Boolean HasString (const TCollection_ExtendedString& theName) const;
  
  //! Returns the named string.
  //! It returns an empty string if there is no such a named string
  //! (use HasString()).
  Standard_EXPORT const TCollection_ExtendedString& GetString (const TCollection_ExtendedString& theName);
  
  //! Defines a named string.
  //! If the string already exists, it changes its value to <theString>.
  Standard_EXPORT void SetString (const TCollection_ExtendedString& theName, const TCollection_ExtendedString& theString);
  
  //! Returns the internal container of named strings.
  Standard_EXPORT const TDataStd_DataMapOfStringString& GetStringsContainer();
  
  //! Replace the container content by new content of the <theStrings>.
  Standard_EXPORT void ChangeStrings (const TDataStd_DataMapOfStringString& theStrings);
  
  //! Returns true if there are some named bytes in the attribute.
    Standard_Boolean HasBytes() const;
  
  //! Returns true if the attribute contains this named byte.
  Standard_EXPORT Standard_Boolean HasByte (const TCollection_ExtendedString& theName) const;
  
  //! Returns the named byte.
  //! It returns 0 if there is no such a named byte
  //! (use HasByte()).
  Standard_EXPORT Standard_Byte GetByte (const TCollection_ExtendedString& theName);
  
  //! Defines a named byte.
  //! If the byte already exists, it changes its value to <theByte>.
  Standard_EXPORT void SetByte (const TCollection_ExtendedString& theName, const Standard_Byte theByte);
  
  //! Returns the internal container of named bytes.
  Standard_EXPORT const TDataStd_DataMapOfStringByte& GetBytesContainer();
  
  //! Replace the container content by new content of the <theBytes>.
  Standard_EXPORT void ChangeBytes (const TDataStd_DataMapOfStringByte& theBytes);
  
  //! Returns true if there are some named arrays of integer values in the attribute.
    Standard_Boolean HasArraysOfIntegers() const;
  
  //! Returns true if the attribute contains this named array of integer values.
  Standard_EXPORT Standard_Boolean HasArrayOfIntegers (const TCollection_ExtendedString& theName) const;
  
  //! Returns the named array of integer values.
  //! It returns a NULL Handle if there is no such a named array of integers
  //! (use HasArrayOfIntegers()).
  Standard_EXPORT const Handle(TColStd_HArray1OfInteger)& GetArrayOfIntegers (const TCollection_ExtendedString& theName);
  
  //! Defines a named array of integer values.
  //! If the array already exists, it changes its value to <theArrayOfIntegers>.
  Standard_EXPORT void SetArrayOfIntegers (const TCollection_ExtendedString& theName, const Handle(TColStd_HArray1OfInteger)& theArrayOfIntegers);
  
  //! Returns the internal container of named arrays of integer values.
  Standard_EXPORT const TDataStd_DataMapOfStringHArray1OfInteger& GetArraysOfIntegersContainer();
  
  //! Replace the container content by new content of the <theArraysOfIntegers>.
  Standard_EXPORT void ChangeArraysOfIntegers (const TDataStd_DataMapOfStringHArray1OfInteger& theArraysOfIntegers);
  
  //! Returns true if there are some named arrays of real values in the attribute.
    Standard_Boolean HasArraysOfReals() const;
  
  //! Returns true if the attribute contains this named array of real values.
  Standard_EXPORT Standard_Boolean HasArrayOfReals (const TCollection_ExtendedString& theName) const;
  
  //! Returns the named array of real values.
  //! It returns a NULL Handle if there is no such a named array of reals
  //! (use HasArrayOfReals()).
  Standard_EXPORT const Handle(TColStd_HArray1OfReal)& GetArrayOfReals (const TCollection_ExtendedString& theName);
  
  //! Defines a named array of real values.
  //! If the array already exists, it changes its value to <theArrayOfReals>.
  Standard_EXPORT void SetArrayOfReals (const TCollection_ExtendedString& theName, const Handle(TColStd_HArray1OfReal)& theArrayOfReals);
  
  //! Returns the internal container of named arrays of real values.
  Standard_EXPORT const TDataStd_DataMapOfStringHArray1OfReal& GetArraysOfRealsContainer();
  
  //! Replace the container content by new content of the <theArraysOfReals>.
  Standard_EXPORT void ChangeArraysOfReals (const TDataStd_DataMapOfStringHArray1OfReal& theArraysOfReals);
  
  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;
  
  Standard_EXPORT void Restore (const Handle(TDF_Attribute)& With) Standard_OVERRIDE;
  
  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;
  
  Standard_EXPORT void Paste (const Handle(TDF_Attribute)& Into, const Handle(TDF_RelocationTable)& RT) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_OStream& Dump (Standard_OStream& anOS) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(TDataStd_NamedData,TDF_Attribute)

protected:




private:


  Handle(TDataStd_HDataMapOfStringInteger) myIntegers;
  Handle(TDataStd_HDataMapOfStringReal) myReals;
  Handle(TDataStd_HDataMapOfStringString) myStrings;
  Handle(TDataStd_HDataMapOfStringByte) myBytes;
  Handle(TDataStd_HDataMapOfStringHArray1OfInteger) myArraysOfIntegers;
  Handle(TDataStd_HDataMapOfStringHArray1OfReal) myArraysOfReals;


};


#include <TDataStd_NamedData.lxx>





#endif // _TDataStd_NamedData_HeaderFile
