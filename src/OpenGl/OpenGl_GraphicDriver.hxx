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
#include <Handle_OpenGl_GraphicDriver.hxx>
#include <OpenGl_Context.hxx>
#include <OpenGl_PrinterContext.hxx>

#include <Standard_CString.hxx>

#include <Handle_TColStd_HArray1OfByte.hxx>
#include <Handle_TColStd_HArray1OfReal.hxx>
#include <Quantity_PlaneAngle.hxx>
#include <Quantity_NameOfColor.hxx>
#include <Handle_OpenGl_View.hxx>
#include <Handle_OpenGl_Workspace.hxx>

#include <Aspect_Display.hxx>
#include <Aspect_DisplayConnection.hxx>
#include <Aspect_GradientFillMethod.hxx>
#include <Aspect_FillMethod.hxx>
#include <Aspect_CLayer2d.hxx>
#include <Aspect_TypeOfTriedronPosition.hxx>
#include <Aspect_TypeOfTriedronEcho.hxx>
#include <Aspect_Handle.hxx>
#include <Aspect_PrintAlgo.hxx>
#include <gp_Ax2.hxx>
#include <Graphic3d_CView.hxx>
#include <Graphic3d_CStructure.hxx>
#include <Graphic3d_TextPath.hxx>
#include <Graphic3d_HorizontalTextAlignment.hxx>
#include <Graphic3d_VerticalTextAlignment.hxx>
#include <Graphic3d_CUserDraw.hxx>
#include <Graphic3d_GraduatedTrihedron.hxx>
#include <Graphic3d_TypeOfComposition.hxx>
#include <Graphic3d_ExportFormat.hxx>
#include <Graphic3d_SortType.hxx>
#include <Graphic3d_PtrFrameBuffer.hxx>
#include <Graphic3d_Array1OfVertex.hxx>
#include <Graphic3d_Array2OfVertex.hxx>
#include <Graphic3d_BufferType.hxx>
#include <NCollection_DataMap.hxx>

class TColStd_Array1OfInteger;
class TColStd_Array1OfReal;
class TColStd_Array2OfReal;
class TColStd_HArray1OfByte;
class Graphic3d_Vector;
class Quantity_Color;
class Graphic3d_Vertex;
class TCollection_ExtendedString;
class Image_PixMap;
class TColStd_HArray1OfReal;
class Handle(OpenGl_Workspace);
class OpenGl_Element;
class OpenGl_Structure;
class OpenGl_Text;

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

#if defined(HAVE_EGL) || defined(__ANDROID__)
  //! Initialize default OpenGL context using existing one.
  //! @param theEglDisplay EGL connection to the Display
  //! @param theEglContext EGL rendering context
  //! @param theEglConfig  EGL configuration
  Standard_EXPORT Standard_Boolean InitEglContext (Aspect_Display          theEglDisplay,
                                                   Aspect_RenderingContext theEglContext,
                                                   void*                   theEglConfig);
#endif

  Standard_EXPORT Standard_Integer InquireLightLimit ();
  Standard_EXPORT Standard_Integer InquireViewLimit ();

public: // Methods for graphical structures

  Standard_EXPORT void DisplayStructure (const Graphic3d_CView&             theCView,
                                         const Handle(Graphic3d_Structure)& theStructure,
                                         const Standard_Integer             thePriority);
  Standard_EXPORT void EraseStructure (const Graphic3d_CView&             theCView,
                                       const Handle(Graphic3d_Structure)& theStructure);
  Standard_EXPORT void RemoveStructure (Handle(Graphic3d_CStructure)& theCStructure);
  Standard_EXPORT Handle(Graphic3d_CStructure) Structure (const Handle(Graphic3d_StructureManager)& theManager);

  Standard_EXPORT Standard_Boolean SetImmediateModeDrawToFront (const Graphic3d_CView& theCView,
                                                                const Standard_Boolean theDrawToFrontBuffer);
  Standard_EXPORT void DisplayImmediateStructure (const Graphic3d_CView&             theCView,
                                                  const Handle(Graphic3d_Structure)& theStructure);
  Standard_EXPORT void EraseImmediateStructure (const Graphic3d_CView&      theCView,
                                                const Graphic3d_CStructure& theCStructure);

