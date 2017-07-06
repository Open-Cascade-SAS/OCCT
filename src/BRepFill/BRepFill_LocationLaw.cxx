// Created on: 1998-01-14
// Created by: Philippe MANGIN
// Copyright (c) 1998-1999 Matra Datavision
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


#include <Adaptor3d_HCurve.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepFill_LocationLaw.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <GCPnts_AbscissaPoint.hxx>
#include <GeomFill_LocationLaw.hxx>
#include <gp_Mat.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>
#include <Precision.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColgp_Array1OfVec2d.hxx>
#include <TColStd_SequenceOfInteger.hxx>
#include <TopExp.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepFill_LocationLaw,Standard_Transient)

//=======================================================================
//function : Norm
//purpose  : Norm of a Matrix
//=======================================================================
static Standard_Real Norm(const gp_Mat& M) {
  Standard_Real R, Norme;
  gp_XYZ Coord;
  Coord = M.Row(1);
  Norme = Abs(Coord.X()) + Abs(Coord.Y())+ Abs(Coord.Z());
  Coord = M.Row(2);
  R = Abs(Coord.X()) + Abs(Coord.Y())+ Abs(Coord.Z());
  if (R>Norme) Norme = R;
  Coord = M.Row(3);
  R = Abs(Coord.X()) + Abs(Coord.Y())+ Abs(Coord.Z());
  if (R>Norme) Norme = R;

  return Norme;
}

//=======================================================================
//function : ToG0
//purpose  : Calculate tranformation T such as T.M2 = M1
//=======================================================================

static void ToG0(const gp_Mat& M1, const gp_Mat& M2, gp_Mat& T) {
  T =  M2.Inverted();
  T *= M1;
}
			  
//=======================================================================
//function : BRepFill_LocationLaw
//purpose  : 
//=======================================================================

void BRepFill_LocationLaw::Init(const TopoDS_Wire& Path)
 
{
  Standard_Integer NbEdge;
  BRepTools_WireExplorer wexp;
// Class BRep_Tool without fields and without Constructor :
//  BRep_Tool B;
  TopoDS_Edge E;

  myPath = Path;
  myTol = 1.e-4;

  for (NbEdge=0, wexp.Init(myPath); 
       wexp.More(); wexp.Next()) 
//    if (! B.Degenerated(wexp.Current())) NbEdge++;
    if (! BRep_Tool::Degenerated(wexp.Current())) NbEdge++;
  

  myLaws = new (GeomFill_HArray1OfLocationLaw)(1, NbEdge);
  myLength = new (TColStd_HArray1OfReal) (1, NbEdge+1);
  myLength->Init(-1.);
  myLength->SetValue(1, 0.);
  myEdges = new (TopTools_HArray1OfShape) (1, NbEdge);
  myDisc.Nullify();
  TangentIsMain();
}

//=======================================================================
//function : GetStatus
//purpose  : 
//=======================================================================
 GeomFill_PipeError BRepFill_LocationLaw::GetStatus() const
{
  Standard_Integer ii, N = myLaws->Length();
  GeomFill_PipeError Status =  GeomFill_PipeOk;
  for (ii=1; ii<=N && (Status == GeomFill_PipeOk); ii++) {
    Status = myLaws->Value(ii)->ErrorStatus();
  }
  return  Status;
}

//=======================================================================
//function : TangentIsMain
//purpose  : 
//=======================================================================
void BRepFill_LocationLaw::TangentIsMain() 
{
  myType = 1;
}

//=======================================================================
//function : NormalIsMain
//purpose  : 
//=======================================================================
void BRepFill_LocationLaw::NormalIsMain() 
{
  myType = 2;
}

//=======================================================================
//function : BiNormalIsMain
//purpose  : 
//=======================================================================
void BRepFill_LocationLaw::BiNormalIsMain() 
{
  myType = 3;
}

