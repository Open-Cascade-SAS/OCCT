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

#ifndef _BinTools_ShapeWriter_HeaderFile
#define _BinTools_ShapeWriter_HeaderFile

#include <BinTools_ShapeSetBase.hxx>
#include <BinTools_OStream.hxx>
#include <NCollection_DataMap.hxx>
#include <TopTools_ShapeMapHasher.hxx>

class Geom_Curve;
class Geom2d_Curve;
class Geom_Surface;
class Poly_Polygon3D;
class Poly_PolygonOnTriangulation;
class Poly_Triangulation;

//! Writes topology in OStream in binary format without grouping of objects by types
//! and using relative positions in a file as references.
class BinTools_ShapeWriter : public BinTools_ShapeSetBase
{
public:
  DEFINE_STANDARD_ALLOC

  //! Builds an empty ShapeSet.
  //! Parameter <theWithTriangles> is added for XML Persistence
  Standard_EXPORT BinTools_ShapeWriter();

  Standard_EXPORT virtual ~BinTools_ShapeWriter();

  //! Clears the content of the set.
  Standard_EXPORT virtual void Clear() Standard_OVERRIDE;

  //! Writes the shape to stream using previously stored shapes and objects to refer them.
  Standard_EXPORT virtual void Write(const TopoDS_Shape& theShape,
                                     Standard_OStream&   theStream) Standard_OVERRIDE;

  //! Writes location to the stream (all the needed sub-information or reference if it is already
  //! used).
  Standard_EXPORT virtual void WriteLocation(BinTools_OStream&      theStream,
                                             const TopLoc_Location& theLocation);

private:
  //! Writes shape to the stream (all the needed sub-information or reference if it is already
  //! used).
  virtual void WriteShape(BinTools_OStream& theStream, const TopoDS_Shape& theShape);
  //! Writes curve to the stream (all the needed sub-information or reference if it is already
  //! used).
  void WriteCurve(BinTools_OStream& theStream, const Handle(Geom_Curve)& theCurve);
  //! Writes curve2d to the stream (all the needed sub-information or reference if it is already
  //! used).
  void WriteCurve(BinTools_OStream& theStream, const Handle(Geom2d_Curve)& theCurve);
  //! Writes surface to the stream.
  void WriteSurface(BinTools_OStream& theStream, const Handle(Geom_Surface)& theSurface);
  //! Writes ploygon3d to the stream.
  void WritePolygon(BinTools_OStream& theStream, const Handle(Poly_Polygon3D)& thePolygon);
  //! Writes polygon on triangulation to the stream.
  void WritePolygon(BinTools_OStream&                          theStream,
                    const Handle(Poly_PolygonOnTriangulation)& thePolygon);
  //! Writes triangulation to the stream.
  void WriteTriangulation(BinTools_OStream&                 theStream,
                          const Handle(Poly_Triangulation)& theTriangulation,
                          const Standard_Boolean            theNeedToWriteNormals);

  /// position of the shape previously stored
  NCollection_DataMap<TopoDS_Shape, uint64_t, TopTools_ShapeMapHasher> myShapePos;
  NCollection_DataMap<TopLoc_Location, uint64_t>                       myLocationPos;
  NCollection_DataMap<Handle(Geom_Curve), uint64_t>                    myCurvePos;
  NCollection_DataMap<Handle(Geom2d_Curve), uint64_t>                  myCurve2dPos;
  NCollection_DataMap<Handle(Geom_Surface), uint64_t>                  mySurfacePos;
  NCollection_DataMap<Handle(Poly_Polygon3D), uint64_t>                myPolygon3dPos;
  NCollection_DataMap<Handle(Poly_PolygonOnTriangulation), uint64_t>   myPolygonPos;
  NCollection_DataMap<Handle(Poly_Triangulation), uint64_t>            myTriangulationPos;
};

#endif // _BinTools_ShapeWriter_HeaderFile