public:

  Standard_EXPORT void ActivateView (const Graphic3d_CView& ACView);
  Standard_EXPORT void AntiAliasing (const Graphic3d_CView& ACView, const Standard_Boolean AFlag);
  Standard_EXPORT void Background (const Graphic3d_CView& ACView);
  Standard_EXPORT void GradientBackground (const Graphic3d_CView& ACView, const Quantity_Color& AColor1, const Quantity_Color& AColor2, const Aspect_GradientFillMethod FillStyle);
  Standard_EXPORT void BackgroundImage (const Standard_CString FileName, const Graphic3d_CView& ACView, const Aspect_FillMethod FillStyle);
  Standard_EXPORT void SetBgImageStyle (const Graphic3d_CView& ACView, const Aspect_FillMethod FillStyle);
  Standard_EXPORT void SetBgGradientStyle (const Graphic3d_CView& ACView, const Aspect_GradientFillMethod FillStyle);
  Standard_EXPORT void ClipLimit (const Graphic3d_CView& ACView, const Standard_Boolean AWait);
  Standard_EXPORT void DeactivateView (const Graphic3d_CView& ACView);
  Standard_EXPORT void DepthCueing (const Graphic3d_CView& ACView, const Standard_Boolean AFlag);
  Standard_EXPORT void RatioWindow (const Graphic3d_CView& ACView);
  Standard_EXPORT void Redraw (const Graphic3d_CView& theCView,
                               const Aspect_CLayer2d& theCUnderLayer,
                               const Aspect_CLayer2d& theCOverLayer,
                               const Standard_Integer theX = 0,
                               const Standard_Integer theY = 0,
                               const Standard_Integer theWidth  = 0,
                               const Standard_Integer theHeight = 0);
  Standard_EXPORT void RedrawImmediate (const Graphic3d_CView& theCView,
                                        const Aspect_CLayer2d& theCUnderLayer,
                                        const Aspect_CLayer2d& theCOverLayer);
  Standard_EXPORT void Invalidate (const Graphic3d_CView& theCView);
  Standard_EXPORT void RemoveView (const Graphic3d_CView& ACView);
  Standard_EXPORT void SetLight (const Graphic3d_CView& ACView);
  Standard_EXPORT void SetClipPlanes (const Graphic3d_CView& theCView);
  Standard_EXPORT void SetCamera (const Graphic3d_CView& theCView);
  Standard_EXPORT void SetVisualisation (const Graphic3d_CView& ACView);
  Standard_EXPORT Standard_Boolean View (Graphic3d_CView& ACView);
  Standard_EXPORT void Environment (const Graphic3d_CView& ACView);
  Standard_EXPORT void ZBufferTriedronSetup (const Graphic3d_CView& theCView,
                                             const Quantity_NameOfColor XColor = Quantity_NOC_RED,
                                             const Quantity_NameOfColor YColor = Quantity_NOC_GREEN,
                                             const Quantity_NameOfColor ZColor = Quantity_NOC_BLUE1,
                                             const Standard_Real SizeRatio = 0.8,
                                             const Standard_Real AxisDiametr = 0.05,
                                             const Standard_Integer NbFacettes = 12);
  Standard_EXPORT void TriedronDisplay (const Graphic3d_CView& ACView, const Aspect_TypeOfTriedronPosition APosition = Aspect_TOTP_CENTER, const Quantity_NameOfColor AColor = Quantity_NOC_WHITE, const Standard_Real AScale = 0.02, const Standard_Boolean AsWireframe = Standard_True);
  Standard_EXPORT void TriedronErase (const Graphic3d_CView& ACView);
  Standard_EXPORT void TriedronEcho (const Graphic3d_CView& ACView, const Aspect_TypeOfTriedronEcho AType = Aspect_TOTE_NONE);

  //! Displays Graduated trihedron
  //! @param theView [in] the graphic view
  //! @param theCubic [in] Graduated Trihedon parameters.
  //! @sa OpenGl_GraduatedTrihedron
  //! @sa Graphic3d_GraduatedTrihedron
  Standard_EXPORT void GraduatedTrihedronDisplay (const Graphic3d_CView& theView, const Graphic3d_GraduatedTrihedron& theCubic);

  //! Erases Graduated Trihedron from the view.
  //! theView [in] graphic view
  //! @sa OpenGl_GraduatedTrihedron
  Standard_EXPORT void GraduatedTrihedronErase (const Graphic3d_CView& theView);

  //! Sets minimum and maximum points of scene bounding box for Graduated Trihedron
  //! stored in graphic view object. 
  //! @param theView [in] current graphic view
  //! @param theMin [in] the minimum point of scene.
  //! @param theMax [in] the maximum point of scene.
  //! @sa OpenGl_GraduatedTrihedron
  Standard_EXPORT void GraduatedTrihedronMinMaxValues (const Graphic3d_CView& theView,
                                                       const Graphic3d_Vec3 theMin, const Graphic3d_Vec3 theMax);

  Standard_EXPORT void Layer (Aspect_CLayer2d& ACLayer);
  Standard_EXPORT void RemoveLayer (const Aspect_CLayer2d& ACLayer);
  Standard_EXPORT void BeginLayer (const Aspect_CLayer2d& ACLayer);
  Standard_EXPORT void BeginPolygon2d ();
  Standard_EXPORT void BeginPolyline2d ();
  Standard_EXPORT void ClearLayer (const Aspect_CLayer2d& ACLayer);
  Standard_EXPORT void Draw (const Standard_ShortReal X,const Standard_ShortReal Y);
  Standard_EXPORT void Edge (const Standard_ShortReal X,const Standard_ShortReal Y);
  Standard_EXPORT void EndLayer ();
  Standard_EXPORT void EndPolygon2d ();
  Standard_EXPORT void EndPolyline2d ();
  Standard_EXPORT void Move (const Standard_ShortReal X,const Standard_ShortReal Y);
  Standard_EXPORT void Rectangle (const Standard_ShortReal X,const Standard_ShortReal Y,const Standard_ShortReal Width,const Standard_ShortReal Height);
  Standard_EXPORT void SetColor (const Standard_ShortReal R,const Standard_ShortReal G,const Standard_ShortReal B);
  Standard_EXPORT void SetTransparency (const Standard_ShortReal ATransparency);
  Standard_EXPORT void UnsetTransparency ();
  Standard_EXPORT void SetLineAttributes (const Standard_Integer Type,const Standard_ShortReal Width);

  //! Set text attributes for under-/overlayer.
  //! @param theFontName the name of the font to be used
  //! @param theType     the display type of the text
  //! theR theG theB values define the color of decal or subtitle background
  //! To set the color of the text you can use the SetColor method.
  Standard_EXPORT void SetTextAttributes (const Standard_CString   theFontName,
                                          const Standard_Integer   theType,
                                          const Standard_ShortReal theR,
                                          const Standard_ShortReal theG,
                                          const Standard_ShortReal theB);
  Standard_EXPORT void Text (const Standard_CString AText,const Standard_ShortReal X,const Standard_ShortReal Y,const Standard_ShortReal AHeight);
  Standard_EXPORT void TextSize (const Standard_CString AText,const Standard_ShortReal AHeight,Standard_ShortReal& AWidth,Standard_ShortReal& AnAscent,Standard_ShortReal& ADescent) const;
  Standard_EXPORT void SetBackFacingModel (const Graphic3d_CView& aView);

  //! Print the contents of all layers of the view to the printer.
  //! @param thePrinterDC        pass the PrinterDeviceContext (HDC)
  //! @param theToShowBackground when set to FALSE then print the view without background color (background is white) else set to TRUE for printing with current background color
  //! @param theFileName         if != NULL, then the view will be printed to a file
  //! @param thePrintAlgorithm   select print algorithm: stretch, tile
  //! @param theScaleFactor      scaling coefficient, used internally to scale the printings accordingly to the scale factor selected in the printer properties dialog
  //! @return Standard_True if the data is passed to the printer, otherwise Standard_False if the print operation failed due to the printer errors,
  //! or lack of system memory. This might be related to insufficient memory or some internal errors.
  //! All this errors are indicated by the message boxes (on level of OpenGl_GraphicDriver).
  //! Warning: This function can reuse FBO assigned to the view, please take it into account if you use it for your purposes.
  Standard_EXPORT Standard_Boolean Print (const Graphic3d_CView& theCView,
                                          const Aspect_CLayer2d& theCUnderLayer,
                                          const Aspect_CLayer2d& theCOverLayer,
                                          const Aspect_Handle    thePrinterDC,
                                          const Standard_Boolean theToShowBackground,
                                          const Standard_CString theFileName,
                                          const Aspect_PrintAlgo thePrintAlgorithm = Aspect_PA_STRETCH,
                                          const Standard_Real    theScaleFactor = 1.0) const;
  Standard_EXPORT void SetDepthTestEnabled (const Graphic3d_CView& view,const Standard_Boolean isEnabled) const;
  Standard_EXPORT Standard_Boolean IsDepthTestEnabled (const Graphic3d_CView& view) const;

  //! Reads depths of shown pixels of the given rectangle (glReadPixels with GL_DEPTH_COMPONENT)
  Standard_EXPORT void ReadDepths (const Graphic3d_CView& view,const Standard_Integer x,const Standard_Integer y,const Standard_Integer width,const Standard_Integer height,const Standard_Address buffer) const;

  //! Generate offscreen FBO (needs OpenGL2+ hardware).
  //! If not supported on hardware returns NULL.
  Standard_EXPORT Graphic3d_PtrFrameBuffer FBOCreate (const Graphic3d_CView& view, const Standard_Integer width, const Standard_Integer height);

  //! Remove offscreen FBO
  Standard_EXPORT void FBORelease (const Graphic3d_CView& view, Graphic3d_PtrFrameBuffer& fboPtr);

  //! Dump active rendering buffer into specified memory buffer.
  Standard_EXPORT Standard_Boolean BufferDump (const Graphic3d_CView&      theCView,
                                               Image_PixMap&               theImage,
                                               const Graphic3d_BufferType& theBufferType);

  Standard_EXPORT void SetGLLightEnabled (const Graphic3d_CView& view,const Standard_Boolean isEnabled) const;

  Standard_EXPORT Standard_Boolean IsGLLightEnabled (const Graphic3d_CView& view) const;

  Standard_EXPORT Standard_Integer InquirePlaneLimit();

  Standard_EXPORT Standard_ShortReal DefaultTextHeight() const;

  Standard_EXPORT void FBOGetDimensions(const Graphic3d_CView& view,const Graphic3d_PtrFrameBuffer fboPtr,Standard_Integer& width,Standard_Integer& height,Standard_Integer& widthMax,Standard_Integer& heightMax);

  Standard_EXPORT void FBOChangeViewport(const Graphic3d_CView& view,Graphic3d_PtrFrameBuffer& fboPtr,const Standard_Integer width,const Standard_Integer height);

  Standard_EXPORT Standard_Boolean Export(const Standard_CString theFileName,const Graphic3d_ExportFormat theFormat,const Graphic3d_SortType theSortType,const Standard_Integer theWidth,const Standard_Integer theHeight,const Graphic3d_CView& theView,const Aspect_CLayer2d& theLayerUnder,const Aspect_CLayer2d& theLayerOver,const Standard_Real thePrecision = 0.005,const Standard_Address theProgressBarFunc = NULL,const Standard_Address theProgressObject = NULL);

  //! Add a new top-level z layer with ID theLayerId for the view. Z layers allow drawing structures in higher layers in foreground of structures in lower layers.
  //! To add a structure to desired layer on display it is necessary to set the layer index for the structure.
  Standard_EXPORT void AddZLayer (const Graphic3d_CView&   theCView,
                                  const Graphic3d_ZLayerId theLayerId);

  //! Remove Z layer from the specified view.
  //! All structures displayed at the moment in layer will be displayed in default layer (the bottom-level z layer).
  //! To unset layer index from associated structures use method UnsetZLayer (...).
  Standard_EXPORT void RemoveZLayer (const Graphic3d_CView&   theCView,
                                     const Graphic3d_ZLayerId theLayerId);

  //! Unset Z layer ID for all structures.
  //! The structure indexes will be set to default layer (the bottom-level z layer with ID = 0).
  Standard_EXPORT void UnsetZLayer (const Graphic3d_ZLayerId theLayerId);

  //! Change Z layer of a structure already presented in view.
  //! It is recommended to update z layer of already displayed structures with this method before setting new z layer index to the structure.
  //! This is usually done by viewer manager.
  Standard_EXPORT void ChangeZLayer (const Graphic3d_CStructure& theCStructure,
                                     const Graphic3d_CView&      theCView,
                                     const Graphic3d_ZLayerId    theNewLayerId);

  //! Sets the settings for a single Z layer of specified view.
  Standard_EXPORT void SetZLayerSettings (const Graphic3d_CView&          theCView,
                                          const Graphic3d_ZLayerId        theLayerId,
                                          const Graphic3d_ZLayerSettings& theSettings);

  //! Changes priority of a structure within its Z layer for the specified view.
  Standard_EXPORT void ChangePriority (const Graphic3d_CStructure& theCStructure,
                                       const Graphic3d_CView& theCView,
                                       const Standard_Integer theNewPriority);

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

  //! VBO usage can be forbidden by this method even if it is supported by GL driver.
  //! Notice that disabling of VBO will cause rendering performance degradation.
  //! Warning! This method should be called only before any primitives are displayed in GL scene!
  Standard_EXPORT void EnableVBO (const Standard_Boolean theToTurnOn);

  //! Returns information about GPU memory usage.
  //! Please read OpenGl_Context::MemoryInfo() for more description.
  Standard_EXPORT Standard_Boolean MemoryInfo (Standard_Size&           theFreeBytes,
                                               TCollection_AsciiString& theInfo) const;

  //! UserDraw function prototype
  typedef OpenGl_Element* (*OpenGl_UserDrawCallback_t )(const CALL_DEF_USERDRAW* );

  //! Method to setup UserDraw callback
  Standard_EXPORT OpenGl_UserDrawCallback_t& UserDrawCallback();

