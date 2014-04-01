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

#ifndef _OpenGl_Layer_Header
#define _OpenGl_Layer_Header

#include <OpenGl_PriorityList.hxx>

class Handle(OpenGl_Workspace);

enum OpenGl_LayerSetting
{
  OpenGl_LayerDepthTest = 1,
  OpenGl_LayerDepthWrite = 2,
  OpenGl_LayerDepthClear = 4,
  OpenGl_LayerDepthOffset = 8
};

struct OpenGl_LayerSettings
{
  //! Initializes settings
  OpenGl_LayerSettings();

  //! Returns true if theSetting is enabled.
  const Standard_Boolean IsSettingEnabled (const OpenGl_LayerSetting theSetting) const
  {
    return (Flags & theSetting) == theSetting;
  }

  Standard_ShortReal DepthOffsetFactor; //!< Factor argument value for OpenGl glPolygonOffset function.
  Standard_ShortReal DepthOffsetUnits;  //!< Units argument value for OpenGl glPolygonOffset function.

  Standard_Integer Flags; //!< Storage field for settings.
};

class OpenGl_Layer
{
public:

  //! Initializes associated priority list and layer properties
  OpenGl_Layer (const Standard_Integer theNbPriorities = 11);

  //! Returns settings of the layer object.
  const OpenGl_LayerSettings LayerSettings() const { return myLayerSettings; };

  //! Sets settings of the layer object.
  void SetLayerSettings (OpenGl_LayerSettings theSettings)
  {
    myLayerSettings = theSettings;
  }

  //! Returns true if theSetting is enabled for the layer.
  const Standard_Boolean IsSettingEnabled (const OpenGl_LayerSetting theSetting) const
  {
    return myLayerSettings.IsSettingEnabled (theSetting);
  }

  //! Returns reference to associated priority list.
  OpenGl_PriorityList& PriorityList() { return myPriorityList; }

  //! Returns const reference to associated priority list.
  const OpenGl_PriorityList& PriorityList() const { return myPriorityList; }

  void Render (const Handle(OpenGl_Workspace) &AWorkspace) const;

private:

  OpenGl_PriorityList myPriorityList;   //!< Associated priority list object.

  OpenGl_LayerSettings myLayerSettings; //!< Layer setting flags.
};
#endif //_OpenGl_Layer_Header
