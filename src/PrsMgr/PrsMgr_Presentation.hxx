// Created on: 1995-01-25
// Created by: Jean-Louis Frenkel
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _PrsMgr_Presentation_HeaderFile
#define _PrsMgr_Presentation_HeaderFile

#include <PrsMgr_PresentableObjectPointer.hxx>
#include <Standard.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <Aspect_TypeOfHighlightMethod.hxx>

class PrsMgr_PresentationManager;
class PrsMgr_Prs;
class PrsMgr_PresentableObject;
class Quantity_Color;
class Geom_Transformation;
class Prs3d_Presentation;
class Prs3d_Drawer;
class Graphic3d_Structure;
class Graphic3d_DataStructureManager;
class Prs3d_Projector;

DEFINE_STANDARD_HANDLE(PrsMgr_Presentation, Standard_Transient)

class PrsMgr_Presentation : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(PrsMgr_Presentation, Standard_Transient)
  friend class PrsMgr_PresentationManager;
  friend class PrsMgr_PresentableObject;
  friend class PrsMgr_Prs;
public:

  //! Destructor
  Standard_EXPORT ~PrsMgr_Presentation();

  const Handle(Prs3d_Presentation)& Presentation() const { return myStructure; }

  //! returns the PresentationManager in which the presentation has been created.
  const Handle(PrsMgr_PresentationManager)& PresentationManager() const { return myPresentationManager; }

  void SetUpdateStatus (const Standard_Boolean theUpdateStatus) { myMustBeUpdated = theUpdateStatus; }

  Standard_Boolean MustBeUpdated() const { return myMustBeUpdated; }

private:

  Standard_EXPORT PrsMgr_Presentation(const Handle(PrsMgr_PresentationManager)& thePresentationManager, const Handle(PrsMgr_PresentableObject)& thePresentableObject);
  
  Standard_EXPORT void Display();
  
  //! Displays myStructure.
  Standard_EXPORT void display (const Standard_Boolean theIsHighlight);
  
  Standard_EXPORT void Erase();
  
  Standard_EXPORT void SetVisible (const Standard_Boolean theValue);
  
  Standard_EXPORT void Highlight (const Handle(Prs3d_Drawer)& theStyle);
  
  Standard_EXPORT void Unhighlight() const;
  
  Standard_EXPORT Standard_Boolean IsHighlighted() const;
  
  Standard_EXPORT Standard_Boolean IsDisplayed() const;
  
  Standard_EXPORT Standard_Integer DisplayPriority() const;
  
  Standard_EXPORT void SetDisplayPriority (const Standard_Integer aNewPrior);
  
  //! Set Z layer ID for the presentation
  Standard_EXPORT void SetZLayer (const Standard_Integer theLayerId);
  
  //! Get Z layer ID for the presentation
  Standard_EXPORT Standard_Integer GetZLayer() const;
  
  //! removes the whole content of the presentation.
  //! Does not remove the other connected presentations.
  Standard_EXPORT void Clear();
  
  Standard_EXPORT void Connect (const Handle(PrsMgr_Presentation)& theOther) const;
  
  Standard_EXPORT void SetTransformation (const Handle(Geom_Transformation)& theTrsf) const;

  Standard_EXPORT void Compute (const Handle(Graphic3d_Structure)& theStructure);
  
  Standard_EXPORT Handle(Graphic3d_Structure) Compute (const Handle(Graphic3d_DataStructureManager)& theProjector);
  
  Standard_EXPORT Handle(Graphic3d_Structure) Compute (const Handle(Graphic3d_DataStructureManager)& theProjector, const Handle(Geom_Transformation)& theTrsf);
  
  Standard_EXPORT void Compute (const Handle(Graphic3d_DataStructureManager)& theProjector, const Handle(Graphic3d_Structure)& theGivenStruct);
  
  Standard_EXPORT void Compute (const Handle(Graphic3d_DataStructureManager)& theProjector, const Handle(Geom_Transformation)& theTrsf, const Handle(Graphic3d_Structure)& theGivenStruct);
  
  Standard_EXPORT static Handle(Prs3d_Projector) Projector (const Handle(Graphic3d_DataStructureManager)& theProjector);

protected:

  Handle(PrsMgr_PresentationManager) myPresentationManager;
  Handle(Prs3d_Presentation) myStructure;
  PrsMgr_PresentableObjectPointer myPresentableObject;
  Standard_Boolean myMustBeUpdated;
  Standard_Integer myBeforeHighlightState;

};

#endif // _PrsMgr_Presentation_HeaderFile
