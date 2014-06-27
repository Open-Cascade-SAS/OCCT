// Created on: 2014-03-31
// Created by: Danila ULYANOV
// Copyright (c) 2014 OPEN CASCADE SAS
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

#include <OpenGl_Layer.hxx>
#include <OpenGl_Workspace.hxx>
#include <OpenGl_GlCore11.hxx>

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
void OpenGl_Layer::Render (const Handle(OpenGl_Workspace) &theWorkspace, const OpenGl_GlobalLayerSettings& theDefaultSettings) const
{
  TEL_POFFSET_PARAM anAppliedOffsetParams = theWorkspace->AppliedPolygonOffset();

  // separate depth buffers
  if (IsSettingEnabled (Graphic3d_ZLayerDepthClear))
  {
    glClear (GL_DEPTH_BUFFER_BIT);
  }
 
  // handle depth test
  if (IsSettingEnabled (Graphic3d_ZLayerDepthTest))
  {
    // assuming depth test is enabled by default
    glDepthFunc (theDefaultSettings.DepthFunc);
  }
  else
  {
    glDepthFunc (GL_ALWAYS);
  }
  
  // handle depth offset
  if (IsSettingEnabled (Graphic3d_ZLayerDepthOffset))
  {
    theWorkspace->SetPolygonOffset (Aspect_POM_Fill,
                                    myLayerSettings.DepthOffsetFactor,
                                    myLayerSettings.DepthOffsetUnits);
  }
  else
  {
    theWorkspace->SetPolygonOffset (anAppliedOffsetParams.mode,
                                    anAppliedOffsetParams.factor,
                                    anAppliedOffsetParams.units);
  }

  // handle depth write
  if (IsSettingEnabled (Graphic3d_ZLayerDepthWrite))
  {
    glDepthMask (GL_TRUE);
  }
  else
  {
    glDepthMask (GL_FALSE);
  }

  // render priority list
  myPriorityList.Render (theWorkspace);

  // always restore polygon offset between layers rendering
  theWorkspace->SetPolygonOffset (anAppliedOffsetParams.mode,
                                  anAppliedOffsetParams.factor,
                                  anAppliedOffsetParams.units);
}
