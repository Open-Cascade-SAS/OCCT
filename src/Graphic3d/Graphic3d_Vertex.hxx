// Created on: 2012-06-20
// Created by: Sergey ZERCHANINOV
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

#ifndef _Graphic3d_Vertex_HeaderFile
#define _Graphic3d_Vertex_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Macro.hxx>
#include <Standard_ShortReal.hxx>
#include <Standard_Real.hxx>

//! This class represents a graphical 3D point.
class Graphic3d_Vertex
{
public:

  DEFINE_STANDARD_ALLOC

  //! Creates a point with 0.0, 0.0, 0.0 coordinates.
  Graphic3d_Vertex()
  {
    SetCoord (0.0f, 0.0f, 0.0f);
  }

  //! Creates a point with coordinates identical to thePoint.
  Graphic3d_Vertex (const Graphic3d_Vertex& thePoint)
  {
    SetCoord (thePoint.X(), thePoint.Y(), thePoint.Z());
  }

  //! Creates a point with theX, theY and theZ coordinates.
  Graphic3d_Vertex (const Standard_ShortReal theX,
                    const Standard_ShortReal theY,
                    const Standard_ShortReal theZ)
  {
    SetCoord (theX, theY, theZ);
  }

  //! Creates a point with theX, theY and theZ coordinates.
  Graphic3d_Vertex (const Standard_Real theX,
                    const Standard_Real theY,
                    const Standard_Real theZ)
  {
    SetCoord (theX, theY, theZ);
  }

  //! Modifies the coordinates.
  void SetCoord (const Standard_ShortReal theX,
                 const Standard_ShortReal theY,
                 const Standard_ShortReal theZ)
  {
    xyz[0] = theX;
    xyz[1] = theY;
    xyz[2] = theZ;
  }

  //! Modifies the coordinates.
  void SetCoord (const Standard_Real theX,
                 const Standard_Real theY,
                 const Standard_Real theZ)
  {
    xyz[0] = Standard_ShortReal (theX);
    xyz[1] = Standard_ShortReal (theY);
    xyz[2] = Standard_ShortReal (theZ);
  }

  //! Returns the coordinates.
  void Coord (Standard_ShortReal& theX,
              Standard_ShortReal& theY,
              Standard_ShortReal& theZ) const
  {
    theX = xyz[0];
    theY = xyz[1];
    theZ = xyz[2];
  }

  //! Returns the coordinates.
  void Coord (Standard_Real& theX,
              Standard_Real& theY,
              Standard_Real& theZ) const
  {
    theX = xyz[0];
    theY = xyz[1];
    theZ = xyz[2];
  }

  //! Returns the X coordinates.
  Standard_ShortReal X() const { return xyz[0]; }

  //! Returns the Y coordinate.
  Standard_ShortReal Y() const { return xyz[1]; }

  //! Returns the Z coordinate.
  Standard_ShortReal Z() const { return xyz[2]; }

  //! Returns the distance between two points.
  Standard_EXPORT Standard_ShortReal Distance (const Graphic3d_Vertex& theOther) const;

  float xyz[3];

};

#endif
