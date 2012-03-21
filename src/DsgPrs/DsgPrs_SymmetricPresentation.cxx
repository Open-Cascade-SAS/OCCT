// Created on: 1997-01-22
// Created by: Prestataire Michael ALEONARD
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



#include <DsgPrs_SymmetricPresentation.ixx>

#include <Precision.hxx>

#include <gp_Lin.hxx>
#include <gp_Circ.hxx>
#include <gp_Vec.hxx>
#include <gp_Dir.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>

#include <Geom_Plane.hxx>
#include <Geom_Line.hxx>

#include <ElCLib.hxx>

#include <gce_MakeLin.hxx>
#include <gce_MakeDir.hxx>

#include <Graphic3d_Group.hxx>
#include <Graphic3d_Array1OfVertex.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_Vertex.hxx>

#include <Prs3d_Arrow.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_LengthAspect.hxx>

#include <TCollection_AsciiString.hxx>

#include <Geom2d_Line.hxx>

#include <GeomAPI.hxx>

#include <IntAna2d_AnaIntersection.hxx>


//===================================================================
//Function:Add
//Purpose: draws the representation of an axial symmetry between two segments.
//===================================================================
void DsgPrs_SymmetricPresentation::Add (const Handle(Prs3d_Presentation)& aPresentation,
					const Handle(Prs3d_Drawer)& aDrawer,	
					const gp_Pnt& AttachmentPoint1,
					const gp_Pnt& AttachmentPoint2,
					const gp_Dir& aDirection1,
					const gp_Lin& aAxis,
					const gp_Pnt& OffsetPoint)
{ 
  Handle(Prs3d_LengthAspect) LA = aDrawer->LengthAspect();
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());  
  gp_Pnt ProjOffsetPoint = ElCLib::Value(ElCLib::Parameter(aAxis,OffsetPoint),aAxis);
  gp_Pnt PjAttachPnt1    = ElCLib::Value(ElCLib::Parameter(aAxis,AttachmentPoint1),aAxis);
  gp_Dir aDirectionAxis  = aAxis.Direction();
//  gp_Lin L1 (AttachmentPoint1,aDirection1);
  Standard_Real h = fabs(ProjOffsetPoint.Distance(PjAttachPnt1)/cos(aDirectionAxis.Angle(aDirection1)));
  
  gp_Vec VL1(aDirection1);
  gp_Vec VLa(PjAttachPnt1,ProjOffsetPoint);
  Standard_Real scal;
  scal = VL1.Dot(VLa);
  if (scal < 0) VL1.Reverse();
  VL1.Multiply(h);

  gp_Pnt P1,P2;

  //===================================
  // SYMETRY OF EDGE PERPEND. TO THE AXIS    
  //   ____        :        ____
  // edge2 |       : -=-   | edge 1
  //       |<------:------>|
  //               :        
  //===================================

  if (VLa.Dot(VL1) == 0) {
    P1 = AttachmentPoint1.Translated(VLa);
    gp_Vec VPntat2Axe(PjAttachPnt1,AttachmentPoint2);  
    P2 = ProjOffsetPoint.Translated(VPntat2Axe);
  }
  else {
    P1 = AttachmentPoint1.Translated(VL1);
    gp_Vec VPntat1Axe(P1,ProjOffsetPoint);
    P2 = ProjOffsetPoint.Translated(VPntat1Axe);
  }

  gp_Lin L3 = gce_MakeLin(P1,P2);
  Standard_Real parmin,parmax,parcur;
  parmin = ElCLib::Parameter(L3,P1);
  parmax = parmin;
  parcur = ElCLib::Parameter(L3,P2);
  Standard_Real dist = Abs(parmin-parcur);
  if (parcur < parmin) parmin = parcur;
  if (parcur > parmax) parmax = parcur;
  parcur = ElCLib::Parameter(L3,OffsetPoint);
  gp_Pnt offp = ElCLib::Value(parcur,L3);

  Standard_Boolean outside = Standard_False;
  if (parcur < parmin) {
    parmin = parcur;
    outside = Standard_True;
  }
  if (parcur > parmax) {
    parmax = parcur;
    outside = Standard_True;
  }

  gp_Pnt PointMin = ElCLib::Value(parmin,L3);
  gp_Pnt PointMax = ElCLib::Value(parmax,L3);
  Graphic3d_Array1OfVertex V(1,2);
  Graphic3d_Array1OfVertex Vc(1,3);
  Quantity_Length X,Y,Z;
  Standard_Real D1,coeff;
  coeff = .5;
  D1 = aAxis.Distance(AttachmentPoint1);
  gp_Pnt pint,Pj_P1,P1Previous;
  P1Previous.SetX(P1.X());
  P1Previous.SetY(P1.Y());
  P1Previous.SetZ(P1.Z());

  
/*//=======================================================
  // TO AVOID CROSSING
  //        P1  -=- P2                P2  -=- P1         
  //          \<-->/                    |<-->|
  //           \  /                     |    |
  //            \/                      |    | 
  //            /\                      |    |
  //           /  \                     |    |
  // Pattach2 /____\ Pattach1 Pattach2 /______\ Pattach1
  //         /  NO \                  /   YES  \
  //=======================================================
*/

  Standard_Boolean Cross = Standard_False;
  gp_Vec Attch1_PjAttch1(AttachmentPoint1,PjAttachPnt1);
  gp_Vec v(P1,ProjOffsetPoint);
  if (v.IsOpposite((Attch1_PjAttch1),Precision::Confusion())){
    Cross = Standard_True;
    gp_Pnt PntTempo;
    PntTempo = P1;
    P1       = P2;
    P2       = PntTempo;
  }  
/*  //===================================
  // FRACTURES OF TRAITS OF CALL    
  //        /             \         
  //       /               \
  //       |      -=-      |
  //       |<------------->| 
  //===================================
*/
  gp_Vec        Vfix;
