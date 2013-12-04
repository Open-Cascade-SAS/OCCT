// Created on: 1995-02-07
// Created by: Jacques GOUSSARD
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <GeomInt_LineConstructor.ixx>

#include <GeomInt_LineTool.hxx>
#include <GeomInt_SequenceOfParameterAndOrientation.hxx>
#include <GeomInt_ParameterAndOrientation.hxx>

#include <IntPatch_Point.hxx>
#include <IntPatch_GLine.hxx>
#include <IntPatch_WLine.hxx>
#include <IntPatch_ALine.hxx>
#include <IntSurf_Transition.hxx>
#include <TopAbs_Orientation.hxx>

#include <Precision.hxx>
#include <gp_Pnt2d.hxx>
#include <Adaptor2d_HCurve2d.hxx>

#include <GeomAdaptor_HSurface.hxx>
#include <Standard_ConstructionError.hxx>
#include <IntSurf_Quadric.hxx>
#include <IntSurf_PntOn2S.hxx>
#include <ElCLib.hxx>
#include <GeomAbs_SurfaceType.hxx>


//=======================================================================
//function : Recadre
//purpose  : 
//=======================================================================
static void Recadre(const Handle(GeomAdaptor_HSurface)& myHS1,
                    const Handle(GeomAdaptor_HSurface)& myHS2,
                    Standard_Real& u1,
                    Standard_Real& v1,
                    Standard_Real& u2,
                    Standard_Real& v2)
{
  Standard_Boolean myHS1IsUPeriodic,myHS1IsVPeriodic;
  const GeomAbs_SurfaceType typs1 = myHS1->GetType();
  switch (typs1)
  {
    case GeomAbs_Cylinder:
    case GeomAbs_Cone:
    case GeomAbs_Sphere:
    { 
      myHS1IsUPeriodic = Standard_True;
      myHS1IsVPeriodic = Standard_False;
      break;
    }
    case GeomAbs_Torus:
    {
      myHS1IsUPeriodic = myHS1IsVPeriodic = Standard_True;
      break;
    }
    default:
    {
       //-- Le cas de biparametrees periodiques est gere en amont
      myHS1IsUPeriodic = myHS1IsVPeriodic = Standard_False;
      break;
    }
  }
  Standard_Boolean myHS2IsUPeriodic,myHS2IsVPeriodic;
  const GeomAbs_SurfaceType typs2 = myHS2->GetType();
  switch (typs2)
  {
    case GeomAbs_Cylinder:
    case GeomAbs_Cone:
    case GeomAbs_Sphere: 
    { 
      myHS2IsUPeriodic = Standard_True;
      myHS2IsVPeriodic = Standard_False;
      break;
    }
    case GeomAbs_Torus:
    {
      myHS2IsUPeriodic = myHS2IsVPeriodic = Standard_True;
      break;
    }
    default:
    {
      //-- Le cas de biparametrees periodiques est gere en amont
      myHS2IsUPeriodic = myHS2IsVPeriodic = Standard_False;
      break;
    }
  }
  if(myHS1IsUPeriodic) {
    const Standard_Real lmf = M_PI+M_PI; //-- myHS1->UPeriod();
    const Standard_Real f = myHS1->FirstUParameter();
    const Standard_Real l = myHS1->LastUParameter();
    while(u1 < f) { u1+=lmf; } 
    while(u1 > l) { u1-=lmf; }
  }
  if(myHS1IsVPeriodic) {
    const Standard_Real lmf = M_PI+M_PI; //-- myHS1->VPeriod(); 
    const Standard_Real f = myHS1->FirstVParameter();
    const Standard_Real l = myHS1->LastVParameter();
    while(v1 < f) { v1+=lmf; } 
    while(v1 > l) { v1-=lmf; }
  }
  if(myHS2IsUPeriodic) { 
    const Standard_Real lmf = M_PI+M_PI; //-- myHS2->UPeriod();
    const Standard_Real f = myHS2->FirstUParameter();
    const Standard_Real l = myHS2->LastUParameter();
    while(u2 < f) { u2+=lmf; } 
    while(u2 > l) { u2-=lmf; }
  }
  if(myHS2IsVPeriodic) { 
    const Standard_Real lmf = M_PI+M_PI; //-- myHS2->VPeriod();
    const Standard_Real f = myHS2->FirstVParameter();
    const Standard_Real l = myHS2->LastVParameter();
    while(v2 < f) { v2+=lmf; } 
    while(v2 > l) { v2-=lmf; }
  }
}


