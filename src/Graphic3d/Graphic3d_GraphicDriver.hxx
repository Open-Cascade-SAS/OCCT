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

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <Aspect_DisplayConnection.hxx>
#include <Standard_Boolean.hxx>
#include <MMgt_TShared.hxx>
#include <Graphic3d_CView.hxx>
#include <Graphic3d_CStructure.hxx>
#include <Aspect_GradientFillMethod.hxx>
#include <Standard_CString.hxx>
#include <Aspect_FillMethod.hxx>
#include <Standard_Size.hxx>
#include <Quantity_NameOfColor.hxx>
#include <Standard_Real.hxx>
#include <Aspect_TypeOfTriedronPosition.hxx>
#include <Aspect_TypeOfTriedronEcho.hxx>
#include <Graphic3d_GraduatedTrihedron.hxx>
#include <Graphic3d_Vec3.hxx>
#include <Standard_ShortReal.hxx>
#include <Standard_Address.hxx>
#include <Graphic3d_PtrFrameBuffer.hxx>
#include <Image_PixMap.hxx>
#include <Graphic3d_BufferType.hxx>
#include <Aspect_Handle.hxx>
#include <Aspect_PrintAlgo.hxx>
#include <Graphic3d_ExportFormat.hxx>
#include <Graphic3d_SortType.hxx>
#include <Graphic3d_ZLayerId.hxx>
#include <Graphic3d_ZLayerSettings.hxx>
#include <Graphic3d_CLight.hxx>
#include <TColStd_Array2OfReal.hxx>
class Graphic3d_TransformError;
class Graphic3d_Structure;
class Graphic3d_StructureManager;
class Quantity_Color;
class TCollection_AsciiString;


class Graphic3d_GraphicDriver;
DEFINE_STANDARD_HANDLE(Graphic3d_GraphicDriver, MMgt_TShared)

//! This class allows the definition of a graphic driver
//! for 3d interface (currently only OpenGl driver is used).
class Graphic3d_GraphicDriver : public MMgt_TShared
{

public:

  
  //! call_togl_inquirelight
  Standard_EXPORT virtual Standard_Integer InquireLightLimit() = 0;
  
  //! call_togl_inquireplane
  Standard_EXPORT virtual Standard_Integer InquirePlaneLimit() = 0;
  
  //! call_togl_inquireview
  Standard_EXPORT virtual Standard_Integer InquireViewLimit() = 0;
  
  //! call_togl_displaystructure
  Standard_EXPORT virtual void DisplayStructure (const Graphic3d_CView& theCView, const Handle(Graphic3d_Structure)& theStructure, const Standard_Integer thePriority) = 0;
  
  //! call_togl_erasestructure
  Standard_EXPORT virtual void EraseStructure (const Graphic3d_CView& theCView, const Handle(Graphic3d_Structure)& theStructure) = 0;
  
  //! call_togl_removestructure
  Standard_EXPORT virtual void RemoveStructure (Handle(Graphic3d_CStructure)& theCStructure) = 0;
  
  //! Creates new empty graphic structure
  Standard_EXPORT virtual Handle(Graphic3d_CStructure) Structure (const Handle(Graphic3d_StructureManager)& theManager) = 0;
  
  //! call_togl_activateview
  Standard_EXPORT virtual void ActivateView (const Graphic3d_CView& ACView) = 0;
  
  //! call_togl_antialiasing
  Standard_EXPORT virtual void AntiAliasing (const Graphic3d_CView& ACView, const Standard_Boolean AFlag) = 0;
  
  //! call_togl_background
  Standard_EXPORT virtual void Background (const Graphic3d_CView& ACView) = 0;
  
  //! call_togl_gradient_background
  Standard_EXPORT virtual void GradientBackground (const Graphic3d_CView& ACView, const Quantity_Color& AColor1, const Quantity_Color& AColor2, const Aspect_GradientFillMethod FillStyle) = 0;
  
  Standard_EXPORT virtual void BackgroundImage (const Standard_CString FileName, const Graphic3d_CView& ACView, const Aspect_FillMethod FillStyle) = 0;
  
  Standard_EXPORT virtual void SetBgImageStyle (const Graphic3d_CView& ACView, const Aspect_FillMethod FillStyle) = 0;
  
  Standard_EXPORT virtual void SetBgGradientStyle (const Graphic3d_CView& ACView, const Aspect_GradientFillMethod FillStyle) = 0;
  
  //! call_togl_cliplimit
  Standard_EXPORT virtual void ClipLimit (const Graphic3d_CView& ACView, const Standard_Boolean AWait) = 0;
  
  //! call_togl_deactivateview
  Standard_EXPORT virtual void DeactivateView (const Graphic3d_CView& ACView) = 0;
  
