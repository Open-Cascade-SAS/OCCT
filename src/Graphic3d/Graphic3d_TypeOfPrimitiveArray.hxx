// Created on: 1993-03-31
// Created by: NW,JPB,CAL
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

#ifndef _Graphic3d_TypeOfPrimitiveArray_HeaderFile
#define _Graphic3d_TypeOfPrimitiveArray_HeaderFile

//! The type of primitive array in a group in a structure.
enum Graphic3d_TypeOfPrimitiveArray
{
  Graphic3d_TOPA_UNDEFINED, //!< undefined primitive type
  // main rendering types
  Graphic3d_TOPA_POINTS,    //!< individual points
  Graphic3d_TOPA_SEGMENTS,  //!< segments array - each 2 vertexes define 1 segment
  Graphic3d_TOPA_POLYLINES, //!< line strip - each new vertex in array defines segment with previous
                            //!< one
  Graphic3d_TOPA_TRIANGLES, //!< triangle array - each 3 vertexes define 1 triangle
  Graphic3d_TOPA_TRIANGLESTRIPS, //!< triangle strip - each new vertex in array defines triangle
                                 //!< with 2 previous vertexes
  Graphic3d_TOPA_TRIANGLEFANS, //!< triangle fan - each new vertex in array define triangle with the
                               //!< previous vertex and the very first vertex (fan center)
  // rendering type with auxiliary adjacent info (can be accessed only within Geometry shader)
  Graphic3d_TOPA_LINES_ADJACENCY, //!< ADVANCED - same as Graphic3d_TOPA_SEGMENTS, but each pair of
                                  //!< vertexes defining 1 segment
                                  //!  is preceded by 1 extra vertex and followed by 1 extra vertex
                                  //!  which are not actually rendered
  Graphic3d_TOPA_LINE_STRIP_ADJACENCY, //!< ADVANCED - same as Graphic3d_TOPA_POLYLINES, but each
                                       //!< sequence of vertexes defining 1 polyline
                                       //!  is preceded by 1 extra vertex and followed by 1 extra
                                       //!  vertex which are not actually rendered
  Graphic3d_TOPA_TRIANGLES_ADJACENCY,  //!< ADVANCED - same as Graphic3d_TOPA_TRIANGLES, but each
                                       //!< vertex defining of triangle
                                       //!  is followed by 1 extra adjacent vertex which is not
                                       //!  actually rendered
  Graphic3d_TOPA_TRIANGLE_STRIP_ADJACENCY, //!< ADVANCED - same as Graphic3d_TOPA_TRIANGLESTRIPS,
                                           //!< but with extra adjacent vertexes
  // deprecated types, unsupported by mobile hardware
  Graphic3d_TOPA_QUADRANGLES,      //!< DEPRECATED - triangle array should be used instead;
                                   //!  array of quads - each 4 vertexes define single quad
  Graphic3d_TOPA_QUADRANGLESTRIPS, //!< DEPRECATED - triangle array should be used instead;
                                   //!  quad strip - each 2 new vertexes define a quad shared 2 more
                                   //!  vertexes of previous quad
  Graphic3d_TOPA_POLYGONS          //!< DEPRECATED - triangle array should be used instead;
                                   //!  array defines a polygon
};

#endif // _Graphic3d_TypeOfPrimitiveArray_HeaderFile
