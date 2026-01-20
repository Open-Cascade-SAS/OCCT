// Copyright (c) 2017 OPEN CASCADE SAS
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

#include <Graphic3d_LightSet.hxx>

#include <NCollection_LocalArray.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_LightSet, Standard_Transient)

namespace
{
//! Suffixes identifying light source type.
static const char THE_LIGHT_KEY_LETTERS[Graphic3d_TypeOfLightSource_NB] = {
  'a', // Graphic3d_TypeOfLightSource_Ambient
  'd', // Graphic3d_TypeOfLightSource_Directional
  'p', // Graphic3d_TypeOfLightSource_Positional
  's'  // Graphic3d_TypeOfLightSource_Spot
};
} // namespace

//=================================================================================================

Graphic3d_LightSet::Graphic3d_LightSet()
    : myAmbient(0.0f, 0.0f, 0.0f, 0.0f),
      myNbEnabled(0),
      myNbCastShadows(0),
      myRevision(1),
      myCacheRevision(0)
{
  memset(myLightTypes, 0, sizeof(myLightTypes));
  memset(myLightTypesEnabled, 0, sizeof(myLightTypesEnabled));
}

//=================================================================================================

bool Graphic3d_LightSet::Add(const occ::handle<Graphic3d_CLight>& theLight)
{
  if (theLight.IsNull())
  {
    throw Standard_ProgramError("Graphic3d_LightSet::Add(), NULL argument");
  }

  const int anOldExtent = myLights.Extent();
  const int anIndex     = myLights.Add(theLight, 0);
  if (anIndex <= anOldExtent)
  {
    return false;
  }

  myLightTypes[theLight->Type()] += 1;
  myLights.ChangeFromIndex(anIndex) = theLight->Revision();
  ++myRevision;
  return true;
}

//=================================================================================================

bool Graphic3d_LightSet::Remove(const occ::handle<Graphic3d_CLight>& theLight)
{
  const int anIndToRemove = myLights.FindIndex(theLight);
  if (anIndToRemove <= 0)
  {
    return false;
  }

  ++myRevision;
  myLights.RemoveFromIndex(anIndToRemove);
  myLightTypes[theLight->Type()] -= 1;
  return true;
}

//=================================================================================================

size_t Graphic3d_LightSet::UpdateRevision()
{
  if (myCacheRevision == myRevision)
  {
    // check implicit updates of light sources
    for (NCollection_IndexedDataMap<occ::handle<Graphic3d_CLight>, size_t>::Iterator aLightIter(
           myLights);
         aLightIter.More();
         aLightIter.Next())
    {
      const occ::handle<Graphic3d_CLight>& aLight = aLightIter.Key();
      if (aLightIter.Value() != aLight->Revision())
      {
        ++myRevision;
        break;
      }
    }
  }
  if (myCacheRevision == myRevision)
  {
    return myRevision;
  }

  myCacheRevision = myRevision;
  myNbCastShadows = 0;
  myAmbient.SetValues(0.0f, 0.0f, 0.0f, 0.0f);
  memset(myLightTypesEnabled, 0, sizeof(myLightTypesEnabled));
  NCollection_LocalArray<char, 32> aKeyLong(myLights.Extent() + 1);
  int                 aLightLast = 0;
  for (NCollection_IndexedDataMap<occ::handle<Graphic3d_CLight>, size_t>::Iterator aLightIter(
         myLights);
       aLightIter.More();
       aLightIter.Next())
  {
    const occ::handle<Graphic3d_CLight>& aLight = aLightIter.Key();
    aLightIter.ChangeValue()               = aLight->Revision();
    if (!aLight->IsEnabled())
    {
      continue;
    }

    myLightTypesEnabled[aLight->Type()] += 1;
    if (aLight->Type() == Graphic3d_TypeOfLightSource_Ambient)
    {
      myAmbient += aLight->PackedColor() * aLight->Intensity();
    }
    else
    {
      if (aLight->ToCastShadows())
      {
        ++myNbCastShadows;
        aKeyLong[aLightLast++] = UpperCase(THE_LIGHT_KEY_LETTERS[aLight->Type()]);
      }
      else
      {
        aKeyLong[aLightLast++] = THE_LIGHT_KEY_LETTERS[aLight->Type()];
      }
    }
  }
  aKeyLong[aLightLast] = '\0';
  myAmbient.a()        = 1.0f;
  myNbEnabled          = myLightTypesEnabled[Graphic3d_TypeOfLightSource_Directional]
                + myLightTypesEnabled[Graphic3d_TypeOfLightSource_Positional]
                + myLightTypesEnabled[Graphic3d_TypeOfLightSource_Spot];
  myKeyEnabledLong = aKeyLong;
  myKeyEnabledShort =
    TCollection_AsciiString(myLightTypesEnabled[Graphic3d_TypeOfLightSource_Directional] > 0
                              ? THE_LIGHT_KEY_LETTERS[Graphic3d_TypeOfLightSource_Directional]
                              : '\0')
    + TCollection_AsciiString(myLightTypesEnabled[Graphic3d_TypeOfLightSource_Positional] > 0
                                ? THE_LIGHT_KEY_LETTERS[Graphic3d_TypeOfLightSource_Positional]
                                : '\0')
    + TCollection_AsciiString(myLightTypesEnabled[Graphic3d_TypeOfLightSource_Spot] > 0
                                ? THE_LIGHT_KEY_LETTERS[Graphic3d_TypeOfLightSource_Spot]
                                : '\0');
  return myRevision;
}
