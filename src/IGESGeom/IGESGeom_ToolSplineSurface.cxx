// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
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

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <IGESBasic_HArray2OfHArray1OfReal.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_Dump.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESGeom_SplineSurface.hxx>
#include <IGESGeom_ToolSplineSurface.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Message_Msg.hxx>
#include <Standard_DomainError.hxx>
#include <TColStd_HArray1OfReal.hxx>

// MGE 30/07/98
IGESGeom_ToolSplineSurface::IGESGeom_ToolSplineSurface ()    {  }


void IGESGeom_ToolSplineSurface::ReadOwnParams
  (const Handle(IGESGeom_SplineSurface)& ent,
   const Handle(IGESData_IGESReaderData)& /* IR */, IGESData_ParamReader& PR) const
{

  // MGE 30/07/98

  Standard_Integer aBoundaryType, aPatchType, allNbUSegments, allNbVSegments;
  Standard_Integer i, j, k;
  Standard_Boolean ubreak=Standard_False, vbreak=Standard_False;
  Handle(TColStd_HArray1OfReal) allUBreakPoints;
  Handle(TColStd_HArray1OfReal) allVBreakPoints;
  Handle(IGESBasic_HArray2OfHArray1OfReal) allXCoeffs;
  Handle(IGESBasic_HArray2OfHArray1OfReal) allYCoeffs;
  Handle(IGESBasic_HArray2OfHArray1OfReal) allZCoeffs;

  //Standard_Boolean st; //szv#4:S4163:12Mar99 moved down

  //szv#4:S4163:12Mar99 `st=` not needed
  if (!PR.ReadInteger(PR.Current(), aBoundaryType)){
    Message_Msg Msg140("XSTEP_140");
    PR.SendFail(Msg140);
  }
  if (!PR.ReadInteger(PR.Current(), aPatchType)){
    Message_Msg Msg278("XSTEP_278");
    PR.SendFail(Msg278);
  }
  //st = PR.ReadInteger(PR.Current(), Msg141, allNbUSegments); //szv#4:S4163:12Mar99 moved in if
  if (PR.ReadInteger(PR.Current(),allNbUSegments)) {
    ubreak = Standard_True;
    allUBreakPoints = new TColStd_HArray1OfReal(1, allNbUSegments+1);
  }
  else{
    Message_Msg Msg141("XSTEP_141");
    PR.SendFail(Msg141);
  }

  //st = PR.ReadInteger(PR.Current(), Msg142, allNbVSegments); //szv#4:S4163:12Mar99 moved in if
  //st = PR.ReadInteger(PR.Current(), "Number Of V Segments", allNbVSegments);
  if (PR.ReadInteger(PR.Current(),allNbVSegments)) {
    vbreak = Standard_True;
    allVBreakPoints = new TColStd_HArray1OfReal(1, allNbVSegments+1);
  }
  else{
    Message_Msg Msg142("XSTEP_142");
    PR.SendFail(Msg142);
  }


  if (!allUBreakPoints.IsNull()){
    Message_Msg Msg143("XSTEP_143");
    PR.ReadReals(PR.CurrentList(allNbUSegments+1), Msg143, allUBreakPoints); //szv#4:S4163:12Mar99 `st=` not needed
    //st = PR.ReadReals(PR.CurrentList(allNbUSegments+1), "U Break Points", allUBreakPoints);
  }

  if (!allVBreakPoints.IsNull()){
    Message_Msg Msg144("XSTEP_144");
    PR.ReadReals(PR.CurrentList(allNbVSegments+1), Msg144, allVBreakPoints); //szv#4:S4163:12Mar99 `st=` not needed
    //st = PR.ReadReals(PR.CurrentList(allNbVSegments+1), "V Break Points", allVBreakPoints);
  }

  if (ubreak && vbreak)
    {
      allXCoeffs = new IGESBasic_HArray2OfHArray1OfReal(1, allNbUSegments, 1, allNbVSegments);
      allYCoeffs = new IGESBasic_HArray2OfHArray1OfReal(1, allNbUSegments, 1, allNbVSegments);
      allZCoeffs = new IGESBasic_HArray2OfHArray1OfReal(1, allNbUSegments, 1, allNbVSegments);
    }

  Handle(TColStd_HArray1OfReal) Temp; // = new TColStd_HArray1OfReal(1, 16);

  if (! allXCoeffs.IsNull()) {
    Standard_Boolean st;

    Message_Msg Msg145_X("XSTEP_145");
    Msg145_X.Arg("X");
    
    Message_Msg Msg145_Y("XSTEP_145");
    Msg145_Y.Arg("Y");
    
    Message_Msg Msg145_Z("XSTEP_145");
    Msg145_Z.Arg("Z");
    
    for (i = 1; i <= allNbUSegments; i++)  {
      for (j = 1; j <= allNbVSegments; j++)  {
	st = PR.ReadReals (PR.CurrentList(16),Msg145_X,Temp);
	//st = PR.ReadReals (PR.CurrentList(16),"X Coefficient Of Patch",Temp);
	if (st && Temp->Length() == 16) allXCoeffs->SetValue(i,j,Temp);
	else {
	  Message_Msg Msg147_X("XSTEP_147");
	  Msg147_X.Arg("X");
	  PR.SendFail (Msg147_X);
	}
	st = PR.ReadReals (PR.CurrentList(16),Msg145_Y,Temp);
	//st = PR.ReadReals (PR.CurrentList(16),"Y Coefficient Of Patch",Temp);
	if (st && Temp->Length() == 16) allYCoeffs->SetValue(i, j, Temp);
	else {
	  Message_Msg Msg147_Y("XSTEP_147");
	  Msg147_Y.Arg("Y");
	  PR.SendFail (Msg147_Y);
	}
	st = PR.ReadReals (PR.CurrentList(16),Msg145_Z,Temp);
	//st = PR.ReadReals (PR.CurrentList(16),"Z Coefficient Of Patch",Temp);
	if (st && Temp->Length() == 16) allZCoeffs->SetValue(i, j, Temp);
	else if (i < allNbUSegments || j < allNbVSegments) {
	  Message_Msg Msg147_Z("XSTEP_147");
	  Msg147_Z.Arg("Z");
	  PR.SendFail (Msg147_Z);
	}
	else {
//  Si fin manquante ... On refait temp !
//  Les valeurs n ont pas ete lues ... il faut d abord les relire !
	  Temp = new TColStd_HArray1OfReal (1,16);  Temp->Init(0.);
	  for (k = 1; k <= 16; k ++) {
	    Standard_Real vl;
	    if (!PR.ReadReal (PR.Current(),vl)) {
	      Message_Msg Msg146("XSTEP_146");
	      PR.SendFail(Msg146);
	      break;
	    }
	    //if (!PR.ReadReal (PR.Current(),"Z of very last patch",vl)) break;
	    Temp->SetValue(k,vl);
	  }
	  allZCoeffs->SetValue(i, j, Temp);
	  PR.Mend("Last patch incomplete, defaulted");
	}
      }
      for (Standard_Integer kk = 1; kk <= 48; kk++)
	PR.SetCurrentNumber(PR.CurrentNumber() + 1);       
      //Skip the Arbitrary Values
    }
  }
  if (vbreak)
    for (k = 1; k <= 48*(allNbVSegments+1); k++)
      PR.SetCurrentNumber(PR.CurrentNumber() + 1);           
  // Skip the Arbitrary Values

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init
    (aBoundaryType , aPatchType, allUBreakPoints, allVBreakPoints, 
     allXCoeffs, allYCoeffs, allZCoeffs);
}

