// Created on: 2013-08-15
// Created by: Anton POLETAEV
// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#include <OpenGl_CappingPlaneResource.hxx>
#include <OpenGl_Context.hxx>
#include <OpenGl_Vec.hxx>
#include <Precision.hxx>

IMPLEMENT_STANDARD_HANDLE (OpenGl_CappingPlaneResource, OpenGl_Resource)
IMPLEMENT_STANDARD_RTTIEXT(OpenGl_CappingPlaneResource, OpenGl_Resource)

// =======================================================================
// function : OpenGl_CappingPlaneResource
// purpose  :
// =======================================================================
OpenGl_CappingPlaneResource::OpenGl_CappingPlaneResource (const Handle(Graphic3d_ClipPlane)& thePlane)
: myOrientation (OpenGl_IdentityMatrix),
  myAspect      (NULL),
  myPlaneRoot   (thePlane),
  myEquationMod ((unsigned int )-1),
  myAspectMod   ((unsigned int )-1)
{}

// =======================================================================
// function : OpenGl_CappingPlaneResource
// purpose  :
// =======================================================================
OpenGl_CappingPlaneResource::~OpenGl_CappingPlaneResource()
{
  Release (NULL);
}

// =======================================================================
// function : Update
// purpose  :
// =======================================================================
void OpenGl_CappingPlaneResource::Update (const Handle(OpenGl_Context)& theContext)
{
  UpdateTransform();
  UpdateAspect (theContext);
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void OpenGl_CappingPlaneResource::Release (OpenGl_Context* theContext)
{
  OpenGl_Element::Destroy (theContext, myAspect);
  myEquationMod = (unsigned int )-1;
  myAspectMod   = (unsigned int )-1;
}

// =======================================================================
// function : UpdateAspect
// purpose  :
// =======================================================================
void OpenGl_CappingPlaneResource::UpdateAspect (const Handle(OpenGl_Context)& theContext)
{
  Handle(Graphic3d_AspectFillArea3d) aCappingAsp = myPlaneRoot->CappingAspect();
  if (myAspect != NULL && !aCappingAsp.IsNull())
  {
    if (myAspectMod == myPlaneRoot->MCountAspect())
      return; // noting to update
    
    myAspect->SetAspect (aCappingAsp);
    myAspectMod = myPlaneRoot->MCountAspect();
    return;
  }

  // no more used
  if (myAspect != NULL && aCappingAsp.IsNull())
  {
    OpenGl_Element::Destroy (theContext.operator->(), myAspect);
    myAspectMod = myPlaneRoot->MCountAspect();
    return;
  }

  // first created
  if (myAspect == NULL && !aCappingAsp.IsNull())
  {
    myAspect = new OpenGl_AspectFace();
    myAspect->SetAspect (aCappingAsp);
    myAspectMod = myPlaneRoot->MCountAspect();
  }
}

// =======================================================================
// function : UpdateTransform
// purpose  :
// =======================================================================
void OpenGl_CappingPlaneResource::UpdateTransform()
{
  const Graphic3d_ClipPlane::Equation& anEquation = myPlaneRoot->GetEquation();
  if (myEquationMod == myPlaneRoot->MCountEquation())
  {
    return; // nothing to update
  }

  // re-evaluate infinite plane transformation matrix
  Standard_ShortReal N[3] = 
    { (Standard_ShortReal)anEquation[0],
      (Standard_ShortReal)anEquation[1],
      (Standard_ShortReal)anEquation[2] };

  Standard_ShortReal T[3] = 
    { (Standard_ShortReal)(anEquation[0] * -anEquation[3]),
      (Standard_ShortReal)(anEquation[1] * -anEquation[3]),
      (Standard_ShortReal)(anEquation[2] * -anEquation[3]) };

  Standard_ShortReal L[3] = { 0.0f, 0.0f, 0.0f };
  Standard_ShortReal F[3] = { 0.0f, 0.0f, 0.0f };

  // project plane normal onto OX to find left vector
  Standard_ShortReal aConfusion = (Standard_ShortReal)Precision::Confusion();
  Standard_ShortReal aProjLen = 
    sqrt (  (Standard_ShortReal)(anEquation[0] * anEquation[0])
          + (Standard_ShortReal)(anEquation[2] * anEquation[2]));
  if (aProjLen < aConfusion)
  {
    L[0] = 1.0f;
  }
  else
  {
    L[0] =  N[2] / aProjLen;
    L[2] = -N[0] / aProjLen;
  }

  // (-aLeft) x aNorm
  F[0] = (-L[1])*N[2] - (-L[2])*N[1];
  F[1] = (-L[2])*N[0] - (-L[0])*N[2];
  F[2] = (-L[0])*N[1] - (-L[1])*N[0];

  myOrientation.mat[0][0] = L[0];
  myOrientation.mat[0][1] = L[1];
  myOrientation.mat[0][2] = L[2];
  myOrientation.mat[0][3] = 0.0f;

  myOrientation.mat[1][0] = N[0];
  myOrientation.mat[1][1] = N[1];
  myOrientation.mat[1][2] = N[2];
  myOrientation.mat[1][3] = 0.0f;

  myOrientation.mat[2][0] = F[0];
  myOrientation.mat[2][1] = F[1];
  myOrientation.mat[2][2] = F[2];
  myOrientation.mat[2][3] = 0.0f;

  myOrientation.mat[3][0] = T[0];
  myOrientation.mat[3][1] = T[1];
  myOrientation.mat[3][2] = T[2];
  myOrientation.mat[3][3] = 1.0f;

  myEquationMod = myPlaneRoot->MCountEquation();
}
