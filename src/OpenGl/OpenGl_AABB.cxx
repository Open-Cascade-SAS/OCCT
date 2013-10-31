// Created on: 2013-08-27
// Created by: Denis BOGOLEPOV
// Copyright (c) 2013 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


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