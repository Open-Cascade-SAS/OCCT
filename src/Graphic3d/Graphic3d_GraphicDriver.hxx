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

#ifndef _Graphic3d_GraphicDriver_HeaderFile
#define _Graphic3d_GraphicDriver_HeaderFile

#include <Aspect_GenId.hxx>
#include <Graphic3d_Vec3.hxx>
#include <Graphic3d_CStructure.hxx>
#include <Graphic3d_ZLayerId.hxx>
#include <Graphic3d_ZLayerSettings.hxx>
#include <Graphic3d_TypeOfLimit.hxx>
#include <Standard_CString.hxx>
#include <Standard_Size.hxx>
#include <TColStd_SequenceOfInteger.hxx>

class Aspect_DisplayConnection;
class Aspect_Window;
class Graphic3d_CView;
class Graphic3d_Layer;
class Graphic3d_StructureManager;
class TCollection_AsciiString;

DEFINE_STANDARD_HANDLE(Graphic3d_GraphicDriver, Standard_Transient)

//! This class allows the definition of a graphic driver
//! for 3d interface (currently only OpenGl driver is used).
class Graphic3d_GraphicDriver : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Graphic3d_GraphicDriver, Standard_Transient)
public:

  //! Request limit of graphic resource of specific type.
  virtual Standard_Integer InquireLimit (const Graphic3d_TypeOfLimit theType) const = 0;

  //! Request maximum number of active light sources supported by driver and hardware.
  Standard_Integer InquireLightLimit() const { return InquireLimit (Graphic3d_TypeOfLimit_MaxNbLights); }

  //! Request maximum number of active clipping planes supported by driver and hardware.
  Standard_Integer InquirePlaneLimit() const { return InquireLimit (Graphic3d_TypeOfLimit_MaxNbClipPlanes); }

  //! Request maximum number of views supported by driver.
  Standard_Integer InquireViewLimit() const { return InquireLimit (Graphic3d_TypeOfLimit_MaxNbViews); }

public:

  //! Creates new empty graphic structure
  virtual Handle(Graphic3d_CStructure) CreateStructure (const Handle(Graphic3d_StructureManager)& theManager) = 0;
  
  //! Removes structure from graphic driver and releases its resources.
  virtual void RemoveStructure (Handle(Graphic3d_CStructure)& theCStructure) = 0;
  
  //! Creates new view for this graphic driver.
  virtual Handle(Graphic3d_CView) CreateView (const Handle(Graphic3d_StructureManager)& theMgr) = 0;
  
  //! Removes view from graphic driver and releases its resources.
  virtual void RemoveView (const Handle(Graphic3d_CView)& theView) = 0;

  //! enables/disables usage of OpenGL vertex buffer arrays while drawing primitive arrays
  virtual void EnableVBO (const Standard_Boolean status) = 0;

  //! Returns TRUE if vertical synchronization with display refresh rate (VSync) should be used; TRUE by default.
  virtual bool IsVerticalSync() const = 0;

  //! Set if vertical synchronization with display refresh rate (VSync) should be used.
  virtual void SetVerticalSync (bool theToEnable) = 0;
  
  //! Returns information about GPU memory usage.
  virtual Standard_Boolean MemoryInfo (Standard_Size& theFreeBytes, TCollection_AsciiString& theInfo) const = 0;
  
  virtual Standard_ShortReal DefaultTextHeight() const = 0;
  
  //! Computes text width.
  virtual void TextSize (const Handle(Graphic3d_CView)& theView,
                         const Standard_CString         theText,
                         const Standard_ShortReal       theHeight,
                         Standard_ShortReal&            theWidth,
                         Standard_ShortReal&            theAscent,
                         Standard_ShortReal&            theDescent) const = 0;

  //! Adds a layer to all views.
  //! To add a structure to desired layer on display it is necessary to set the layer ID for the structure.
  //! @param theNewLayerId [in] id of new layer, should be > 0 (negative values are reserved for default layers).
  //! @param theSettings   [in] new layer settings
  //! @param theLayerAfter [in] id of layer to append new layer before
  Standard_EXPORT virtual void InsertLayerBefore (const Graphic3d_ZLayerId theNewLayerId,
                                                  const Graphic3d_ZLayerSettings& theSettings,
                                                  const Graphic3d_ZLayerId theLayerAfter) = 0;

  //! Adds a layer to all views.
  //! @param theNewLayerId  [in] id of new layer, should be > 0 (negative values are reserved for default layers).
  //! @param theSettings    [in] new layer settings
  //! @param theLayerBefore [in] id of layer to append new layer after
  Standard_EXPORT virtual void InsertLayerAfter (const Graphic3d_ZLayerId theNewLayerId,
                                                 const Graphic3d_ZLayerSettings& theSettings,
                                                 const Graphic3d_ZLayerId theLayerBefore) = 0;

  //! Removes Z layer. All structures displayed at the moment in layer will be displayed in
  //! default layer (the bottom-level z layer). By default, there are always default
  //! bottom-level layer that can't be removed.  The passed theLayerId should be not less than 0
  //! (reserved for default layers that can not be removed).
  Standard_EXPORT virtual void RemoveZLayer (const Graphic3d_ZLayerId theLayerId) = 0;

  //! Returns list of Z layers defined for the graphical driver.
  Standard_EXPORT virtual void ZLayers (TColStd_SequenceOfInteger& theLayerSeq) const;

  //! Sets the settings for a single Z layer.
  Standard_EXPORT virtual void SetZLayerSettings (const Graphic3d_ZLayerId theLayerId, const Graphic3d_ZLayerSettings& theSettings) = 0;

  //! Returns the settings of a single Z layer.
  Standard_EXPORT virtual const Graphic3d_ZLayerSettings& ZLayerSettings (const Graphic3d_ZLayerId theLayerId) const;

  //! Returns view associated with the window if it is exists and is activated.
  //! Returns Standard_True if the view associated to the window exists.
  virtual Standard_Boolean ViewExists (const Handle(Aspect_Window)& theWindow, Handle(Graphic3d_CView)& theView) = 0;

  //! returns Handle to display connection
  Standard_EXPORT const Handle(Aspect_DisplayConnection)& GetDisplayConnection() const;

  //! Returns a new identification number for a new structure.
  Standard_EXPORT Standard_Integer NewIdentification();

  //! Frees the identifier of a structure.
  Standard_EXPORT void RemoveIdentification(const Standard_Integer theId);
  
  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;

protected:
  
  //! Initializes the Driver
  Standard_EXPORT Graphic3d_GraphicDriver(const Handle(Aspect_DisplayConnection)& theDisp);

protected:

  Handle(Aspect_DisplayConnection) myDisplayConnection;
  Aspect_GenId myStructGenId;
  NCollection_List<Handle(Graphic3d_Layer)> myLayers;
  NCollection_DataMap<Graphic3d_ZLayerId, Handle(Graphic3d_Layer)> myLayerIds;

};

#endif // _Graphic3d_GraphicDriver_HeaderFile
