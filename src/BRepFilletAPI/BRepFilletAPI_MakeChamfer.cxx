// Created on: 1995-06-22
// Created by: Flore Lantheaume
// Copyright (c) 1995-1999 Matra Datavision
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



#include <BRepFilletAPI_MakeChamfer.ixx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <ChFiDS_Spine.hxx>
#include <TopExp_Explorer.hxx>



//=======================================================================
//function : BRepFilletAPI_MakeChamfer
//purpose  : 
//=======================================================================
BRepFilletAPI_MakeChamfer::BRepFilletAPI_MakeChamfer(const TopoDS_Shape &S):myBuilder(S)
{
}



//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void BRepFilletAPI_MakeChamfer::Add(const TopoDS_Edge &E )
{
  myBuilder.Add(E);
}



//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void BRepFilletAPI_MakeChamfer::Add(const Standard_Real Dis, 
			      const TopoDS_Edge &E,
			      const TopoDS_Face &F)
{
  myBuilder.Add(Dis, E, F);
}


//=======================================================================
//function : SetDist
//purpose  : 
//=======================================================================

void BRepFilletAPI_MakeChamfer::SetDist(const Standard_Real Dis, 
				  const Standard_Integer IC,
				  const TopoDS_Face &F)
{
  myBuilder.SetDist(Dis, IC, F);
}

//=======================================================================
//function : GetDist
//purpose  : 
//================================================================

void BRepFilletAPI_MakeChamfer::GetDist(const Standard_Integer IC,
				  Standard_Real&         Dis) const
{
  myBuilder.GetDist(IC, Dis);

}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void BRepFilletAPI_MakeChamfer::Add(const Standard_Real Dis1, 
			      const Standard_Real Dis2,
			      const TopoDS_Edge &E,
			      const TopoDS_Face &F)
{
  myBuilder.Add(Dis1,Dis2,E,F);
}


//=======================================================================
//function : SetDists
//purpose  : 
//=======================================================================

void BRepFilletAPI_MakeChamfer::SetDists(const Standard_Real Dis1, 
				   const Standard_Real Dis2,
				   const Standard_Integer IC,
				   const TopoDS_Face &F)
{
  myBuilder.SetDists(Dis1,Dis2,IC,F);
}

//=======================================================================
//function : Dists
//purpose  : 
//================================================================

void BRepFilletAPI_MakeChamfer::Dists(const Standard_Integer IC,
				Standard_Real&         Dis1, 
				Standard_Real&         Dis2) const
{
  Standard_Real temp1, temp2;
  myBuilder.Dists(IC,temp1,temp2);
  Dis1 = temp1;
  Dis2 = temp2;

}


//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void BRepFilletAPI_MakeChamfer::AddDA(const Standard_Real Dis, 
				const Standard_Real Angle,
				const TopoDS_Edge &E,
				const TopoDS_Face &F)
{
  myBuilder.AddDA(Dis, Angle, E, F);
}

//=======================================================================
//function : SetDist
//purpose  : 
//=======================================================================

void BRepFilletAPI_MakeChamfer::SetDistAngle(const Standard_Real Dis, 
				       const Standard_Real Angle,
				       const Standard_Integer IC,
				       const TopoDS_Face &F)
{
  myBuilder.SetDistAngle(Dis, Angle, IC, F);
}

//=======================================================================
//function : GetDistAngle
//purpose  : 
//================================================================

void BRepFilletAPI_MakeChamfer::GetDistAngle(const Standard_Integer IC,
				       Standard_Real&         Dis, 
				       Standard_Real&         Angle,
				       Standard_Boolean&      DisOnFace1) const
{
  myBuilder.GetDistAngle(IC, Dis, Angle, DisOnFace1);

}

//=======================================================================
//function : IsSymetric
//purpose  : 
//===============================================================

Standard_Boolean BRepFilletAPI_MakeChamfer::IsSymetric(const Standard_Integer IC) const
{
  ChFiDS_ChamfMethod ChamfMeth =  myBuilder.IsChamfer(IC);
  Standard_Boolean ret = Standard_False;

  if (ChamfMeth == ChFiDS_Sym) ret = Standard_True;

  return ret;
}


