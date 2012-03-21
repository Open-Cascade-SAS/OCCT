// Created on: 1997-04-17
// Created by: Christophe MARION
// Copyright (c) 1997-1999 Matra Datavision
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


#include <HLRBRep_FaceData.ixx>
#include <HLRAlgo_EdgesBlock.hxx>
#include <BRep_Tool.hxx>

//=======================================================================
//function : FaceData
//purpose  : 
//=======================================================================

HLRBRep_FaceData::HLRBRep_FaceData () :
myFlags(0),mySize(0)
{ Selected(Standard_True); }

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void HLRBRep_FaceData::Set (const TopoDS_Face& FG,
			    const TopAbs_Orientation Or,
			    const Standard_Boolean Cl,
			    const Standard_Integer NW)
{
  Closed(Cl);
  Geometry().Surface(FG);
  myTolerance = (Standard_ShortReal)(BRep_Tool::Tolerance(FG));
  Orientation(Or);
  Wires() = new HLRAlgo_WiresBlock(NW);
}

//=======================================================================
//function : SetWire
//purpose  : 
//=======================================================================

void HLRBRep_FaceData::SetWire (const Standard_Integer WI,
				const Standard_Integer NE)
{
  Wires()->Set(WI,new HLRAlgo_EdgesBlock(NE));
}

//=======================================================================
//function : SetWEdge
//purpose  : 
//=======================================================================

void HLRBRep_FaceData::SetWEdge (const Standard_Integer WI,
				 const Standard_Integer EWI,
				 const Standard_Integer EI,
				 const TopAbs_Orientation Or,
				 const Standard_Boolean OutL,
				 const Standard_Boolean Inte,
				 const Standard_Boolean Dble,
				 const Standard_Boolean IsoL)
{
  Wires()->Wire(WI)->Edge       (EWI,EI);
  Wires()->Wire(WI)->Orientation(EWI,Or);
  Wires()->Wire(WI)->OutLine    (EWI,OutL);
  Wires()->Wire(WI)->Internal   (EWI,Inte);
  Wires()->Wire(WI)->Double     (EWI,Dble);
  Wires()->Wire(WI)->IsoLine    (EWI,IsoL);
}
