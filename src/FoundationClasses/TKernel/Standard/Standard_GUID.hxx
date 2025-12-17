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

#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_CString.hxx>
#include <Standard_UUID.hxx>
#include <Standard_PCharacter.hxx>
#include <Standard_PExtCharacter.hxx>
#include <Standard_OStream.hxx>

#define Standard_GUID_SIZE 36
#define Standard_GUID_SIZE_ALLOC Standard_GUID_SIZE + 1

class Standard_GUID
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates a GUID with all zeros.
  constexpr Standard_GUID() noexcept
      : my32b(0),
        my16b1(0),
        my16b2(0),
        my16b3(0),
        my8b1(0),
        my8b2(0),
        my8b3(0),
        my8b4(0),
        my8b5(0),
        my8b6(0)
  {
  }

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

  //! Creates a GUID from the given components.
  constexpr Standard_GUID(const Standard_Integer      a32b,
                          const Standard_ExtCharacter a16b1,
                          const Standard_ExtCharacter a16b2,
                          const Standard_ExtCharacter a16b3,
                          const Standard_Byte         a8b1,
                          const Standard_Byte         a8b2,
                          const Standard_Byte         a8b3,
                          const Standard_Byte         a8b4,
                          const Standard_Byte         a8b5,
                          const Standard_Byte         a8b6) noexcept
      : my32b(a32b),
        my16b1(a16b1),
        my16b2(a16b2),
        my16b3(a16b3),
        my8b1(a8b1),
        my8b2(a8b2),
        my8b3(a8b3),
        my8b4(a8b4),
        my8b5(a8b5),
        my8b6(a8b6)
  {
  }

  //! Creates a GUID from a Standard_UUID.
  constexpr Standard_GUID(const Standard_UUID& theUUID) noexcept
      : my32b(theUUID.Data1),
        my16b1(theUUID.Data2),
        my16b2(theUUID.Data3),
        my16b3(static_cast<Standard_ExtCharacter>((theUUID.Data4[0] << 8) | theUUID.Data4[1])),
        my8b1(theUUID.Data4[2]),
        my8b2(theUUID.Data4[3]),
        my8b3(theUUID.Data4[4]),
        my8b4(theUUID.Data4[5]),
        my8b5(theUUID.Data4[6]),
        my8b6(theUUID.Data4[7])
  {
  }

  //! Copy constructor.
  constexpr Standard_GUID(const Standard_GUID& theGuid) noexcept = default;

  //! translate the GUID into ascii string
  //! the aStrGuid is allocated by user.
  //! the guid have the following format:
  //!
  //! "00000000-0000-0000-0000-000000000000"
  Standard_EXPORT void ToCString(const Standard_PCharacter aStrGuid) const;

  //! translate the GUID into unicode string
  //! the aStrGuid is allocated by user.
  //! the guid have the following format:
  //!
  //! "00000000-0000-0000-0000-000000000000"
  Standard_EXPORT void ToExtString(const Standard_PExtCharacter aStrGuid) const;

  //! Converts to Standard_UUID.
  constexpr Standard_UUID ToUUID() const noexcept
  {
    return Standard_UUID{static_cast<uint32_t>(my32b),
                         static_cast<uint16_t>(my16b1),
                         static_cast<uint16_t>(my16b2),
                         {static_cast<uint8_t>((my16b3 >> 8) & 0xFF),
                          static_cast<uint8_t>(my16b3 & 0xFF),
                          my8b1,
                          my8b2,
                          my8b3,
                          my8b4,
                          my8b5,
                          my8b6}};
  }

  //! Returns true if this GUID is equal to uid.
  constexpr Standard_Boolean IsSame(const Standard_GUID& uid) const noexcept
  {
    return my32b == uid.my32b && my16b1 == uid.my16b1 && my16b2 == uid.my16b2
           && my16b3 == uid.my16b3 && my8b1 == uid.my8b1 && my8b2 == uid.my8b2
           && my8b3 == uid.my8b3 && my8b4 == uid.my8b4 && my8b5 == uid.my8b5 && my8b6 == uid.my8b6;
  }

  constexpr Standard_Boolean operator==(const Standard_GUID& uid) const noexcept { return IsSame(uid); }

  //! Returns true if this GUID is not equal to uid.
  constexpr Standard_Boolean IsNotSame(const Standard_GUID& uid) const noexcept { return !IsSame(uid); }

  constexpr Standard_Boolean operator!=(const Standard_GUID& uid) const noexcept { return IsNotSame(uid); }

  //! Assigns uid to this GUID.
  constexpr void Assign(const Standard_GUID& uid) noexcept
  {
    my32b  = uid.my32b;
    my16b1 = uid.my16b1;
    my16b2 = uid.my16b2;
    my16b3 = uid.my16b3;
    my8b1  = uid.my8b1;
    my8b2  = uid.my8b2;
    my8b3  = uid.my8b3;
    my8b4  = uid.my8b4;
    my8b5  = uid.my8b5;
    my8b6  = uid.my8b6;
  }

  constexpr Standard_GUID& operator=(const Standard_GUID& uid) noexcept
  {
    Assign(uid);
    return *this;
  }

  //! Assigns uid to this GUID.
  constexpr void Assign(const Standard_UUID& uid) noexcept
  {
    my32b  = uid.Data1;
    my16b1 = uid.Data2;
    my16b2 = uid.Data3;
    my16b3 = static_cast<Standard_ExtCharacter>((uid.Data4[0] << 8) | uid.Data4[1]);
    my8b1  = uid.Data4[2];
    my8b2  = uid.Data4[3];
    my8b3  = uid.Data4[4];
    my8b4  = uid.Data4[5];
    my8b5  = uid.Data4[6];
    my8b6  = uid.Data4[7];
  }

  constexpr Standard_GUID& operator=(const Standard_UUID& uid) noexcept
  {
    Assign(uid);
    return *this;
  }

  //! Display the GUID with the following format:
  //!
  //! "00000000-0000-0000-0000-000000000000"
  Standard_EXPORT void ShallowDump(Standard_OStream& aStream) const;

  //! Check the format of a GUID string.
  //! It checks the size, the position of the '-' and the correct size of fields.
  Standard_EXPORT static Standard_Boolean CheckGUIDFormat(const Standard_CString aGuid);

  template <class T>
  friend struct std::hash;