  //! call_togl_cliplimit
  Standard_EXPORT virtual void DepthCueing (const Graphic3d_CView& ACView, const Standard_Boolean AFlag) = 0;
  
  //! call_togl_ratio_window
  Standard_EXPORT virtual void RatioWindow (const Graphic3d_CView& ACView) = 0;
  
  //! Redraw content of the view
  Standard_EXPORT virtual void Redraw (const Graphic3d_CView& theCView, const Standard_Integer theX = 0, const Standard_Integer theY = 0, const Standard_Integer theWidth = 0, const Standard_Integer theHeight = 0) = 0;
  
  //! Redraw layer of immediate presentations
  Standard_EXPORT virtual void RedrawImmediate (const Graphic3d_CView& theCView) = 0;
  
  //! Invalidates content of the view but does not redraw it
  Standard_EXPORT virtual void Invalidate (const Graphic3d_CView& theCView) = 0;

  //! Returns true if cached view content has been invalidated.
  Standard_EXPORT virtual Standard_Boolean IsInvalidated (const Graphic3d_CView& theCView) const = 0;

  //! call_togl_removeview
  Standard_EXPORT virtual void RemoveView (const Graphic3d_CView& ACView) = 0;
  
  //! call_togl_setlight
  Standard_EXPORT virtual void SetLight (const Graphic3d_CView& ACView) = 0;
  
  //! Pass clip planes to the associated graphic driver view.
  Standard_EXPORT virtual void SetClipPlanes (const Graphic3d_CView& theCView) = 0;
  
  //! Inform graphic driver if camera assigned to view changes.
  Standard_EXPORT virtual void SetCamera (const Graphic3d_CView& theCView) = 0;
  
  //! call_togl_setvisualisation
  Standard_EXPORT virtual void SetVisualisation (const Graphic3d_CView& ACView) = 0;
  
  //! call_togl_view
  Standard_EXPORT virtual Standard_Boolean View (Graphic3d_CView& ACView) = 0;
  
  Standard_EXPORT virtual void Environment (const Graphic3d_CView& ACView) = 0;
  
  //! enables/disables usage of OpenGL vertex buffer arrays while drawing primitiev arrays
  Standard_EXPORT virtual void EnableVBO (const Standard_Boolean status) = 0;
  
  //! Returns information about GPU memory usage.
  Standard_EXPORT virtual Standard_Boolean MemoryInfo (Standard_Size& theFreeBytes, TCollection_AsciiString& theInfo) const = 0;
  
  //! call_togl_ztriedron_setup
  Standard_EXPORT virtual void ZBufferTriedronSetup (const Graphic3d_CView& theCView, const Quantity_NameOfColor XColor = Quantity_NOC_RED, const Quantity_NameOfColor YColor = Quantity_NOC_GREEN, const Quantity_NameOfColor ZColor = Quantity_NOC_BLUE1, const Standard_Real SizeRatio = 0.8, const Standard_Real AxisDiametr = 0.05, const Standard_Integer NbFacettes = 12) = 0;
  
  //! call_togl_triedron_display
  Standard_EXPORT virtual void TriedronDisplay (const Graphic3d_CView& ACView, const Aspect_TypeOfTriedronPosition APosition = Aspect_TOTP_CENTER, const Quantity_NameOfColor AColor = Quantity_NOC_WHITE, const Standard_Real AScale = 0.02, const Standard_Boolean AsWireframe = Standard_True) = 0;
  
  //! call_togl_triedron_erase
  Standard_EXPORT virtual void TriedronErase (const Graphic3d_CView& ACView) = 0;
  
  //! call_togl_triedron_echo
  Standard_EXPORT virtual void TriedronEcho (const Graphic3d_CView& ACView, const Aspect_TypeOfTriedronEcho AType = Aspect_TOTE_NONE) = 0;
  
  //! call_togl_graduatedtrihedron_display
  Standard_EXPORT virtual void GraduatedTrihedronDisplay (const Graphic3d_CView& theView, const Graphic3d_GraduatedTrihedron& theCubic) = 0;
  
  //! call_togl_graduatedtrihedron_erase
  Standard_EXPORT virtual void GraduatedTrihedronErase (const Graphic3d_CView& theView) = 0;
  
  //! Sets minimum and maximum points of scene bounding box for Graduated Trihedron
  //! stored in graphic view object.
  //! @param theView [in] current graphic view
  //! @param theMin [in] the minimum point of scene.
  //! @param theMax [in] the maximum point of scene.
  Standard_EXPORT virtual void GraduatedTrihedronMinMaxValues (const Graphic3d_CView& theView, const Graphic3d_Vec3 theMin, const Graphic3d_Vec3 theMax) = 0;
  
