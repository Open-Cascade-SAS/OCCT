// Created on: 2012-06-20
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#ifndef _Graphic3d_Vertex_HeaderFile
#define _Graphic3d_Vertex_HeaderFile

#include <Standard_Macro.hxx>
#include <InterfaceGraphic_telem.hxx>
#include <Standard_ShortReal.hxx>
#include <Standard_Real.hxx>

//! This class represents a graphical 3D point. <br>
class Graphic3d_Vertex : public TEL_POINT
{
 public:
  //! Creates a point with 0.0, 0.0, 0.0 coordinates. <br>
  Graphic3d_Vertex()
  { SetCoord(0.f,0.f,0.f); }
  //! Creates a point with coordinates identical to <APoint>. <br>
  Graphic3d_Vertex(const Graphic3d_Vertex& APoint)
  { SetCoord(APoint.X(),APoint.Y(),APoint.Z()); }
  //! Creates a point with <AX>, <AY> and <AZ> coordinates. <br>
  Graphic3d_Vertex(const Standard_ShortReal AX,const Standard_ShortReal AY,const Standard_ShortReal AZ)
  { SetCoord(AX,AY,AZ); }
  //! Creates a point with <AX>, <AY> and <AZ> coordinates. <br>
  Graphic3d_Vertex(const Standard_Real AX,const Standard_Real AY,const Standard_Real AZ)
  { SetCoord(AX,AY,AZ); }
  //! Modifies the coordinates of the point <me>. <br>
  void SetCoord(const Standard_ShortReal AX,const Standard_ShortReal AY,const Standard_ShortReal AZ)
  { xyz[0] = AX, xyz[1] = AY, xyz[2] = AZ; }
  //! Modifies the coordinates of the point <me>. <br>
  void SetCoord(const Standard_Real AX,const Standard_Real AY,const Standard_Real AZ)
  { xyz[0] = Standard_ShortReal(AX), xyz[1] = Standard_ShortReal(AY), xyz[2] = Standard_ShortReal(AZ); }
  //! Returns the coordinates of the point <me>. <br>
  void Coord(Standard_ShortReal& AX,Standard_ShortReal& AY,Standard_ShortReal& AZ) const
  { AX = xyz[0], AY = xyz[1], AZ = xyz[2]; }
  //! Returns the coordinates of the point <me>. <br>
  void Coord(Standard_Real& AX,Standard_Real& AY,Standard_Real& AZ) const
  { AX = xyz[0], AY = xyz[1], AZ = xyz[2]; }
  //! Returns the X coordinates of the point <me>. <br>
  Standard_ShortReal X() const { return xyz[0]; }
  //! Returns the Y coordinate of the point <me>. <br>
  Standard_ShortReal Y() const { return xyz[1]; }
  //! Returns the Z coordinate of the point <me>. <br>
  Standard_ShortReal Z() const { return xyz[2]; }
  //! Returns the distance between <AV1> and <AV2>. <br>
  Standard_EXPORT Standard_ShortReal Distance(const Graphic3d_Vertex& AOther) const;
};

#endif
