// Created on: 1997-04-10
// Created by: Prestataire Mary FABIEN
// Copyright (c) 1997-1999 Matra Datavision
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


#include <BRep_Tool.hxx>
#include <gp_Pnt.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopOpeBRepDS.hxx>
#include <TopOpeBRepDS_Check.hxx>
#include <TopOpeBRepDS_CheckStatus.hxx>
#include <TopOpeBRepDS_CurvePointInterference.hxx>
#include <TopOpeBRepDS_DataMapIteratorOfDataMapOfCheckStatus.hxx>
#include <TopOpeBRepDS_DataMapOfCheckStatus.hxx>
#include <TopOpeBRepDS_EdgeVertexInterference.hxx>
#include <TopOpeBRepDS_HDataStructure.hxx>
#include <TopOpeBRepDS_Interference.hxx>
#include <TopOpeBRepDS_InterferenceTool.hxx>
#include <TopOpeBRepDS_Kind.hxx>
#include <TopOpeBRepDS_ListIteratorOfListOfInterference.hxx>
#include <TopOpeBRepDS_ListOfInterference.hxx>
#include <TopOpeBRepDS_PointExplorer.hxx>
#include <TopOpeBRepTool_ShapeTool.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>

#include <string.h>
IMPLEMENT_STANDARD_RTTIEXT(TopOpeBRepDS_Check,Standard_Transient)

//=======================================================================
//function : Create
//purpose  : 
//=======================================================================
TopOpeBRepDS_Check::TopOpeBRepDS_Check(const Handle(TopOpeBRepDS_HDataStructure)& HDS)
{
  myHDS = HDS;
  myMapSurfaceStatus.Clear();
  myMapCurveStatus.Clear();
  myMapPointStatus.Clear();
  myMapShapeStatus.Clear();
}

//=======================================================================
//function : Create
//purpose  : 
//=======================================================================

TopOpeBRepDS_Check::TopOpeBRepDS_Check()
{
    myMapSurfaceStatus.Clear();
    myMapCurveStatus.Clear();
    myMapPointStatus.Clear();
    myMapShapeStatus.Clear();
}

//=======================================================================
//function : ChkIntg
//purpose  : Check Integrity
//=======================================================================
Standard_Boolean CheckEdgeParameter(const Handle(TopOpeBRepDS_HDataStructure)& myHDS);
Standard_Boolean TopOpeBRepDS_Check::ChkIntg()
{
  const TopOpeBRepDS_DataStructure& DS = myHDS->DS();
  Standard_Boolean bI=Standard_False;
  // Check the integrity of the DS
  Standard_Integer i,nshape = DS.NbShapes();
  for (i = 1; i <= nshape; i++) {
    // Integrity of Interferences : Check support and geometry
    const TopOpeBRepDS_ListOfInterference& LI = DS.ShapeInterferences(i);
    bI = ChkIntgInterf(LI);
  }
  Standard_Integer nsurface = DS.NbSurfaces();
  for (i = 1; i <= nsurface; i++) {
    // Integrity of Interferences : Check support and geometry
    const TopOpeBRepDS_ListOfInterference& LI = DS.SurfaceInterferences(i);
    bI = bI && ChkIntgInterf(LI);
  }
  
  Standard_Integer ncurve = DS.NbCurves();
  for (i = 1; i <= ncurve; i++) {
    // Integrity of Interferences : Check support and geometry
    const TopOpeBRepDS_ListOfInterference& LI = DS.CurveInterferences(i);
    bI = bI && ChkIntgInterf(LI);
  }
  
  Standard_Integer npoint = DS.NbPoints();
  for (i = 1; i <= npoint; i++) {
    // Integrity of Interferences : Check support and geometry
    const TopOpeBRepDS_ListOfInterference& LI = DS.PointInterferences(i);
    bI = bI && ChkIntgInterf(LI);
  }

  //  CheckEdgeParameter();
  CheckEdgeParameter(myHDS);

  return bI;
}

//=======================================================================
//function : ChkIntgInterf
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepDS_Check::ChkIntgInterf
(const TopOpeBRepDS_ListOfInterference& LI)
{
  TopOpeBRepDS_ListIteratorOfListOfInterference it1;
  it1.Initialize(LI);
  Standard_Boolean IsOK = Standard_True;
  while (it1.More() ) {
    Handle(TopOpeBRepDS_Interference)& I1 = it1.Value();
    IsOK = IsOK && CheckDS(I1->Support(), I1->SupportType());
    IsOK = IsOK && CheckDS(I1->Geometry(), I1->GeometryType());
    it1.Next();
  }
  return IsOK;
} 

