// Created on: 1995-02-07
// Created by: Jacques GOUSSARD
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


#include <IntTools_LineConstructor.ixx>

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
#include <TColStd_IndexedMapOfInteger.hxx>

static
  void Parameters(const Handle(GeomAdaptor_HSurface)& myHS1,
		  const gp_Pnt& Ptref,
		  Standard_Real& U1,
		  Standard_Real& V1);
static
  void Parameters(const Handle(GeomAdaptor_HSurface)& myHS1,
		  const Handle(GeomAdaptor_HSurface)& myHS2,
		  const gp_Pnt& Ptref,
		  Standard_Real& U1,
		  Standard_Real& V1,
		  Standard_Real& U2,
		  Standard_Real& V2);

static 
  void GLinePoint(const IntPatch_IType typl,
		  const Handle(IntPatch_GLine)& GLine,
		  const Standard_Real aT,
		  gp_Pnt& aP);
static
  void Recadre(const Handle(GeomAdaptor_HSurface)& myHS1,
	       const Handle(GeomAdaptor_HSurface)& myHS2,
	       Standard_Real& u1,
	       Standard_Real& v1,
	       Standard_Real& u2,
	       Standard_Real& v2);

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void IntTools_LineConstructor::Perform(const Handle(IntPatch_Line)& L)
{
  Standard_Integer i,nbvtx;
  Standard_Real firstp,lastp;
  const Standard_Real Tol = Precision::PConfusion() * 35.0;
  
  const IntPatch_IType typl = L->ArcType();
  if(typl == IntPatch_Analytic)  {
    Standard_Real u1,v1,u2,v2;
    Handle(IntPatch_ALine)& ALine =  *((Handle(IntPatch_ALine) *)&L);
    seqp.Clear();
    nbvtx = GeomInt_LineTool::NbVertex(L);
    for(i=1;i<nbvtx;i++)   {
      firstp = GeomInt_LineTool::Vertex(L,i).ParameterOnLine();
      lastp =  GeomInt_LineTool::Vertex(L,i+1).ParameterOnLine();
      if(firstp!=lastp)      {
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
  } // if(typl == IntPatch_Analytic)  {
  else if(typl == IntPatch_Walking)  {
    Standard_Real u1,v1,u2,v2;
    Handle(IntPatch_WLine)& WLine =  *((Handle(IntPatch_WLine) *)&L);
    seqp.Clear();
    nbvtx = GeomInt_LineTool::NbVertex(L);
    for(i=1;i<nbvtx;i++)    { 
      firstp = GeomInt_LineTool::Vertex(L,i).ParameterOnLine();
      lastp =  GeomInt_LineTool::Vertex(L,i+1).ParameterOnLine();
      if(firstp!=lastp) { 
	if(lastp != firstp+1)  {
	  const Standard_Integer pmid = (Standard_Integer )( (firstp+lastp)/2);
	  const IntSurf_PntOn2S& Pmid = WLine->Point(pmid);
	  Pmid.Parameters(u1,v1,u2,v2);
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
	else {
	  const IntSurf_PntOn2S& Pfirst = WLine->Point((Standard_Integer)(firstp));
	  Pfirst.Parameters(u1,v1,u2,v2);
	  Recadre(myHS1,myHS2,u1,v1,u2,v2);
	  TopAbs_State in1 = myDom1->Classify(gp_Pnt2d(u1,v1),Tol);
	  if(in1 !=  TopAbs_OUT) {  //-- !=ON donne Pb 
	    TopAbs_State in2 = myDom2->Classify(gp_Pnt2d(u2,v2),Tol);
	    if(in2 != TopAbs_OUT) { //-- !=ON  
	      const IntSurf_PntOn2S& Plast = WLine->Point((Standard_Integer)(lastp));
	      Plast.Parameters(u1,v1,u2,v2);
	      Recadre(myHS1,myHS2,u1,v1,u2,v2);
	      in1 = myDom1->Classify(gp_Pnt2d(u1,v1),Tol);
	      if(in1 !=  TopAbs_OUT) {  //-- !=ON donne Pb 
		in2 = myDom2->Classify(gp_Pnt2d(u2,v2),Tol);
		if(in2 != TopAbs_OUT) {
		  seqp.Append(firstp);
		  seqp.Append(lastp);
		}
	      }
	    }
	  }
	}
      }
    }
    //
    // The One resulting curve consists of 7 segments that are 
    // connected between each other.
    // The aim of the block is to reject these segments and have
    // one segment instead of 7. 
    //     The other reason to do that is value of TolReached3D=49.
    //     Why -? It is not known yet. 
    //                                             PKV 22.Apr.2002
    //
    Standard_Integer aNbParts;
    //
    aNbParts = seqp.Length()/2;
    if (aNbParts > 1) {  
      Standard_Boolean bCond;
      GeomAbs_SurfaceType aST1, aST2;
      aST1 = myHS1->Surface().GetType();
      aST2 = myHS2->Surface().GetType();
      //
      bCond=Standard_False;
      if (aST1==GeomAbs_Plane) {
	if (aST2==GeomAbs_SurfaceOfExtrusion || 
	    aST2==GeomAbs_SurfaceOfRevolution) {//+zft
	  bCond=!bCond;
	}
      }
      else if (aST2==GeomAbs_Plane) {
	if (aST1==GeomAbs_SurfaceOfExtrusion || 
	    aST1==GeomAbs_SurfaceOfRevolution) {//+zft
	  bCond=!bCond;
	}
      }
      //
      if (bCond) {
	Standard_Integer aNb, anIndex, aNbTmp, jx;
	TColStd_IndexedMapOfInteger aMap;
	TColStd_SequenceOfReal aSeqTmp;
	//
	aNb=seqp.Length();
	for(i=1; i<=aNb; ++i) {
	  lastp =seqp(i);
	  anIndex=(Standard_Integer)lastp;
	  if (!aMap.Contains(anIndex)){
	    aMap.Add(anIndex);
	    aSeqTmp.Append(lastp);
	  }
	  else {
	    aNbTmp=aSeqTmp.Length();
	    aSeqTmp.Remove(aNbTmp);
	  }
	}
	//
	seqp.Clear();
	//
	aNb=aSeqTmp.Length()/2;
	for(i=1; i<=aNb;++i) {
	  jx=2*i;
	  firstp=aSeqTmp(jx-1);
	  lastp =aSeqTmp(jx);
	  seqp.Append(firstp);
	  seqp.Append(lastp);
	}
      }//if (bCond) {
    }
    done = Standard_True;
    return;
  }// else if(typl == IntPatch_Walking)  {
  //
  else if (typl != IntPatch_Restriction)  {
    Standard_Boolean intrvtested;
    Standard_Real u1,v1,u2,v2;
    //
    seqp.Clear();
    //
    Handle(IntPatch_GLine)& GLine =  *((Handle(IntPatch_GLine) *)&L);
    // reject micro circles, ellipses
    switch (typl) {
      case IntPatch_Circle: {
	Standard_Real aR;
	gp_Circ aCirc;
	//
	aCirc=GLine->Circle();
	aR=aCirc.Radius();
	if (aR<Tol) {
	  done = Standard_True;
	  return;
	}
	break;
      }
      case IntPatch_Ellipse: {
	Standard_Real aR;
	gp_Elips aEllipse;
	//
	aEllipse=GLine->Ellipse();
	aR=aEllipse.MajorRadius();
	if (aR<Tol) {
	  done = Standard_True;
	  return;
	}
	break;
      }
      default:
	break;
    }
    //modified by NIZNHY-PKV Wed Nov 02 11:50:23 2011t
    //
    nbvtx = GeomInt_LineTool::NbVertex(L);
    intrvtested = Standard_False;
    for(i=1; i<nbvtx; ++i) { 
      firstp = GeomInt_LineTool::Vertex(L,i).ParameterOnLine();
      lastp =  GeomInt_LineTool::Vertex(L,i+1).ParameterOnLine();
      if(Abs(firstp-lastp)>Precision::PConfusion()) {
	intrvtested = Standard_True;
	const Standard_Real pmid = (firstp+lastp)*0.5;
	gp_Pnt Pmid;
	//modified by NIZNHY-PKV Fri Nov 11 10:27:01 2011f
	GLinePoint(typl, GLine, pmid, Pmid);
	/*
        switch (typl) {
          case IntPatch_Lin:       
	    Pmid = ElCLib::Value(pmid,GLine->Line()); 
	    break;
          case IntPatch_Circle:    
	    Pmid = ElCLib::Value(pmid,GLine->Circle()); 
	    break;
          case IntPatch_Ellipse:   
	    Pmid = ElCLib::Value(pmid,GLine->Ellipse()); 
	    break;
          case IntPatch_Hyperbola: 
	    Pmid = ElCLib::Value(pmid,GLine->Hyperbola()); 
	    break;
          case IntPatch_Parabola:  
	    Pmid = ElCLib::Value(pmid,GLine->Parabola()); 
	    break;
	  default:
	    break;
        }
	*/
	//modified by NIZNHY-PKV Fri Nov 11 12:25:40 2011t
	//
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
    //
    if(typl == IntPatch_Circle || typl == IntPatch_Ellipse)    { 
      firstp = GeomInt_LineTool::Vertex(L,nbvtx).ParameterOnLine();
      lastp  = M_PI + M_PI + GeomInt_LineTool::Vertex(L,1).ParameterOnLine();
      const Standard_Real cadrinf = GeomInt_LineTool::FirstParameter(L);
      const Standard_Real cadrsup = GeomInt_LineTool::LastParameter(L);
      Standard_Real acadr = (firstp+lastp)*0.5;
      while(acadr < cadrinf) { 
	acadr+=M_PI+M_PI; 
      }
      while(acadr > cadrsup) { 
	acadr-=M_PI+M_PI; 
      } 
      if(acadr>=cadrinf && acadr<=cadrsup)      { 
	if(Abs(firstp-lastp)>Precision::PConfusion())        {
	  intrvtested = Standard_True;
	  const Standard_Real pmid = (firstp+lastp)*0.5;
	  gp_Pnt Pmid;
	  if (typl == IntPatch_Circle) {
	    Pmid = ElCLib::Value(pmid,GLine->Circle());
	  }
	  else {
	    Pmid = ElCLib::Value(pmid,GLine->Ellipse());
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
    }      
    if (!intrvtested) {
      // Keep a priori. A point 2d on each
      // surface is required to make the decision. Will be done in the caller
      seqp.Append(GeomInt_LineTool::FirstParameter(L));
      seqp.Append(GeomInt_LineTool::LastParameter(L));
    }
    //
    // Treatment Circles/Ellipses that are the results of intersection
    // between Plane / (Cylinder, Sphere).
    // In these cases the intersection curves can contain 
    // a lot of 'vertices' on the curve that leads to a lot of parts 
    // of the curve. Some adjacent parts can be united to the one part.
    // 
    Standard_Integer aNbParts;
    //
    aNbParts = seqp.Length()/2;
    if (aNbParts > 1 && (typl == IntPatch_Circle || typl == IntPatch_Ellipse))  {
      Standard_Boolean bCond, bPCS, bPCS1, bPCS2, bCC;
      GeomAbs_SurfaceType aST1, aST2;
      //
      aST1 = myHS1->Surface().GetType();
      aST2 = myHS2->Surface().GetType();
      //
      bPCS1=((aST1==GeomAbs_Plane) && (aST2==GeomAbs_Cylinder || aST2==GeomAbs_Sphere));
      bPCS2=((aST2==GeomAbs_Plane) && (aST1==GeomAbs_Cylinder || aST1==GeomAbs_Sphere));
      bPCS=(bPCS1 || bPCS2);
      bCC=(aST1==GeomAbs_Cylinder && aST2==GeomAbs_Cylinder);
      //
      // ZZ
      //modified by NIZNHY-PKV Fri Nov 11 10:13:58 2011f
      Standard_Integer j, i1, i2;
      Standard_Real aT, aU, aV;
      Handle(GeomAdaptor_HSurface) aHS;
      //
      bCond=Standard_False;
      //
      if (bCC) {
	bCond=Standard_True;
      }
      else if (bPCS) {
	if ((aST1==GeomAbs_Sphere) || (aST2==GeomAbs_Sphere)) {
	  if (aST1==GeomAbs_Sphere) {
	    aHS=myHS1;
	  }
	  else if (aST2==GeomAbs_Sphere){
	    aHS=myHS2;
	  }
	  //
	  Standard_Integer aNbP;
	  Standard_Real aHalfPI, aPPC;
	  //
	  bCond=Standard_True;
	  //
	  aNbP=seqp.Length();
	  aPPC=Precision::PConfusion();
	  aHalfPI=0.5*M_PI;
	  i1=0;
	  i2=0;
	  //
	  for (i=1; i<=aNbP; ++i) {
	    gp_Pnt aP;
	  //
	    aT=seqp(i);
	    GLinePoint(typl, GLine, aT, aP);
	    Parameters(aHS, aP, aU, aV);
	    if (aV<0.) {
	      if (fabs(aV+aHalfPI) < aPPC) {
		++i2;
	      }
	    }
	    else {
	      if (fabs(aV-aHalfPI) < aPPC) {
		++i1;
	      }
	    }
	  }
	  if (i1==2 || i2==2) {
	    bCond=Standard_False;
	  }
	} 
      }// else if (bPCS1 || bPCS2) {
      //modified by NIZNHY-PKV Fri Nov 11 10:14:00 2011t
      //
      if (bCond){
	Standard_Integer i2, j2;
	Standard_Real aFi, aLi, aFj, aLj, aF, aL;
	TColStd_SequenceOfReal aSeq;
	//
	aFi=seqp(1);
	aSeq.Append(aFi);
	for (i=1; i<aNbParts; ++i) {
	  j=i+1;
	  i2=2*i;
	  j2=2*j;
	  //
	  aFi=seqp(i2-1);
	  aLi=seqp(i2);
	  //
	  aFj=seqp(j2-1);
	  aLj=seqp(j2);
	  //
	  if (fabs (aFj-aLi) < Tol) {
	    aL=aLj;
	  }
	  else {
	    aL=aLi;
	    aSeq.Append(aL);
	    aF=aFj;
	    aSeq.Append(aF);
	  }
	}
	aSeq.Append(aLj);
	//
	seqp.Clear();
	aNbParts=aSeq.Length();
	for (i=1; i<=aNbParts; ++i) {
	  aF=aSeq(i);
	  seqp.Append(aF);
	}
      }
    }
    //
    done =Standard_True;
    return;
  }// else if (typl != IntPatch_Restriction)  { 

  done = Standard_False;
  seqp.Clear();
  nbvtx = GeomInt_LineTool::NbVertex(L);
  if (nbvtx == 0) { // Keep a priori. Point 2d is required on each
                    // surface to make the decision. Will be done in the caller
    seqp.Append(GeomInt_LineTool::FirstParameter(L));
    seqp.Append(GeomInt_LineTool::LastParameter(L));
    done = Standard_True;
    return;
  }

  GeomInt_SequenceOfParameterAndOrientation seqpss;
  TopAbs_Orientation or1=TopAbs_FORWARD,or2=TopAbs_FORWARD;

  for (i=1; i<=nbvtx; i++)  {
    const IntPatch_Point& thevtx = GeomInt_LineTool::Vertex(L,i);
    const Standard_Real prm = thevtx.ParameterOnLine();
    if (thevtx.IsOnDomS1())    {
      switch (thevtx.TransitionLineArc1().TransitionType())      {
        case IntSurf_In:        or1 = TopAbs_FORWARD; break;  
        case IntSurf_Out:       or1 = TopAbs_REVERSED; break;  
        case IntSurf_Touch:     or1 = TopAbs_INTERNAL; break;  
        case IntSurf_Undecided: or1 = TopAbs_INTERNAL; break;  
      }
    }
    else {
      or1 = TopAbs_INTERNAL;
    }
    
    if (thevtx.IsOnDomS2())    {
      switch (thevtx.TransitionLineArc2().TransitionType())      {
        case IntSurf_In:        or2 = TopAbs_FORWARD; break;
        case IntSurf_Out:       or2 = TopAbs_REVERSED; break;
        case IntSurf_Touch:     or2 = TopAbs_INTERNAL; break;
        case IntSurf_Undecided: or2 = TopAbs_INTERNAL; break;
      }
    }
    else {
      or2 = TopAbs_INTERNAL;
    }
    //
    const Standard_Integer nbinserted = seqpss.Length();
    Standard_Boolean inserted = Standard_False;
    for (Standard_Integer j=1; j<=nbinserted;j++)    {
      if (Abs(prm-seqpss(j).Parameter()) <= Tol)      {
	// accumulate
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
	// insert before position j
	seqpss.InsertBefore(j,GeomInt_ParameterAndOrientation(prm,or1,or2));
	inserted = Standard_True;
	break;
      }
      
    }
    if (!inserted) {
      seqpss.Append(GeomInt_ParameterAndOrientation(prm,or1,or2));
    }
  }

  // determine the state at the beginning of line
  Standard_Boolean trim = Standard_False;
  Standard_Boolean dansS1 = Standard_False;
  Standard_Boolean dansS2 = Standard_False;

  nbvtx = seqpss.Length();
  for (i=1; i<= nbvtx; i++)  {
    or1 = seqpss(i).Orientation1();
    if (or1 != TopAbs_INTERNAL)    {
      trim = Standard_True;
      dansS1 = (or1 != TopAbs_FORWARD);
      break;
    }
  }
  
  if (i > nbvtx)  {
    Standard_Real U,V;
    for (i=1; i<=GeomInt_LineTool::NbVertex(L); i++ )    {
      if (!GeomInt_LineTool::Vertex(L,i).IsOnDomS1() )      {
	GeomInt_LineTool::Vertex(L,i).ParametersOnS1(U,V);
	gp_Pnt2d PPCC(U,V);
	if (myDom1->Classify(PPCC,Tol) == TopAbs_OUT) {
	  done = Standard_True;
	  return;
	}
        break;
      }
    }
    dansS1 = Standard_True; // Keep in doubt
  }
  //
  for (i=1; i<= nbvtx; i++)  {
    or2 = seqpss(i).Orientation2();
    if (or2 != TopAbs_INTERNAL)    {
      trim = Standard_True;
      dansS2 = (or2 != TopAbs_FORWARD);
      break;
    }
  }
  
  if (i > nbvtx)  {
    Standard_Real U,V;
    for (i=1; i<=GeomInt_LineTool::NbVertex(L); i++ )    {
      if (!GeomInt_LineTool::Vertex(L,i).IsOnDomS2() )      {
	GeomInt_LineTool::Vertex(L,i).ParametersOnS2(U,V);
	if (myDom2->Classify(gp_Pnt2d(U,V),Tol) == TopAbs_OUT) {
	  done = Standard_True;
	  return;
	}
        break;
      }
    }
    dansS2 = Standard_True; //  Keep in doubt
  }

  if (!trim) { // necessarily dansS1 == dansS2 == Standard_True
    seqp.Append(GeomInt_LineTool::FirstParameter(L));
    seqp.Append(GeomInt_LineTool::LastParameter(L));
    done = Standard_True;
    return;
  }

  // sequence seqpss is peeled to create valid ends 
  // and store them in seqp(2*i+1) and seqp(2*i+2)
  Standard_Real thefirst = GeomInt_LineTool::FirstParameter(L);
  Standard_Real thelast = GeomInt_LineTool::LastParameter(L);
  firstp = thefirst;

  for (i=1; i<=nbvtx; i++)  {
    or1 = seqpss(i).Orientation1(); 
    or2 = seqpss(i).Orientation2(); 
    if (dansS1 && dansS2)    {
      if (or1 == TopAbs_REVERSED){
	dansS1 = Standard_False;
      }
      
      if (or2 == TopAbs_REVERSED){
	dansS2 = Standard_False;
      }
      if (!dansS1 || !dansS2)      {
	lastp = seqpss(i).Parameter();
	Standard_Real stofirst = Max(firstp, thefirst);
	Standard_Real stolast  = Min(lastp,  thelast) ;

	if (stolast > stofirst) {
	  seqp.Append(stofirst);
	  seqp.Append(stolast);
	}
	if (lastp > thelast) {
	  break;
	}
      }
    }
    else    {
      if (dansS1)      {
	if (or1 == TopAbs_REVERSED) {
	  dansS1 = Standard_False;
	}
      }
      else      {
	if (or1 == TopAbs_FORWARD){
	  dansS1 = Standard_True;
	}
      }
      if (dansS2) {
	if (or2 == TopAbs_REVERSED) {
	  dansS2 = Standard_False;
	}
      }
      else {
	if (or2 == TopAbs_FORWARD){
	  dansS2 = Standard_True;
	}
      }
      if (dansS1 && dansS2){
	firstp = seqpss(i).Parameter();
      }
    }
  }
  //
  // finally to add
  if (dansS1 && dansS2)  {
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
void IntTools_LineConstructor::PeriodicLine (const Handle(IntPatch_Line)& L) const
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
//modified by NIZNHY-PKV Fri Nov 11 10:30:34 2011f
//=======================================================================
//function : Recadre
//purpose  : 
//=======================================================================
void Recadre(const Handle(GeomAdaptor_HSurface)& myHS1,
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
      //-- Case of periodic biparameters is processed upstream
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
      //-- Case of periodic biparameters is processed upstream
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
void Parameters(const Handle(GeomAdaptor_HSurface)& myHS1,
		const Handle(GeomAdaptor_HSurface)& myHS2,
		const gp_Pnt& Ptref,
		Standard_Real& U1,
		Standard_Real& V1,
		Standard_Real& U2,
		Standard_Real& V2)
{
  //modified by NIZNHY-PKV Fri Nov 11 10:07:55 2011f
  Parameters(myHS1, Ptref, U1, V1);
  Parameters(myHS2, Ptref, U2, V2);
  /*
  IntSurf_Quadric quad1,quad2;
  switch (myHS1->Surface().GetType())
  {
    case GeomAbs_Plane:    quad1.SetValue(myHS1->Surface().Plane()); break;
    case GeomAbs_Cylinder: quad1.SetValue(myHS1->Surface().Cylinder()); break;
    case GeomAbs_Cone:     quad1.SetValue(myHS1->Surface().Cone()); break;
    case GeomAbs_Sphere:   quad1.SetValue(myHS1->Surface().Sphere()); break;
    default: Standard_ConstructionError::Raise("IntTools_LineConstructor::Parameters");
  }
  switch (myHS2->Surface().GetType())
  {
    case GeomAbs_Plane:    quad2.SetValue(myHS2->Surface().Plane()); break;
    case GeomAbs_Cylinder: quad2.SetValue(myHS2->Surface().Cylinder()); break;
    case GeomAbs_Cone:     quad2.SetValue(myHS2->Surface().Cone()); break;
    case GeomAbs_Sphere:   quad2.SetValue(myHS2->Surface().Sphere()); break;
    default: Standard_ConstructionError::Raise("IntTools_LineConstructor::Parameters");
  }
  quad1.Parameters(Ptref,U1,V1);
  quad2.Parameters(Ptref,U2,V2);
  */
  //modified by NIZNHY-PKV Fri Nov 11 10:08:38 2011t
}
//modified by NIZNHY-PKV Fri Nov 11 10:06:02 2011f
//=======================================================================
//function : Parameter
//purpose  : 
//=======================================================================
void Parameters(const Handle(GeomAdaptor_HSurface)& myHS1,
		const gp_Pnt& Ptref,
		Standard_Real& U1,
		Standard_Real& V1)
{
  IntSurf_Quadric quad1;
  //
  switch (myHS1->Surface().GetType())  {
    case GeomAbs_Plane:    
      quad1.SetValue(myHS1->Surface().Plane()); 
      break;
    case GeomAbs_Cylinder: 
      quad1.SetValue(myHS1->Surface().Cylinder()); 
      break;
    case GeomAbs_Cone:     
      quad1.SetValue(myHS1->Surface().Cone()); 
      break;
    case GeomAbs_Sphere:   
      quad1.SetValue(myHS1->Surface().Sphere()); 
      break;
    default: 
      Standard_ConstructionError::Raise("IntTools_LineConstructor::Parameters");
  }
  quad1.Parameters(Ptref,U1,V1);
}

//=======================================================================
//function : GLinePoint
//purpose  : 
//=======================================================================
void GLinePoint(const IntPatch_IType typl,
		const Handle(IntPatch_GLine)& GLine,
		const Standard_Real aT,
		gp_Pnt& aP)
{
  switch (typl) {
    case IntPatch_Lin:       
    aP = ElCLib::Value(aT, GLine->Line()); 
    break;
  case IntPatch_Circle:    
    aP = ElCLib::Value(aT, GLine->Circle()); 
    break;
  case IntPatch_Ellipse:   
    aP = ElCLib::Value(aT, GLine->Ellipse()); 
    break;
  case IntPatch_Hyperbola: 
    aP = ElCLib::Value(aT, GLine->Hyperbola()); 
    break;
  case IntPatch_Parabola:  
    aP = ElCLib::Value(aT, GLine->Parabola()); 
    break;
  default:
    Standard_ConstructionError::Raise("IntTools_LineConstructor::Parameters");
  }
}
//modified by NIZNHY-PKV Fri Nov 11 10:06:04 2011t
