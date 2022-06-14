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

#include <Prs3d_Presentation.hxx>

class PrsMgr_PresentationManager;
class PrsMgr_PresentableObject;
class Graphic3d_Camera;
class Prs3d_Drawer;

DEFINE_STANDARD_HANDLE(PrsMgr_Presentation, Graphic3d_Structure)

class PrsMgr_Presentation : public Graphic3d_Structure
{
  DEFINE_STANDARD_RTTIEXT(PrsMgr_Presentation, Graphic3d_Structure)
  friend class PrsMgr_PresentationManager;
  friend class PrsMgr_PresentableObject;
public:

  //! Destructor
  Standard_EXPORT ~PrsMgr_Presentation();

  Standard_DEPRECATED("Dummy to simplify porting - returns self")
  Prs3d_Presentation* Presentation() { return this; }

  //! returns the PresentationManager in which the presentation has been created.
  const Handle(PrsMgr_PresentationManager)& PresentationManager() const { return myPresentationManager; }

  void SetUpdateStatus (const Standard_Boolean theUpdateStatus) { myMustBeUpdated = theUpdateStatus; }

  Standard_Boolean MustBeUpdated() const { return myMustBeUpdated; }

  //! Return display mode index.
  Standard_Integer Mode() const { return myMode; }

  //! Display structure.
  Standard_EXPORT virtual void Display() Standard_OVERRIDE;

  //! Remove structure.
  Standard_EXPORT virtual void Erase() Standard_OVERRIDE;

  //! Highlight structure.
  Standard_EXPORT void Highlight (const Handle(Prs3d_Drawer)& theStyle);

  //! Unhighlight structure.
  Standard_EXPORT void Unhighlight();

  //! Return TRUE if structure has been displayed and in no hidden state.
  virtual Standard_Boolean IsDisplayed() const Standard_OVERRIDE
  {
    return base_type::IsDisplayed()
        && base_type::IsVisible();
  }

  //! removes the whole content of the presentation.
  //! Does not remove the other connected presentations.
  Standard_EXPORT virtual void Clear (const Standard_Boolean theWithDestruction = Standard_True) Standard_OVERRIDE;

  //! Compute structure using presentation manager.
  Standard_EXPORT virtual void Compute() Standard_OVERRIDE;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const Standard_OVERRIDE;

protected:

  //! Main constructor.
  Standard_EXPORT PrsMgr_Presentation (const Handle(PrsMgr_PresentationManager)& thePresentationManager,
                                       const Handle(PrsMgr_PresentableObject)& thePresentableObject,
                                       const Standard_Integer theMode);

  //! Displays myStructure.
  Standard_EXPORT void display (const Standard_Boolean theIsHighlight);

  Standard_EXPORT virtual void computeHLR (const Handle(Graphic3d_Camera)& theProjector,
                                           Handle(Graphic3d_Structure)& theGivenStruct) Standard_OVERRIDE;
protected:

  Handle(PrsMgr_PresentationManager) myPresentationManager;
  PrsMgr_PresentableObject* myPresentableObject;
  Standard_Integer myBeforeHighlightState;
  Standard_Integer myMode;
  Standard_Boolean myMustBeUpdated;

};

#endif // _PrsMgr_Presentation_HeaderFile