//=======================================================================
//function : CheckDS
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepDS_Check::CheckDS(const Standard_Integer I,
					     const TopOpeBRepDS_Kind K)
{
  // geometry
  switch(K) {
  case TopOpeBRepDS_SURFACE :
    {
      if(myHDS->NbSurfaces() < I) {
	if(myMapSurfaceStatus.IsBound(I))
	  myMapSurfaceStatus.UnBind(I);
	myMapSurfaceStatus.Bind(I, TopOpeBRepDS_NOK);
	return Standard_False;
      }
      if(!myMapSurfaceStatus.IsBound(I))
	myMapSurfaceStatus.Bind(I,TopOpeBRepDS_OK);
      return Standard_True;
    }
  case TopOpeBRepDS_CURVE :
    {
      if(myHDS->NbCurves() < I) {
	if(myMapCurveStatus.IsBound(I))
	  myMapCurveStatus.UnBind(I);
	myMapCurveStatus.Bind(I, TopOpeBRepDS_NOK);
	return Standard_False;
      }
      
      if(!myMapCurveStatus.IsBound(I))
	myMapCurveStatus.Bind(I,TopOpeBRepDS_OK);
      return Standard_True;
    }
  case TopOpeBRepDS_POINT :
    {
      if(myHDS->NbPoints() < I) {
	if(myMapPointStatus.IsBound(I))
	  myMapPointStatus.UnBind(I);
	myMapPointStatus.Bind(I, TopOpeBRepDS_NOK);
	return Standard_False;
      }
      if(!myMapPointStatus.IsBound(I))
	myMapPointStatus.Bind(I,TopOpeBRepDS_OK);
      return Standard_True;
    }
  default:
    break ;
  }
  

  // topology
  if(myHDS->NbShapes() < I) {
    if(myMapShapeStatus.IsBound(I))
      myMapShapeStatus.UnBind(I);
    myMapShapeStatus.Bind(I,TopOpeBRepDS_NOK);
    return Standard_False;
  }
  const TopoDS_Shape& S =myHDS->Shape(I);

  TopAbs_ShapeEnum se=TopAbs_COMPOUND;

  switch (K) {
  case TopOpeBRepDS_SOLID  : se = TopAbs_SOLID; break;
  case TopOpeBRepDS_SHELL  : se = TopAbs_SHELL; break;
  case TopOpeBRepDS_FACE   : se = TopAbs_FACE;  break;
  case TopOpeBRepDS_WIRE   : se = TopAbs_WIRE;  break;
  case TopOpeBRepDS_EDGE   : se = TopAbs_EDGE;  break;
  case TopOpeBRepDS_VERTEX : se = TopAbs_VERTEX;break;
  default:
    break ;
  }
  if(S.ShapeType() != se) {
    if(myMapShapeStatus.IsBound(I))
      myMapShapeStatus.UnBind(I);
    myMapShapeStatus.Bind(I,TopOpeBRepDS_NOK);
    return Standard_False;
  }
  if(!myMapShapeStatus.IsBound(I))
    myMapShapeStatus.Bind(I,TopOpeBRepDS_OK);
  return Standard_True;;
}

