// Created on: 1996-04-22
// Created by: Herve LOUESSARD
// Copyright (c) 1996-1999 Matra Datavision
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

// Modified: Mps(10-04-97) portage WNT

#include <BRepExtrema_DistShapeShape.hxx>

#include <Standard_OStream.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <BRepBndLib.hxx>
#include <Bnd_Box.hxx>
#include <TopExp.hxx>
#include <BRepExtrema_DistanceSS.hxx>
#include <TopoDS.hxx>
#include <TopAbs.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <Precision.hxx>
#include <Bnd_SeqOfBox.hxx>
#include <BRepExtrema_UnCompatibleShape.hxx>
#include <BRep_Tool.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <NCollection_Comparator.hxx>
#include <NCollection_Vector.hxx>
#include <StdFail_NotDone.hxx>

#include <algorithm>
namespace
{

  static void Decomposition(const TopoDS_Shape&         S,
                            TopTools_IndexedMapOfShape& MapV,
                            TopTools_IndexedMapOfShape& MapE,
                            TopTools_IndexedMapOfShape& MapF)
  {
    MapV.Clear();
    MapE.Clear();
    MapF.Clear();
    TopExp::MapShapes(S,TopAbs_VERTEX,MapV);
    TopExp::MapShapes(S,TopAbs_EDGE,MapE);
    TopExp::MapShapes(S,TopAbs_FACE,MapF);
  }

  static void BoxCalculation(const TopTools_IndexedMapOfShape& Map,
                             Bnd_SeqOfBox&                     SBox)
  {
    for (Standard_Integer i = 1; i <= Map.Extent(); i++)
    {
      Bnd_Box box;
      BRepBndLib::Add(Map(i), box);
      SBox.Append(box);
    }
  }

  inline Standard_Real DistanceInitiale(const TopoDS_Vertex V1,
                                        const TopoDS_Vertex V2)
  {
    return (BRep_Tool::Pnt(V1).Distance(BRep_Tool::Pnt(V2)));
  }

  //! Pair of objects to check extrema.
  struct BRepExtrema_CheckPair
  {
    Standard_Integer Index1;   //!< Index of the 1st sub-shape
    Standard_Integer Index2;   //!< Index of the 2nd sub-shape
    Standard_Real    Distance; //!< Distance between sub-shapes

    //! Uninitialized constructor for collection.
    BRepExtrema_CheckPair() {}

    //! Creates new pair of sub-shapes.
    BRepExtrema_CheckPair (Standard_Integer theIndex1,
                           Standard_Integer theIndex2,
                           Standard_Real    theDistance)
    : Index1   (theIndex1),
      Index2   (theIndex2),
      Distance (theDistance) {}
  };

  // Used by std::sort function
  static Standard_Boolean BRepExtrema_CheckPair_Comparator (const BRepExtrema_CheckPair& theLeft,
                                                            const BRepExtrema_CheckPair& theRight)
  {
    return (theLeft.Distance < theRight.Distance);
  }
}

//=======================================================================
//function : DistanceMapMap
//purpose  : 
//=======================================================================

