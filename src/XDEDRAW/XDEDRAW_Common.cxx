// Created on: 2003-08-15
// Created by: Sergey ZARITCHNY
// Copyright (c) 2003-2014 OPEN CASCADE SAS
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


#include <DDocStd.hxx>
#include <DDocStd_DrawDocument.hxx>
#include <Dico_DictionaryOfTransient.hxx>
#include <Dico_IteratorOfDictionaryOfTransient.hxx>
#include <Draw.hxx>
#include <Draw_Interpretor.hxx>
#include <IFSelect_SessionPilot.hxx>
#include <IGESCAFControl_Reader.hxx>
#include <IGESCAFControl_Writer.hxx>
#include <IGESControl_Controller.hxx>
#include <Interface_Macros.hxx>
#include <STEPCAFControl_DictionaryOfExternFile.hxx>
#include <STEPCAFControl_ExternFile.hxx>
#include <STEPCAFControl_IteratorOfDictionaryOfExternFile.hxx>
#include <STEPCAFControl_Reader.hxx>
#include <STEPCAFControl_Writer.hxx>
#include <STEPControl_Controller.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TDataStd_Name.hxx>
#include <TDF_Data.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>
#include <XDEDRAW.hxx>
#include <XDEDRAW_Common.hxx>
#include <XSControl_WorkSession.hxx>
#include <XSDRAW.hxx>
#include <XSDRAW_Vars.hxx>
#include <XSDRAWIGES.hxx>
#include <XSDRAWSTEP.hxx>
#include <DDF.hxx>

#include <stdio.h>
//============================================================
// Support for several models in DRAW
//============================================================
static Handle(Dico_DictionaryOfTransient) thedictws = new Dico_DictionaryOfTransient;

static Standard_Boolean ClearDicWS()
{
  thedictws->Clear();
  return Standard_True;
}

static void AddWS(TCollection_AsciiString filename,
		  const Handle(XSControl_WorkSession)& WS)
{
  WS->SetVars ( new XSDRAW_Vars ); // support of DRAW variables
  thedictws->SetItem( filename, WS );
}


static Standard_Boolean FillDicWS(Handle(STEPCAFControl_DictionaryOfExternFile)& dicFile)
{
  ClearDicWS();
  if ( dicFile->IsEmpty() ) {
    return Standard_False;
  }
  Handle(STEPCAFControl_ExternFile) EF;
  STEPCAFControl_IteratorOfDictionaryOfExternFile DicEFIt ( dicFile );
  for (; DicEFIt.More(); DicEFIt.Next() ) {
    TCollection_AsciiString filename = DicEFIt.Name();
    EF = DicEFIt.Value();
    AddWS ( filename, EF->GetWS() );
  }
  return Standard_True;  
}

static Standard_Boolean SetCurrentWS (TCollection_AsciiString filename)
{
  if ( !thedictws->HasItem(filename) ) return Standard_False;
  Handle(XSControl_WorkSession) CurrentWS = 
    Handle(XSControl_WorkSession)::DownCast( thedictws->Item(filename) );
  XSDRAW::Pilot()->SetSession( CurrentWS );
  
  return Standard_True;
}


//=======================================================================
//function : SetCurWS
//purpose  : Set current file if many files are read
//=======================================================================

static Standard_Integer SetCurWS (Draw_Interpretor& di , Standard_Integer argc, const char** argv)
{
  if (argc <2) {
    di<<"Use: "<<argv[0]<<" filename "<<"\n";
    return 1;
  }
  TCollection_AsciiString filename (argv[1]);
  SetCurrentWS( filename );
  return 0;
}


//=======================================================================
//function : GetDicWSList
//purpose  : List all files recorded after translation
//=======================================================================

static Standard_Integer GetDicWSList (Draw_Interpretor& di, Standard_Integer /*argc*/, const char** /*argv*/)
{
  Handle(Dico_DictionaryOfTransient) DictWS = thedictws;
  if ( DictWS->IsEmpty() ) return 1;
  Dico_IteratorOfDictionaryOfTransient DicIt ( DictWS );
  di << " The list of last translated files:" << "\n";
  Standard_Integer num = 0;
  for (; DicIt.More() ; DicIt.Next(), num++ ) {
    TCollection_AsciiString strng ( DicIt.Name() );
    if ( num ) di << "\n";
    di << "\"" << strng.ToCString() << "\"";
  }
  return 0;
}

//=======================================================================
//function : GetCurWS
//purpose  : Return name of file which is current
//=======================================================================

static Standard_Integer GetCurWS (Draw_Interpretor& di, Standard_Integer /*argc*/, const char** /*argv*/)
{
  Handle(XSControl_WorkSession) WS = XSDRAW::Session();
  di << "\"" << WS->LoadedFile() << "\"";
  return 0;
}