//  Standard_Real alpha,b,s,d;
  Standard_Real alpha,b;

  if(aAxis.Distance(P1) > D1*(1 + coeff) && !Cross){

    //==== PROCESSING OF FACE ===========
    Prs3d_Root::NewGroup(aPresentation);
    Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
    Pj_P1 = ElCLib::Value(ElCLib::Parameter(aAxis,P1),aAxis);
    gp_Vec Vp(Pj_P1,P1);
    Vfix = Vp.Divided(Vp.Magnitude()).Multiplied(D1*(1 + coeff));
    Pj_P1.Translated(Vfix).Coord(X,Y,Z);
    P1.SetCoord(X,Y,Z);
    Pj_P1.Translated(Vfix.Reversed()).Coord(X,Y,Z);
    P2.SetCoord(X,Y,Z);

    //=================================
    // LISTING AT THE EXTERIOR
    //                        -=-
    //      ->|----------|<------
    //        |          |
    //=================================
    
    L3 = gce_MakeLin(P1,P2);
    parmin = ElCLib::Parameter(L3,P1);
    parmax = parmin;
    parcur = ElCLib::Parameter(L3,P2);
    dist = Abs(parmin-parcur);
    if (parcur < parmin) parmin = parcur;
    if (parcur > parmax) parmax = parcur;
    parcur = ElCLib::Parameter(L3,OffsetPoint);
    offp = ElCLib::Value(parcur,L3);  
    outside = Standard_False;
    if (parcur < parmin) {
      parmin = parcur;
      outside = Standard_True;
    }
    if (parcur > parmax) {
      parmax = parcur;
      outside = Standard_True;
    }    
    PointMin = ElCLib::Value(parmin,L3);
    PointMax = ElCLib::Value(parmax,L3);

    PointMin.Coord(X,Y,Z);
    V(1).SetCoord(X,Y,Z);
    PointMax.Coord(X,Y,Z);
    V(2).SetCoord(X,Y,Z);
    Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);
    
    //==== PROCESSING OF CALL 1 =====
    Prs3d_Root::NewGroup(aPresentation);
    Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
    AttachmentPoint1.Coord(X,Y,Z);
    Vc(1).SetCoord(X,Y,Z);
    
    alpha = aDirectionAxis.Angle(aDirection1);
    b = (coeff*D1)/sin(alpha);
    gp_Vec Vpint(AttachmentPoint1,P1Previous);
    AttachmentPoint1.Translated(Vpint.Divided(Vpint.Magnitude()).Multiplied(b)).Coord(X,Y,Z);
    pint.SetCoord(X,Y,Z);

    pint.Coord(X,Y,Z);
    Vc(2).SetCoord(X,Y,Z);
    P1.Coord(X,Y,Z);
    Vc(3).SetCoord(X,Y,Z);
    Prs3d_Root::CurrentGroup(aPresentation)->Polyline(Vc);
    
    //==== PROCESSING OF CALL 2 =====
    Prs3d_Root::NewGroup(aPresentation);
    Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
    AttachmentPoint2.Coord(X,Y,Z);
    Vc(1).SetCoord(X,Y,Z);
    gp_Pnt Pj_pint  = ElCLib::Value(ElCLib::Parameter(aAxis,pint),aAxis);
    gp_Vec V_int(pint, Pj_pint);
    gp_Pnt Sym_pint;
    Pj_pint.Translated(V_int).Coord(X,Y,Z);
    Sym_pint.SetCoord(X,Y,Z);

    Sym_pint.Coord(X,Y,Z);
    Vc(2).SetCoord(X,Y,Z);
    P2.Coord(X,Y,Z);
    Vc(3).SetCoord(X,Y,Z);
    Prs3d_Root::CurrentGroup(aPresentation)->Polyline(Vc);
  }