//=======================================================================
//function : TransformInCompatibleLaw
//purpose  : Set in continuity of laws
//=======================================================================
 void BRepFill_LocationLaw::TransformInCompatibleLaw(const Standard_Real TolAngular)
{

  Standard_Real First, Last, Angle;
  Standard_Integer ipath;
  gp_Mat Trsf, M1, M2;
  gp_Vec V, T1, T2, N1, N2;
  gp_XYZ OZ(0, 0, 1);

  myLaws->Value(1)->GetDomain(First, Last);

  for (ipath=2; ipath<=myLaws->Length(); ipath++) {
    myLaws->Value(ipath-1)->D0(Last, M1, V);
    myLaws->Value(ipath)->GetDomain(First, Last);
    myLaws->Value(ipath)->D0(First, M2, V);
    T1.SetXYZ(M1.Column(3));
    T2.SetXYZ(M2.Column(3));
    N1.SetXYZ(M1.Column(1));
    N2.SetXYZ(M2.Column(1));
    if (T1.IsParallel(T2, TolAngular ) && 
	!T1.IsOpposite(T2, TolAngular)) { // Correction G0
      ToG0(M1, M2, Trsf);
    }
    else {
      Standard_Real alpha;
      gp_Vec cross(T1);
      cross.Cross(T2);
      alpha = T2.AngleWithRef(T1, cross);
      gp_Ax1 axe(gp::Origin(), cross.XYZ());
      N2.Rotate(axe, alpha); 

#ifdef OCCT_DEBUG
      if (N2.Dot(T1) > 1.e-9) {
	cout << "Inprecision in TransformInCompatibleLaw" << endl;
        cout << "--- T1.R(N2) = " << N2.Dot(T1) << endl;
	gp_Vec tt;
	tt = T1;
	tt.Rotate(axe, alpha);
	cout << "--- T1.R(T2) = " << tt.Dot(T1) << endl;
	cout << "--- R(N2).R(T2) = " << N2.Dot(tt) << endl;
      }      
#endif
      Angle = N2.AngleWithRef(N1, T1);
      Trsf.SetRotation(OZ, Angle);
    }
    myLaws->Value(ipath)->SetTrsf(Trsf);
  }
} 

//=======================================================================
//function : TransformInG0Law
//purpose  : Set in continuity of laws
//=======================================================================
 void BRepFill_LocationLaw::TransformInG0Law()
{

  Standard_Real First, Last;
  Standard_Integer ipath;
  gp_Mat  M1, M2, aux;//,Trsf
  gp_Vec V;
  myLaws->Value(1)->GetDomain(First, Last);
  for (ipath=2; ipath<=myLaws->Length(); ipath++) {
    myLaws->Value(ipath-1)->D0(Last, M1, V);
    myLaws->Value(ipath)->GetDomain(First, Last);
    myLaws->Value(ipath)->D0(First, M2, V);
    ToG0(M1, M2, aux);
    myLaws->Value(ipath)->SetTrsf(aux);
  }
  
  // Is the law periodical ?
  if  (myPath.Closed()) {
    myLaws->Value(myLaws->Length())->D0(Last, M1, V);
    myLaws->Value(1)->GetDomain(First, Last);
    myLaws->Value(1)->D0(First, M2, V);
  }
}

//=======================================================================
//function : DeleteTransform
//purpose  : Remove the setting in continuity of law. 
//=======================================================================
 void BRepFill_LocationLaw::DeleteTransform()
{
  gp_Mat Id;
  Id.SetIdentity();
  for (Standard_Integer ii=1; ii<=myEdges->Length(); ii++) {
    myLaws->ChangeValue(ii)->SetTrsf(Id);
  }
  myDisc.Nullify();
}

//=======================================================================
//function : NbHoles
//purpose  : Find "Holes"
//=======================================================================
 Standard_Integer BRepFill_LocationLaw::NbHoles(const Standard_Real Tol) 
{
  if (myDisc.IsNull()) {
    TColStd_SequenceOfInteger Seq;
    Standard_Integer ii, NbDisc;
    for (ii=2, NbDisc=-1; ii<=myLaws->Length()+1; ii++) {
      if (IsG1(ii-1, Tol, 1.e-12) == -1) {
	Seq.Append(ii);
      }
    }
    NbDisc = Seq.Length();
    if ( NbDisc > 0) {
      myDisc = new (TColStd_HArray1OfInteger)(1, NbDisc);
      for (ii=1; ii<=NbDisc; ii++)
	 myDisc->SetValue(ii, Seq(ii));
    }   
  }
  if (myDisc.IsNull()) return 0;
  return myDisc->Length();

}

//=======================================================================
//function : Holes
//purpose  : 
//=======================================================================
 void BRepFill_LocationLaw::Holes(TColStd_Array1OfInteger& Disc) const
{
  if (!myDisc.IsNull()) {
    for (Standard_Integer ii=1; ii<=myDisc->Length(); ii++)
      Disc(ii) = myDisc->Value(ii);
  }
}

