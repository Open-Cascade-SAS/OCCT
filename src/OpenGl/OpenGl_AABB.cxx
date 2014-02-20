// Created on: 2013-08-27
// Created by: Denis BOGOLEPOV
// Copyright (c) 2013 OPEN CASCADE SAS
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

#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#ifdef HAVE_OPENCL

#include <Standard_ShortReal.hxx>

#include <OpenGl_AABB.hxx>


OpenGl_AABB::OpenGl_AABB() : myMinPoint(),
                             myMaxPoint(),
                             myIsValid (false)
{ }

OpenGl_AABB::OpenGl_AABB (const OpenGl_RTVec4f& thePoint) : myMinPoint (thePoint),
                                                            myMaxPoint (thePoint),
                                                            myIsValid (true)
{ }

OpenGl_AABB::OpenGl_AABB (const OpenGl_RTVec4f& theMinPoint,
                          const OpenGl_RTVec4f& theMaxPoint) : myMinPoint (theMinPoint),
                                                               myMaxPoint (theMaxPoint),
                                                               myIsValid (true)
{ }

OpenGl_AABB::OpenGl_AABB (const OpenGl_AABB& theVolume) : myMinPoint (theVolume.myMinPoint),
                                                          myMaxPoint (theVolume.myMaxPoint),
                                                          myIsValid (theVolume.myIsValid)
{ }

void OpenGl_AABB::Add (const OpenGl_RTVec4f& thePoint)
{
  if (!myIsValid)
  {
    myMinPoint = thePoint;
    myMaxPoint = thePoint;
    myIsValid = true;
  }
  else
  {
    myMinPoint = OpenGl_RTVec4f (Min (myMinPoint.x(), thePoint.x()),
                                 Min (myMinPoint.y(), thePoint.y()),
                                 Min (myMinPoint.z(), thePoint.z()),
                                 1.f);

    myMaxPoint = OpenGl_RTVec4f (Max (myMaxPoint.x(), thePoint.x()),
                                 Max (myMaxPoint.y(), thePoint.y()),
                                 Max (myMaxPoint.z(), thePoint.z()),
                                 1.f);
  }
}

void OpenGl_AABB::Combine (const OpenGl_AABB& theVolume)
{
  if (!theVolume.myIsValid)
    return;

  if (!myIsValid)
  {
    myMinPoint = theVolume.myMinPoint;
    myMaxPoint = theVolume.myMaxPoint;
    myIsValid = true;
  }
  else
  {
    myMinPoint = OpenGl_RTVec4f (Min (myMinPoint.x(), theVolume.myMinPoint.x()),
                                 Min (myMinPoint.y(), theVolume.myMinPoint.y()),
                                 Min (myMinPoint.z(), theVolume.myMinPoint.z()),
                                 1.f);

    myMaxPoint = OpenGl_RTVec4f (Max (myMaxPoint.x(), theVolume.myMaxPoint.x()),
                                 Max (myMaxPoint.y(), theVolume.myMaxPoint.y()),
                                 Max (myMaxPoint.z(), theVolume.myMaxPoint.z()),
                                 1.f);
  }
}

OpenGl_AABB OpenGl_AABB::Added (const OpenGl_RTVec4f& thePoint) const
{
  OpenGl_AABB result (*this);

  result.Add (thePoint);

  return result;
}

OpenGl_AABB OpenGl_AABB::Combined (const OpenGl_AABB& theVolume) const
{
  OpenGl_AABB result (*this);

  result.Combine (theVolume);

  return result;
}

void OpenGl_AABB::Clear()
{
  myIsValid = false;
}

OpenGl_RTVec4f OpenGl_AABB::Size() const
{
  return myMaxPoint - myMinPoint;
}

float OpenGl_AABB::Area() const
{
  const float aXLen = myMaxPoint.x() - myMinPoint.x();
  const float aYLen = myMaxPoint.y() - myMinPoint.y();
  const float aZLen = myMaxPoint.z() - myMinPoint.z();
  
  return ( aXLen * aYLen + aXLen * aZLen + aZLen * aYLen ) * 2.f; 
}

#endif
