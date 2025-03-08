// Created on: 1993-12-02
// Created by: Jacques GOUSSARD
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

#ifndef _BRepBlend_Line_HeaderFile
#define _BRepBlend_Line_HeaderFile

#include <Blend_SequenceOfPoint.hxx>
#include <BRepBlend_Extremity.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Integer.hxx>

class Blend_Point;

class BRepBlend_Line;
DEFINE_STANDARD_HANDLE(BRepBlend_Line, Standard_Transient)

class BRepBlend_Line : public Standard_Transient
{

public:
  Standard_EXPORT BRepBlend_Line();

  //! Clears the content of the line.
  Standard_EXPORT void Clear();

  //! Adds a point in the line.
  void Append(const Blend_Point& P);

  //! Adds a point in the line at the first place.
  void Prepend(const Blend_Point& P);

  //! Adds a point in the line at the first place.
  void InsertBefore(const Standard_Integer Index, const Blend_Point& P);

  //! Removes  from  <me>    all  the  items  of
  //! positions between <FromIndex> and <ToIndex>.
  //! Raises an exception if the indices are out of bounds.
  void Remove(const Standard_Integer FromIndex, const Standard_Integer ToIndex);

  //! Sets the value of the transition of the line on S1 and
  //! the line on S2.
  Standard_EXPORT void Set(const IntSurf_TypeTrans TranS1, const IntSurf_TypeTrans TranS2);

  //! Sets the value of the transition of the line on a surface
  Standard_EXPORT void Set(const IntSurf_TypeTrans Trans);

  //! Sets the values of the start points for the line.
  void SetStartPoints(const BRepBlend_Extremity& StartPt1, const BRepBlend_Extremity& StartPt2);

  //! Sets tne values of the end points for the line.
  void SetEndPoints(const BRepBlend_Extremity& EndPt1, const BRepBlend_Extremity& EndPt2);

  //! Returns the number of points in the line.
  Standard_Integer NbPoints() const;

  //! Returns the point of range Index.
  const Blend_Point& Point(const Standard_Integer Index) const;

  //! Returns the type of the transition of the line defined
  //! on the first surface. The transition is "constant"
  //! along the line.
  //! The transition is IN if the line is oriented in such
  //! a way that the system of vectors (N,DRac,T) is
  //! right-handed, where
  //! N is the normal to the first surface at a point P,
  //! DRac is a vector tangent to the blending patch,
  //! oriented towards the valid part of this patch,
  //! T  is the tangent to the line on S1 at P.
  //! The transitioon is OUT when the system of vectors is
  //! left-handed.
  IntSurf_TypeTrans TransitionOnS1() const;

  //! Returns the type of the transition of the line defined
  //! on the second surface. The transition is "constant"
  //! along the line.
  IntSurf_TypeTrans TransitionOnS2() const;

  //! Returns the start point on S1.
  const BRepBlend_Extremity& StartPointOnFirst() const;

  //! Returns the start point on S2
  const BRepBlend_Extremity& StartPointOnSecond() const;

  //! Returns the end point on S1.
  const BRepBlend_Extremity& EndPointOnFirst() const;

  //! Returns the point on S2.
  const BRepBlend_Extremity& EndPointOnSecond() const;

  //! Returns the type of the transition of the line defined
  //! on the surface.
  IntSurf_TypeTrans TransitionOnS() const;

  DEFINE_STANDARD_RTTIEXT(BRepBlend_Line, Standard_Transient)

protected:
private:
  Blend_SequenceOfPoint seqpt;
  IntSurf_TypeTrans     tras1;
  IntSurf_TypeTrans     tras2;
  BRepBlend_Extremity   stp1;
  BRepBlend_Extremity   stp2;
  BRepBlend_Extremity   endp1;
  BRepBlend_Extremity   endp2;
  Standard_Boolean      hass1;
  Standard_Boolean      hass2;
};

#include <BRepBlend_Line.lxx>

#endif // _BRepBlend_Line_HeaderFile