//=======================================================================
//function : FromShape
//purpose  : Apply fromshape command to all the loaded WSs
//=======================================================================

static Standard_Integer FromShape (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if ( argc <2 ) {
    di << argv[0] << " shape: search for shape origin among all last tranalated files" << "\n";
    return 0;
  }
  
  char command[256];
  Sprintf ( command, "fromshape %.200s -1", argv[1] );
  Handle(Dico_DictionaryOfTransient) DictWS = thedictws;
  if ( DictWS->IsEmpty() ) return di.Eval ( command );
  
  Handle(XSControl_WorkSession) WS = XSDRAW::Session();

  Dico_IteratorOfDictionaryOfTransient DicIt ( DictWS );
//  di << "Searching for shape among all the loaded files:" << "\n";
  Standard_Integer num = 0;
  for (; DicIt.More() ; DicIt.Next(), num++ ) {
    Handle(XSControl_WorkSession) CurrentWS = 
      Handle(XSControl_WorkSession)::DownCast( DicIt.Value() );
    XSDRAW::Pilot()->SetSession( CurrentWS );
    di.Eval ( command );
  }

  XSDRAW::Pilot()->SetSession( WS );
  return 0;
}

//=======================================================================
//function : ReadIges
//purpose  : Read IGES to DECAF document
//=======================================================================

static Standard_Integer ReadIges (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if ( argc <3 ) {
    di << "Use: " << argv[0] << " Doc filename [mode]: read IGES file to a document" << "\n";
    return 0;
  }
  
  DeclareAndCast(IGESControl_Controller,ctl,XSDRAW::Controller());
  if (ctl.IsNull()) XSDRAW::SetNorm("IGES");

  IGESCAFControl_Reader reader ( XSDRAW::Session(),Standard_True);
  
  if (argc == 4) {
    Standard_Boolean mode = Standard_True;
    for ( Standard_Integer i = 0; argv[3][i] ; i++ ) 
      switch (argv[3][i]) {
      case '-' : mode = Standard_False; break;
      case '+' : mode = Standard_True; break;
      case 'c' : reader.SetColorMode (mode); break;
      case 'n' : reader.SetNameMode (mode); break;
      case 'l' : reader.SetLayerMode (mode); break;
      }
  }
  TCollection_AsciiString fnom,rnom;
  Standard_Boolean modfic = XSDRAW::FileAndVar (argv[2],argv[1],"IGES",fnom,rnom);
  if (modfic) di<<" File IGES to read : "<<fnom.ToCString()<<"\n";
  else        di<<" Model taken from the session : "<<fnom.ToCString()<<"\n";
//  di<<" -- Names of variables BREP-DRAW prefixed by : "<<rnom<<"\n";
  IFSelect_ReturnStatus readstat = IFSelect_RetVoid;
  if (modfic) readstat = reader.ReadFile (fnom.ToCString());
  else  if (XSDRAW::Session()->NbStartingEntities() > 0) readstat = IFSelect_RetDone;
  if (readstat != IFSelect_RetDone) {
    if (modfic) di<<"Could not read file "<<fnom.ToCString()<<" , abandon"<<"\n";
    else di<<"No model loaded"<<"\n";
    return 1;
  }

  Handle(TDocStd_Document) doc;
  if (!DDocStd::GetDocument(argv[1],doc,Standard_False)) {  
    Handle(TDocStd_Application) A;
    if (!DDocStd::Find(A)) {di<<"No application found"<<"\n";return 1;}
    A->NewDocument("BinXCAF",doc);  
    TDataStd_Name::Set(doc->GetData()->Root(),argv[1]);  
    Handle(DDocStd_DrawDocument) DD = new DDocStd_DrawDocument(doc);  
    Draw::Set(argv[1],DD);       
//     di << "Document saved with name " << argv[1];
  }
  if ( ! reader.Transfer ( doc ) ) {
    di << "Cannot read any relevant data from the IGES file" << "\n";
    return 1;
  }
  
//  Handle(DDocStd_DrawDocument) DD = new DDocStd_DrawDocument(doc);  
//  Draw::Set(argv[1],DD);       
  di << "Document saved with name " << argv[1];
  
  return 0;
}

//=======================================================================
//function : WriteIges
//purpose  : Write DECAF document to IGES
//=======================================================================

static Standard_Integer WriteIges (Draw_Interpretor& di, Standard_Integer argc, const char** argv) 
{
  if ( argc <3 ) {
    di << "Use: " << argv[0] << " Doc filename [mode]: write document to IGES file" << "\n";
    return 0;
  }
  
  Handle(TDocStd_Document) Doc;
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) {
    di << argv[1] << " is not a document" << "\n";
    return 1;
  }
  
  XSDRAW::SetNorm ("IGES");
  
