// Created on: 2000-11-21
// Created by: Peter KURNEV
// Copyright (c) 2000-2012 OPEN CASCADE SAS
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



#include <BOPTools_SSInterference.ixx>

#include <IntTools_Curve.hxx>
#include <IntTools_SequenceOfCurves.hxx>
#include <IntTools_Tools.hxx>
#include <IntTools_PntOn2Faces.hxx>

#include <BOPTools_Curve.hxx>
//modified by NIZNHY-PKV Fri Jun 30 10:08:51 2006
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopoDS_Shape.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>


//=======================================================================
//function :  BOPTools_SSInterference
//purpose  : 
//=======================================================================
BOPTools_SSInterference::BOPTools_SSInterference()
:
  BOPTools_ShapeShapeInterference(0, 0),
  myTangentFacesFlag(Standard_False),
  mySenseFlag(0)
{}

//=======================================================================
//function :  BOPTools_SSInterference::BOPTools_SSInterference
//purpose  : 
//=======================================================================
  BOPTools_SSInterference::BOPTools_SSInterference
  (const Standard_Integer anIndex1,
   const Standard_Integer anIndex2,
   const Standard_Real aTolR3D,
   const Standard_Real aTolR2D,
   const IntTools_SequenceOfCurves& aCvs,
   const IntTools_SequenceOfPntOn2Faces& aPnts)
:  
  BOPTools_ShapeShapeInterference(anIndex1, anIndex2),
  myTangentFacesFlag(Standard_False),
  mySenseFlag(0)
{
  myTolR3D=aTolR3D;
  myTolR2D=aTolR2D;
  
  myCurves.Clear();
  Standard_Integer i, aNbCurves;
  aNbCurves=aCvs.Length();
  for (i=1; i<=aNbCurves; i++) {
    const IntTools_Curve& aIC=aCvs(i);
    BOPTools_Curve aBC(aIC);
    myCurves.Append(aBC);
  }
  //
  SetAlonePnts(aPnts);
}
//=======================================================================
//function :  AppendBlock
//purpose  : 
//=======================================================================
  void BOPTools_SSInterference::AppendBlock(const BOPTools_PaveBlock& aPB) 
{
  myPBs.Append(aPB);
}

//=======================================================================
//function :  PaveBlocks
//purpose  : 
//=======================================================================
  const BOPTools_ListOfPaveBlock& BOPTools_SSInterference::PaveBlocks() const
{
  return myPBs;
}

//=======================================================================
//function :  TolR3D
//purpose  : 
//=======================================================================
  Standard_Real BOPTools_SSInterference::TolR3D() const
{
  return myTolR3D;
}

//=======================================================================
//function :  TolR2D
//purpose  : 
//=======================================================================
  Standard_Real BOPTools_SSInterference::TolR2D() const
{
  return myTolR2D;
}

//=======================================================================
//function :  Curves
//purpose  : 
//=======================================================================
  BOPTools_SequenceOfCurves& BOPTools_SSInterference::Curves() 
{
  return myCurves;
}

//=======================================================================
//function :  NewPaveSet
//purpose  : 
//=======================================================================
  BOPTools_PaveSet& BOPTools_SSInterference::NewPaveSet() 
{
  return myNewPaveSet;
}

//=======================================================================
//function :  SetTangentFacesFlag
//purpose  : 
//=======================================================================
  void BOPTools_SSInterference::SetTangentFacesFlag(const Standard_Boolean aFlag) 
{
  myTangentFacesFlag=aFlag;
}
//=======================================================================
//function :  IsTangentFaces
//purpose  : 
//=======================================================================
  Standard_Boolean BOPTools_SSInterference::IsTangentFaces()const 
{
  return myTangentFacesFlag;
}

//=======================================================================
// function: SetSenseFlag
// purpose: 
//=======================================================================
  void BOPTools_SSInterference::SetSenseFlag (const Standard_Integer iFlag)
				 
{
   mySenseFlag=iFlag;
}
//=======================================================================
// function: SenseFlag
// purpose: 
//=======================================================================
  Standard_Integer BOPTools_SSInterference::SenseFlag () const
				 
{
   return mySenseFlag;
}
//=======================================================================
// function: SetStatesMap
// purpose: 
//=======================================================================
  void BOPTools_SSInterference::SetStatesMap (const BOPTools_IndexedDataMapOfIntegerState& aMap)
{
  myStatesMap=aMap;
}
//=======================================================================
// function: StatesMap
// purpose: 
//=======================================================================
  const BOPTools_IndexedDataMapOfIntegerState& BOPTools_SSInterference::StatesMap () const
{
  return myStatesMap;
}

//=======================================================================
// function: SetAlonePnts
// purpose: 
//=======================================================================
  void BOPTools_SSInterference::SetAlonePnts (const IntTools_SequenceOfPntOn2Faces& aPnts)
{
  Standard_Integer i, aNb;
  myAlonePnts.Clear();

  aNb=aPnts.Length();
  for (i=1; i<=aNb; ++i) {
    const IntTools_PntOn2Faces& aPntOn2Faces=aPnts(i);
    myAlonePnts.Append(aPntOn2Faces);
  }
}

//=======================================================================
// function: AlonePnts
// purpose: 
//=======================================================================
  const IntTools_SequenceOfPntOn2Faces& BOPTools_SSInterference::AlonePnts ()const 
{
  return myAlonePnts;
}

//=======================================================================
// function: AloneVertices
// purpose: 
//=======================================================================
  TColStd_ListOfInteger& BOPTools_SSInterference::AloneVertices () 
{
  return myAloneVertices;
}
//modified by NIZNHY-PKV Fri Jun 30 10:06:12 2006f
//=======================================================================
// function: SetSharedEdges
// purpose: 
//=======================================================================
  void BOPTools_SSInterference::SetSharedEdges (const TColStd_ListOfInteger& aLS) 
{
  Standard_Integer nE;
  TColStd_ListIteratorOfListOfInteger aIt;
  //
  aIt.Initialize(aLS);
  for(; aIt.More(); aIt.Next()) {
    nE=aIt.Value();
    mySharedEdges.Append(nE);
  }
}
//=======================================================================
// function: SharedEdges
// purpose: 
//=======================================================================
  const TColStd_ListOfInteger& BOPTools_SSInterference::SharedEdges()const 
{
  return mySharedEdges;
}
/*
//=======================================================================
// function: SetSharedEdges
// purpose: 
//=======================================================================
  void BOPTools_SSInterference::SetSharedEdges (const TopTools_ListOfShape& aLS) 
{
  TopTools_ListIteratorOfListOfShape aIt;
  //
  aIt.Initialize(aLS);
  for(; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aE=aIt.Value();
    mySharedEdges.Append(aE);
  }
}
//=======================================================================
// function: SharedEdges
// purpose: 
//=======================================================================
  const TopTools_ListOfShape& BOPTools_SSInterference::SharedEdges()const 
{
  return mySharedEdges;
}
*/
//modified by NIZNHY-PKV Fri Jun 30 10:06:14 2006t
