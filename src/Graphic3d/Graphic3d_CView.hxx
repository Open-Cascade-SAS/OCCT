// Copyright (c) 2015 OPEN CASCADE SAS
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

#ifndef _Graphic3d_CView_HeaderFile
#define _Graphic3d_CView_HeaderFile

#include <Aspect_Handle.hxx>
#include <Aspect_RenderingContext.hxx>
#include <Aspect_Window.hxx>
#include <Graphic3d_BufferType.hxx>
#include <Graphic3d_Camera.hxx>
#include <Graphic3d_CLight.hxx>
#include <Graphic3d_CStructure.hxx>
#include <Graphic3d_DataStructureManager.hxx>
#include <Graphic3d_DiagnosticInfo.hxx>
#include <Graphic3d_ExportFormat.hxx>
#include <Graphic3d_GraduatedTrihedron.hxx>
#include <Graphic3d_MapOfStructure.hxx>
#include <Graphic3d_NMapOfTransient.hxx>
#include <Graphic3d_RenderingParams.hxx>
#include <Graphic3d_SequenceOfHClipPlane.hxx>
#include <Graphic3d_SequenceOfStructure.hxx>
#include <Graphic3d_SortType.hxx>
#include <Graphic3d_Structure.hxx>
#include <Graphic3d_TextureEnv.hxx>
#include <Graphic3d_TypeOfAnswer.hxx>
#include <Graphic3d_TypeOfBackfacingModel.hxx>
#include <Graphic3d_TypeOfShadingModel.hxx>
#include <Graphic3d_TypeOfVisualization.hxx>
#include <Graphic3d_Vec3.hxx>
#include <Graphic3d_ZLayerId.hxx>
#include <Graphic3d_ZLayerSettings.hxx>
#include <Image_PixMap.hxx>
#include <Quantity_NameOfColor.hxx>
#include <Standard_Address.hxx>
#include <Standard_Transient.hxx>
#include <TColStd_IndexedDataMapOfStringString.hxx>

class Graphic3d_CView;
class Graphic3d_GraphicDriver;
class Graphic3d_StructureManager;

DEFINE_STANDARD_HANDLE (Graphic3d_CView, Graphic3d_DataStructureManager)

//! Base class of a graphical view that carries out rendering process for a concrete
//! implementation of graphical driver. Provides virtual interfaces for redrawing its
//! contents, management of displayed structures and render settings. The source code 
//! of the class itself implements functionality related to management of
//! computed (HLR or "view-dependent") structures.
class Graphic3d_CView : public Graphic3d_DataStructureManager
{
public:

  //! Constructor.
  Standard_EXPORT Graphic3d_CView (const Handle(Graphic3d_StructureManager)& theMgr);

  //! Destructor.
  Standard_EXPORT virtual ~Graphic3d_CView();

  //! Returns the identification number of the view.
  Standard_Integer Identification() const { return myId; }

  //! Activates the view. Maps presentations defined within structure manager onto this view.
  Standard_EXPORT virtual void Activate();

  //! Deactivates the view. Unmaps presentations defined within structure manager.
  //! The view in deactivated state will ignore actions on structures such as Display().
  Standard_EXPORT virtual void Deactivate();

  //! Returns the activity flag of the view.
  Standard_Boolean IsActive() const { return myIsActive; }

  //! Erases the view and removes from graphic driver.
  //! No more graphic operations are allowed in this view after the call.
  Standard_EXPORT virtual void Remove();

  //! Returns true if the view was removed.
  Standard_Boolean IsRemoved() const { return myIsRemoved; }

public:

  //! Returns visualization type of the view.
  Graphic3d_TypeOfVisualization VisualizationType() const { return myVisualization; }

  //! Sets visualization type of the view.
  void SetVisualizationType (const Graphic3d_TypeOfVisualization theType) { myVisualization = theType; }

  //! Switches computed HLR mode in the view
  Standard_EXPORT void SetComputedMode (const Standard_Boolean theMode);

  //! Returns the computed HLR mode state
  Standard_Boolean ComputedMode() const { return myIsInComputedMode; }

  //! Computes the new presentation of the structure  displayed in this view with the type Graphic3d_TOS_COMPUTED.
  Standard_EXPORT void ReCompute (const Handle(Graphic3d_Structure)& theStructure);

  //! Invalidates bounding box of specified ZLayerId.
  Standard_EXPORT void Update (const Graphic3d_ZLayerId theLayerId = Graphic3d_ZLayerId_UNKNOWN);

