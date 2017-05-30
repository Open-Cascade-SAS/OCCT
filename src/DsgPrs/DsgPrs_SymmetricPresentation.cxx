// Created on: 1997-01-22
// Created by: Prestataire Michael ALEONARD
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


#include <DsgPrs_SymmetricPresentation.hxx>
#include <ElCLib.hxx>
#include <gce_MakeDir.hxx>
#include <gce_MakeLin.hxx>
#include <Geom2d_Line.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <GeomAPI.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>
#include <gp_Dir.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <Graphic3d_ArrayOfPoints.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_Vertex.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <Precision.hxx>
#include <Prs3d_Arrow.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <Prs3d_DimensionAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <TCollection_AsciiString.hxx>

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
  Handle(Prs3d_DimensionAspect) LA = aDrawer->DimensionAspect();
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());

  gp_Pnt ProjOffsetPoint = ElCLib::Value(ElCLib::Parameter(aAxis,OffsetPoint),aAxis);
  gp_Pnt PjAttachPnt1    = ElCLib::Value(ElCLib::Parameter(aAxis,AttachmentPoint1),aAxis);
  gp_Dir aDirectionAxis  = aAxis.Direction();
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

  Standard_Real X,Y,Z;
  Standard_Real D1(aAxis.Distance(AttachmentPoint1)),coeff(.5);
  gp_Pnt pint,Pj_P1,P1Previous = P1;
  
  /*=======================================================
   TO AVOID CROSSING
          P1  -=- P2                P2  -=- P1         
            \<-->/                    |<-->|
             \  /                     |    |
              \/                      |    | 
              /\                      |    |
             /  \                     |    |
   Pattach2 /____\ Pattach1 Pattach2 /______\ Pattach1
           /  NO \                  /   YES  \	
  =======================================================*/

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
  /*===================================
   FRACTURES OF TRAITS OF CALL    
          /             \		
         /               \	
         |      -=-      |
         |<------------->| 
  ===================================*/

  gp_Vec        Vfix;
  Standard_Real alpha,b;

  if(aAxis.Distance(P1) > D1*(1 + coeff) && !Cross){

    //==== PROCESSING OF FACE ===========
    Prs3d_Root::NewGroup(aPresentation);
    Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());

    Pj_P1 = ElCLib::Value(ElCLib::Parameter(aAxis,P1),aAxis);
    gp_Vec Vp(Pj_P1,P1);
    Vfix = Vp.Divided(Vp.Magnitude()).Multiplied(D1*(1 + coeff));
    P1 = Pj_P1.Translated(Vfix);
    P2 = Pj_P1.Translated(Vfix.Reversed());

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

    Handle(Graphic3d_ArrayOfPolylines) aPrims = new Graphic3d_ArrayOfPolylines(8,3);

    aPrims->AddBound(2);
    aPrims->AddVertex(PointMin);
    aPrims->AddVertex(PointMax);
    
    //==== PROCESSING OF CALL 1 =====
    alpha = aDirectionAxis.Angle(aDirection1);
    b = (coeff*D1)/sin(alpha);
    gp_Vec Vpint(AttachmentPoint1,P1Previous);
    pint = AttachmentPoint1.Translated(Vpint.Divided(Vpint.Magnitude()).Multiplied(b));

    aPrims->AddBound(3);
    aPrims->AddVertex(AttachmentPoint1);
    aPrims->AddVertex(pint);
    aPrims->AddVertex(P1);

    //==== PROCESSING OF CALL 2 =====
    gp_Pnt Pj_pint  = ElCLib::Value(ElCLib::Parameter(aAxis,pint),aAxis);
    gp_Vec V_int(pint, Pj_pint);
    gp_Pnt Sym_pint = Pj_pint.Translated(V_int);

    aPrims->AddBound(3);
    aPrims->AddVertex(AttachmentPoint2);
    aPrims->AddVertex(Sym_pint);
    aPrims->AddVertex(P2);

	Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims);
  }

  /*===================================
   FRACTURES OF PROCESSING OF CALL    
                -=-    
           |<--------->| 
           |           |   
          /             \	
         /               \	 
  ===================================*/
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

    Handle(Graphic3d_ArrayOfPolylines) aPrims = new Graphic3d_ArrayOfPolylines(8,3);

    aPrims->AddBound(2);
	aPrims->AddVertex(PointMin);
    aPrims->AddVertex(PointMax);
    
    //==== PROCESSING OF CALL 1 =====
    alpha = aDirectionAxis.Angle(aDirection1);
    b = (coeff*D1)/sin(alpha);
    gp_Vec Vpint(AttachmentPoint1,P1Previous);
    pint = AttachmentPoint1.Translated(Vpint.Divided(Vpint.Magnitude()).Multiplied(b));

    aPrims->AddBound(3);
    aPrims->AddVertex(AttachmentPoint1);
    aPrims->AddVertex(pint);
    aPrims->AddVertex(P1);
    
    //==== PROCESSING OF CALL 2 =====
    gp_Pnt Pj_pint  = ElCLib::Value(ElCLib::Parameter(aAxis,pint),aAxis);
    gp_Vec V_int(pint, Pj_pint);
    gp_Pnt Sym_pint = Pj_pint.Translated(V_int);

    aPrims->AddBound(3);
    aPrims->AddVertex(AttachmentPoint2);
    aPrims->AddVertex(Sym_pint);
    aPrims->AddVertex(P2);

	Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims);
  }
  else {
    
    //==== PROCESSING OF FACE ===========
    Prs3d_Root::NewGroup(aPresentation);
    Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());

    Handle(Graphic3d_ArrayOfSegments) aPrims = new Graphic3d_ArrayOfSegments(6);

	aPrims->AddVertex(PointMin);
    aPrims->AddVertex(PointMax);
    
    //==== PROCESSING OF CALL 1 =====
    aPrims->AddVertex(AttachmentPoint1);
    aPrims->AddVertex(P1);

    //==== PROCESSING OF CALL 2 =====
    aPrims->AddVertex(AttachmentPoint2);
    aPrims->AddVertex(P2);

	Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims);
  }

  //==== ARROWS ================
  Prs3d_Root::NewGroup(aPresentation);
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  
  if (dist < (LA->ArrowAspect()->Length()+LA->ArrowAspect()->Length())) outside = Standard_True;
  gp_Dir arrdir = L3.Direction().Reversed();
  if (outside) arrdir.Reverse();
  // arrow 1 ----
  Prs3d_Arrow::Draw (Prs3d_Root::CurrentGroup (aPresentation), P1, arrdir, LA->ArrowAspect()->Angle(), LA->ArrowAspect()->Length());
  
  // arrow 2 ----
  Prs3d_Arrow::Draw (Prs3d_Root::CurrentGroup (aPresentation), P2, arrdir.Reversed(), LA->ArrowAspect()->Angle(), LA->ArrowAspect()->Length());

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
  pOff = OffsetPoint.Translated(vecB);
  
  //Calculate the extremities of the symbol axis
  gp_Vec vecAxe = vecA.Multiplied(.7);

  Prs3d_Root::NewGroup(aPresentation);
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());

  Handle(Graphic3d_ArrayOfPolylines) aPrims = new Graphic3d_ArrayOfPolylines(13,5);

  aPrims->AddBound(2);
  aPrims->AddVertex(pOff.Translated(vecAxe));
  aPrims->AddVertex(pOff.Translated(vecAxe.Reversed()));

  //Calculate the extremities of the superior segment of the symbol
  gp_Vec vec1 = vecAxe.Multiplied(.6);
  vecAxe = Vaxe.Multiplied(vecAxe.Magnitude());
  gp_Vec vec2 = vecAxe.Multiplied(.4);

  aPrims->AddBound(2);
  aPrims->AddVertex(pOff.Translated(vec1.Added(vec2)));
  aPrims->AddVertex(pOff.Translated(vec1.Reversed().Added(vec2)));

  //Calculate the extremities of the inferior segment of the symbol
  aPrims->AddBound(2);
  aPrims->AddVertex(pOff.Translated(vec1.Added(vec2.Reversed())));
  aPrims->AddVertex(pOff.Translated(vec1.Reversed().Added(vec2.Reversed())));

  /*--------------------------------------------------------------------------------------
  |                          MARKING OF THE SYMMETRY AXIS                             |
  --------------------------------------------------------------------------------------     
          ____
          \  / :Cursor
           \/
           /\
          /__\
*/
  Standard_Real Dist = (aAxis.Distance(AttachmentPoint1)+aAxis.Distance(AttachmentPoint2))/75;
  gp_Vec vs(aDirectionAxis);
  gp_Vec vsym(vs.Divided(vs.Magnitude()).Multiplied(Dist).XYZ());
  gp_Vec vsymper(vsym.Y(),-vsym.X(),vsym.Z());
  
  aPrims->AddBound(5);
  gp_Pnt pm1 = pm.Translated(vsym.Added(vsymper));
  aPrims->AddVertex(pm1);
  pm1 = pm1.Translated(vsym.Reversed().Multiplied(2).Added(vsymper.Reversed().Multiplied(2)));
  aPrims->AddVertex(pm1);
  pm1 = pm1.Translated(vsymper.Multiplied(2));
  aPrims->AddVertex(pm1);
  pm1 = pm1.Translated(vsym.Multiplied(2).Added(vsymper.Reversed().Multiplied(2)));
  aPrims->AddVertex(pm1);
  pm1 = pm1.Translated(vsymper.Multiplied(2));
  aPrims->AddVertex(pm1);

  vsym.Multiply(4);

  aPrims->AddBound(2);
  aPrims->AddVertex(pm.Translated(vsym));
  aPrims->AddVertex(pm.Translated(vsym.Reversed()));

  Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims);
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
  Handle(Prs3d_DimensionAspect) LA = aDrawer->DimensionAspect();
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

  //==== PROCESSING OF FACE ===========
  Handle(Graphic3d_ArrayOfPrimitives) aPrims = new Graphic3d_ArrayOfSegments(2);
  aPrims->AddVertex(PointMin);
  aPrims->AddVertex(PointMax);
  Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims);

  //==== PROCESSING OF CALL 1 =====
  Standard_Integer nbp = 10;
  Standard_Real ParamP1       = ElCLib::Parameter(aCircle1,P1);
  Standard_Real ParamPAttach1 = ElCLib::Parameter(aCircle1,AttachmentPoint1);
  Standard_Real alpha,Dalpha,alphaIter;

  alpha = fabs(ParamP1 - ParamPAttach1);
  if(ParamP1 < ParamPAttach1){
    if(alpha > M_PI){
      alpha  = (2.*M_PI) - alpha;
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
      alpha  = (2.*M_PI) - alpha;
      nbp    = (Standard_Integer ) IntegerPart(alpha/(alpha*.02));
      Dalpha = -alpha/(nbp - 1);
    }
    else{
      nbp    = (Standard_Integer ) IntegerPart(alpha/(alpha*.02));
      Dalpha = alpha/(nbp - 1);
    }
  }

  aPrims = new Graphic3d_ArrayOfPolylines(nbp);
  aPrims->AddVertex(AttachmentPoint1);
  alphaIter = Dalpha;
  gp_Pnt PntIter;
  Standard_Integer i;
  for(i = 2; i <= nbp; i++, alphaIter += Dalpha)
    aPrims->AddVertex(ElCLib::Value(ParamPAttach1 + alphaIter,aCircle1));
  Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims);
  
  //==== PROCESSING OF CALL 2 =====
  gp_Pnt Center2 = ProjCenter1.Translated(Vp.Reversed());
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

  aPrims = new Graphic3d_ArrayOfPolylines(nbp);
  aPrims->AddVertex(AttachmentPoint2);
  alphaIter = Dalpha;
  for(i = 2; i <= nbp; i++, alphaIter += Dalpha)
    aPrims->AddVertex(ElCLib::Value(ParamPAttach2 + alphaIter,aCircle2));
  Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims);

  //==== ARROWS ================
  Prs3d_Root::NewGroup(aPresentation);
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  
  if (dist < (LA->ArrowAspect()->Length()+LA->ArrowAspect()->Length())) outside = Standard_True;
  gp_Dir arrdir = L3.Direction().Reversed();
  if (outside) arrdir.Reverse();
  // arrow 1 ----
  Prs3d_Arrow::Draw (Prs3d_Root::CurrentGroup (aPresentation), P1, arrdir, LA->ArrowAspect()->Angle(), LA->ArrowAspect()->Length());
  
  // arrow 2 ----
  Prs3d_Arrow::Draw (Prs3d_Root::CurrentGroup (aPresentation), P2, arrdir.Reversed(), LA->ArrowAspect()->Angle(), LA->ArrowAspect()->Length());

  //-------------------------------------------------------------------------------------
  //|                                SYMBOL OF SYMMETRY                                 |
  //-------------------------------------------------------------------------------------

  //           -------    : Superior Segment
  //         -----------  : Axis
  //           -------    : Inferior Segment 
  
  gp_Vec Vvar(P1,P2);
  gp_Vec Vtmp = Vvar.Divided(Vvar.Magnitude()).Multiplied(2*(aAxis.Distance(Center1)));
  gp_Vec vec = Vtmp;
  gp_Vec vecA = vec.Multiplied(.1);

  gp_Dir DirAxis = aAxis.Direction();
  gp_Vec Vaxe(DirAxis);
  gp_Vec vecB = Vaxe.Multiplied(vecA.Magnitude());
  vecB.Multiply(.5);

  gp_Pnt pm = P1.Translated(Vvar.Multiplied(.5));
  gp_Pnt pOff = OffsetPnt.Translated(vecB);

  //Calculation of extremas of the axis of the symbol
  gp_Vec vecAxe = vecA.Multiplied(.7);

  Prs3d_Root::NewGroup(aPresentation);
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());

  aPrims = new Graphic3d_ArrayOfPolylines(13,5);

  aPrims->AddBound(2);
  aPrims->AddVertex(pOff.Translated(vecAxe));
  aPrims->AddVertex(pOff.Translated(vecAxe.Reversed()));

  //Calculation of extremas of the superior segment of the symbol
  gp_Vec vec1 = vecAxe.Multiplied(.6);
  vecAxe = Vaxe.Multiplied(vecAxe.Magnitude());
  gp_Vec vec2 = vecAxe.Multiplied(.4);

  aPrims->AddBound(2);
  aPrims->AddVertex(pOff.Translated(vec1.Added(vec2)));
  aPrims->AddVertex(pOff.Translated(vec1.Reversed().Added(vec2)));

  //Calculation of extremas of the inferior segment of the symbol
  aPrims->AddBound(2);
  aPrims->AddVertex(pOff.Translated(vec1.Added(vec2.Reversed())));
  aPrims->AddVertex(pOff.Translated(vec1.Reversed().Added(vec2.Reversed())));
  
