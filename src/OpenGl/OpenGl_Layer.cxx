// Created on: 2014-03-31
// Created by: Danila ULYANOV
// Copyright (c) 2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <OpenGl_Layer.hxx>

#include <OpenGl_GlCore11.hxx>

//=======================================================================
//function : OpenGl_LayerSettings
//purpose  : 
//=======================================================================
OpenGl_LayerSettings::OpenGl_LayerSettings()
  : DepthOffsetFactor (1.0f),
    DepthOffsetUnits  (1.0f),
    Flags (OpenGl_LayerDepthTest
          | OpenGl_LayerDepthWrite
          | OpenGl_LayerDepthClear)
{
  //
}

//=======================================================================
//function : OpenGl_Layer
//purpose  : 
//=======================================================================
OpenGl_Layer::OpenGl_Layer (const Standard_Integer theNbPriorities)
  : myPriorityList (theNbPriorities)
{
  //
}

//=======================================================================
//function : Render
//purpose  : 
//=======================================================================
void OpenGl_Layer::Render (const Handle(OpenGl_Workspace) &AWorkspace) const
{
  // separate depth buffers
  if (IsSettingEnabled (OpenGl_LayerDepthClear))
  {
    glClear (GL_DEPTH_BUFFER_BIT);
  }

  // handle depth test
  if (IsSettingEnabled (OpenGl_LayerDepthTest))
  {
    glDepthFunc (GL_LESS);
  }
  else
  {
    glDepthFunc (GL_ALWAYS);
  }

  // handle depth offset
  if (IsSettingEnabled (OpenGl_LayerDepthOffset))
  {
    glPolygonOffset (myLayerSettings.DepthOffsetFactor, myLayerSettings.DepthOffsetUnits);
  }
  else
  {
    glPolygonOffset (0.f, 0.f);
  }

  // handle depth write
  if (IsSettingEnabled (OpenGl_LayerDepthWrite))
  {
    glDepthMask (GL_TRUE);
  }
  else
  {
    glDepthMask (GL_FALSE);
  }

  // render priority list
  myPriorityList.Render (AWorkspace);
}
