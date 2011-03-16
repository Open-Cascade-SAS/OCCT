// File:	DDataStd.cxx
// Created:	Thu Mar 27 09:24:53 1997
// Author:	Denis PASCAL
//		<dp@dingox.paris1.matra-dtv.fr>


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
    if (t == TDataStd_ANGULAR) val = (180.*val)/PI;
    anOS << " ";
    anOS << val;
  }
  if (!CTR->Verified()) anOS << " NotVerifed"; 
}
