// Created on: 1996-01-29
// Created by: Jean Yves LEBEY
// Copyright (c) 1996-1999 Matra Datavision
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

#ifdef OCCT_DEBUG

#include <Standard_Type.hxx>
//#include <Standard_OStream.hxx>
//#include <stdio.h>

static Standard_Boolean TopOpeBRep_traceFITOL = Standard_False;
Standard_EXPORT void TopOpeBRep_SettraceFITOL(const Standard_Boolean b) 
{ TopOpeBRep_traceFITOL = b; }
Standard_EXPORT Standard_Boolean TopOpeBRep_GettraceFITOL() 
{ return TopOpeBRep_traceFITOL; }

static Standard_Boolean TopOpeBRep_traceFI = Standard_False;
Standard_EXPORT void TopOpeBRep_SettraceFI(const Standard_Boolean b) 
{ TopOpeBRep_traceFI = b; }
Standard_EXPORT Standard_Boolean TopOpeBRep_GettraceFI() 
{ return TopOpeBRep_traceFI; }

static Standard_Boolean TopOpeBRep_traceSAVFF = Standard_False;
Standard_EXPORT void TopOpeBRep_SettraceSAVFF(const Standard_Boolean b) 
{ TopOpeBRep_traceSAVFF = b; }
Standard_EXPORT Standard_Boolean TopOpeBRep_GettraceSAVFF() 
{ return TopOpeBRep_traceSAVFF; }

static Standard_Boolean TopOpeBRep_tracePROEDG = Standard_False;
Standard_EXPORT void TopOpeBRep_SettracePROEDG(const Standard_Boolean b) 
{ TopOpeBRep_tracePROEDG = b; }
Standard_EXPORT Standard_Boolean TopOpeBRep_GettracePROEDG() 
{ return TopOpeBRep_tracePROEDG; }

static Standard_Boolean TopOpeBRep_contextNOPROEDG = Standard_False;
Standard_EXPORT void TopOpeBRep_SetcontextNOPROEDG(const Standard_Boolean b) 
{ TopOpeBRep_contextNOPROEDG = b; }
Standard_EXPORT Standard_Boolean TopOpeBRep_GetcontextNOPROEDG() 
{ return TopOpeBRep_contextNOPROEDG; }

static Standard_Boolean TopOpeBRep_contextFFOR = Standard_False;
Standard_EXPORT void TopOpeBRep_SetcontextFFOR(const Standard_Boolean b) 
{ TopOpeBRep_contextFFOR = b; }
Standard_EXPORT Standard_Boolean TopOpeBRep_GetcontextFFOR() 
{ return TopOpeBRep_contextFFOR; }

static Standard_Boolean TopOpeBRep_contextNOPUNK = Standard_False;
Standard_EXPORT void TopOpeBRep_SetcontextNOPUNK(const Standard_Boolean b) 
{ TopOpeBRep_contextNOPUNK = b; }
Standard_EXPORT Standard_Boolean TopOpeBRep_GetcontextNOPUNK() 
{ return TopOpeBRep_contextNOPUNK; }

static Standard_Boolean TopOpeBRep_contextNOFEI = Standard_False;
Standard_EXPORT void TopOpeBRep_SetcontextNOFEI(const Standard_Boolean b) 
{ TopOpeBRep_contextNOFEI = b; }
Standard_EXPORT Standard_Boolean TopOpeBRep_GetcontextNOFEI() 
{ return TopOpeBRep_contextNOFEI; }

static Standard_Boolean TopOpeBRep_traceSI = Standard_False;
Standard_EXPORT void TopOpeBRep_SettraceSI(const Standard_Boolean b) { TopOpeBRep_traceSI = b; }
Standard_EXPORT Standard_Boolean TopOpeBRep_GettraceSI() { return TopOpeBRep_traceSI; }

static Standard_Boolean TopOpeBRep_traceBIPS = Standard_False;
Standard_EXPORT void TopOpeBRep_SettraceBIPS(const Standard_Boolean b) { TopOpeBRep_traceBIPS = b; }
Standard_EXPORT Standard_Boolean TopOpeBRep_GettraceBIPS() { return TopOpeBRep_traceBIPS; }

static Standard_Boolean TopOpeBRep_traceCONIC = Standard_False;
Standard_EXPORT void TopOpeBRep_SettraceCONIC(const Standard_Boolean b) { TopOpeBRep_traceCONIC = b; }
Standard_EXPORT Standard_Boolean TopOpeBRep_GettraceCONIC() { return TopOpeBRep_traceCONIC; }

