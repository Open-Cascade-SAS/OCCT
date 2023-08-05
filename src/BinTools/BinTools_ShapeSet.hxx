// Created on: 2004-05-11
// Created by: Sergey ZARITCHNY <szy@opencascade.com>
// Copyright (c) 2004-2014 OPEN CASCADE SAS
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

#ifndef _BinTools_ShapeSet_HeaderFile
#define _BinTools_ShapeSet_HeaderFile

#include <BinTools_ShapeSetBase.hxx>

#include <TopTools_IndexedMapOfShape.hxx>
#include <BinTools_LocationSet.hxx>
#include <BRep_Builder.hxx>
#include <BinTools_SurfaceSet.hxx>
#include <BinTools_CurveSet.hxx>
#include <BinTools_Curve2dSet.hxx>
#include <TColStd_IndexedMapOfTransient.hxx>
#include <Standard_OStream.hxx>
#include <Standard_IStream.hxx>


//! Writes topology in OStream in binary format
class BinTools_ShapeSet : public BinTools_ShapeSetBase
{
public:

  DEFINE_STANDARD_ALLOC

  //! Builds an empty ShapeSet.
  //! @param theWithTriangles [in] flag to write triangulation data
  Standard_EXPORT BinTools_ShapeSet ();

  Standard_EXPORT virtual ~BinTools_ShapeSet();

  //! Clears the content of the set.
  Standard_EXPORT virtual void Clear();
  
  //! Stores <S> and its sub-shape. Returns the index of <S>.
  //! The method AddGeometry is called on each sub-shape.
  Standard_EXPORT Standard_Integer Add (const TopoDS_Shape& S);
  
  //! Returns the sub-shape of index <I>.
  Standard_EXPORT const TopoDS_Shape& Shape (const Standard_Integer I);
  
  //! Returns the index of <S>.
  Standard_EXPORT Standard_Integer Index (const TopoDS_Shape& S) const;
  
  Standard_EXPORT const BinTools_LocationSet& Locations() const;
  
  Standard_EXPORT BinTools_LocationSet& ChangeLocations();
  
  //! Returns number of shapes read from file.
  Standard_EXPORT Standard_Integer NbShapes() const;
  
  //! Writes the content of  me  on the stream <OS> in binary
  //! format that can be read back by Read.
  //!
  //! Writes the locations.
  //!
  //! Writes the geometry calling WriteGeometry.
  //!
  //! Dumps the shapes from last to first.
  //! For each shape  :
  //! Write the type.
  //! calls WriteGeometry(S).
  //! Write the flags, the subshapes.
  Standard_EXPORT virtual void Write
    (Standard_OStream& OS,
     const Message_ProgressRange& theRange = Message_ProgressRange());
  
  //! Reads the content of me from the binary stream  <IS>. me
  //! is first cleared.
  //!
  //! Reads the locations.
  //!
  //! Reads the geometry calling ReadGeometry.
  //!
  //! Reads the shapes.
  //! For each shape
  //! Reads the type.
  //! calls ReadGeometry(T,S).
  //! Reads the flag, the subshapes.
  Standard_EXPORT virtual void Read
    (Standard_IStream& IS,
     const Message_ProgressRange& theRange = Message_ProgressRange());
  
  //! Writes   on  <OS>   the shape   <S>.    Writes the
  //! orientation, the index of the TShape and the index
  //! of the Location.
  Standard_EXPORT virtual void Write (const TopoDS_Shape& S, Standard_OStream& OS);
  
  //! Writes the geometry of  me  on the stream <OS> in a
  //! binary format that can be read back by Read.
  Standard_EXPORT virtual void WriteGeometry
    (Standard_OStream& OS,
     const Message_ProgressRange& theRange = Message_ProgressRange()) const;
  
  //! Reads the geometry of me from the  stream  <IS>.
  Standard_EXPORT virtual void ReadGeometry
    (Standard_IStream& IS,
     const Message_ProgressRange& theRange = Message_ProgressRange());
  
