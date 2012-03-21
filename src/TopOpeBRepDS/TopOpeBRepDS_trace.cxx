// Created on: 1994-03-18
// Created by: Jean Yves LEBEY
// Copyright (c) 1994-1999 Matra Datavision
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


#ifdef DEB

#include <Standard_Type.hxx>

static Standard_Boolean TopOpeBRepDS_traceBUTO = Standard_False;
Standard_EXPORT void TopOpeBRepDS_SettraceBUTO(const Standard_Boolean b) 
{ TopOpeBRepDS_traceBUTO = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepDS_GettraceBUTO() 
{ return TopOpeBRepDS_traceBUTO; }

static Standard_Boolean TopOpeBRepDS_traceTRPE = Standard_False;
Standard_EXPORT void TopOpeBRepDS_SettraceTRPE(const Standard_Boolean b) 
{ TopOpeBRepDS_traceTRPE = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepDS_GettraceTRPE() 
{ return TopOpeBRepDS_traceTRPE; }

static Standard_Boolean TopOpeBRepDS_traceEDPR = Standard_False;
Standard_EXPORT void TopOpeBRepDS_SettraceEDPR(const Standard_Boolean b) 
{ TopOpeBRepDS_traceEDPR = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepDS_GettraceEDPR() 
{ return TopOpeBRepDS_traceEDPR; }

static Standard_Boolean TopOpeBRepDS_traceISTO = Standard_False;
Standard_EXPORT void TopOpeBRepDS_SettraceISTO(const Standard_Boolean b) 
{ TopOpeBRepDS_traceISTO = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepDS_GettraceISTO() 
{ return TopOpeBRepDS_traceISTO; }

static Standard_Boolean TopOpeBRepDS_traceSANTRAN = Standard_False;
Standard_EXPORT void TopOpeBRepDS_SettraceSANTRAN(const Standard_Boolean b) 
{ TopOpeBRepDS_traceSANTRAN = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepDS_GettraceSANTRAN() 
{ return TopOpeBRepDS_traceSANTRAN; }

static Standard_Boolean TopOpeBRepDS_contextNOPFI = Standard_False;
Standard_EXPORT void TopOpeBRepDS_SetcontextNOPFI(const Standard_Boolean b) 
{ TopOpeBRepDS_contextNOPFI = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepDS_GetcontextNOPFI() 
{ return TopOpeBRepDS_contextNOPFI; }

static Standard_Boolean TopOpeBRepDS_contextMKTONREG = Standard_False;
Standard_EXPORT void TopOpeBRepDS_SetcontextMKTONREG(const Standard_Boolean b) 
{ TopOpeBRepDS_contextMKTONREG = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepDS_GetcontextMKTONREG() 
{ return TopOpeBRepDS_contextMKTONREG; }

static Standard_Boolean TopOpeBRepDS_contextNOPNC = Standard_False;
Standard_EXPORT void TopOpeBRepDS_SetcontextNOPNC(const Standard_Boolean b) 
{ TopOpeBRepDS_contextNOPNC = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepDS_GetcontextNOPNC() 
{ return TopOpeBRepDS_contextNOPNC; }

static Standard_Boolean TopOpeBRepDS_traceDSNC = Standard_False;
Standard_EXPORT void TopOpeBRepDS_SettraceDSNC(const Standard_Boolean b)
{ TopOpeBRepDS_traceDSNC = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepDS_GettraceDSNC()
{ return TopOpeBRepDS_traceDSNC; }

static Standard_Boolean TopOpeBRepDS_traceDSF = Standard_False;
Standard_EXPORT void TopOpeBRepDS_SettraceDSF(const Standard_Boolean b)
{ TopOpeBRepDS_traceDSF = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepDS_GettraceDSF()
{ return TopOpeBRepDS_traceDSF; }

static Standard_Boolean TopOpeBRepDS_traceDSP = Standard_False;
Standard_EXPORT void TopOpeBRepDS_SettraceDSP(const Standard_Boolean b)
{ TopOpeBRepDS_traceDSP = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepDS_GettraceDSP()
{ return TopOpeBRepDS_traceDSP; }

static Standard_Boolean TopOpeBRepDS_traceEDSF = Standard_False;
Standard_EXPORT void TopOpeBRepDS_SettraceEDSF(const Standard_Boolean b)
{ TopOpeBRepDS_traceEDSF = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepDS_GettraceEDSF()
{ return TopOpeBRepDS_traceEDSF; }

static Standard_Boolean TopOpeBRepDS_traceDSFD = Standard_False;
Standard_EXPORT void TopOpeBRepDS_SettraceDSFD(const Standard_Boolean b)
{ TopOpeBRepDS_traceDSFD = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepDS_GettraceDSFD()
{ return TopOpeBRepDS_traceDSFD; }

static Standard_Boolean TopOpeBRepDS_traceDSFK = Standard_False;
Standard_EXPORT void TopOpeBRepDS_SettraceDSFK(const Standard_Boolean b) { TopOpeBRepDS_traceDSFK = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepDS_GettraceDSFK() { return TopOpeBRepDS_traceDSFK; }

static Standard_Boolean TopOpeBRepDS_traceDSLT = Standard_False;
Standard_EXPORT void TopOpeBRepDS_SettraceDSLT(const Standard_Boolean b) { TopOpeBRepDS_traceDSLT = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepDS_GettraceDSLT() { return TopOpeBRepDS_traceDSLT; }

static Standard_Boolean TopOpeBRepDS_traceDEGEN = Standard_False;
Standard_EXPORT void TopOpeBRepDS_SettraceDEGEN(const Standard_Boolean b)
{ TopOpeBRepDS_traceDEGEN = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepDS_GettraceDEGEN()
{ return TopOpeBRepDS_traceDEGEN; }

static Standard_Boolean TopOpeBRepDS_tracePCI = Standard_False;
Standard_EXPORT void TopOpeBRepDS_SettracePCI(const Standard_Boolean b) { TopOpeBRepDS_tracePCI = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepDS_GettracePCI() { return TopOpeBRepDS_tracePCI; }

static Standard_Boolean TopOpeBRepDS_tracePEI = Standard_False;
Standard_EXPORT void TopOpeBRepDS_SettracePEI(const Standard_Boolean b) { TopOpeBRepDS_tracePEI = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepDS_GettracePEI() { return TopOpeBRepDS_tracePEI; }

static Standard_Boolean TopOpeBRepDS_tracePFI = Standard_False;
Standard_EXPORT void TopOpeBRepDS_SettracePFI(const Standard_Boolean b) { TopOpeBRepDS_tracePFI = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepDS_GettracePFI() { return TopOpeBRepDS_tracePFI; }

static Standard_Boolean TopOpeBRepDS_tracePI = Standard_False;
Standard_EXPORT void TopOpeBRepDS_SettracePI(const Standard_Boolean b) { TopOpeBRepDS_tracePI = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepDS_GettracePI() { return TopOpeBRepDS_tracePI; }

static Standard_Boolean TopOpeBRepDS_traceSTRANGE = Standard_False;
Standard_EXPORT void TopOpeBRepDS_SettraceSTRANGE(const Standard_Boolean b) {TopOpeBRepDS_traceSTRANGE = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepDS_GettraceSTRANGE() { return TopOpeBRepDS_traceSTRANGE; }

static Standard_Boolean TopOpeBRepDS_traceGap = Standard_False;
Standard_EXPORT void TopOpeBRepDS_SettraceGAP(const Standard_Boolean b) {TopOpeBRepDS_traceGap = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepDS_GettraceGAP() { return TopOpeBRepDS_traceGap; }

static Standard_Boolean TopOpeBRepDS_contextNOGAP = Standard_False;
Standard_EXPORT void TopOpeBRepDS_SetcontextNOGAP(const Standard_Boolean b) {TopOpeBRepDS_contextNOGAP = b; }
Standard_EXPORT Standard_Boolean TopOpeBRepDS_GetcontextNOGAP(char* str) { 
  if (TopOpeBRepDS_contextNOGAP) {
    cout<<"context NOGAP actif"; if (str!=NULL) cout<<str; cout<<endl;
  }
  return TopOpeBRepDS_contextNOGAP; 
}
Standard_EXPORT Standard_Boolean TopOpeBRepDS_GetcontextNOGAP() { 
  if (TopOpeBRepDS_contextNOGAP) cout<<"context NOGAP actif"<<endl; 
  return TopOpeBRepDS_contextNOGAP; 
}


// #ifdef DEB
#endif