  //! Computes the new presentation of the structures displayed in this view with the type Graphic3d_TOS_COMPUTED.
  Standard_EXPORT void Compute();

  //! Returns Standard_True if one of the structures displayed in the view contains Polygons, Triangles or Quadrangles.
  Standard_EXPORT Standard_Boolean ContainsFacet() const;

  //! Returns Standard_True if one of the structures in the set contains Polygons, Triangles or Quadrangles.
  Standard_EXPORT Standard_Boolean ContainsFacet (const Graphic3d_MapOfStructure& theSet) const;

  //! Returns the set of structures displayed in this view.
  Standard_EXPORT void DisplayedStructures (Graphic3d_MapOfStructure& theStructures) const;

  //! Returns number of displayed structures in the view.
  virtual Standard_Integer NumberOfDisplayedStructures() const { return myStructsDisplayed.Extent(); }

  //! Returns map of objects hidden within this specific view (not viewer-wise).
  const Handle(Graphic3d_NMapOfTransient)& HiddenObjects() const { return myHiddenObjects; }

  //! Returns map of objects hidden within this specific view (not viewer-wise).
  Handle(Graphic3d_NMapOfTransient)& ChangeHiddenObjects() { return myHiddenObjects; }

  //! Returns Standard_True in case if the structure with the given <theStructId> is
  //! in list of structures to be computed and stores computed struct to <theComputedStruct>.
  Standard_EXPORT Standard_Boolean IsComputed (const Standard_Integer theStructId,
                                               Handle(Graphic3d_Structure)& theComputedStruct) const;

  //! Returns the bounding box of all structures displayed in the view.
  //! If theToIncludeAuxiliary is TRUE, then the boundary box also includes minimum and maximum limits
  //! of graphical elements forming parts of infinite and other auxiliary structures.
  //! @param theToIncludeAuxiliary consider also auxiliary presentations (with infinite flag or with trihedron transformation persistence)
  //! @return computed bounding box
  Standard_EXPORT virtual Bnd_Box MinMaxValues (const Standard_Boolean theToIncludeAuxiliary = Standard_False) const;

  //! Returns the coordinates of the boundary box of all structures in the set <theSet>.
  //! If <theToIgnoreInfiniteFlag> is TRUE, then the boundary box
  //! also includes minimum and maximum limits of graphical elements
  //! forming parts of infinite structures.
  Standard_EXPORT Bnd_Box MinMaxValues (const Graphic3d_MapOfStructure& theSet,
                                        const Standard_Boolean theToIncludeAuxiliary = Standard_False) const;

  //! Returns the structure manager handle which manage structures associated with this view.
  const Handle(Graphic3d_StructureManager)& StructureManager() const { return myStructureManager; }

private:

  friend class Graphic3d_StructureManager;

  //! Is it possible to display the structure in the view?
  Standard_EXPORT Graphic3d_TypeOfAnswer acceptDisplay (const Graphic3d_TypeOfStructure theStructType) const;

  //! Clears the structure in this view.
  Standard_EXPORT void Clear (const Handle(Graphic3d_Structure)& theStructure, const Standard_Boolean theWithDestruction);

  //! Connects the structures.
  Standard_EXPORT void Connect (const Handle(Graphic3d_Structure)& theMother,
                                const Handle(Graphic3d_Structure)& theDaughter);

  //! Disconnects the structures.
  Standard_EXPORT void Disconnect (const Handle(Graphic3d_Structure)& theMother,
                                   const Handle(Graphic3d_Structure)& theDaughter);

  //! Displays the structure in the view.
  Standard_EXPORT void Display (const Handle(Graphic3d_Structure)& theStructure);

  //! Erases the structure from the view.
  Standard_EXPORT void Erase (const Handle(Graphic3d_Structure)& theStructure);

  //! Highlights the structure in the view.
  Standard_EXPORT void Highlight (const Handle(Graphic3d_Structure)& theStructure);

  //! Transforms the structure in the view.
  Standard_EXPORT void SetTransform (const Handle(Graphic3d_Structure)& theStructure,
                                     const Handle(Geom_Transformation)& theTrsf);

  //! Suppress the highlighting on the structure <AStructure>
  //! in the view <me>.
  Standard_EXPORT void UnHighlight (const Handle(Graphic3d_Structure)& theStructure);

  //! Returns an index != 0 if the structure have another structure computed for the view <me>.
  Standard_EXPORT Standard_Integer IsComputed (const Handle(Graphic3d_Structure)& theStructure) const;

