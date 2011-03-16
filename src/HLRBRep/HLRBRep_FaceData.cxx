// File:	HLRBRep_FaceData.cxx
// Created:	Thu Apr 17 12:02:21 1997
// Author:	Christophe MARION
//		<cma@partox.paris1.matra-dtv.fr>

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

