// Created on: 1992-05-06
// Created by: Jacques GOUSSARD
// Copyright (c) 1992-1999 Matra Datavision
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

#include <IntPatch_Point.ixx>

#include <Standard_DomainError.hxx>
#include <stdio.h>


void IntPatch_Point::SetValue (const gp_Pnt& Pt,
                               const Standard_Real Tol,
                               const Standard_Boolean Tangent)
{
  onS1    = Standard_False;
  onS2    = Standard_False;
  vtxonS1 = Standard_False;
  vtxonS2 = Standard_False;
  mult    = Standard_False;
  tgt     = Tangent;
  pt.SetValue(Pt);
  tol = Tol;
}

void IntPatch_Point::SetVertex(const Standard_Boolean OnFirst, const Handle(Adaptor3d_HVertex)& V)
{
  if (OnFirst) {
    onS1    = Standard_True;
    vtxonS1 = Standard_True;
    vS1     = V;
  }
  else {
    onS2    = Standard_True;
    vtxonS2 = Standard_True;
    vS2   = V;
  }
}

void IntPatch_Point::SetArc (const Standard_Boolean OnFirst,
                             const Handle(Adaptor2d_HCurve2d)& A,
                             const Standard_Real Param,
                             const IntSurf_Transition& TLine,
                             const IntSurf_Transition& TArc)
{
  if (OnFirst) {
    onS1     = Standard_True;
    arcS1    = A;
    traline1 = TLine;
    tra1     = TArc;
    prm1     = Param;
  }
  else {
    onS2     = Standard_True;
    arcS2    = A;
    traline2 = TLine;
    tra2     = TArc;
    prm2     = Param;
  }
}

void IntPatch_Point::ReverseTransition()
{ 
  if(onS1)
  {
    IntSurf_Transition TLine;
    switch (traline1.TransitionType())
    {
    case IntSurf_In: TLine.SetValue(Standard_False,IntSurf_Out); break;
    case IntSurf_Out: TLine.SetValue(Standard_False,IntSurf_In); break;
    default:
      break;
    }
    traline1=TLine;
    IntSurf_Transition TArc;
    switch (tra1.TransitionType())
    {
    case IntSurf_In: TArc.SetValue(Standard_False,IntSurf_Out); break;
    case IntSurf_Out: TArc.SetValue(Standard_False,IntSurf_In); break;
    default:
      break;
    }
    tra1=TArc;
  }
  if(onS2)
  {
    IntSurf_Transition TLine;
    switch (traline2.TransitionType())
    {
    case IntSurf_In: TLine.SetValue(Standard_False,IntSurf_Out); break;
    case IntSurf_Out: TLine.SetValue(Standard_False,IntSurf_In); break;
    default:
      break;
    }
    traline2=TLine;
    IntSurf_Transition TArc;
    switch (tra2.TransitionType())
    {
    case IntSurf_In: TArc.SetValue(Standard_False,IntSurf_Out); break;
    case IntSurf_Out: TArc.SetValue(Standard_False,IntSurf_In); break;
    default:
      break;
    }
    tra2=TArc;
  }  
}


#include <IntSurf_Situation.hxx>
#include <IntSurf_Transition.hxx>
#include <IntSurf_TypeTrans.hxx>

static void DumpTransition(const IntSurf_Transition& T)
{
  IntSurf_TypeTrans typetrans = T.TransitionType(); 
  if(typetrans == IntSurf_In)             cout<<" In  ";
  else if(typetrans == IntSurf_Out)       cout<<" Out ";
  else if(typetrans == IntSurf_Undecided) cout<<" Und.";
  else if(typetrans == IntSurf_Touch) { 
    cout<<" Touch ";
    if(T.Situation() == IntSurf_Inside)       cout<<" Inside ";
    else if(T.Situation() == IntSurf_Outside) cout<<" Outside";
    else                                      cout<<" Unknown";
    if(T.IsOpposite()) cout<<"     Opposite "; 
    else               cout<<" Non_Opposite ";
  }

  if(typetrans != IntSurf_Undecided) { 
    if(T.IsTangent()) cout<<" Tangent    ";
    else              cout<<" Non_Tangent";
  }
}
    
#include <Standard_Transient.hxx>

