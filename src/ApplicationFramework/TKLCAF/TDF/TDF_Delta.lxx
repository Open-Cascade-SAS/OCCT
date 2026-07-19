// Created by: DAUTRY Philippe
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

//      	-------------

// Version:	0.0
// Version	Date		Purpose
//		0.0	Sep  8 1997	Creation

inline bool TDF_Delta::IsEmpty() const
{
  return myAttDeltaList.IsEmpty();
}

inline bool TDF_Delta::IsApplicable(const int aCurrentTime) const
{
  return (myEndTime == aCurrentTime);
}

inline int TDF_Delta::BeginTime() const
{
  return myBeginTime;
}

inline int TDF_Delta::EndTime() const
{
  return myEndTime;
}

inline const NCollection_List<occ::handle<TDF_AttributeDelta>>& TDF_Delta::AttributeDeltas() const
{
  return myAttDeltaList;
}

inline void TDF_Delta::SetName(const TCollection_ExtendedString& theName)
{
  myName = theName;
}

inline TCollection_ExtendedString TDF_Delta::Name() const
{
  return myName;
}

inline void TDF_Delta::ReplaceDeltaList(
  const NCollection_List<occ::handle<TDF_AttributeDelta>>& theList)
{
  myAttDeltaList = theList;
}