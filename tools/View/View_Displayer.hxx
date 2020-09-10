// Created on: 2017-06-16
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2017 OPEN CASCADE SAS
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

#ifndef View_Displayer_H
#define View_Displayer_H

#include <AIS_ListOfInteractive.hxx>
#include <AIS_InteractiveContext.hxx>

#include <NCollection_DataMap.hxx>
#include <NCollection_Shared.hxx>
#include <TopoDS_Shape.hxx>
#include <Quantity_Color.hxx>

#include <inspector/View_PresentationType.hxx>
#include <inspector/View_DisplayActionType.hxx>

class AIS_Trihedron;
class AIS_ViewCube;
class V3d_View;
class View_DisplayPreview;

//! \class View_Displayer
//! \brief It is responsible for communication with AIS Interactive Context to:
//! - display/erase presentations;
//! - change display mode of visualized presentations (Shaded or WireFrame mode)
//!
//! It contains containers of visualized presentations to obtain presentations relating only to this displayer.
//! Displayer is connected to AIS Interactive Context
class View_Displayer
{
public:

  //! Constructor
  Standard_EXPORT View_Displayer();

  //! Destructor
  virtual ~View_Displayer() {}

  //! Returns preview display instance
  View_DisplayPreview* DisplayPreview() const { return myDisplayPreview; }

  //! Stores the current context where the presentations will be displayed/erased.
  //! Erases previuously displayd presentations if there were some displayed
  //! \param theContext a context instance
  Standard_EXPORT void SetContext (const Handle(AIS_InteractiveContext)& theContext);

  //! Stores boolean value if presentations should be keeped. If true, new displayed presentation is added to already
  //! displayed ones, otherwise the displayed presentation is shown only.
  //! \param theToKeepPresentation boolean state
  void KeepPresentations (const bool theToKeepPresentations) { myIsKeepPresentations = theToKeepPresentations; }

  //! Returns true if fit Fit All should be peformed automatically by each Display
  bool IsFitAllActive() const { return myFitAllActive; }

  //! Stores flag whether the FitAll shoud be done automatically for each display
  //! \param theFitAllActive boolean value
  void SetFitAllActive (const bool theFitAllActive) { myFitAllActive = theFitAllActive; }

  //! Returns current display mode: 0 - AIS_WireFrame, 1 - AIS_Shaded
  int DisplayMode() const { return myDisplayMode; }

  //! Stores display mode and changes display mode of displayed presentations
  //! \param theDisplayMode a mode: 0 - AIS_WireFrame, 1 - AIS_Shaded
  //! \param theType presentation type
  //! \param isToUpdateView boolean state if viewer should be updated
  Standard_EXPORT void SetDisplayMode (const int theDisplayMode,
                                       const View_PresentationType theType = View_PresentationType_Main,
                                       const bool theToUpdateViewer = true);

  //! Displays presentation in context, erase previous presentations if KeepPresentations is false,
  //! Color of presentation is from attribute color if exists or the default color
  //! Display mode is equal to the view state
  //! If KeepPresentations is false, the viewer is fit all
  //! Displayed presentation is stored in an internal map of displayed presentations
  //! \param thePresentation a presentation, it will be casted to AIS_InteractiveObject
  //! \param theType presentation type
  //! \param isToUpdateView boolean state if viewer should be updated
  Standard_EXPORT void DisplayPresentation (const Handle(Standard_Transient)& thePresentation,
                                            const View_PresentationType theType = View_PresentationType_Main,
                                            const bool theToUpdateViewer = true);

  //! Redisplays the parameter presentation in current context
  //! \param thePresentation a presentation, it will be casted to AIS_InteractiveObject
  //! \param isToUpdateView boolean state if viewer should be updated
  Standard_EXPORT void RedisplayPresentation (const Handle(Standard_Transient)& thePresentation,
                                              const bool theToUpdateViewer = true);

  //! Erases all presentations from viewer. Iterates by internal map of displayed presentations and 
  //! erase these presentations.
  //! \param isToUpdateView boolean state if viewer should be updated
  Standard_EXPORT void EraseAllPresentations (const bool theToUpdateViewer = true);

  //! Erases presentations of the given type
  //! \param theType presentation type
  //! \param isToUpdateView boolean state if viewer should be updated
  Standard_EXPORT void ErasePresentations (const View_PresentationType theType = View_PresentationType_Main,
                                           const bool theToUpdateViewer = true);

