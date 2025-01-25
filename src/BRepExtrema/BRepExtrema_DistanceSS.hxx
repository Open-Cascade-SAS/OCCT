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

//! This class allows to compute minimum distance between two brep shapes
//! (face edge vertex) and is used in DistShapeShape class.
class BRepExtrema_DistanceSS
{
public:
  DEFINE_STANDARD_ALLOC

public: //! @name Constructor from two shapes
  //! Computes the distance between two Shapes (face edge vertex).
  //! @param theS1 - First shape
  //! @param theS2 - Second shape
  //! @param theBox1 - Bounding box of first shape
  //! @param theBox2 - Bounding box of second shape
  //! @param theDstRef - Initial distance between the shapes to start with
  //! @param theDeflection - Maximum deviation of extreme distances from the minimum
  //!                        one (default is Precision::Confusion()).
  //! @param theExtFlag - Specifies which extrema solutions to look for
  //!                     (default is MINMAX, applied only to point-face extrema)
  //! @param theExtAlgo - Specifies which extrema algorithm is to be used
  //!                     (default is Grad algo, applied only to point-face extrema)
  BRepExtrema_DistanceSS(const TopoDS_Shape&   theS1,
                         const TopoDS_Shape&   theS2,
                         const Bnd_Box&        theBox1,
                         const Bnd_Box&        theBox2,
                         const Standard_Real   theDstRef,
                         const Standard_Real   theDeflection = Precision::Confusion(),
                         const Extrema_ExtFlag theExtFlag    = Extrema_ExtFlag_MINMAX,
                         const Extrema_ExtAlgo theExtAlgo    = Extrema_ExtAlgo_Grad)
      : myDstRef(theDstRef),
        myModif(Standard_False),
        myEps(theDeflection),
        myFlag(theExtFlag),
        myAlgo(theExtAlgo)
  {
    Perform(theS1, theS2, theBox1, theBox2);
  }

public: //! @name Results
  //! Returns true if the distance has been computed, false otherwise.
  Standard_Boolean IsDone() const { return myModif; }

  //! Returns the distance value.
  Standard_Real DistValue() const { return myDstRef; }

  //! Returns the list of solutions on the first shape.
  const BRepExtrema_SeqOfSolution& Seq1Value() const { return mySeqSolShape1; }

  //! Returns the list of solutions on the second shape.
  const BRepExtrema_SeqOfSolution& Seq2Value() const { return mySeqSolShape2; }

private: //! @name private methods performing the search
  //! Computes the distance between two Shapes (face edge vertex).
  //! General method to sort out the shape types and call the specific method.
  Standard_EXPORT void Perform(const TopoDS_Shape& theS1,
                               const TopoDS_Shape& theS2,
                               const Bnd_Box&      theBox1,
                               const Bnd_Box&      theBox2);

  //! Computes the distance between two vertices.
  void Perform(const TopoDS_Vertex&       S1,
               const TopoDS_Vertex&       S2,
               BRepExtrema_SeqOfSolution& theSeqSolShape1,
               BRepExtrema_SeqOfSolution& theSeqSolShape2);

  //! Computes the minimum distance between a vertex and an edge.
  void Perform(const TopoDS_Vertex&       theS1,
               const TopoDS_Edge&         theS2,
               BRepExtrema_SeqOfSolution& theSeqSolShape1,
               BRepExtrema_SeqOfSolution& theSeqSolShape2);

  //! Computes the minimum distance between a vertex and a face.
  void Perform(const TopoDS_Vertex&       theS1,
               const TopoDS_Face&         theS2,
               BRepExtrema_SeqOfSolution& theSeqSolShape1,
               BRepExtrema_SeqOfSolution& theSeqSolShape2);

  //! Computes the minimum distance between two edges.
  void Perform(const TopoDS_Edge&         theS1,
               const TopoDS_Edge&         theS2,
               BRepExtrema_SeqOfSolution& theSeqSolShape1,
               BRepExtrema_SeqOfSolution& theSeqSolShape2);

  //! Computes the minimum distance between an edge and a face.
  void Perform(const TopoDS_Edge&         theS1,
               const TopoDS_Face&         theS2,
               BRepExtrema_SeqOfSolution& theSeqSolShape1,
               BRepExtrema_SeqOfSolution& theSeqSolShape2);

  //! Computes the minimum distance between two faces.
  void Perform(const TopoDS_Face&         theS1,
               const TopoDS_Face&         theS2,
               BRepExtrema_SeqOfSolution& theSeqSolShape1,
               BRepExtrema_SeqOfSolution& theSeqSolShape2);

private:                                    //! @name Fields
  BRepExtrema_SeqOfSolution mySeqSolShape1; //!< Solutions on the first shape
  BRepExtrema_SeqOfSolution mySeqSolShape2; //!< Solutions on the second shape
  Standard_Real             myDstRef;       //!< The minimal distance found
  Standard_Boolean          myModif; //!< Flag indicating whether the solution was improved or not
  Standard_Real             myEps;   //!< Deflection
  Extrema_ExtFlag           myFlag;  //!< Extrema flag indicating what solutions to look for
  Extrema_ExtAlgo           myAlgo;  //!< Extrema algo to be used to look for solutions
};

#endif
