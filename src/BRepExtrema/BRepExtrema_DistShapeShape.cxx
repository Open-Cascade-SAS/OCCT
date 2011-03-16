// File:	BRepExtrema_DistShapeShape.cxx
// Created:	Mon Apr 22 17:03:37 1996
// Modified :   Mps(10-04-97) portage WNT 
// Author:	Maria PUMBORIOS
// Author:      Herve LOUESSARD 
//		<mps@sgi64>

#include <BRepExtrema_DistShapeShape.ixx>
#include <Standard_OStream.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <BRepBndLib.hxx>
#include <Bnd_Box.hxx>
#include <TopExp.hxx>
#include <TCollection.hxx>
#include <Standard_Real.hxx>
#include <BRepExtrema_DistanceSS.hxx>
#include <TopoDS.hxx>
#include <TopAbs.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <Precision.hxx>
#include <BRepExtrema_SeqOfSolution.hxx>
#include <BRepExtrema_SolutionElem.hxx>
#include <Bnd_SeqOfBox.hxx>
#include <BRepExtrema_UnCompatibleShape.hxx>
#include <BRep_Tool.hxx>
#include <BRepClass3d_SolidClassifier.hxx>

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
   Standard_Integer i = 0;
   for(i = 1; i <= Map.Extent(); i++) {
     Bnd_Box box;
     BRepBndLib::Add( Map(i), box);
     SBox.Append(box);
   }
}

static Standard_Real DistanceInitiale(const TopoDS_Vertex V1,
                                      const TopoDS_Vertex V2)
{
  gp_Pnt P1,P2;
  P1= BRep_Tool::Pnt(V1);
  P2= BRep_Tool::Pnt(V2); 
  return(P1.Distance(P2));  
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
  Standard_Integer i = 0, j = 0;
  Bnd_Box box1, box2;
  TopoDS_Shape S1, S2;
  BRepExtrema_SeqOfSolution  seq1, seq2;

  Standard_Integer n1 = Map1.Extent();
  Standard_Integer n2 = Map2.Extent();
  for(i = 1; i <= n1; i++) {
    box1= LBox1.Value(i);
    S1= TopoDS_Shape (Map1(i)); 
    for(j = 1; j <= n2; j++) {
      box2= LBox2.Value(j);
      S2 = TopoDS_Shape (Map2(j));
      BRepExtrema_DistanceSS  dist(S1,S2,box1,box2,myDistRef,myEps);
      if(dist.IsDone())
        if(dist.DistValue() < (myDistRef-myEps)) {
          ListeDeSolutionShape1.Clear();
          ListeDeSolutionShape2.Clear();
          seq1= dist.Seq1Value();
          seq2= dist.Seq2Value();
          ListeDeSolutionShape1.Append(seq1);
          ListeDeSolutionShape2.Append(seq2);
          myDistRef=dist.DistValue();
        }
        else if(fabs (dist.DistValue()-myDistRef)< myEps ) {
          seq1= dist.Seq1Value();
          seq2= dist.Seq2Value();
          ListeDeSolutionShape1.Append(seq1);
          ListeDeSolutionShape2.Append(seq2);
          //  Modified by Sergey KHROMOV - Tue Mar  6 12:15:39 2001 Begin
          if (myDistRef > dist.DistValue())
            myDistRef=dist.DistValue();
          //  Modified by Sergey KHROMOV - Tue Mar  6 12:15:37 2001 End
        }
    }
  }
}

//=======================================================================
//function : BRepExtrema_DistShapeShape
//purpose  : 
//=======================================================================

BRepExtrema_DistShapeShape::BRepExtrema_DistShapeShape()
{
  myEps = Precision::Confusion();
}

//=======================================================================
//function : BRepExtrema_DistShapeShape
//purpose  : 
//=======================================================================
BRepExtrema_DistShapeShape::BRepExtrema_DistShapeShape(const TopoDS_Shape& Shape1,
						       const TopoDS_Shape& Shape2)
{
  myEps = Precision::Confusion();
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
						       const Standard_Real theDeflection)
{
  myEps = theDeflection;
  LoadS1(Shape1);
  LoadS2(Shape2);
  Perform();
}

//=======================================================================
//function : SetDeflection
//purpose  : 
//=======================================================================