void IGESGeom_ToolSplineSurface::WriteOwnParams
  (const Handle(IGESGeom_SplineSurface)& ent, IGESData_IGESWriter& IW)  const
{
  Standard_Integer I,J,k;

  IW.Send(ent->BoundaryType());
  IW.Send(ent->PatchType());

  Standard_Integer nbUSegs = ent->NbUSegments();
  Standard_Integer nbVSegs = ent->NbVSegments();
  IW.Send(nbUSegs);
  IW.Send(nbVSegs);

  for (I = 1; I <= nbUSegs + 1; I ++)
    IW.Send(ent->UBreakPoint(I));

  for (I = 1; I <= nbVSegs + 1; I ++)
    IW.Send(ent->VBreakPoint(I));

  for (I = 1; I <= nbUSegs; I++)
    {
      for (J = 1 ; J <= nbVSegs; J++)
	{
          for (k = 1; k <= 16; k++)
	    IW.Send((ent->XPolynomial(I, J))->Value(k));
          for (k = 1; k <= 16; k++)
	    IW.Send((ent->YPolynomial(I, J))->Value(k));
          for (k = 1; k <= 16; k++)
	    IW.Send((ent->ZPolynomial(I, J))->Value(k));
	}
      for (k = 1; k <= 48; k++)
	IW.Send( 0.0);  //Send Arbitrary Values
    }
  for (J = 1; J <= (nbVSegs+1)*48; J++)
    IW.Send( 0.0);  //Send Arbitrary Values
}