  //! Returns true if the structure is displayed in the view.
  Standard_EXPORT Standard_Boolean IsDisplayed (const Handle(Graphic3d_Structure)& theStructure) const;

  //! Changes the display priority of the structure.
  Standard_EXPORT void ChangePriority (const Handle(Graphic3d_Structure)& theStructure,
                                       const Standard_Integer theOldPriority,
                                       const Standard_Integer theNewPriority);

  //! Change Z layer of already displayed structure in the view.
  Standard_EXPORT void ChangeZLayer (const Handle(Graphic3d_Structure)& theStructure,
                                     const Graphic3d_ZLayerId theLayerId);

  //! Returns an index != 0 if the structure have the same owner than another structure
  //! in the sequence of the computed structures.
  Standard_EXPORT Standard_Integer HaveTheSameOwner (const Handle(Graphic3d_Structure)& theStructure) const;

public:

  //! Redraw content of the view.
  virtual void Redraw() = 0;

  //! Redraw immediate content of the view.
  virtual void RedrawImmediate() = 0;

  //! Invalidates content of the view but does not redraw it.
  virtual void Invalidate() = 0;

  //! Return true if view content cache has been invalidated.
  virtual Standard_Boolean IsInvalidated() = 0;

  //! Handle changing size of the rendering window.
  virtual void Resized() = 0;

  //! @param theDrawToFrontBuffer Advanced option to modify rendering mode:
  //! 1. TRUE.  Drawing immediate mode structures directly to the front buffer over the scene image.
  //! Fast, so preferred for interactive work (used by default).
  //! However these extra drawings will be missed in image dump since it is performed from back buffer.
  //! Notice that since no pre-buffering used the V-Sync will be ignored and rendering could be seen
  //! in run-time (in case of slow hardware) and/or tearing may appear.
  //! So this is strongly recommended to draw only simple (fast) structures.
  //! 2. FALSE. Drawing immediate mode structures to the back buffer.
  //! The complete scene is redrawn first, so this mode is slower if scene contains complex data and/or V-Sync
  //! is turned on. But it works in any case and is especially useful for view dump because the dump image is read
  //! from the back buffer.
  //! @return previous mode.
  virtual Standard_Boolean SetImmediateModeDrawToFront (const Standard_Boolean theDrawToFrontBuffer) = 0;

  //! Creates and maps rendering window to the view.
  //! @param theWindow [in] the window.
  //! @param theContext [in] the rendering context. If NULL the context will be created internally.
  virtual void SetWindow (const Handle(Aspect_Window)& theWindow,
                          const Aspect_RenderingContext theContext = NULL) = 0;

  //! Returns the window associated to the view.
  virtual Handle(Aspect_Window) Window() const = 0;

  //! Returns True if the window associated to the view is defined.
  virtual Standard_Boolean IsDefined() const = 0;

  //! Returns data of a graduated trihedron
  virtual const Graphic3d_GraduatedTrihedron& GetGraduatedTrihedron() = 0;

  //! Displays Graduated Trihedron.
  virtual void GraduatedTrihedronDisplay (const Graphic3d_GraduatedTrihedron& theTrihedronData) = 0;

  //! Erases Graduated Trihedron.
  virtual void GraduatedTrihedronErase() = 0;

  //! Sets minimum and maximum points of scene bounding box for Graduated Trihedron stored in graphic view object.
  //! @param theMin [in] the minimum point of scene.
  //! @param theMax [in] the maximum point of scene.
  virtual void GraduatedTrihedronMinMaxValues (const Graphic3d_Vec3 theMin, const Graphic3d_Vec3 theMax) = 0;

  //! Dump active rendering buffer into specified memory buffer.
  virtual Standard_Boolean BufferDump (Image_PixMap& theImage, const Graphic3d_BufferType& theBufferType) = 0;

  //! Export scene into the one of the Vector graphics formats (SVG, PS, PDF...).
  //! In contrast to Bitmaps, Vector graphics is scalable (so you may got quality benefits
  //! on printing to laser printer). Notice however that results may differ a lot and
  //! do not contain some elements.
  virtual Standard_Boolean Export (const Standard_CString theFileName,
                                   const Graphic3d_ExportFormat theFormat,
                                   const Graphic3d_SortType theSortType = Graphic3d_ST_BSP_Tree) = 0;

  //! Marks BVH tree and the set of BVH primitives of correspondent priority list with id theLayerId as outdated.
  virtual void InvalidateBVHData (const Standard_Integer theLayerId) = 0;