//=======================================================================
//function : NbLaw
//purpose  : 
//=======================================================================
 Standard_Integer BRepFill_LocationLaw::NbLaw() const
{
  return myLaws->Length();
}

//=======================================================================
//function : Law
//purpose  : 
//=======================================================================
const Handle(GeomFill_LocationLaw)& 
BRepFill_LocationLaw::Law(const Standard_Integer Index) const
{
  return myLaws->Value(Index);
}

//=======================================================================
//function : Wire
//purpose  : 
//=======================================================================
const TopoDS_Wire& BRepFill_LocationLaw::Wire() const
{
  return myPath;
}

//=======================================================================
//function : Edge
//purpose  : 
//=======================================================================
const TopoDS_Edge& BRepFill_LocationLaw::Edge(const Standard_Integer Index) const
{
  return TopoDS::Edge(myEdges->Value(Index));
}

//=======================================================================
//function : Vertex
//purpose  : 
//=======================================================================
 TopoDS_Vertex BRepFill_LocationLaw::Vertex(const Standard_Integer Index) const
{
  TopoDS_Edge E;
  TopoDS_Vertex V;
  if (Index <= myEdges->Length()) {
    E = TopoDS::Edge(myEdges->Value(Index));
    if (E.Orientation() == TopAbs_REVERSED) 
       V = TopExp::LastVertex(E);
    else V = TopExp::FirstVertex(E);
  }
  else if (Index == myEdges->Length()+1) {
    E = TopoDS::Edge(myEdges->Value(Index-1));
    if (E.Orientation() == TopAbs_REVERSED) 
      V = TopExp::FirstVertex(E);
    else V = TopExp::LastVertex(E);
  }
  return V;
}

//===================================================================
//function : PerformVertex
//purpose  : Calculate a vertex of sweeping from a vertex of section
//           and the index of the edge in the trajectory
//===================================================================
void BRepFill_LocationLaw::PerformVertex(const Standard_Integer Index,
					 const TopoDS_Vertex& Input,
					 const Standard_Real TolMin,
					 TopoDS_Vertex& Output,
					 const Standard_Integer ILoc) const
{
  BRep_Builder B;
  Standard_Boolean IsBary = (ILoc == 0);
  Standard_Real First, Last;
  gp_Pnt P;
  gp_Vec V1, V2;//, V;
  gp_Mat M1, M2;

  if (Index>0 && Index<myLaws->Length()) {
    if (ILoc <=0) {
      myLaws->Value(Index)->GetDomain(First, Last);
      myLaws->Value(Index)->D0(Last, M1, V1);
    }

    if (ILoc >= 0) {
      myLaws->Value(Index+1)->GetDomain(First, Last);
      if (ILoc == 0)
	myLaws->Value(Index+1)->D0(First, M2, V2);
      else
	myLaws->Value(Index+1)->D0(First, M1, V1);
    }
  }

  if (Index == 0 || Index == myLaws->Length()) {
    if (!myPath.Closed() || (IsG1(Index, TolMin) != 1)) {
      IsBary = Standard_False;
      if (Index == 0) {
	myLaws->Value(1)->GetDomain(First, Last);
	myLaws->Value(1)->D0(First, M1, V1);
      }
      else {
	myLaws->Value(myLaws->Length())->GetDomain(First, Last);
	myLaws->Value(myLaws->Length())->D0(Last, M1, V1);
      } 
    }
    else {
      if (ILoc <=0) {
	myLaws->Value(myLaws->Length())->GetDomain(First, Last);
	myLaws->Value(myLaws->Length())->D0(Last, M1, V1);
      }

      if (ILoc >=0) {      
	myLaws->Value(1)->GetDomain(First, Last);
	if (ILoc==0)
	  myLaws->Value(1)->D0(First, M2, V2);
	else
	  myLaws->Value(1)->D0(First, M1, V1);
      }
    }
  }

  P = BRep_Tool::Pnt(Input);

  if (IsBary) { 
    gp_XYZ P1(P.XYZ()), P2(P.XYZ());
    P1 *= M1;
    P1 += V1.XYZ();
    P2 *= M2;
    P2 += V2.XYZ();
  
    P.ChangeCoord().SetLinearForm(0.5, P1, 0.5, P2);
    P1 -= P2;
    Standard_Real Tol =  P1.Modulus()/2;
    Tol += TolMin;
    B.MakeVertex(Output, P, Tol);
  }
  else {
    P.ChangeCoord() *= M1;
    P.ChangeCoord() += V1.XYZ();
    B.MakeVertex(Output, P, TolMin);    
  }
  
}