void  IGESGeom_ToolSplineSurface::OwnShared
  (const Handle(IGESGeom_SplineSurface)& /* ent */, Interface_EntityIterator& /* iter */) const
{
}

void IGESGeom_ToolSplineSurface::OwnCopy
  (const Handle(IGESGeom_SplineSurface)& another,
   const Handle(IGESGeom_SplineSurface)& ent, Interface_CopyTool& /* TC */) const
{

  Standard_Integer aBoundaryType, aPatchType, allNbUSegments, allNbVSegments;
  Standard_Integer I, J;

  aBoundaryType = another->BoundaryType();
  aPatchType = another->PatchType();
  allNbUSegments = another->NbUSegments();
  allNbVSegments = another->NbVSegments();

  Handle(TColStd_HArray1OfReal) allUBreakPoints =
    new TColStd_HArray1OfReal(1, allNbUSegments+1);

  Handle(TColStd_HArray1OfReal) allVBreakPoints =
    new TColStd_HArray1OfReal(1, allNbVSegments+1);

  for ( I =1; I <= allNbUSegments+1; I++)
    allUBreakPoints->SetValue(I, another->UBreakPoint(I));

  for ( I =1; I <= allNbVSegments+1; I++)
    allVBreakPoints->SetValue(I, another->VBreakPoint(I));

  Handle(IGESBasic_HArray2OfHArray1OfReal) allXCoeffs = new 
    IGESBasic_HArray2OfHArray1OfReal(1,allNbUSegments,1,allNbVSegments);

  Handle(IGESBasic_HArray2OfHArray1OfReal) allYCoeffs = new 
    IGESBasic_HArray2OfHArray1OfReal(1,allNbUSegments,1,allNbVSegments);

  Handle(IGESBasic_HArray2OfHArray1OfReal) allZCoeffs = new 
    IGESBasic_HArray2OfHArray1OfReal(1,allNbUSegments,1,allNbVSegments);

  Handle(TColStd_HArray1OfReal) temp =
    new TColStd_HArray1OfReal(1, 16);

  for (I =1; I <= allNbUSegments; I++) 
    for (J = 1; J <= allNbVSegments; J++)
      {
	temp = another->XPolynomial(I, J);
	allXCoeffs->SetValue(I,J,temp);
	temp = another->YPolynomial(I,J);
	allYCoeffs->SetValue(I,J,temp);
	temp = another->ZPolynomial(I,J);
	allZCoeffs->SetValue(I,J,temp);
      }

  ent->Init(aBoundaryType , aPatchType, allUBreakPoints, allVBreakPoints, 
	    allXCoeffs, allYCoeffs, allZCoeffs);
}   