//  IGESControl_Writer ICW (Interface_Static::CVal("write.iges.unit"),
//			  Interface_Static::IVal("write.iges.brep.mode"));
    
  IGESCAFControl_Writer writer ( XSDRAW::Session(), Standard_True );
  if (argc == 4) {
    Standard_Boolean mode = Standard_True;
    for ( Standard_Integer i = 0; argv[3][i] ; i++ ) 
      switch (argv[3][i]) {
      case '-' : mode = Standard_False; break;
      case '+' : mode = Standard_True; break;
      case 'c' : writer.SetColorMode (mode); break;
      case 'n' : writer.SetNameMode (mode); break;
      case 'l' : writer.SetLayerMode (mode); break;
      }
  }
  writer.Transfer ( Doc );

  di << "Writig IGES model to file " << argv[2] << "\n";
  if ( writer.Write ( argv[2] ) ) di<<" Write OK"<<"\n";
  else di<<" Write failed"<<"\n";

  return 0;
}

//=======================================================================
//function : ReadStep
//purpose  : Read STEP file to DECAF document 
//=======================================================================

static Standard_Integer ReadStep (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if ( argc <3 ) {
    di << "Use: " << argv[0] << " Doc filename [mode]: read STEP file to a document" << "\n";
    return 0;
  }
  
  DeclareAndCast(STEPControl_Controller,ctl,XSDRAW::Controller());
  if (ctl.IsNull()) XSDRAW::SetNorm("STEP");

  STEPCAFControl_Reader reader ( XSDRAW::Session(),Standard_True);
  
  if (argc == 4) {
    Standard_Boolean mode = Standard_True;
    for ( Standard_Integer i = 0; argv[3][i] ; i++ ) 
      switch (argv[3][i]) {
      case '-' : mode = Standard_False; break;
      case '+' : mode = Standard_True; break;
      case 'c' : reader.SetColorMode (mode); break;
      case 'n' : reader.SetNameMode (mode); break;
      case 'l' : reader.SetLayerMode (mode); break;
      case 'v' : reader.SetPropsMode (mode); break;
      }
  }
  
  TCollection_AsciiString fnom,rnom;
  Standard_Boolean modfic = XSDRAW::FileAndVar (argv[2],argv[1],"STEP",fnom,rnom);
  if (modfic) di<<" File STEP to read : "<<fnom.ToCString()<<"\n";
  else        di<<" Model taken from the session : "<<fnom.ToCString()<<"\n";
//  di<<" -- Names of variables BREP-DRAW prefixed by : "<<rnom<<"\n";
  IFSelect_ReturnStatus readstat = IFSelect_RetVoid;
  if (modfic) readstat = reader.ReadFile (fnom.ToCString());
  else  if (XSDRAW::Session()->NbStartingEntities() > 0) readstat = IFSelect_RetDone;
  if (readstat != IFSelect_RetDone) {
    if (modfic) di<<"Could not read file "<<fnom.ToCString()<<" , abandon"<<"\n";
    else di<<"No model loaded"<<"\n";
    return 1;
  }

  Handle(TDocStd_Document) doc;
  if (!DDocStd::GetDocument(argv[1],doc,Standard_False)) {  
    Handle(TDocStd_Application) A;
    if (!DDocStd::Find(A)) {di<<"No application found"<<"\n";return 1;}
    A->NewDocument("BinXCAF",doc);  
    TDataStd_Name::Set(doc->GetData()->Root(),argv[1]);  
    Handle(DDocStd_DrawDocument) DD = new DDocStd_DrawDocument(doc);  
    Draw::Set(argv[1],DD);       
//     di << "Document saved with name " << argv[1];
  }
  if ( ! reader.Transfer ( doc ) ) {
    di << "Cannot read any relevant data from the STEP file" << "\n";
    return 1;
  }
  
  Handle(DDocStd_DrawDocument) DD = new DDocStd_DrawDocument(doc);  
  Draw::Set(argv[1],DD);       
  di << "Document saved with name " << argv[1];

  Handle(STEPCAFControl_DictionaryOfExternFile) DicFile = reader.ExternFiles();
  FillDicWS( DicFile );
  AddWS ( fnom , XSDRAW::Session() );
  
  return 0;
}

//=======================================================================
//function : WriteStep
//purpose  : Write DECAF document to STEP
//=======================================================================