/*//===================================
  // FRACTURES OF PROCESSING OF CALL    
  //              -=-    
  //         |<--------->| 
  //         |           |   
  //        /             \         
  //       /               \
  //===================================
*/
  else if (aAxis.Distance(P1) < D1*(1 - coeff) || Cross) {

    //------ PROCESSING OF FACE ------------
    Prs3d_Root::NewGroup(aPresentation);
    Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
    Pj_P1 = ElCLib::Value(ElCLib::Parameter(aAxis,P1),aAxis);
    gp_Vec VpInf(Pj_P1,P1);
    Vfix = VpInf.Divided(VpInf.Magnitude()).Multiplied(D1*(1 - coeff));
    Pj_P1.Translated(Vfix).Coord(X,Y,Z);
    P1.SetCoord(X,Y,Z);
    Pj_P1.Translated(Vfix.Reversed()).Coord(X,Y,Z);
    P2.SetCoord(X,Y,Z);

    //=================================
    // LISTING AT THE EXTERIOR
    //                        -=-
    //      ->|----------|<------
    //        |          |
    //=================================
    L3 = gce_MakeLin(P1,P2);
    parmin = ElCLib::Parameter(L3,P1);
    parmax = parmin;
    parcur = ElCLib::Parameter(L3,P2);
    dist = Abs(parmin-parcur);
    if (parcur < parmin) parmin = parcur;
    if (parcur > parmax) parmax = parcur;
    parcur = ElCLib::Parameter(L3,OffsetPoint);
    offp = ElCLib::Value(parcur,L3);  
    outside = Standard_False;
    if (parcur < parmin) {
      parmin = parcur;
      outside = Standard_True;
    }
    if (parcur > parmax) {
      parmax = parcur;
      outside = Standard_True;
    }    
    PointMin = ElCLib::Value(parmin,L3);
    PointMax = ElCLib::Value(parmax,L3);

    PointMin.Coord(X,Y,Z);
    V(1).SetCoord(X,Y,Z);
    PointMax.Coord(X,Y,Z);
    V(2).SetCoord(X,Y,Z);
    Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);
    
    //==== PROCESSING OF CALL 1 =====
    Prs3d_Root::NewGroup(aPresentation);
    Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
    AttachmentPoint1.Coord(X,Y,Z);
    Vc(1).SetCoord(X,Y,Z);
    
    alpha = aDirectionAxis.Angle(aDirection1);
    b = (coeff*D1)/sin(alpha);
    gp_Vec Vpint(AttachmentPoint1,P1Previous);
    AttachmentPoint1.Translated(Vpint.Divided(Vpint.Magnitude()).Multiplied(b)).Coord(X,Y,Z);
    pint.SetCoord(X,Y,Z);

    pint.Coord(X,Y,Z);
    Vc(2).SetCoord(X,Y,Z);
    P1.Coord(X,Y,Z);
    Vc(3).SetCoord(X,Y,Z);
    Prs3d_Root::CurrentGroup(aPresentation)->Polyline(Vc);
    
    //==== PROCESSING OF CALL 2 =====
    Prs3d_Root::NewGroup(aPresentation);
    Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
    AttachmentPoint2.Coord(X,Y,Z);
    Vc(1).SetCoord(X,Y,Z);
    gp_Pnt Pj_pint  = ElCLib::Value(ElCLib::Parameter(aAxis,pint),aAxis);
    gp_Vec V_int(pint, Pj_pint);
    gp_Pnt Sym_pint;
    Pj_pint.Translated(V_int).Coord(X,Y,Z);
    Sym_pint.SetCoord(X,Y,Z);

    Sym_pint.Coord(X,Y,Z);
    Vc(2).SetCoord(X,Y,Z);
    P2.Coord(X,Y,Z);
    Vc(3).SetCoord(X,Y,Z);
    Prs3d_Root::CurrentGroup(aPresentation)->Polyline(Vc);
  }
  else {
    
    //==== PROCESSING OF FACE ===========
    Prs3d_Root::NewGroup(aPresentation);
    Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
    PointMin.Coord(X,Y,Z);
    V(1).SetCoord(X,Y,Z);
    PointMax.Coord(X,Y,Z);
    V(2).SetCoord(X,Y,Z);
    Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);

    //==== PROCESSING OF CALL 1 =====
    Prs3d_Root::NewGroup(aPresentation);
    Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
    AttachmentPoint1.Coord(X,Y,Z);
    V(1).SetCoord(X,Y,Z);
    P1.Coord(X,Y,Z);
    V(2).SetCoord(X,Y,Z);
    Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);

    //==== PROCESSING OF CALL 2 =====
    Prs3d_Root::NewGroup(aPresentation);
    Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
    AttachmentPoint2.Coord(X,Y,Z);
    V(1).SetCoord(X,Y,Z);
    P2.Coord(X,Y,Z);
    V(2).SetCoord(X,Y,Z);
    Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);
  }

  //==== ARROWS ================
  Prs3d_Root::NewGroup(aPresentation);
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  
  if (dist < (LA->Arrow1Aspect()->Length()+LA->Arrow2Aspect()->Length())) outside = Standard_True;
  gp_Dir arrdir = L3.Direction().Reversed();
  if (outside) arrdir.Reverse();
  // arrow 1 ----
  Prs3d_Arrow::Draw(aPresentation,P1,arrdir,
		    LA->Arrow1Aspect()->Angle(),
		    LA->Arrow1Aspect()->Length());
  
  // arrow 2 ----
  Prs3d_Arrow::Draw(aPresentation,P2,arrdir.Reversed(),
		    LA->Arrow2Aspect()->Angle(),
		    LA->Arrow2Aspect()->Length());

  //-------------------------------------------------------------------------------------
  //|                                SYMBOL OF SYMMETRY                                 |
  //-------------------------------------------------------------------------------------

  //           -------    : Superior Segment 
  //         -----------  : Axis
  //           -------    : Inferior Segment 
  
  gp_Vec Vvar(P1,P2);
  gp_Vec vec;
  gp_Vec Vtmp = Vvar.Divided(Vvar.Magnitude()).Multiplied((aAxis.Distance(AttachmentPoint1)+
							   aAxis.Distance(AttachmentPoint2)));
  vec.SetCoord(Vtmp.X(),Vtmp.Y(),Vtmp.Z());
  gp_Vec vecA = vec.Multiplied(.1);

  gp_Dir DirAxis = aAxis.Direction();
  gp_Vec Vaxe(DirAxis);
  gp_Vec vecB = Vaxe.Multiplied(vecA.Magnitude());
  vecB.Multiply(.5);

  gp_Pnt pm,pOff;
  if (VLa.Dot(VL1) == 0) {
    gp_Vec Vper(P1,ElCLib::Value(ElCLib::Parameter(aAxis,P1),aAxis));
    pm = P1.Translated(Vper);
  }
  else {
    pm = P1.Translated(Vvar.Multiplied(.5));
  }
  //pm = P1.Translated(Vvar.Multiplied(.5));
  pOff = OffsetPoint.Translated(vecB);
  
  //Calculate the extremities of the symbol axis
  gp_Vec vecAxe = vecA.Multiplied(.7);
  pOff.Translated(vecAxe).Coord(X,Y,Z);
  V(1).SetCoord(X,Y,Z);
  pOff.Translated(vecAxe.Reversed()).Coord(X,Y,Z);
  V(2).SetCoord(X,Y,Z);
  Prs3d_Root::NewGroup(aPresentation);
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);

  //Calculate the extremities of the superior segment of the symbol
  gp_Vec vec1 = vecAxe.Multiplied(.6);
  vecAxe = Vaxe.Multiplied(vecAxe.Magnitude());
  gp_Vec vec2 = vecAxe.Multiplied(.4);

  pOff.Translated(vec1.Added(vec2)).Coord(X,Y,Z);
  V(1).SetCoord(X,Y,Z);
  pOff.Translated(vec1.Reversed().Added(vec2)).Coord(X,Y,Z);
  V(2).SetCoord(X,Y,Z);
  Prs3d_Root::NewGroup(aPresentation);
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);

  //Calculate the extremities of the inferior segment of the symbol
  pOff.Translated(vec1.Added(vec2.Reversed())).Coord(X,Y,Z);
  V(1).SetCoord(X,Y,Z);
  pOff.Translated(vec1.Reversed().Added(vec2.Reversed())).Coord(X,Y,Z);
  V(2).SetCoord(X,Y,Z);
  Prs3d_Root::NewGroup(aPresentation);
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);
  