public:

  //! Method to retrieve valid GL context.
  //! Could return NULL-handle if no window created by this driver.
  Standard_EXPORT const Handle(OpenGl_Context)& GetSharedContext() const;

  //! Marks BVH tree for given priority list as dirty and
  //! marks primitive set for rebuild.
  Standard_EXPORT void InvalidateBVHData (Graphic3d_CView& theCView, const Standard_Integer theLayerId);

#if defined(HAVE_EGL) || defined(__ANDROID__)
  Aspect_Display          getRawGlDisplay() const { return myEglDisplay; }
  Aspect_RenderingContext getRawGlContext() const { return myEglContext;  }
  void*                   getRawGlConfig()  const { return myEglConfig; }
#endif

public:

  DEFINE_STANDARD_RTTI(OpenGl_GraphicDriver)

private:

  Standard_Boolean        myIsOwnContext; //!< indicates that shared context has been created within OpenGl_GraphicDriver
#if defined(HAVE_EGL) || defined(__ANDROID__)
  Aspect_Display          myEglDisplay;   //!< EGL connection to the Display : EGLDisplay
  Aspect_RenderingContext myEglContext;   //!< EGL rendering context         : EGLContext
  void*                   myEglConfig;    //!< EGL configuration             : EGLConfig
#endif

  Handle(OpenGl_Caps)                                             myCaps;
  NCollection_Map<Handle(OpenGl_View)>                            myMapOfView;
  NCollection_Map<Handle(OpenGl_Workspace)>                       myMapOfWS;
  NCollection_DataMap<Standard_Integer, OpenGl_Structure*>        myMapOfStructure;
  mutable Handle(OpenGl_PrinterContext)                           myPrintContext;
  OpenGl_UserDrawCallback_t                                       myUserDrawCallback;
  OpenGl_Text*                                                    myTempText;         //!< variable for compatibility (drawing text in layers)

public:

  //! State counter for OpenGl structures.
  OpenGl_StateCounter* GetStateCounter() const { return &myStateCounter; }

  //! Returns unique ID for primitive arrays.
  const Standard_Size GetNextPrimitiveArrayUID() const { return myUIDGenerator.Increment(); }

private:

  mutable OpenGl_StateCounter myStateCounter; //!< State counter for OpenGl structures.
  mutable OpenGl_StateCounter myUIDGenerator; //!< Unique ID counter for primitive arrays.

};

#endif //_OpenGl_GraphicDriver_HeaderFile
