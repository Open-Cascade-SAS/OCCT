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

#include <Standard_Boolean.hxx>
#include <TDF_Attribute.hxx>
#include <TColStd_HPackedMapOfInteger.hxx>
#include <Standard_Integer.hxx>
#include <Standard_OStream.hxx>

class Standard_GUID;
class TDF_Label;
class TDF_RelocationTable;
class TDF_DeltaOnModification;

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
  Standard_EXPORT static occ::handle<TDataStd_IntPackedMap> Set(const TDF_Label& label,
                                                                const bool       isDelta = false);

  Standard_EXPORT TDataStd_IntPackedMap();

  Standard_EXPORT bool ChangeMap(const occ::handle<TColStd_HPackedMapOfInteger>& theMap);

  Standard_EXPORT bool ChangeMap(const TColStd_PackedMapOfInteger& theMap);

  const TColStd_PackedMapOfInteger& GetMap() const { return myMap->Map(); }

  const occ::handle<TColStd_HPackedMapOfInteger>& GetHMap() const { return myMap; }

  Standard_EXPORT bool Clear();

  Standard_EXPORT bool Add(const int theKey);

  Standard_EXPORT bool Remove(const int theKey);

  Standard_EXPORT bool Contains(const int theKey) const;

  int Extent() const { return myMap->Map().Extent(); }

  bool IsEmpty() const { return myMap->Map().IsEmpty(); }

  bool GetDelta() const { return myIsDelta; }

  //! for internal use only!
  void SetDelta(const bool isDelta) { myIsDelta = isDelta; }

  Standard_EXPORT const Standard_GUID& ID() const override;

  Standard_EXPORT void Restore(const occ::handle<TDF_Attribute>& with) override;

  Standard_EXPORT occ::handle<TDF_Attribute> NewEmpty() const override;

  Standard_EXPORT void Paste(const occ::handle<TDF_Attribute>&       into,
                             const occ::handle<TDF_RelocationTable>& RT) const override;

  Standard_EXPORT Standard_OStream& Dump(Standard_OStream& anOS) const override;

  //! Makes a DeltaOnModification between <me> and
  //! <anOldAttribute>.
  Standard_EXPORT occ::handle<TDF_DeltaOnModification> DeltaOnModification(
    const occ::handle<TDF_Attribute>& anOldAttribute) const override;

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream,
                                        int               theDepth = -1) const override;

private:
  void RemoveMap() { myMap.Nullify(); }

private:
  occ::handle<TColStd_HPackedMapOfInteger> myMap;
  bool                                     myIsDelta;
};

#endif // _TDataStd_IntPackedMap_HeaderFile
