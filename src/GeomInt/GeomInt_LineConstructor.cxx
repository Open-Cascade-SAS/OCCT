// Created on: 1995-02-07
// Created by: Jacques GOUSSARD
// Copyright (c) 1995-1999 Matra Datavision
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
#include <Adaptor3d_TopolTool.hxx>
#include <ElCLib.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <GeomInt.hxx>
#include <GeomInt_LineConstructor.hxx>
#include <GeomInt_LineTool.hxx>
#include <GeomInt_ParameterAndOrientation.hxx>
#include <GeomInt_SequenceOfParameterAndOrientation.hxx>
#include <gp_Pnt2d.hxx>
#include <IntPatch_ALine.hxx>
#include <IntPatch_GLine.hxx>
#include <IntPatch_Line.hxx>
#include <IntPatch_Point.hxx>
#include <IntPatch_WLine.hxx>
#include <IntSurf_PntOn2S.hxx>
#include <IntSurf_Quadric.hxx>
#include <IntSurf_Transition.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>
#include <TopAbs_Orientation.hxx>

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

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//
//=======================================================================
//class    : GeomInt_RealWithFlag
//purpose  : 
//=======================================================================
class GeomInt_RealWithFlag {
 public:
  GeomInt_RealWithFlag() : 
    myValue(-99.), myFlag(1) 
  {
  };
  //
  ~GeomInt_RealWithFlag() {
  };
  //
  void SetValue(const Standard_Real aT) {
    myValue=aT;
  };
  //
  Standard_Real Value() const {
    return myValue;
  }
  //
  void SetFlag(const Standard_Integer aFlag) {
    myFlag=aFlag;
  };
  //
  Standard_Integer Flag() const {
    return myFlag;
  }
  //
  Standard_Boolean operator < (const GeomInt_RealWithFlag& aOther) {
    return myValue<aOther.myValue;
  }
  //
 protected:
  Standard_Real myValue;
  Standard_Integer myFlag;
};
//------------
static 
  void SortShell(const Standard_Integer, 
                 GeomInt_RealWithFlag *); 
static
  void RejectDuplicates(Standard_Integer& aNbVtx, 
                        GeomInt_RealWithFlag *pVtx, 
                        Standard_Real aTolPrm);
static
  void RejectNearBeacons(Standard_Integer& aNbVtx, 
                         GeomInt_RealWithFlag *pVtx, 
                         Standard_Real aTolPC1,
                         const GeomAbs_SurfaceType aTS1,
                         const GeomAbs_SurfaceType aTS2); 
static
  Standard_Real AdjustOnPeriod(const Standard_Real aTr,
                               const Standard_Real aPeriod);

static
  Standard_Boolean RejectMicroCircle(const Handle(IntPatch_GLine)& aGLine,
                                     const IntPatch_IType aType,
                                     const Standard_Real aTol3D);

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
  if(typl == IntPatch_Analytic)  {
    Standard_Real u1,v1,u2,v2;
    Handle(IntPatch_ALine) ALine (Handle(IntPatch_ALine)::DownCast (L));
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
    Handle(IntPatch_WLine) WLine (Handle(IntPatch_WLine)::DownCast (L));
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
  //-----------------------------------------------------------
  else if (typl != IntPatch_Restriction)  {
    seqp.Clear();
    //
    Handle(IntPatch_GLine) GLine (Handle(IntPatch_GLine)::DownCast (L));
    //
    if(typl == IntPatch_Circle || typl == IntPatch_Ellipse) { 
      TreatCircle(L, Tol);
      done=Standard_True;
      return;
    }
    //----------------------------
    Standard_Boolean intrvtested;
    Standard_Real u1,v1,u2,v2;
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
        GLinePoint(typl, GLine, pmid, Pmid);
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
    if (!intrvtested) {
      // Keep a priori. A point 2d on each
      // surface is required to make the decision. Will be done in the caller
      seqp.Append(GeomInt_LineTool::FirstParameter(L));
      seqp.Append(GeomInt_LineTool::LastParameter(L));
    }
    //
    done =Standard_True;
    return;
  } // else if (typl != IntPatch_Restriction)  { 

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
  Standard_Real du, dv;
  //
  if(myHS1IsUPeriodic) {
    const Standard_Real lmf = M_PI+M_PI; //-- myHS1->UPeriod();
    const Standard_Real f = myHS1->FirstUParameter();
    const Standard_Real l = myHS1->LastUParameter();
    GeomInt::AdjustPeriodic(u1, f, l, lmf, u1, du);
  }
  if(myHS1IsVPeriodic) {
    const Standard_Real lmf = M_PI+M_PI; //-- myHS1->VPeriod(); 
    const Standard_Real f = myHS1->FirstVParameter();
    const Standard_Real l = myHS1->LastVParameter();
    GeomInt::AdjustPeriodic(v1, f, l, lmf, v1, dv);
  }
  if(myHS2IsUPeriodic) { 
    const Standard_Real lmf = M_PI+M_PI; //-- myHS2->UPeriod();
    const Standard_Real f = myHS2->FirstUParameter();
    const Standard_Real l = myHS2->LastUParameter();
    GeomInt::AdjustPeriodic(u2, f, l, lmf, u2, du);
  }
  if(myHS2IsVPeriodic) { 
    const Standard_Real lmf = M_PI+M_PI; //-- myHS2->VPeriod();
    const Standard_Real f = myHS2->FirstVParameter();
    const Standard_Real l = myHS2->LastVParameter();
    GeomInt::AdjustPeriodic(v2, f, l, lmf, v2, dv);
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
  Parameters(myHS1, Ptref, U1, V1);
  Parameters(myHS2, Ptref, U2, V2);
}
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
    case GeomAbs_Torus:
      quad1.SetValue(myHS1->Surface().Torus()); 
      break;
    default: 
      throw Standard_ConstructionError("GeomInt_LineConstructor::Parameters");
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
    throw Standard_ConstructionError("GeomInt_LineConstructor::Parameters");
  }
}