  //! Add a new top-level z layer with ID <theLayerId> for
  //! the view. Z layers allow drawing structures in higher layers
  //! in foreground of structures in lower layers. To add a structure
  //! to desired layer on display it is necessary to set the layer
  //! ID for the structure.
  virtual void AddZLayer (const Graphic3d_ZLayerId theLayerId) = 0;

  //! Returns the maximum Z layer ID.
  //! First layer ID is Graphic3d_ZLayerId_Default, last ID is ZLayerMax().
  virtual Standard_Integer ZLayerMax() const = 0;

  //! Returns the bounding box of all structures displayed in the Z layer.
  virtual void InvalidateZLayerBoundingBox (const Graphic3d_ZLayerId theLayerId) const = 0;

  //! Returns the bounding box of all structures displayed in the Z layer.
  //! @param theLayerId            layer identifier
  //! @param theCamera             camera definition
  //! @param theWindowWidth        viewport width  (for applying transformation-persistence)
  //! @param theWindowHeight       viewport height (for applying transformation-persistence)
  //! @param theToIncludeAuxiliary consider also auxiliary presentations (with infinite flag or with trihedron transformation persistence)
  //! @return computed bounding box
  virtual Bnd_Box ZLayerBoundingBox (const Graphic3d_ZLayerId        theLayerId,
                                     const Handle(Graphic3d_Camera)& theCamera,
                                     const Standard_Integer          theWindowWidth,
                                     const Standard_Integer          theWindowHeight,
                                     const Standard_Boolean          theToIncludeAuxiliary) const = 0;

  //! Remove Z layer from the specified view. All structures
  //! displayed at the moment in layer will be displayed in default layer
  //! ( the bottom-level z layer ). To unset layer ID from associated
  //! structures use method UnsetZLayer (...).
  virtual void RemoveZLayer (const Graphic3d_ZLayerId theLayerId) = 0;

  //! Sets the settings for a single Z layer of specified view.
  virtual void SetZLayerSettings (const Graphic3d_ZLayerId theLayerId,
                                  const Graphic3d_ZLayerSettings& theSettings) = 0;

  //! Returns zoom-scale factor.
  Standard_EXPORT Standard_Real ConsiderZoomPersistenceObjects();

  //! Returns pointer to an assigned framebuffer object.
  virtual Handle(Standard_Transient) FBO() const = 0;

  //! Sets framebuffer object for offscreen rendering.
  virtual void SetFBO (const Handle(Standard_Transient)& theFbo) = 0;

  //! Generate offscreen FBO in the graphic library.
  //! If not supported on hardware returns NULL.
  virtual Handle(Standard_Transient) FBOCreate (const Standard_Integer theWidth,
                                                const Standard_Integer theHeight) = 0;

  //! Remove offscreen FBO from the graphic library
  virtual void FBORelease (Handle(Standard_Transient)& theFbo) = 0;

  //! Read offscreen FBO configuration.
  virtual void FBOGetDimensions (const Handle(Standard_Transient)& theFbo,
                                 Standard_Integer& theWidth,
                                 Standard_Integer& theHeight,
                                 Standard_Integer& theWidthMax,
                                 Standard_Integer& theHeightMax) = 0;

  //! Change offscreen FBO viewport.
  virtual void FBOChangeViewport (const Handle(Standard_Transient)& theFbo,
                                  const Standard_Integer theWidth,
                                  const Standard_Integer theHeight) = 0;

public:

  //! Copy visualization settings from another view.
  //! Method is used for cloning views in viewer when its required to create view
  //! with same view properties.
  Standard_EXPORT virtual void CopySettings (const Handle(Graphic3d_CView)& theOther);

  //! Returns current rendering parameters and effect settings.
  const Graphic3d_RenderingParams& RenderingParams() const { return myRenderParams; }

  //! Returns reference to current rendering parameters and effect settings.
  Graphic3d_RenderingParams& ChangeRenderingParams() { return myRenderParams; }

  //! Returns background  fill color.
  virtual Aspect_Background Background() const = 0;

  //! Sets background fill color.
  virtual void SetBackground (const Aspect_Background& theBackground) = 0;

  //! Returns gradient background fill colors.
  virtual Aspect_GradientBackground GradientBackground() const = 0;

  //! Sets gradient background fill colors.
  virtual void SetGradientBackground (const Aspect_GradientBackground& theBackground) = 0;

  //! Returns background image texture file path.
  virtual TCollection_AsciiString BackgroundImage() = 0;

  //! Sets background image texture file path.
  virtual void SetBackgroundImage (const TCollection_AsciiString& theFilePath) = 0;