static Standard_Boolean TopOpeBRep_contextREST1 = Standard_False;
Standard_EXPORT void TopOpeBRep_SetcontextREST1(const Standard_Boolean b) { TopOpeBRep_contextREST1 = b; }
Standard_EXPORT Standard_Boolean TopOpeBRep_GetcontextREST1(char* str) { 
  if (TopOpeBRep_contextREST1) {
    cout<<"context REST1 actif"; if (str!=NULL) cout<<str; cout<<endl;
  }
  return TopOpeBRep_contextREST1; 
}
Standard_EXPORT Standard_Boolean TopOpeBRep_GetcontextREST1() { 
  if (TopOpeBRep_contextREST1) cout<<"context REST1 actif"<<endl; 
  return TopOpeBRep_contextREST1; 
}

static Standard_Boolean TopOpeBRep_contextREST2 = Standard_False;
Standard_EXPORT void TopOpeBRep_SetcontextREST2(const Standard_Boolean b) { TopOpeBRep_contextREST2 = b; }
Standard_EXPORT Standard_Boolean TopOpeBRep_GetcontextREST2(char* str) { 
  if (TopOpeBRep_contextREST2) {
    cout<<"context REST2 actif"; if (str!=NULL) cout<<str; cout<<endl;
  }
  return TopOpeBRep_contextREST2; 
}
Standard_EXPORT Standard_Boolean TopOpeBRep_GetcontextREST2() { 
  if (TopOpeBRep_contextREST2) cout<<"context REST2 actif"<<endl; 
  return TopOpeBRep_contextREST2; 
}

static Standard_Boolean TopOpeBRep_contextINL = Standard_False;
Standard_EXPORT void TopOpeBRep_SetcontextINL(const Standard_Boolean b) { TopOpeBRep_contextINL = b; }
Standard_EXPORT Standard_Boolean TopOpeBRep_GetcontextINL(char* str) { 
  if (TopOpeBRep_contextINL) {
    cout<<"context INL actif"; if (str!=NULL) cout<<str; cout<<endl;
  }
  return TopOpeBRep_contextINL; 
}
Standard_EXPORT Standard_Boolean TopOpeBRep_GetcontextINL() { 
  if (TopOpeBRep_contextINL) cout<<"context INL actif"<<endl; 
  return TopOpeBRep_contextINL; 
}

static Standard_Boolean TopOpeBRep_contextNEWKP = Standard_False;
Standard_EXPORT void TopOpeBRep_SetcontextNEWKP(const Standard_Boolean b) { TopOpeBRep_contextNEWKP = b; }
Standard_EXPORT Standard_Boolean TopOpeBRep_GetcontextNEWKP(char* str) { 
  if (TopOpeBRep_contextNEWKP) {
    cout<<"context NEWKP actif"; if (str!=NULL) cout<<str; cout<<endl;
  }
  return TopOpeBRep_contextNEWKP; 
}
Standard_EXPORT Standard_Boolean TopOpeBRep_GetcontextNEWKP() { 
  if (TopOpeBRep_contextNEWKP) cout<<"context NEWKP actif"<<endl; 
  return TopOpeBRep_contextNEWKP; 
}

static Standard_Boolean TopOpeBRep_contextTOL0 = Standard_False;
Standard_EXPORT void TopOpeBRep_SetcontextTOL0(const Standard_Boolean b) { TopOpeBRep_contextTOL0 = b; }
Standard_EXPORT Standard_Boolean TopOpeBRep_GetcontextTOL0(char* /*str*/) { 
  if (TopOpeBRep_contextTOL0) {
  }
  return TopOpeBRep_contextTOL0; 
}
Standard_EXPORT Standard_Boolean TopOpeBRep_GetcontextTOL0() { 
  if (TopOpeBRep_contextTOL0) cout<<"context TOL0 actif"<<endl; 
  return TopOpeBRep_contextTOL0; 
}

static Standard_Boolean TopOpeBRep_contextNONOG = Standard_False;
Standard_EXPORT void TopOpeBRep_SetcontextNONOG(const Standard_Boolean b) { TopOpeBRep_contextNONOG = b; }
Standard_EXPORT Standard_Boolean TopOpeBRep_GetcontextNONOG(char* str) { 
  if (TopOpeBRep_contextNONOG) {
    cout<<"context NONOG actif"; if (str!=NULL) cout<<str; cout<<endl;
  }
  return TopOpeBRep_contextNONOG; 
}
Standard_EXPORT Standard_Boolean TopOpeBRep_GetcontextNONOG() { 
  if (TopOpeBRep_contextNONOG) cout<<"context NONOG actif"<<endl; 
  return TopOpeBRep_contextNONOG; 
}

// #ifdef OCCT_DEBUG
#endif