//=======================================================================
//function : Parameters
//purpose  : 
//=======================================================================
static void Parameters(const Handle(GeomAdaptor_HSurface)& myHS1,
                       const Handle(GeomAdaptor_HSurface)& myHS2,
                       const gp_Pnt& Ptref,
                       Standard_Real& U1,
                       Standard_Real& V1,
                       Standard_Real& U2,
                       Standard_Real& V2)
{
  IntSurf_Quadric quad1,quad2;
  switch (myHS1->Surface().GetType())
  {
    case GeomAbs_Plane:    quad1.SetValue(myHS1->Surface().Plane()); break;
    case GeomAbs_Cylinder: quad1.SetValue(myHS1->Surface().Cylinder()); break;
    case GeomAbs_Cone:     quad1.SetValue(myHS1->Surface().Cone()); break;
    case GeomAbs_Sphere:   quad1.SetValue(myHS1->Surface().Sphere()); break;
    default: Standard_ConstructionError::Raise("GeomInt_LineConstructor::Parameters");
  }
  switch (myHS2->Surface().GetType())
  {
    case GeomAbs_Plane:    quad2.SetValue(myHS2->Surface().Plane()); break;
    case GeomAbs_Cylinder: quad2.SetValue(myHS2->Surface().Cylinder()); break;
    case GeomAbs_Cone:     quad2.SetValue(myHS2->Surface().Cone()); break;
    case GeomAbs_Sphere:   quad2.SetValue(myHS2->Surface().Sphere()); break;
    default: Standard_ConstructionError::Raise("GeomInt_LineConstructor::Parameters");
  }
  quad1.Parameters(Ptref,U1,V1);
  quad2.Parameters(Ptref,U2,V2);
}


