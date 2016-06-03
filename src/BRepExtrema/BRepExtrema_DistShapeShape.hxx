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

#include <Bnd_SeqOfBox.hxx>
#include <BRepExtrema_SeqOfSolution.hxx>
#include <BRepExtrema_SolutionElem.hxx>
#include <BRepExtrema_SupportType.hxx>
#include <Extrema_ExtAlgo.hxx>
#include <Extrema_ExtFlag.hxx>
#include <gp_Pnt.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <Standard_OStream.hxx>
#include <Standard_DefineAlloc.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

//! This class  provides tools to compute minimum distance <br>
//! between two Shapes (Compound,CompSolid, Solid, Shell, Face, Wire, Edge, Vertex). <br>
class BRepExtrema_DistShapeShape
{
 public:

  DEFINE_STANDARD_ALLOC

  //! create empty tool <br>
  Standard_EXPORT BRepExtrema_DistShapeShape();
  //! computation of the minimum distance (value and pair of points) using default deflection <br>
  //! Default value is Precision::Confusion(). <br>
  Standard_EXPORT BRepExtrema_DistShapeShape(const TopoDS_Shape& Shape1,const TopoDS_Shape& Shape2,const Extrema_ExtFlag F = Extrema_ExtFlag_MINMAX,const Extrema_ExtAlgo A = Extrema_ExtAlgo_Grad);
  //! create tool and load both shapes into it <br>
  Standard_EXPORT BRepExtrema_DistShapeShape(const TopoDS_Shape& Shape1,const TopoDS_Shape& Shape2,const Standard_Real theDeflection,const Extrema_ExtFlag F = Extrema_ExtFlag_MINMAX,const Extrema_ExtAlgo A = Extrema_ExtAlgo_Grad);
  
  void SetDeflection(const Standard_Real theDeflection)
  {
    myEps = theDeflection;
  }
  //! load first shape into extrema <br>
  Standard_EXPORT void LoadS1(const TopoDS_Shape& Shape1);
  //! load second shape into extrema <br>
  Standard_EXPORT void LoadS2(const TopoDS_Shape& Shape1);
  //! computation of  the minimum  distance  (value  and <br>
  //!          couple  of points). Parameter theDeflection is used <br>
  //!          to specify a maximum deviation of extreme distances <br>
  //!          from the minimum one. <br>
  //!          Returns IsDone status. <br>
  Standard_EXPORT Standard_Boolean Perform();
  //! True if the minimum distance is found. <br>
  Standard_Boolean IsDone() const
  { 
    return myIsDone;
  }
  //! Returns the number of solutions satisfying the minimum distance. <br>
  Standard_Integer NbSolution() const
  { 
    return mySolutionsShape1.Length();
  }
  //! Returns the value of the minimum distance. <br>
  Standard_EXPORT Standard_Real Value() const;
  //! True if one of the shapes is a solid and the other shape <br>
  //! is completely or partially inside the solid. <br>
  Standard_Boolean InnerSolution() const
  { 
    return myInnerSol;
  }
  //! Returns the Point corresponding to the <N>th solution on the first Shape <br>
  const gp_Pnt & PointOnShape1(const Standard_Integer N) const
  { 
    return mySolutionsShape1.Value(N).Point();
  }
  //! Returns the Point corresponding to the <N>th solution on the second Shape <br>
  const gp_Pnt & PointOnShape2(const Standard_Integer N) const
  { 
    return mySolutionsShape2.Value(N).Point();
  }
  //! gives the type of the support where the Nth solution on the first shape is situated: <br>
  //!   IsVertex => the Nth solution on the first shape is a Vertex <br>
  //!   IsOnEdge => the Nth soluion on the first shape is on a Edge <br>
  //!   IsInFace => the Nth solution on the first shape is inside a face <br>
  //! the corresponding support is obtained by the method SupportOnShape1 <br>
  BRepExtrema_SupportType SupportTypeShape1(const Standard_Integer N) const
  { 
    return mySolutionsShape1.Value(N).SupportKind();
  }
  //! gives the type of the support where the Nth solution on the second shape is situated: <br>
  //!   IsVertex => the Nth solution on the second shape is a Vertex <br>
  //!   IsOnEdge => the Nth soluion on the secondt shape is on a Edge <br>
  //!   IsInFace => the Nth solution on the second shape is inside a face <br>
  //! the corresponding support is obtained by the method SupportOnShape2 <br>
  BRepExtrema_SupportType SupportTypeShape2(const Standard_Integer N) const
  { 
    return mySolutionsShape2.Value(N).SupportKind();
  }
  //! gives the support where the Nth solution on the first shape is situated. <br>
  //! This support can be a Vertex, an Edge or a Face. <br>
  Standard_EXPORT TopoDS_Shape SupportOnShape1(const Standard_Integer N) const;
  //! gives the support where the Nth solution on the second shape is situated. <br>
  //! This support can be a Vertex, an Edge or a Face. <br>
  Standard_EXPORT TopoDS_Shape SupportOnShape2(const Standard_Integer N) const;
  //! gives the corresponding parameter t if the Nth solution <br>
  //! is situated on an Egde of the first shape <br>
  Standard_EXPORT void ParOnEdgeS1(const Standard_Integer N,Standard_Real& t) const;
  //! gives the corresponding parameter t if the Nth solution <br>
  //! is situated on an Egde of the first shape <br>
  Standard_EXPORT void ParOnEdgeS2(const Standard_Integer N,Standard_Real& t) const;
  //! gives the corresponding parameters (U,V) if the Nth solution <br>
  //! is situated on an face of the first shape <br>
  Standard_EXPORT void ParOnFaceS1(const Standard_Integer N,Standard_Real& u,Standard_Real& v) const;
  //! gives the corresponding parameters (U,V) if the Nth solution <br>
  //! is situated on an Face of the second shape <br>
  Standard_EXPORT void ParOnFaceS2(const Standard_Integer N,Standard_Real& u,Standard_Real& v) const;
  //! Prints on the stream o information on the current state of the object. <br>
  Standard_EXPORT void Dump(Standard_OStream& o) const;

