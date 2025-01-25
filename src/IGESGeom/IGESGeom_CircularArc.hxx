// Created on: 1993-01-09
// Created by: CKY / Contract Toubro-Larsen ( Kiran )
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _IGESGeom_CircularArc_HeaderFile
#define _IGESGeom_CircularArc_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <gp_XY.hxx>
#include <IGESData_IGESEntity.hxx>
class gp_Pnt2d;
class gp_Pnt;
class gp_Dir;

class IGESGeom_CircularArc;
DEFINE_STANDARD_HANDLE(IGESGeom_CircularArc, IGESData_IGESEntity)

//! defines IGESCircularArc, Type <100> Form <0>
//! in package IGESGeom
//! A circular arc is a connected portion of a parent circle
//! which consists of more than one point. The definition space
//! coordinate system is always chosen so that the circular arc
//! remains in a plane either coincident with or parallel to
//! the XT, YT plane.
class IGESGeom_CircularArc : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESGeom_CircularArc();

  //! This method is used to set the fields of the class
  //! CircularArc
  //! - aZT     : Shift above the Z plane
  //! - aCenter : Center of the circle of which the arc forms a part
  //! - aStart  : Starting point of the circular arc
  //! - anEnd   : Ending point of the circular arc
  Standard_EXPORT void Init(const Standard_Real aZT,
                            const gp_XY&        aCenter,
                            const gp_XY&        aStart,
                            const gp_XY&        anEnd);

  //! returns the center of the circle of which arc forms a part
  Standard_EXPORT gp_Pnt2d Center() const;

  //! returns the center of the circle of which arc forms a part
  //! after applying Transf. Matrix
  Standard_EXPORT gp_Pnt TransformedCenter() const;

  //! returns the start point of the arc
  Standard_EXPORT gp_Pnt2d StartPoint() const;

  //! returns the start point of the arc after applying Transf. Matrix
  Standard_EXPORT gp_Pnt TransformedStartPoint() const;

  //! returns the parallel displacement of the plane containing the
  //! arc from the XT, YT plane
  Standard_EXPORT Standard_Real ZPlane() const;

  //! returns the end point of the arc
  Standard_EXPORT gp_Pnt2d EndPoint() const;

  //! returns the end point of the arc after applying Transf. Matrix
  Standard_EXPORT gp_Pnt TransformedEndPoint() const;

  //! returns the radius of the circle of which arc forms a part
  Standard_EXPORT Standard_Real Radius() const;

  //! returns the angle subtended by the arc at the center in radians
  Standard_EXPORT Standard_Real Angle() const;

  //! Z-Axis of circle (i.e. [0,0,1])
  Standard_EXPORT gp_Dir Axis() const;

  //! Z-Axis after applying Trans. Matrix
  Standard_EXPORT gp_Dir TransformedAxis() const;

  //! True if StartPoint = EndPoint
  Standard_EXPORT Standard_Boolean IsClosed() const;

  DEFINE_STANDARD_RTTIEXT(IGESGeom_CircularArc, IGESData_IGESEntity)

protected:
private:
  Standard_Real theZT;
  gp_XY         theCenter;
  gp_XY         theStart;
  gp_XY         theEnd;
};

#endif // _IGESGeom_CircularArc_HeaderFile