void BRepExtrema_DistShapeShape::DistanceMapMap (const TopTools_IndexedMapOfShape& theMap1,
                                                 const TopTools_IndexedMapOfShape& theMap2,
                                                 const Bnd_SeqOfBox&               theLBox1,
                                                 const Bnd_SeqOfBox&               theLBox2)
{
  NCollection_Vector<BRepExtrema_CheckPair> aPairList;
  const Standard_Integer aCount1 = theMap1.Extent();
  const Standard_Integer aCount2 = theMap2.Extent();
  for (Standard_Integer anIdx1 = 1; anIdx1 <= aCount1; ++anIdx1)
  {
    for (Standard_Integer anIdx2 = 1; anIdx2 <= aCount2; ++anIdx2)
    {
      const Bnd_Box& aBox1 = theLBox1.Value (anIdx1);
      const Bnd_Box& aBox2 = theLBox2.Value (anIdx2);
      if (aBox1.IsVoid()
       || aBox2.IsVoid())
      {
        continue;
      }

      const Standard_Real aDist = aBox1.Distance (aBox2);
      if (aDist < myDistRef - myEps || fabs (aDist - myDistRef) < myEps)
      {
        aPairList.Append (BRepExtrema_CheckPair (anIdx1, anIdx2, aDist));
      }
    }
  }

  std::stable_sort(aPairList.begin(), aPairList.end(), BRepExtrema_CheckPair_Comparator);

  for (NCollection_Vector<BRepExtrema_CheckPair>::Iterator aPairIter (aPairList);
       aPairIter.More(); aPairIter.Next())
  {
    const BRepExtrema_CheckPair& aPair = aPairIter.Value();
    if (aPair.Distance > myDistRef + myEps)
    {
      break; // early search termination
    }

    const Bnd_Box& aBox1 = theLBox1.Value (aPair.Index1);
    const Bnd_Box& aBox2 = theLBox2.Value (aPair.Index2);

    const TopoDS_Shape& aShape1 = theMap1 (aPair.Index1);
    const TopoDS_Shape& aShape2 = theMap2 (aPair.Index2);

    BRepExtrema_DistanceSS aDistTool (aShape1, aShape2, aBox1, aBox2, myDistRef, myEps);
    if (aDistTool.IsDone())
    {
      if (aDistTool.DistValue() < myDistRef - myEps)
      {
        mySolutionsShape1.Clear();
        mySolutionsShape2.Clear();

        BRepExtrema_SeqOfSolution aSeq1 = aDistTool.Seq1Value();
        BRepExtrema_SeqOfSolution aSeq2 = aDistTool.Seq2Value();

        mySolutionsShape1.Append (aSeq1);
        mySolutionsShape2.Append (aSeq2);

        myDistRef = aDistTool.DistValue();
      }
      else if (fabs (aDistTool.DistValue() - myDistRef) < myEps)
      {
        BRepExtrema_SeqOfSolution aSeq1 = aDistTool.Seq1Value();
        BRepExtrema_SeqOfSolution aSeq2 = aDistTool.Seq2Value();

        mySolutionsShape1.Append (aSeq1);
        mySolutionsShape2.Append (aSeq2);

        if (myDistRef > aDistTool.DistValue())
        {
          myDistRef = aDistTool.DistValue();
        }
      }
    }
  }
}

//=======================================================================
//function : BRepExtrema_DistShapeShape
//purpose  : 
//=======================================================================

BRepExtrema_DistShapeShape::BRepExtrema_DistShapeShape()
: myDistRef (0.0),
  myIsDone (Standard_False),
  myInnerSol (Standard_False),
  myEps (Precision::Confusion()),
  myIsInitS1 (Standard_False),
  myIsInitS2 (Standard_False),
  myFlag (Extrema_ExtFlag_MINMAX),
  myAlgo (Extrema_ExtAlgo_Grad)
{
  //
}

//=======================================================================
//function : BRepExtrema_DistShapeShape
//purpose  : 
//=======================================================================
BRepExtrema_DistShapeShape::BRepExtrema_DistShapeShape(const TopoDS_Shape& Shape1,
                                                       const TopoDS_Shape& Shape2,
                                                       const Extrema_ExtFlag F,
                                                       const Extrema_ExtAlgo A)
: myDistRef (0.0),
  myIsDone (Standard_False),
  myInnerSol (Standard_False),
  myEps (Precision::Confusion()),
  myIsInitS1 (Standard_False),
  myIsInitS2 (Standard_False),
  myFlag (F),
  myAlgo (A)
{
  LoadS1(Shape1);
  LoadS2(Shape2);
  Perform();
}

//=======================================================================
//function : BRepExtrema_DistShapeShape
//purpose  : 
//=======================================================================

BRepExtrema_DistShapeShape::BRepExtrema_DistShapeShape(const TopoDS_Shape& Shape1,
                                                       const TopoDS_Shape& Shape2,
                                                       const Standard_Real theDeflection,
                                                       const Extrema_ExtFlag F,
                                                       const Extrema_ExtAlgo A)
: myDistRef (0.0),
  myIsDone (Standard_False),
  myInnerSol (Standard_False),
  myEps (theDeflection),
  myIsInitS1 (Standard_False),
  myIsInitS2 (Standard_False),
  myFlag (F),
  myAlgo (A)
{
  LoadS1(Shape1);
  LoadS2(Shape2);
  Perform();
}

