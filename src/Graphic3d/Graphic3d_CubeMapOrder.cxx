// Author: Ilya Khramov
// Copyright (c) 2019 OPEN CASCADE SAS
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

#include <Graphic3d_CubeMapOrder.hxx>

#include <Standard_Failure.hxx>

#include <bitset>

// =======================================================================
// function : Graphic3d_CubeMapOrder
// purpose  :
// =======================================================================
Graphic3d_CubeMapOrder::Graphic3d_CubeMapOrder()
  :
  myConvolution  (0),
  myHasOverflows (false)
{}

// =======================================================================
// function : Graphic3d_CubeMapOrder
// purpose  :
// =======================================================================
Graphic3d_CubeMapOrder::Graphic3d_CubeMapOrder (unsigned char thePosXLocation,
                                                unsigned char theNegXLocation,
                                                unsigned char thePosYLocation,
                                                unsigned char theNegYLocation,
                                                unsigned char thePosZLocation,
                                                unsigned char theNegZLocation)
  :
  myConvolution  (0),
  myHasOverflows (false)
{
  Set (Graphic3d_CMS_POS_X, thePosXLocation);
  Set (Graphic3d_CMS_NEG_X, theNegXLocation);
  Set (Graphic3d_CMS_POS_Y, thePosYLocation);
  Set (Graphic3d_CMS_NEG_Y, theNegYLocation);
  Set (Graphic3d_CMS_POS_Z, thePosZLocation);
  Set (Graphic3d_CMS_NEG_Z, theNegZLocation);
}

// =======================================================================
// function : Graphic3d_CubeMapOrder
// purpose  :
// =======================================================================
Graphic3d_CubeMapOrder::Graphic3d_CubeMapOrder (const Graphic3d_ValidatedCubeMapOrder& theOrder)
  :
  myConvolution  (theOrder.Order.myConvolution),
  myHasOverflows (theOrder.Order.myHasOverflows)
{}

// =======================================================================
// function : Set
// purpose  :
// =======================================================================
Graphic3d_CubeMapOrder& Graphic3d_CubeMapOrder::Set (const Graphic3d_CubeMapOrder& theOrder)
{
  myConvolution = theOrder.myConvolution;
  myHasOverflows = theOrder.myHasOverflows;
  return *this;
}

// =======================================================================
// function : operator=
// purpose  :
// =======================================================================
Graphic3d_ValidatedCubeMapOrder Graphic3d_CubeMapOrder::Validated() const
{
  if (!IsValid())
  {
    throw Standard_Failure("Try of Graphic3d_ValidatedCubeMapOrder creation using invalid Graphic3d_CubeMapOrder");
  }

  return *this;
}

// =======================================================================
// function : Set
// purpose  :
// =======================================================================
Graphic3d_CubeMapOrder& Graphic3d_CubeMapOrder::Set (Graphic3d_CubeMapSide theCubeMapSide, unsigned char theValue)
{
  if (theValue > 5)
  {
    myHasOverflows = true;
    return *this;
  }
  set (theCubeMapSide, theValue);
  return *this;
}

// =======================================================================
// function : Get
// purpose  :
// =======================================================================
unsigned char Graphic3d_CubeMapOrder::Get (Graphic3d_CubeMapSide theCubeMapSide) const
{
  return get (static_cast<unsigned char> (theCubeMapSide));
}

// =======================================================================
// function : operator[]
// purpose  :
// =======================================================================
unsigned char Graphic3d_CubeMapOrder::operator[] (Graphic3d_CubeMapSide theCubeMapSide) const
{
  return Get (theCubeMapSide);
}

// =======================================================================
// function : SetDefault
// purpose  :
// =======================================================================
Graphic3d_CubeMapOrder& Graphic3d_CubeMapOrder::SetDefault()
{
  for (unsigned char i = 0; i < 6; ++i)
  {
    set (Graphic3d_CubeMapSide (i), i);
  }
  return *this;
}

