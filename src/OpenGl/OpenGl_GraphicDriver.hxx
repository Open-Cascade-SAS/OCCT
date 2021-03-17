// Created on: 2011-10-20
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2014 OPEN CASCADE SAS
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

#ifndef _OpenGl_GraphicDriver_HeaderFile
#define _OpenGl_GraphicDriver_HeaderFile

#include <Graphic3d_GraphicDriver.hxx>

#include <Aspect_Display.hxx>
#include <Aspect_DisplayConnection.hxx>
#include <Aspect_RenderingContext.hxx>
#include <Graphic3d_CView.hxx>
#include <Graphic3d_CStructure.hxx>
#include <NCollection_DataMap.hxx>
#include <OpenGl_Context.hxx>

class Aspect_Window;
class Quantity_Color;
class OpenGl_Structure;
class OpenGl_View;
class OpenGl_Window;

//! Tool class to implement consistent state counter
//! for objects inside the same driver instance.
class OpenGl_StateCounter
{
public:

  OpenGl_StateCounter() : myCounter (0) { }

  Standard_Size Increment() { return ++myCounter; }

private:

  Standard_Size myCounter;
};

//! This class defines an OpenGl graphic driver
class OpenGl_GraphicDriver : public Graphic3d_GraphicDriver
{
  DEFINE_STANDARD_RTTIEXT(OpenGl_GraphicDriver, Graphic3d_GraphicDriver)
public:

  //! Constructor.
  //! @param theDisp connection to display, required on Linux but optional on other systems
  //! @param theToInitialize perform initialization of default OpenGL context on construction
  Standard_EXPORT OpenGl_GraphicDriver (const Handle(Aspect_DisplayConnection)& theDisp,
                                        const Standard_Boolean                  theToInitialize = Standard_True);

  //! Destructor.
  Standard_EXPORT virtual ~OpenGl_GraphicDriver();

  //! Release default context.
  Standard_EXPORT void ReleaseContext();

  //! Perform initialization of default OpenGL context.
  Standard_EXPORT Standard_Boolean InitContext();

  //! Initialize default OpenGL context using existing one.
  //! @param theEglDisplay EGL connection to the Display
  //! @param theEglContext EGL rendering context
  //! @param theEglConfig  EGL configuration
  Standard_EXPORT Standard_Boolean InitEglContext (Aspect_Display          theEglDisplay,
                                                   Aspect_RenderingContext theEglContext,
                                                   void*                   theEglConfig);

  //! Request limit of graphic resource of specific type.
  Standard_EXPORT virtual Standard_Integer InquireLimit (const Graphic3d_TypeOfLimit theType) const Standard_OVERRIDE;

public:

  Standard_EXPORT virtual Handle(Graphic3d_CStructure) CreateStructure (const Handle(Graphic3d_StructureManager)& theManager) Standard_OVERRIDE;

  Standard_EXPORT virtual void RemoveStructure (Handle(Graphic3d_CStructure)& theCStructure) Standard_OVERRIDE;

  Standard_EXPORT virtual Handle(Graphic3d_CView) CreateView (const Handle(Graphic3d_StructureManager)& theMgr) Standard_OVERRIDE;

  Standard_EXPORT virtual void RemoveView (const Handle(Graphic3d_CView)& theView) Standard_OVERRIDE;

  Standard_EXPORT virtual Handle(OpenGl_Window) CreateRenderWindow (const Handle(Aspect_Window)& theWindow, const Aspect_RenderingContext theContext);

public:

  Standard_EXPORT void TextSize (const Handle(Graphic3d_CView)& theView,
                                 const Standard_CString         theText,
                                 const Standard_ShortReal       theHeight,
                                 Standard_ShortReal&            theWidth,
                                 Standard_ShortReal&            theAscent,
                                 Standard_ShortReal&            theDescent) const Standard_OVERRIDE;

  Standard_EXPORT Standard_ShortReal DefaultTextHeight() const Standard_OVERRIDE;

  Standard_EXPORT Standard_Boolean ViewExists (const Handle(Aspect_Window)& theWindow, Handle(Graphic3d_CView)& theView) Standard_OVERRIDE;

public:

  //! Adds a layer to all views.
  //! @param theLayerId    [in] id of new layer, should be > 0 (negative values are reserved for default layers).
  //! @param theSettings   [in] new layer settings
  //! @param theLayerAfter [in] id of layer to append new layer before
  Standard_EXPORT virtual void InsertLayerBefore (const Graphic3d_ZLayerId theNewLayerId,
                                                  const Graphic3d_ZLayerSettings& theSettings,
                                                  const Graphic3d_ZLayerId theLayerAfter) Standard_OVERRIDE;

