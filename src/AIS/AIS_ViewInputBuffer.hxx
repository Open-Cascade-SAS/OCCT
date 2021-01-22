// Copyright (c) 2016-2019 OPEN CASCADE SAS
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

#ifndef _AIS_ViewInputBuffer_HeaderFile
#define _AIS_ViewInputBuffer_HeaderFile

#include <Aspect_ScrollDelta.hxx>

#include <AIS_SelectionScheme.hxx>
#include <Graphic3d_Vec2.hxx>
#include <NCollection_Sequence.hxx>
#include <V3d_TypeOfOrientation.hxx>

//! Selection mode
enum AIS_ViewSelectionTool
{
  AIS_ViewSelectionTool_Picking,    //!< pick to select
  AIS_ViewSelectionTool_RubberBand, //!< rubber-band to select
  AIS_ViewSelectionTool_Polygon,    //!< polyline to select
  AIS_ViewSelectionTool_ZoomWindow, //!< zoom-in window (no selection)
};

//! Input buffer type.
enum AIS_ViewInputBufferType
{
  AIS_ViewInputBufferType_UI, //!< input buffer for filling from UI thread
  AIS_ViewInputBufferType_GL, //!< input buffer accessible  from GL thread
};

//! Auxiliary structure defining viewer events
class AIS_ViewInputBuffer
{
public:

  bool IsNewGesture;     //!< transition from one action to another

  NCollection_Sequence<Aspect_ScrollDelta> ZoomActions; //!< the queue with zoom actions

  struct _orientation
  {
    bool                  ToFitAll;         //!< perform FitAll operation
    bool                  ToSetViewOrient;  //!< set new view orientation
    V3d_TypeOfOrientation ViewOrient;       //!< new view orientation

    _orientation() : ToFitAll (false), ToSetViewOrient (false), ViewOrient (V3d_Xpos) {}
  } Orientation;

  struct _highlighting
  {
    bool            ToHilight; //!< perform dynamic highlighting at specified point
    Graphic3d_Vec2i Point;     //!< the new point for dynamic highlighting

    _highlighting() : ToHilight (false) {}
  } MoveTo;

  struct _selection
  {
    AIS_ViewSelectionTool Tool;          //!< perform selection
    AIS_SelectionScheme   Scheme;        //!< selection scheme
    NCollection_Sequence<Graphic3d_Vec2i>
                          Points;        //!< the points for selection
    bool                  ToApplyTool;   //!< apply rubber-band selection tool

    _selection() : Tool (AIS_ViewSelectionTool_Picking), Scheme (AIS_SelectionScheme_UNKNOWN), ToApplyTool (false) {}
  } Selection;

  struct _panningParams
  {
    bool            ToStart;    //!< start panning
    Graphic3d_Vec2i PointStart; //!< panning start point
    bool            ToPan;      //!< perform panning
    Graphic3d_Vec2i Delta;      //!< panning delta

    _panningParams() : ToStart (false), ToPan (false) {}
  } Panning;

  struct _draggingParams
  {
    bool            ToStart;    //!< start dragging
    bool            ToStop;     //!< stop  dragging
    bool            ToAbort;    //!< abort dragging (restore previous position)
    Graphic3d_Vec2i PointStart; //!< drag start point
    Graphic3d_Vec2i PointTo;    //!< drag end point

    _draggingParams() : ToStart (false), ToStop (false), ToAbort (false) {}
  } Dragging;

  struct _orbitRotation
  {
    bool            ToStart;    //!< start orbit rotation
    Graphic3d_Vec2d PointStart; //!< orbit rotation start point
    bool            ToRotate;   //!< perform orbit rotation
    Graphic3d_Vec2d PointTo;    //!< orbit rotation end point

    _orbitRotation() : ToStart (false), ToRotate (false) {}
  } OrbitRotation;

  struct _viewRotation
  {
    bool            ToStart;    //!< start view rotation
    Graphic3d_Vec2d PointStart; //!< view rotation start point
    bool            ToRotate;   //!< perform view rotation
    Graphic3d_Vec2d PointTo;    //!< view rotation end point

    _viewRotation() : ToStart (false), ToRotate (false) {}
  } ViewRotation;

  struct _zrotateParams
  {
    Graphic3d_Vec2i Point;    //!< Z rotation start point
    double          Angle;    //!< Z rotation angle
    bool            ToRotate; //!< start Z rotation

    _zrotateParams() : Angle (0.0), ToRotate (false) {}
  } ZRotate;

public:

  AIS_ViewInputBuffer()
  : IsNewGesture (false) {}

  //! Reset events buffer.
  void Reset()
  {
    Orientation.ToFitAll = false;
    Orientation.ToSetViewOrient = false;
    MoveTo.ToHilight = false;
    Selection.ToApplyTool = false;
    IsNewGesture     = false;
    ZoomActions.Clear();
    Panning.ToStart  = false;
    Panning.ToPan    = false;
    Dragging.ToStart = false;
    Dragging.ToStop  = false;
    Dragging.ToAbort = false;
    OrbitRotation.ToStart  = false;
    OrbitRotation.ToRotate = false;
    ViewRotation.ToStart   = false;
    ViewRotation.ToRotate  = false;
    ZRotate.ToRotate = false;
  }

};

#endif // _AIS_ViewInputBuffer_HeaderFile
