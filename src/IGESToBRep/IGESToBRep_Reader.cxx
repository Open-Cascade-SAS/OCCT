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

//pdn 11.01.99 including <stdio.h> for compilation on NT
//#70 rln 03.03.99 syntax correction
// sln 11.06.2002 OCC448 : Initialize "read.onlyvisiable" parameter  to control transfering invisiable sub entities which logicaly depend on the grouping entities

#include <BRep_Builder.hxx>
#include <BRepLib.hxx>
#include <BRepTools_Modifier.hxx>
#include <gp_Trsf.hxx>
#include <IGESAppli.hxx>
#include <IGESAppli_Protocol.hxx>
#include <IGESData_FileProtocol.hxx>
#include <IGESData_GlobalSection.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESModel.hxx>
#include <IGESFile_Read.hxx>
#include <IGESSolid.hxx>
#include <IGESSolid_Protocol.hxx>
#include <IGESToBRep.hxx>
#include <IGESToBRep_Actor.hxx>
#include <IGESToBRep_CurveAndSurface.hxx>
#include <IGESToBRep_Reader.hxx>
#include <Interface_Check.hxx>
#include <Interface_CheckIterator.hxx>
#include <Interface_CheckTool.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ShareFlags.hxx>
#include <Interface_Static.hxx>
#include <Message_Messenger.hxx>
#include <Message_Msg.hxx>
#include <Message_ProgressSentry.hxx>
#include <OSD_Timer.hxx>
#include <ShapeAlgo.hxx>
#include <ShapeAlgo_AlgoContainer.hxx>
#include <ShapeExtend_Explorer.hxx>
#include <ShapeFix_ShapeTolerance.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <TopAbs.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Shape.hxx>
#include <Transfer_IteratorOfProcessForTransient.hxx>
#include <Transfer_TransferOutput.hxx>
#include <Transfer_TransientProcess.hxx>
#include <TransferBRep.hxx>
#include <TransferBRep_ShapeBinder.hxx>
#include <TransferBRep_ShapeListBinder.hxx>
#include <XSAlgo.hxx>
#include <XSAlgo_AlgoContainer.hxx>

#include <stdio.h>
//#include <ShapeCustom.hxx>
#ifdef _MSC_VER
#include <stdlib.h>
#else
#include <errno.h>
#endif
//extern int errno;

static Handle(IGESData_FileProtocol) protocol;


//=======================================================================
//function : IGESToBRep_Reader
//purpose  : 
//=======================================================================
    IGESToBRep_Reader::IGESToBRep_Reader ()
{
  theDone = Standard_False;
  if (protocol.IsNull()) {
    IGESAppli::Init();  IGESSolid::Init();
    protocol = new IGESData_FileProtocol;
    protocol->Add(IGESAppli::Protocol());
    protocol->Add(IGESSolid::Protocol());
  }
  theActor = new IGESToBRep_Actor;
  theProc = new Transfer_TransientProcess;
}


//=======================================================================
//function : LoadFile
//purpose  : loads a Model from a file
//=======================================================================