//=======================================================================
//function : LoadS1
//purpose  : 
//=======================================================================

void BRepExtrema_DistShapeShape::LoadS1 (const TopoDS_Shape& Shape1)
{
  myShape1 = Shape1;
  myIsInitS1 = Standard_False;
  Decomposition (Shape1, myMapV1, myMapE1, myMapF1);
}

//=======================================================================
//function : LoadS2
//purpose  : 
//=======================================================================

void BRepExtrema_DistShapeShape::LoadS2 (const TopoDS_Shape& Shape2)
{
  myShape2 = Shape2;
  myIsInitS2 = Standard_False;
  Decomposition (Shape2, myMapV2, myMapE2, myMapF2);
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

Standard_Boolean BRepExtrema_DistShapeShape::Perform()
{
  myIsDone=Standard_False;
  myInnerSol=Standard_False;
  mySolutionsShape1.Clear();
  mySolutionsShape2.Clear();

  if ( myShape1.IsNull() || myShape2.IsNull() )
    return Standard_False;

  TopoDS_Vertex V;
  const Standard_Real tol = 0.001;

  // Treatment of solids
  const TopAbs_ShapeEnum Type1 = myShape1.ShapeType();
  if ((Type1==TopAbs_SOLID) || (Type1 == TopAbs_COMPSOLID))
  {
    BRepClass3d_SolidClassifier Classi(myShape1);
    const Standard_Integer nbv2 = myMapV2.Extent();
    Standard_Integer nbv1 = 0;
    while ( (nbv1<nbv2) && (!myInnerSol) )
    {
      nbv1++;
      V = TopoDS::Vertex(myMapV2(nbv1));
      const gp_Pnt &P = BRep_Tool::Pnt(V);
      Classi.Perform(P,tol);
      if (Classi.State()==TopAbs_IN)
      {
        myInnerSol = Standard_True;
        myDistRef = 0.;
        myIsDone = Standard_True; 
        BRepExtrema_SolutionElem Sol(0,P,BRepExtrema_IsVertex,V);
        mySolutionsShape1.Append(Sol);
        mySolutionsShape2.Append(Sol);
      }  
    }
  }
  
  const TopAbs_ShapeEnum Type2 = myShape2.ShapeType();
  if (((Type2==TopAbs_SOLID) || (Type2==TopAbs_COMPSOLID)) && (!myInnerSol))
  {
    BRepClass3d_SolidClassifier Classi(myShape2);
    const Standard_Integer nbv1 = myMapV1.Extent();
    Standard_Integer nbv2 = 0;
    while ( (nbv2<nbv1) && (!myInnerSol) )
    {
      nbv2++;
      V = TopoDS::Vertex(myMapV1(nbv2));
      const gp_Pnt &P = BRep_Tool::Pnt(V);
      Classi.Perform(P,tol);
      if (Classi.State()==TopAbs_IN) {
        myInnerSol = Standard_True;
        myDistRef = 0;
        myIsDone = Standard_True; 
        BRepExtrema_SolutionElem Sol (0,P,BRepExtrema_IsVertex,V);
        mySolutionsShape1.Append(Sol);
        mySolutionsShape2.Append(Sol);
      }
    }
  }

  if (!myInnerSol)
  {
    if (!myIsInitS1) // rebuild cached data for 1st shape
    {
      myBV1.Clear();
      myBE1.Clear();
      myBF1.Clear();

      BoxCalculation (myMapV1, myBV1);
      BoxCalculation (myMapE1, myBE1);
      BoxCalculation (myMapF1, myBF1);

      myIsInitS1 = Standard_True;
    }

    if (!myIsInitS2) // rebuild cached data for 2nd shape
    {
      myBV2.Clear();
      myBE2.Clear();
      myBF2.Clear();

      BoxCalculation (myMapV2, myBV2);
      BoxCalculation (myMapE2, myBE2);
      BoxCalculation (myMapF2, myBF2);

      myIsInitS2 = Standard_True;
    }

    if (myMapV1.Extent() && myMapV2.Extent())
    {
      TopoDS_Vertex V1 = TopoDS::Vertex(myMapV1(1));
      TopoDS_Vertex V2 = TopoDS::Vertex(myMapV2(1));
      myDistRef = DistanceInitiale(V1, V2);
    }
    else
      myDistRef= 1.e30; //szv:!!!

    DistanceMapMap (myMapV1, myMapV2, myBV1, myBV2);
    DistanceMapMap (myMapV1, myMapE2, myBV1, myBE2);
    DistanceMapMap (myMapE1, myMapV2, myBE1, myBV2);
    DistanceMapMap (myMapV1, myMapF2, myBV1, myBF2);
    DistanceMapMap (myMapF1, myMapV2, myBF1, myBV2);
    DistanceMapMap (myMapE1, myMapE2, myBE1, myBE2);
    DistanceMapMap (myMapE1, myMapF2, myBE1, myBF2);
    DistanceMapMap (myMapF1, myMapE2, myBF1, myBE2);

    if (fabs (myDistRef) > myEps)
    {
      DistanceMapMap (myMapF1, myMapF2, myBF1, myBF2);
    }
    
    //  Modified by Sergey KHROMOV - Tue Mar  6 11:55:03 2001 Begin
    Standard_Integer i = 1;
    for (; i <= mySolutionsShape1.Length(); i++)
      if (mySolutionsShape1.Value(i).Dist() > myDistRef + myEps)
      {
        mySolutionsShape1.Remove(i);
        mySolutionsShape2.Remove(i);
      }
    //  Modified by Sergey KHROMOV - Tue Mar  6 11:55:04 2001 End
    myIsDone = ( mySolutionsShape1.Length() > 0 );
  }
  return myIsDone;
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Standard_Real BRepExtrema_DistShapeShape::Value() const 
{ 
  if (!myIsDone)
    throw StdFail_NotDone("BRepExtrema_DistShapeShape::Value: There's no solution ");

  return myDistRef;
}

//=======================================================================
//function : SupportOnShape1
//purpose  : 
//=======================================================================

TopoDS_Shape BRepExtrema_DistShapeShape::SupportOnShape1(const Standard_Integer N) const
{ 
  if (!myIsDone)
    throw StdFail_NotDone("BRepExtrema_DistShapeShape::SupportOnShape1: There's no solution ");

  const BRepExtrema_SolutionElem &sol = mySolutionsShape1.Value(N);
  switch (sol.SupportKind())
  {
    case BRepExtrema_IsVertex : return sol.Vertex();
    case BRepExtrema_IsOnEdge : return sol.Edge();
    case BRepExtrema_IsInFace : return sol.Face();
  }
  return TopoDS_Shape();
}

//=======================================================================
//function : SupportOnShape2
//purpose  : 
//=======================================================================

TopoDS_Shape BRepExtrema_DistShapeShape::SupportOnShape2(const Standard_Integer N) const 
{ 
  if (!myIsDone)
    throw StdFail_NotDone("BRepExtrema_DistShapeShape::SupportOnShape2: There's no solution ");

  const BRepExtrema_SolutionElem &sol = mySolutionsShape2.Value(N);
  switch (sol.SupportKind())
  {
    case BRepExtrema_IsVertex : return sol.Vertex();
    case BRepExtrema_IsOnEdge : return sol.Edge();
    case BRepExtrema_IsInFace : return sol.Face();
  }
  return TopoDS_Shape();
}

//=======================================================================
//function : ParOnEdgeS1
//purpose  : 
//=======================================================================

void BRepExtrema_DistShapeShape::ParOnEdgeS1(const Standard_Integer N, Standard_Real& t) const 
{ 
  if (!myIsDone)
    throw StdFail_NotDone("BRepExtrema_DistShapeShape::ParOnEdgeS1: There's no solution");

  const BRepExtrema_SolutionElem &sol = mySolutionsShape1.Value(N);
  if (sol.SupportKind() != BRepExtrema_IsOnEdge)
    throw BRepExtrema_UnCompatibleShape("BRepExtrema_DistShapeShape::ParOnEdgeS1: ParOnEdgeS1 is impossible without EDGE");

  sol.EdgeParameter(t);
}

//=======================================================================
//function : ParOnEdgeS2
//purpose  : 
//=======================================================================

void BRepExtrema_DistShapeShape::ParOnEdgeS2(const Standard_Integer N,  Standard_Real& t) const 
{ 
  if (!myIsDone)
    throw StdFail_NotDone("BRepExtrema_DistShapeShape::ParOnEdgeS2: There's no solution");

  const BRepExtrema_SolutionElem &sol = mySolutionsShape2.Value(N);
  if (sol.SupportKind() != BRepExtrema_IsOnEdge)
    throw BRepExtrema_UnCompatibleShape("BRepExtrema_DistShapeShape::ParOnEdgeS2: ParOnEdgeS2 is impossible without EDGE");
 
  sol.EdgeParameter(t);
}

//=======================================================================
//function : ParOnFaceS1
//purpose  : 
//=======================================================================

void BRepExtrema_DistShapeShape::ParOnFaceS1(const Standard_Integer N,  Standard_Real& u,  Standard_Real& v) const 
{ 
  if (!myIsDone)
    throw StdFail_NotDone("BRepExtrema_DistShapeShape::ParOnFaceS1: There's no solution");

  const BRepExtrema_SolutionElem &sol = mySolutionsShape1.Value(N);
  if (sol.SupportKind() != BRepExtrema_IsInFace)
    throw BRepExtrema_UnCompatibleShape("BRepExtrema_DistShapeShape::ParOnFaceS1: ParOnFaceS1 is impossible without FACE");
  
  sol.FaceParameter(u, v);
}

//=======================================================================
//function : ParOnFaceS2
//purpose  : 
//=======================================================================

void BRepExtrema_DistShapeShape::ParOnFaceS2(const Standard_Integer N,  Standard_Real& u, Standard_Real& v) const 
{ 
  if (!myIsDone)
    throw StdFail_NotDone("BRepExtrema_DistShapeShape::ParOnFaceS2: There's no solution");

  const BRepExtrema_SolutionElem &sol = mySolutionsShape2.Value(N);
  if (sol.SupportKind() != BRepExtrema_IsInFace)
    throw BRepExtrema_UnCompatibleShape("BRepExtrema_DistShapeShape::ParOnFaceS2:ParOnFaceS2 is impossible without FACE ");
  
  sol.FaceParameter(u, v);
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void BRepExtrema_DistShapeShape::Dump(Standard_OStream& o) const 
{
  Standard_Integer i;
  Standard_Real r1,r2;
  
  o<< "the distance  value is :  " << Value()<<endl;
  o<< "the number of solutions is :"<<NbSolution()<<endl;
  o<<endl;
  for (i=1;i<=NbSolution();i++)
  {
    o<<"solution number "<<i<<": "<< endl;
    o<<"the type of the solution on the first shape is " <<Standard_Integer( SupportTypeShape1(i)) <<endl; 
    o<<"the type of the solution on the second shape is "<<Standard_Integer( SupportTypeShape2(i))<< endl;
    o<< "the coordinates of  the point on the first shape are: "<<endl; 
    o<<"X=" <<PointOnShape1(i).X()<<" Y=" <<PointOnShape1(i).Y()<<" Z="<<PointOnShape1(i).Z()<<endl;
    o<< "the coordinates of  the point on the second shape are: "<<endl; 
    o<<"X="<< PointOnShape2(i).X()<< " Y="<<PointOnShape2(i).Y()<<" Z="<< PointOnShape2(i).Z()<<endl;
    
    switch (SupportTypeShape1(i))
    {
      case BRepExtrema_IsVertex:
        break;
      case BRepExtrema_IsOnEdge:
        ParOnEdgeS1(i,r1);
        o << "parameter on the first edge :  t= " << r1 << endl;
        break;
      case BRepExtrema_IsInFace:
        ParOnFaceS1(i,r1,r2);
        o << "parameters on the first face :  u= " << r1 << " v=" <<  r2 << endl;
        break;
    }
    switch (SupportTypeShape2(i))
    {
      case BRepExtrema_IsVertex:
        break;
      case BRepExtrema_IsOnEdge:
        ParOnEdgeS2(i,r1);
        o << "parameter on the second edge : t=" << r1 << endl;
        break;
      case BRepExtrema_IsInFace:
        ParOnFaceS2(i,r1,r2);
        o << "parameters on the second face : u= " << r1 << " v=" <<  r2 << endl;
        break;
    }
    o<<endl;
  } 
}
