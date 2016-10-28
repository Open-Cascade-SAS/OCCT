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

#include <IGESData.hxx>
#include <IGESData_DefaultGeneral.hxx>
#include <IGESData_DefaultSpecific.hxx>
#include <IGESData_GlobalSection.hxx>
#include <IGESData_IGESModel.hxx>
#include <IGESData_Protocol.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Static.hxx>
#include <Interface_Version.hxx>
#include <Message_MsgFile.hxx>
#include <OSD_Process.hxx>
#include <Quantity_Date.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <TCollection_HAsciiString.hxx>

#include <stdio.h>
static Handle(IGESData_Protocol) proto;

static Handle(IGESData_DefaultGeneral)    stmod;
static Handle(IGESData_DefaultSpecific)   speci;


    void  IGESData::Init ()
{
  if (proto.IsNull()) proto = new IGESData_Protocol;
  if (stmod.IsNull()) stmod = new IGESData_DefaultGeneral;
  if (speci.IsNull()) speci = new IGESData_DefaultSpecific;
//  et modele template "iges"
  if (Interface_InterfaceModel::HasTemplate("iges")) return;

  OSD_Process process;

  // Static Parameters - see also Static Standards
  // -----------------
  Interface_Static::Standards();

  Interface_Static::Init ("XSTEP","read.iges.bspline.approxd1.mode",'e',"");
  Interface_Static::Init ("XSTEP","read.iges.bspline.approxd1.mode",'&',"ematch 0");
  Interface_Static::Init ("XSTEP","read.iges.bspline.approxd1.mode",'&',"eval Off");
  Interface_Static::Init ("XSTEP","read.iges.bspline.approxd1.mode",'&',"eval On");
  Interface_Static::SetIVal ("read.iges.bspline.approxd1.mode",0);

  Interface_Static::Init ("XSTEP","read.iges.bspline.continuity",'i',"1");
  Interface_Static::Init ("XSTEP","read.iges.bspline.continuity",'&',"imin 0");
  Interface_Static::Init ("XSTEP","read.iges.bspline.continuity",'&',"imax 2");

  //#58 rln Interface_Static::Init ("XSTEP","write.iges.header.product" ,'t',"");
  Interface_Static::Init ("XSTEP","write.iges.header.receiver",'t',"");//58 rln Unknown
  Interface_Static::Init ("XSTEP","write.iges.header.author"  ,'t',process.UserName().ToCString());
  Interface_Static::Init ("XSTEP","write.iges.header.company" ,'t',"");//58 rln Matra Datavision

  Interface_Static::Init ("XSTEP","write.iges.unit",'e',"");
  Interface_Static::Init ("XSTEP","write.iges.unit",'&',"enum 1");
  Interface_Static::Init ("XSTEP","write.iges.unit",'&',"eval INCH");  // 1
  Interface_Static::Init ("XSTEP","write.iges.unit",'&',"eval MM");    // 2
  Interface_Static::Init ("XSTEP","write.iges.unit",'&',"eval ??");    // 3
  Interface_Static::Init ("XSTEP","write.iges.unit",'&',"eval FT");    // 4
  Interface_Static::Init ("XSTEP","write.iges.unit",'&',"eval MI");    // 5
  Interface_Static::Init ("XSTEP","write.iges.unit",'&',"eval M");     // 6
  Interface_Static::Init ("XSTEP","write.iges.unit",'&',"eval KM");    // 7
  Interface_Static::Init ("XSTEP","write.iges.unit",'&',"eval MIL");   // 8
  Interface_Static::Init ("XSTEP","write.iges.unit",'&',"eval UM");    // 9
  Interface_Static::Init ("XSTEP","write.iges.unit",'&',"eval CM");    //10
  Interface_Static::Init ("XSTEP","write.iges.unit",'&',"eval UIN");   //11
  Interface_Static::SetCVal ("write.iges.unit","MM");

  Interface_Static::Init ("XSTEP","write.iges.brep.mode",'e',"");
  Interface_Static::Init ("XSTEP","write.iges.brep.mode",'&',"ematch 0");
  Interface_Static::Init ("XSTEP","write.iges.brep.mode",'&',"eval Faces");
  Interface_Static::Init ("XSTEP","write.iges.brep.mode",'&',"eval BRep");
  Interface_Static::SetIVal ("write.iges.brep.mode",0);

  //S4181 pdn 23.04.99: adding new parameter handling writing of elementary surfaces
  Interface_Static::Init ("XSTEP","write.convertsurface.mode",'e',"");
  Interface_Static::Init ("XSTEP","write.convertsurface.mode",'&',"ematch 0");
  Interface_Static::Init ("XSTEP","write.convertsurface.mode",'&',"eval Off");
  Interface_Static::Init ("XSTEP","write.convertsurface.mode",'&',"eval On");
  Interface_Static::SetIVal ("write.convertsurface.mode",0);

  // abv 15.11.00: ShapeProcessing
  Interface_Static::Init ("XSTEP","write.iges.resource.name",'t',"IGES");
  Interface_Static::Init ("XSTEP","read.iges.resource.name",'t',"IGES");
  Interface_Static::Init ("XSTEP","write.iges.sequence",'t',"ToIGES");
  Interface_Static::Init ("XSTEP","read.iges.sequence",'t',"FromIGES");
  
  // sln 11.06.2002 OCC448 : Initialize "read.onlyvisible" parameter  to control transfering 
  // invisiable sub entities which logicaly depend on the grouping entities
  Interface_Static::Init ("XSTEP","read.iges.onlyvisible",'e',"");
  Interface_Static::Init ("XSTEP","read.iges.onlyvisible",'&',"ematch 0");
  Interface_Static::Init ("XSTEP","read.iges.onlyvisible",'&',"eval Off");
  Interface_Static::Init ("XSTEP","read.iges.onlyvisible",'&',"eval On");
  Interface_Static::SetIVal ("read.iges.onlyvisible",0);

  //gka added parameter for reading failed entities. 19.07
  Interface_Static::Init ("XSTEP","read.iges.faulty.entities",'e',"");
  Interface_Static::Init ("XSTEP","read.iges.faulty.entities",'&',"ematch 0");
  Interface_Static::Init ("XSTEP","read.iges.faulty.entities",'&',"eval Off");
  Interface_Static::Init ("XSTEP","read.iges.faulty.entities",'&',"eval On");
  Interface_Static::SetIVal ("read.iges.faulty.entities",0);

  //ika added parameter for writing planes mode 2.11.2012 
  Interface_Static::Init ("XSTEP","write.iges.plane.mode",'e',"");
  Interface_Static::Init ("XSTEP","write.iges.plane.mode",'&',"ematch 0");
  Interface_Static::Init ("XSTEP","write.iges.plane.mode",'&',"eval Plane");
  Interface_Static::Init ("XSTEP","write.iges.plane.mode",'&',"eval BSpline");
  Interface_Static::SetIVal ("write.iges.plane.mode",0);

   //ika added parameter for writing offset curves like BSplines 12.04.2013 
  Interface_Static::Init ("XSTEP","write.iges.offset.mode",'e',"");
  Interface_Static::Init ("XSTEP","write.iges.offset.mode",'&',"ematch 0");
  Interface_Static::Init ("XSTEP","write.iges.offset.mode",'&',"eval On");
  Interface_Static::Init ("XSTEP","write.iges.offset.mode",'&',"eval Off");
  Interface_Static::SetIVal ("write.iges.offset.mode",0);

  // Creating the Global Section
  //----------------------------

  IGESData_GlobalSection GS;
  //#58 rln 28.12.98 changing default values for Global Section
  char procver[80];
  sprintf (procver, XSTEP_PROCESSOR_VERSION, "IGES");
  Handle(TCollection_HAsciiString) gsys = new TCollection_HAsciiString (procver);
  Interface_Static::Init ("XSTEP","write.iges.header.product" ,'t',procver);
  
/*  Handle(TCollection_HAsciiString) gsys = new TCollection_HAsciiString
    (XSTEP_VERSION);
  gsys->AssignCat(" on ");
  gsys->AssignCat
#ifdef HPUX
    ("HP-UX");
  cout<<"--  OSD_Path::SystemVersion, does not work well on HP-UX"<<endl;
#endif
#ifndef HPUX
    (host.SystemVersion().ToCString());
#endif

//  SendName : nom significatif de la piece transmise par exemple
//  SystemId : c est MDTV etc
//  InterfaceVersion : la version en cours de XSTEP; incluant la plateforme

  char nomsys[100]; int istat; long lstat;
  struct utsname infosy;
  istat = uname (&infosy);
  lstat = sysinfo (SI_HW_PROVIDER,nomsys,99);
  Handle(TCollection_HAsciiString) gsys = new TCollection_HAsciiString(nomsys);
  gsys->AssignCat(" ");
  lstat = sysinfo (SI_ARCHITECTURE,nomsys,99);
  gsys->AssignCat(nomsys);
  gsys->AssignCat("/");
  gsys->AssignCat(infosy.sysname);
  gsys->AssignCat(" ");
  gsys->AssignCat(infosy.release);
*/
  Standard_Integer year;                                //gka 19.01.99
  OSD_Process system;
  Quantity_Date ladate = system.SystemDate(); 
  year = ladate.Year();
  Handle(TCollection_HAsciiString) datestr;
  if( year < 2000 )
    //#65 rln 12.02.99 S4151 (explicitly force YYMMDD.HHMMSS before Y2000)
    datestr = GS.NewDateString(0,0,0,0,0,0,0);
  else 
    //#65 rln 12.02.99 S4151 (explicitly force YYYYMMDD.HHMMSS after Y2000)
    datestr = GS.NewDateString(0,0,0,0,0,0,-1);
  GS.SetSeparator (',');
  GS.SetEndMark   (';');
  GS.SetSendName (new TCollection_HAsciiString(Interface_Static::CVal("write.iges.header.product")));
  GS.SetFileName (new TCollection_HAsciiString("Filename.iges"));
  GS.SetSystemId (new TCollection_HAsciiString(XSTEP_SYSTEM_VERSION));//#58 rln "MATRA-DATAVISION EUCLID-QUANTUM"
  GS.SetInterfaceVersion (gsys);
  GS.SetIntegerBits      ( IntegerSize() );
  GS.SetMaxPower10Single ( RealLast10Exp() );
  GS.SetMaxDigitsSingle  ( RealDigits() );
  GS.SetMaxPower10Double ( RealLast10Exp() );
  GS.SetMaxDigitsDouble  ( RealDigits() );
  GS.SetReceiveName (Interface_Static::Static("write.iges.header.receiver")->HStringValue());
// new TCollection_HAsciiString("Unknown");
  GS.SetScale       (1.0);
  GS.SetUnitFlag    (Interface_Static::IVal("write.iges.unit"));
  GS.SetUnitName    (new TCollection_HAsciiString(Interface_Static::CVal("write.iges.unit")));
  GS.SetLineWeightGrad (1);
  GS.SetMaxLineWeight  (0.01);
  GS.SetDate        (datestr);
  GS.SetResolution  (0.0001);
  GS.SetMaxCoord    (0.0 /*1000.0*/);//22.10.98 rln BUC60081
  GS.SetAuthorName  (Interface_Static::Static("write.iges.header.author")->HStringValue());
//  new TCollection_HAsciiString (process.UserName());
  GS.SetCompanyName (Interface_Static::Static("write.iges.header.company")->HStringValue());
//  new TCollection_HAsciiString("Matra Datavision");
  GS.SetIGESVersion (11);         // pour IGES-5.3 //gka 19.01.99
  GS.SetDraftingStandard (0);
  GS.SetLastChangeDate  (datestr);
  GS.SetApplicationProtocol(new TCollection_HAsciiString("")); //gka 19.01.99

//  Interface_Check check;
//  GS.Init(params, check);

  // Creating the Model
  //-------------------

  Handle(IGESData_IGESModel) model = new IGESData_IGESModel;
  model->SetGlobalSection(GS);
  Interface_InterfaceModel::SetTemplate ("iges", model);
}

    Handle(IGESData_Protocol) IGESData::Protocol ()
{
  return proto;
}
