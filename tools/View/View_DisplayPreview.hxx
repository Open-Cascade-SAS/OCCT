// Created on: 2020-01-25
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2020 OPEN CASCADE SAS
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

#ifndef View_DisplayPreview_H
#define View_DisplayPreview_H

#include <inspector/View_DisplayActionType.hxx>

#include <AIS_InteractiveContext.hxx>
#include <Standard.hxx>
#include <Standard_Handle.hxx>
#include <Standard_Macro.hxx>

class AIS_InteractiveObject;
class View_PreviewParameters;

//! \class View_DisplayPreview
//! \brief It is responsible for communication with AIS Interactive Context to:
//! - display/erase presentations;
//! - change display mode of visualized presentations (Shaded or WireFrame mode)
//!
//! It contains containers of visualized presentations to obtain presentations relating only to this displayer.
//! Displayer is connected to AIS Interactive Context
class View_DisplayPreview
{
public:

  //! Constructor
  Standard_EXPORT View_DisplayPreview();

  //! Destructor
  virtual ~View_DisplayPreview() {}

  //! Stores the current context where the presentations will be displayed/erased.
  //! Erases previuously displayed presentations if there were some displayed
  //! \param theContext a context instance
  Standard_EXPORT void SetContext (const Handle(AIS_InteractiveContext)& theContext);

  //! Returns preview parameters
  View_PreviewParameters* GetPreviewParameters() const { return myPreviewParameters; }

  //! Updates visibility of the presentations for the display type
  Standard_EXPORT void UpdatePreview (const View_DisplayActionType theType,
                                      const NCollection_List<Handle(Standard_Transient)>& thePresentations);

  //! Returns true if preview presentation is shown
  Standard_Boolean HasPreview() const { return !myPreviewPresentation.IsNull(); }

  //! Custom preview selection mode
  static Standard_Integer PreviewSelectionMode() { return 100; }

private:

  //! Returns the current context
  const Handle(AIS_InteractiveContext)& GetContext() const { return myContext; }

private:

  Handle(AIS_InteractiveContext) myContext; //!< context, where the displayer works

  View_PreviewParameters* myPreviewParameters; //!< drawer of preview presentation
  Handle(AIS_InteractiveObject) myPreviewPresentation; //!< presentation of preview for a selected object
  NCollection_List<Handle(AIS_InteractiveObject)> myPreviewReadyPresentations; //!< presentation of preview for a selected object
};

#endif