//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void GeomInt_LineConstructor::Perform(const Handle(IntPatch_Line)& L)
{
  Standard_Integer i,nbvtx;
  Standard_Real firstp,lastp;
  const Standard_Real Tol = Precision::PConfusion() * 35.0;
  
  const IntPatch_IType typl = L->ArcType();
  if(typl == IntPatch_Analytic)
  {
    Standard_Real u1,v1,u2,v2;
    Handle(IntPatch_ALine)& ALine =  *((Handle(IntPatch_ALine) *)&L);
    seqp.Clear();
    nbvtx = GeomInt_LineTool::NbVertex(L);
    for(i=1;i<nbvtx;i++)
    {
      firstp = GeomInt_LineTool::Vertex(L,i).ParameterOnLine();
      lastp =  GeomInt_LineTool::Vertex(L,i+1).ParameterOnLine();
      if(firstp!=lastp)
	  { 
        const Standard_Real pmid = (firstp+lastp)*0.5;
        const gp_Pnt Pmid = ALine->Value(pmid);
        Parameters(myHS1,myHS2,Pmid,u1,v1,u2,v2);
        Recadre(myHS1,myHS2,u1,v1,u2,v2);
        const TopAbs_State in1 = myDom1->Classify(gp_Pnt2d(u1,v1),Tol);
        if(in1 !=  TopAbs_OUT) { 
          const TopAbs_State in2 = myDom2->Classify(gp_Pnt2d(u2,v2),Tol);
          if(in2 != TopAbs_OUT) { 
            seqp.Append(firstp);
            seqp.Append(lastp);
          }
        }
      }
    }
    done = Standard_True;
    return;
  }
  else if(typl == IntPatch_Walking)
  { 
    Standard_Real u1,v1,u2,v2;
    Handle(IntPatch_WLine)& WLine =  *((Handle(IntPatch_WLine) *)&L);
    seqp.Clear();
    nbvtx = GeomInt_LineTool::NbVertex(L);
    for(i=1;i<nbvtx;i++)
	{
      firstp = GeomInt_LineTool::Vertex(L,i).ParameterOnLine();
      lastp =  GeomInt_LineTool::Vertex(L,i+1).ParameterOnLine();
      if(firstp!=lastp)
      {  
        const Standard_Integer pmid = (Standard_Integer )( (firstp+lastp)/2);
        const IntSurf_PntOn2S& Pmid = WLine->Point(pmid);
        Pmid.Parameters(u1,v1,u2,v2);
        Recadre(myHS1,myHS2,u1,v1,u2,v2);
        const TopAbs_State in1 = myDom1->Classify(gp_Pnt2d(u1,v1),Tol);
        if(in1 !=  TopAbs_OUT) {  //-- !=ON donne Pb 
          const TopAbs_State in2 = myDom2->Classify(gp_Pnt2d(u2,v2),Tol);
          if(in2 != TopAbs_OUT) { //-- !=ON  
            seqp.Append(firstp);
            seqp.Append(lastp);
          }
        }
      }
    }
    done = Standard_True;
    return;
  }
  else if (typl != IntPatch_Restriction)
  {
    Standard_Real u1,v1,u2,v2;
    Handle(IntPatch_GLine)& GLine =  *((Handle(IntPatch_GLine) *)&L);
    seqp.Clear();
    nbvtx = GeomInt_LineTool::NbVertex(L);
    Standard_Boolean intrvtested = Standard_False;
    for(i=1;i<nbvtx;i++)
    {
      firstp = GeomInt_LineTool::Vertex(L,i).ParameterOnLine();
      lastp =  GeomInt_LineTool::Vertex(L,i+1).ParameterOnLine();
      if(Abs(firstp-lastp)>Precision::PConfusion())
      {
        intrvtested = Standard_True;
        const Standard_Real pmid = (firstp+lastp)*0.5;
        gp_Pnt Pmid;
        switch (typl)
        {
          case IntPatch_Lin:       Pmid = ElCLib::Value(pmid,GLine->Line()); break;
          case IntPatch_Circle:    Pmid = ElCLib::Value(pmid,GLine->Circle()); break;
          case IntPatch_Ellipse:   Pmid = ElCLib::Value(pmid,GLine->Ellipse()); break;
          case IntPatch_Hyperbola: Pmid = ElCLib::Value(pmid,GLine->Hyperbola()); break;
          case IntPatch_Parabola:  Pmid = ElCLib::Value(pmid,GLine->Parabola()); break;
          case IntPatch_Analytic:
          case IntPatch_Walking:
          case IntPatch_Restriction: break; // cases Analytic, Walking and Restriction are handled above
        }
        Parameters(myHS1,myHS2,Pmid,u1,v1,u2,v2);
        Recadre(myHS1,myHS2,u1,v1,u2,v2);
        const TopAbs_State in1 = myDom1->Classify(gp_Pnt2d(u1,v1),Tol);
        if(in1 !=  TopAbs_OUT) { 
          const TopAbs_State in2 = myDom2->Classify(gp_Pnt2d(u2,v2),Tol);
          if(in2 != TopAbs_OUT) { 
            seqp.Append(firstp);
            seqp.Append(lastp);
          }
        }
      }
    }
    if(typl == IntPatch_Circle || typl == IntPatch_Ellipse)
    { 
      firstp = GeomInt_LineTool::Vertex(L,nbvtx).ParameterOnLine();
      lastp  = M_PI + M_PI + GeomInt_LineTool::Vertex(L,1).ParameterOnLine();
      const Standard_Real cadrinf = GeomInt_LineTool::FirstParameter(L);
      const Standard_Real cadrsup = GeomInt_LineTool::LastParameter(L);
      Standard_Real acadr = (firstp+lastp)*0.5;
      while(acadr < cadrinf) { acadr+=M_PI+M_PI; }
      while(acadr > cadrsup) { acadr-=M_PI+M_PI; } 
      if(acadr>=cadrinf && acadr<=cadrsup)
      { 
        if(Abs(firstp-lastp)>Precision::PConfusion())
        {
          intrvtested = Standard_True;
          const Standard_Real pmid = (firstp+lastp)*0.5;
          gp_Pnt Pmid;
          if (typl == IntPatch_Circle)
            Pmid = ElCLib::Value(pmid,GLine->Circle());
          else
            Pmid = ElCLib::Value(pmid,GLine->Ellipse());
          Parameters(myHS1,myHS2,Pmid,u1,v1,u2,v2);
          Recadre(myHS1,myHS2,u1,v1,u2,v2);
          const TopAbs_State in1 = myDom1->Classify(gp_Pnt2d(u1,v1),Tol);
          if(in1 !=  TopAbs_OUT) { 
            const TopAbs_State in2 = myDom2->Classify(gp_Pnt2d(u2,v2),Tol);
            if(in2 != TopAbs_OUT) { 
              seqp.Append(firstp);
              seqp.Append(lastp);
            }
          }
        }
      }
    }      
    if (!intrvtested) {
      // on garde a priori. Il faudrait un point 2d sur chaque
      // surface pour prendre la decision. Sera fait dans 
      // l`appelant
      seqp.Append(GeomInt_LineTool::FirstParameter(L));
      seqp.Append(GeomInt_LineTool::LastParameter(L));
    }
    done = Standard_True;
    return;
  }

  done = Standard_False;
  seqp.Clear();
  nbvtx = GeomInt_LineTool::NbVertex(L);
  if (nbvtx == 0) { // on garde a priori. Il faudrait un point 2d sur chaque
                    // surface pour prendre la decision. Sera fait dans 
                    // l`appelant
    seqp.Append(GeomInt_LineTool::FirstParameter(L));
    seqp.Append(GeomInt_LineTool::LastParameter(L));
    done = Standard_True;
    return;
  }

  GeomInt_SequenceOfParameterAndOrientation seqpss;
  TopAbs_Orientation or1=TopAbs_FORWARD,or2=TopAbs_FORWARD;

  for (i=1; i<=nbvtx; i++)
  {
    const IntPatch_Point& thevtx = GeomInt_LineTool::Vertex(L,i);
    const Standard_Real prm = thevtx.ParameterOnLine();
    if (thevtx.IsOnDomS1())
    {
      switch (thevtx.TransitionLineArc1().TransitionType())
      {
        case IntSurf_In: or1 = TopAbs_FORWARD; break;  
        case IntSurf_Out: or1 = TopAbs_REVERSED; break;  
        case IntSurf_Touch: or1 = TopAbs_INTERNAL; break;  
        case IntSurf_Undecided: or1 = TopAbs_INTERNAL; break;  
      }
    }
    else
      or1 = TopAbs_INTERNAL;

    if (thevtx.IsOnDomS2())
    {
      switch (thevtx.TransitionLineArc2().TransitionType())
      {
        case IntSurf_In: or2 = TopAbs_FORWARD; break;  
        case IntSurf_Out: or2 = TopAbs_REVERSED; break;  
        case IntSurf_Touch: or2 = TopAbs_INTERNAL; break;  
        case IntSurf_Undecided: or2 = TopAbs_INTERNAL; break;  
      }
    }
    else
      or2 = TopAbs_INTERNAL;

    const Standard_Integer nbinserted = seqpss.Length();
    Standard_Boolean inserted = Standard_False;
    for (Standard_Integer j=1; j<=nbinserted;j++)
    {
      if (Abs(prm-seqpss(j).Parameter()) <= Tol)
      {
        // on cumule
        GeomInt_ParameterAndOrientation& valj = seqpss.ChangeValue(j);
        if (or1 != TopAbs_INTERNAL) {
          if (valj.Orientation1() != TopAbs_INTERNAL) {
            if (or1 != valj.Orientation1()) {
              valj.SetOrientation1(TopAbs_INTERNAL);
            }
          }
          else {
            valj.SetOrientation1(or1);
          }
        }
	
        if (or2 != TopAbs_INTERNAL) {
          if (valj.Orientation2() != TopAbs_INTERNAL) {
            if (or2 != valj.Orientation2()) {
              valj.SetOrientation2(TopAbs_INTERNAL);
            }
          }
          else {
            valj.SetOrientation2(or2);
          }
		}

        inserted = Standard_True;
        break;
      }
      
      if (prm < seqpss(j).Parameter()-Tol ) {
        // on insere avant la position j
        seqpss.InsertBefore(j,GeomInt_ParameterAndOrientation(prm,or1,or2));
        inserted = Standard_True;
        break;
      }
    }
    if (!inserted) {
      seqpss.Append(GeomInt_ParameterAndOrientation(prm,or1,or2));
    }
  }

  // on determine l`etat en debut de ligne
  Standard_Boolean trim = Standard_False;
  Standard_Boolean dansS1 = Standard_False;
  Standard_Boolean dansS2 = Standard_False;

  nbvtx = seqpss.Length();
  for (i=1; i<= nbvtx; i++)
  {
    or1 = seqpss(i).Orientation1();
    if (or1 != TopAbs_INTERNAL)
    {
      trim = Standard_True;
      dansS1 = (or1 != TopAbs_FORWARD);
      break;
    }
  }

  if (i > nbvtx)
  {
    Standard_Real U,V;
    for (i=1; i<=GeomInt_LineTool::NbVertex(L); i++ )
    {
      if (!GeomInt_LineTool::Vertex(L,i).IsOnDomS1() )
      {
        GeomInt_LineTool::Vertex(L,i).ParametersOnS1(U,V);
        gp_Pnt2d PPCC(U,V);
        if (myDom1->Classify(PPCC,Tol) == TopAbs_OUT) {
          done = Standard_True;
          return;
        }
        break;
      }
    }
    dansS1 = Standard_True; // on garde dans le doute
  }

  for (i=1; i<= nbvtx; i++)
  {
    or2 = seqpss(i).Orientation2();
    if (or2 != TopAbs_INTERNAL)
    {
      trim = Standard_True;
      dansS2 = (or2 != TopAbs_FORWARD);
      break;
    }
  }
  
  if (i > nbvtx)
  {
    Standard_Real U,V;
    for (i=1; i<=GeomInt_LineTool::NbVertex(L); i++ )
    {
      if (!GeomInt_LineTool::Vertex(L,i).IsOnDomS2() )
      {
        GeomInt_LineTool::Vertex(L,i).ParametersOnS2(U,V);
        if (myDom2->Classify(gp_Pnt2d(U,V),Tol) == TopAbs_OUT) {
          done = Standard_True;
          return;
        }
        break;
      }
    }
    dansS2 = Standard_True; // on garde dans le doute
  }

  if (!trim) { // on a necessairement dansS1 == dansS2 == Standard_True
    seqp.Append(GeomInt_LineTool::FirstParameter(L));
    seqp.Append(GeomInt_LineTool::LastParameter(L));
    done = Standard_True;
    return;
  }

  // On epluche la sequence seqpss pour constituer les bouts valides
  // et les stocker dans seqp(2*i+1) et seqp(2*i+2)
  Standard_Real thefirst = GeomInt_LineTool::FirstParameter(L);
  Standard_Real thelast = GeomInt_LineTool::LastParameter(L);
  firstp = thefirst;

  for (i=1; i<=nbvtx; i++)
  {
    or1 = seqpss(i).Orientation1(); 
    or2 = seqpss(i).Orientation2(); 
    if (dansS1 && dansS2)
    {
      if (or1 == TopAbs_REVERSED)
        dansS1 = Standard_False;
      /*else if (or1 == TopAbs_FORWARD) {
      }*/
      if (or2 == TopAbs_REVERSED)
        dansS2 = Standard_False;
      /*else if (or2 == TopAbs_FORWARD) {
      }*/
      if (!dansS1 || !dansS2)
      {
        lastp = seqpss(i).Parameter();
        Standard_Real stofirst = Max(firstp, thefirst);
        Standard_Real stolast  = Min(lastp,  thelast) ;
        if (stolast > stofirst) {
          seqp.Append(stofirst);
          seqp.Append(stolast);
        }
        if (lastp > thelast)
          break;
      }
    }
    else
    {
      if (dansS1)
      {
        if (or1 == TopAbs_REVERSED)
          dansS1 = Standard_False;
        /*else if (or1 == TopAbs_FORWARD) {
        }*/
      }
      else
      {
        if (or1 == TopAbs_FORWARD)
          dansS1 = Standard_True;
        /*else if (or1 == TopAbs_REVERSED) {
        }*/
      }
      if (dansS2)
      {
        if (or2 == TopAbs_REVERSED)
          dansS2 = Standard_False;
        /*else if (or2 == TopAbs_FORWARD) {
        }*/
      }
      else
      {
        if (or2 == TopAbs_FORWARD)
          dansS2 = Standard_True;
        /*else if (or2 == TopAbs_REVERSED) {
        }*/
      }
      if (dansS1 && dansS2)
        firstp = seqpss(i).Parameter();
    }
  }

  // le petit dernier a rajouter
  if (dansS1 && dansS2)
  {
    lastp  = thelast;
    firstp = Max(firstp,thefirst);
    if (lastp > firstp) {
      seqp.Append(firstp);
      seqp.Append(lastp);
    }
  }
  done = Standard_True;
}


