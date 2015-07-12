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

#include <gp_XY.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_Dump.hxx>
#include <IGESData_IGESDumper.hxx>
#include <IGESData_IGESReaderData.hxx>
#include <IGESData_IGESWriter.hxx>
#include <IGESData_ParamCursor.hxx>
#include <IGESData_ParamReader.hxx>
#include <IGESGeom_CopiousData.hxx>
#include <IGESGeom_ToolCopiousData.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareTool.hxx>
#include <Message_Messenger.hxx>
#include <Message_Msg.hxx>
#include <Standard_DomainError.hxx>
#include <TColStd_HArray1OfReal.hxx>

// MGE 28/07/98
//=======================================================================
//function : IGESGeom_ToolCopiousData
//purpose  : 
//=======================================================================
IGESGeom_ToolCopiousData::IGESGeom_ToolCopiousData ()
{
}


//=======================================================================
//function : ReadOwnParams
//purpose  : 
//=======================================================================

void IGESGeom_ToolCopiousData::ReadOwnParams(const Handle(IGESGeom_CopiousData)& ent,
                                             const Handle(IGESData_IGESReaderData)& /* IR */,
                                             IGESData_ParamReader& PR) const
{
  // MGE 28/07/98
  // Building of messages

  Standard_Integer aDataType, nbData;
  Standard_Real aZPlane = 0;
  Handle(TColStd_HArray1OfReal) allData;

  Standard_Integer upper;
  //Standard_Boolean st; //szv#4:S4163:12Mar99 moved down
  Standard_Boolean data = Standard_False;

  if (!PR.ReadInteger(PR.Current(), aDataType)){ //szv#4:S4163:12Mar99 `st=` not needed
    //st = PR.ReadInteger(PR.Current(), "Data Type", aDataType);
    Message_Msg Msg85("XSTEP_85");
    PR.SendFail(Msg85);
  }

  Standard_Boolean st = PR.ReadInteger(PR.Current(), nbData);
  //st = PR.ReadInteger(PR.Current(), "Number of n-tuples", nbData);
  if (st && (nbData > 0)) data = Standard_True;
  else {
    Message_Msg Msg86("XSTEP_86");
    PR.SendFail(Msg86);
  }

  if (aDataType == 1){
    if (!PR.ReadReal(PR.Current(), aZPlane)){ //szv#4:S4163:12Mar99 `st=` not needed
      Message_Msg Msg87("XSTEP_87");
      PR.SendFail(Msg87);
    }
  //if (aDataType == 1)  st = PR.ReadReal(PR.Current(), "Z Plane", aZPlane);
  }

  if ( data )
    {
      if (aDataType == 1)            upper = 2*nbData;
      else if (aDataType == 2)       upper = 3*nbData;
      else                           upper = 6*nbData;

      Message_Msg Msg88("XSTEP_88");
      //allData = new TColStd_HArray1OfReal(1, upper) then fill it :
      PR.ReadReals(PR.CurrentList(upper),Msg88, allData); //szv#4:S4163:12Mar99 `st=` not needed
      //st = PR.ReadReals(PR.CurrentList(upper), "Tuples", allData);
    }

  DirChecker(ent).CheckTypeAndForm(PR.CCheck(),ent);
  ent->Init(aDataType, aZPlane, allData);
}


//=======================================================================
//function : WriteOwnParams
//purpose  : 
//=======================================================================

void IGESGeom_ToolCopiousData::WriteOwnParams(const Handle(IGESGeom_CopiousData)& ent,
                                              IGESData_IGESWriter& IW)  const
{
  Standard_Integer upper = ent->NbPoints();
  Standard_Integer dtype = ent->DataType();
  IW.Send(dtype);
  IW.Send(upper);
  if (ent->DataType() == 1)
    IW.Send(ent->ZPlane());
  for (Standard_Integer I = 1; I <= upper; I++) {
// DataType = 1 : XY , 2 : XYZ , 3 : XYZ*2
    IW.Send( ent->Data(I,1) );
    IW.Send( ent->Data(I,2) );
    if (dtype > 1) IW.Send( ent->Data(I,3) );
    if (dtype <= 2) continue;
    IW.Send( ent->Data(I,4) );
    IW.Send( ent->Data(I,5) );
    IW.Send( ent->Data(I,6) );
  }
}


//=======================================================================
//function : OwnShared
//purpose  : 
//=======================================================================

void IGESGeom_ToolCopiousData::OwnShared(const Handle(IGESGeom_CopiousData)& /* ent */,
                                         Interface_EntityIterator& /* iter */) const
{
}


//=======================================================================
//function : OwnCopy
//purpose  : 
//=======================================================================