//=======================================================================
//function : TreatCircle
//purpose  : 
//=======================================================================
void GeomInt_LineConstructor::TreatCircle(const Handle(IntPatch_Line)& aLine,
                                           const Standard_Real aTol)
{  
  Standard_Boolean bRejected;
  IntPatch_IType aType;
  //
  aType=aLine->ArcType();
  Handle(IntPatch_GLine) aGLine (Handle(IntPatch_GLine)::DownCast (aLine));
  //
  bRejected=RejectMicroCircle(aGLine, aType, aTol);
  if (bRejected) {
    return;
  }
  //----------------------------------------
  Standard_Boolean bFound;
  Standard_Integer aNbVtx, aNbVtxWas, i;
  Standard_Real aTolPC, aT, aT1, aT2, aTmid, aTwoPI, aTolPC1;
  Standard_Real aU1, aV1, aU2, aV2;
  TopAbs_State aIn1, aIn2;
  GeomAbs_SurfaceType aTS1, aTS2;
  gp_Pnt aPmid;
  gp_Pnt2d aP2D;
  GeomInt_RealWithFlag *pVtx;
  //-------------------------------------1
  aTwoPI=M_PI+M_PI;
  aTolPC=Precision::PConfusion();
  aNbVtxWas=GeomInt_LineTool::NbVertex(aLine);
  
  aNbVtx=aNbVtxWas+2;
  //-------------------------------------2
  aTS1=myHS1->GetType();
  aTS2=myHS2->GetType();
  //
  // About the value aTolPC1=1000.*aTolPC,
  // see IntPatch_GLine.cxx, line:398
  // for more details;
  aTolPC1=1000.*aTolPC; 
  //-------------------------------------
  //
  pVtx=new GeomInt_RealWithFlag [aNbVtx];
  //
  pVtx[0].SetValue(0.);
  pVtx[1].SetValue(aTwoPI);
  //
  for(i=1; i<=aNbVtxWas; ++i) {
    aT=GeomInt_LineTool::Vertex(aLine, i).ParameterOnLine();
    aT=AdjustOnPeriod(aT, aTwoPI);
    pVtx[i+1].SetValue(aT);
  }
  //
  SortShell(aNbVtx, pVtx);
  //
  RejectNearBeacons(aNbVtx, pVtx, aTolPC1, aTS1, aTS2);
  //
  RejectDuplicates(aNbVtx, pVtx, aTolPC);
  //
  if ((aType==IntPatch_Circle || aType==IntPatch_Ellipse)&& aNbVtx>2) { // zz
    bFound=Standard_False;
    for(i=1; i<=aNbVtxWas; ++i) {
      aT=GeomInt_LineTool::Vertex(aLine, i).ParameterOnLine();
      if (fabs(aT) < aTolPC1 || fabs(aT-aTwoPI) < aTolPC1) {
        bFound=!bFound;
        break;
      }
    }
    if (!bFound) {
      aT=pVtx[1].Value()+aTwoPI;
      pVtx[aNbVtx-1].SetValue(aT);
      //
      for(i=0; i<aNbVtx - 1; ++i) { 
        aT=pVtx[i+1].Value();
        pVtx[i].SetValue(aT);
      }
      --aNbVtx;
    }
  }
  //
  for(i=0; i<aNbVtx-1; ++i) { 
    aT1=pVtx[i].Value();
    aT2=pVtx[i+1].Value();
    aTmid=(aT1+aT2)*0.5;
    GLinePoint(aType, aGLine, aTmid, aPmid);
    //
    Parameters(myHS1, myHS2, aPmid, aU1, aV1, aU2, aV2);
    Recadre(myHS1, myHS2, aU1, aV1, aU2, aV2);
    //
    aP2D.SetCoord(aU1, aV1);
    aIn1=myDom1->Classify(aP2D, aTol);
    if(aIn1 !=  TopAbs_OUT) { 
      aP2D.SetCoord(aU2, aV2);
      aIn2=myDom2->Classify(aP2D, aTol);
      if(aIn2 != TopAbs_OUT) { 
        seqp.Append(aT1);
        seqp.Append(aT2);
      }
    }
  }
  //
  delete [] pVtx;
}
//=======================================================================
//function : RejectNearBeacons
//purpose  : Reject the thickenings near the beacon points (if exist)
//           The gifts, made by sweep algo.  
//           chl/930/B5 B8 C2 C5 E2 E5 E8 F2 G8 H2 H5 H8
//=======================================================================
void RejectNearBeacons(Standard_Integer& aNbVtx, 
                       GeomInt_RealWithFlag *pVtx, 
                       Standard_Real aTolPC1,
                       const GeomAbs_SurfaceType aTS1,
                       const GeomAbs_SurfaceType aTS2) 
{
  Standard_Integer i, j, iBcn;
  Standard_Real aT, aBcn[2];
  //
  if (aTS1==GeomAbs_Cylinder && aTS2==GeomAbs_Cylinder) {
    aBcn[0]=0.5*M_PI;
    aBcn[1]=1.5*M_PI;
    //
    for (j=0; j<2; ++j) {
      iBcn=-1;
      for(i=0; i<aNbVtx; ++i) {
        aT=pVtx[i].Value();
        if (aT==aBcn[j]) {
          iBcn=i;
          break;
        }
      }
      //
      if (iBcn<0) {
        // The beacon is not found
        continue;
      }
      //  
      for(i=0; i<aNbVtx; ++i) {
        if (i!=iBcn) {
          aT=pVtx[i].Value();
          if (fabs(aT-aBcn[j]) < aTolPC1) {
            pVtx[i].SetFlag(0);
          }
        }
      }
    }// for (j=0; j<2; ++j) {
    //------------------------------------------
    j=0;
    for(i=0; i<aNbVtx; ++i) {
      if (pVtx[i].Flag()) {
        pVtx[j]=pVtx[i];
        ++j;
      }
    }
    aNbVtx=j;
  }// if (aTS1==GeomAbs_Cylinder && aTS2==GeomAbs_Cylinder) {
}