Standard_Integer IGESToBRep_Reader::LoadFile (const Standard_CString filename)
{ 
  if ( theProc.IsNull() )
    theProc = new Transfer_TransientProcess;
  Handle(Message_Messenger) TF = theProc->Messenger();
  
  // Message for Diagnostic file.
  Message_Msg msg2000("IGES_2000");   
  msg2000.Arg(filename);
  TF->Send (msg2000, Message_Info);
  //Message_Msg msg2001("IGES_2001");   // Date
  Message_Msg msg2005("IGES_2005");
  msg2005.Arg(theProc->TraceLevel());
  TF->Send (msg2005, Message_Info);
  /////////////////////////////////////////////////////////
  Handle(IGESData_IGESModel) model = new IGESData_IGESModel;

  OSD_Timer c; c.Reset(); c.Start();    
  char *pfilename=(char *)filename;
  Standard_Integer StatusFile = IGESFile_Read(pfilename,model,protocol);
  if (StatusFile != 0) {
    // Sending of message : IGES file opening error 
    Message_Msg Msg2("XSTEP_2");
    TF->Send (Msg2, Message_Info);
    //Message_Msg Msg3("XSTEP_3");
    //Message_Msg Msg4("XSTEP_4");
    //Message_Msg Msg5("XSTEP_5");
    //Message_Msg Msg6("XSTEP_6");
    //Message_Msg Msg7("XSTEP_7");
    // Reasons of the file opening error
    switch(errno)
      {
      case 2  : // Sending of message : No such file or directory
        {
          Message_Msg Msg3("XSTEP_3");
          TF->Send (Msg3, Message_Info);
        }
	break;
      case 12 : // Sending of message : Not enough space
        {
          Message_Msg Msg4("XSTEP_4");
          TF->Send (Msg4, Message_Info);
        }
	break;
      case 13 : // Sending of message : Permission Denied
        {
          Message_Msg Msg5("XSTEP_5");
          TF->Send (Msg5, Message_Info);
        }
	break;
      case 24 : // Sending of message : Too many open files
        {
          Message_Msg Msg6("XSTEP_6");
          TF->Send (Msg6, Message_Info);
        }
	break;
      default : // Sending of message : No determined
        {
          Message_Msg Msg7("XSTEP_7");
          TF->Send (Msg7, Message_Info);
        }
	break;
      }
  }  
  
  Message_Msg Msg8  ("XSTEP_8");
  Message_Msg Msg25 ("XSTEP_25");
  Message_Msg Msg26 ("XSTEP_26");
   // Nb warning in global section.

  Standard_Integer nbWarn = 0,nbFail = 0;
  // Add the number of warning on enities :
  Interface_CheckTool cht (model,protocol);
  Interface_CheckIterator anIter = cht.CompleteCheckList();
  for(anIter.Start(); anIter.More(); anIter.Next()) {
    const Handle(Interface_Check) ach = anIter.Value();
    nbWarn += ach->NbWarnings();
    nbFail += ach->NbFails();
  }
// Messages nbWarn and nbFail;
  Msg25.Arg(nbFail);
  Msg26.Arg(nbWarn);
  TF->Send (Msg25, Message_Info);
  TF->Send (Msg26, Message_Info);
  
  // Message fin de loading iGES file (elapsed time %s)
  char t[20];
  t[0]='\0';
  Standard_Real second, cpu;
  Standard_Integer minute, hour;
  c.Show(second, minute, hour,cpu);
  if (hour > 0)
    Sprintf(t,"%dh:%dm:%.2fs",hour,minute,second);
  else if (minute > 0)
    Sprintf(t,"%dm:%.2fs",minute,second);
  else
    Sprintf(t,"%.2fs",second);
  // Sending of message : End of Loading
  Msg8.Arg(t);
  TF->Send (Msg8, Message_Info);
  
  SetModel(model);
  return StatusFile;
}


//=======================================================================
//function : SetModel
//purpose  : Specifies a Model to work on
//=======================================================================
    void  IGESToBRep_Reader::SetModel (const Handle(IGESData_IGESModel)& model)
{
  theModel = model;
  theDone  = Standard_False;
  theShapes.Clear();
  if ( theProc.IsNull() )
    theProc = new Transfer_TransientProcess (theModel->NbEntities());
  else 
    theProc->Clear();
}


//=======================================================================
//function : Model
//purpose  : returns the Model to be worked on
//=======================================================================
    Handle(IGESData_IGESModel)  IGESToBRep_Reader::Model () const
      {  return theModel;  }


//=======================================================================
//function : SetTransientProcess
//purpose  : Specifies a TransferProcess
//=======================================================================
    void  IGESToBRep_Reader::SetTransientProcess
  (const Handle(Transfer_TransientProcess)& TP)
     {  theProc = TP;  }