  //! Adds a layer to all views.
  //! @param theLayerId     [in] id of created layer
  //! @param theSettings    [in] new layer settings
  //! @param theLayerBefore [in] id of layer to append new layer after
  Standard_EXPORT virtual void InsertLayerAfter (const Graphic3d_ZLayerId theNewLayerId,
                                                 const Graphic3d_ZLayerSettings& theSettings,
                                                 const Graphic3d_ZLayerId theLayerBefore) Standard_OVERRIDE;

  //! Removes Z layer. All structures displayed at the moment in layer will be displayed in
  //! default layer (the bottom-level z layer). By default, there are always default
  //! bottom-level layer that can't be removed.  The passed theLayerId should be not less than 0
  //! (reserved for default layers that can not be removed).
  Standard_EXPORT void RemoveZLayer (const Graphic3d_ZLayerId theLayerId) Standard_OVERRIDE;

  //! Sets the settings for a single Z layer.
  Standard_EXPORT void SetZLayerSettings (const Graphic3d_ZLayerId theLayerId, const Graphic3d_ZLayerSettings& theSettings) Standard_OVERRIDE;

public:

  //! @return the visualization options
  inline const OpenGl_Caps& Options() const
  {
    return *myCaps.operator->();
  }

  //! @return the visualization options
  inline OpenGl_Caps& ChangeOptions()
  {
    return *myCaps.operator->();
  }

  //! Specify swap buffer behavior.
  Standard_EXPORT void SetBuffersNoSwap (const Standard_Boolean theIsNoSwap);

  //! VBO usage can be forbidden by this method even if it is supported by GL driver.
  //! Notice that disabling of VBO will cause rendering performance degradation.
  //! Warning! This method should be called only before any primitives are displayed in GL scene!
  Standard_EXPORT virtual void EnableVBO (const Standard_Boolean theToTurnOn) Standard_OVERRIDE;


  //! Returns TRUE if vertical synchronization with display refresh rate (VSync) should be used; TRUE by default.
  Standard_EXPORT virtual bool IsVerticalSync() const Standard_OVERRIDE;

  //! Set if vertical synchronization with display refresh rate (VSync) should be used.
  Standard_EXPORT virtual void SetVerticalSync (bool theToEnable) Standard_OVERRIDE;

  //! Returns information about GPU memory usage.
  //! Please read OpenGl_Context::MemoryInfo() for more description.
  Standard_EXPORT Standard_Boolean MemoryInfo (Standard_Size&           theFreeBytes,
                                               TCollection_AsciiString& theInfo) const Standard_OVERRIDE;

public:

  //! Method to retrieve valid GL context.
  //! Could return NULL-handle if no window created by this driver.
  //! @param theBound if TRUE then currently bound context will be returned,
  //!                 any context will be returned otherwise
  Standard_EXPORT const Handle(OpenGl_Context)& GetSharedContext (bool theBound = false) const;

  Aspect_Display          getRawGlDisplay() const { return myEglDisplay; }
  Aspect_RenderingContext getRawGlContext() const { return myEglContext;  }
  void*                   getRawGlConfig()  const { return myEglConfig; }

  //! Set device lost flag for redrawn views.
  Standard_EXPORT void setDeviceLost();

public:

  //! State counter for OpenGl structures.
  OpenGl_StateCounter* GetStateCounter() const { return &myStateCounter; }

  //! Returns unique ID for primitive arrays.
  Standard_Size GetNextPrimitiveArrayUID() const { return myUIDGenerator.Increment(); }

protected:

  //! Choose default visual for new windows created by Aspect_DisplayConnection.
  Standard_EXPORT void chooseVisualInfo();

protected:

  Standard_Boolean        myIsOwnContext; //!< indicates that shared context has been created within OpenGl_GraphicDriver

  Aspect_Display          myEglDisplay;   //!< EGL connection to the Display : EGLDisplay
  Aspect_RenderingContext myEglContext;   //!< EGL rendering context         : EGLContext
  void*                   myEglConfig;    //!< EGL configuration             : EGLConfig

  Handle(OpenGl_Caps)                                      myCaps;
  NCollection_Map<Handle(OpenGl_View)>                     myMapOfView;
  NCollection_DataMap<Standard_Integer, OpenGl_Structure*> myMapOfStructure;

  mutable OpenGl_StateCounter myStateCounter; //!< State counter for OpenGl structures.
  mutable OpenGl_StateCounter myUIDGenerator; //!< Unique ID counter for primitive arrays.

};

DEFINE_STANDARD_HANDLE(OpenGl_GraphicDriver, Graphic3d_GraphicDriver)

#endif //_OpenGl_GraphicDriver_HeaderFile
