// Created: 2011-10-27
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

#ifndef __IVTKTOOLS_SHAPEPICKER_H__
#define __IVTKTOOLS_SHAPEPICKER_H__

#include <IVtkTools.hxx>
#include <IVtk_Types.hxx>
#include <IVtkOCC_ShapePickerAlgo.hxx>

// prevent disabling some MSVC warning messages by VTK headers
#include <Standard_WarningsDisable.hxx>
#include <vtkAbstractPropPicker.h>
#include <vtkSmartPointer.h>
#include <Standard_WarningsRestore.hxx>

class vtkRenderer;
class vtkActorCollection;

#ifdef _MSC_VER
  #pragma warning(push)
  #pragma warning(disable : 4251) // avoid warning C4251: "class needs to have dll-interface..."
#endif

//! @class IVtkTools_ShapePicker
//! @brief VTK picker for OCC shapes with OCC selection algorithm.
class Standard_EXPORT IVtkTools_ShapePicker : public vtkAbstractPropPicker
{
public:
  vtkTypeMacro(IVtkTools_ShapePicker, vtkAbstractPropPicker)

    static IVtkTools_ShapePicker* New();

  //! Pick entities in the given point or area.
  //! @return Number of detected entities.
  int pick(double* thePos, vtkRenderer* theRenderer, const int theNbPoints = -1);

  //! Pick entities in the given point.
  //! @return Number of detected entities.
  virtual int Pick(double theX, double theY, double theZ, vtkRenderer* theRenderer = NULL)
    Standard_OVERRIDE;

  //! Pick entities in the given rectangle area.
  //! @return Number of detected entities.
  int Pick(double theX0, double theY0, double theX1, double theY1, vtkRenderer* theRenderer = NULL);

  //! Pick entities in the given polygonal area.
  //! @return Number of detected entities.
  int Pick(double poly[][3], const int theNbPoints, vtkRenderer* theRenderer = NULL);

  //! Setter for tolerance of picking.
  void SetTolerance(float theTolerance);
  //! Getter for tolerance of picking.
  float GetTolerance() const;

  //! Sets the renderer to be used by OCCT selection algorithm
  void SetRenderer(vtkRenderer* theRenderer);
  //! Sets area selection on/off
  //! @param[in]  theIsOn true if area selection is turned on, false otherwise.
  void SetAreaSelection(bool theIsOn);

  //! Get activated selection modes for a shape.
  //! @param[in]  theShape a shape with activated selection mode(s)
  //! @return list of active selection modes
  IVtk_SelectionModeList GetSelectionModes(const IVtk_IShape::Handle& theShape) const;

  //! Get activated selection modes for a shape actor.
  //! @param[in]  theShapeActor an actor with activated selection mode(s)
  //! @return list of active selection modes
  IVtk_SelectionModeList GetSelectionModes(vtkActor* theShapeActor) const;

  //! Turn on/off a selection mode for a shape actor.
  //! @param[in]  theShape a shape to set a selection mode for
  //! @param[in]  theMode selection mode to be activated
  //! @param[in]  theIsTurnOn Flag to turn on/off the selection mode
  void SetSelectionMode(const IVtk_IShape::Handle& theShape,
                        const IVtk_SelectionMode   theMode,
                        const bool                 theIsTurnOn = true) const;

  //! Turn on/off a selection mode for a shape actor.
  //! @param[in]  theShapeActor shape presentation actor to set a selection mode for
  //! @param[in]  theMode selection mode to be activated
  //! @param[in]  theIsTurnOn Flag to turn on/off the selection mode
  void SetSelectionMode(vtkActor*                theShapeActor,
                        const IVtk_SelectionMode theMode,
                        const bool               theIsTurnOn = true) const;

  //! Sets the current selection mode for all visible shape objects.
  //! @param[in]  theMode selection mode to be activated
  //! @param[in]  theIsTurnOn Flag to turn on/off the selection mode
  void SetSelectionMode(const IVtk_SelectionMode theMode, const bool theIsTurnOn = true) const;

  // Picking results

  //! Access to the list of top-level shapes picked.
  //! If all argument is true, the picker returns the list of
  //! all OccShape objects found by the picking algorithm. e.g. all
  //! shapes under the mouse cursor. Otherwise, ID of the shape closest to the eye
  //! is returned.
  //! @param[in]  theIsAll Get all selected shapes or just the only
  //!        top one is returned, has no effect during area selection.
  //! @return List of top-level shape IDs
  IVtk_ShapeIdList GetPickedShapesIds(bool theIsAll = false) const;

  //! Access to the list of sub-shapes ids picked.
  //! @param[in]  theId top-level shape ID
  //! @param[in]  theIsAll Get all selected sub-shapes or just the
  //!        only top one is returned, has no effect during area selection.
  //! @return List of sub-shapes IDs
  IVtk_ShapeIdList GetPickedSubShapesIds(const IVtk_IdType theId, bool theIsAll = false) const;

  //! Access to the list of actors picked.
  //! @param[in]  theIsAll Get all selected actors or just the only
  //!         top one is returned, has no effect during area selection.
  //! @return List of actors IDs
  vtkSmartPointer<vtkActorCollection> GetPickedActors(bool theIsAll = false) const;

  //! Remove selectable object from the picker (from internal maps).
  //! @param[in]  theShape the selectable shape
  void RemoveSelectableObject(const IVtk_IShape::Handle& theShape);

  //! Remove selectable object from the picker (from internal maps).
  //! @param[in]  theShapeActor the shape presentation actor to be removed from the picker
  void RemoveSelectableActor(vtkActor* theShapeActor);

protected:
  //! Constructs the picker with empty renderer and ready for point selection.
  IVtkTools_ShapePicker();
  //! Destructor
  virtual ~IVtkTools_ShapePicker();

  //! Convert display coordinates to world coordinates
  static bool convertDisplayToWorld(vtkRenderer* theRenderer,
                                    double       theDisplayCoord[3],
                                    double       theWorldCoord[3]);

private: // not copyable
  IVtkTools_ShapePicker(const IVtkTools_ShapePicker&);
  IVtkTools_ShapePicker& operator=(const IVtkTools_ShapePicker&);

  //! Implementation of picking algorithm.
  //! The coordinates accepted by this method are display (pixel) coordinates.
  //! @param[in]  pos contains the pick point (3 coordinates) or pick rectangle (6 coordinates)
  //! or polyline (array of 2d coordinates)
  //! @param[in]  renderer vtkRenderer object to be used (normally set in advance with
  //! setRenderer())
  //! @param[in]  nbPoints number of points for polyline case
  //! @see IVtkTools_ShapePicker::setRenderer
  virtual void doPickImpl(double*, vtkRenderer* theRenderer, const int theNbPoints = -1);

private:
  IVtkOCC_ShapePickerAlgo::Handle myOccPickerAlgo;   //!< Picking algorithm implementation
  vtkSmartPointer<vtkRenderer>    myRenderer;        //!< VTK renderer
  bool                            myIsRectSelection; //!< Rectangle selection mode flag
  bool                            myIsPolySelection; //!< Polyline selection mode flag
  float                           myTolerance;       //!< Selection tolerance
};

#ifdef _MSC_VER
  #pragma warning(pop)
#endif

#endif // __IVTKTOOLS_SHAPEPICKER_H__
