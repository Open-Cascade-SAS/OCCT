// Created on: 1998-09-03
// Created by: Yves FRICAUD
// Copyright (c) 1998-1999 Matra Datavision
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

#include <TopOpeBRepDS_Association.hxx>
#include <TopOpeBRepDS_Interference.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TopOpeBRepDS_Association, Standard_Transient)

//=================================================================================================

static bool Contains(const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LI,
                     const occ::handle<TopOpeBRepDS_Interference>&                   I)
{
  for (NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it(LI); it.More();
       it.Next())
  {
    if (I->HasSameGeometry(it.Value()))
      return 1;
  }
  return 0;
}

//=================================================================================================

TopOpeBRepDS_Association::TopOpeBRepDS_Association() {}

//=================================================================================================

void TopOpeBRepDS_Association::Associate(const occ::handle<TopOpeBRepDS_Interference>& I,
                                         const occ::handle<TopOpeBRepDS_Interference>& K)
{
  if (!myMap.IsBound(I))
  {
    NCollection_List<occ::handle<TopOpeBRepDS_Interference>> empty;
    myMap.Bind(I, empty);
    myMap(I).Append(K);
  }
  else if (!Contains(myMap(I), K))
  {
    myMap(I).Append(K);
  }
  if (!myMap.IsBound(K))
  {
    NCollection_List<occ::handle<TopOpeBRepDS_Interference>> empty;
    myMap.Bind(K, empty);
    myMap(K).Append(I);
  }
  else if (!Contains(myMap(K), I))
  {
    myMap(K).Append(I);
  }
}

//=================================================================================================

void TopOpeBRepDS_Association::Associate(
  const occ::handle<TopOpeBRepDS_Interference>&                   I,
  const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LI)
{
  for (NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it(LI); it.More();
       it.Next())
  {
    Associate(I, it.Value());
  }
}

//=================================================================================================

bool TopOpeBRepDS_Association::HasAssociation(const occ::handle<TopOpeBRepDS_Interference>& I) const
{
  return myMap.IsBound(I);
}

//=================================================================================================

NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& TopOpeBRepDS_Association::Associated(
  const occ::handle<TopOpeBRepDS_Interference>& I)
{
  if (myMap.IsBound(I))
  {
    return myMap.ChangeFind(I);
  }
  static NCollection_List<occ::handle<TopOpeBRepDS_Interference>> empty;
  return empty;
}

//=================================================================================================

bool TopOpeBRepDS_Association::AreAssociated(const occ::handle<TopOpeBRepDS_Interference>& I,
                                             const occ::handle<TopOpeBRepDS_Interference>& K) const
{
  return (myMap.IsBound(I) && Contains(myMap(I), K));
}
