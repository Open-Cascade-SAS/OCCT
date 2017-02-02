// Created on: 1993-11-10
// Created by: Jean Yves LEBEY
// Copyright (c) 1993-1999 Matra Datavision
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


#include <Adaptor2d_HCurve2d.hxx>
#include <Adaptor3d_HVertex.hxx>
#include <BRepAdaptor_Curve2d.hxx>
#include <BRepAdaptor_HCurve2d.hxx>
#include <BRepTopAdaptor_HVertex.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <IntPatch_Point.hxx>
#include <Precision.hxx>
#include <Standard_DomainError.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRep_define.hxx>
#include <TopOpeBRep_FFTransitionTool.hxx>
#include <TopOpeBRep_VPointInter.hxx>
#include <TopOpeBRepDS_Transition.hxx>
#include <TopOpeBRepTool_ShapeTool.hxx>

#ifdef OCCT_DEBUG
static TCollection_AsciiString PRODINP("dinp ");
#endif

//=======================================================================
//function : SetPoint
//purpose  : 
//=======================================================================
void TopOpeBRep_VPointInter::SetPoint(const IntPatch_Point& P)
{
  myPPOI = (IntPatch_Point*)&P;

  Standard_Boolean isOn1 = P.IsOnDomS1(); 
  Standard_Boolean isOn2 = P.IsOnDomS2();
  if (isOn1 && isOn2) myShapeIndex = 3;
  else if (isOn2)     myShapeIndex = 2;
  else if (isOn1)     myShapeIndex = 1;
  else                myShapeIndex = 0;
}

//=======================================================================
//function : ArcOnS1
//purpose  : 
//=======================================================================
const TopoDS_Shape&  TopOpeBRep_VPointInter::ArcOnS1() const
{
  const Handle(Adaptor2d_HCurve2d)& HAHC2 = myPPOI->ArcOnS1();
  const BRepAdaptor_Curve2d& BRAC2P = *((BRepAdaptor_Curve2d*)&(HAHC2->Curve2d()));
  return BRAC2P.Edge();
}

//=======================================================================
//function : ArcOnS2
//purpose  : 
//=======================================================================
const TopoDS_Shape&  TopOpeBRep_VPointInter::ArcOnS2() const
{
  const Handle(Adaptor2d_HCurve2d)& HAHC2 = myPPOI->ArcOnS2();
  const BRepAdaptor_Curve2d& BRAC2P = *((BRepAdaptor_Curve2d*)&(HAHC2->Curve2d()));
  return BRAC2P.Edge();
}

//=======================================================================
//function : VertexOnS1
//purpose  : 
//=======================================================================
const TopoDS_Shape& TopOpeBRep_VPointInter::VertexOnS1() const
{
  if ( !myPPOI->IsVertexOnS1() )
    throw Standard_DomainError("TopOpeBRep_VPointInter::VertexOnS1");

  const Handle(BRepTopAdaptor_HVertex)* HBRTAHV = (Handle(BRepTopAdaptor_HVertex)*)&(myPPOI->VertexOnS1());
  return (*HBRTAHV)->Vertex();
}

//=======================================================================
//function : VertexOnS2
//purpose  : 
//=======================================================================
const TopoDS_Shape& TopOpeBRep_VPointInter::VertexOnS2() const 
{
  if ( !myPPOI->IsVertexOnS2() )
    throw Standard_DomainError("TopOpeBRep_VPointInter::VertexOnS2");

  const Handle(BRepTopAdaptor_HVertex)* HBRTAHV = (Handle(BRepTopAdaptor_HVertex)*)&(myPPOI->VertexOnS2());
  return (*HBRTAHV)->Vertex();
}

//=======================================================================
//function : State
//purpose  : 
//=======================================================================
void TopOpeBRep_VPointInter::State(const TopAbs_State S,const Standard_Integer I)
{
  if      (I == 1) myState1 = S;
  else if (I == 2) myState2 = S;
  else throw Standard_DomainError("TopOpeBRep_VPointInter::State");
  UpdateKeep();
}

//=======================================================================
//function : State
//purpose  : 
//=======================================================================
TopAbs_State TopOpeBRep_VPointInter::State(const Standard_Integer I) const
{
  if      (I == 1) return myState1;
  else if (I == 2) return myState2;
  else { throw Standard_DomainError("TopOpeBRep_VPointInter::State");}
}