/*//--------------------------------------------------------------------------------------
  //|                          MARKING OF THE SYMMETRY AXIS                             |
  //--------------------------------------------------------------------------------------     
  //        ____
  //        \  / :Cursor
  //         \/
  //         /\
  //        /__\
*/

  Graphic3d_Array1OfVertex cursor(1,5);
  Standard_Real Dist = (aAxis.Distance(AttachmentPoint1)+
			aAxis.Distance(AttachmentPoint2))/75;
  gp_Vec vs(aDirectionAxis);
  gp_Vec vsym(vs.Divided(vs.Magnitude()).Multiplied(Dist).XYZ());
  
  gp_Vec vsymper(vsym.Y(),-vsym.X(),vsym.Z());
  
  gp_Pnt pm1 = pm.Translated(vsym.Added(vsymper));
  cursor(1).SetCoord(pm1.X(),pm1.Y(),pm1.Z());
  pm1 = pm1.Translated(vsym.Reversed().Multiplied(2).Added(vsymper.Reversed().Multiplied(2)));
  cursor(2).SetCoord(pm1.X(),pm1.Y(),pm1.Z());
  pm1 = pm1.Translated(vsymper.Multiplied(2));
  cursor(3).SetCoord(pm1.X(),pm1.Y(),pm1.Z());
  pm1 = pm1.Translated(vsym.Multiplied(2).Added(vsymper.Reversed().Multiplied(2)));
  cursor(4).SetCoord(pm1.X(),pm1.Y(),pm1.Z());
  pm1 = pm1.Translated(vsymper.Multiplied(2));
  cursor(5).SetCoord(pm1.X(),pm1.Y(),pm1.Z());

  Prs3d_Root::NewGroup(aPresentation);
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(cursor);

  vsym.Multiply(4);
  V(1).SetCoord(pm.Translated(vsym).X(),
		pm.Translated(vsym).Y(),
		pm.Translated(vsym).Z());
  V(2).SetCoord(pm.Translated(vsym.Reversed()).X(),
		pm.Translated(vsym.Reversed()).Y(),
		pm.Translated(vsym.Reversed()).Z());

  Prs3d_Root::NewGroup(aPresentation);
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);  
}
  
//===================================================================
//Function:Add
//Purpose: draws the representation of an axial symmetry between two arcs.
//===================================================================
void DsgPrs_SymmetricPresentation::Add (const Handle(Prs3d_Presentation)& aPresentation,
					const Handle(Prs3d_Drawer)& aDrawer,	
					const gp_Pnt&  AttachmentPoint1,
					const gp_Pnt&  AttachmentPoint2,
					const gp_Circ& aCircle1,
					const gp_Lin&  aAxis,
					const gp_Pnt&  OffsetPoint)
{
  Handle(Prs3d_LengthAspect) LA = aDrawer->LengthAspect();
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());  
  gp_Pnt OffsetPnt(OffsetPoint.X(),OffsetPoint.Y(),OffsetPoint.Z());
  gp_Pnt Center1 = aCircle1.Location();
  gp_Pnt ProjOffsetPoint = ElCLib::Value(ElCLib::Parameter(aAxis,OffsetPnt),aAxis);
  gp_Pnt ProjCenter1     = ElCLib::Value(ElCLib::Parameter(aAxis,Center1),aAxis);
  gp_Vec Vp(ProjCenter1,Center1);
  if (Vp.Magnitude() <= Precision::Confusion()) Vp = gp_Vec(aAxis.Direction())^aCircle1.Position().Direction();


  Standard_Real Dt,R,h;
  Dt = ProjCenter1.Distance(ProjOffsetPoint);
  R  = aCircle1.Radius();
  if (Dt > .999*R) {
    Dt = .999*R;
    gp_Vec Vout(ProjCenter1,ProjOffsetPoint);
    ProjOffsetPoint = ProjCenter1.Translated(Vout.Divided(Vout.Magnitude()).Multiplied(Dt));
    OffsetPnt = ProjOffsetPoint;
  }
  h  = Sqrt(R*R - Dt*Dt);
  gp_Pnt P1 = ProjOffsetPoint.Translated(Vp.Added(Vp.Divided(Vp.Magnitude()).Multiplied(h)));
  gp_Vec v(P1,ProjOffsetPoint);
  gp_Pnt P2 = ProjOffsetPoint.Translated(v);

  gp_Lin L3 = gce_MakeLin(P1,P2);
  Standard_Real parmin,parmax,parcur;
  parmin = ElCLib::Parameter(L3,P1);
  parmax = parmin;
  parcur = ElCLib::Parameter(L3,P2);
  Standard_Real dist = Abs(parmin-parcur);
  if (parcur < parmin) parmin = parcur;
  if (parcur > parmax) parmax = parcur;
  parcur = ElCLib::Parameter(L3,OffsetPnt);
#ifdef DEB
  gp_Pnt offp =
#endif
                ElCLib::Value(parcur,L3);

  Standard_Boolean outside = Standard_False;
  if (parcur < parmin) {
    parmin = parcur;
    outside = Standard_True;
  }
  if (parcur > parmax) {
    parmax = parcur;
    outside = Standard_True;
  }
  gp_Pnt PointMin = ElCLib::Value(parmin,L3);
  gp_Pnt PointMax = ElCLib::Value(parmax,L3);
  Graphic3d_Array1OfVertex V(1,2);
  Quantity_Length X,Y,Z;
    
  //==== PROCESSING OF FACE ===========
  Prs3d_Root::NewGroup(aPresentation);
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  PointMin.Coord(X,Y,Z);
  V(1).SetCoord(X,Y,Z);
  PointMax.Coord(X,Y,Z);
  V(2).SetCoord(X,Y,Z);
  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);

  //==== PROCESSING OF CALL 1 =====
  Standard_Integer nbp = 10;
  Prs3d_Root::NewGroup(aPresentation);
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect()); 
  Standard_Real ParamP1       = ElCLib::Parameter(aCircle1,P1);
  Standard_Real ParamPAttach1 = ElCLib::Parameter(aCircle1,AttachmentPoint1);