//=======================================================================
//function : TransientProcess
//purpose  : Returns the TransferProcess
//=======================================================================
    Handle(Transfer_TransientProcess)  IGESToBRep_Reader::TransientProcess () const
     {  return theProc;  }

//=======================================================================
//function : Actor
//purpose  : returns theActor
//=======================================================================
    Handle(IGESToBRep_Actor)  IGESToBRep_Reader::Actor () const
      {  return theActor;  }


//=======================================================================
//function : Clear
//purpose  : Clears the result and Done status
//=======================================================================
    void  IGESToBRep_Reader::Clear ()
{
  theDone  = Standard_False;
  theShapes.Clear();
}


//=======================================================================
//function : Check
//purpose  : Checks the Model
//=======================================================================
    Standard_Boolean  IGESToBRep_Reader::Check
  (const Standard_Boolean withprint) const
{
  Interface_CheckTool cht (theModel,protocol);
  Interface_CheckIterator chl = cht.CompleteCheckList();
  if (withprint && !theProc.IsNull()) 
    cht.Print(chl, theProc->Messenger());
  return chl.IsEmpty(Standard_True);
}


//=======================================================================
//function : IsDone
//purpose  : returns True if the last transfert was a success
//=======================================================================
    Standard_Boolean  IGESToBRep_Reader::IsDone () const
      {  return theDone;  }


//=======================================================================
//function : EncodeRegul
//purpose  : INTERNAL to encode regularity on edges
//=======================================================================

static Standard_Boolean  EncodeRegul (const TopoDS_Shape& sh)
{
  Standard_Real tolang = Interface_Static::RVal("read.encoderegularity.angle");
  if (sh.IsNull()) return Standard_True;
  if (tolang <= 0) return Standard_True;
  try {
    OCC_CATCH_SIGNALS
    BRepLib::EncodeRegularity (sh,tolang);
  }
  catch(Standard_Failure) {
    return Standard_False;
  }
  return Standard_True;
}

//=======================================================================
//function : UpdateMap
//purpose  : Updates the correspondence map (Transfer_TransientProcess),
//           setting as translation results, the shapes received after
//           modification by modifier (BRepTools_Modifier)
//warning  : BRepTools_Modifier raises exception when it cannot find input
//           shape in its internal list
//=======================================================================

// coment as unused PTV 18.09.2000
// static void UpdateMap (const Handle(Transfer_TransientProcess)& map,
// 		       const BRepTools_Modifier& modifier)
// {
//   Transfer_IteratorOfProcessForTransient iterator = map->CompleteResult(Standard_True);
//   for (iterator.Start(); iterator.More(); iterator.Next()) {
//     const Handle(Transfer_Binder) binder = iterator.Value();
//     try { //to avoid exception in BRepTools_Modifier
//       OCC_CATCH_SIGNALS
//       if (binder->IsKind (STANDARD_TYPE (TransferBRep_ShapeBinder))) {
// 	DeclareAndCast(TransferBRep_ShapeBinder, shapebinder, binder);
// 	if (shapebinder->HasResult()) {
// 	  TopoDS_Shape result = shapebinder->Result();
// 	  TopoDS_Shape modified = modifier.ModifiedShape (result);
// 	  if (shapebinder->Status() != Transfer_StatusUsed) //to avoid exception
// 	    shapebinder->SetResult (modified);
// 	}
//       }
//       else if (binder->IsKind (STANDARD_TYPE (TransferBRep_ShapeListBinder))) {
// 	DeclareAndCast(TransferBRep_ShapeListBinder, shapelistbinder, binder);
// 	for (Standard_Integer i = 1; i <= shapelistbinder->NbShapes(); i++) {
// 	  TopoDS_Shape result = shapelistbinder->Shape (i);
// 	  TopoDS_Shape modified = modifier.ModifiedShape (result);
// 	  shapelistbinder->SetResult (i, modified);
// 	}
//       }
//     }
//     catch(Standard_Failure) {
//       continue;
//     }
//   }
// }

