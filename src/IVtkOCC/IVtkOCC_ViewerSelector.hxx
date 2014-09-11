// Created on: 2011-10-20 
// Created by: Roman KOZLOV
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

#ifndef __IVTKOCC_VIEWERSELECTOR_H__
#define __IVTKOCC_VIEWERSELECTOR_H__

#include <IVtk_IView.hxx>
#include <Select3D_Projector.hxx>
#include <SelectMgr_Selection.hxx>
#include <SelectMgr_ViewerSelector.hxx>

// -----------------------------------------------------------------------------
//! @class IVtkOCC_ViewerSelector
//! @brief Class that implements OCCT selection algorithm.
//!
//! Inspired by StdSelect_ViewerSelector3d class from OCCT 6.5.1

class IVtkOCC_ViewerSelector : public SelectMgr_ViewerSelector
{
public:
  IVtkOCC_ViewerSelector();

  //! Projects all sensitive entities from the given selection container to 2D space
  //! param [in] theSelection Container with sensitive entities to project
  void Convert (const Handle(SelectMgr_Selection)& theSelection);

  //! Implements point picking
  //! @param [in] theXPix, theYPix Display coordinates of the point
  //! @param [in] theView  ICamera interface to update the projection parameters.
  void Pick (const Standard_Integer    theXPix,
             const Standard_Integer    theYPix,
             const IVtk_IView::Handle& theView);

  //! Picking by rectangle
  //! @param [in] theXMin, theYMin, theXMax, theYMax Rectangle coords
  //! @param [in] theView ICamera interface to calculate projections
  void Pick (const Standard_Integer    theXMin,
             const Standard_Integer    theYMin,
             const Standard_Integer    theXMax,
             const Standard_Integer    theYMax,
             const IVtk_IView::Handle& theView);

  //! Implements point picking
  void Pick (double** thePoly, const int theNbPoints, const IVtk_IView::Handle& theView);

  //! Activates the given selection
  void Activate (const Handle(SelectMgr_Selection)& theSelection,
                 const Standard_Boolean             isAutomaticProj = Standard_True);

  //! Deactivate the given selection
  void Deactivate (const Handle(SelectMgr_Selection)& theSelection);

  //! Checks if some projection parameters have changed,
  //! and updates the 2D projections of all sensitive entities if necessary.
  //! @param [in] theView Interface to VTK renderer to access projection parameters
  Standard_Boolean Update (const IVtk_IView::Handle& theView);

  //! Returns picking line.
  //! @param theX direction X.
  //! @param theX direction Y.
  //! @return picking direction.
  virtual gp_Lin PickingLine (const Standard_Real theX, const Standard_Real theY) const;

  DEFINE_STANDARD_RTTI( IVtkOCC_ViewerSelector )

private:
  Standard_Real myCoeff[14];
  Standard_Real myPrevCoeff[14];
  Standard_Real myCenter[2];
  Standard_Real myPrevCenter[2];
  Standard_Integer myPixTol;
  Handle(Select3D_Projector) myPrj;
  Standard_Boolean myToUpdateTol;
};

DEFINE_STANDARD_HANDLE( IVtkOCC_ViewerSelector, SelectMgr_ViewerSelector )
#endif // __IVTKOCC_VIEWERSELECTOR_H__
