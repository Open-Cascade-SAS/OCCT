// Created on: 1994-01-17
// Created by: Modelistation
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _Prs3d_PlaneAspect_HeaderFile
#define _Prs3d_PlaneAspect_HeaderFile

#include <Prs3d_LineAspect.hxx>

//! A framework to define the display of planes.
class Prs3d_PlaneAspect : public Prs3d_BasicAspect
{
  DEFINE_STANDARD_RTTIEXT(Prs3d_PlaneAspect, Prs3d_BasicAspect)
public:
  //! Constructs an empty framework for the display of planes.
  Standard_EXPORT Prs3d_PlaneAspect();

  //! Returns the attributes of displayed edges involved in the presentation of planes.
  const occ::handle<Prs3d_LineAspect>& EdgesAspect() const { return myEdgesAspect; }

  //! Returns the attributes of displayed isoparameters involved in the presentation of planes.
  const occ::handle<Prs3d_LineAspect>& IsoAspect() const { return myIsoAspect; }

  //! Returns the settings for displaying an arrow.
  const occ::handle<Prs3d_LineAspect>& ArrowAspect() const { return myArrowAspect; }

  void SetArrowsLength(const double theLength) { myArrowsLength = theLength; }

  //! Returns the length of the arrow shaft used in the display of arrows.
  double ArrowsLength() const { return myArrowsLength; }

  //! Sets the angle of the arrowhead used in the display of planes.
  void SetArrowsSize(const double theSize) { myArrowsSize = theSize; }

  //! Returns the size of arrows used in the display of planes.
  double ArrowsSize() const { return myArrowsSize; }

  //! Sets the angle of the arrowhead used in the display
  //! of arrows involved in the presentation of planes.
  void SetArrowsAngle(const double theAngle) { myArrowsAngle = theAngle; }

  //! Returns the angle of the arrowhead used in the
  //! display of arrows involved in the presentation of planes.
  double ArrowsAngle() const { return myArrowsAngle; }

  //! Sets the display attributes defined in DisplayCenterArrow to active.
  void SetDisplayCenterArrow(const bool theToDraw) { myDrawCenterArrow = theToDraw; }

  //! Returns true if the display of center arrows is allowed.
  bool DisplayCenterArrow() const { return myDrawCenterArrow; }

  //! Sets the display attributes defined in DisplayEdgesArrows to active.
  void SetDisplayEdgesArrows(const bool theToDraw) { myDrawEdgesArrows = theToDraw; }

  //! Returns true if the display of edge arrows is allowed.
  bool DisplayEdgesArrows() const { return myDrawEdgesArrows; }

  void SetDisplayEdges(const bool theToDraw) { myDrawEdges = theToDraw; }

  bool DisplayEdges() const { return myDrawEdges; }

  //! Sets the display attributes defined in DisplayIso to active.
  void SetDisplayIso(const bool theToDraw) { myDrawIso = theToDraw; }

  //! Returns true if the display of isoparameters is allowed.
  bool DisplayIso() const { return myDrawIso; }

  void SetPlaneLength(const double theLX, const double theLY)
  {
    myPlaneXLength = theLX;
    myPlaneYLength = theLY;
  }

  //! Returns the length of the x axis used in the display of planes.
  double PlaneXLength() const { return myPlaneXLength; }

  //! Returns the length of the y axis used in the display of planes.
  double PlaneYLength() const { return myPlaneYLength; }

  //! Sets the distance L between isoparameters used in the display of planes.
  void SetIsoDistance(const double theL) { myIsoDistance = theL; }

  //! Returns the distance between isoparameters used in the display of planes.
  double IsoDistance() const { return myIsoDistance; }

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream,
                                        int               theDepth = -1) const override;

protected:
  occ::handle<Prs3d_LineAspect> myEdgesAspect;
  occ::handle<Prs3d_LineAspect> myIsoAspect;
  occ::handle<Prs3d_LineAspect> myArrowAspect;
  double                        myArrowsLength;
  double                        myArrowsSize;
  double                        myArrowsAngle;
  double                        myPlaneXLength;
  double                        myPlaneYLength;
  double                        myIsoDistance;
  bool                          myDrawCenterArrow;
  bool                          myDrawEdgesArrows;
  bool                          myDrawEdges;
  bool                          myDrawIso;
};

#endif // _Prs3d_PlaneAspect_HeaderFile
