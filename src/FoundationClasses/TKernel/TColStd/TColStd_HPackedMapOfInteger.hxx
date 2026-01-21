// Copyright (c) 2006-2026 OPEN CASCADE SAS
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

#ifndef _TColStd_HPackedMapOfInteger_HeaderFile
#define _TColStd_HPackedMapOfInteger_HeaderFile

#include <TColStd_PackedMapOfInteger.hxx>
#include <Standard_Type.hxx>
#include <Standard_Transient.hxx>

//! @deprecated This Handle wrapper class is deprecated.
//! Use TColStd_PackedMapOfInteger directly instead.
class TColStd_HPackedMapOfInteger : public Standard_Transient
{
public:
  //! Constructor of empty map.
  //! @param theNbBuckets initial number of buckets
  TColStd_HPackedMapOfInteger(const int theNbBuckets = 1)
      : myMap(theNbBuckets)
  {
  }

  //! Constructor from already existing map; performs copying.
  //! @param theOther the map to copy
  TColStd_HPackedMapOfInteger(const TColStd_PackedMapOfInteger& theOther)
      : myMap(theOther)
  {
  }

  //! Constructor from already existing map; performs copying.
  //! @param theOther the map to copy
  TColStd_HPackedMapOfInteger(TColStd_PackedMapOfInteger&& theOther)
      : myMap(std::move(theOther))
  {
  }

  //! Returns const reference to the underlying map.
  const TColStd_PackedMapOfInteger& Map() const { return myMap; }

  //! Returns mutable reference to the underlying map.
  TColStd_PackedMapOfInteger& ChangeMap() { return myMap; }

  DEFINE_STANDARD_RTTI_INLINE(TColStd_HPackedMapOfInteger, Standard_Transient)

private:
  TColStd_PackedMapOfInteger myMap;
};

#endif // _TColStd_HPackedMapOfInteger_HeaderFile