//  gp_Vec Center1_PAttach1(Center1,AttachmentPoint1);
//  gp_Vec Center1_P1(Center1,P1);
  Standard_Real alpha,Dalpha,alphaIter;

  alpha = fabs(ParamP1 - ParamPAttach1);
  if(ParamP1 < ParamPAttach1){
    if(alpha > M_PI){
      alpha  = (2*M_PI) - alpha;
      nbp    = (Standard_Integer ) IntegerPart(alpha/(alpha*.02));
      Dalpha = alpha/(nbp - 1);
    }
    else{
      nbp    = (Standard_Integer ) IntegerPart(alpha/(alpha*.02));
      Dalpha = -alpha/(nbp - 1);
    }
  }
  else{
    if(alpha > M_PI){
      alpha  = (2*M_PI) - alpha;
      nbp    = (Standard_Integer ) IntegerPart(alpha/(alpha*.02));
      Dalpha = -alpha/(nbp - 1);
    }
    else{
      nbp    = (Standard_Integer ) IntegerPart(alpha/(alpha*.02));
      Dalpha = alpha/(nbp - 1);
    }
  }

  Graphic3d_Array1OfVertex Vc1(1,nbp);
  AttachmentPoint1.Coord(X,Y,Z);
  Vc1(1).SetCoord(X,Y,Z);
  alphaIter = Dalpha;
  gp_Pnt PntIter;
  Standard_Integer i ;
  for( i = 2; i <= nbp; i++){    
    PntIter = ElCLib::Value(ParamPAttach1 + alphaIter,aCircle1);
    alphaIter = alphaIter + Dalpha;
    PntIter.Coord(X,Y,Z);
    Vc1(i).SetCoord(X,Y,Z);
    
  }
  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(Vc1);
  
  //==== PROCESSING OF CALL 2 =====
  Prs3d_Root::NewGroup(aPresentation);
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  gp_Pnt Center2 = ProjCenter1.Translated(Vp.Reversed());
//  gp_Vec Center2_PAttach2(Center2,AttachmentPoint2);
//  gp_Vec Center2_P2(Center2,P2);
  
  gp_Dir DirC2 = aCircle1.Axis().Direction();
  gp_Ax2 AxeC2(Center2,DirC2);
  gp_Circ aCircle2(AxeC2,aCircle1.Radius());
  Standard_Real ParamP2       = ElCLib::Parameter(aCircle2,P2);
  Standard_Real ParamPAttach2 = ElCLib::Parameter(aCircle2,AttachmentPoint2);

  alpha = fabs(ParamP2 - ParamPAttach2);
  if (alpha <= Precision::Confusion()) alpha = 1.e-5;
  if(ParamP2 < ParamPAttach2){
    if(alpha > M_PI){
      alpha  = (2*M_PI) - alpha;
      nbp    = (Standard_Integer ) IntegerPart(alpha/(alpha*.02));
      Dalpha = alpha/(nbp - 1);
    }
    else{
      nbp    = (Standard_Integer ) IntegerPart(alpha/(alpha*.02));
      Dalpha = -alpha/(nbp - 1);
    }
  }
  else{
    if(alpha > M_PI){
      alpha  = (2*M_PI) - alpha;
      nbp    = (Standard_Integer ) IntegerPart(alpha/(alpha*.02));
      Dalpha = -alpha/(nbp - 1);
    }
    else{
      nbp    = (Standard_Integer ) IntegerPart(alpha/(alpha*.02));
      Dalpha = alpha/(nbp - 1);
    }
  }

  Graphic3d_Array1OfVertex Vc2(1,nbp);
  AttachmentPoint2.Coord(X,Y,Z);
  Vc2(1).SetCoord(X,Y,Z);
  alphaIter = Dalpha;
  for(i = 2; i <= nbp; i++){
    PntIter = ElCLib::Value(ParamPAttach2 + alphaIter,aCircle2);
    alphaIter = alphaIter + Dalpha;
    PntIter.Coord(X,Y,Z);
    Vc2(i).SetCoord(X,Y,Z);
  }
  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(Vc2);
  
  //==== ARROWS ================
  Prs3d_Root::NewGroup(aPresentation);
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  
  if (dist < (LA->Arrow1Aspect()->Length()+LA->Arrow2Aspect()->Length())) outside = Standard_True;
  gp_Dir arrdir = L3.Direction().Reversed();
  if (outside) arrdir.Reverse();
  // arrow 1 ----
  Prs3d_Arrow::Draw(aPresentation,P1,arrdir,
		    LA->Arrow1Aspect()->Angle(),
		    LA->Arrow1Aspect()->Length());
  
  // arrow 2 ----
  Prs3d_Arrow::Draw(aPresentation,P2,arrdir.Reversed(),
		    LA->Arrow2Aspect()->Angle(),
		    LA->Arrow2Aspect()->Length());

  //-------------------------------------------------------------------------------------
  //|                                SYMBOL OF SYMMETRY                                 |
  //-------------------------------------------------------------------------------------

  //           -------    : Superior Segment
  //         -----------  : Axis
  //           -------    : Inferior Segment 
  
  gp_Vec Vvar(P1,P2);
  gp_Vec vec;
  gp_Vec Vtmp = Vvar.Divided(Vvar.Magnitude()).Multiplied(2*(aAxis.Distance(Center1)));
  vec.SetCoord(Vtmp.X(),Vtmp.Y(),Vtmp.Z());
  gp_Vec vecA = vec.Multiplied(.1);

  gp_Dir DirAxis = aAxis.Direction();
  gp_Vec Vaxe(DirAxis);
  gp_Vec vecB = Vaxe.Multiplied(vecA.Magnitude());
  vecB.Multiply(.5);

  //gp_Vec vecB(-vec.Y(),vec.X(),vec.Z());
  //vecB.Multiply(.05);
  gp_Pnt pm,pOff;
  pm = P1.Translated(Vvar.Multiplied(.5));
  pOff = OffsetPnt.Translated(vecB);

  //Calculation of extremas of the axis of the symbol
  gp_Vec vecAxe = vecA.Multiplied(.7);
  pOff.Translated(vecAxe).Coord(X,Y,Z);
  V(1).SetCoord(X,Y,Z);
  pOff.Translated(vecAxe.Reversed()).Coord(X,Y,Z);
  V(2).SetCoord(X,Y,Z);
  Prs3d_Root::NewGroup(aPresentation);
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);

  //Calculation of extremas of the superior segment of the symbol
  gp_Vec vec1 = vecAxe.Multiplied(.6);

  vecAxe = Vaxe.Multiplied(vecAxe.Magnitude());
  //vecAxe.SetCoord(-vecAxe.Y(),vecAxe.X(),vecAxe.Z());

  gp_Vec vec2 = vecAxe.Multiplied(.4);

  pOff.Translated(vec1.Added(vec2)).Coord(X,Y,Z);
  V(1).SetCoord(X,Y,Z);
  pOff.Translated(vec1.Reversed().Added(vec2)).Coord(X,Y,Z);
  V(2).SetCoord(X,Y,Z);
  Prs3d_Root::NewGroup(aPresentation);
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);

  //Calculation of extremas of the inferior segment of the symbol
  pOff.Translated(vec1.Added(vec2.Reversed())).Coord(X,Y,Z);
  V(1).SetCoord(X,Y,Z);
  pOff.Translated(vec1.Reversed().Added(vec2.Reversed())).Coord(X,Y,Z);
  V(2).SetCoord(X,Y,Z);
  Prs3d_Root::NewGroup(aPresentation);
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);
  
