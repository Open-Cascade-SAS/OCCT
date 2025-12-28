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

#ifndef _BRepExtrema_DistShapeShape_HeaderFile
#define _BRepExtrema_DistShapeShape_HeaderFile

#include <Bnd_Box.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Sequence.hxx>
#include <BRepExtrema_SolutionElem.hxx>
#include <BRepExtrema_SolutionElem.hxx>
#include <BRepExtrema_SupportType.hxx>
#include <Extrema_ExtAlgo.hxx>
#include <Extrema_ExtFlag.hxx>
#include <Message_ProgressRange.hxx>
#include <TopoDS_Shape.hxx>
#include <Standard_OStream.hxx>
#include <Standard_DefineAlloc.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedMap.hxx>

//! This class provides tools to compute minimum distance
//! between two Shapes (Compound,CompSolid, Solid, Shell, Face, Wire, Edge, Vertex).
class BRepExtrema_DistShapeShape
{
public:
  DEFINE_STANDARD_ALLOC

  //! create empty tool
  Standard_EXPORT BRepExtrema_DistShapeShape();

  //! create tool and computation of the minimum distance (value and pair of points)
  //! using default deflection in single thread mode.
  //! Default deflection value is Precision::Confusion().
  //! @param Shape1 - the first shape for distance computation
  //! @param Shape2 - the second shape for distance computation
  //! @param F and @param A are not used in computation and are obsolete.
  //! @param theRange - the progress indicator of algorithm
  Standard_EXPORT BRepExtrema_DistShapeShape(
    const TopoDS_Shape&          Shape1,
    const TopoDS_Shape&          Shape2,
    const Extrema_ExtFlag        F        = Extrema_ExtFlag_MINMAX,
    const Extrema_ExtAlgo        A        = Extrema_ExtAlgo_Grad,
    const Message_ProgressRange& theRange = Message_ProgressRange());
  //! create tool and computation of the minimum distance
  //! (value and pair of points) in single thread mode.
  //! Default deflection value is Precision::Confusion().
  //! @param Shape1 - the first shape for distance computation
  //! @param Shape2 - the second shape for distance computation
  //! @param theDeflection - the presition of distance computation
  //! @param F and @param A are not used in computation and are obsolete.
  //! @param theRange - the progress indicator of algorithm
  Standard_EXPORT BRepExtrema_DistShapeShape(
    const TopoDS_Shape&          Shape1,
    const TopoDS_Shape&          Shape2,
    const double          theDeflection,
    const Extrema_ExtFlag        F        = Extrema_ExtFlag_MINMAX,
    const Extrema_ExtAlgo        A        = Extrema_ExtAlgo_Grad,
    const Message_ProgressRange& theRange = Message_ProgressRange());

  //! Sets deflection to computation of the minimum distance
  void SetDeflection(const double theDeflection) { myEps = theDeflection; }

  //! load first shape into extrema
  Standard_EXPORT void LoadS1(const TopoDS_Shape& Shape1);

  //! load second shape into extrema
  Standard_EXPORT void LoadS2(const TopoDS_Shape& Shape1);

  //! computation of the minimum distance (value and
  //!          couple of points). Parameter theDeflection is used
  //!          to specify a maximum deviation of extreme distances
  //!          from the minimum one.
  //!          Returns IsDone status.
  //! theRange - the progress indicator of algorithm
  Standard_EXPORT bool
    Perform(const Message_ProgressRange& theRange = Message_ProgressRange());

  //! True if the minimum distance is found.
  bool IsDone() const { return myIsDone; }

  //! Returns the number of solutions satisfying the minimum distance.
  int NbSolution() const { return mySolutionsShape1.Length(); }

  //! Returns the value of the minimum distance.
  Standard_EXPORT double Value() const;

  //! True if one of the shapes is a solid and the other shape
  //! is completely or partially inside the solid.
  bool InnerSolution() const { return myInnerSol; }

  //! Returns the Point corresponding to the <N>th solution on the first Shape
  const gp_Pnt& PointOnShape1(const int N) const
  {
    return mySolutionsShape1.Value(N).Point();
  }

  //! Returns the Point corresponding to the <N>th solution on the second Shape
  const gp_Pnt& PointOnShape2(const int N) const
  {
    return mySolutionsShape2.Value(N).Point();
  }

