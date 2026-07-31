// Created on: 1999-03-05
// Created by: Fabrice SERVANT
// Copyright (c) 1999 Matra Datavision
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

#ifndef _IntPolyh_Edge_HeaderFile
#define _IntPolyh_Edge_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

//! The class represents the edge built between the two IntPolyh points.
//! It is linked to two IntPolyh triangles.
class IntPolyh_Edge
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor
  IntPolyh_Edge()
      : myPoint1(-1),
        myPoint2(-1),
        myTriangle1(-1),
        myTriangle2(-1)
  {
  }

  //! Constructor
  IntPolyh_Edge(const int thePoint1,
                const int thePoint2,
                const int theTriangle1,
                const int theTriangle2)
      : myPoint1(thePoint1),
        myPoint2(thePoint2),
        myTriangle1(theTriangle1),
        myTriangle2(theTriangle2)
  {
  }

  //! Returns the first point
  int FirstPoint() const { return myPoint1; }

  //! Returns the second point
  int SecondPoint() const { return myPoint2; }

  //! Returns the first triangle
  int FirstTriangle() const { return myTriangle1; }

  //! Returns the second triangle
  int SecondTriangle() const { return myTriangle2; }

  //! Sets the first point
  void SetFirstPoint(const int thePoint) { myPoint1 = thePoint; }

  //! Sets the second point
  void SetSecondPoint(const int thePoint) { myPoint2 = thePoint; }

  //! Sets the first triangle
  void SetFirstTriangle(const int theTriangle) { myTriangle1 = theTriangle; }

  //! Sets the second triangle
  void SetSecondTriangle(const int theTriangle) { myTriangle2 = theTriangle; }

  Standard_EXPORT void Dump(const int v) const;

private:
  int myPoint1;
  int myPoint2;
  int myTriangle1;
  int myTriangle2;
};

#endif // _IntPolyh_Edge_HeaderFile