IGESData_DirChecker IGESGeom_ToolSplineSurface::DirChecker
  (const Handle(IGESGeom_SplineSurface)& /* ent */ )  const
{
  IGESData_DirChecker DC(114,0);
  DC.Structure(IGESData_DefVoid);
  DC.LineFont(IGESData_DefAny);
//  DC.LineWeight(IGESData_DefValue);
  DC.Color(IGESData_DefAny);
  DC.HierarchyStatusIgnored ();
  return DC;
}

void IGESGeom_ToolSplineSurface::OwnCheck
  (const Handle(IGESGeom_SplineSurface)& ent,
   const Interface_ShareTool& , Handle(Interface_Check)& ach)  const
{

  // MGE 30/07/98
  // Building of messages
  //========================================
 // Message_Msg Msg140("XSTEP_140");
  //========================================

  if (ent->BoundaryType() < 1 || ent->BoundaryType() > 6){
    Message_Msg Msg140("XSTEP_140");
    ach->SendFail(Msg140);
  }
//  if (ent->PatchType() < 0 || ent->PatchType() > 1)
//    ach.AddFail("Incorrect Patch Type not in [0-1]");
}

void IGESGeom_ToolSplineSurface::OwnDump
  (const Handle(IGESGeom_SplineSurface)& ent, const IGESData_IGESDumper& /* dumper */,
   const Handle(Message_Messenger)& S, const Standard_Integer level)  const
{
  S << "IGESGeom_SplineSurface" << endl;

  Standard_Integer I, J;
  Standard_Integer nbUSegs = ent->NbUSegments();
  Standard_Integer nbVSegs = ent->NbVSegments();
  Handle(TColStd_HArray1OfReal) temp;

  S << "The  Spline Boundary Type : " << ent->BoundaryType() ;
  switch (ent->BoundaryType())
    {
      case 1 : S << "  (Linear)"; break;
      case 2 : S << "  (Quadratic)"; break;
      case 3 : S << "  (Cubic)"; break;
      case 4 : S << "  (Wilson-Fowler)"; break;
      case 5 : S << "  (Modified Wison-Fowler)"; break;
      case 6 : S << "  (B-Spline)"; break;
      default : S << "  (Invalid value)"; break;
    }
  S << endl;
  S << "The  Patch Type : " << ent->PatchType();
  if (ent->PatchType() == 1)    S << "  (Cartesian Product)";
  else                          S << "  (Unspecified)";
  S << endl;
  S << "Number Of Segments. In U : "
    << nbUSegs << "    In V : " << nbVSegs << endl;
  S << "The U Break Points : ";
  IGESData_DumpVals(S ,level,1, nbUSegs+1,ent->UBreakPoint);
  S <<endl<< "The V Break Points : ";
  IGESData_DumpVals(S ,level,1, nbVSegs+1,ent->VBreakPoint);

  S <<endl<< " X-Y-Z Polynomials Of Segments : ";
  S << endl;
  if (level > 4)
    {
      for (I = 1; I <= nbUSegs; I++)
	for (J = 1; J <= nbVSegs; J++) 
          {
	    S << "[" << I <<"," << J << "]: " << endl;
	    S << "X Polynomial : ";
	    temp = ent->XPolynomial(I,J);
	    IGESData_DumpVals(S,level,1, temp->Length(),temp->Value);
	    S << endl;
	    S << "Y Polynomial : ";
	    temp = ent->YPolynomial(I,J);
	    IGESData_DumpVals(S,level,1, temp->Length(),temp->Value);
	    S << endl;
	    S << "Z Polynomial : ";
	    temp = ent->ZPolynomial(I,J);
	    IGESData_DumpVals(S,level,1, temp->Length(),temp->Value);
	    S << endl;
          }
    }
  else  S << endl;
}