//=======================================================================
//function : ChkIntgSamDom
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepDS_Check::ChkIntgSamDom()
{
  Standard_Boolean b = Standard_True, bb = Standard_False;
  TopOpeBRepDS_DataStructure& BDS = myHDS->ChangeDS();
  Standard_Integer NbSh = myHDS->NbShapes(), i, Curr, Loc;
  for(i = 1;i <= NbSh; i++) {
    // Verifie que les Shapes de mySameDomaine existe bien dans la DS
    const TopoDS_Shape& Sind = myHDS->Shape(i);
    const TopTools_ListOfShape& losi = BDS.ShapeSameDomain(Sind);
    if(!CheckShapes(losi)) {
      b = Standard_False;
    }
    
    // Verification de SameDomaineRef
    Curr = BDS.SameDomainRef(i);
    Loc = BDS.SameDomainRef(Curr);
    if(Curr && (Curr != Loc)) {
      b = Standard_False;
    }
    
    if(Curr) {
      // Verification du type des differents Shapes SameDomain
      const TopoDS_Shape& Sref = myHDS->Shape(Curr);
      if(Sind.ShapeType() != Sref.ShapeType()) {
	b = Standard_False;
      }

      // Verifier que ShapeSameDomain(Sref) contient bien Sind
      // sauf si Sind == Sref
      if(i != Curr) {
	const TopTools_ListOfShape& losr = BDS.ShapeSameDomain(Sref);
	TopTools_ListIteratorOfListOfShape liolos;
	liolos.Initialize(losr);
	while (liolos.More() ) {
	  const TopoDS_Shape& Sh = liolos.Value();
	  Loc = myHDS->Shape(Sh);
	  if(Loc == i) {
	    bb = Standard_True;
	    break;
	  }
	  liolos.Next();
	}
	if(!bb) {
	  b = Standard_False;
	}
      }
    }
  }
  return b;
}

//=======================================================================
//function : CheckShapes
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepDS_Check::CheckShapes
(const TopTools_ListOfShape& LS ) const
{
  Standard_Integer index;
  TopTools_ListIteratorOfListOfShape it(LS);
  while (it.More()) {
    const TopoDS_Shape& itS = it.Value();
    index = myHDS->Shape(itS);
    if (!index) return Standard_False;
    it.Next();
  }
  return Standard_True;
}

//=======================================================================
//function : OneVertexOnPnt
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepDS_Check::OneVertexOnPnt(){
  Standard_Boolean b = Standard_True;
//  Standard_Integer i, j, k;
  Standard_Integer i, j;
  Standard_Integer Curr1, Curr2, sdr1, sdr2;
  Standard_Integer NbVe = 0, NbPo = myHDS->NbPoints();
//  Standard_Real tol, tol1, tol2, Dist;
  Standard_Real tol1, tol2, Dist;
  TColStd_IndexedMapOfInteger vert;
  vert.Clear();
  for(i = 1;i <= myHDS->NbShapes();i++) {
    const TopoDS_Shape& S = myHDS->Shape(i);
    if((S.ShapeType() == TopAbs_VERTEX) &&
       myHDS->HasShape(S))
      vert.Add(i);
  }
  NbVe = vert.Extent();
  for(i = 1;i <= NbVe; i++) {
    Curr1 = vert.FindKey(i);
    const TopoDS_Shape& S1 = myHDS->Shape(Curr1);
    sdr1 = myHDS->SameDomainReference(S1);
    for(j = i+1;j <= NbVe; j++) {
      Curr2 = vert.FindKey(j);
      const TopoDS_Shape& S2 = myHDS->Shape(Curr2);
      sdr2 = myHDS->SameDomainReference(S2);
      tol1 = TopOpeBRepTool_ShapeTool::Tolerance(S1);
      tol2 = TopOpeBRepTool_ShapeTool::Tolerance(S2);
      const gp_Pnt& P1 = TopOpeBRepTool_ShapeTool::Pnt(S1);
      const gp_Pnt& P2 = TopOpeBRepTool_ShapeTool::Pnt(S2);
      Dist = P1.Distance(P2);
      if(Dist <= tol1 + tol2) {
	if(sdr1 != sdr2) {
	  b = Standard_False;
	}
      }
      else if(sdr1 == sdr2) {
	b = Standard_False;
      }
    }
    TopOpeBRepDS_PointExplorer PE(myHDS->DS());
    for(;PE.More(); PE.Next()) {
      const TopOpeBRepDS_Point& dsPnt = PE.Point();
      const gp_Pnt& Pnt1 = dsPnt.Point();
      tol1 = dsPnt.Tolerance();
      tol2 = TopOpeBRepTool_ShapeTool::Tolerance(S1);
      const gp_Pnt& Pnt2 = TopOpeBRepTool_ShapeTool::Pnt(S1);
      Dist = Pnt1.Distance(Pnt2);
      if(Dist <= tol1 + tol2) {
	b = Standard_False;
      }
    }
  }
  for(i = 1;i <= NbPo; i++) {
    TopOpeBRepDS_PointExplorer PE(myHDS->DS());
    if(PE.IsPoint(i)) {
      const TopOpeBRepDS_Point& dsPnt1 = myHDS->Point(i);
      for(j = i+1;j < NbPo;j++) {
	const TopOpeBRepDS_Point& dsPnt2 = myHDS->Point(j);
	if(dsPnt1.IsEqual(dsPnt2)) {
	}
      }
    }
  }
  
  return b;
}