//=======================================================================
//function : EdgeON
//purpose  : 
//=======================================================================
void TopOpeBRep_VPointInter::EdgeON(const TopoDS_Shape& Eon,const Standard_Real Par,const Standard_Integer I)
{
  if      (I == 1) {
    myEdgeON1 = Eon;
    myEdgeONPar1 = Par;
  }
  else if (I == 2) {
    myEdgeON2 = Eon;
    myEdgeONPar2 = Par;
  }
}

//=======================================================================
//function : EdgeON
//purpose  : 
//=======================================================================
const TopoDS_Shape& TopOpeBRep_VPointInter::EdgeON(const Standard_Integer I) const
{
  if      (I == 1) return myEdgeON1;
  else if (I == 2) return myEdgeON2;
  else throw Standard_DomainError("TopOpeBRep_VPointInter::EdgeON");
}

//=======================================================================
//function : EdgeONParameter
//purpose  : 
//=======================================================================
Standard_Real TopOpeBRep_VPointInter::EdgeONParameter(const Standard_Integer I) const
{
  if      (I == 1) return myEdgeONPar1;
  else if (I == 2) return myEdgeONPar2;
  else throw Standard_DomainError("TopOpeBRep_VPointInter::EdgeONParameter");
}

//=======================================================================
//function : Edge
//purpose  : 
//=======================================================================
const TopoDS_Shape& TopOpeBRep_VPointInter::Edge(const Standard_Integer I) const
{
  if      (I == 1 && IsOnDomS1() ) return ArcOnS1(); 
  else if (I == 2 && IsOnDomS2() ) return ArcOnS2(); 

  return myNullShape;
}

//=======================================================================
//function : EdgeParameter
//purpose  : 
//=======================================================================
Standard_Real TopOpeBRep_VPointInter::EdgeParameter(const Standard_Integer I) const
{
  if      (I == 1 && IsOnDomS1() ) return ParameterOnArc1();
  else if (I == 2 && IsOnDomS2() ) return ParameterOnArc2();
  return 0.;
}

//=======================================================================
//function : EdgeParameter
//purpose  : 
//=======================================================================
gp_Pnt2d TopOpeBRep_VPointInter::SurfaceParameters(const Standard_Integer I) const
{ 
  Standard_Real u = 0., v = 0.;
  //if      (I == 1 && IsOnDomS1() ) ParametersOnS1(u,v);
  //else if (I == 2 && IsOnDomS1() ) ParametersOnS2(u,v);
  if      (I == 1 ) ParametersOnS1(u,v);
  else if (I == 2 ) ParametersOnS2(u,v);
  gp_Pnt2d p2d(u,v);
  return p2d;
}

//=======================================================================
//function : IsVertex
//purpose  : 
//=======================================================================
Standard_Boolean TopOpeBRep_VPointInter::IsVertex(const Standard_Integer I) const
{
  if      ( I == 0 )                   return Standard_False;
  if      ( I == 1 && IsVertexOnS1() ) return Standard_True;
  else if ( I == 2 && IsVertexOnS2() ) return Standard_True;
  return Standard_False;
}

//=======================================================================
//function : Vertex
//purpose  : 
//=======================================================================
const TopoDS_Shape& TopOpeBRep_VPointInter::Vertex(const Standard_Integer I) const
{
  if      ( I == 1 && IsVertexOnS1() ) return VertexOnS1();
  else if ( I == 2 && IsVertexOnS2() ) return VertexOnS2();
  return myNullShape;
}

//=======================================================================
//function : UpdateKeep
//purpose  :
//=======================================================================
void TopOpeBRep_VPointInter::UpdateKeep() 
{
#define M_SINON(s) (((s) == TopAbs_IN) || ((s) == TopAbs_ON))
  
  TopAbs_State pos1 = State(1);
  TopAbs_State pos2 = State(2);

  Standard_Integer SI = ShapeIndex();

  Standard_Boolean condition=Standard_False; 

  if      (SI == 1) condition = M_SINON(pos2);
  else if (SI == 2) condition = M_SINON(pos1);
  else if (SI == 0) condition = M_SINON(pos1) && M_SINON(pos2);
  else if (SI == 3) condition = M_SINON(pos1) && M_SINON(pos2);
  // NYI : SI == 3 --> le VP devrait toujours etre Keep() (par definition)
  
  myKeep = condition;
}