private:
  Standard_Integer      my32b;
  Standard_ExtCharacter my16b1;
  Standard_ExtCharacter my16b2;
  Standard_ExtCharacter my16b3;
  Standard_Byte         my8b1;
  Standard_Byte         my8b2;
  Standard_Byte         my8b3;
  Standard_Byte         my8b4;
  Standard_Byte         my8b5;
  Standard_Byte         my8b6;
};

namespace std
{
template <>
struct hash<Standard_GUID>
{
  size_t operator()(const Standard_GUID& theGUID) const noexcept
  {
    struct GUID
    {
      Standard_Integer      my32b;
      Standard_ExtCharacter my16b1;
      Standard_ExtCharacter my16b2;
      Standard_ExtCharacter my16b3;
      Standard_Byte         my8b1;
      Standard_Byte         my8b2;
      Standard_Byte         my8b3;
      Standard_Byte         my8b4;
      Standard_Byte         my8b5;
      Standard_Byte         my8b6;
    };

    GUID aGUID{theGUID.my32b,
               theGUID.my16b1,
               theGUID.my16b2,
               theGUID.my16b3,
               theGUID.my8b1,
               theGUID.my8b2,
               theGUID.my8b3,
               theGUID.my8b4,
               theGUID.my8b5,
               theGUID.my8b6};
    return opencascade::hashBytes(&aGUID, sizeof(GUID));
  }
};
} // namespace std

#endif // _Standard_GUID_HeaderFile