//=======================================================================
//function : CheckEdgeParameter
//purpose  : 
//=======================================================================

///Standard_Boolean TopOpeBRepDS_Check::CheckEdgeParameter() const
Standard_Boolean CheckEdgeParameter(const Handle(TopOpeBRepDS_HDataStructure)& myHDS)
{
  TopOpeBRepDS_ListIteratorOfListOfInterference it1;
  const TopOpeBRepDS_DataStructure& DS = myHDS->DS();
  Standard_Integer i,nshape = DS.NbShapes();
  Standard_Boolean IsOK = Standard_True;
  for (i = 1; i <= nshape; i++) {
    // Integrity of Interferences : Check parameter of EdgeInterferences
    const TopOpeBRepDS_ListOfInterference& LI = DS.ShapeInterferences(i);
    it1.Initialize(LI);
    while (it1.More() ) {
      Handle(TopOpeBRepDS_Interference)& I1 = it1.Value();
      Handle(TopOpeBRepDS_EdgeVertexInterference) EVI =
	Handle(TopOpeBRepDS_EdgeVertexInterference)::DownCast(I1);
      if(!EVI.IsNull()) {
	Standard_Integer Param = (Standard_Integer ) EVI->Parameter();
	if(Param > 1.e50) {
	  IsOK = Standard_False;
	}
      }
      it1.Next();
    }
  }
  
  Standard_Integer ncurve = DS.NbCurves();
  for (i = 1; i <= ncurve; i++) {
    // Integrity of Interferences : Check parameter of CurvesInterferences
    const TopOpeBRepDS_ListOfInterference& LI = DS.CurveInterferences(i);
    it1.Initialize(LI);
    while (it1.More() ) {
      const Handle(TopOpeBRepDS_Interference)& I1 = it1.Value();
      Handle(TopOpeBRepDS_CurvePointInterference) CPI (Handle(TopOpeBRepDS_CurvePointInterference)::DownCast(I1));
      if(!CPI.IsNull()) {
	Standard_Integer Param = 
	  (Standard_Integer ) TopOpeBRepDS_InterferenceTool::Parameter(CPI);
	if(Param > 1.e50) {
	  IsOK = Standard_False;
	}
      }
      it1.Next();
    }
    
  }
  return IsOK;
}

//=======================================================================
//function : PrintIntg
//purpose  : 
//=======================================================================
Standard_OStream& TopOpeBRepDS_Check::PrintIntg(Standard_OStream& OS)
{
  OS<<endl<<endl<<"************************************************"<<endl;
  OS<<"state of the DS : (only the tested elements)"<<endl<<endl;
  
  //Display of the geometry
  PrintMap(myMapSurfaceStatus, "Surface", OS);
  PrintMap(myMapCurveStatus,   "Curve",   OS);
  PrintMap(myMapPointStatus,   "Point",   OS);
  
  //display of the topology
  TopOpeBRepDS_DataMapIteratorOfDataMapOfCheckStatus DMI(myMapShapeStatus);
  TopOpeBRepDS_DataMapOfCheckStatus MapVertex, MapEdge, MapWire, MapFace, MapSolid;
  Standard_Integer i;
  // different Map keep their index of myMapShapeStatus
  for(DMI.Reset();DMI.More();DMI.Next()) {
    i = DMI.Key();
    const TopoDS_Shape& S =myHDS->Shape(i);
    switch(S.ShapeType()) {
    case TopAbs_VERTEX: MapVertex.Bind(i, DMI.Value()); break;
    case TopAbs_EDGE: MapEdge.Bind(i, DMI.Value()); break;
    case TopAbs_WIRE: MapWire.Bind(i, DMI.Value()); break;
    case TopAbs_FACE: MapFace.Bind(i, DMI.Value()); break;
    case TopAbs_SOLID: MapSolid.Bind(i, DMI.Value()); break;
    default:
      break ;
    }
  }
  
  PrintMap(MapVertex, "Vertex", OS);
  PrintMap(MapEdge, "Edge",   OS);
  PrintMap(MapWire, "Wire",   OS);
  PrintMap(MapFace, "Face",   OS);
  PrintMap(MapSolid, "Solid",  OS);
  
  OS<<endl<<endl;
  return OS;
}

