// Created on: 1997-06-19
// Created by: Christophe LEYNADIER
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _Standard_GUID_HeaderFile
#define _Standard_GUID_HeaderFile

#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_ExtCharacter.hxx>
#include <Standard_Byte.hxx>
#include <Standard_CString.hxx>
#include <Standard_ExtString.hxx>
#include <Standard_UUID.hxx>
#include <Standard_PCharacter.hxx>
#include <Standard_PExtCharacter.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_OStream.hxx>

#define Standard_GUID_SIZE 36
#define Standard_GUID_SIZE_ALLOC Standard_GUID_SIZE+1

class Standard_GUID 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Standard_GUID();
  
  //! build a GUID from an ascii string with the
  //! following format:
  //! Length : 36 char
  //! "00000000-0000-0000-0000-000000000000"
  Standard_EXPORT Standard_GUID(const Standard_CString aGuid);
  
  //! build a GUID from an unicode string with the
  //! following format:
  //!
  //! "00000000-0000-0000-0000-000000000000"
  Standard_EXPORT Standard_GUID(const Standard_ExtString aGuid);
  
  Standard_EXPORT Standard_GUID(const Standard_Integer a32b, const Standard_ExtCharacter a16b1, const Standard_ExtCharacter a16b2, const Standard_ExtCharacter a16b3, const Standard_Byte a8b1, const Standard_Byte a8b2, const Standard_Byte a8b3, const Standard_Byte a8b4, const Standard_Byte a8b5, const Standard_Byte a8b6);
  
  Standard_EXPORT Standard_GUID(const Standard_UUID& aGuid);
  
  Standard_EXPORT Standard_GUID(const Standard_GUID& aGuid);
  
  Standard_EXPORT Standard_UUID ToUUID() const;
  
  //! translate the GUID into ascii string
  //! the aStrGuid is allocated by user.
  //! the guid have the following format:
  //!
  //! "00000000-0000-0000-0000-000000000000"
  Standard_EXPORT void ToCString (const Standard_PCharacter aStrGuid) const;
  
  //! translate the GUID into unicode string
  //! the aStrGuid is allocated by user.
  //! the guid have the following format:
  //!
  //! "00000000-0000-0000-0000-000000000000"
  Standard_EXPORT void ToExtString (const Standard_PExtCharacter aStrGuid) const;
  
  Standard_EXPORT Standard_Boolean IsSame (const Standard_GUID& uid) const;
Standard_Boolean operator == (const Standard_GUID& uid) const
{
  return IsSame(uid);
}
  
  Standard_EXPORT Standard_Boolean IsNotSame (const Standard_GUID& uid) const;
Standard_Boolean operator != (const Standard_GUID& uid) const
{
  return IsNotSame(uid);
}
  
  Standard_EXPORT void Assign (const Standard_GUID& uid);
void operator = (const Standard_GUID& uid)
{
  Assign(uid);
}
  
  Standard_EXPORT void Assign (const Standard_UUID& uid);
void operator = (const Standard_UUID& uid)
{
  Assign(uid);
}
  
  //! Display the GUID with the following format:
  //!
  //! "00000000-0000-0000-0000-000000000000"
  Standard_EXPORT void ShallowDump (Standard_OStream& aStream) const;
  
  //! Check the format of a GUID string.
  //! It checks the size, the position of the '-' and the correct size of fields.
  Standard_EXPORT static Standard_Boolean CheckGUIDFormat (const Standard_CString aGuid);
  
  //! Hash function for GUID.
  Standard_EXPORT Standard_Integer Hash (const Standard_Integer Upper) const;
  
  //! H method used by collections.
  Standard_EXPORT static Standard_Integer HashCode (const Standard_GUID& aguid, const Standard_Integer Upper);
  
  //! Returns True  when the two GUID are the same.
  Standard_EXPORT static Standard_Boolean IsEqual (const Standard_GUID& string1, const Standard_GUID& string2);




protected:





private:



  Standard_Integer my32b;
  Standard_ExtCharacter my16b1;
  Standard_ExtCharacter my16b2;
  Standard_ExtCharacter my16b3;
  Standard_Byte my8b1;
  Standard_Byte my8b2;
  Standard_Byte my8b3;
  Standard_Byte my8b4;
  Standard_Byte my8b5;
  Standard_Byte my8b6;


};







#endif // _Standard_GUID_HeaderFile