  //! Erases presentation from viewer
  //! \param thePresentation a presentation, it will be casted to AIS_InteractiveObject
  //! \param theType presentation type
  //! \param isToUpdateView boolean state if viewer should be updated
  Standard_EXPORT void ErasePresentation (const Handle(Standard_Transient)& thePresentation,
                                          const View_PresentationType theType = View_PresentationType_Main,
                                          const bool theToUpdateViewer = true);

  //! Displays presentation of default trihedron, create it by the first call
  //! \param toDisplay flag to display presentation if true, or erase it
  //! \param isToUpdateView boolean state if viewer should be updated
  Standard_EXPORT void DisplayDefaultTrihedron (const Standard_Boolean toDisplay, const bool theToUpdateViewer);

  //! Displays presentation of view cube, create it by the first call
  //! \param toDisplay flag to display presentation if true, or erase it
  //! \param isToUpdateView boolean state if viewer should be updated
  Standard_EXPORT void DisplayViewCube (const Standard_Boolean toDisplay, const bool theToUpdateViewer);

  //! Sets shape visible/invisible
  //! \theShape shape instance
  //! \theState visibility state
  Standard_EXPORT void SetVisible (const TopoDS_Shape& theShape, const bool theState,
                                   const View_PresentationType theType = View_PresentationType_Main);

  //! Returns visibility state value
  //! \theShape shape instance
  Standard_EXPORT bool IsVisible (const TopoDS_Shape& theShape,
                                  const View_PresentationType theType = View_PresentationType_Main) const;

  //! Updates visibility of the presentations for the display type
  Standard_EXPORT void UpdatePreview (const View_DisplayActionType theType,
                                      const NCollection_List<Handle(Standard_Transient)>& thePresentations);

  //! Calls UpdateCurrentViewer of context
  Standard_EXPORT void UpdateViewer();

  //! Stores color for type of presentation. During display of presentation of the given type, the color is used
  //! \param theColor a presentation color
  //! \param theType presentation type
  Standard_EXPORT void SetAttributeColor (const Quantity_Color& theColor,
                                          const View_PresentationType theType = View_PresentationType_Main);

  //! Returns container of displayed presentations for the given type
  //! \param thePresentations a container to be filled
  //! \param theType presentation type
  Standard_EXPORT void DisplayedPresentations (NCollection_Shared<AIS_ListOfInteractive>& thePresentations,
                                               const View_PresentationType theType = View_PresentationType_Main) const;

  //! Returns all displayed by the trihedron objects
  const NCollection_DataMap<View_PresentationType, NCollection_Shared<AIS_ListOfInteractive>>& GetDisplayed() const { return myDisplayed; }

  //! Returns presentation if there is displayed AIS_Shape presentation for the parameter shape
  //! \param theShape a shape instance
  //! \param theType presentation type
  //! \return presentation instance or NULL
  Standard_EXPORT Handle(AIS_InteractiveObject) FindPresentation (const TopoDS_Shape& theShape,
    const View_PresentationType theType = View_PresentationType_Main) const;

  //! Creates AIS_Shape for the shape
  //! \param theShape a shape
  //! \return presentation
  Standard_EXPORT static Handle(Standard_Transient) CreatePresentation (const TopoDS_Shape& theShape);

private:

  //! Returns the current context
  const Handle(AIS_InteractiveContext)& GetContext() const { return myContext; }

  //! Returns 3d view
  Handle(V3d_View) GetView() const;

  //! Fits all view
  void fitAllView();

  //! Returns default trihedron, create it if flag allows
  //! \param toCreate boolean state if trihedron should be created if it is NULL
  const Handle(AIS_Trihedron)& defaultTrihedron (const bool toCreate);

private:

  View_DisplayPreview* myDisplayPreview; //!< class for preview display

  Handle(AIS_InteractiveContext) myContext; //!< context, where the displayer works
  Handle(AIS_Trihedron) myDefaultTrihedron; //!< trihedron presentation for the current context
  Handle(AIS_ViewCube) myViewCube; //!< view cube presentation for current context

  NCollection_DataMap<View_PresentationType, NCollection_Shared<AIS_ListOfInteractive>> myDisplayed; //!< visualized presentations
  NCollection_DataMap<View_PresentationType, Quantity_Color> myColorAttributes; //!< color properties of presentations

  bool myIsKeepPresentations; //!< flag if previously shown presentations stays in the context by displaying a new one
  bool myFitAllActive; //!< flag if Fit All should be peformed automatically by each Display
  int myDisplayMode; //!< display mode: 0 - AIS_WireFrame, 1 - AIS_Shaded
};

#endif
