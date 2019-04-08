// Created on: 1997-01-28
// Created by: CAL
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

#include <Graphic3d_GraphicDriver.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_GraphicDriver,Standard_Transient)

// =======================================================================
// function : Graphic3d_GraphicDriver
// purpose  :
// =======================================================================
Graphic3d_GraphicDriver::Graphic3d_GraphicDriver (const Handle(Aspect_DisplayConnection)& theDisp)
: myDisplayConnection (theDisp)
{
  // default layers are always presented in display layer sequence it can not be removed
  {
    Graphic3d_ZLayerSettings aSettings;
    aSettings.SetImmediate          (Standard_False);
    aSettings.SetEnvironmentTexture (Standard_False);
    aSettings.SetEnableDepthTest    (Standard_False);
    aSettings.SetEnableDepthWrite   (Standard_False);
    aSettings.SetClearDepth         (Standard_False);
    aSettings.SetPolygonOffset (Graphic3d_PolygonOffset());
    myLayerIds.Add             (Graphic3d_ZLayerId_BotOSD);
    myLayerSeq.Append          (Graphic3d_ZLayerId_BotOSD);
    myMapOfZLayerSettings.Bind (Graphic3d_ZLayerId_BotOSD, aSettings);
  }

  {
    Graphic3d_ZLayerSettings aSettings;
    aSettings.SetImmediate          (Standard_False);
    aSettings.SetEnvironmentTexture (Standard_True);
    aSettings.SetEnableDepthTest    (Standard_True);
    aSettings.SetEnableDepthWrite   (Standard_True);
    aSettings.SetClearDepth         (Standard_False);
    aSettings.SetPolygonOffset (Graphic3d_PolygonOffset());
    myLayerIds.Add             (Graphic3d_ZLayerId_Default);
    myLayerSeq.Append          (Graphic3d_ZLayerId_Default);
    myMapOfZLayerSettings.Bind (Graphic3d_ZLayerId_Default, aSettings);
  }

  {
    Graphic3d_ZLayerSettings aSettings;
    aSettings.SetImmediate          (Standard_True);
    aSettings.SetEnvironmentTexture (Standard_True);
    aSettings.SetEnableDepthTest    (Standard_True);
    aSettings.SetEnableDepthWrite   (Standard_True);
    aSettings.SetClearDepth         (Standard_False);
    aSettings.SetPolygonOffset (Graphic3d_PolygonOffset());
    myLayerIds.Add             (Graphic3d_ZLayerId_Top);
    myLayerSeq.Append          (Graphic3d_ZLayerId_Top);
    myMapOfZLayerSettings.Bind (Graphic3d_ZLayerId_Top, aSettings);
  }

  {
    Graphic3d_ZLayerSettings aSettings;
    aSettings.SetImmediate          (Standard_True);
    aSettings.SetEnvironmentTexture (Standard_True);
    aSettings.SetEnableDepthTest    (Standard_True);
    aSettings.SetEnableDepthWrite   (Standard_True);
    aSettings.SetClearDepth         (Standard_True);
    aSettings.SetPolygonOffset (Graphic3d_PolygonOffset());
    myLayerIds.Add             (Graphic3d_ZLayerId_Topmost);
    myLayerSeq.Append          (Graphic3d_ZLayerId_Topmost);
    myMapOfZLayerSettings.Bind (Graphic3d_ZLayerId_Topmost, aSettings);
  }

  {
    Graphic3d_ZLayerSettings aSettings;
    aSettings.SetImmediate          (Standard_True);
    aSettings.SetEnvironmentTexture (Standard_False);
    aSettings.SetEnableDepthTest    (Standard_False);
    aSettings.SetEnableDepthWrite   (Standard_False);
    aSettings.SetClearDepth         (Standard_False);
    aSettings.SetPolygonOffset (Graphic3d_PolygonOffset());
    myLayerIds.Add             (Graphic3d_ZLayerId_TopOSD);
    myLayerSeq.Append          (Graphic3d_ZLayerId_TopOSD);
    myMapOfZLayerSettings.Bind (Graphic3d_ZLayerId_TopOSD, aSettings);
  }
}

// =======================================================================
// function : GetDisplayConnection
// purpose  :
// =======================================================================
const Handle(Aspect_DisplayConnection)& Graphic3d_GraphicDriver::GetDisplayConnection() const
{
  return myDisplayConnection;
}

// =======================================================================
// function : NewIdentification
// purpose  :
// =======================================================================
Standard_Integer Graphic3d_GraphicDriver::NewIdentification()
{
  return myStructGenId.Next();
}

// =======================================================================
// function : RemoveIdentification
// purpose  :
// =======================================================================
void Graphic3d_GraphicDriver::RemoveIdentification(const Standard_Integer theId)
{
  myStructGenId.Free(theId);
}

//=======================================================================
//function : ZLayerSettings
//purpose  :
//=======================================================================
const Graphic3d_ZLayerSettings& Graphic3d_GraphicDriver::ZLayerSettings (const Graphic3d_ZLayerId theLayerId) const
{
  Standard_ASSERT_RAISE (myLayerIds.Contains (theLayerId), "Graphic3d_GraphicDriver::ZLayerSettings, Layer with theLayerId does not exist");
  return myMapOfZLayerSettings.Find (theLayerId);
}

//=======================================================================
//function : addZLayerIndex
//purpose  :
//=======================================================================
void Graphic3d_GraphicDriver::addZLayerIndex (const Graphic3d_ZLayerId theLayerId)
{
  // remove index
  for (TColStd_SequenceOfInteger::Iterator aLayerIt (myLayerSeq); aLayerIt.More(); aLayerIt.Next())
  {
    if (aLayerIt.Value() == theLayerId)
    {
      myLayerSeq.Remove (aLayerIt);
      break;
    }
  }

  if (myMapOfZLayerSettings.Find (theLayerId).IsImmediate())
  {
    myLayerSeq.Append (theLayerId);
    return;
  }

  for (TColStd_SequenceOfInteger::Iterator aLayerIt (myLayerSeq); aLayerIt.More(); aLayerIt.Next())
  {
    const Graphic3d_ZLayerSettings& aSettings = myMapOfZLayerSettings.Find (aLayerIt.Value());
    if (aSettings.IsImmediate())
    {
      aLayerIt.Previous();
      if (aLayerIt.More())
      {
        myLayerSeq.InsertAfter (aLayerIt, theLayerId);
        return;
      }

      // first non-immediate layer
      myLayerSeq.Prepend (theLayerId);
      return;
    }
  }

  // no immediate layers
  myLayerSeq.Append (theLayerId);
}

//=======================================================================
//function : SetZLayerSettings
//purpose  :
//=======================================================================
void Graphic3d_GraphicDriver::SetZLayerSettings (const Graphic3d_ZLayerId theLayerId,
                                                 const Graphic3d_ZLayerSettings& theSettings)
{
  Graphic3d_ZLayerSettings* aSettings = myMapOfZLayerSettings.ChangeSeek (theLayerId);
  if (aSettings != NULL)
  {
    const bool isChanged = (aSettings->IsImmediate() != theSettings.IsImmediate());
    *aSettings = theSettings;
    if (isChanged)
    {
      addZLayerIndex (theLayerId);
    }
  }
  else
  {
    // abnormal case
    myMapOfZLayerSettings.Bind (theLayerId, theSettings);
    addZLayerIndex (theLayerId);
  }
}