/*//--------------------------------------------------------------------------------------
  //|                          MARKING OF THE AXIS OF SYMMETRY                           |
  //--------------------------------------------------------------------------------------     
  //        ____
  //        \  / :Cursor
  //         \/
  //         /\
  //        /__\
*/

  Graphic3d_Array1OfVertex cursor(1,5);
  //Standard_Real Dist = aCircle1.Radius()/37;
  Standard_Real Dist = aAxis.Distance(Center1)/37;
  gp_Dir aDirectionAxis = aAxis.Direction();
  gp_Vec vs(aDirectionAxis);
  gp_Vec vsym(vs.Divided(vs.Magnitude()).Multiplied(Dist).XYZ());
  
  gp_Vec vsymper(vsym.Y(),-vsym.X(),vsym.Z());
  gp_Pnt pm1 = pm.Translated(vsym.Added(vsymper));
  cursor(1).SetCoord(pm1.X(),pm1.Y(),pm1.Z());
  pm1 = pm1.Translated(vsym.Reversed().Multiplied(2).Added(vsymper.Reversed().Multiplied(2)));
  cursor(2).SetCoord(pm1.X(),pm1.Y(),pm1.Z());
  pm1 = pm1.Translated(vsymper.Multiplied(2));
  cursor(3).SetCoord(pm1.X(),pm1.Y(),pm1.Z());
  pm1 = pm1.Translated(vsym.Multiplied(2).Added(vsymper.Reversed().Multiplied(2)));
  cursor(4).SetCoord(pm1.X(),pm1.Y(),pm1.Z());
  pm1 = pm1.Translated(vsymper.Multiplied(2));
  cursor(5).SetCoord(pm1.X(),pm1.Y(),pm1.Z());

  Prs3d_Root::NewGroup(aPresentation);
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(cursor);

  vsym.Multiply(4);
  V(1).SetCoord(pm.Translated(vsym).X(),
		pm.Translated(vsym).Y(),
		pm.Translated(vsym).Z());
  V(2).SetCoord(pm.Translated(vsym.Reversed()).X(),
		pm.Translated(vsym.Reversed()).Y(),
		pm.Translated(vsym.Reversed()).Z());

  Prs3d_Root::NewGroup(aPresentation);
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);  
}
  