//=======================================================================
//function : CurvilinearBounds
//purpose  : 
//=======================================================================
void BRepFill_LocationLaw::CurvilinearBounds(const Standard_Integer Index,
					     Standard_Real& First, 
					     Standard_Real& Last) const
{
  First = myLength->Value(Index);
  Last  = myLength->Value(Index+1);
  if (Last<0) { //It is required to carry out the calculation 
    Standard_Integer ii, NbE = myEdges->Length();
    Standard_Real Length, f, l;
    GCPnts_AbscissaPoint AbsC;

    for (ii=1, Length=0.; ii<=NbE; ii++) {
      myLaws->Value(ii)->GetDomain(f, l);
      Length += AbsC.Length(myLaws->Value(ii)->GetCurve()->GetCurve(), myTol);
      myLength->SetValue(ii+1, Length);
    }

    First = myLength->Value(Index);
    Last  = myLength->Value(Index+1); 
  }
}

 Standard_Boolean BRepFill_LocationLaw::IsClosed() const
{
  return myPath.Closed();
}

//=======================================================================
//function : IsG1
//purpose  : Evaluate the continuity of the law by a vertex
//=======================================================================
 Standard_Integer 
 BRepFill_LocationLaw::IsG1(const Standard_Integer Index,
			    const Standard_Real SpatialTolerance,
			    const Standard_Real AngularTolerance) const
{
  gp_Vec V1, DV1, V2, DV2;
  gp_Mat M1, M2, DM1, DM2;
  Standard_Real First, Last, EpsNul = 1.e-12;
  Standard_Real TolEps = SpatialTolerance;
  Standard_Boolean Ok_D1 = Standard_False;
  TopoDS_Vertex V;
  TopoDS_Edge E;
  TColgp_Array1OfPnt2d Bid1 (1,1);
  TColgp_Array1OfVec2d Bid2 (1,1);
  
  if (Index>0 && Index<myLaws->Length()) {
    myLaws->Value(Index)->GetDomain(First, Last);
    Ok_D1 = myLaws->Value(Index)->D1(Last, M1, V1, DM1, DV1, 
				     Bid1, Bid2);
    if (!Ok_D1)  myLaws->Value(Index)->D0(Last, M1, V1);

    myLaws->Value(Index+1)->GetDomain(First, Last);
    if (Ok_D1)
      Ok_D1 = myLaws->Value(Index+1)->D1(First, M2, V2, DM2, DV2, 
					 Bid1, Bid2);
    if (!Ok_D1)  myLaws->Value(Index+1)->D0(First, M2, V2);

    E = TopoDS::Edge(myEdges->Value(Index+1));
  }
  if (Index == 0 || Index == myLaws->Length()) {
    if (!myPath.Closed()) return -1;
    myLaws->Value(myLaws->Length())->GetDomain(First, Last);
    Ok_D1 = myLaws->Value(myLaws->Length())->D1(Last, M1, V1, DM1, DV1, 
						Bid1, Bid2);
    if (!Ok_D1)  myLaws->Value(myLaws->Length())->D0(Last, M1, V1);

    myLaws->Value(1)->GetDomain(First, Last);
    if (Ok_D1)
      myLaws->Value(1)->D1(First, M2, V2, DM2, DV2, 
			   Bid1, Bid2);
    if (!Ok_D1)  myLaws->Value(1)->D0(First, M2, V2);

    E = TopoDS::Edge(myEdges->Value(1));
  }

  if (E.Orientation() == TopAbs_REVERSED)
    V = TopExp::LastVertex(E);
  else 
    V = TopExp::FirstVertex(E);
  
  TolEps += 2*BRep_Tool::Tolerance(V);

  Standard_Boolean isG0 = Standard_True;
  Standard_Boolean isG1 = Standard_True;
  
  if ((V1-V2).Magnitude() > TolEps) isG0 = Standard_False;
  if (Norm(M1-M2) > SpatialTolerance) isG0 = Standard_False;
  
  if (!isG0) return -1;
  if (!Ok_D1) return 0; // No control of the derivative
  
  if ( (DV1.Magnitude()>EpsNul) && (DV2.Magnitude()>EpsNul)
       && (DV1.Angle(DV2) > AngularTolerance) ) isG1 = Standard_False;

  // For the next, the tests are mostly empirical
  Standard_Real Norm1 = Norm(DM1);
  Standard_Real Norm2 = Norm(DM2);
  // It two 2 norms are null, it is good
  if ((Norm1 > EpsNul) || (Norm2 > EpsNul)) {
    // otherwise the normalized matrices are compared
    if ((Norm1 > EpsNul) && (Norm2 > EpsNul)) {
      DM1 /= Norm1;
      DM2 /= Norm2;
      if (Norm(DM1 - DM2) > AngularTolerance) isG1 = Standard_False;
    }
    else isG1 = Standard_False; // 1 Null the other is not   
  }
  
  if (isG1) return 1;
  else return 0;
}


