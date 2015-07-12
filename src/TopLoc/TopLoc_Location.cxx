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
#include <Standard_ConstructionError.hxx>
#include <Standard_NoSuchObject.hxx>
#include <TopLoc_Datum3D.hxx>
#include <TopLoc_ItemLocation.hxx>
#include <TopLoc_Location.hxx>
#include <TopLoc_SListOfItemLocation.hxx>

static const gp_Trsf TheIdentity;

//=======================================================================
//function : TopLoc_Location
//purpose  : constructor Identity
//=======================================================================

TopLoc_Location::TopLoc_Location () 
{
}
//=======================================================================
//function : TopLoc_Location
//purpose  : constructor Datum
//=======================================================================

TopLoc_Location::TopLoc_Location (const Handle(TopLoc_Datum3D)& D)
{
  myItems.Construct(TopLoc_ItemLocation(D,1));
}

//=======================================================================
//function : TopLoc_Location
//purpose  : 
//=======================================================================

TopLoc_Location::TopLoc_Location(const gp_Trsf& T)
{
  Handle(TopLoc_Datum3D) D = new TopLoc_Datum3D(T);
  myItems.Construct(TopLoc_ItemLocation(D,1));
}

//=======================================================================
//function : Transformation
//purpose  : 
//=======================================================================

const gp_Trsf& TopLoc_Location::Transformation() const
{
  if (IsIdentity())
    return TheIdentity;
  else
    return myItems.Value().myTrsf;
}

TopLoc_Location::operator gp_Trsf() const
{
  return Transformation();
}

//=======================================================================
//function : Inverted
//purpose  : return the inverse
//=======================================================================

TopLoc_Location TopLoc_Location::Inverted () const
{
  //
  // the inverse of a Location is a chain in revert order
  // with opposite powers and same Local
  //
  TopLoc_Location result;
  TopLoc_SListOfItemLocation items = myItems;
  while (items.More()) {
    result.myItems.Construct(TopLoc_ItemLocation(items.Value().myDatum,
						 -items.Value().myPower));
    items.Next();
  }
  return result;
}

//=======================================================================
//function : Multiplied
//purpose  : operator *
//=======================================================================

TopLoc_Location TopLoc_Location::Multiplied(const TopLoc_Location& Other) const
{
  // prepend the chain Other in front of this
  // cancelling null exponents
  
  if (IsIdentity()) return Other;
  if (Other.IsIdentity()) return *this;
  
  // prepend the queue of Other
  TopLoc_Location result = Multiplied(Other.NextLocation());
  // does the head of Other cancel the head of result

  Standard_Integer p = Other.FirstPower();
  if (!result.IsIdentity()) {
    if (Other.FirstDatum() == result.FirstDatum()) {
      p += result.FirstPower();
      result.myItems.ToTail();
    }
  }
  if (p != 0)
    result.myItems.Construct(TopLoc_ItemLocation(Other.FirstDatum(),p));
  return result;
}

//=======================================================================
//function : Divided
//purpose  : operator /   this*Other.Inverted()
//=======================================================================

TopLoc_Location TopLoc_Location::Divided (const TopLoc_Location& Other) const
{
  return Multiplied(Other.Inverted());
}

//=======================================================================
//function : Predivided
//purpose  : return Other.Inverted() * this
//=======================================================================

TopLoc_Location TopLoc_Location::Predivided (const TopLoc_Location& Other) 
     const
{
  return Other.Inverted().Multiplied(*this);
}

//=======================================================================
//function : Powered
//purpose  : power elevation
//=======================================================================

TopLoc_Location TopLoc_Location::Powered (const Standard_Integer pwr) const
{
  if (IsIdentity()) return *this;
  if (pwr == 1) return *this;
  if (pwr == 0) return TopLoc_Location();
  
  // optimisation when just one element
  if (myItems.Tail().IsEmpty()) {
    TopLoc_Location result;
    result.myItems.Construct(TopLoc_ItemLocation(FirstDatum(),
						 FirstPower() * pwr));
    return result;
  }

  if (pwr > 0) return Multiplied(Powered(pwr - 1));
  else         return Inverted().Powered(-pwr);
}

//=======================================================================
//function : HashCode
//purpose  : 
//=======================================================================

Standard_Integer TopLoc_Location::HashCode(const Standard_Integer upper) const
{
  // the HashCode computed for a Location is the bitwise exclusive or
  // of values computed for each element of the list
  // to compute this value, the depth of the element is computed 
  // the depth is the position of the element in the list
  // this depth is multiplied by 3
  // each element is an elementary Datum raised to a Power
  // the Power is bitwise left shifted by depth
  // this is added to the HashCode of the Datum
  // this value is biwise rotated by depth
  // the use of depth avoids getting the same result for two permutated lists.

  Standard_Integer depth = 0;
  unsigned int h = 0;
  TopLoc_SListOfItemLocation items = myItems;
  while (items.More()) {
    depth += 3;
    unsigned int i = ::HashCode (items.Value().myDatum, upper);
    unsigned int j = ( (i + items.Value().myPower) <<depth);
    j = j>>(32-depth) | j<<depth;
    h ^= j;
    items.Next();
  }
  return h % upper;
}

//=======================================================================
//function : IsEqual
//purpose  : operator ==
//=======================================================================

// two locations are Equal if the Items have the same LocalValues and Powers
// this is a recursive function to test it

Standard_Boolean TopLoc_Location::IsEqual (const TopLoc_Location& Other) const
{
  const void** p = (const void**) &myItems;
  const void** q = (const void**) &Other.myItems;
  if (*p            == *q                  ) {return Standard_True ; }
  if (IsIdentity()  || Other.IsIdentity()  ) {return Standard_False; }
  if (FirstDatum()  != Other.FirstDatum()  ) {return Standard_False; }
  if (FirstPower()  != Other.FirstPower()  ) {return Standard_False; }
  else { return NextLocation() == Other.NextLocation();}
}

//=======================================================================
//function : IsDifferent
//purpose  : 
//=======================================================================

Standard_Boolean TopLoc_Location::IsDifferent
  (const TopLoc_Location& Other) const
{
  return !IsEqual(Other);
}

//=======================================================================
//function : ShallowDump
//purpose  : 
//=======================================================================

void TopLoc_Location::ShallowDump(Standard_OStream& S) const
{
  S << "TopLoc_Location : ";
  TopLoc_SListOfItemLocation items  = myItems;
  if (items.IsEmpty()) S << "Identity"<<endl;
  while (items.More()) {
    S<<"\n";
    S << "       Exponent : " << items.Value().myPower <<endl;
    items.Value().myDatum->ShallowDump(S);
    items.Next();
  }
  S << "\n";
}


