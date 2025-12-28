// Created on: 2000-05-22
// Created by: Peter KURNEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#include <IntTools_Root.hxx>

//=================================================================================================

IntTools_Root::IntTools_Root()
    : myRoot(0.),
      myType(-1),
      myLayerHeight(0.),
      myStateBefore(TopAbs_UNKNOWN),
      myStateAfter(TopAbs_UNKNOWN),
      myt1(0.),
      myt2(0.),
      myf1(0.),
      myf2(0.)
{
}

//=================================================================================================

IntTools_Root::IntTools_Root(const double aRoot, const int aType)
    : myLayerHeight(0.),
      myStateBefore(TopAbs_UNKNOWN),
      myStateAfter(TopAbs_UNKNOWN),
      myt1(0.),
      myt2(0.),
      myf1(0.),
      myf2(0.)
{
  myRoot = aRoot;
  myType = aType;
}

//=================================================================================================

void IntTools_Root::SetRoot(const double aRoot)
{
  myRoot = aRoot;
}

//=================================================================================================

void IntTools_Root::SetType(const int aType)
{
  myType = aType;
}

//=================================================================================================

void IntTools_Root::SetStateBefore(const TopAbs_State aState)
{
  myStateBefore = aState;
}

//=================================================================================================

void IntTools_Root::SetStateAfter(const TopAbs_State aState)
{
  myStateAfter = aState;
}

//=================================================================================================

void IntTools_Root::SetLayerHeight(const double aHeight)
{
  myLayerHeight = aHeight;
}

//=================================================================================================

void IntTools_Root::SetInterval(const double t1,
                                const double t2,
                                const double f1,
                                const double f2)
{
  myt1 = t1;
  myt2 = t2;
  myf1 = f1;
  myf2 = f2;
}

//=================================================================================================

void IntTools_Root::Interval(double& t1,
                             double& t2,
                             double& f1,
                             double& f2) const
{
  t1 = myt1;
  t2 = myt2;
  f1 = myf1;
  f2 = myf2;
}

//=================================================================================================

double IntTools_Root::Root() const
{
  return myRoot;
}

//=================================================================================================

int IntTools_Root::Type() const
{
  return myType;
}

//=================================================================================================

TopAbs_State IntTools_Root::StateBefore() const
{
  return myStateBefore;
}

//=================================================================================================

TopAbs_State IntTools_Root::StateAfter() const
{
  return myStateAfter;
}

//=================================================================================================

double IntTools_Root::LayerHeight() const
{
  return myLayerHeight;
}

//=================================================================================================

bool IntTools_Root::IsValid() const
{
  if (myStateBefore == TopAbs_OUT && myStateAfter == TopAbs_IN)
    return true;
  if (myStateBefore == TopAbs_OUT && myStateAfter == TopAbs_ON)
    return true;
  if (myStateBefore == TopAbs_ON && myStateAfter == TopAbs_OUT)
    return true;
  if (myStateBefore == TopAbs_IN && myStateAfter == TopAbs_OUT)
    return true;

  return false;
}
