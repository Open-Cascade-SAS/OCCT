// Created on: 2011-10-12
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

#ifndef __IVTK_ISHAPEPICKERALGO_H__
#define __IVTK_ISHAPEPICKERALGO_H__

#include <IVtk_IView.hxx>
#include <IVtk_IShape.hxx>

class IVtk_IShapePickerAlgo;
DEFINE_STANDARD_HANDLE(IVtk_IShapePickerAlgo, IVtk_Interface)

//! @class IVtk_IShapePickerAlgo
//! @brief Interface for 3D shapes picking algorithm.
class IVtk_IShapePickerAlgo : public IVtk_Interface
{
public:
  typedef Handle(IVtk_IShapePickerAlgo) Handle;

  virtual ~IVtk_IShapePickerAlgo() {}

  DEFINE_STANDARD_RTTIEXT(IVtk_IShapePickerAlgo, IVtk_Interface)

  virtual void SetView(const IVtk_IView::Handle& theView) = 0;
  virtual int  NbPicked()                                 = 0;

  //! Get activated selection modes for a shape.
  //! @param[in]  theShape a shape with activated selection mode(s)
  //! @return list of active selection modes
  virtual IVtk_SelectionModeList GetSelectionModes(const IVtk_IShape::Handle& theShape) const = 0;

public: // @name Set selectable shapes and selection modes
  //! Activates/deactivates the given selection mode for the shape.
  //! If mode == SM_None, the shape becomes non-selectable and
  //! is removed from the internal selection data.
  //! @param[in]  theShape Shape for which the selection mode should be activated
  //! @param[in]  theMode Selection mode to be activated
  //! @param[in]  theIsTurnOn Flag to turn on/off the selection mode
  virtual void SetSelectionMode(const IVtk_IShape::Handle& theShape,
                                const IVtk_SelectionMode   theMode,
                                const bool                 theIsTurnOn = true) = 0;

  //! Activates/deactivates the given selection mode for the shape.
  //! If mode == SM_None, the shape becomes non-selectable and
  //! is removed from the internal selection data.
  //! @param[in]  theShapes List of shapes for which the selection mode should be activated
  //! @param[in]  theMode Selection mode to be activated
  //! @param[in]  theIsTurnOn Flag to turn on/off the selection mode
  virtual void SetSelectionMode(const IVtk_ShapePtrList& theShapes,
                                const IVtk_SelectionMode theMode,
                                const bool               theIsTurnOn = true) = 0;

public: // @name Picking methods
  virtual bool Pick(const double theX, const double theY) = 0;
  virtual bool Pick(const double theXMin,
                    const double theYMin,
                    const double theXMax,
                    const double theYMax)                 = 0;

  virtual bool Pick(double** /* double poly[][3]*/, const int theNbPoints) = 0;

public: // @name Obtain picking results
  //! @return the list of picked top-level shape IDs,
  //! in the order of increasing depth (the ID of the shape closest to the eye
  //! is the first in the list)
  virtual const IVtk_ShapeIdList& ShapesPicked() const = 0;

  //! @param[in]  theId Top-level shape ID
  //! @param[out]  theShapeList the list of picked sub-shape IDs for the given top-level shape ID,
  //! in the order of increasing depth (the ID of the sub-shape closest to the eye
  //! is the first in the list)
  virtual void SubShapesPicked(const IVtk_IdType theId, IVtk_ShapeIdList& theShapeList) const = 0;
};

#endif // __IVTK_ISHAPEPICKERALGO_H__