void IGESGeom_ToolCopiousData::OwnCopy(const Handle(IGESGeom_CopiousData)& another,
                                       const Handle(IGESGeom_CopiousData)& ent,
                                       Interface_CopyTool& /* TC */) const
{
  Standard_Integer upper;
  Standard_Real    aZPlane   = 0;
  Standard_Integer nbTuples  = another->NbPoints();
  Standard_Integer aDataType = another->DataType();
  Handle(TColStd_HArray1OfReal) allData;

  if      (aDataType == 1)    upper = 2*nbTuples;
  else if (aDataType == 2)    upper = 3*nbTuples;
  else                        upper = 6*nbTuples;

  allData = new TColStd_HArray1OfReal(1, upper);

  if (aDataType == 1)    aZPlane = another->ZPlane();

  for (Standard_Integer I = 1; I <= nbTuples; I++)  {

    switch (aDataType) {
    case 1:
      allData->SetValue(2*I-1, another->Data(I,1));
      allData->SetValue(2*I  , another->Data(I,2));
      break;
    case 2:
      allData->SetValue(3*I-2, another->Data(I,1));
      allData->SetValue(3*I-1, another->Data(I,2));
      allData->SetValue(3*I  , another->Data(I,3));
      break;
    case 3:
      allData->SetValue(6*I-5, another->Data(I,1));
      allData->SetValue(6*I-4, another->Data(I,2));
      allData->SetValue(6*I-3, another->Data(I,3));
      allData->SetValue(6*I-2, another->Data(I,4));
      allData->SetValue(6*I-1, another->Data(I,5));
      allData->SetValue(6*I  , another->Data(I,6));
      break;
    }
  }

  ent->Init(aDataType, aZPlane, allData);
  if (another->IsClosedPath2D()) ent->SetClosedPath2D();
  else  ent->SetPolyline (another->IsPolyline());
}


//=======================================================================
//function : DirChecker
//purpose  : 
//=======================================================================

IGESData_DirChecker IGESGeom_ToolCopiousData::DirChecker
  (const Handle(IGESGeom_CopiousData)& ent )  const
{
  IGESData_DirChecker DC(106, 1, 63);
  DC.Structure(IGESData_DefVoid);
  if (ent->FormNumber() <= 3)
    {
      DC.LineFont(IGESData_DefVoid);
      DC.LineWeight(IGESData_DefVoid);
    }
  else
    {
      DC.LineFont(IGESData_DefAny);
      DC.LineWeight(IGESData_DefValue);
    }
  DC.Color(IGESData_DefAny);
  DC.HierarchyStatusIgnored ();
  return DC;
}


//=======================================================================
//function : OwnCheck
//purpose  : 
//=======================================================================

void IGESGeom_ToolCopiousData::OwnCheck(const Handle(IGESGeom_CopiousData)& ent,
                                        const Interface_ShareTool&,
                                        Handle(Interface_Check)& ach)  const
{
  // MGE 28/07/98
  // Building of messages
  //======================================
  Message_Msg Msg71("XSTEP_71");
  //Message_Msg Msg85("XSTEP_85");
  //======================================

  Standard_Integer fn = ent->FormNumber();
  if ( (fn > 3 && fn < 11) || (fn > 14 && fn < 63)) {
//    Message_Msg Msg71("XSTEP_71");
    ach->SendFail(Msg71);
  }
  Standard_Integer dt = ent->DataType();
  if (dt < 1 || dt > 3) {
    Message_Msg Msg85("XSTEP_85");
    ach->SendFail(Msg85);
  }
  if ((dt == 1 && (fn != 1 && fn != 11 && fn != 63)) ||
      (dt == 2 && (fn != 2 && fn != 12)) || (dt == 3 && (fn != 3 && fn != 13))) {
//    Message_Msg Msg71("XSTEP_71");
    ach->SendFail(Msg71);
  }
}


//=======================================================================
//function : OwnDump
//purpose  : 
//=======================================================================

void IGESGeom_ToolCopiousData::OwnDump(const Handle(IGESGeom_CopiousData)& ent,
                                       const IGESData_IGESDumper& /* dumper */,
                                       const Handle(Message_Messenger)& S,
                                       const Standard_Integer level)  const
{
  Standard_Integer nbPnts = ent->NbPoints();
  Standard_Integer dtype  = ent->DataType();
  Standard_Integer i;
  gp_GTrsf loca = ent->Location();
  gp_GTrsf locv = ent->VectorLocation();
  Standard_Boolean yatr = (level > 5 && ent->HasTransf());

  S << "IGESGeom_CopiousData" << endl;

  if (ent->IsPointSet())          S << "Point Set  ";
  else if (ent->IsPolyline())     S << "Polyline  ";
  else if (ent->IsClosedPath2D()) S << "Closed Path 2D  ";
  S << "DataType " << ent->DataType() << "  ";
  S << "Number of T-uples = " << nbPnts << "  ";
  if (dtype == 1) {
    S << "(Points 2D)  ";
    S << "ZPlane = " << ent->ZPlane() << endl;
  }
  else if (dtype == 2)
    S << "(Points 3D)" << endl;
  else if (dtype == 3)
    S << "(Points 3D + Vectors 3D)" << endl;

  if (level > 4) {
    for (i = 1; i <= nbPnts; i ++) {
      gp_XYZ T = ent->Point(i).XYZ();
      if (dtype == 1)
	S << "[" << i << "] (" << T.X() << "," << T.Y() << ")";
      else
	S << "[" << i << "] (" << T.X() << "," << T.Y() << "," << T.Z() << ")";

      if (yatr) {
	loca.Transforms(T);
	S << " Transformed ("<< T.X() << "," << T.Y() << "," << T.Z() << ")";
      }

      if (dtype == 3) {
//	gp_XYZ P = ent->Vector(i).XYZ();
	S << "\n   Vector (" << T.X() << "," << T.Y() << "," << T.Z() << ")";
	if (yatr) {
	  locv.Transforms(T);
	  S << " Transformed ("<< T.X() << "," << T.Y() << "," << T.Z() << ")";
	}
      }
      S << endl;
    }
  }
  else S << " [ for content, ask level > 4 ]";
  S << endl;
}
