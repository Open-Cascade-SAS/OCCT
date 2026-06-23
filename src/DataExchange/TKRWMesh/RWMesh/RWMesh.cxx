// Copyright (c) 2021 OPEN CASCADE SAS
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

#include <RWMesh.hxx>

#include <NCollection_UtfString.hxx>
#include <TDataStd_Name.hxx>
#include <TDF_Tool.hxx>

namespace
{
static TCollection_AsciiString ExtendedToUtf8(const TCollection_ExtendedString& theStr)
{
  NCollection_UtfString<char> utf8(reinterpret_cast<const char16_t*>(theStr.ToExtString()));
  return TCollection_AsciiString(utf8.ToCString());
}
} // namespace

//=================================================================================================

TCollection_AsciiString RWMesh::ReadNameAttribute(const TDF_Label& theLabel)
{
  occ::handle<TDataStd_Name> aNodeName;
  return theLabel.FindAttribute(TDataStd_Name::GetID(), aNodeName)
           ? ExtendedToUtf8(aNodeName->Get())
           : TCollection_AsciiString();
}

//=================================================================================================

TCollection_AsciiString RWMesh::FormatName(RWMesh_NameFormat theFormat,
                                           const TDF_Label&  theLabel,
                                           const TDF_Label&  theRefLabel)
{
  switch (theFormat)
  {
    case RWMesh_NameFormat_Empty: {
      return TCollection_AsciiString();
    }
    case RWMesh_NameFormat_Product: {
      occ::handle<TDataStd_Name> aRefNodeName;
      return theRefLabel.FindAttribute(TDataStd_Name::GetID(), aRefNodeName)
               ? ExtendedToUtf8(aRefNodeName->Get())
               : TCollection_AsciiString();
    }
    case RWMesh_NameFormat_Instance: {
      occ::handle<TDataStd_Name> aNodeName;
      return theLabel.FindAttribute(TDataStd_Name::GetID(), aNodeName)
               ? ExtendedToUtf8(aNodeName->Get())
               : TCollection_AsciiString();
    }
    case RWMesh_NameFormat_InstanceOrProduct: {
      occ::handle<TDataStd_Name> aNodeName;
      if (theLabel.FindAttribute(TDataStd_Name::GetID(), aNodeName) && !aNodeName->Get().IsEmpty())
      {
        return ExtendedToUtf8(aNodeName->Get());
      }

      occ::handle<TDataStd_Name> aRefNodeName;
      return theRefLabel.FindAttribute(TDataStd_Name::GetID(), aRefNodeName)
               ? ExtendedToUtf8(aRefNodeName->Get())
               : TCollection_AsciiString();
    }
    case RWMesh_NameFormat_ProductOrInstance: {
      occ::handle<TDataStd_Name> aRefNodeName;
      if (theRefLabel.FindAttribute(TDataStd_Name::GetID(), aRefNodeName)
          && !aRefNodeName->Get().IsEmpty())
      {
        return ExtendedToUtf8(aRefNodeName->Get());
      }

      occ::handle<TDataStd_Name> aNodeName;
      return theLabel.FindAttribute(TDataStd_Name::GetID(), aNodeName)
               ? ExtendedToUtf8(aNodeName->Get())
               : TCollection_AsciiString();
    }
    case RWMesh_NameFormat_ProductAndInstance: {
      const TCollection_AsciiString anInstName = ReadNameAttribute(theLabel);
      const TCollection_AsciiString aProdName  = ReadNameAttribute(theRefLabel);
      return !anInstName.IsEmpty() && aProdName != anInstName
               ? aProdName + " [" + anInstName + "]"
               : (!aProdName.IsEmpty() ? aProdName : TCollection_AsciiString(""));
    }
    case RWMesh_NameFormat_ProductAndInstanceAndOcaf: {
      const TCollection_AsciiString anInstName = ReadNameAttribute(theLabel);
      const TCollection_AsciiString aProdName  = ReadNameAttribute(theRefLabel);
      TCollection_AsciiString       anEntryId;
      TDF_Tool::Entry(theLabel, anEntryId);
      return !anInstName.IsEmpty() && aProdName != anInstName
               ? aProdName + " [" + anInstName + "]" + " [" + anEntryId + "]"
               : aProdName + " [" + anEntryId + "]";
    }
  }

  return TCollection_AsciiString();
}