//=======================================================================
//function : Parameter
//purpose  : 
//=======================================================================
 void BRepFill_LocationLaw::Parameter(const Standard_Real Abcissa,
				      Standard_Integer& Index,
				      Standard_Real& U)
{
  Standard_Integer iedge, NbE=myEdges->Length();
  Standard_Boolean Trouve =  Standard_False;

  //Control that the lengths are calculated
  if (myLength->Value(NbE+1) < 0) {
    Standard_Real f, l;
    CurvilinearBounds(NbE, f, l);
  }

  // Find the interval
  for (iedge=1; iedge<=NbE && !Trouve; ) {
    if (myLength->Value(iedge+1) >= Abcissa) {
      Trouve = Standard_True;
    }
    else iedge++;
  }
  
  if (Trouve) {
    Standard_Real f, l;
    const Handle(GeomFill_LocationLaw)& Law =  myLaws->Value(iedge);
    Law->GetDomain(f, l);

    if  (Abcissa == myLength->Value(iedge+1)) {
      U = l;
    }
    else if  (Abcissa == myLength->Value(iedge)) {
      U = f;
    } 
    else {
      GCPnts_AbscissaPoint 
	AbsC(myTol, 
	     myLaws->Value(iedge)->GetCurve()->GetCurve(), 
	     Abcissa-myLength->Value(iedge), f);
      U =  AbsC.Parameter();
    }
    Index = iedge;
  }
  else {
    Index = 0;
  } 
}


//===================================================================
//function : D0
//purpose  : Position of a section, with a given curviline abscissa
//===================================================================
 void BRepFill_LocationLaw::D0(const Standard_Real Abcissa,
			       TopoDS_Shape& W)
{
  Standard_Real u;
  Standard_Integer ind;
  gp_Mat M;
  gp_Vec V;

  Parameter(Abcissa, ind, u);
  if (ind != 0) {
    // Positionement
    myLaws->Value(ind)->D0(u, M, V);
    gp_Trsf fila;
    fila.SetValues(M(1,1), M(1,2), M(1,3), V.X(),
		   M(2,1), M(2,2), M(2,3), V.Y(),
		   M(3,1), M(3,2), M(3,3), V.Z());
    //TopLoc_Location Loc(fila);
    //W.Location(Loc.Multiplied(W.Location()));
    W = BRepBuilderAPI_Transform(W, fila, Standard_True); //copy
    ///////////////////////////////////////////
  }
  else {
    W.Nullify();
#ifdef OCCT_DEBUG
    cout << "BRepFill_LocationLaw::D0 : Attention position out of limits" 
         << endl;
#endif
  }
}

//=======================================================================
//function : Abscissa
//purpose  : Calculate the abscissa of a point
//=======================================================================
 Standard_Real BRepFill_LocationLaw::Abscissa(const Standard_Integer Index,
					      const Standard_Real Param)
{
  GCPnts_AbscissaPoint AbsC;
  Standard_Real Length = myLength->Value(Index);
  if (Length < 0) {
    Standard_Real bid;
    CurvilinearBounds(Index, bid, Length);
  }
 
  Length += AbsC.Length(myLaws->Value(Index)->GetCurve()->GetCurve(),
			myLaws->Value(Index)->GetCurve()->FirstParameter(),
			Param, myTol);
  return Length;
}