//=======================================================================
//function : IsTwoDistances
//purpose  : 
//===============================================================

Standard_Boolean BRepFilletAPI_MakeChamfer::IsTwoDistances(const Standard_Integer IC) const
{
  ChFiDS_ChamfMethod ChamfMeth =  myBuilder.IsChamfer(IC);
  Standard_Boolean ret = Standard_False;

  if (ChamfMeth == ChFiDS_TwoDist) ret = Standard_True;

  return ret;
}



//=======================================================================
//function : IsDistanceAngle
//purpose  : 
//===============================================================

Standard_Boolean BRepFilletAPI_MakeChamfer::IsDistanceAngle(const Standard_Integer IC) const
{
  ChFiDS_ChamfMethod ChamfMeth =  myBuilder.IsChamfer(IC);
  Standard_Boolean ret = Standard_False;

  if (ChamfMeth == ChFiDS_DistAngle) ret = Standard_True;

  return ret;
}



//=======================================================================
//function : ResetContour
//purpose  : 
//===============================================================

void BRepFilletAPI_MakeChamfer::ResetContour(const Standard_Integer IC)
{
  myBuilder.ResetContour(IC);
}


//=======================================================================
//function : NbContours
//purpose  : 
//=======================================================================

Standard_Integer BRepFilletAPI_MakeChamfer::NbContours()const
{
  return myBuilder.NbElements();
}



//=======================================================================
//function : Contour
//purpose  : 
//=======================================================================

Standard_Integer BRepFilletAPI_MakeChamfer::Contour(const TopoDS_Edge &E)const
{
  return myBuilder.Contains(E);
}



//=======================================================================
//function : NbEdges
//purpose  : 
//=======================================================================

Standard_Integer BRepFilletAPI_MakeChamfer::NbEdges(const Standard_Integer I)const

{
  const Handle(ChFiDS_Spine)& Spine = myBuilder.Value(I);
  return (Spine->NbEdges());
}


//=======================================================================
//function : Edge
//purpose  : 
//=======================================================================

const TopoDS_Edge& BRepFilletAPI_MakeChamfer::Edge(const Standard_Integer I,
					     const Standard_Integer J)const
{
  const Handle(ChFiDS_Spine)& Spine = myBuilder.Value(I);
  const TopoDS_Edge& E = Spine->Edges(J);
  return E;
}

//=======================================================================
//function : Remove
//purpose  : 
//=======================================================================

void BRepFilletAPI_MakeChamfer::Remove(const TopoDS_Edge& E)
{
  myBuilder.Remove(E);
}

//=======================================================================
//function : Length
//purpose  : 
//=======================================================================

Standard_Real BRepFilletAPI_MakeChamfer::Length(const Standard_Integer IC)const
{
  return myBuilder.Length(IC);
}

//=======================================================================
//function : FirstVertex
//purpose  : 
//=======================================================================

TopoDS_Vertex BRepFilletAPI_MakeChamfer::FirstVertex(const Standard_Integer IC)const
{
  return myBuilder.FirstVertex(IC);
}


//=======================================================================
//function : LastVertex
//purpose  : 
//=======================================================================

TopoDS_Vertex BRepFilletAPI_MakeChamfer::LastVertex(const Standard_Integer IC)const
{
  return myBuilder.LastVertex(IC);
}


//=======================================================================
//function : Abscissa
//purpose  : 
//=======================================================================

Standard_Real BRepFilletAPI_MakeChamfer::Abscissa(const Standard_Integer IC,
					   const TopoDS_Vertex& V)const
{
  return myBuilder.Abscissa(IC,V);
}


//=======================================================================
//function : RelativeAbscissa
//purpose  : 
//=======================================================================

Standard_Real BRepFilletAPI_MakeChamfer::RelativeAbscissa(const Standard_Integer IC,
						   const TopoDS_Vertex& V)const
{
  return myBuilder.RelativeAbscissa(IC,V);
}