void IntPatch_Point::Dump() const { 
  cout<<"----------- IntPatch_Point : "<<endl;
  Standard_Real u1,v1,u2,v2;
  pt.Parameters(u1,v1,u2,v2);
#if 0 
  cout<<"IntSurf_PntOn2S :   Pnt ("<<pt.Value().X()
    <<","<<pt.Value().Y()<<","<<pt.Value().Z()<<")"<<endl;
  cout<<" u1:"<<u1;  cout<<" u2:"<<u2;
  cout<<" v1:"<<v1;
  cout<<" v2:"<<v2<<endl;
  cout<<"  para("<<para<<")";
  cout<<"  mult("<<mult<<")";
  cout<<"  OnS1("<<onS1<<")";
  cout<<"  VtxOnS1("<<vtxonS1<<")"<<endl;
  cout<<"  OnS2("<<onS2<<")";
  cout<<"  VtxOnS2("<<vtxonS2<<")";
  cout<<"  prm1("<<prm1<<")";
  cout<<"  prm2("<<prm2<<")";
#else

#ifndef DEB //  for porting on linux
  printf("P(%15.10f,%15.10f,%15.10f) UV1(%15.10f,%15.10f)  UV2(%15.10f,%15.10f) (Para:%15.10f)\n",
	 (double)(pt.Value().X()),
	 (double)(pt.Value().Y()),
	 (double)(pt.Value().Z()),
	 (double)u1,(double)v1,(double)u2,(double)v2,(double)para);
  if(onS1)    { printf("*OnS1*  par=%15.10f arc1=%10lu",(double)prm1,
		       *((long unsigned *)(((Handle_Standard_Transient *)(&arcS1))))
		       );  }
  if(vtxonS1) { printf(" *Vtx1*  vtx1=%10lu",
		       *((long unsigned *)(((Handle_Standard_Transient *)(&vS1))))); } 
  if(onS1 || vtxonS1) printf("\n");
  if(onS2)    { printf("*OnS2*  par=%15.10f arc2=%10lu",(double)prm2,
		       *((long unsigned *)(((Handle_Standard_Transient *)(&arcS2))))); } 
  if(vtxonS2) { printf(" *Vtx2*  vtx2=%10lu",
		       *((long unsigned *)(((Handle_Standard_Transient *)(&vS2))))); } 
  
#else // debug mode
  printf("P(%15.10lf,%15.10lf,%15.10lf) UV1(%15.10lf,%15.10lf)  UV2(%15.10lf,%15.10lf) (Para:%15.10lf)\n",
	 (double)(pt.Value().X()),
	 (double)(pt.Value().Y()),
	 (double)(pt.Value().Z()),
	 (double)u1,(double)v1,(double)u2,(double)v2,(double)para);
  if(onS1)    { printf("*OnS1*  par=%15.10lf arc1=%10lu",(double)prm1,
		       *((long unsigned *)(((Handle_Standard_Transient *)(&arcS1))))
		       );  }
  if(vtxonS1) { printf(" *Vtx1*  vtx1=%10lu",
		       *((long unsigned *)(((Handle_Standard_Transient *)(&vS1))))); } 
  if(onS1 || vtxonS1) printf("\n");
  if(onS2)    { printf("*OnS2*  par=%15.10lf arc2=%10lu",(double)prm2,
		       *((long unsigned *)(((Handle_Standard_Transient *)(&arcS2))))); } 
  if(vtxonS2) { printf(" *Vtx2*  vtx2=%10lu",
		       *((long unsigned *)(((Handle_Standard_Transient *)(&vS2))))); } 
  
#endif
  if(onS2 || vtxonS2) printf("\n");
  fflush(stdout);
  
#endif  
  if(onS1 || onS2) { 
    cout<<" tgt:"<<((tgt)? 1 : 0) <<"   mult:"<<((mult)? 1 :0);
    if(onS1) { 
      cout<<"\n-traline1  : ";   DumpTransition(traline1);
      cout<<"  -tra1  : ";       DumpTransition(tra1);
    }
    if(onS2) { 
      cout<<"\n-traline2  : ";   DumpTransition(traline2);
      cout<<"  -tra2  : ";      DumpTransition(tra2);
    }
    cout<<endl;
  }
}
