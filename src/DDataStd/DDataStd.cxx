// Created on: 1997-03-27
// Created by: Denis PASCAL
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



#include <DDataStd.ixx> 
#include <TCollection_AsciiString.hxx>
#include <TDF_Label.hxx>
#include <TDF_Tool.hxx>
#include <TDataStd.hxx>
#include <TDataXtd.hxx>
#include <TDataStd_RealEnum.hxx>
#include <TDataStd_Real.hxx>
#include <TNaming_NamedShape.hxx>

//=======================================================================
//function : AllCommands
//purpose  : 
//=======================================================================

void DDataStd::AllCommands (Draw_Interpretor& theCommands)
{   
  NamedShapeCommands (theCommands);  
  BasicCommands (theCommands);  
  DatumCommands (theCommands);   
  ConstraintCommands (theCommands); 
  ObjectCommands (theCommands); 
  DrawDisplayCommands (theCommands);
  NameCommands(theCommands); 
  TreeCommands(theCommands);   
}


//=======================================================================
//function : DumpConstraint
//purpose  : 
//=======================================================================

void DDataStd::DumpConstraint (const Handle(TDataXtd_Constraint)& CTR, Standard_OStream& anOS)
{
  TCollection_AsciiString S;
  TDF_Tool::Entry(CTR->Label(),S); 
  anOS << S << " ";
  TDataXtd::Print(CTR->GetType(),anOS); 
  for (Standard_Integer i = 1; i <= CTR->NbGeometries(); i++) {
    anOS << " G_" << i << " (";
    TDF_Tool::Entry(CTR->GetGeometry(i)->Label(),S); 
    anOS << S << ") ";
  }	
  if (CTR->IsPlanar()) {
    anOS << " P (";
    TDF_Tool::Entry(CTR->GetPlane()->Label(),S); 
    anOS << S << ") ";  
  }
  if (CTR->IsDimension()) {
    anOS << " V (";
    TDF_Tool::Entry(CTR->GetValue()->Label(),S); 
    anOS << S << ") ";     
    TDataStd_RealEnum t = CTR->GetValue()->GetDimension();  
    TDataStd::Print(t,anOS); 
    Standard_Real val = CTR->GetValue()->Get();
    if (t == TDataStd_ANGULAR) val = (180.*val)/M_PI;
    anOS << " ";
    anOS << val;
  }
  if (!CTR->Verified()) anOS << " NotVerifed"; 
}