//=======================================================================
//function : ClosedAndTangent
//purpose  : 
//=======================================================================

Standard_Boolean BRepFilletAPI_MakeChamfer::ClosedAndTangent
(const Standard_Integer IC)const
{
  return myBuilder.ClosedAndTangent(IC);
}


//=======================================================================
//function : Closed
//purpose  : 
//=======================================================================

Standard_Boolean BRepFilletAPI_MakeChamfer::Closed
(const Standard_Integer IC)const
{
  return myBuilder.Closed(IC);
}

//=======================================================================
//function : Builder
//purpose  : 
//=======================================================================

Handle(TopOpeBRepBuild_HBuilder) BRepFilletAPI_MakeChamfer::Builder()const 
{
  return myBuilder.Builder();
}

//=======================================================================
//function : Build
//purpose  : 
//=======================================================================

void BRepFilletAPI_MakeChamfer::Build()
{
  myBuilder.Compute();
  if (myBuilder.IsDone()){
    Done();
    myShape = myBuilder.Shape();
      
      //creation of the Map.
    TopExp_Explorer ex;
    for (ex.Init(myShape, TopAbs_FACE); ex.More(); ex.Next()) {
      myMap.Add(ex.Current());
    }
  }
}

//=======================================================================
//function : Reset
//purpose  : 
//=======================================================================

void BRepFilletAPI_MakeChamfer::Reset()
{
  NotDone();
  myBuilder.Reset();
  myMap.Clear();
}

//=======================================================================
//function : Generated
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepFilletAPI_MakeChamfer::Generated
  (const TopoDS_Shape& EorV)
{
  return myBuilder.Generated(EorV);
}

//=======================================================================
//function : Modified
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepFilletAPI_MakeChamfer::Modified
  (const TopoDS_Shape& F)
{
  myGenerated.Clear();

  if (myBuilder.Builder()->IsSplit(F, TopAbs_OUT)) {
    TopTools_ListIteratorOfListOfShape It(myBuilder.Builder()->Splits(F, TopAbs_OUT));
    for(;It.More();It.Next()) {
      myGenerated.Append(It.Value());
    }
  }
  if (myBuilder.Builder()->IsSplit(F, TopAbs_IN)) {
    TopTools_ListIteratorOfListOfShape It(myBuilder.Builder()->Splits(F, TopAbs_IN));
    for(;It.More();It.Next()) {
      myGenerated.Append(It.Value());
    }
  }
  if (myBuilder.Builder()->IsSplit(F, TopAbs_ON)) {
    TopTools_ListIteratorOfListOfShape It(myBuilder.Builder()->Splits(F, TopAbs_ON));
    for(;It.More();It.Next()) {
      myGenerated.Append(It.Value());
    }
  }
  return myGenerated;
}

//=======================================================================
//function : IsDeleted
//purpose  : 
//=======================================================================

Standard_Boolean BRepFilletAPI_MakeChamfer::IsDeleted(const TopoDS_Shape& F) 
{
  if (myMap.Contains(F) || 
      myBuilder.Builder()->IsSplit (F, TopAbs_OUT)  ||
      myBuilder.Builder()->IsSplit (F, TopAbs_IN)   ||
      myBuilder.Builder()->IsSplit (F, TopAbs_ON))
    return Standard_False;
  
  return Standard_True;    
}

//=======================================================================
//function : Simulate
//purpose  : 
//=======================================================================

void BRepFilletAPI_MakeChamfer::Simulate(const Standard_Integer IC)
{
  myBuilder.Simulate(IC);
}


//=======================================================================
//function : NbSurf
//purpose  : 
//=======================================================================

Standard_Integer  BRepFilletAPI_MakeChamfer::NbSurf(const Standard_Integer IC)const
{
  return myBuilder.NbSurf(IC);
}


//=======================================================================
//function : Sect
//purpose  : 
//=======================================================================

Handle(ChFiDS_SecHArray1) BRepFilletAPI_MakeChamfer::Sect(const Standard_Integer IC,
						    const Standard_Integer IS)const
{
  return myBuilder.Sect(IC, IS);
}
