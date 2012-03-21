// Created on: 1998-09-21
// Created by: LECLERE Florence
// Copyright (c) 1998-1999 Matra Datavision
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

static Standard_Boolean BRepFeat_traceFEAT = Standard_True;
Standard_EXPORT void BRepFeat_SettraceFEAT(const Standard_Boolean b) 
{ BRepFeat_traceFEAT = b; }
Standard_EXPORT Standard_Boolean BRepFeat_GettraceFEAT() 
{ return BRepFeat_traceFEAT; }

static Standard_Boolean BRepFeat_traceFEATFORM = Standard_False;
Standard_EXPORT void BRepFeat_SettraceFEATFORM(const Standard_Boolean b) 
{ BRepFeat_traceFEATFORM = b; }
Standard_EXPORT Standard_Boolean BRepFeat_GettraceFEATFORM() 
{ return BRepFeat_traceFEATFORM; }

static Standard_Boolean BRepFeat_traceFEATPRISM = Standard_False;
Standard_EXPORT void BRepFeat_SettraceFEATPRISM(const Standard_Boolean b) 
{ BRepFeat_traceFEATPRISM = b; }
Standard_EXPORT Standard_Boolean BRepFeat_GettraceFEATPRISM() 
{ return BRepFeat_traceFEATPRISM; }

static Standard_Boolean BRepFeat_traceFEATRIB = Standard_False;
Standard_EXPORT void BRepFeat_SettraceFEATRIB(const Standard_Boolean b) 
{ BRepFeat_traceFEATRIB = b; }
Standard_EXPORT Standard_Boolean BRepFeat_GettraceFEATRIB() 
{ return BRepFeat_traceFEATRIB; }

static Standard_Boolean BRepFeat_traceFEATDRAFT = Standard_False;
Standard_EXPORT void BRepFeat_SettraceFEATDRAFT(const Standard_Boolean b) 
{ BRepFeat_traceFEATDRAFT = b; }
Standard_EXPORT Standard_Boolean BRepFeat_GettraceFEATDRAFT() 
{ return BRepFeat_traceFEATDRAFT; }

static Standard_Boolean BRepFeat_contextCHRONO = Standard_False;
Standard_EXPORT void BRepFeat_SetcontextCHRONO(const Standard_Boolean b) 
{ BRepFeat_contextCHRONO = b; }
Standard_EXPORT Standard_Boolean BRepFeat_GetcontextCHRONO() {
  Standard_Boolean b = BRepFeat_contextCHRONO;
  if (b) cout<<"context (BRepFeat) CHRONO actif"<<endl;
  return b;
}

#endif