  //! Reads from <IS> a shape flags and sub-shapes and modifies S.
  Standard_EXPORT virtual void ReadFlagsAndSubs
    (TopoDS_Shape& S, const TopAbs_ShapeEnum T,
     Standard_IStream& IS, const Standard_Integer NbShapes);

  //! Reads from <IS> a shape and returns it in S.
  //! <NbShapes> is the number of tshapes in the set.
  Standard_EXPORT virtual void ReadSubs
  (TopoDS_Shape& S, Standard_IStream& IS, const Standard_Integer NbShapes);

  //! An empty virtual method for redefinition in shape-reader.
  Standard_EXPORT virtual void Read (Standard_IStream& /*theStream*/, TopoDS_Shape& /*theShape*/) {};

  //! Writes the shape <S> on the stream <OS> in a
  //! binary format that can be read back by Read.
  Standard_EXPORT virtual void WriteShape (const TopoDS_Shape& S, Standard_OStream& OS) const;
  
  //! Reads  a shape of type <T> from the stream <IS> and returns it in <S>.
  Standard_EXPORT virtual void ReadShape (const TopAbs_ShapeEnum T, Standard_IStream& IS, TopoDS_Shape& S);

  //! Stores the shape <S>.
  Standard_EXPORT virtual void AddShape (const TopoDS_Shape& S);
  
  //! Inserts  the shape <S2> in the shape <S1>.
  Standard_EXPORT virtual void AddShapes (TopoDS_Shape& S1, const TopoDS_Shape& S2);
  
  //! Reads the 3d polygons  of me
  //! from the  stream  <IS>.
  Standard_EXPORT void ReadPolygon3D
    (Standard_IStream& IS,
        const Message_ProgressRange& theRange = Message_ProgressRange());
  
  //! Writes the 3d polygons
  //! on the stream <OS> in a format that can
  //! be read back by Read.
  Standard_EXPORT void WritePolygon3D
    (Standard_OStream& OS,
        const Message_ProgressRange& theRange = Message_ProgressRange()) const;
  
  //! Reads the triangulation of me
  //! from the  stream  <IS>.
  Standard_EXPORT void ReadTriangulation
    (Standard_IStream& IS,
        const Message_ProgressRange& theRange = Message_ProgressRange());
  
  //! Writes the triangulation
  //! on the stream <OS> in a format that can
  //! be read back by Read.
  Standard_EXPORT void WriteTriangulation
    (Standard_OStream& OS,
        const Message_ProgressRange& theRange = Message_ProgressRange()) const;
  
  //! Reads the polygons on triangulation of me
  //! from the  stream  <IS>.
  Standard_EXPORT void ReadPolygonOnTriangulation
    (Standard_IStream& IS,
        const Message_ProgressRange& theRange = Message_ProgressRange());
  
  //! Writes the polygons on triangulation
  //! on the stream <OS> in a format that can
  //! be read back by Read.
  Standard_EXPORT void WritePolygonOnTriangulation
    (Standard_OStream& OS,
        const Message_ProgressRange& theRange = Message_ProgressRange()) const;

private:

  TopTools_IndexedMapOfShape myShapes; ///< index and its shape (started from 1)
  BinTools_LocationSet myLocations;
  BRep_Builder myBuilder;
  BinTools_SurfaceSet mySurfaces;
  BinTools_CurveSet myCurves;
  BinTools_Curve2dSet myCurves2d;
  NCollection_IndexedMap<Handle(Poly_Polygon2D)> myPolygons2D;
  NCollection_IndexedMap<Handle(Poly_Polygon3D)> myPolygons3D;
  NCollection_IndexedDataMap<Handle(Poly_Triangulation),
                             Standard_Boolean> myTriangulations; //!< Contains a boolean flag with information
                                                                 //!  to save normals for triangulation
  NCollection_IndexedMap<Handle(Poly_PolygonOnTriangulation)> myNodes;
};

#endif // _BinTools_ShapeSet_HeaderFile