//=======================================================================
//function : RejectDuplicates
//purpose  : 
//=======================================================================
void RejectDuplicates(Standard_Integer& aNbVtx, 
                      GeomInt_RealWithFlag *pVtx, 
                      Standard_Real aTolPC) 
{
  Standard_Integer i, j;
  Standard_Real dX, aT1, aT2; 
  //
  for(i=0; i<aNbVtx-1; ++i) {
    aT2=pVtx[i+1].Value();
    aT1=pVtx[i].Value();
    dX=aT2-aT1;
    if (dX<aTolPC) {
      pVtx[i+1].SetFlag(0);
    }
  }
  //
  j=0;
  for(i=0; i<aNbVtx; ++i) {
    if (pVtx[i].Flag()) {
      pVtx[j]=pVtx[i];
      ++j;
    }
  }
  aNbVtx=j;
}
//=======================================================================
//function : AdjustOnPeriod
//purpose  : 
//=======================================================================
Standard_Real AdjustOnPeriod(const Standard_Real aTr,
                             const Standard_Real aPeriod)
{
  Standard_Integer k;
  Standard_Real aT;
  //
  aT=aTr;
  if (aT<0.) {
    k=-(Standard_Integer)(aT/aPeriod)+1;
    aT=aT+k*aPeriod;
  }
  //
  if (!(aT>=0. && aT<=aPeriod)) {
    k=(Standard_Integer)(aT/aPeriod);
    aT=aT-k*aPeriod;
  }
  //
  return aT;
}
//=======================================================================
//function : RejectMicroCrcles
//purpose  : 
//=======================================================================
Standard_Boolean RejectMicroCircle(const Handle(IntPatch_GLine)& aGLine,
                                   const IntPatch_IType aType,
                                   const Standard_Real aTol3D)
{
  Standard_Boolean bRet;
  Standard_Real aR;
  //
  bRet=Standard_False;
  //
  if (aType==IntPatch_Circle) {
    aR=aGLine->Circle().Radius();
    bRet=(aR<aTol3D);
  }
  else if (aType==IntPatch_Ellipse) {
    aR=aGLine->Ellipse().MajorRadius();
    bRet=(aR<aTol3D);
  }
  return bRet;
}
//=======================================================================
// function: SortShell
// purpose : 
//=======================================================================
void SortShell(const Standard_Integer n, 
               GeomInt_RealWithFlag *a) 
{
  Standard_Integer nd, i, j, l, d=1;
  GeomInt_RealWithFlag x;
  //
  while(d<=n) {
    d*=2;
  }
  //
  while (d) {
    d=(d-1)/2;
    //
    nd=n-d;
    for (i=0; i<nd; ++i) {
      j=i;
    m30:;
      l=j+d;
      if (a[l] < a[j]){
        x=a[j];
        a[j]=a[l];
        a[l]=x;
        j-=d;
        if (j > -1) goto m30;
      }//if (a[l] < a[j]){
    }//for (i=0; i<nd; ++i) 
  }//while (1)
}
