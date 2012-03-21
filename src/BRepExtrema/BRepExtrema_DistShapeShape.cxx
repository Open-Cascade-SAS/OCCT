// Created on: 1996-04-22
// Created by: Herve LOUESSARD
// Copyright (c) 1996-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

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
#include <StdFail_NotDone.hxx>

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

//=======================================================================
//function : DistanceMapMap
//purpose  : 
//=======================================================================

void BRepExtrema_DistShapeShape::DistanceMapMap(const TopTools_IndexedMapOfShape& Map1,
                                                const TopTools_IndexedMapOfShape& Map2,
                                                const Bnd_SeqOfBox&               LBox1,
                                                const Bnd_SeqOfBox&               LBox2)
{
  Standard_Integer i, j;
  BRepExtrema_SeqOfSolution seq1, seq2;

  const Standard_Integer n1 = Map1.Extent();
  const Standard_Integer n2 = Map2.Extent();
  for (i = 1; i <= n1; i++)
  {
    const Bnd_Box &box1 = LBox1.Value(i);
    const TopoDS_Shape &S1 = Map1(i);
    for (j = 1; j <= n2; j++)
    {
      const Bnd_Box &box2= LBox2.Value(j);
      const TopoDS_Shape &S2 = Map2(j);

      BRepExtrema_DistanceSS  dist(S1,S2,box1,box2,myDistRef,myEps);
      if (dist.IsDone())
        if(dist.DistValue() < (myDistRef-myEps))
        {
          mySolutionsShape1.Clear();
          mySolutionsShape2.Clear();
          seq1= dist.Seq1Value();
          seq2= dist.Seq2Value();
          mySolutionsShape1.Append(seq1);
          mySolutionsShape2.Append(seq2);
          myDistRef=dist.DistValue();
        }
        else if(fabs(dist.DistValue()-myDistRef) < myEps)
        {
          seq1= dist.Seq1Value();
          seq2= dist.Seq2Value();
          mySolutionsShape1.Append(seq1);
          mySolutionsShape2.Append(seq2);
          if (myDistRef > dist.DistValue())
            myDistRef=dist.DistValue();
        }
    }
  }
}

//=======================================================================
//function : BRepExtrema_DistShapeShape
//purpose  : 
//=======================================================================

BRepExtrema_DistShapeShape::BRepExtrema_DistShapeShape()
: myDistRef(0.),
  myDistValue(0.),
  myIsDone(Standard_False),
  myInnerSol(Standard_False),
  myEps(Precision::Confusion()),
  myFlag(Extrema_ExtFlag_MINMAX),
  myAlgo(Extrema_ExtAlgo_Grad)
{
}

//=======================================================================
//function : BRepExtrema_DistShapeShape
//purpose  : 
//=======================================================================
BRepExtrema_DistShapeShape::BRepExtrema_DistShapeShape(const TopoDS_Shape& Shape1,
                                                       const TopoDS_Shape& Shape2,
                                                       const Extrema_ExtFlag F,
                                                       const Extrema_ExtAlgo A)
: myDistRef(0.),
  myDistValue(0.),
  myIsDone(Standard_False),
  myInnerSol(Standard_False),
  myEps(Precision::Confusion()),
  myFlag(F),
  myAlgo(A)
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
: myDistRef(0.),
  myDistValue(0.),
  myIsDone(Standard_False),
  myInnerSol(Standard_False),
  myEps(theDeflection),
  myFlag(F),
  myAlgo(A)
{
  LoadS1(Shape1);
  LoadS2(Shape2);
  Perform();
}

//=======================================================================
//function : LoadS1
//purpose  : 
//=======================================================================

void BRepExtrema_DistShapeShape::LoadS1(const TopoDS_Shape& Shape1)
{
  myShape1 = Shape1;
  Decomposition(Shape1, myMapV1, myMapE1, myMapF1);
}

//=======================================================================
//function : LoadS2
//purpose  : 
//=======================================================================