//===================================================================
//Function:Add
//Purpose: draws the representation of an axial symmetry between two vertex.
//===================================================================
void DsgPrs_SymmetricPresentation::Add (const Handle(Prs3d_Presentation)& aPresentation,
					const Handle(Prs3d_Drawer)& aDrawer,	
					const gp_Pnt&  AttachmentPoint1,
					const gp_Pnt&  AttachmentPoint2,
					const gp_Lin&  aAxis,
					const gp_Pnt&  OffsetPoint)
{
  Handle(Prs3d_LengthAspect) LA = aDrawer->LengthAspect();
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());  

  if (AttachmentPoint1.IsEqual(AttachmentPoint2,Precision::Confusion())){
    //==============================================================
    //  SYMMETRY WHEN THE REFERENCE POINT IS ON THE AXIS OF SYM.:
    //==============================================================
    //Marker of localisation of the face
    Handle(Graphic3d_AspectMarker3d) MarkerAsp = new Graphic3d_AspectMarker3d();
    MarkerAsp->SetType(Aspect_TOM_BALL);
    MarkerAsp->SetScale(0.8);
    Quantity_Color acolor;
    Aspect_TypeOfLine atype;
    Standard_Real awidth;
    LA->LineAspect()->Aspect()->Values(acolor, atype, awidth);
    MarkerAsp->SetColor(acolor);
    Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(MarkerAsp);
    Graphic3d_Vertex V3d(AttachmentPoint1.X(),
			 AttachmentPoint1.Y(), 
			 AttachmentPoint1.Z());
    Prs3d_Root::CurrentGroup(aPresentation)->Marker(V3d);

    //Trace of the linking segment 
    Prs3d_Root::NewGroup(aPresentation);
    Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
    Graphic3d_Array1OfVertex Vrac(1,2);
    Vrac(1).SetCoord(AttachmentPoint1.X(),
		     AttachmentPoint1.Y(), 
		     AttachmentPoint1.Z());
    Vrac(2).SetCoord(OffsetPoint.X(),
		     OffsetPoint.Y(),
		     OffsetPoint.Z());
    Prs3d_Root::CurrentGroup(aPresentation)->Polyline(Vrac);
  
    //--------------------------------------------------------------------------------------
    //|                                SYMBOL OF SYMMETRY                                 |
    //--------------------------------------------------------------------------------------
    //           -------    : Superior Segment 
    //         -----------  : Axis
    //           -------    : Inferior Segment 

    //Calculate extremas of the axis of the symbol
    gp_Vec VAO (AttachmentPoint1,OffsetPoint);
    gp_Vec uVAO  = VAO.Divided(VAO.Magnitude());
    gp_Pnt pDaxe = OffsetPoint.Translated(uVAO.Multiplied(3.));
    gp_Pnt pFaxe = pDaxe.Translated(uVAO.Multiplied(12.));
    Vrac(1).SetCoord(pDaxe.X(),pDaxe.Y(),pDaxe.Z());
    Vrac(2).SetCoord(pFaxe.X(),pFaxe.Y(),pFaxe.Z());
    Prs3d_Root::NewGroup(aPresentation);
    Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
    Prs3d_Root::CurrentGroup(aPresentation)->Polyline(Vrac);

    //Calculate extremas of the superior segment of the symbol
    gp_Vec nVAO  (-uVAO.Y(),uVAO.X(),uVAO.Z());
    gp_Pnt sgP11 = pDaxe.Translated(uVAO.Multiplied(2.).Added(nVAO.Multiplied(2.)));
    gp_Pnt sgP12 = sgP11.Translated(uVAO.Multiplied(8.));
    Vrac(1).SetCoord(sgP11.X(),sgP11.Y(),sgP11.Z());
    Vrac(2).SetCoord(sgP12.X(),sgP12.Y(),sgP12.Z());
    Prs3d_Root::NewGroup(aPresentation);
    Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
    Prs3d_Root::CurrentGroup(aPresentation)->Polyline(Vrac);

    //Calculate extremas of the inferior segment of the symbol
    gp_Vec nVAOr = nVAO.Reversed();
    gp_Pnt sgP21 = pDaxe.Translated(uVAO.Multiplied(2.).Added(nVAOr.Multiplied(2.)));
    gp_Pnt sgP22 = sgP21.Translated(uVAO.Multiplied(8.));
    Vrac(1).SetCoord(sgP21.X(),sgP21.Y(),sgP21.Z());
    Vrac(2).SetCoord(sgP22.X(),sgP22.Y(),sgP22.Z());
    Prs3d_Root::NewGroup(aPresentation);
    Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
    Prs3d_Root::CurrentGroup(aPresentation)->Polyline(Vrac);
  }
  //==============================================================
  //  OTHER CASES                                                 :
  //==============================================================

  else{
    gp_Pnt ProjOffsetPoint      = ElCLib::Value(ElCLib::Parameter(aAxis,OffsetPoint),aAxis);
    gp_Pnt ProjAttachmentPoint1 = ElCLib::Value(ElCLib::Parameter(aAxis,AttachmentPoint1),aAxis);
    gp_Vec PjAtt1_Att1(ProjAttachmentPoint1,AttachmentPoint1);
    gp_Pnt P1 = ProjOffsetPoint.Translated(PjAtt1_Att1);
    gp_Pnt P2 = ProjOffsetPoint.Translated(PjAtt1_Att1.Reversed());
        
    gp_Lin L3 = gce_MakeLin(P1,P2);
    Standard_Real parmin,parmax,parcur;
    parmin = ElCLib::Parameter(L3,P1);
    parmax = parmin;
    parcur = ElCLib::Parameter(L3,P2);
    Standard_Real dist = Abs(parmin-parcur);
    if (parcur < parmin) parmin = parcur;
    if (parcur > parmax) parmax = parcur;
    parcur = ElCLib::Parameter(L3,OffsetPoint);
#ifdef DEB
    gp_Pnt offp =
#endif
                  ElCLib::Value(parcur,L3);
    
    Standard_Boolean outside = Standard_False;
    if (parcur < parmin) {
      parmin = parcur;
      outside = Standard_True;
    }
    if (parcur > parmax) {
      parmax = parcur;
      outside = Standard_True;
    }
    gp_Pnt PointMin = ElCLib::Value(parmin,L3);
    gp_Pnt PointMax = ElCLib::Value(parmax,L3);
    Graphic3d_Array1OfVertex V(1,2);
    Quantity_Length X,Y,Z;
    
    //==== PROCESSING OF FACE ===========
    Prs3d_Root::NewGroup(aPresentation);
    Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
    PointMin.Coord(X,Y,Z);
    V(1).SetCoord(X,Y,Z);
    PointMax.Coord(X,Y,Z);
    V(2).SetCoord(X,Y,Z);
    Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);
    
    //==== PROCESSING OF CALL 1 =====
    Prs3d_Root::NewGroup(aPresentation);
    Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect()); 
    AttachmentPoint1.Coord(X,Y,Z);
    V(1).SetCoord(X,Y,Z);
    P1.Coord(X,Y,Z);
    V(2).SetCoord(X,Y,Z);
    Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);
    
    //==== PROCESSING OF CALL 2 =====
    Prs3d_Root::NewGroup(aPresentation);
    Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect()); 
    AttachmentPoint2.Coord(X,Y,Z);
    V(1).SetCoord(X,Y,Z);
    P2.Coord(X,Y,Z);
    V(2).SetCoord(X,Y,Z);
    Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);
 
    //==== ARROWS ================
    Prs3d_Root::NewGroup(aPresentation);
    Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
    
    if (dist < (LA->Arrow1Aspect()->Length()+LA->Arrow2Aspect()->Length())) outside = Standard_True;
    gp_Dir arrdir = L3.Direction().Reversed();
    if (outside) arrdir.Reverse();
    // arrow 1 ----
    Prs3d_Arrow::Draw(aPresentation,P1,arrdir,
		      LA->Arrow1Aspect()->Angle(),
		      LA->Arrow1Aspect()->Length());
  
    // arrow 2 ----
    Prs3d_Arrow::Draw(aPresentation,P2,arrdir.Reversed(),
		      LA->Arrow2Aspect()->Angle(),
		      LA->Arrow2Aspect()->Length());
    
    //==== POINTS ================
    //Marker of localization of attachment points:
    Prs3d_Root::NewGroup(aPresentation);
    Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
    Handle(Graphic3d_AspectMarker3d) MarkerAspAtt = new Graphic3d_AspectMarker3d();
    MarkerAspAtt->SetType(Aspect_TOM_BALL);
    MarkerAspAtt->SetScale(0.8);
    Quantity_Color color;
    Aspect_TypeOfLine type;
    Standard_Real width;
    LA->LineAspect()->Aspect()->Values(color, type, width);
    MarkerAspAtt->SetColor(color);
    Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(MarkerAspAtt);
    Graphic3d_Vertex Vatt1(AttachmentPoint1.X(),
			   AttachmentPoint1.Y(), 
			   AttachmentPoint1.Z());
    Prs3d_Root::CurrentGroup(aPresentation)->Marker(Vatt1);  

    Prs3d_Root::NewGroup(aPresentation);
    Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
    Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(MarkerAspAtt);
    Graphic3d_Vertex Vatt2(AttachmentPoint2.X(),
			   AttachmentPoint2.Y(), 
			   AttachmentPoint2.Z());
    Prs3d_Root::CurrentGroup(aPresentation)->Marker(Vatt2);    
      
    //-------------------------------------------------------------------------------------
    //|                                SYMBOL OF SYMMETRY                                 |
    //-------------------------------------------------------------------------------------
    
    //           -------    : Superior Segment 
    //         -----------  : Axis
    //           -------    : Inferior Segment
    
    gp_Vec vec(P1,P2);
    gp_Vec vecA = vec.Multiplied(.1);

    gp_Dir DirAxis = aAxis.Direction();
    gp_Vec Vaxe(DirAxis);
    gp_Vec vecB = Vaxe.Multiplied(vecA.Magnitude());
    vecB.Multiply(.5);

    //gp_Vec vecB(-vec.Y(),vec.X(),vec.Z());
    //vecB.Multiply(.05);

    gp_Pnt pm,pOff;
    pm = P1.Translated(vec.Multiplied(.5));
    pOff = OffsetPoint.Translated(vecB);
    
    //Calculate the extremas of the axis of the symbol
    gp_Vec vecAxe = vecA.Multiplied(.7);
    pOff.Translated(vecAxe).Coord(X,Y,Z);
    V(1).SetCoord(X,Y,Z);
    pOff.Translated(vecAxe.Reversed()).Coord(X,Y,Z);
    V(2).SetCoord(X,Y,Z);
    Prs3d_Root::NewGroup(aPresentation);
    Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
    Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);
    
    //Calculate the extremas of the superior segment of the symbol
    gp_Vec vec1 = vecAxe.Multiplied(.6);

    vecAxe = Vaxe.Multiplied(vecAxe.Magnitude());

    //vecAxe.SetCoord(-vecAxe.Y(),vecAxe.X(),vecAxe.Z());
    gp_Vec vec2 = vecAxe.Multiplied(.4);
    
    pOff.Translated(vec1.Added(vec2)).Coord(X,Y,Z);
    V(1).SetCoord(X,Y,Z);
    pOff.Translated(vec1.Reversed().Added(vec2)).Coord(X,Y,Z);
    V(2).SetCoord(X,Y,Z);
    Prs3d_Root::NewGroup(aPresentation);
    Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
    Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);
    
    //Calculate the extremas of the inferior segment of the symbol
    pOff.Translated(vec1.Added(vec2.Reversed())).Coord(X,Y,Z);
    V(1).SetCoord(X,Y,Z);
    pOff.Translated(vec1.Reversed().Added(vec2.Reversed())).Coord(X,Y,Z);
    V(2).SetCoord(X,Y,Z);
    Prs3d_Root::NewGroup(aPresentation);
    Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
    Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);
    
