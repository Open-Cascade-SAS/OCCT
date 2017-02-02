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

/***********************************************************************
     FONCTION :
     ----------
        Classe V3d_View_2.cxx :
     HISTORIQUE DES MODIFICATIONS   :
     --------------------------------
      00-09-92 : GG  ; Creation.
      24-12-97 : FMN ; Suppression de GEOMLITE
      23-11-00 : GG  ; Add IsActiveLight() and IsActivePlane() methods
************************************************************************/
/*
 * Includes
 */

#include <Aspect_GradientBackground.hxx>
#include <Aspect_Grid.hxx>
#include <Aspect_Window.hxx>
#include <Bnd_Box.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_GraphicDriver.hxx>
#include <Graphic3d_Structure.hxx>
#include <Graphic3d_TextureEnv.hxx>
#include <Graphic3d_Vector.hxx>
#include <Quantity_Color.hxx>
#include <Standard_MultiplyDefined.hxx>
#include <Standard_TypeMismatch.hxx>
#include <V3d.hxx>
#include <V3d_BadValue.hxx>
#include <V3d_Light.hxx>
#include <V3d_UnMapped.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>

//=============================================================================
//function : SetLightOn
//purpose  :
//=============================================================================
void V3d_View::SetLightOn (const Handle(V3d_Light)& theLight)
{
  if (!myActiveLights.Contains (theLight))
  {
    if (myActiveLights.Extent() >= LightLimit())
      throw V3d_BadValue("V3d_View::SetLightOn, too many lights");
    myActiveLights.Append (theLight);
    UpdateLights();
  }
}

//=============================================================================
//function : SetLightOff
//purpose  :
//=============================================================================
void V3d_View::SetLightOff (const Handle(V3d_Light)& theLight)
{
  if (MyViewer->IsGlobalLight (theLight))
    throw Standard_TypeMismatch("V3d_View::SetLightOff, the light is global");
  myActiveLights.Remove (theLight);
  UpdateLights();
}

//=============================================================================
//function : IsActiveLight
//purpose  :
//=============================================================================
Standard_Boolean V3d_View::IsActiveLight (const Handle(V3d_Light)& theLight) const
{
  return !theLight.IsNull()
       && myActiveLights.Contains (theLight);
}

//=============================================================================
//function : SetLightOn
//purpose  :
//=============================================================================
void V3d_View::SetLightOn()
{
  for (V3d_ListOfLightIterator aDefLightIter (MyViewer->DefinedLightIterator()); aDefLightIter.More(); aDefLightIter.Next())
  {
    if (!myActiveLights.Contains (aDefLightIter.Value()))
    {
      myActiveLights.Append (aDefLightIter.Value());
    }
  }
  UpdateLights();
}

//=============================================================================
//function : SetLightOff
//purpose  :
//=============================================================================
void V3d_View::SetLightOff()
{
  for (V3d_ListOfLight::Iterator anActiveLightIter (myActiveLights); anActiveLightIter.More();)
  {
    if (!MyViewer->IsGlobalLight (anActiveLightIter.Value()))
    {
      myActiveLights.Remove (anActiveLightIter);
    }
    else
    {
      anActiveLightIter.Next();
    }
  }
  UpdateLights();
}

//=============================================================================
//function : IfMoreLights
//purpose  :
//=============================================================================
Standard_Boolean V3d_View::IfMoreLights() const
{
  return myActiveLights.Extent() < LightLimit();
}

//=======================================================================
//function : LightLimit
//purpose  :
//=======================================================================
Standard_Integer V3d_View::LightLimit() const
{
  return Viewer()->Driver()->InquireLightLimit();
}

//=======================================================================
//function : AddClipPlane
//purpose  :
//=======================================================================
void V3d_View::AddClipPlane (const Handle(Graphic3d_ClipPlane)& thePlane)
{
  Handle(Graphic3d_SequenceOfHClipPlane) aSeqOfPlanes = ClipPlanes();
  if (aSeqOfPlanes.IsNull())
  {
    aSeqOfPlanes = new Graphic3d_SequenceOfHClipPlane();
  }
  else
  {
    for (Graphic3d_SequenceOfHClipPlane::Iterator aPlaneIt (*aSeqOfPlanes); aPlaneIt.More(); aPlaneIt.Next())
    {
      const Handle(Graphic3d_ClipPlane)& aPlane = aPlaneIt.Value();
      if (aPlane == thePlane)
      {
        // plane is already defined in view
        return;
      }
    }
  }

  aSeqOfPlanes->Append (thePlane);
  SetClipPlanes (aSeqOfPlanes);
}

//=======================================================================
//function : RemoveClipPlane
//purpose  :
//=======================================================================
void V3d_View::RemoveClipPlane (const Handle(Graphic3d_ClipPlane)& thePlane)
{
  Handle(Graphic3d_SequenceOfHClipPlane) aSeqOfPlanes = ClipPlanes();
  if (aSeqOfPlanes.IsNull())
  {
    return;
  }

  for (Graphic3d_SequenceOfHClipPlane::Iterator aPlaneIt(*aSeqOfPlanes); aPlaneIt.More(); aPlaneIt.Next())
  {
    const Handle(Graphic3d_ClipPlane)& aPlane = aPlaneIt.Value();
    if (aPlane != thePlane)
      continue;

    aSeqOfPlanes->Remove (aPlaneIt);
    SetClipPlanes (aSeqOfPlanes);
    return;
  }
}

//=======================================================================
//function : SetClipPlanes
//purpose  :
//=======================================================================
void V3d_View::SetClipPlanes (const Handle(Graphic3d_SequenceOfHClipPlane)& thePlanes)
{
  myView->SetClipPlanes (thePlanes);
}

//=======================================================================
//function : ClipPlanes
//purpose  :
//=======================================================================
const Handle(Graphic3d_SequenceOfHClipPlane)& V3d_View::ClipPlanes() const
{
  return myView->ClipPlanes();
}

//=======================================================================
//function : PlaneLimit
//purpose  :
//=======================================================================
Standard_Integer V3d_View::PlaneLimit() const
{
  return Viewer()->Driver()->InquirePlaneLimit();
}