static Standard_Integer WriteStep (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if ( argc <3 ) {
    di << "Use: " << argv[0] << " Doc filename [mode [multifile_prefix [label]]]: write document to the STEP file" << "\n";
    di << "mode can be: a or 0 : AsIs (default)" << "\n";
    di << "             f or 1 : FacettedBRep        s or 2 : ShellBasedSurfaceModel\n";
    di << "             m or 3 : ManifoldSolidBrep   w or 4 : GeometricCurveSet/WireFrame\n";
    di << "multifile_prefix: triggers writing assembly components as separate files,\n";
    di << "                  and defines common prefix for their names\n";
    di << "label: tag of the sub-assembly label to save only that sub-assembly\n";
    return 0;
  }
  
  Handle(TDocStd_Document) Doc;   
  DDocStd::GetDocument(argv[1], Doc);
  if ( Doc.IsNull() ) {
    di << argv[1] << " is not a document" << "\n";
    return 1;
  }
  Standard_CString multifile = 0;
  
  Standard_Integer k = 3;
  DeclareAndCast(STEPControl_Controller,ctl,XSDRAW::Controller());
  if (ctl.IsNull()) XSDRAW::SetNorm("STEP");
  STEPCAFControl_Writer writer ( XSDRAW::Session(), Standard_True );
   
  STEPControl_StepModelType mode = STEPControl_AsIs;
  if ( argc > k ) {
    switch (argv[k][0]) {
    case 'a' :
    case '0' : mode = STEPControl_AsIs;                    break;
    case 'f' :
    case '1' : mode = STEPControl_FacetedBrep;             break;
    case 's' :
    case '2' : mode = STEPControl_ShellBasedSurfaceModel;  break;
    case 'm' :
    case '3' : mode = STEPControl_ManifoldSolidBrep;       break;
    case 'w' :
    case '4' : mode = STEPControl_GeometricCurveSet;       break;
    default :  di<<"3st arg = mode, incorrect [give fsmw]"<<"\n"; return 1;
    }
    Standard_Boolean wrmode = Standard_True;
    for ( Standard_Integer i = 0; argv[k][i] ; i++ ) 
      switch (argv[3][i]) {
      case '-' : wrmode = Standard_False; break;
      case '+' : wrmode = Standard_True; break;
      case 'c' : writer.SetColorMode (wrmode); break;
      case 'n' : writer.SetNameMode (wrmode); break;
      case 'l' : writer.SetLayerMode (wrmode); break;
      case 'v' : writer.SetPropsMode (wrmode); break;
      }
    k++;
  }

  TDF_Label label;
  if( argc > k)
  {
    TCollection_AsciiString aStr(argv[k]);
    if( aStr.Search(":") ==-1)
      multifile = argv[k++];
    
  }
  if( argc > k)
  {
      
    if( !DDF::FindLabel(Doc->Main().Data(), argv[k], label) || label.IsNull()) {  
      di << "No label for entry"  << "\n";
      return 1; 
      
    }
  }
  if( !label.IsNull())
  {  
    di << "Translating label "<< argv[k]<<" of document " << argv[1] << " to STEP" << "\n";
    if(!writer.Transfer ( label, mode, multifile )) 
    {
      di << "The label of document cannot be translated or gives no result" << "\n";
      return 1;
    }

  }
  else
  {
    di << "Translating document " << argv[1] << " to STEP" << "\n";
    if ( ! writer.Transfer ( Doc, mode, multifile ) ) {
      di << "The document cannot be translated or gives no result" << "\n";
    }
  }
  

  di << "Writing STEP file " << argv[2] << "\n";
  IFSelect_ReturnStatus stat = writer.Write(argv[2]);
  switch (stat) {
    case IFSelect_RetVoid : di<<"No file written"<<"\n"; break;
    case IFSelect_RetDone : {
      di<<"File "<<argv[2]<<" written"<<"\n";

      Handle(STEPCAFControl_DictionaryOfExternFile) DicFile = writer.ExternFiles();
      FillDicWS( DicFile );
      AddWS( argv[2], XSDRAW::Session() );
      break;
    }
    default : di<<"Error on writing file"<<"\n"; break;
  }
  return 0;
}

void XDEDRAW_Common::InitCommands(Draw_Interpretor& di) {

  static Standard_Boolean initactor = Standard_False;
  if (initactor) return;  initactor = Standard_True;

  Standard_CString g = "XDE translation commands";

  di.Add("ReadIges" , "Doc filename: Read IGES file to DECAF document" ,__FILE__, ReadIges, g);
  di.Add("WriteIges" , "Doc filename: Write DECAF document to IGES file" ,__FILE__, WriteIges, g);
  di.Add("ReadStep" , "Doc filename: Read STEP file to DECAF document" ,__FILE__, ReadStep, g);
  di.Add("WriteStep" , "Doc filename [mode=a [multifile_prefix] [label]]: Write DECAF document to STEP file" ,__FILE__, WriteStep, g);  
  
  di.Add("XFileList","Print list of files that was transfered by the last transfer" ,__FILE__, GetDicWSList , g);
  di.Add("XFileCur", ": returns name of file which is set as current",__FILE__, GetCurWS, g);
  di.Add("XFileSet", "filename: Set the specified file to be the current one",__FILE__, SetCurWS, g);
  di.Add("XFromShape", "shape: do fromshape command for all the files",__FILE__, FromShape, g);

}
