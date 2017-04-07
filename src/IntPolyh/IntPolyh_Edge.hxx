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

#include <Standard_Integer.hxx>

//! The class represents the edge built between the two IntPolyh points.<br>
//! It is linked to two IntPolyh triangles.
class IntPolyh_Edge
{
public:

  DEFINE_STANDARD_ALLOC

  //! Constructor
  IntPolyh_Edge() :
    myPoint1(-1), myPoint2(-1), myTriangle1(-1), myTriangle2(-1)
  {}
  //! Constructor
  IntPolyh_Edge(const Standard_Integer thePoint1,
                const Standard_Integer thePoint2,
                const Standard_Integer theTriangle1,
                const Standard_Integer theTriangle2)
  :
    myPoint1(thePoint1),
    myPoint2(thePoint2),
    myTriangle1(theTriangle1),
    myTriangle2(theTriangle2)
  {}

  //! Returns the first point
  Standard_Integer FirstPoint() const
  {
    return myPoint1;
  }
  //! Returns the second point
  Standard_Integer SecondPoint() const
  {
    return myPoint2;
  }
  //! Returns the first triangle
  Standard_Integer FirstTriangle() const
  {
    return myTriangle1;
  }
  //! Returns the second triangle
  Standard_Integer SecondTriangle() const
  {
    return myTriangle2;
  }
  //! Sets the first point
  void SetFirstPoint (const Standard_Integer thePoint)
  {
    myPoint1 = thePoint;
  }
  //! Sets the second point
  void SetSecondPoint (const Standard_Integer thePoint)
  {
    myPoint2 = thePoint;
  }
  //! Sets the first triangle
  void SetFirstTriangle (const Standard_Integer theTriangle)
  {
    myTriangle1 = theTriangle;
  }
  //! Sets the second triangle
  void SetSecondTriangle (const Standard_Integer theTriangle)
  {
    myTriangle2 = theTriangle;
  }
  
  Standard_EXPORT void Dump (const Standard_Integer v) const;

protected:

private:

  Standard_Integer myPoint1;
  Standard_Integer myPoint2;
  Standard_Integer myTriangle1;
  Standard_Integer myTriangle2;
};

#endif // _IntPolyh_Edge_HeaderFile