void BRepExtrema_DistShapeShape::LoadS2(const TopoDS_Shape& Shape2)
{
  myShape2 = Shape2;
  Decomposition(Shape2, myMapV2, myMapE2, myMapF2);
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
  Bnd_SeqOfBox BV1, BV2, BE1, BE2, BF1, BF2;
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
    BoxCalculation(myMapV1,BV1);
    BoxCalculation(myMapE1,BE1);
    BoxCalculation(myMapF1,BF1);
    BoxCalculation(myMapV2,BV2);
    BoxCalculation(myMapE2,BE2);
    BoxCalculation(myMapF2,BF2);
    
    if (myMapV1.Extent() && myMapV2.Extent())
    {
      TopoDS_Vertex V1 = TopoDS::Vertex(myMapV1(1));
      TopoDS_Vertex V2 = TopoDS::Vertex(myMapV2(1));
      myDistRef = DistanceInitiale(V1, V2);
    }
    else
      myDistRef= 1.e30; //szv:!!!

    DistanceMapMap(myMapV1, myMapV2, BV1, BV2);
    DistanceMapMap(myMapV1, myMapE2, BV1, BE2);
    DistanceMapMap(myMapE1, myMapV2, BE1, BV2);
    DistanceMapMap(myMapV1, myMapF2, BV1, BF2);
    DistanceMapMap(myMapF1, myMapV2, BF1, BV2);
    DistanceMapMap(myMapE1, myMapE2, BE1, BE2);
    DistanceMapMap(myMapE1, myMapF2, BE1, BF2);
    DistanceMapMap(myMapF1, myMapE2, BF1, BE2);

    if( (fabs(myDistRef)) > myEps )
      DistanceMapMap(myMapF1,myMapF2,BF1,BF2);
    
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
    StdFail_NotDone::Raise("BRepExtrema_DistShapeShape::Value: There's no solution ");

  return myDistRef;
}

//=======================================================================
//function : SupportOnShape1
//purpose  : 
//=======================================================================

TopoDS_Shape BRepExtrema_DistShapeShape::SupportOnShape1(const Standard_Integer N) const
{ 
  if (!myIsDone)
    StdFail_NotDone::Raise("BRepExtrema_DistShapeShape::SupportOnShape1: There's no solution ");

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
    StdFail_NotDone::Raise("BRepExtrema_DistShapeShape::SupportOnShape2: There's no solution ");

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
    StdFail_NotDone::Raise("BRepExtrema_DistShapeShape::ParOnEdgeS1: There's no solution");

  const BRepExtrema_SolutionElem &sol = mySolutionsShape1.Value(N);
  if (sol.SupportKind() != BRepExtrema_IsOnEdge)
    BRepExtrema_UnCompatibleShape::Raise
      ("BRepExtrema_DistShapeShape::ParOnEdgeS1: ParOnEdgeS1 is impossible without EDGE");

  sol.EdgeParameter(t);
}

//=======================================================================
//function : ParOnEdgeS2
//purpose  : 
//=======================================================================

void BRepExtrema_DistShapeShape::ParOnEdgeS2(const Standard_Integer N,  Standard_Real& t) const 
{ 
  if (!myIsDone)
    StdFail_NotDone::Raise("BRepExtrema_DistShapeShape::ParOnEdgeS2: There's no solution");

  const BRepExtrema_SolutionElem &sol = mySolutionsShape2.Value(N);
  if (sol.SupportKind() != BRepExtrema_IsOnEdge)
    BRepExtrema_UnCompatibleShape::Raise
      ("BRepExtrema_DistShapeShape::ParOnEdgeS2: ParOnEdgeS2 is impossible without EDGE");
 
  sol.EdgeParameter(t);
}

//=======================================================================
//function : ParOnFaceS1
//purpose  : 
//=======================================================================

void BRepExtrema_DistShapeShape::ParOnFaceS1(const Standard_Integer N,  Standard_Real& u,  Standard_Real& v) const 
{ 
  if (!myIsDone)
    StdFail_NotDone::Raise("BRepExtrema_DistShapeShape::ParOnFaceS1: There's no solution");

  const BRepExtrema_SolutionElem &sol = mySolutionsShape1.Value(N);
  if (sol.SupportKind() != BRepExtrema_IsInFace)
    BRepExtrema_UnCompatibleShape::Raise
      ("BRepExtrema_DistShapeShape::ParOnFaceS1: ParOnFaceS1 is impossible without FACE");
  
  sol.FaceParameter(u, v);
}

//=======================================================================
//function : ParOnFaceS2
//purpose  : 
//=======================================================================

void BRepExtrema_DistShapeShape::ParOnFaceS2(const Standard_Integer N,  Standard_Real& u, Standard_Real& v) const 
{ 
  if (!myIsDone)
    StdFail_NotDone::Raise("BRepExtrema_DistShapeShape::ParOnFaceS2: There's no solution");

  const BRepExtrema_SolutionElem &sol = mySolutionsShape2.Value(N);
  if (sol.SupportKind() != BRepExtrema_IsInFace)
    BRepExtrema_UnCompatibleShape::Raise
      ("BRepExtrema_DistShapeShape::ParOnFaceS2:ParOnFaceS2 is impossible without FACE ");
  
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