  //! @param theDrawToFrontBuffer Advanced option to modify rendering mode:
  //! 1. TRUE.  Drawing immediate mode structures directly to the front buffer over the scene image.
  //! Fast, so preferred for interactive work (used by default).
  //! However these extra drawings will be missed in image dump since it is performed from back buffer.
  //! Notice that since no pre-buffering used the V-Sync will be ignored and rendering could be seen
  //! in run-time (in case of slow hardware) and/or tearing may appear.
  //! So this is strongly recommended to draw only simple (fast) structures.
  //! 2. FALSE. Drawing immediate mode structures to the back buffer.
  //! The complete scene is redrawn first, so this mode is slower if scene contains complex data and/or V-Sync is turned on.
  //! But it works in any case and is especially useful for view dump because the dump image is read from the back buffer.
  //! @return previous mode.
  Standard_EXPORT virtual Standard_Boolean SetImmediateModeDrawToFront (const Graphic3d_CView& theCView, const Standard_Boolean theDrawToFrontBuffer) = 0;

  Standard_EXPORT virtual Standard_ShortReal DefaultTextHeight() const = 0;
  
  //! call_togl_textsize2d
  Standard_EXPORT virtual void TextSize (const Standard_CString AText, const Standard_ShortReal AHeight, Standard_ShortReal& AWidth, Standard_ShortReal& AnAscent, Standard_ShortReal& ADescent) const = 0;
  
  //! call_togl_backfacing
  Standard_EXPORT virtual void SetBackFacingModel (const Graphic3d_CView& aView) = 0;
  
  //! Reads depths of shown pixels of the given
  //! rectangle (glReadPixels with GL_DEPTH_COMPONENT)
  Standard_EXPORT virtual void ReadDepths (const Graphic3d_CView& view, const Standard_Integer x, const Standard_Integer y, const Standard_Integer width, const Standard_Integer height, const Standard_Address buffer) const = 0;
  
  //! Generate offscreen FBO in the graphic library.
  //! If not supported on hardware returns NULL.
  Standard_EXPORT virtual Graphic3d_PtrFrameBuffer FBOCreate (const Graphic3d_CView& view, const Standard_Integer width, const Standard_Integer height) = 0;
  
  //! Remove offscreen FBO from the graphic library
  Standard_EXPORT virtual void FBORelease (const Graphic3d_CView& view, Graphic3d_PtrFrameBuffer& fboPtr) = 0;
  
  //! Read offscreen FBO configuration.
  Standard_EXPORT virtual void FBOGetDimensions (const Graphic3d_CView& view, const Graphic3d_PtrFrameBuffer fboPtr, Standard_Integer& width, Standard_Integer& height, Standard_Integer& widthMax, Standard_Integer& heightMax) = 0;
  
  //! Change offscreen FBO viewport.
  Standard_EXPORT virtual void FBOChangeViewport (const Graphic3d_CView& view, Graphic3d_PtrFrameBuffer& fboPtr, const Standard_Integer width, const Standard_Integer height) = 0;
  
  //! Dump active rendering buffer into specified memory buffer.
  Standard_EXPORT virtual Standard_Boolean BufferDump (const Graphic3d_CView& theCView, Image_PixMap& theImage, const Graphic3d_BufferType& theBufferType) = 0;
  
  //! call_togl_gllight
  Standard_EXPORT virtual void SetGLLightEnabled (const Graphic3d_CView& view, const Standard_Boolean isEnabled) const = 0;
  
  //! call_togl_isgllight
  Standard_EXPORT virtual Standard_Boolean IsGLLightEnabled (const Graphic3d_CView& view) const = 0;
  
  //! print the contents of all layers of the view to the printer.
  //! <hPrnDC> : Pass the PrinterDeviceContext (HDC),
  //! <showBackground> : When set to FALSE then print the view without background color
  //! (background is white)
  //! else set to TRUE for printing with current background color.
  //! <filename>: If != NULL, then the view will be printed to a file.
  //! <printAlgorithm>: Select print algorithm: stretch, tile.
  //! <theScaleFactor>: Scaling coefficient, used internally to scale the
  //! printings accordingly to the scale factor selected in the printer
  //! properties dialog.
  //! Returns Standard_True if the data is passed to the printer, otherwise
  //! Standard_False if the print operation failed due to the printer errors,
  //! or insufficient system memory available.
  Standard_EXPORT virtual Standard_Boolean Print (const Graphic3d_CView& ACView, const Aspect_Handle hPrnDC, const Standard_Boolean showBackground, const Standard_CString filename, const Aspect_PrintAlgo printAlgorithm = Aspect_PA_STRETCH, const Standard_Real theScaleFactor = 1.0) const = 0;
  

