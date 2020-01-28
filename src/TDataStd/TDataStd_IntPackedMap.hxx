// Created on: 2007-07-31
// Created by: Sergey ZARITCHNY
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

#ifndef _TDataStd_IntPackedMap_HeaderFile
#define _TDataStd_IntPackedMap_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Boolean.hxx>
#include <TDF_Attribute.hxx>
#include <TColStd_HPackedMapOfInteger.hxx>
#include <Standard_Integer.hxx>
#include <Standard_OStream.hxx>

class TDataStd_DeltaOnModificationOfIntPackedMap;
class Standard_GUID;
class TDF_Label;
class TDF_Attribute;
class TDF_RelocationTable;
class TDF_DeltaOnModification;

class TDataStd_IntPackedMap;
DEFINE_STANDARD_HANDLE(TDataStd_IntPackedMap, TDF_Attribute)

//! Attribute for storing TColStd_PackedMapOfInteger
class TDataStd_IntPackedMap : public TDF_Attribute
{
  friend class TDataStd_DeltaOnModificationOfIntPackedMap;
  DEFINE_STANDARD_RTTIEXT(TDataStd_IntPackedMap, TDF_Attribute)
public:

  //! class methods
  //! =============
  //! Returns the GUID of the attribute.
  Standard_EXPORT static const Standard_GUID& GetID();
  
  //! Finds or creates an integer map attribute on the given label.
  //! If <isDelta> == False, DefaultDeltaOnModification is used.
  //! If <isDelta> == True, DeltaOnModification of the current attribute is used.
  //! If attribute is already set, input parameter <isDelta> is refused and the found
  //! attribute returned.
  //! Attribute methods
  //! ===================
  Standard_EXPORT static Handle(TDataStd_IntPackedMap) Set (const TDF_Label& label, const Standard_Boolean isDelta = Standard_False);
  
  Standard_EXPORT TDataStd_IntPackedMap();
  
  Standard_EXPORT Standard_Boolean ChangeMap (const Handle(TColStd_HPackedMapOfInteger)& theMap);

  Standard_EXPORT Standard_Boolean ChangeMap (const TColStd_PackedMapOfInteger& theMap);

  const TColStd_PackedMapOfInteger& GetMap() const { return  myMap->Map(); }

  const Handle(TColStd_HPackedMapOfInteger)& GetHMap() const { return myMap; }

  Standard_EXPORT Standard_Boolean Clear();
  
  Standard_EXPORT Standard_Boolean Add (const Standard_Integer theKey);
  
  Standard_EXPORT Standard_Boolean Remove (const Standard_Integer theKey);
  
  Standard_EXPORT Standard_Boolean Contains (const Standard_Integer theKey) const;

  Standard_Integer Extent() const { return myMap->Map().Extent(); }

  Standard_Boolean IsEmpty() const { return myMap->Map().IsEmpty(); }

  Standard_Boolean GetDelta() const { return myIsDelta; }

  //! for  internal  use  only!
  void SetDelta (const Standard_Boolean isDelta) { myIsDelta = isDelta; }

  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;
  
  Standard_EXPORT void Restore (const Handle(TDF_Attribute)& with) Standard_OVERRIDE;
  
  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;
  
  Standard_EXPORT void Paste (const Handle(TDF_Attribute)& into, const Handle(TDF_RelocationTable)& RT) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_OStream& Dump (Standard_OStream& anOS) const Standard_OVERRIDE;
  
  //! Makes a DeltaOnModification between <me> and
  //! <anOldAttribute>.
  Standard_EXPORT virtual Handle(TDF_DeltaOnModification) DeltaOnModification (const Handle(TDF_Attribute)& anOldAttribute) const Standard_OVERRIDE;
  
  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const Standard_OVERRIDE;

private:

  void RemoveMap() { myMap.Nullify(); }

private:

  Handle(TColStd_HPackedMapOfInteger) myMap;
  Standard_Boolean myIsDelta;

};

#endif // _TDataStd_IntPackedMap_HeaderFile