// =======================================================================
// function : Permute
// purpose  :
// =======================================================================
Graphic3d_CubeMapOrder& Graphic3d_CubeMapOrder::Permute (const Graphic3d_ValidatedCubeMapOrder& thePermutation)
{
  for (unsigned char i = 0; i < 6; ++i)
  {
    set (i, thePermutation->get (get (i)));
  }

  return *this;
}

// =======================================================================
// function : Permuted
// purpose  :
// =======================================================================
Graphic3d_CubeMapOrder Graphic3d_CubeMapOrder::Permuted (const Graphic3d_ValidatedCubeMapOrder& thePermutation) const
{
  Graphic3d_CubeMapOrder anOrder = *this;
  anOrder.Permute (thePermutation);
  return anOrder;
}

// =======================================================================
// function : Swap
// purpose  :
// =======================================================================
Graphic3d_CubeMapOrder& Graphic3d_CubeMapOrder::Swap (Graphic3d_CubeMapSide theFirstSide,
                                                      Graphic3d_CubeMapSide theSecondSide)
{
  unsigned char aTmp = Get (theFirstSide);
  set (theFirstSide, Get(theSecondSide));
  set (theSecondSide, aTmp);
  return *this;
}

// =======================================================================
// function : Swapped
// purpose  :
// =======================================================================
Graphic3d_CubeMapOrder Graphic3d_CubeMapOrder::Swapped (Graphic3d_CubeMapSide theFirstSide,
                                                        Graphic3d_CubeMapSide theSecondSide) const
{
  Graphic3d_CubeMapOrder anOrder = *this;
  anOrder.Swap (theFirstSide, theSecondSide);
  return anOrder;
}

// =======================================================================
// function : Clear
// purpose  :
// =======================================================================
Graphic3d_CubeMapOrder& Graphic3d_CubeMapOrder::Clear()
{
  myConvolution = 0;
  myHasOverflows = false;
  return *this;
}

// =======================================================================
// function : IsEmpty
// purpose  :
// =======================================================================
bool Graphic3d_CubeMapOrder::IsEmpty() const
{
  return myConvolution == 0;
}

// =======================================================================
// function : HasRepetitions
// purpose  :
// =======================================================================
bool Graphic3d_CubeMapOrder::HasRepetitions() const
{
  std::bitset<6> aBitSet;
  for (unsigned char i = 0; i < 6; ++i)
  {
    std::bitset<6>::reference aFlag = aBitSet[get (i)];
    if (aFlag)
    {
      return true;
    }
    aFlag = true;
  }
  return false;
}

// =======================================================================
// function : HasOverflows
// purpose  :
// =======================================================================
bool Graphic3d_CubeMapOrder::HasOverflows() const
{
  return myHasOverflows;
}

// =======================================================================
// function : IsValid
// purpose  :
// =======================================================================
bool Graphic3d_CubeMapOrder::IsValid() const
{
  return !HasRepetitions() && !HasOverflows();
}

// =======================================================================
// function : get
// purpose  :
// =======================================================================
unsigned char Graphic3d_CubeMapOrder::get (unsigned char theCubeMapSide) const
{
  return (myConvolution / (1 << (theCubeMapSide * 3))) % (1 << 3);
}

// =======================================================================
// function : set
// purpose  :
// =======================================================================
void Graphic3d_CubeMapOrder::set (unsigned char theCubeMapSide, unsigned char theValue)
{
  unsigned int aValuePlace = 1 << (theCubeMapSide * 3);
  myConvolution -= aValuePlace * get (theCubeMapSide);
  myConvolution += aValuePlace * theValue;
}

// =======================================================================
// function : set
// purpose  :
// =======================================================================
void Graphic3d_CubeMapOrder::set (Graphic3d_CubeMapSide theCubeMapSide, unsigned char theValue)
{
  set (static_cast<unsigned char> (theCubeMapSide), theValue);
}

// =======================================================================
// function : Default
// purpose  :
// =======================================================================
const Graphic3d_ValidatedCubeMapOrder& Graphic3d_CubeMapOrder::Default()
{
  static const Graphic3d_ValidatedCubeMapOrder aCubeMapOrder = Graphic3d_CubeMapOrder().SetDefault().Validated();
  return aCubeMapOrder;
}