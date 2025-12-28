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
#include <Standard_OStream.hxx>

//! This class represents a graphical 3D point.
class Graphic3d_Vertex
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates a point with 0.0, 0.0, 0.0 coordinates.
  Graphic3d_Vertex() { SetCoord(0.0f, 0.0f, 0.0f); }

  //! Creates a point with theX, theY and theZ coordinates.
  Graphic3d_Vertex(const float theX, const float theY, const float theZ)
  {
    SetCoord(theX, theY, theZ);
  }

  //! Creates a point with theX, theY and theZ coordinates.
  Graphic3d_Vertex(const double theX, const double theY, const double theZ)
  {
    SetCoord(theX, theY, theZ);
  }

  //! Modifies the coordinates.
  void SetCoord(const float theX, const float theY, const float theZ)
  {
    xyz[0] = theX;
    xyz[1] = theY;
    xyz[2] = theZ;
  }

  //! Modifies the coordinates.
  void SetCoord(const double theX, const double theY, const double theZ)
  {
    xyz[0] = float(theX);
    xyz[1] = float(theY);
    xyz[2] = float(theZ);
  }

  //! Returns the coordinates.
  void Coord(float& theX, float& theY, float& theZ) const
  {
    theX = xyz[0];
    theY = xyz[1];
    theZ = xyz[2];
  }

  //! Returns the coordinates.
  void Coord(double& theX, double& theY, double& theZ) const
  {
    theX = xyz[0];
    theY = xyz[1];
    theZ = xyz[2];
  }

  //! Returns the X coordinates.
  float X() const { return xyz[0]; }

  //! Returns the Y coordinate.
  float Y() const { return xyz[1]; }

  //! Returns the Z coordinate.
  float Z() const { return xyz[2]; }

  //! Returns the distance between two points.
  Standard_EXPORT float Distance(const Graphic3d_Vertex& theOther) const;

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const;

  float xyz[3];
};

#endif