//=======================================================================
//function : EqualpP
//purpose  : returns <True> if the 3d points and the parameters of the
//           VPoints are same.
//=======================================================================
Standard_Boolean TopOpeBRep_VPointInter::EqualpP(const TopOpeBRep_VPointInter& VP) const
{
  Standard_Real p1 = ParameterOnLine();
  Standard_Real p2 = VP.ParameterOnLine();      
  Standard_Boolean pequal = fabs(p1-p2) < Precision::PConfusion();
  gp_Pnt P1 = Value(); gp_Pnt P2 = VP.Value();
  Standard_Real Ptol1 = Tolerance(), Ptol2 = VP.Tolerance();
  Standard_Real Ptol = (Ptol1 > Ptol2) ? Ptol1 : Ptol2;
  Standard_Boolean Pequal = P1.IsEqual(P2,Ptol);
  Standard_Boolean pPequal = ( pequal && Pequal );
  return pPequal;  
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
Standard_Boolean TopOpeBRep_VPointInter::ParonE(const TopoDS_Edge& E,Standard_Real& par) const
{
  Standard_Boolean found = Standard_False;
  if (IsOnDomS1()) { 
    if(E.IsSame(ArcOnS1())) found = Standard_True;
    if (found) {par = ParameterOnArc1(); return found;}
  }
  if (IsOnDomS2()) { 
    if(E.IsSame(ArcOnS2())) found = Standard_True;
    if (found) {par = ParameterOnArc2(); return found;}
  }

  for (Standard_Integer i = 1; i <= 2; i++) {
    if (State(i) != TopAbs_ON) continue;
    if (EdgeON(i).IsSame(E)) {
      par = EdgeONParameter(i); 
      return Standard_True;
    }
  }
  return found;
}

//=======================================================================
//function : DumpEdge
//purpose  : 
//=======================================================================
Standard_OStream& TopOpeBRep_VPointInter::Dump(const Standard_Integer I,const TopoDS_Face& F,Standard_OStream& OS) const
{
  const TopoDS_Edge& E = TopoDS::Edge(Edge(I)); 
#ifdef OCCT_DEBUG
  Standard_Real Epar =
#endif
             EdgeParameter(I); 
#ifdef OCCT_DEBUG
  TopAbs_Orientation O =
#endif
           E.Orientation();
#ifdef OCCT_DEBUG
  Standard_Boolean closingedge = 
#endif
                    TopOpeBRepTool_ShapeTool::Closed(E,F);

#ifdef OCCT_DEBUG
  if (closingedge) OS<<"on closing edge "; else OS<<"on edge "; TopAbs::Print(O,cout);
  cout<<" of "<<I<<" : par : "<<Epar<<endl;
  TopOpeBRep_FFTransitionTool::ProcessLineTransition(*this,I,O);
  OS<<"line transition ";
  if (closingedge) OS<<"on closing edge "; else OS<<"on edge "; TopAbs::Print(O,cout);
  OS<<" of "<<I<<endl;
#endif
  
  return OS;
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================
#ifdef OCCT_DEBUG
Standard_OStream& TopOpeBRep_VPointInter::Dump(const TopoDS_Face& FF1,const TopoDS_Face& FF2,Standard_OStream& OS) const
{
  const TopoDS_Face& F1 = TopoDS::Face(FF1);
  const TopoDS_Face& F2 = TopoDS::Face(FF2);
  OS<<"VP "<<myIndex<<" on "<<myShapeIndex<<" :";
  Standard_Real Cpar = ParameterOnLine(); OS<<" on curve : "<<Cpar; 
  if (!myKeep) OS<<" NOT kept";
  OS<<endl;
  const gp_Pnt& P = Value(); 
  OS<<PRODINP<<"P"<<myIndex<<" "; OS<<P.X()<<" "<<P.Y()<<" "<<P.Z();
  OS<<"; #draw"<<endl;
   
  if (IsVertexOnS1()) { OS<<"is vertex of 1"<<endl; }
  if (IsVertexOnS2()) { OS<<"is vertex of 2"<<endl; }
  if (IsMultiple())   { OS<<"is multiple"<<endl; }
  if (IsInternal())   { OS<<"is internal"<<endl; }

  if      (myShapeIndex == 1) { 
    Dump(1,F1,OS); 
  }
  else if (myShapeIndex == 2) { 
    Dump(2,F2,OS); 
  }
  else if (myShapeIndex == 3) { 
    Dump(1,F1,OS);
    Dump(2,F2,OS); 
  }
#else
Standard_OStream& TopOpeBRep_VPointInter::Dump(const TopoDS_Face&,const TopoDS_Face&,Standard_OStream& OS) const
{
#endif

  return OS;
}

//=======================================================================
//function : PThePointOfIntersectionDummy
//purpose  : 
//=======================================================================
TopOpeBRep_PThePointOfIntersection TopOpeBRep_VPointInter::PThePointOfIntersectionDummy() const
{
  return myPPOI;
}
