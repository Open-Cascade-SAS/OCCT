// Copyright (c) 2020 OPEN CASCADE SAS
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

#include <TDF_DerivedAttribute.hxx>

#include <NCollection_DataMap.hxx>
#include <Standard_Mutex.hxx>
#include <TCollection_AsciiString.hxx>

namespace TDF_DerivedAttributeGlobals
{

  //! Data for the derived attribute correct creation
  struct CreatorData
  {
    TDF_DerivedAttribute::NewDerived myCreator;
    Standard_CString                 myNameSpace;
    Standard_CString                 myTypeName;
  };

  //! List that contains the methods that create all registered derived attributes
  static NCollection_List<CreatorData>& Creators()
  {
    static NCollection_List<CreatorData> THE_CREATORS_LIST;
    return THE_CREATORS_LIST;
  }
  //! Global map of the string-type of derived attribute -> instance of such attribute
  static NCollection_DataMap<Standard_CString, Handle(TDF_Attribute)>& Attributes()
  {
    static NCollection_DataMap<Standard_CString, Handle(TDF_Attribute)> THE_DERIVED;
    return THE_DERIVED;
  }

  //! Global map of the string-type of derived attribute -> type name to identify this attribute
  static NCollection_DataMap<Standard_CString, TCollection_AsciiString*>& Types()
  {
    static NCollection_DataMap<Standard_CString, TCollection_AsciiString*> THE_DERIVED_TYPES;
    return THE_DERIVED_TYPES;
  }

  //! To minimize simultaneous access to global "DERIVED" maps from parallel threads
  static Standard_Mutex& Mutex()
  {
    static Standard_Mutex THE_DERIVED_MUTEX;
    return THE_DERIVED_MUTEX;
  }
} // namespace TDF_DerivedAttributeGlobals

//=======================================================================
// function : Register
// purpose  : Registers a derived by the pointer to a method that creates a new derived attribute instance
//=======================================================================
TDF_DerivedAttribute::NewDerived TDF_DerivedAttribute::Register (NewDerived       theNewAttributeFunction,
                                                                 Standard_CString theNameSpace,
                                                                 Standard_CString theTypeName)
{
  TDF_DerivedAttributeGlobals::CreatorData aData = {theNewAttributeFunction, theNameSpace, theTypeName};
  Standard_Mutex::Sentry                   aSentry (TDF_DerivedAttributeGlobals::Mutex());
  TDF_DerivedAttributeGlobals::Creators().Append (aData);
  return theNewAttributeFunction;
}

//=======================================================================
// function : Initialize
// purpose  : Checks synchronization and performs initialization of derived attributes maps if needed
//=======================================================================
static void Initialize()
{
  if (!TDF_DerivedAttributeGlobals::Creators().IsEmpty())
  { // initialization
    NCollection_List<TDF_DerivedAttributeGlobals::CreatorData>::Iterator aCreator;
    for (aCreator.Initialize (TDF_DerivedAttributeGlobals::Creators()); aCreator.More(); aCreator.Next())
    {
      Handle(TDF_Attribute) aDerived      = aCreator.Value().myCreator();
      Standard_CString aDerivedDynamicType = aDerived->DynamicType()->Name();

      TCollection_AsciiString aTypeName;
      if (aCreator.Value().myNameSpace != NULL)
      {
        if (aCreator.Value().myNameSpace[0] != '\0')
        {
          aTypeName = aCreator.Value().myNameSpace;
          aTypeName += ':';
        }
      }
      if (aCreator.Value().myTypeName == NULL)
      {
        aTypeName += aDerivedDynamicType;
      }
      else
      {
        aTypeName += aCreator.Value().myTypeName;
      }

      // persistent storage of types strings: they are not changed like maps on resize
      static NCollection_List<TCollection_AsciiString> THE_TYPES_STORAGE;
      THE_TYPES_STORAGE.Append(aTypeName);
      TDF_DerivedAttributeGlobals::Types().Bind (aDerivedDynamicType, &(THE_TYPES_STORAGE.Last()));
      TDF_DerivedAttributeGlobals::Attributes().Bind (aDerivedDynamicType, aDerived);
    }
    TDF_DerivedAttributeGlobals::Creators().Clear();
  }
}

//=======================================================================
// function : Attribute
// purpose  :
//=======================================================================
Handle(TDF_Attribute) TDF_DerivedAttribute::Attribute (Standard_CString theType)
{
  Standard_Mutex::Sentry aSentry (TDF_DerivedAttributeGlobals::Mutex());
  Initialize();
  if (const Handle(TDF_Attribute)* aResult = TDF_DerivedAttributeGlobals::Attributes().Seek (theType))
  {
    return *aResult;
  }

  static const Handle(TDF_Attribute) aNullAttrib;
  return aNullAttrib;
}

//=======================================================================
// function : TypeName
// purpose  :
//=======================================================================
const TCollection_AsciiString& TDF_DerivedAttribute::TypeName (Standard_CString theType)
{
  Standard_Mutex::Sentry aSentry (TDF_DerivedAttributeGlobals::Mutex());
  Initialize();
  if (TCollection_AsciiString *const* aResult = TDF_DerivedAttributeGlobals::Types().Seek (theType))
  {
    return **aResult;
  }

  static const TCollection_AsciiString anEmpty;
  return anEmpty;
}

//=======================================================================
// function : Attributes
// purpose  :
//=======================================================================
void TDF_DerivedAttribute::Attributes (NCollection_List<Handle(TDF_Attribute)>& theList)
{
  Standard_Mutex::Sentry aSentry (TDF_DerivedAttributeGlobals::Mutex());
  Initialize();
  NCollection_DataMap<Standard_CString, Handle(TDF_Attribute)>::Iterator anAttrIter;
  for (anAttrIter.Initialize (TDF_DerivedAttributeGlobals::Attributes()); anAttrIter.More(); anAttrIter.Next())
  {
    theList.Append (anAttrIter.Value());
  }
}