/*  //--------------------------------------------------------------------------------------
    //|                          MARKING OF THE AXIS OF SYMMETRY                           |
    //--------------------------------------------------------------------------------------     
    //        ____
    //        \  / :Cursor
    //         \/
    //         /\
    //        /__\
*/
    
    Graphic3d_Array1OfVertex cursor(1,5);
    Standard_Real Dist = P1.Distance(P2)/75;
    gp_Dir aDirectionAxis = aAxis.Direction();
    gp_Vec vs(aDirectionAxis);
    gp_Vec vsym(vs.Divided(vs.Magnitude()).Multiplied(Dist).XYZ());
    
    gp_Vec vsymper(vsym.Y(),-vsym.X(),vsym.Z());
    gp_Pnt pm1 = pm.Translated(vsym.Added(vsymper));
    cursor(1).SetCoord(pm1.X(),pm1.Y(),pm1.Z());
    pm1 = pm1.Translated(vsym.Reversed().Multiplied(2).Added(vsymper.Reversed().Multiplied(2)));
    cursor(2).SetCoord(pm1.X(),pm1.Y(),pm1.Z());
    pm1 = pm1.Translated(vsymper.Multiplied(2));
    cursor(3).SetCoord(pm1.X(),pm1.Y(),pm1.Z());
    pm1 = pm1.Translated(vsym.Multiplied(2).Added(vsymper.Reversed().Multiplied(2)));
    cursor(4).SetCoord(pm1.X(),pm1.Y(),pm1.Z());
    pm1 = pm1.Translated(vsymper.Multiplied(2));
    cursor(5).SetCoord(pm1.X(),pm1.Y(),pm1.Z());
    
    Prs3d_Root::NewGroup(aPresentation);
    Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
    Prs3d_Root::CurrentGroup(aPresentation)->Polyline(cursor);
    
    vsym.Multiply(4);
    V(1).SetCoord(pm.Translated(vsym).X(),
		  pm.Translated(vsym).Y(),
		  pm.Translated(vsym).Z());
    V(2).SetCoord(pm.Translated(vsym.Reversed()).X(),
		  pm.Translated(vsym.Reversed()).Y(),
		  pm.Translated(vsym.Reversed()).Z());
    
    Prs3d_Root::NewGroup(aPresentation);
    Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
    Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V); 
  } 
}