//=======================================================================
//function : TrimTolerances
//purpose  : Trims tolerances of the shape according to static parameters
//          
//=======================================================================

static void TrimTolerances (const TopoDS_Shape& shape,
			    const Standard_Real tol)
{
  if( Interface_Static::IVal("read.maxprecision.mode")==1) {
    ShapeFix_ShapeTolerance SFST;
    SFST.LimitTolerance (shape, 0, Max(tol,Interface_Static::RVal ("read.maxprecision.val")));
  }
}
  
//=======================================================================
//function : TransferRoots
//purpose  : Transfers all Roots Entities
//=======================================================================
void  IGESToBRep_Reader::TransferRoots (const Standard_Boolean onlyvisible)
{
  if (theModel.IsNull() || theProc.IsNull()) return;

  Handle(Message_Messenger) TF = theProc->Messenger();
  // Declaration of messages.
  Message_Msg msg2030("IGES_2030");
  TF->Send (msg2030, Message_Info);
  Message_Msg msg2065("IGES_2065");
  OSD_Timer c; c.Reset(); c.Start();   // Initialisation du CHRONO
  theDone = Standard_False;
  theShapes.Clear();
  
  Standard_Integer level = theProc->TraceLevel();
  theProc->SetErrorHandle(Standard_True);
  theProc->SetRootManagement(Standard_True);
//  PrepareTransfer();  -> protocol, actor
  theActor->SetModel(theModel);
  Standard_Integer continuity = Interface_Static::IVal("read.iges.bspline.continuity");
  theActor->SetContinuity (continuity);
  theProc->SetModel (theModel);
  theProc->SetActor (theActor);
  Transfer_TransferOutput TP (theProc,theModel);

  const Handle(Interface_Protocol) aProtocol = protocol; // to avoid ambiguity
  Interface_ShareFlags SH (theModel, aProtocol);
  Standard_Integer nb = theModel->NbEntities();
  ShapeExtend_Explorer SBE;

  
  Standard_Integer precisionMode = Interface_Static::IVal("read.precision.mode");
  Message_Msg msg2035("IGES_2035");
  msg2035.Arg(precisionMode);
  TF->Send (msg2035, Message_Info);
  if (precisionMode==1) {
    Message_Msg msg2040("IGES_2040");
    msg2040.Arg(Interface_Static::RVal("read.precision.val"));//#70 rln 03.03.99
    TF->Send (msg2040, Message_Info);
  }
  Message_Msg msg2045("IGES_2045");
  msg2045.Arg(continuity);
  TF->Send (msg2045, Message_Info);
  Message_Msg msg2050("IGES_2050");
  msg2050.Arg(Interface_Static::IVal("read.surfacecurve.mode"));
  TF->Send (msg2050, Message_Info);

  // sln 11.06.2002 OCC448
  Interface_Static::SetIVal("read.iges.onlyvisible",onlyvisible);
  
  Message_ProgressSentry PS ( theProc->GetProgress(), "Root", 0, nb, 1 );
  for (Standard_Integer i = 1; i <= nb && PS.More(); i++, PS.Next()) {
    Handle(IGESData_IGESEntity) ent = theModel->Entity(i);
    if ( SH.IsShared(ent) || ! theActor->Recognize (ent) ) continue;
    if (level > 0) {
      Message_Msg msg2070("IGES_2070");
      msg2070.Arg(2*i-1);
      msg2070.Arg(ent->TypeNumber());
      TF->Send (msg2070, Message_Info);
    }
    // on ajoute un traitement pour ne prendre que les entites visibles
    if ( ! onlyvisible || ent->BlankStatus() == 0 ) {
      TopoDS_Shape shape;
      theDone = Standard_True;
      try {
        OCC_CATCH_SIGNALS
        TP.Transfer(ent);
        shape = TransferBRep::ShapeResult (theProc,ent);
      } 
      catch(Standard_Failure) {
        Message_Msg msg1005("IGES_1005");
        TF->Send (msg1005, Message_Info);
        continue;
      }
      if (shape.IsNull()) {
        Message_Msg msg2076("IGES_2076");
        TF->Send (msg2076, Message_Info);
      }
      else {
        if (SBE.ShapeType(shape,Standard_True) != TopAbs_SHAPE) {
          if (!shape.IsNull()) {
            EncodeRegul (shape);
            //#74 rln 03.03.99 S4135
            TrimTolerances (shape, theActor->UsedTolerance());
            theShapes.Append(shape);
          }
        }
      }
    }
  }
  char t [20];
  t[0]='\0';
  Standard_Real second, cpu;
  Standard_Integer minute, hour;
  c.Show(second, minute, hour,cpu);
  if (hour > 0)
    Sprintf(t,"%dh:%dm:%.2fs",hour,minute,second);
  else if (minute > 0)
    Sprintf(t,"%dm:%.2fs",minute,second);
  else
    Sprintf(t,"%.2fs",second);
  // Sending of message : End of Loading
  msg2065.Arg(t);
  TF->Send (msg2065, Message_Info);
}