//=======================================================================
//function : PrintMap
//purpose  : 
//=======================================================================

Standard_OStream& TopOpeBRepDS_Check::PrintMap(TopOpeBRepDS_DataMapOfCheckStatus& MapStat,
					       const Standard_CString eltstr,
					       Standard_OStream& OS)
{
  TopOpeBRepDS_DataMapIteratorOfDataMapOfCheckStatus DMI(MapStat);
  DMI.Initialize(MapStat);
  if(DMI.More()) {
    Standard_Boolean b = Standard_True;
    OS<<" "<<eltstr<<"\t(/"<<MapStat.Extent()<<")\tnumber ";
    
    PrintElts(MapStat, TopOpeBRepDS_OK,  b, OS);
    if(!b) OS<<" = OK"<<endl;
    PrintElts(MapStat, TopOpeBRepDS_NOK, b, OS);
    if(!b) OS<<" = NOK"<<endl;
  }    
  return OS;
}

//=======================================================================
//function : PrintElts
//purpose  : Print the elements in the state stat of MapStat
//=======================================================================
Standard_OStream& TopOpeBRepDS_Check::PrintElts(TopOpeBRepDS_DataMapOfCheckStatus& MapStat,
						const TopOpeBRepDS_CheckStatus Stat,
						Standard_Boolean& b,
						Standard_OStream& OS)
{
  TopOpeBRepDS_DataMapIteratorOfDataMapOfCheckStatus DMI(MapStat);
  Standard_Boolean bb = !b;
  b = Standard_True;
  Standard_Integer i;
  TopOpeBRepDS_CheckStatus s;
  for(DMI.Reset();DMI.More();DMI.Next()) {
    s = DMI.Value();
    i = DMI.Key();
    if(s == Stat) {
      if(b) b=!b; 
      if(bb){OS<<"\t\tnumber ";bb=!bb;}
      OS<<i<<" ";
    }
  }
  return OS;
}

//=======================================================================
//function : Print
//purpose  : 
//=======================================================================
Standard_OStream& TopOpeBRepDS_Check::Print
(const TopOpeBRepDS_CheckStatus stat,
 Standard_OStream& OS)
{
  switch(stat) {
  case TopOpeBRepDS_OK: OS<<"OK";break;
  case TopOpeBRepDS_NOK: OS<<"NOK";break;
  default : break;
  }
  return OS;
}

//=======================================================================
//function : PrintShape
//purpose  : 
//=======================================================================
Standard_OStream& TopOpeBRepDS_Check::PrintShape
(const TopAbs_ShapeEnum SE,
 Standard_OStream& OS)
{
  switch(SE) {
  case TopAbs_SOLID   : OS<<"Solid ";   break;
  case TopAbs_SHELL   : OS<<"Shell ";   break;
  case TopAbs_FACE    : OS<<"Face  ";    break;
  case TopAbs_WIRE    : OS<<"Wire  ";    break;
  case TopAbs_EDGE    : OS<<"Edge  ";    break;
  case TopAbs_VERTEX  : OS<<"Vertex";  break;
  default:
    break ;
  }
  return OS;
}

//=======================================================================
//function : PrintShape
//purpose  : 
//=======================================================================
Standard_OStream& TopOpeBRepDS_Check::PrintShape
(const Standard_Integer index,
 Standard_OStream& OS)
{
  if(myHDS->NbShapes() < index) { 
    OS<<"**PB**IN**TopOpeBRepDS_Check::PrintShape** ";
    return OS;
  }
  if(!myMapShapeStatus.IsBound(index)) {
    OS<<"NO CHECK HAS PROCESSING"<<endl;
    return OS;
  }
  OS<<" ";
  myHDS->Shape(index);

  return OS;
}

//=======================================================================
//function : HDS
//purpose  : 
//=======================================================================

const Handle(TopOpeBRepDS_HDataStructure)&  TopOpeBRepDS_Check::HDS()const 
{
  return myHDS;
}


//=======================================================================
//function : ChangeHDS
//purpose  : 
//=======================================================================

Handle(TopOpeBRepDS_HDataStructure)&  TopOpeBRepDS_Check::ChangeHDS()
{
  return myHDS;
}
