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

#ifndef _OpenGl_Layer_Header
#define _OpenGl_Layer_Header

#include <OpenGl_PriorityList.hxx>
#include <Graphic3d_ZLayerSettings.hxx>
#include <OpenGl_GlCore11.hxx>

class Handle(OpenGl_Workspace);

struct OpenGl_GlobalLayerSettings
{
  GLint DepthFunc;
  GLboolean DepthMask;
};

class OpenGl_Layer
{
public:

  //! Initializes associated priority list and layer properties
  OpenGl_Layer (const Standard_Integer theNbPriorities = 11);

  //! Returns settings of the layer object.
  const Graphic3d_ZLayerSettings LayerSettings() const { return myLayerSettings; };

  //! Sets settings of the layer object.
  void SetLayerSettings (Graphic3d_ZLayerSettings theSettings)
  {
    myLayerSettings = theSettings;
  }

  //! Returns true if theSetting is enabled for the layer.
  const Standard_Boolean IsSettingEnabled (const Graphic3d_ZLayerSetting theSetting) const
  {
    return myLayerSettings.IsSettingEnabled (theSetting);
  }

  //! Returns reference to associated priority list.
  OpenGl_PriorityList& PriorityList() { return myPriorityList; }

  //! Returns const reference to associated priority list.
  const OpenGl_PriorityList& PriorityList() const { return myPriorityList; }

  void Render (const Handle(OpenGl_Workspace) &AWorkspace, const OpenGl_GlobalLayerSettings& theDefaultSettings) const;

private:

  OpenGl_PriorityList myPriorityList;       //!< Associated priority list object.

  Graphic3d_ZLayerSettings myLayerSettings; //!< Layer setting flags.
};
#endif //_OpenGl_Layer_Header