  //! gives the type of the support where the Nth solution on the first shape is situated:
  //!   IsVertex => the Nth solution on the first shape is a Vertex
  //!   IsOnEdge => the Nth soluion on the first shape is on a Edge
  //!   IsInFace => the Nth solution on the first shape is inside a face
  //! the corresponding support is obtained by the method SupportOnShape1
  BRepExtrema_SupportType SupportTypeShape1(const int N) const
  {
    return mySolutionsShape1.Value(N).SupportKind();
  }

  //! gives the type of the support where the Nth solution on the second shape is situated:
  //!   IsVertex => the Nth solution on the second shape is a Vertex
  //!   IsOnEdge => the Nth soluion on the secondt shape is on a Edge
  //!   IsInFace => the Nth solution on the second shape is inside a face
  //! the corresponding support is obtained by the method SupportOnShape2
  BRepExtrema_SupportType SupportTypeShape2(const int N) const
  {
    return mySolutionsShape2.Value(N).SupportKind();
  }

  //! gives the support where the Nth solution on the first shape is situated.
  //! This support can be a Vertex, an Edge or a Face.
  Standard_EXPORT TopoDS_Shape SupportOnShape1(const int N) const;

  //! gives the support where the Nth solution on the second shape is situated.
  //! This support can be a Vertex, an Edge or a Face.
  Standard_EXPORT TopoDS_Shape SupportOnShape2(const int N) const;

  //! gives the corresponding parameter t if the Nth solution
  //! is situated on an Edge of the first shape
  Standard_EXPORT void ParOnEdgeS1(const int N, double& t) const;

  //! gives the corresponding parameter t if the Nth solution
  //! is situated on an Edge of the first shape
  Standard_EXPORT void ParOnEdgeS2(const int N, double& t) const;

  //! gives the corresponding parameters (U,V) if the Nth solution
  //! is situated on an face of the first shape
  Standard_EXPORT void ParOnFaceS1(const int N,
                                   double&         u,
                                   double&         v) const;

  //! gives the corresponding parameters (U,V) if the Nth solution
  //! is situated on an Face of the second shape
  Standard_EXPORT void ParOnFaceS2(const int N,
                                   double&         u,
                                   double&         v) const;

  //! Prints on the stream o information on the current state of the object.
  Standard_EXPORT void Dump(Standard_OStream& o) const;

  //! Sets unused parameter
  //! Obsolete
  void SetFlag(const Extrema_ExtFlag F) { myFlag = F; }

  //! Sets unused parameter
  //! Obsolete
  void SetAlgo(const Extrema_ExtAlgo A) { myAlgo = A; }

  //! If isMultiThread == true then computation will be performed in parallel.
  void SetMultiThread(bool theIsMultiThread) { myIsMultiThread = theIsMultiThread; }

  //! Returns true then computation will be performed in parallel
  //! Default value is false
  bool IsMultiThread() const { return myIsMultiThread; }

private:
  //! computes the minimum distance between two maps of shapes (Face,Edge,Vertex)
  bool DistanceMapMap(const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& Map1,
                                  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& Map2,
                                  const NCollection_Array1<Bnd_Box>&            LBox1,
                                  const NCollection_Array1<Bnd_Box>&            LBox2,
                                  const Message_ProgressRange&      theRange);

  //! computes the minimum distance between two maps of vertices
  bool DistanceVertVert(const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMap1,
                                    const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMap2,
                                    const Message_ProgressRange&      theRange);

  bool SolidTreatment(const TopoDS_Shape&               theShape,
                                  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMap,
                                  const Message_ProgressRange&      theRange);

private:
  double              myDistRef;
  bool           myIsDone;
  NCollection_Sequence<BRepExtrema_SolutionElem>  mySolutionsShape1;
  NCollection_Sequence<BRepExtrema_SolutionElem>  mySolutionsShape2;
  bool           myInnerSol;
  double              myEps;
  TopoDS_Shape               myShape1;
  TopoDS_Shape               myShape2;
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> myMapV1;
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> myMapV2;
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> myMapE1;
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> myMapE2;
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> myMapF1;
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> myMapF2;
  bool           myIsInitS1;
  bool           myIsInitS2;
  Extrema_ExtFlag            myFlag;
  Extrema_ExtAlgo            myAlgo;
  NCollection_Array1<Bnd_Box>            myBV1;
  NCollection_Array1<Bnd_Box>            myBV2;
  NCollection_Array1<Bnd_Box>            myBE1;
  NCollection_Array1<Bnd_Box>            myBE2;
  NCollection_Array1<Bnd_Box>            myBF1;
  NCollection_Array1<Bnd_Box>            myBF2;
  bool           myIsMultiThread;
};

#endif
