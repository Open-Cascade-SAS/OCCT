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
  const occ::handle<PrsMgr_PresentationManager>& PresentationManager() const
  {
    return myPresentationManager;
  }

  void SetUpdateStatus(const bool theUpdateStatus) { myMustBeUpdated = theUpdateStatus; }

  bool MustBeUpdated() const { return myMustBeUpdated; }

  //! Return display mode index.
  int Mode() const { return myMode; }

  //! Display structure.
  Standard_EXPORT virtual void Display() override;

  //! Remove structure.
  Standard_EXPORT virtual void Erase() override;

  //! Highlight structure.
  Standard_EXPORT void Highlight(const occ::handle<Prs3d_Drawer>& theStyle);

  //! Unhighlight structure.
  Standard_EXPORT void Unhighlight();

  //! Return TRUE if structure has been displayed and in no hidden state.
  virtual bool IsDisplayed() const override
  {
    return base_type::IsDisplayed() && base_type::IsVisible();
  }

  //! removes the whole content of the presentation.
  //! Does not remove the other connected presentations.
  Standard_EXPORT virtual void Clear(const bool theWithDestruction = true) override;

  //! Compute structure using presentation manager.
  Standard_EXPORT virtual void Compute() override;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream,
                                        int               theDepth = -1) const override;

protected:
  //! Main constructor.
  Standard_EXPORT PrsMgr_Presentation(
    const occ::handle<PrsMgr_PresentationManager>& thePresentationManager,
    const occ::handle<PrsMgr_PresentableObject>&   thePresentableObject,
    const int                                      theMode);

  //! Displays myStructure.
  Standard_EXPORT void display(const bool theIsHighlight);

  Standard_EXPORT virtual void computeHLR(
    const occ::handle<Graphic3d_Camera>& theProjector,
    occ::handle<Graphic3d_Structure>&    theGivenStruct) override;

  Standard_EXPORT virtual void RecomputeTransformation(
    const occ::handle<Graphic3d_Camera>& theProjector) override;

protected:
  occ::handle<PrsMgr_PresentationManager> myPresentationManager;
  PrsMgr_PresentableObject*               myPresentableObject;
  int                                     myBeforeHighlightState;
  int                                     myMode;
  bool                                    myMustBeUpdated;
};

#endif // _PrsMgr_Presentation_HeaderFile