//=======================================================================
//function : Transfer
//purpose  : Transfers an Entity given
//=======================================================================
Standard_Boolean  IGESToBRep_Reader::Transfer(const Standard_Integer num)
{ 
  Handle(Message_Messenger) TF = theProc->Messenger();
  theDone = Standard_False;
  if (theModel.IsNull()) {
    Message_Msg msg2031("IGES_2031");
    TF->Send (msg2031, Message_Info);
    return Standard_False;
  }
  if (num <= 0 || num > theModel->NbEntities()) {
    Message_Msg msg2032("IGES_2032");
    msg2032.Arg(num);
    TF->Send (msg2032, Message_Info);
    return Standard_False;
  }
  // declaration of messages
  Message_Msg msg2030("IGES_2030");
  TF->Send (msg2030, Message_Info);
  Message_Msg msg2065("IGES_2065");
  OSD_Timer c; c.Reset(); c.Start();   // Initialisation du CHRONO
  
  Handle(IGESData_IGESEntity) ent = theModel->Entity(num);

  Message_ProgressSentry PS ( theProc->GetProgress(), "OneEnt", 0, 1, 1 ); //skl

  XSAlgo::AlgoContainer()->PrepareForTransfer();
  IGESToBRep_CurveAndSurface CAS;
  CAS.SetModel(theModel);
  Standard_Real eps;
  Standard_Integer Ival = Interface_Static::IVal("read.precision.mode");
  Message_Msg msg2035("IGES_2035");
  msg2035.Arg(Ival);
  TF->Send (msg2035, Message_Info);
  if ( Ival == 0)
    eps = theModel->GlobalSection().Resolution();
  else {
    //mjm : modif du 19/12/97 pour prise en compte effective du parametre
    eps = Interface_Static::RVal("read.precision.val");
    Message_Msg msg2040("IGES_2040");
    msg2040.Arg(eps);//#70 rln 03.03.99
    TF->Send (msg2040, Message_Info);
    
  }
  Ival = Interface_Static::IVal("read.iges.bspline.approxd1.mode");
  CAS.SetModeApprox ( (Ival > 0) );
  Message_Msg msg2045("IGES_2045");
  Ival = Interface_Static::IVal("read.iges.bspline.continuity");
  msg2045.Arg(Ival);
  TF->Send (msg2045, Message_Info);
  CAS.SetContinuity(Ival);
  Message_Msg msg2050("IGES_2050");
  Ival = Interface_Static::IVal("read.surfacecurve.mode");
  msg2050.Arg(Ival);
  TF->Send (msg2050, Message_Info);
  CAS.SetSurfaceCurve (Ival);

  if (eps > 1.E-08) CAS.SetEpsGeom(eps);
  CAS.SetTransferProcess(theProc);

  Standard_Boolean exceptionRaised = Standard_False;
  TopoDS_Shape shape;
  Standard_Integer nbTPitems = theProc->NbMapped();
  {
    try {
      OCC_CATCH_SIGNALS
      shape = CAS.TransferGeometry (ent);
    } 
    catch(Standard_Failure) {
      Message_Msg msg1015("IGES_1015");
      TF->Send (msg1015, Message_Info);
      exceptionRaised = Standard_True;
    }
  }
  if (!exceptionRaised) {
    // fixing shape
//    shape = XSAlgo::AlgoContainer()->PerformFixShape ( shape, theProc, eps*CAS.GetUnitFactor(), CAS.GetMaxTol() );

    Handle(Standard_Transient) info;
    shape = XSAlgo::AlgoContainer()->ProcessShape( shape, eps*CAS.GetUnitFactor(), CAS.GetMaxTol(),
                                                   "read.iges.resource.name", 
                                                   "read.iges.sequence", info,
                                                   theProc->GetProgress() );
    XSAlgo::AlgoContainer()->MergeTransferInfo(theProc, info, nbTPitems);

    ShapeExtend_Explorer SBE;
    if (SBE.ShapeType (shape,Standard_True) != TopAbs_SHAPE) {
      TransferBRep::SetShapeResult (theProc,ent,shape);
      theProc->SetRoot (ent);
      if (!shape.IsNull()) {
	theDone = Standard_True;
	EncodeRegul (shape);
	//#74 rln 03.03.99 S4135
	TrimTolerances (shape, CAS.GetMaxTol());
	theShapes.Append(shape);
      }
    }
  }

  PS.Relieve(); //skl

  char t [20];
  t[0]='\0';
  Standard_Real second, cpu;
  Standard_Integer minute, hour;
  c.Show(second, minute, hour,cpu);
  if (hour > 0)
    Sprintf(t,"%dh:%dm:%.2fs",hour,minute,second);
  else if (minute > 0)
    Sprintf(t,"%dm:%.2fs",minute,second);
  else
    Sprintf(t,"%.2fs",second);
  // Sending of message : End of Loading
  msg2065.Arg(t);
  TF->Send (msg2065, Message_Info);
  return Standard_True;
}


