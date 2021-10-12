// Created on: 1993-07-19
// Created by: Remi LEQUETTE
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

#ifndef _BRepTools_ShapeSet_HeaderFile
#define _BRepTools_ShapeSet_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRep_Builder.hxx>
#include <GeomTools_SurfaceSet.hxx>
#include <GeomTools_CurveSet.hxx>
#include <GeomTools_Curve2dSet.hxx>
#include <TColStd_IndexedMapOfTransient.hxx>
#include <Standard_Boolean.hxx>
#include <TopTools_ShapeSet.hxx>
#include <Standard_OStream.hxx>
#include <Standard_IStream.hxx>
#include <TopAbs_ShapeEnum.hxx>

class TopoDS_Shape;


//! Contains a Shape and all  its subshapes, locations
//! and geometries.
//!
//! The topology is inherited from TopTools.
class BRepTools_ShapeSet  : public TopTools_ShapeSet
{
public:

  DEFINE_STANDARD_ALLOC

  //! Builds an empty ShapeSet.
  //! @param theWithTriangles flag to write triangulation data
  Standard_EXPORT BRepTools_ShapeSet (const Standard_Boolean theWithTriangles = Standard_True,
                                      const Standard_Boolean theWithNormals = Standard_False);
  
  //! Builds an empty ShapeSet.
  //! @param theWithTriangles flag to write triangulation data
  Standard_EXPORT BRepTools_ShapeSet (const BRep_Builder& theBuilder,
                                      const Standard_Boolean theWithTriangles = Standard_True,
                                      const Standard_Boolean theWithNormals = Standard_False);

  Standard_EXPORT virtual ~BRepTools_ShapeSet();

  //! Return true if shape should be stored with triangles.
  Standard_Boolean IsWithTriangles() const { return myWithTriangles; }
  //! Return true if shape should be stored triangulation with normals.
  Standard_Boolean IsWithNormals() const { return myWithNormals; }

  //! Define if shape will be stored with triangles.
  //! Ignored (always written) if face defines only triangulation (no surface).
  void SetWithTriangles (const Standard_Boolean theWithTriangles) { myWithTriangles = theWithTriangles; }
  //! Define if shape will be stored triangulation with normals.
  //! Ignored (always written) if face defines only triangulation (no surface).
  void SetWithNormals (const Standard_Boolean theWithNormals) { myWithNormals = theWithNormals; }

  //! Clears the content of the set.
  Standard_EXPORT virtual void Clear() Standard_OVERRIDE;
  
  //! Stores the goemetry of <S>.
  Standard_EXPORT virtual void AddGeometry (const TopoDS_Shape& S) Standard_OVERRIDE;
  
  //! Dumps the geometry of me on the stream <OS>.
  Standard_EXPORT virtual void DumpGeometry (Standard_OStream& OS) const Standard_OVERRIDE;
  
  //! Writes the geometry of  me  on the stream <OS> in a
  //! format that can be read back by Read.
  Standard_EXPORT virtual void WriteGeometry (Standard_OStream& OS,
                                              const Message_ProgressRange& theProgress = Message_ProgressRange()) Standard_OVERRIDE;
  
  //! Reads the geometry of me from the  stream  <IS>.
  Standard_EXPORT virtual void ReadGeometry (Standard_IStream& IS,
                                             const Message_ProgressRange& theProgress = Message_ProgressRange()) Standard_OVERRIDE;
  
  //! Dumps the geometry of <S> on the stream <OS>.
  Standard_EXPORT virtual void DumpGeometry (const TopoDS_Shape& S, Standard_OStream& OS) const Standard_OVERRIDE;
  
  //! Writes the geometry of <S>  on the stream <OS> in a
  //! format that can be read back by Read.
  Standard_EXPORT virtual void WriteGeometry (const TopoDS_Shape& S, Standard_OStream& OS) const Standard_OVERRIDE;
  
  //! Reads the geometry of a shape of type <T> from the
  //! stream <IS> and returns it in <S>.
  Standard_EXPORT virtual void ReadGeometry (const TopAbs_ShapeEnum T, Standard_IStream& IS, TopoDS_Shape& S) Standard_OVERRIDE;
  
  //! Inserts  the shape <S2> in  the  shape <S1>.  This
  //! method must be   redefined  to  use   the  correct
  //! builder.
  Standard_EXPORT virtual void AddShapes (TopoDS_Shape& S1, const TopoDS_Shape& S2) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void Check (const TopAbs_ShapeEnum T, TopoDS_Shape& S) Standard_OVERRIDE;
  
  //! Reads the 3d polygons  of me
  //! from the  stream  <IS>.
  Standard_EXPORT void ReadPolygon3D (Standard_IStream& IS,
                                      const Message_ProgressRange& theProgress = Message_ProgressRange());
  
  //! Writes the 3d polygons
  //! on the stream <OS> in a format that can
  //! be read back by Read.
  Standard_EXPORT void WritePolygon3D (Standard_OStream& OS,
                                       const Standard_Boolean Compact = Standard_True,
                                       const Message_ProgressRange& theProgress = Message_ProgressRange()) const;
  
  //! Dumps the 3d polygons
  //! on the stream <OS>.
  Standard_EXPORT void DumpPolygon3D (Standard_OStream& OS) const;
  
  //! Reads the triangulation of me
  //! from the  stream  <IS>.
  Standard_EXPORT void ReadTriangulation (Standard_IStream& IS,
                                          const Message_ProgressRange& theProgress = Message_ProgressRange());
  
  //! Writes the triangulation
  //! on the stream <OS> in a format that can
  //! be read back by Read.
  Standard_EXPORT void WriteTriangulation (Standard_OStream& OS,
                                           const Standard_Boolean Compact = Standard_True,
                                           const Message_ProgressRange& theProgress = Message_ProgressRange()) const;
  
  //! Dumps the triangulation
  //! on the stream <OS>.
  Standard_EXPORT void DumpTriangulation (Standard_OStream& OS) const;
  
  //! Reads the polygons on triangulation of me
  //! from the  stream  <IS>.
  Standard_EXPORT void ReadPolygonOnTriangulation (Standard_IStream& IS,
                                                   const Message_ProgressRange& theProgress = Message_ProgressRange());
  
  //! Writes the polygons on triangulation
  //! on the stream <OS> in a format that can
  //! be read back by Read.
  Standard_EXPORT void WritePolygonOnTriangulation (Standard_OStream& OS,
                                                    const Standard_Boolean Compact = Standard_True,
                                                    const Message_ProgressRange& theProgress = Message_ProgressRange()) const;
  
  //! Dumps the polygons on triangulation
  //! on the stream <OS>.
  Standard_EXPORT void DumpPolygonOnTriangulation (Standard_OStream& OS) const;




protected:





private:



  BRep_Builder myBuilder;
  GeomTools_SurfaceSet mySurfaces;
  GeomTools_CurveSet myCurves;
  GeomTools_Curve2dSet myCurves2d;
  TColStd_IndexedMapOfTransient myPolygons2D;
  TColStd_IndexedMapOfTransient myPolygons3D;
  NCollection_IndexedDataMap<Handle(Poly_Triangulation),
                             Standard_Boolean> myTriangulations; //!< Contains a boolean flag with information
                                                                 //!  to save normals for triangulation
  TColStd_IndexedMapOfTransient myNodes;
  Standard_Boolean myWithTriangles;
  Standard_Boolean myWithNormals;

};

#endif // _BRepTools_ShapeSet_HeaderFile
