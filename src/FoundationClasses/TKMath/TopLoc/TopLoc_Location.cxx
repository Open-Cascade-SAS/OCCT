// Created on: 1991-01-21
// Created by: Christophe MARION
// Copyright (c) 1991-1999 Matra Datavision
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

#define No_Standard_NoSuchObject

#include <gp_Trsf.hxx>
#include <Standard_Dump.hxx>
#include <TopLoc_Datum3D.hxx>
#include <TopLoc_Location.hxx>
#include <TopLoc_SListOfItemLocation.hxx>

namespace
{
//! Append a datum power, merging or cancelling an adjacent equal datum.
//! theItems is the storage of theLocation, passed explicitly to keep this helper local.
void multiplyDatum(const TopLoc_Location&             theLocation,
                   TopLoc_SListOfItemLocation&        theItems,
                   const occ::handle<TopLoc_Datum3D>& theDatum,
                   int                                thePower)
{
  if (!theLocation.IsIdentity() && theLocation.FirstDatum() == theDatum)
  {
    thePower += theLocation.FirstPower();
    theItems.ToTail();
  }
  if (thePower != 0)
  {
    theItems.Construct(TopLoc_ItemLocation(theDatum, thePower));
  }
}
} // namespace

//=================================================================================================

TopLoc_Location::TopLoc_Location(const occ::handle<TopLoc_Datum3D>& D)
{
  myItems.Construct(TopLoc_ItemLocation(D, 1));
}

//=================================================================================================

TopLoc_Location::TopLoc_Location(const gp_Trsf& T)
    : TopLoc_Location(new TopLoc_Datum3D(T))
{
}

//=================================================================================================

const gp_Trsf& TopLoc_Location::Transformation() const
{
  static const gp_Trsf THE_IDENTITY_TRSF;
  return IsIdentity() ? THE_IDENTITY_TRSF : myItems.Value().myTrsf;
}

TopLoc_Location::operator gp_Trsf() const
{
  return Transformation();
}

//=================================================================================================

TopLoc_Location TopLoc_Location::Inverted() const
{
  //
  // the inverse of a Location is a chain in revert order
  // with opposite powers and same Local
  //
  TopLoc_Location result;
  for (const TopLoc_SListOfItemLocation* anItems = &myItems; anItems->More();
       anItems                                   = &anItems->Tail())
  {
    const TopLoc_ItemLocation& anItem = anItems->Value();
    result.myItems.Construct(TopLoc_ItemLocation(anItem.myDatum, -anItem.myPower));
  }
  return result;
}

//=======================================================================
// function : Multiplied
// purpose  : operator *
//=======================================================================

TopLoc_Location TopLoc_Location::Multiplied(const TopLoc_Location& Other) const
{
  // prepend the chain Other in front of this
  // cancelling null exponents

  if (IsIdentity())
  {
    return Other;
  }

  if (Other.IsIdentity())
  {
    return *this;
  }

  // prepend the queue of Other
  TopLoc_Location result = Multiplied(Other.NextLocation());
  multiplyDatum(result, result.myItems, Other.FirstDatum(), Other.FirstPower());
  return result;
}

//=======================================================================
// function : Divided
// purpose  : operator /   this*Other.Inverted()
//=======================================================================

TopLoc_Location TopLoc_Location::Divided(const TopLoc_Location& Other) const
{
  // Append inverse powers directly instead of allocating an intermediate inverse chain.
  TopLoc_Location aResult = *this;
  for (const TopLoc_SListOfItemLocation* anItems = &Other.myItems; anItems->More();
       anItems                                   = &anItems->Tail())
  {
    const TopLoc_ItemLocation& anItem = anItems->Value();
    multiplyDatum(aResult, aResult.myItems, anItem.myDatum, -anItem.myPower);
  }
  return aResult;
}

//=======================================================================
// function : Predivided
// purpose  : return Other.Inverted() * this
//=======================================================================

TopLoc_Location TopLoc_Location::Predivided(const TopLoc_Location& Other) const
{
  if (Other.IsIdentity())
  {
    return *this;
  }
  if (IsIdentity())
  {
    return Other.Inverted();
  }
  if (*this == Other)
  {
    return TopLoc_Location();
  }
  return Other.Inverted().Multiplied(*this);
}

//=================================================================================================

TopLoc_Location TopLoc_Location::Powered(const int pwr) const
{
  if (IsIdentity() || pwr == 1)
  {
    return *this;
  }
  if (pwr == 0)
  {
    return TopLoc_Location();
  }

  // optimisation when just one element
  if (myItems.Tail().IsEmpty())
  {
    TopLoc_Location result;
    result.myItems.Construct(TopLoc_ItemLocation(FirstDatum(), FirstPower() * pwr));
    return result;
  }

  if (pwr > 0)
  {
    return Multiplied(Powered(pwr - 1));
  }
  else
  {
    return Inverted().Powered(-pwr);
  }
}

//=================================================================================================

void TopLoc_Location::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_CLASS_BEGIN(theOStream, TopLoc_Location)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &Transformation())
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, IsIdentity())
}

//=================================================================================================

void TopLoc_Location::ShallowDump(Standard_OStream& S) const
{
  S << "TopLoc_Location : ";
  TopLoc_SListOfItemLocation items = myItems;
  if (items.IsEmpty())
  {
    S << "Identity" << '\n';
  }
  while (items.More())
  {
    S << "\n";
    S << "       Exponent : " << items.Value().myPower << '\n';
    items.Value().myDatum->ShallowDump(S);
    items.Next();
  }
  S << "\n";
}