//=======================================================================
//function : UsedTolerance
//purpose  : Returns the used tolerance (input)
//=======================================================================
    Standard_Real  IGESToBRep_Reader::UsedTolerance () const
      {  return theActor->UsedTolerance();  }

//=======================================================================
//function : NbShapes
//purpose  : Returns the count of produced Shapes
//=======================================================================
    Standard_Integer  IGESToBRep_Reader::NbShapes () const
      {  return theShapes.Length();  }


//=======================================================================
//function : Shape
//purpose  : Returns a Shape given its rank
//=======================================================================
    TopoDS_Shape  IGESToBRep_Reader::Shape (const Standard_Integer num) const
{
  TopoDS_Shape res;
  if (num > 0 && num <= theShapes.Length()) res = theShapes.Value(num);
  return res;
}


//=======================================================================
//function : OneShape
//purpose  : Returns a unique Shape
//=======================================================================
    TopoDS_Shape  IGESToBRep_Reader::OneShape () const
{
  TopoDS_Shape res;
  Standard_Integer nb = theShapes.Length();
  if (nb == 0) return res;
  else if (nb == 1) return theShapes.Value(1);
  else {
    TopoDS_Compound C;
    BRep_Builder B;
    B.MakeCompound(C);
    for (Standard_Integer i = 1; i <= nb; i ++)  B.Add (C,theShapes.Value(i));
    return C;
  }
}