/*--------------------------------------------------------------------------------------
  |                          MARKING OF THE AXIS OF SYMMETRY                           |
  --------------------------------------------------------------------------------------     
          ____
          \  / :Cursor
           \/
           /\
          /__\
*/
  Standard_Real Dist = aAxis.Distance(Center1)/37;
  gp_Dir aDirectionAxis = aAxis.Direction();
  gp_Vec vs(aDirectionAxis);
  gp_Vec vsym(vs.Divided(vs.Magnitude()).Multiplied(Dist).XYZ());
  gp_Vec vsymper(vsym.Y(),-vsym.X(),vsym.Z());

  aPrims->AddBound(5);
  gp_Pnt pm1 = pm.Translated(vsym.Added(vsymper));
  aPrims->AddVertex(pm1);
  pm1 = pm1.Translated(vsym.Reversed().Multiplied(2).Added(vsymper.Reversed().Multiplied(2)));
  aPrims->AddVertex(pm1);
  pm1 = pm1.Translated(vsymper.Multiplied(2));
  aPrims->AddVertex(pm1);
  pm1 = pm1.Translated(vsym.Multiplied(2).Added(vsymper.Reversed().Multiplied(2)));
  aPrims->AddVertex(pm1);
  pm1 = pm1.Translated(vsymper.Multiplied(2));
  aPrims->AddVertex(pm1);

  vsym.Multiply(4);

  aPrims->AddBound(2);
  aPrims->AddVertex(pm.Translated(vsym));
  aPrims->AddVertex(pm.Translated(vsym.Reversed()));

  Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims);
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
  Handle(Prs3d_DimensionAspect) LA = aDrawer->DimensionAspect();
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());  

  if (AttachmentPoint1.IsEqual(AttachmentPoint2,Precision::Confusion()))
  {
    //==============================================================
    //  SYMMETRY WHEN THE REFERENCE POINT IS ON THE AXIS OF SYM.:
    //==============================================================
    //Marker of localisation of the face
    Quantity_Color aColor = LA->LineAspect()->Aspect()->Color();
    Handle(Graphic3d_AspectMarker3d) aMarkerAsp = new Graphic3d_AspectMarker3d (Aspect_TOM_O, aColor, 1.0);
    Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect (aMarkerAsp);
    Handle(Graphic3d_ArrayOfPoints) anArrayOfPoints = new Graphic3d_ArrayOfPoints (1);
    anArrayOfPoints->AddVertex (AttachmentPoint1.X(), AttachmentPoint1.Y(), AttachmentPoint1.Z());
    Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray (anArrayOfPoints);


    //Trace of the linking segment 
    Prs3d_Root::NewGroup(aPresentation);
    Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());

    Handle(Graphic3d_ArrayOfSegments) aPrims = new Graphic3d_ArrayOfSegments(8);

	aPrims->AddVertex(AttachmentPoint1);
    aPrims->AddVertex(OffsetPoint);
  
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

    aPrims->AddVertex(pDaxe);
    aPrims->AddVertex(pFaxe);

    //Calculate extremas of the superior segment of the symbol
    gp_Vec nVAO  (-uVAO.Y(),uVAO.X(),uVAO.Z());
    gp_Pnt sgP11 = pDaxe.Translated(uVAO.Multiplied(2.).Added(nVAO.Multiplied(2.)));
    gp_Pnt sgP12 = sgP11.Translated(uVAO.Multiplied(8.));

    aPrims->AddVertex(sgP11);
    aPrims->AddVertex(sgP12);

    //Calculate extremas of the inferior segment of the symbol
    gp_Vec nVAOr = nVAO.Reversed();
    gp_Pnt sgP21 = pDaxe.Translated(uVAO.Multiplied(2.).Added(nVAOr.Multiplied(2.)));
    gp_Pnt sgP22 = sgP21.Translated(uVAO.Multiplied(8.));

    aPrims->AddVertex(sgP21);
    aPrims->AddVertex(sgP22);

	Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims);
  }
  //==============================================================
  //  OTHER CASES                                                 :
  //==============================================================

  else {
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

    //==== PROCESSING OF FACE ===========
    Prs3d_Root::NewGroup(aPresentation);
    Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());

    Handle(Graphic3d_ArrayOfPrimitives) aPrims = new Graphic3d_ArrayOfSegments(6);

	aPrims->AddVertex(PointMin);
    aPrims->AddVertex(PointMax);
    
    //==== PROCESSING OF CALL 1 =====
    aPrims->AddVertex(AttachmentPoint1);
    aPrims->AddVertex(P1);
    
    //==== PROCESSING OF CALL 2 =====
    aPrims->AddVertex(AttachmentPoint2);
    aPrims->AddVertex(P2);

	Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims);
 
    //==== ARROWS ================
    if (dist < (LA->ArrowAspect()->Length()+LA->ArrowAspect()->Length())) outside = Standard_True;
    gp_Dir arrdir = L3.Direction().Reversed();
    if (outside) arrdir.Reverse();
    // arrow 1 ----
    Prs3d_Arrow::Draw (Prs3d_Root::CurrentGroup (aPresentation), P1, arrdir, LA->ArrowAspect()->Angle(), LA->ArrowAspect()->Length());
  
    // arrow 2 ----
    Prs3d_Arrow::Draw (Prs3d_Root::CurrentGroup (aPresentation), P2, arrdir.Reversed(), LA->ArrowAspect()->Angle(), LA->ArrowAspect()->Length());
    
    //==== POINTS ================
    //Marker of localization of attachment points:
    Prs3d_Root::NewGroup(aPresentation);
    Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());

    Quantity_Color aColor = LA->LineAspect()->Aspect()->Color();
    Handle(Graphic3d_AspectMarker3d) aMarkerAspAtt = new Graphic3d_AspectMarker3d (Aspect_TOM_O, aColor, 1.0);
    Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect (aMarkerAspAtt);
    Handle(Graphic3d_ArrayOfPoints) anArrayOfPoints1 = new Graphic3d_ArrayOfPoints (1);
    anArrayOfPoints1->AddVertex (AttachmentPoint1.X(), AttachmentPoint1.Y(), AttachmentPoint1.Z());
    Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray (anArrayOfPoints1);

    Prs3d_Root::NewGroup(aPresentation);
    Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
    Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect (aMarkerAspAtt);
    Handle(Graphic3d_ArrayOfPoints) anArrayOfPoints2 = new Graphic3d_ArrayOfPoints (1);
    anArrayOfPoints2->AddVertex (AttachmentPoint2.X(), AttachmentPoint2.Y(), AttachmentPoint2.Z());
    Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray (anArrayOfPoints2);
      
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

    gp_Pnt pm = P1.Translated(vec.Multiplied(.5));
    gp_Pnt pOff = OffsetPoint.Translated(vecB);
    
    //Calculate the extremas of the axis of the symbol
    gp_Vec vecAxe = vecA.Multiplied(.7);

    Prs3d_Root::NewGroup(aPresentation);
    Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());

	aPrims = new Graphic3d_ArrayOfPolylines(13,5);

    aPrims->AddBound(2);
	aPrims->AddVertex(pOff.Translated(vecAxe));
    aPrims->AddVertex(pOff.Translated(vecAxe.Reversed()));

    //Calculate the extremas of the superior segment of the symbol
    gp_Vec vec1 = vecAxe.Multiplied(.6);
    vecAxe = Vaxe.Multiplied(vecAxe.Magnitude());
    gp_Vec vec2 = vecAxe.Multiplied(.4);

    aPrims->AddBound(2);
    aPrims->AddVertex(pOff.Translated(vec1.Added(vec2)));
    aPrims->AddVertex(pOff.Translated(vec1.Reversed().Added(vec2)));
    
    //Calculate the extremas of the inferior segment of the symbol
    aPrims->AddBound(2);
    aPrims->AddVertex(pOff.Translated(vec1.Added(vec2.Reversed())));
    aPrims->AddVertex(pOff.Translated(vec1.Reversed().Added(vec2.Reversed())));

    /*--------------------------------------------------------------------------------------
    |                          MARKING OF THE AXIS OF SYMMETRY                           |
    --------------------------------------------------------------------------------------     
            ____
            \  / :Cursor
             \/
             /\
            /__\
    */
    Standard_Real Dist = P1.Distance(P2)/75;
    gp_Dir aDirectionAxis = aAxis.Direction();
    gp_Vec vs(aDirectionAxis);
    gp_Vec vsym(vs.Divided(vs.Magnitude()).Multiplied(Dist).XYZ());
    gp_Vec vsymper(vsym.Y(),-vsym.X(),vsym.Z());

    aPrims->AddBound(5);
    gp_Pnt pm1 = pm.Translated(vsym.Added(vsymper));
    aPrims->AddVertex(pm1);
    pm1 = pm1.Translated(vsym.Reversed().Multiplied(2).Added(vsymper.Reversed().Multiplied(2)));
    aPrims->AddVertex(pm1);
    pm1 = pm1.Translated(vsymper.Multiplied(2));
    aPrims->AddVertex(pm1);
    pm1 = pm1.Translated(vsym.Multiplied(2).Added(vsymper.Reversed().Multiplied(2)));
    aPrims->AddVertex(pm1);
    pm1 = pm1.Translated(vsymper.Multiplied(2));
    aPrims->AddVertex(pm1);

    vsym.Multiply(4);

    aPrims->AddBound(2);
    aPrims->AddVertex(pm.Translated(vsym));
    aPrims->AddVertex(pm.Translated(vsym.Reversed()));

	Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims);
  } 
}