  void SetFlag(const Extrema_ExtFlag F)
  {
    myFlag = F;
  }

  void SetAlgo(const Extrema_ExtAlgo A)
  {
    myAlgo = A;
  }

private:

  //! computes the minimum distance between two maps of shapes (Face,Edge,Vertex) <br>
  Standard_EXPORT void DistanceMapMap(const TopTools_IndexedMapOfShape& Map1,const TopTools_IndexedMapOfShape& Map2,const Bnd_SeqOfBox& LBox1,const Bnd_SeqOfBox& LBox2);

  Standard_Real myDistRef;
  Standard_Boolean myIsDone;
  BRepExtrema_SeqOfSolution mySolutionsShape1;
  BRepExtrema_SeqOfSolution mySolutionsShape2;
  Standard_Boolean myInnerSol;
  Standard_Real myEps;
  TopoDS_Shape myShape1;
  TopoDS_Shape myShape2;
  TopTools_IndexedMapOfShape myMapV1;
  TopTools_IndexedMapOfShape myMapV2;
  TopTools_IndexedMapOfShape myMapE1;
  TopTools_IndexedMapOfShape myMapE2;
  TopTools_IndexedMapOfShape myMapF1;
  TopTools_IndexedMapOfShape myMapF2;
  Standard_Boolean myIsInitS1;
  Standard_Boolean myIsInitS2;
  Extrema_ExtFlag myFlag;
  Extrema_ExtAlgo myAlgo;
  Bnd_SeqOfBox myBV1;
  Bnd_SeqOfBox myBV2;
  Bnd_SeqOfBox myBE1;
  Bnd_SeqOfBox myBE2;
  Bnd_SeqOfBox myBF1;
  Bnd_SeqOfBox myBF2;
};

#endif
