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

#ifndef _BRepExtrema_DistanceSS_HeaderFile
#define _BRepExtrema_DistanceSS_HeaderFile

#include <BRepExtrema_SeqOfSolution.hxx>
#include <Extrema_ExtFlag.hxx>
#include <Extrema_ExtAlgo.hxx>
#include <Precision.hxx>
#include <Standard_DefineAlloc.hxx>

class TopoDS_Shape;
class Bnd_Box;
class TopoDS_Vertex;
class TopoDS_Edge;
class TopoDS_Face;


//!  This class allows to compute minimum distance between two shapes <br>
//! (face edge vertex) and is used in DistShapeShape class. <br>
class BRepExtrema_DistanceSS
{
 public:

  DEFINE_STANDARD_ALLOC

  //! computes the distance between two Shapes ( face edge vertex). <br>
  BRepExtrema_DistanceSS(const TopoDS_Shape& S1, const TopoDS_Shape& S2,
                                         const Bnd_Box& B1, const Bnd_Box& B2,
                                         const Standard_Real DstRef,
                                         const Extrema_ExtFlag F = Extrema_ExtFlag_MINMAX,
                                         const Extrema_ExtAlgo A = Extrema_ExtAlgo_Grad)
  : myDstRef(DstRef), myModif(Standard_False), myEps(Precision::Confusion()), myFlag(F), myAlgo(A)
  {
    Perform(S1, S2, B1, B2);
  }
  //! computes the distance between two Shapes ( face edge vertex). <br>
  //! Parameter theDeflection is used to specify a maximum deviation <br>
  //! of extreme distances from the minimum one. <br>
  //! Default value is Precision::Confusion(). <br>
  BRepExtrema_DistanceSS(const TopoDS_Shape& S1, const TopoDS_Shape& S2,
                                         const Bnd_Box& B1, const Bnd_Box& B2,
                                         const Standard_Real DstRef, const Standard_Real aDeflection,
                                         const Extrema_ExtFlag F = Extrema_ExtFlag_MINMAX,
                                         const Extrema_ExtAlgo A = Extrema_ExtAlgo_Grad)
  : myDstRef(DstRef), myModif(Standard_False), myEps(aDeflection), myFlag(F), myAlgo(A)
  {
    Perform(S1, S2, B1, B2);
  }
  //! True if the distance has been computed <br>
  Standard_Boolean IsDone() const
  {
    return myModif;
  }
  //! returns the distance value <br>
  Standard_Real DistValue() const
  {
    return myDstRef;
  }
  //! returns the list of solutions on the first shape <br>
  const BRepExtrema_SeqOfSolution& Seq1Value() const
  {
    return SeqSolShape1;
  }
  //! returns the list of solutions on the second shape <br>
  const BRepExtrema_SeqOfSolution& Seq2Value() const
  {
    return SeqSolShape2;
  }
  //! sets the flag controlling minimum and maximum search
  void SetFlag(const Extrema_ExtFlag F)
  {
    myFlag = F;
  }
  //! sets the flag controlling ...
  void SetAlgo(const Extrema_ExtAlgo A)
  {
    myAlgo = A;
  }

 private:

  //! computes the distance between two Shapes ( face edge vertex) <br>
  Standard_EXPORT void Perform(const TopoDS_Shape& S1,const TopoDS_Shape& S2,const Bnd_Box& B1,const Bnd_Box& B2);

  //! computes the distance between two vertices <br>
  void Perform(const TopoDS_Vertex& S1,const TopoDS_Vertex& S2);
  //! computes the minimum distance between a vertex and an edge <br>
  void Perform(const TopoDS_Vertex& S1,const TopoDS_Edge& S2,const Bnd_Box& B1,const Bnd_Box& B2);
  //! computes the minimum distance between a vertex and a face <br>
  void Perform(const TopoDS_Vertex& S1,const TopoDS_Face& S2,const Bnd_Box& B1,const Bnd_Box& B2);

  //! computes the minimum distance between an edge and a vertex <br>
  void Perform(const TopoDS_Edge& S1,const TopoDS_Vertex& S2,const Bnd_Box& B1,const Bnd_Box& B2);
  /*{
    Perform(S2, S1, B2, B1);
  }*/
  //! computes the minimum distance between two edges <br>
  void Perform(const TopoDS_Edge& S1,const TopoDS_Edge& S2,const Bnd_Box& B1,const Bnd_Box& B2);
  //! computes the minimum distance an edge and a face <br>
  void Perform(const TopoDS_Edge& S1,const TopoDS_Face& S2,const Bnd_Box& B1,const Bnd_Box& B2);

  //! computes the minimum distance betwwen a face and a vertex <br>
  void Perform(const TopoDS_Face& S1,const TopoDS_Vertex& S2,const Bnd_Box& B1,const Bnd_Box& B2);
  /*{
    Perform(S2, S1, B2, B1);
  }*/
  //! computes the minimum distance between a face and an edge <br>
  void Perform(const TopoDS_Face& S1,const TopoDS_Edge& S2,const Bnd_Box& B1,const Bnd_Box& B2);
  /*{
    Perform(S2, S1, B2, B1);
  }*/
  //! computes the minimum distance between two faces <br>
  void Perform(const TopoDS_Face& S1,const TopoDS_Face& S2,const Bnd_Box& B1,const Bnd_Box& B2);

  BRepExtrema_SeqOfSolution SeqSolShape1;
  BRepExtrema_SeqOfSolution SeqSolShape2;
  Standard_Real myDstRef;
  Standard_Boolean myModif;
  Standard_Real myEps;
  Extrema_ExtFlag myFlag;
  Extrema_ExtAlgo myAlgo;
};

#endif