  //! Export scene into the one of the Vector graphics formats (SVG, PS, PDF...).
  //! In contrast to Bitmaps, Vector graphics is scalable (so you may got quality benefits on printing to laser printer).
  //! Notice however that results may differ a lot and do not contain some elements.
  Standard_EXPORT virtual Standard_Boolean Export (const Standard_CString theFileName, const Graphic3d_ExportFormat theFormat, const Graphic3d_SortType theSortType, const Standard_Integer theWidth, const Standard_Integer theHeight, const Graphic3d_CView& theView, const Standard_Real thePrecision = 0.005, const Standard_Address theProgressBarFunc = NULL, const Standard_Address theProgressObject = NULL) = 0;
  

  //! Marks BVH tree and the set of BVH primitives of correspondent priority list with id theLayerId as outdated.
  Standard_EXPORT virtual void InvalidateBVHData (Graphic3d_CView& theCView, const Standard_Integer theLayerId) = 0;
  
  //! Add a new top-level z layer with ID <theLayerId> for
  //! the view. Z layers allow drawing structures in higher layers
  //! in foreground of structures in lower layers. To add a structure
  //! to desired layer on display it is necessary to set the layer
  //! ID for the structure.
  Standard_EXPORT virtual void AddZLayer (const Graphic3d_CView& theCView, const Graphic3d_ZLayerId theLayerId) = 0;
  
  //! Remove Z layer from the specified view. All structures
  //! displayed at the moment in layer will be displayed in default layer
  //! ( the bottom-level z layer ). To unset layer ID from associated
  //! structures use method UnsetZLayer (...).
  Standard_EXPORT virtual void RemoveZLayer (const Graphic3d_CView& theCView, const Graphic3d_ZLayerId theLayerId) = 0;
  
  //! Unset Z layer ID for all structures. The structure
  //! indexes will be set to default layer ( the bottom-level z layer
  //! with ID = 0 ).
  Standard_EXPORT virtual void UnsetZLayer (const Graphic3d_ZLayerId theLayerId) = 0;

  //! Change Z layer of a structure already presented in view.
  Standard_EXPORT virtual void ChangeZLayer (const Graphic3d_CStructure& theCStructure, const Graphic3d_CView& theCView, const Graphic3d_ZLayerId theNewLayerId) = 0;
  
  //! Sets the settings for a single Z layer of specified view.
  Standard_EXPORT virtual void SetZLayerSettings (const Graphic3d_CView& theCView, const Graphic3d_ZLayerId theLayerId, const Graphic3d_ZLayerSettings& theSettings) = 0;

  //! Changes the priority of a structure within its Z layer in the specified view.
  Standard_EXPORT virtual void ChangePriority (const Graphic3d_CStructure& theCStructure, const Graphic3d_CView& theCView, const Standard_Integer theNewPriority) = 0;
  
  Standard_EXPORT void PrintBoolean (const Standard_CString AComment, const Standard_Boolean AValue) const;
  
  Standard_EXPORT void PrintCLight (const Graphic3d_CLight& ACLight, const Standard_Integer AField) const;

  Standard_EXPORT void PrintCStructure (const Graphic3d_CStructure& ACStructure, const Standard_Integer AField) const;
  
  Standard_EXPORT void PrintCView (const Graphic3d_CView& ACView, const Standard_Integer AField) const;
  
  Standard_EXPORT void PrintFunction (const Standard_CString AFunc) const;
  
  Standard_EXPORT void PrintInteger (const Standard_CString AComment, const Standard_Integer AValue) const;
  
  Standard_EXPORT void PrintIResult (const Standard_CString AFunc, const Standard_Integer AResult) const;
  
  Standard_EXPORT void PrintShortReal (const Standard_CString AComment, const Standard_ShortReal AValue) const;
  
  Standard_EXPORT void PrintMatrix (const Standard_CString AComment, const TColStd_Array2OfReal& AMatrix) const;
  
  Standard_EXPORT void PrintString (const Standard_CString AComment, const Standard_CString AString) const;
  
  Standard_EXPORT void SetTrace (const Standard_Integer ALevel);
  
  Standard_EXPORT Standard_Integer Trace() const;
  
  //! returns Handle to display connection
  Standard_EXPORT const Handle(Aspect_DisplayConnection)& GetDisplayConnection() const;
  
  Standard_EXPORT Standard_Boolean IsDeviceLost() const;
  
  Standard_EXPORT void ResetDeviceLostFlag();




  DEFINE_STANDARD_RTTI(Graphic3d_GraphicDriver,MMgt_TShared)

protected:

  
  //! Initialises the Driver
  Standard_EXPORT Graphic3d_GraphicDriver(const Handle(Aspect_DisplayConnection)& theDisp);

  Standard_Integer MyTraceLevel;
  Handle(Aspect_DisplayConnection) myDisplayConnection;
  Standard_Boolean myDeviceLostFlag;


private:




};







#endif // _Graphic3d_GraphicDriver_HeaderFile