  //! Returns background image fill style.
  virtual Aspect_FillMethod BackgroundImageStyle() const = 0;

  //! Sets background image fill style.
  virtual void SetBackgroundImageStyle (const Aspect_FillMethod theFillStyle) = 0;

  //! Returns environment texture set for the view.
  virtual Handle(Graphic3d_TextureEnv) TextureEnv() const = 0; 

  //! Sets environment texture for the view.
  virtual void SetTextureEnv (const Handle(Graphic3d_TextureEnv)& theTextureEnv) = 0;

  //! Returns the state of frustum culling optimization.
  virtual Standard_Boolean IsCullingEnabled() const = 0;

  //! Enables or disables frustum culling optimization.
  virtual void SetCullingEnabled (const Standard_Boolean theIsEnabled) = 0;

  //! Returns shading model of the view.
  virtual Graphic3d_TypeOfShadingModel ShadingModel() const = 0;

  //! Sets shading model of the view.
  virtual void SetShadingModel (const Graphic3d_TypeOfShadingModel theModel) = 0;

  //! Return backfacing model used for the view.
  virtual Graphic3d_TypeOfBackfacingModel BackfacingModel() const = 0;

  //! Sets backfacing model for the view.
  virtual void SetBackfacingModel (const Graphic3d_TypeOfBackfacingModel theModel) = 0;

  //! Returns camera object of the view.
  virtual const Handle(Graphic3d_Camera)& Camera() const = 0;

  //! Sets camera used by the view.
  virtual void SetCamera (const Handle(Graphic3d_Camera)& theCamera) = 0;

  //! Returns list of lights of the view.
  virtual const Graphic3d_ListOfCLight& Lights() const = 0;

  //! Sets list of lights for the view.
  virtual void SetLights (const Graphic3d_ListOfCLight& theLights) = 0;

  //! Returns list of clip planes set for the view.
  virtual const Handle(Graphic3d_SequenceOfHClipPlane)& ClipPlanes() const = 0;

  //! Sets list of clip planes for the view.
  virtual void SetClipPlanes (const Handle(Graphic3d_SequenceOfHClipPlane)& thePlanes) = 0;

  //! Fill in the dictionary with diagnostic info.
  //! Should be called within rendering thread.
  //!
  //! This API should be used only for user output or for creating automated reports.
  //! The format of returned information (e.g. key-value layout)
  //! is NOT part of this API and can be changed at any time.
  //! Thus application should not parse returned information to weed out specific parameters.
  virtual void DiagnosticInformation (TColStd_IndexedDataMapOfStringString& theDict,
                                      Graphic3d_DiagnosticInfo theFlags) const = 0;

private:

  //! Adds the structure to display lists of the view.
  virtual void displayStructure (const Handle(Graphic3d_CStructure)& theStructure,
                                 const Standard_Integer thePriority) = 0;

  //! Erases the structure from display lists of the view.
  virtual void eraseStructure (const Handle(Graphic3d_CStructure)& theStructure) = 0;

  //! Change Z layer of a structure already presented in view.
  virtual void changeZLayer (const Handle(Graphic3d_CStructure)& theCStructure,
                             const Graphic3d_ZLayerId theNewLayerId) = 0;

  //! Changes the priority of a structure within its Z layer in the specified view.
  virtual void changePriority (const Handle(Graphic3d_CStructure)& theCStructure,
                               const Standard_Integer theNewPriority) = 0;

  //! Returns zoom-scale factor.
  virtual Standard_Real considerZoomPersistenceObjects (const Graphic3d_ZLayerId        theLayerId,
                                                        const Handle(Graphic3d_Camera)& theCamera,
                                                        const Standard_Integer          theWindowWidth,
                                                        const Standard_Integer          theWindowHeight) const = 0;

protected:

  Standard_Integer myId;
  Graphic3d_RenderingParams myRenderParams;
  Handle(Graphic3d_StructureManager) myStructureManager;
  Graphic3d_SequenceOfStructure myStructsToCompute;
  Graphic3d_SequenceOfStructure myStructsComputed;
  Graphic3d_MapOfStructure myStructsDisplayed;
  Handle(Graphic3d_NMapOfTransient) myHiddenObjects;
  Standard_Boolean myIsInComputedMode;
  Standard_Boolean myIsActive;
  Standard_Boolean myIsRemoved;
  Graphic3d_TypeOfVisualization myVisualization;

private:

  DEFINE_STANDARD_RTTIEXT(Graphic3d_CView,Graphic3d_DataStructureManager)
};

#endif // _Graphic3d_CView_HeaderFile