//=======================================================================
//function : PeriodicLine
//purpose  : 
//=======================================================================
void GeomInt_LineConstructor::PeriodicLine (const Handle(IntPatch_Line)& L) const
{
  const IntPatch_IType typl = L->ArcType();
  if (typl != IntPatch_Circle && typl != IntPatch_Ellipse)
    return;

  const Standard_Real Tol = Precision::PConfusion();
  Handle(IntPatch_GLine) glin = Handle(IntPatch_GLine)::DownCast(L);
  Standard_Integer i,j,nbvtx = glin->NbVertex();
  for (i=1; i<=nbvtx; i++)
  {
    IntPatch_Point thevtx = glin->Vertex(i);
    const Standard_Real prm = thevtx.ParameterOnLine();
    Standard_Boolean changevtx = Standard_False;
    if (thevtx.IsOnDomS1() || thevtx.IsOnDomS2())
    {
      for (j=1; j<=nbvtx; j++)
      {
        if (j!=i)
        {
          const IntPatch_Point& thevtxbis = glin->Vertex(j);
          const Standard_Real prmbis = thevtxbis.ParameterOnLine();
          if (Abs(prm-prmbis) <= Tol)
          {
            Standard_Real u,v;
            gp_Pnt2d p2d;
            if (thevtx.IsOnDomS1() && thevtxbis.IsOnDomS1() &&
                thevtxbis.TransitionLineArc1().TransitionType()==IntSurf_In)
            {
              p2d = thevtx.ArcOnS1()->Value(thevtx.ParameterOnArc1());
              u = p2d.X(); v = p2d.Y();
              p2d = thevtxbis.ArcOnS1()->Value(thevtxbis.ParameterOnArc1());
              if (Abs(u-p2d.X()) > Tol || Abs(v-p2d.Y()) > Tol)
              {
                changevtx = Standard_True;
                break;
              }
            }
            if (thevtx.IsOnDomS2() && thevtxbis.IsOnDomS2() &&
                thevtxbis.TransitionLineArc2().TransitionType()==IntSurf_In)
            {
              p2d = thevtx.ArcOnS2()->Value(thevtx.ParameterOnArc2());
              u = p2d.X(); v = p2d.Y();
              p2d = thevtxbis.ArcOnS2()->Value(thevtxbis.ParameterOnArc2());
              if (Abs(u-p2d.X()) > Tol || Abs(v-p2d.Y()) > Tol)
              {
                changevtx = Standard_True;
                break;
              }
            }
          }
        }
      }
    }
    if (changevtx) {
      thevtx.SetParameter(prm + 2.*M_PI);
      glin->Replace(i,thevtx);
    }
  }
}
