// Copyright (c) 2021 OPEN CASCADE SAS
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

#ifndef _BinTools_ShapeReader_HeaderFile
#define _BinTools_ShapeReader_HeaderFile

#include <BinTools_ShapeSetBase.hxx>
#include <BinTools_IStream.hxx>
#include <NCollection_DataMap.hxx>

class TopLoc_Location;
class Geom_Curve;
class Geom2d_Curve;
class Geom_Surface;
class Poly_Polygon3D;
class Poly_PolygonOnTriangulation;
class Poly_Triangulation;

//! Reads topology from IStream in binary format without grouping of objects by types
//! and using relative positions in a file as references.
class BinTools_ShapeReader : public BinTools_ShapeSetBase
{
public:

  DEFINE_STANDARD_ALLOC
  
  //! Initializes a shape reader.
  Standard_EXPORT BinTools_ShapeReader();
  
  Standard_EXPORT virtual ~BinTools_ShapeReader();

  //! Clears the content of the set.
  Standard_EXPORT virtual void Clear() Standard_OVERRIDE;
  
  //! Reads the shape from stream using previously restored shapes and objects by references.
  Standard_EXPORT void Read (Standard_IStream& theStream, TopoDS_Shape& theShape) Standard_OVERRIDE;

  //! Reads location from the stream.
  Standard_EXPORT const TopLoc_Location* ReadLocation (BinTools_IStream& theStream);

private:
  //! Reads the shape from stream using previously restored shapes and objects by references.
  TopoDS_Shape ReadShape (BinTools_IStream& theStream);
  //! Reads curve from the stream.
  Handle(Geom_Curve) ReadCurve (BinTools_IStream& theStream);
  //! Reads curve2d from the stream.
  Handle(Geom2d_Curve) ReadCurve2d (BinTools_IStream& theStream);
  //! Reads surface from the stream.
  Handle(Geom_Surface) ReadSurface (BinTools_IStream& theStream);
  //! Reads ploygon3d from the stream.
  Handle(Poly_Polygon3D) ReadPolygon3d (BinTools_IStream& theStream);
  //! Reads polygon on triangulation from the stream.
  Handle(Poly_PolygonOnTriangulation) ReadPolygon (BinTools_IStream& theStream);
  //! Reads triangulation from the stream.
  Handle(Poly_Triangulation) ReadTriangulation (BinTools_IStream& theStream);

  /// position of the shape previously restored
  NCollection_DataMap<uint64_t, TopoDS_Shape> myShapePos;
  NCollection_DataMap<uint64_t, TopLoc_Location> myLocationPos;
  NCollection_DataMap<uint64_t, Handle(Geom_Curve)> myCurvePos;
  NCollection_DataMap<uint64_t, Handle(Geom2d_Curve)> myCurve2dPos;
  NCollection_DataMap<uint64_t, Handle(Geom_Surface)> mySurfacePos;
  NCollection_DataMap<uint64_t, Handle(Poly_Polygon3D)> myPolygon3dPos;
  NCollection_DataMap<uint64_t, Handle(Poly_PolygonOnTriangulation)> myPolygonPos;
  NCollection_DataMap<uint64_t, Handle(Poly_Triangulation)> myTriangulationPos;
};

#endif // _BinTools_ShapeReader_HeaderFile