void BRepExtrema_DistShapeShape::SetDeflection(const Standard_Real theDeflection)
{
  myEps = theDeflection;
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
  ListeDeSolutionShape1.Clear();
  ListeDeSolutionShape2.Clear();
  
  if( myShape1.IsNull() || myShape2.IsNull() )
    return Standard_False;

  Bnd_SeqOfBox BV1, BV2, BE1, BE2, BF1, BF2;
  Standard_Real tol = 0.001;
  gp_Pnt P;
  Standard_Integer nbv1,nbv2;

  // traitement des solides
  TopAbs_ShapeEnum Type1 = myShape1.ShapeType();
  TopAbs_ShapeEnum Type2 = myShape2.ShapeType();
  if((Type1==TopAbs_SOLID) || (Type1 == TopAbs_COMPSOLID)) {
    BRepClass3d_SolidClassifier Classi(myShape1);
    nbv2=myMapV2.Extent();
    nbv1=0;
    do {
      nbv1++;
      TopoDS_Vertex V2 = TopoDS::Vertex(myMapV2(nbv1));
      P=BRep_Tool::Pnt(V2);
      Classi.Perform(P,tol);
      if(Classi.State()==TopAbs_IN) {
        myInnerSol = Standard_True;
        myNbSolution = 1;
        myDistRef = 0;
        myIsDone = Standard_True; 
        BRepExtrema_SolutionElem Sol(0,P,BRepExtrema_IsVertex,V2);
        ListeDeSolutionShape1.Append(Sol);
        ListeDeSolutionShape2.Append(Sol);
      }  
    } while ( (nbv1<nbv2) && (! myInnerSol));
  }
  
  if(((Type2==TopAbs_SOLID)||(Type2==TopAbs_COMPSOLID))&&(!myInnerSol)) {
    BRepClass3d_SolidClassifier Classi(myShape2);
    nbv1= myMapV1.Extent();
    nbv2=0;
    do {
      nbv2++;
      TopoDS_Vertex V1=TopoDS::Vertex(myMapV1(nbv2));
      P=BRep_Tool::Pnt(V1);
      Classi.Perform(P,tol);
      if (Classi.State()==TopAbs_IN) {
        myInnerSol = Standard_True;
        myNbSolution = 1;
        myDistRef = 0;
        myIsDone = Standard_True; 
        BRepExtrema_SolutionElem Sol (0,P,BRepExtrema_IsVertex,V1);
        ListeDeSolutionShape1.Append(Sol);
        ListeDeSolutionShape2.Append(Sol);
      }
    } while ((nbv2<nbv1) && (! myInnerSol));
  }
  
  if (!myInnerSol) {  
    BoxCalculation( myMapV1,BV1);
    BoxCalculation( myMapE1,BE1);
    BoxCalculation( myMapF1,BF1);
    BoxCalculation( myMapV2,BV2);
    BoxCalculation( myMapE2,BE2);
    BoxCalculation( myMapF2,BF2);
    
    if (myMapV1.Extent()!=0 && myMapV2.Extent()!=0) {
      TopoDS_Vertex V1 = TopoDS::Vertex(myMapV1(1));
      TopoDS_Vertex V2 = TopoDS::Vertex(myMapV2(1));
      myDistRef = DistanceInitiale(V1, V2);  
    }
    else
      myDistRef= 1.e30;
    
    DistanceMapMap( myMapV1, myMapV2, BV1, BV2);
    DistanceMapMap( myMapV1, myMapE2, BV1, BE2);
    DistanceMapMap( myMapE1, myMapV2, BE1, BV2);
    DistanceMapMap( myMapV1, myMapF2, BV1, BF2);
    DistanceMapMap( myMapF1, myMapV2, BF1, BV2);
    DistanceMapMap( myMapE1, myMapE2, BE1, BE2);
    DistanceMapMap( myMapE1, myMapF2, BE1, BF2);
    DistanceMapMap( myMapF1, myMapE2, BF1, BE2);
    
    if( (fabs(myDistRef)) > myEps )
      DistanceMapMap(myMapF1,myMapF2,BF1,BF2);
    
    //  Modified by Sergey KHROMOV - Tue Mar  6 11:55:03 2001 Begin
    Standard_Integer i = 0;
    for(i = 1; i <= ListeDeSolutionShape1.Length(); i++)
      if (ListeDeSolutionShape1.Value(i).Dist() > myDistRef + myEps) {
        ListeDeSolutionShape1.Remove(i);
        ListeDeSolutionShape2.Remove(i);
      }
    //  Modified by Sergey KHROMOV - Tue Mar  6 11:55:04 2001 End
    myNbSolution = ListeDeSolutionShape1.Length();
    if( myNbSolution > 0 )
      myIsDone = Standard_True;
    else
      myIsDone = Standard_False;
  }
  return myIsDone;
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Boolean BRepExtrema_DistShapeShape:: InnerSolution () const 
{ 
  return (myInnerSol);
}

//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================

Standard_Boolean BRepExtrema_DistShapeShape::IsDone() const 
{ 
  return (myIsDone);
}

//=======================================================================
//function : NbSolution
//purpose  : 
//=======================================================================

Standard_Integer BRepExtrema_DistShapeShape::NbSolution() const 
{ 
  if (myIsDone == Standard_False) {
    StdFail_NotDone::Raise
      ("BRepExtrema_DistShapeShape::NbSolution: There's no solution ");
  }  
  return (myNbSolution);
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Standard_Real BRepExtrema_DistShapeShape::Value() const 
{ 
  if (myIsDone == Standard_False) {
    StdFail_NotDone::Raise
      ("BRepExtrema_DistShapeShape::Value: There's no solution ");
  }  
  return (myDistRef);
}

//=======================================================================
//function : PointOnShape1
//purpose  : 
//=======================================================================

gp_Pnt BRepExtrema_DistShapeShape::PointOnShape1(const Standard_Integer N) const 
{ 
  if (myIsDone == Standard_False)
     { StdFail_NotDone::Raise
       ("BRepExtrema_DistShapeShape::PointOnShape1: There's no solution ");
     } 
  if ((N<1)||(N>myNbSolution))
     { Standard_OutOfRange::Raise
       ("BRepExtrema_DistShapeShape::PointOnShape1:  Nth solution doesn't exist ");
     } 
  return ((ListeDeSolutionShape1.Value(N)).Point());
}

//=======================================================================
//function : PointOnShape2
//purpose  : 
//=======================================================================

gp_Pnt BRepExtrema_DistShapeShape::PointOnShape2(const Standard_Integer N) const 
{ 
  if (myIsDone == Standard_False)
     { StdFail_NotDone::Raise
       ("BRepExtrema_DistShapeShape::PointOnShape2: There's no solution ");
     } 
  if ((N<1)||(N>myNbSolution))
     { Standard_OutOfRange::Raise
       ("BRepExtrema_DistShapeShape::PointOnShape2:  Nth solution doesn't exist ");
     } 
  return ((ListeDeSolutionShape2.Value(N)).Point());
}

//=======================================================================
//function : SupportTypeShape1
//purpose  : 
//=======================================================================

BRepExtrema_SupportType BRepExtrema_DistShapeShape::SupportTypeShape1(const Standard_Integer N) const 
{ 
  if (myIsDone == Standard_False)
     { StdFail_NotDone::Raise
       ("BRepExtrema_DistShapeShape::SupportTypeShape1: There's no solution ");
     } 
  if ((N<1)||(N>myNbSolution))
     { Standard_OutOfRange::Raise
       ("BRepExtrema_DistShapeShape::SupportTypeShape1:  Nth solution doesn't exist ");
     } 
 return ((ListeDeSolutionShape1.Value(N)).SupportKind());
}

//=======================================================================
//function : SupportTypeShape2
//purpose  : 
//=======================================================================

BRepExtrema_SupportType BRepExtrema_DistShapeShape::SupportTypeShape2(const Standard_Integer N) const 
{ 
  if (myIsDone == Standard_False)
     { StdFail_NotDone::Raise
       ("BRepExtrema_DistShapeShape::SupportTypeShape2: There's no solution ");
     } 
  if ((N<1)||(N>myNbSolution))
     { Standard_OutOfRange::Raise
       ("BRepExtrema_DistShapeShape::SupportTypeShape2:  Nth solution doesn't exist ");
     } 
 return ((ListeDeSolutionShape2.Value(N)).SupportKind());
}

//=======================================================================
//function : SupportOnShape1
//purpose  : 
//=======================================================================

TopoDS_Shape BRepExtrema_DistShapeShape::SupportOnShape1(const Standard_Integer N) const 
{ 
  BRepExtrema_SupportType Type;
  TopoDS_Shape a_shape;
 
  if (myIsDone == Standard_False)
     { StdFail_NotDone::Raise
       ("BRepExtrema_DistShapeShape::SupportOnShape1: There's no solution ");
     } 
  if ((N<1)||(N>myNbSolution))
     { Standard_OutOfRange::Raise
       ("BRepExtrema_DistShapeShape::SupportOnShape1:  Nth solution doesn't exist ");
     } 
  Type = ((ListeDeSolutionShape1.Value(N)).SupportKind());  
  switch ( Type)
   { case BRepExtrema_IsVertex : a_shape=ListeDeSolutionShape1.Value(N).Vertex();
                                 break;
     case BRepExtrema_IsOnEdge : a_shape=ListeDeSolutionShape1.Value(N).Edge();
                                 break;       
     case BRepExtrema_IsInFace : a_shape=ListeDeSolutionShape1.Value(N).Face();
                                 break;	
     default :{} 
   }
  return a_shape ;
}

//=======================================================================
//function : SupportOnShape2
//purpose  : 
//=======================================================================

TopoDS_Shape BRepExtrema_DistShapeShape::SupportOnShape2(const Standard_Integer N) const 
{ 
  BRepExtrema_SupportType Type;   
  TopoDS_Shape a_shape ;
  
  if (myIsDone == Standard_False)
     { StdFail_NotDone::Raise
       ("BRepExtrema_DistShapeShape::SupportOnShape2: There's no solution ");
     } 
  if ((N<1)||(N>myNbSolution))
     { Standard_OutOfRange::Raise
       ("BRepExtrema_DistShapeShape::SupportOnShape2:  Nth solution doesn't exist ");
     } 
  Type = ((ListeDeSolutionShape2.Value(N)).SupportKind());  
  switch ( Type)
   { case BRepExtrema_IsVertex : a_shape=ListeDeSolutionShape2.Value(N).Vertex();
                                 break;
     case BRepExtrema_IsOnEdge : a_shape=ListeDeSolutionShape2.Value(N).Edge();
                                 break;       
     case BRepExtrema_IsInFace : a_shape=ListeDeSolutionShape2.Value(N).Face();
                                 break;	
     default :{} 
   } 
  return a_shape ;
}

//=======================================================================
//function : ParOnEdgeS1
//purpose  : 
//=======================================================================

void BRepExtrema_DistShapeShape::ParOnEdgeS1(const Standard_Integer N, Standard_Real& t) const 
{ 
  BRepExtrema_SupportType Type;
 
  if (myIsDone == Standard_False)
     { StdFail_NotDone::Raise
       ("BRepExtrema_DistShapeShape::ParOnEdgeS1: There's no solution ");
     } 
  if ((N<1)||(N>myNbSolution))
     { Standard_OutOfRange::Raise
       ("BRepExtrema_DistShapeShape::ParOnEdgeS1:  Nth solution doesn't exist ");
     } 

  Type = ((ListeDeSolutionShape1.Value(N)).SupportKind());  
  if (Type != BRepExtrema_IsOnEdge)
     { BRepExtrema_UnCompatibleShape::Raise
       ("BRepExtrema_DistShapeShape::ParOnEdgeS1:ParOnEdgeS1 is impossible without EDGE  ");
     }  

  (ListeDeSolutionShape1.Value(N)).EdgeParameter(t);
}

//=======================================================================
//function : ParOnEdgeS2
//purpose  : 
//=======================================================================

void BRepExtrema_DistShapeShape::ParOnEdgeS2(const Standard_Integer N,  Standard_Real& t) const 
{ 
  BRepExtrema_SupportType Type;

  if (myIsDone == Standard_False)
     { StdFail_NotDone::Raise
       ("BRepExtrema_DistShapeShape::ParOnEdgeS2: There's no solution ");
     } 
  if ((N<1)||(N>myNbSolution))
     { Standard_OutOfRange::Raise
       ("BRepExtrema_DistShapeShape::ParOnEdgeS2:  Nth solution doesn't exist ");
     } 

  Type = ((ListeDeSolutionShape2.Value(N)).SupportKind());  
  if (Type != BRepExtrema_IsOnEdge)
     { BRepExtrema_UnCompatibleShape::Raise
       ("BRepExtrema_DistShapeShape::ParOnEdgeS2:ParOnEdgeS2 is impossible without EDGE  ");
     }  
 
  (ListeDeSolutionShape2.Value(N)).EdgeParameter(t);
}

//=======================================================================
//function : ParOnFaceS1
//purpose  : 
//=======================================================================

void BRepExtrema_DistShapeShape::ParOnFaceS1(const Standard_Integer N,  Standard_Real& u,  Standard_Real& v) const 
{ 
  BRepExtrema_SupportType Type;

  if (myIsDone == Standard_False)
     { StdFail_NotDone::Raise
       ("BRepExtrema_DistShapeShape::ParOnFaceS1: There's no solution ");
     } 
  if ((N<1)||(N>myNbSolution))
     { Standard_OutOfRange::Raise
       ("BRepExtrema_DistShapeShape::ParOnFaceS1:  Nth solution doesn't exist ");
     } 

  Type = ((ListeDeSolutionShape1.Value(N)).SupportKind());  
  if (Type != BRepExtrema_IsInFace)
    { BRepExtrema_UnCompatibleShape::Raise
        ("BRepExtrema_DistShapeShape::ParOnFaceS1:ParOnFaceS1 is impossible without FACE ");
    }  
  
  (ListeDeSolutionShape1.Value(N)).FaceParameter(u, v); 
}

void BRepExtrema_DistShapeShape::ParOnFaceS2(const Standard_Integer N,  Standard_Real& u, Standard_Real& v) const 
{ 
  BRepExtrema_SupportType  Type;     

  if (myIsDone == Standard_False)
     { StdFail_NotDone::Raise
       ("BRepExtrema_DistShapeShape::ParOnFaceS2: There's no solution ");
     } 
  if ((N<1)||(N>myNbSolution))
     { Standard_OutOfRange::Raise
       ("BRepExtrema_DistShapeShape::ParOnFaceS2:  Nth solution doesn't exist ");
     } 
  Type = ((ListeDeSolutionShape2.Value(N)).SupportKind());  
  if (Type != BRepExtrema_IsInFace)
    { BRepExtrema_UnCompatibleShape::Raise
        ("BRepExtrema_DistShapeShape::ParOnFaceS2:ParOnFaceS2 is impossible without FACE ");
    }  
  
  (ListeDeSolutionShape2.Value(N)).FaceParameter(u, v); 

}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void BRepExtrema_DistShapeShape::Dump(Standard_OStream& o) const 
{
  Standard_Integer i;
  Standard_Real r1,r2;
  BRepExtrema_SupportType       Type1, Type2;
  
  o<< "the distance  value is :  " << Value()<<endl;
  o<< "the number of solutions is :"<<NbSolution()<<endl;
  o<<endl;
  for (i=1;i<=NbSolution();i++) { 
    o<<"solution number "<<i<<": "<< endl; 
    o<<"the type of the solution on the first shape is " <<Standard_Integer( SupportTypeShape1(i)) <<endl; 
    o<<"the type of the solution on the second shape is "<<Standard_Integer( SupportTypeShape2(i))<< endl;
    o<< "the coordinates of  the point on the first shape are: "<<endl; 
    o<<"X=" <<PointOnShape1(i).X()<<" Y=" <<PointOnShape1(i).Y()<<" Z="<<PointOnShape1(i).Z()<<endl;
    o<< "the coordinates of  the point on the second shape are: "<<endl; 
    o<<"X="<< PointOnShape2(i).X()<< " Y="<<PointOnShape2(i).Y()<<" Z="<< PointOnShape2(i).Z()<<endl;
    
     Type1=SupportTypeShape1(i);
     Type2=SupportTypeShape2(i);
     if (Type1 == BRepExtrema_IsOnEdge) 
       { 
	 ParOnEdgeS1(i,r1);
	 o << "parameter on the first edge :  t= " << r1 << endl;
	} 
     if (Type1 == BRepExtrema_IsInFace) 
	{
	 ParOnFaceS1(i,r1,r2);
	 o << "parameters on the first face :  u= " << r1 << " v=" <<  r2 << endl;
	}
      if (Type2 == BRepExtrema_IsOnEdge) 
	{
	 ParOnEdgeS2(i,r1);
	 o << "parameter on the second edge : t=" << r1 << endl;
       } 
    if (Type2 == BRepExtrema_IsInFace) 
	{
	 ParOnFaceS2(i,r1,r2);
	 o << "parameters on the second face : u= " << r1 << " v=" <<  r2 << endl;
        }
    
    o<<endl;
  } 
}
