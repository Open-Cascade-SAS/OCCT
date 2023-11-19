// Copyright (c) 2023 OPEN CASCADE SAS
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

#include <XSDRAWIGES.hxx>

#include <BRepTools.hxx>
#include <DBRep.hxx>
#include <DDocStd.hxx>
#include <DDocStd_DrawDocument.hxx>
#include <Draw.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_PluginMacro.hxx>
#include <Draw_ProgressIndicator.hxx>
#include <DrawTrSurf.hxx>
#include <IGESCAFControl_Reader.hxx>
#include <IGESCAFControl_Writer.hxx>
#include <IGESControl_Controller.hxx>
#include <IGESControl_Reader.hxx>
#include <IGESControl_Writer.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESModel.hxx>
#include <IGESSelect_Activator.hxx>
#include <Interface_Macros.hxx>
#include <Interface_Static.hxx>
#include <Message.hxx>
#include <OSD_OpenFile.hxx>
#include <OSD_Path.hxx>
#include <TColStd_Array1OfAsciiString.hxx>
#include <TColStd_MapIteratorOfMapOfTransient.hxx>
#include <TDataStd_Name.hxx>
#include <TDocStd_Application.hxx>
#include <TopoDS_Shape.hxx>
#include <Transfer_IteratorOfProcessForTransient.hxx>
#include <Transfer_TransientProcess.hxx>
#include <XSAlgo.hxx>
#include <XSAlgo_AlgoContainer.hxx>
#include <XSControl_TransferReader.hxx>
#include <XSControl_WorkSession.hxx>
#include <XSDRAW.hxx>

//=======================================================================
//function : WriteShape
//purpose  : Creates a file Shape_'number'
//=======================================================================
void WriteShape(const TopoDS_Shape& shape,
                const Standard_Integer number)
{
  char fname[110];
  sprintf(fname, "Shape_%d", number);
  std::ofstream f(fname, std::ios::out | std::ios::binary);
  std::cout << "Output file name : " << fname << std::endl;
  f << "DBRep_DrawableShape\n";

  BRepTools::Write(shape, f);
  f.close();
}

//=======================================================================
//function : XSDRAW_CommandPart
//purpose  :
//=======================================================================
TCollection_AsciiString XSDRAW_CommandPart(Standard_Integer argc,
                                           const char** argv,
                                           const Standard_Integer argf)
{
  TCollection_AsciiString res;
  for (Standard_Integer i = argf; i < argc; i++)
  {
    if (i > argf) res.AssignCat(" ");
    res.AssignCat(argv[i]);
  }
  return res;
}

//=======================================================================
//function : GiveEntityNumber
//purpose  :
//=======================================================================
static Standard_Integer GiveEntityNumber(const Handle(XSControl_WorkSession)& WS,
                                         const Standard_CString name)
{
  Standard_Integer num = 0;
  if (!name || name[0] == '\0')
  {
    char ligne[80];  ligne[0] = '\0';
    std::cin >> ligne;
    //    std::cin.clear();  std::cin.getline (ligne,79);
    if (ligne[0] == '\0') return 0;
    num = WS->NumberFromLabel(ligne);
  }
  else num = WS->NumberFromLabel(name);
  return num;
}

//=======================================================================
//function : FileAndVar
//purpose  : 
//=======================================================================
Standard_Boolean FileAndVar(const Handle(XSControl_WorkSession)& session,
                            const Standard_CString file,
                            const Standard_CString var,
                            const Standard_CString def,
                            TCollection_AsciiString& resfile,
                            TCollection_AsciiString& resvar)
{
  Standard_Boolean iafic = Standard_True;
  resfile.Clear();  resvar.Clear();
  if (file)
    if (file[0] == '\0' ||
        (file[0] == '.' && file[1] == '\0')) iafic = Standard_False;
  if (!iafic) resfile.AssignCat(session->LoadedFile());
  else        resfile.AssignCat(file);

  if (var && var[0] != '\0' && (var[0] != '.' || var[1] != '\0'))
    resvar.AssignCat(var);
  else if (resfile.Length() == 0) resvar.AssignCat(def);
  else
  {
    Standard_Integer nomdeb, nomfin;
    nomdeb = resfile.SearchFromEnd("/");
    if (nomdeb <= 0) nomdeb = resfile.SearchFromEnd("\\");  // pour NT
    if (nomdeb < 0) nomdeb = 0;
    nomfin = resfile.SearchFromEnd(".");
    if (nomfin < nomdeb) nomfin = resfile.Length() + 1;
    resvar = resfile.SubString(nomdeb + 1, nomfin - 1);
  }
  return iafic;
}

//=======================================================================
//function : igesbrep
//purpose  :
//=======================================================================
static Standard_Integer igesbrep(Draw_Interpretor& theDI,
                                 Standard_Integer theNbArgs,
                                 const char** theArgVec)
{
  Handle(XSControl_WorkSession) aWS = XSDRAW::Session();
  Handle(IGESControl_Controller) aCtl =
    Handle(IGESControl_Controller)::DownCast(aWS->NormAdaptor());
  if (aCtl.IsNull())
  {
    aWS->SelectNorm("IGES");
  }

  // Progress indicator
  Handle(Draw_ProgressIndicator) progress = new Draw_ProgressIndicator(theDI, 1);
  Message_ProgressScope aPSRoot(progress->Start(), "Reading", 100);

  IGESControl_Reader Reader(XSDRAW::Session(), Standard_False);
  Standard_Boolean aFullMode = Standard_True;
  Reader.WS()->SetModeStat(aFullMode);

  TCollection_AsciiString fnom, rnom;

  Standard_Boolean modfic = FileAndVar
  (aWS, theArgVec[1], theArgVec[2], "IGESBREP", fnom, rnom);
  if (modfic) theDI << " File IGES to read : " << fnom.ToCString() << "\n";
  else        theDI << " Model taken from the session : " << fnom.ToCString() << "\n";
  theDI << " -- Names of variables BREP-DRAW prefixed by : " << rnom.ToCString() << "\n";
  IFSelect_ReturnStatus readstat = IFSelect_RetVoid;

#ifdef CHRONOMESURE
  OSD_Timer Chr; Chr.Reset();
  IDT_SetLevel(3);
#endif

  // Reading the file
  aPSRoot.SetName("Loading");
  progress->Show(aPSRoot);

  if (modfic) readstat = Reader.ReadFile(fnom.ToCString());
  else  if (XSDRAW::Session()->NbStartingEntities() > 0) readstat = IFSelect_RetDone;

  aPSRoot.Next(20); // On average loading takes 20% 
  if (aPSRoot.UserBreak())
    return 1;

  if (readstat != IFSelect_RetDone)
  {
    if (modfic) theDI << "Could not read file " << fnom.ToCString() << " , abandon\n";
    else theDI << "No model loaded\n";
    return 1;
  }
  // Choice of treatment
  Standard_Boolean fromtcl = (theNbArgs > 3);
  Standard_Integer modepri = 1, nent, nbs;
  if (fromtcl) modepri = 4;

  while (modepri)
  {
    //Roots for transfer are defined before setting mode ALL or OnlyVisible - gka 
    //mode OnlyVisible does not work.
    // nent = Reader.NbRootsForTransfer();
    if (!fromtcl)
    {
      std::cout << "Mode (0 End, 1 Visible Roots, 2 All Roots, 3 Only One Entity, 4 Selection) :" << std::flush;
      modepri = -1;

      // amv 26.09.2003 : this is used to avoid error of enter's simbol        
      char str[80];
      std::cin >> str;
      modepri = Draw::Atoi(str);
    }

    if (modepri == 0)
    {  //fin
      theDI << "Bye and good luck! \n";
      break;
    }

    else if (modepri <= 2)
    {  // 1 : Visible Roots, 2 : All Roots
      theDI << "All Geometry Transfer\n";
      theDI << "spline_continuity (read) : " << Interface_Static::IVal("read.iges.bspline.continuity") << " (0 : no modif, 1 : C1, 2 : C2)\n";
      theDI << "  To modify : command  param read.iges.bspline.continuity\n";
      const Handle(XSControl_WorkSession)& thesession = Reader.WS();
      thesession->TransferReader()->Context().Clear();

      aPSRoot.SetName("Translation");
      progress->Show(aPSRoot);

      if (modepri == 1) Reader.SetReadVisible(Standard_True);
      Reader.TransferRoots(aPSRoot.Next(80));

      if (aPSRoot.UserBreak())
        return 1;

      // result in only one shape for all the roots
      //        or in one shape for one root.
      theDI << "Count of shapes produced : " << Reader.NbShapes() << "\n";
      Standard_Integer answer = 1;
      if (Reader.NbShapes() > 1)
      {
        std::cout << " pass(0)  one shape for all (1)\n or one shape per root (2)\n + WriteBRep (one for all : 3) (one per root : 4) : " << std::flush;
        answer = -1;
        //amv 26.09.2003                                                        
        char str_a[80];
        std::cin >> str_a;
        answer = Draw::Atoi(str_a);
      }
      if (answer == 0) continue;
      if (answer == 1 || answer == 3)
      {
        TopoDS_Shape shape = Reader.OneShape();
        // save the shape
        if (shape.IsNull()) { theDI << "No Shape produced\n"; continue; }
        char fname[110];
        Sprintf(fname, "%s", rnom.ToCString());
        theDI << "Saving shape in variable Draw : " << fname << "\n";
        if (answer == 3) WriteShape(shape, 1);
        try
        {
          OCC_CATCH_SIGNALS
            DBRep::Set(fname, shape);
        }
        catch (Standard_Failure const& anException)
        {
          theDI << "** Exception : ";
          theDI << anException.GetMessageString();
          theDI << " ** Skip\n";
          theDI << "Saving shape in variable Draw : " << fname << "\n";
          WriteShape(shape, 1);
        }
      }

      else if (answer == 2 || answer == 4)
      {
        Standard_Integer numshape = Reader.NbShapes();
        for (Standard_Integer inum = 1; inum <= numshape; inum++)
        {
          // save all the shapes
          TopoDS_Shape shape = Reader.Shape(inum);
          if (shape.IsNull()) { theDI << "No Shape produced\n"; continue; }
          char fname[110];
          Sprintf(fname, "%s_%d", rnom.ToCString(), inum);
          theDI << "Saving shape in variable Draw : " << fname << "\n";
          if (answer == 4) WriteShape(shape, inum);
          try
          {
            OCC_CATCH_SIGNALS
              DBRep::Set(fname, shape);
          }
          catch (Standard_Failure const& anException)
          {
            theDI << "** Exception : ";
            theDI << anException.GetMessageString();
            theDI << " ** Skip\n";
          }
        }
      }
      else return 0;
    }

    else if (modepri == 3)
    {  // One Entity
      std::cout << "Only One Entity" << std::endl;
      std::cout << "spline_continuity (read) : " << Interface_Static::IVal("read.iges.bspline.continuity") << " (0 : no modif, 1 : C1, 2 : C2)" << std::endl;
      std::cout << "  To modify : command  param read.iges.bspline.continuity" << std::endl;
      std::cout << " give the number of the Entity : " << std::flush;
      nent = GiveEntityNumber(aWS, "");

      if (!Reader.TransferOne(nent))
        theDI << "Transfer entity n0 " << nent << " : no result\n";
      else
      {
        nbs = Reader.NbShapes();
        char shname[30];  Sprintf(shname, "%s_%d", rnom.ToCString(), nent);
        theDI << "Transfer entity n0 " << nent << " OK  -> DRAW Shape: " << shname << "\n";
        theDI << "Now, " << nbs << " Shapes produced\n";
        TopoDS_Shape sh = Reader.Shape(nbs);
        DBRep::Set(shname, sh);
      }
    }

    else if (modepri == 4)
    {   // Selection
      Standard_Integer answer = 1;
      Handle(TColStd_HSequenceOfTransient)  list;

      //  Selection, nommee ou via tcl. tcl : raccourcis admis
      //   * donne iges-visible + xst-transferrable-roots
      //   *r donne xst-model-roots (TOUTES racines)

      if (fromtcl && theArgVec[3][0] == '*' && theArgVec[3][1] == '\0')
      {
        theDI << "All Geometry Transfer\n";
        theDI << "spline_continuity (read) : " << Interface_Static::IVal("read.iges.bspline.continuity") << " (0 : no modif, 1 : C1, 2 : C2)\n";
        theDI << "  To modify : command  param read.iges.bspline.continuity\n";
        const Handle(XSControl_WorkSession)& thesession = Reader.WS();
        thesession->TransferReader()->Context().Clear();

        aPSRoot.SetName("Translation");
        progress->Show(aPSRoot);

        Reader.SetReadVisible(Standard_True);
        Reader.TransferRoots(aPSRoot.Next(80));

        if (aPSRoot.UserBreak())
          return 1;

        // result in only one shape for all the roots
        TopoDS_Shape shape = Reader.OneShape();
        // save the shape
        char fname[110];
        Sprintf(fname, "%s", rnom.ToCString());
        theDI << "Saving shape in variable Draw : " << fname << "\n";
        try
        {
          OCC_CATCH_SIGNALS
            DBRep::Set(fname, shape);
        }
        catch (Standard_Failure const& anException)
        {
          theDI << "** Exception : ";
          theDI << anException.GetMessageString();
          theDI << " ** Skip\n";
          theDI << "Saving shape in variable Draw : " << fname << "\n";
          WriteShape(shape, 1);
        }
        return 0;
      }

      if (fromtcl)
      {
        modepri = 0;    // d office, une seule passe
        if (theArgVec[3][0] == '*' && theArgVec[3][1] == 'r' && theArgVec[3][2] == '\0')
        {
          theDI << "All Roots : ";
          list = XSDRAW::Session()->GiveList("xst-model-roots");
        }
        else
        {
          TCollection_AsciiString compart = XSDRAW_CommandPart(theNbArgs, theArgVec, 3);
          theDI << "List given by " << compart.ToCString() << " : ";
          list = XSDRAW::Session()->GiveList(compart.ToCString());
        }
        if (list.IsNull())
        {
          theDI << "No list defined. Give a selection name or * for all visible transferrable roots\n";
          continue;
        }
      }
      else
      {
        std::cout << "Name of Selection :" << std::flush;
        list = XSDRAW::Session()->GiveList("");
        if (list.IsNull()) { std::cout << "No list defined" << std::endl; continue; }
      }

      Standard_Integer nbl = list->Length();
      theDI << "Nb entities selected : " << nbl << "\n";
      if (nbl == 0) continue;
      while (answer)
      {
        if (!fromtcl)
        {
          std::cout << "Choice: 0 abandon  1 transfer all  2 with confirmation  3 list n0s ents :" << std::flush;
          answer = -1;
          // anv 26.09.2003                                                     
          char str_answer[80];
          std::cin >> str_answer;
          answer = Draw::Atoi(str_answer);
        }
        if (answer <= 0 || answer > 3) continue;
        if (answer == 3)
        {
          for (Standard_Integer ill = 1; ill <= nbl; ill++)
          {
            Handle(Standard_Transient) ent = list->Value(ill);
            theDI << "  ";// model->Print(ent,theDI);
          }
          theDI << "\n";
        }
        if (answer == 1 || answer == 2)
        {
          Standard_Integer nbt = 0;
          Handle(XSControl_WorkSession) thesession = Reader.WS();

          aPSRoot.SetName("Translation");
          progress->Show(aPSRoot);

          Message_ProgressScope aPS(aPSRoot.Next(80), "Root", nbl);
          for (Standard_Integer ill = 1; ill <= nbl && aPS.More(); ill++)
          {
            nent = Reader.Model()->Number(list->Value(ill));
            if (nent == 0) continue;
            if (!Reader.TransferOne(nent, aPS.Next()))
              theDI << "Transfer entity n0 " << nent << " : no result\n";
            else
            {
              nbs = Reader.NbShapes();
              char shname[30];  Sprintf(shname, "%s_%d", rnom.ToCString(), nbs);
              theDI << "Transfer entity n0 " << nent << " OK  -> DRAW Shape: " << shname << "\n";
              theDI << "Now, " << nbs << " Shapes produced\n";
              TopoDS_Shape sh = Reader.Shape(nbs);
              DBRep::Set(shname, sh);
              nbt++;
            }
          }
          if (aPSRoot.UserBreak())
            return 1;
          theDI << "Nb Shapes successfully produced : " << nbt << "\n";
          answer = 0;  // on ne reboucle pas
        }
      }
    }
    else theDI << "Unknown mode n0 " << modepri << "\n";
  }
  return 0;
}

//=======================================================================
//function : testread
//purpose  :
//=======================================================================
static Standard_Integer testread(Draw_Interpretor& theDI,
                                 Standard_Integer theNbArgs,
                                 const char** theArgVec)
{
  if (theNbArgs != 3)
  {
    theDI << "ERROR in " << theArgVec[0] << "Wrong Number of Arguments.\n";
    theDI << " Usage : " << theArgVec[0] << " file_name shape_name\n";
    return 1;
  }
  IGESControl_Reader Reader;
  Standard_CString filename = theArgVec[1];
  IFSelect_ReturnStatus readstat = Reader.ReadFile(filename);
  theDI << "Status from reading IGES file " << filename << " : ";
  switch (readstat)
  {
    case IFSelect_RetVoid: { theDI << "empty file\n"; return 1; }
    case IFSelect_RetDone: { theDI << "file read\n";    break; }
    case IFSelect_RetError: { theDI << "file not found\n";   return 1; }
    case IFSelect_RetFail: { theDI << "error during read\n";  return 1; }
    default: { theDI << "failure\n";   return 1; }
  }
  Reader.TransferRoots();
  TopoDS_Shape shape = Reader.OneShape();
  DBRep::Set(theArgVec[2], shape);
  theDI << "Count of shapes produced : " << Reader.NbShapes() << "\n";
  return 0;
}

//=======================================================================
//function : brepiges
//purpose  :
//=======================================================================
static Standard_Integer brepiges(Draw_Interpretor& theDI,
                                 Standard_Integer theNbArgs,
                                 const char** theArgVec)
{
  Handle(XSControl_WorkSession) aWorkSession = XSDRAW::Session();
  aWorkSession->SelectNorm("IGES");

  IGESControl_Writer anIgesWriter(Interface_Static::CVal("write.iges.unit"),
    Interface_Static::IVal("write.iges.brep.mode"));
  theDI << "unit (write) : " << Interface_Static::CVal("write.iges.unit") << "\n";
  theDI << "mode  write  : " << Interface_Static::CVal("write.iges.brep.mode") << "\n";
  theDI << "  To modify : command  param\n";

  const char* aFileName = nullptr;
  Standard_Integer aNumShapesProcessed = 0;

  Handle(Draw_ProgressIndicator) aProgress = new Draw_ProgressIndicator(theDI, 1);
  Message_ProgressScope aRootProgress(aProgress->Start(), "Translating", 100);
  aProgress->Show(aRootProgress);

  Message_ProgressScope aStepProgress(aRootProgress.Next(90), NULL, theNbArgs);
  for (Standard_Integer i = 1; i < theNbArgs && aStepProgress.More(); i++)
  {
    const char* aVariableName = theArgVec[i];
    if (theArgVec[i][0] == '+') aVariableName = &(theArgVec[i])[1];
    else if (i > 1) {
      aFileName = theArgVec[i];  break;
    }

    TopoDS_Shape aShape = DBRep::Get(aVariableName);
    if (anIgesWriter.AddShape(aShape, aStepProgress.Next())) aNumShapesProcessed++;
    else if (anIgesWriter.AddGeom(DrawTrSurf::GetCurve(aVariableName))) aNumShapesProcessed++;
    else if (anIgesWriter.AddGeom(DrawTrSurf::GetSurface(aVariableName))) aNumShapesProcessed++;
  }
  anIgesWriter.ComputeModel();

  if (aRootProgress.UserBreak())
    return 1;

  aRootProgress.SetName("Writing");
  aProgress->Show(aRootProgress);

  theDI << aNumShapesProcessed << " Shapes written, giving " << anIgesWriter.Model()->NbEntities() << " Entities\n";

  if (!aFileName) // delayed write
  {
    theDI << " Now, to write a file, command : writeall filename\n";
    return 0;
  }

  // write file
  if (!anIgesWriter.Write(aFileName))
  {
    theDI << " Error: could not write file " << aFileName << "\n";
    return 1;
  }
  theDI << " File " << aFileName << " written\n";
  aWorkSession->SetModel(anIgesWriter.Model());

  return 0;
}

//=======================================================================
//function : testwrite
//purpose  :
//=======================================================================
static Standard_Integer testwrite(Draw_Interpretor& theDI,
                                  Standard_Integer theNbArgs,
                                  const char** theArgVec)
{
  if (theNbArgs != 3)
  {
    theDI << "ERROR in " << theArgVec[0] << "Wrong Number of Arguments.\n";
    theDI << " Usage : " << theArgVec[0] << " file_name shape_name\n";
    return 1;
  }
  IGESControl_Writer Writer;
  Standard_CString filename = theArgVec[1];
  TopoDS_Shape shape = DBRep::Get(theArgVec[2]);
  Standard_Boolean ok = Writer.AddShape(shape);
  if (!ok)
  {
    theDI << "Shape not add\n";
    return 1;
  }

  if (!(Writer.Write(filename)))
  {
    theDI << "Error on writing file\n";
    return 1;
  }
  theDI << "File Is Written\n";
  return 0;
}

//=======================================================================
//function : igesparam
//purpose  :
//=======================================================================
static Standard_Integer igesparam(Draw_Interpretor& theDI,
                                  Standard_Integer,
                                  const char**)
{
  //  liste des parametres
  theDI << "List of parameters which control IGES :\n";
  theDI << "  unit : write.iges.unit\n  mode write : write.iges.brep.mode\n  spline_continuity (read) : read.iges.bspline.continuity\nSee definition by  defparam, read/edit value by  param\n";
  theDI << "unit (write) : " << Interface_Static::CVal("write.iges.unit") << "\n";
  theDI << "mode  write  : " << Interface_Static::CVal("write.iges.brep.mode") << "\n";
  theDI << "spline_continuity (read) : " << Interface_Static::IVal("read.iges.bspline.continuity") << " (0 : no modif, 1 : C1, 2 : C2)\n";
  theDI << "\n To modifier, param nom_param new_val\n";
  return 0;
}

//=======================================================================
//function : XSDRAWIGES_tplosttrim
//purpose  :
//=======================================================================
static Standard_Integer XSDRAWIGES_tplosttrim(Draw_Interpretor& theDI,
                                              Standard_Integer theNbArgs,
                                              const char** theArgVec)
{
  Handle(XSControl_WorkSession) aWorkSession = XSDRAW::Session();
  const Handle(Transfer_TransientProcess)& anTransientProcess = aWorkSession->TransferReader()->TransientProcess();
  TColStd_Array1OfAsciiString aTypeStrings(1, 3);
  TColStd_Array1OfAsciiString aKindStrings(1, 3);
  aTypeStrings.SetValue(1, "xst-type(CurveOnSurface)");
  aTypeStrings.SetValue(2, "xst-type(Boundary)");
  aTypeStrings.SetValue(3, "xst-type(Loop)");
  aKindStrings.SetValue(1, "IGESGeom_TrimmedSurface");
  aKindStrings.SetValue(2, "IGESGeom_BoundedSurface");
  aKindStrings.SetValue(3, "IGESSolid_Face");
  if (anTransientProcess.IsNull())
  {
    theDI << "No Transfer Read\n";
    return 1;
  }
  Standard_Integer anNumFaces = 0, aTotalFaces = 0;
  Transfer_IteratorOfProcessForTransient anIterator = anTransientProcess->AbnormalResult();
  Standard_Integer anIndex = 0;
  if (theNbArgs > 1)
  {
    TCollection_AsciiString anArg(theArgVec[1]);
    for (anIndex = 1; anIndex <= 3; anIndex++)
    {
      if (aKindStrings.Value(anIndex).Location(anArg, 1, aKindStrings.Value(anIndex).Length()) != 0)
      {
        break;
      }
    }
  }

  if (anIndex == 4)
  {
    theDI << "Invalid argument\n";
    return 0;
  }

  for (Standard_Integer j = 1; j <= 3; j++)
  {
    TColStd_MapOfTransient aFaceMap;

    if (theNbArgs == 1)
    {
      anIndex = j;
    }
    Handle(TColStd_HSequenceOfTransient) aFaceList = aWorkSession->GiveList(aTypeStrings.Value(anIndex).ToCString());
    if (!aFaceList.IsNull())
    {
      anIterator.Filter(aFaceList);
    }
    else
    {
      theDI << "No untrimmed faces\n";
      return 0;
    }
    for (anIterator.Start(); anIterator.More(); anIterator.Next())
    {
      Handle(Standard_Transient) anEntity = anIterator.Starting();
      Handle(TColStd_HSequenceOfTransient) aSharingEntities = aWorkSession->Sharings(anEntity);
      if (!aSharingEntities.IsNull())
      {
        Standard_Integer aNumSharingEntities = aSharingEntities->Length();
        if (aNumSharingEntities > 0)
        {
          for (Standard_Integer i = 1; i <= aNumSharingEntities; i++)
          {
            if (aSharingEntities->Value(i)->IsKind(aKindStrings.Value(anIndex).ToCString()))
            {
              if (aFaceMap.Add(aSharingEntities->Value(i)))
              {
                anNumFaces++;
              }
            }
          }
        }
      }
    }
    if (anNumFaces != 0)
    {
      if (j == 1)
      {
        theDI << "Number of untrimmed faces: \n";
      }
      switch (anIndex)
      {
      case 1:
        theDI << "Trimmed Surface: \n";
        break;
      case 2:
        theDI << "Bounded Surface: \n";
        break;
      case 3:
        theDI << "Face: \n";
        break;
      }
      TColStd_MapIteratorOfMapOfTransient anMapIterator;
      Standard_SStream aTmpStream;
      for (anMapIterator.Initialize(aFaceMap); anMapIterator.More(); anMapIterator.Next())
      {
        aWorkSession->Model()->Print(anMapIterator.Key(), aTmpStream);
        aTmpStream << "  ";
      }
      theDI << aTmpStream.str().c_str();
      theDI << "\n";
      theDI << "\nNumber:" << anNumFaces << "\n";
      aTotalFaces += anNumFaces;
    }
    if (theNbArgs > 1)
    {
      break;
    }
    anNumFaces = 0;
  }
  if (aTotalFaces == 0)
  {
    theDI << "No untrimmed faces\n";
  }
  else
  {
    theDI << "Total number :" << aTotalFaces << "\n";
  }
  return 0;
}


//=======================================================================
//function : XSDRAWIGES_TPSTAT
//purpose  :
//=======================================================================
static Standard_Integer XSDRAWIGES_TPSTAT(Draw_Interpretor& theDI,
                                          Standard_Integer theNbArgs,
                                          const char** theArgVec)
{
  Handle(XSControl_WorkSession) aWorkSession = XSDRAW::Session();
  const Standard_CString anArg1 = theArgVec[1];
  const Handle(Transfer_TransientProcess)& aTransientProcess = aWorkSession->TransferReader()->TransientProcess();
  IGESControl_Reader aReader;
  Handle(Interface_InterfaceModel) aModel = aTransientProcess->Model();
  if (aModel.IsNull()) { theDI << "No Transfer Read\n"; return -1; }
  Handle(XSControl_WorkSession) aSession = aReader.WS();
  aSession->SetMapReader(aTransientProcess);
  Standard_Integer aMode = 0;
  if (theNbArgs > 1)
  {
    char a2 = anArg1[1]; if (a2 == '\0') a2 = '!';
    switch (anArg1[0])
    {
      case 'g': aReader.PrintTransferInfo(IFSelect_FailAndWarn, IFSelect_GeneralInfo); break;
      case 'c': aReader.PrintTransferInfo(IFSelect_FailAndWarn, IFSelect_CountByItem); break;
      case 'C': aReader.PrintTransferInfo(IFSelect_FailAndWarn, IFSelect_ListByItem); break;
      case 'r': aReader.PrintTransferInfo(IFSelect_FailAndWarn, IFSelect_ResultCount); break;
      case 's': aReader.PrintTransferInfo(IFSelect_FailAndWarn, IFSelect_Mapping); break;
      case '?': aMode = -1; break;
      default: aMode = -2; break;
    }
  }
  if (aMode < -1) theDI << "Unknown Mode\n";
  if (aMode < 0)
  {
    theDI << "Modes available :\n"
      << "g : general    c : checks (count)  C (list)\n"
      << "r : number of CasCade resulting shapes\n"
      << "s : mapping between IGES entities and CasCade shapes\n";
    if (aMode < -1) return -1;
    return 0;
  }
  return 0;
}


//=======================================================================
//function : etest
//purpose  :
//=======================================================================
static Standard_Integer etest(Draw_Interpretor& theDI,
                              Standard_Integer theNbArgs,
                              const char** theArgVec)
{
  if (theNbArgs < 3)
  {
    theDI << "etest igesfile shape\n";
    return 0;
  }
  IGESControl_Reader aReader;
  aReader.ReadFile(theArgVec[1]);
  aReader.SetReadVisible(Standard_True);
  aReader.TransferRoots();
  TopoDS_Shape shape = aReader.OneShape();
  DBRep::Set(theArgVec[2], shape);
  return 0;
}

//=======================================================================
//function : ReadIges
//purpose  : Read IGES to DECAF document
//=======================================================================
static Standard_Integer ReadIges(Draw_Interpretor& theDI,
                                 Standard_Integer theNbArgs,
                                 const char** theArgVec)
{
  if (theNbArgs < 3)
  {
    theDI << "Use: " << theArgVec[0] << " Doc filename [mode]: read IGES file to a document\n";
    return 0;
  }

  DeclareAndCast(IGESControl_Controller, aController, XSDRAW::Controller());
  if (aController.IsNull()) XSDRAW::SetNorm("IGES");

  TCollection_AsciiString aFileName, aModelName;
  Standard_Boolean isModified = XSDRAW::FileAndVar(theArgVec[2], theArgVec[1], "IGES", aFileName, aModelName);
  if (isModified) theDI << " File IGES to read : " << aFileName.ToCString() << "\n";
  else theDI << " Model taken from the session : " << aModelName.ToCString() << "\n";

  IGESCAFControl_Reader aReader(XSDRAW::Session(), isModified);
  Standard_Integer onlyVisible = Interface_Static::IVal("read.iges.onlyvisible");
  aReader.SetReadVisible(onlyVisible == 1);

  if (theNbArgs == 4)
  {
    Standard_Boolean mode = Standard_True;
    for (Standard_Integer i = 0; theArgVec[3][i]; i++)
      switch (theArgVec[3][i])
      {
      case '-': mode = Standard_False; break;
      case '+': mode = Standard_True; break;
      case 'c': aReader.SetColorMode(mode); break;
      case 'n': aReader.SetNameMode(mode); break;
      case 'l': aReader.SetLayerMode(mode); break;
      }
  }
  Handle(Draw_ProgressIndicator) aProgress = new Draw_ProgressIndicator(theDI);
  Message_ProgressScope aRootScope(aProgress->Start(), "IGES import", isModified ? 2 : 1);

  IFSelect_ReturnStatus aReadStatus = IFSelect_RetVoid;
  if (isModified)
  {
    Message_ProgressScope aReadScope(aRootScope.Next(), "File reading", 1);
    aReadScope.Show();
    aReadStatus = aReader.ReadFile(aFileName.ToCString());
  }
  else if (XSDRAW::Session()->NbStartingEntities() > 0)
  {
    aReadStatus = IFSelect_RetDone;
  }
  if (aReadStatus != IFSelect_RetDone)
  {
    if (isModified)
    {
      theDI << "Could not read file " << aFileName.ToCString() << " , abandon\n";
    }
    else
    {
      theDI << "No model loaded\n";
    }
    return 1;
  }

  Handle(TDocStd_Document) aDocument;
  if (!DDocStd::GetDocument(theArgVec[1], aDocument, Standard_False))
  {
    Handle(TDocStd_Application) anApplication = DDocStd::GetApplication();
    anApplication->NewDocument("BinXCAF", aDocument);
    TDataStd_Name::Set(aDocument->GetData()->Root(), theArgVec[1]);
    Handle(DDocStd_DrawDocument) aDrawDocument = new DDocStd_DrawDocument(aDocument);
    Draw::Set(theArgVec[1], aDrawDocument);
  }
  if (!aReader.Transfer(aDocument, aRootScope.Next()))
  {
    theDI << "Cannot read any relevant data from the IGES file\n";
    return 1;
  }
  theDI << "Document saved with name " << theArgVec[1];

  return 0;
}

//=======================================================================
//function : WriteIges
//purpose  : Write DECAF document to IGES
//=======================================================================
static Standard_Integer WriteIges(Draw_Interpretor& theDI, Standard_Integer theNbArgs, const char** theArgVec)
{
  if (theNbArgs < 3)
  {
    theDI << "Use: " << theArgVec[0] << " Doc filename [mode]: write document to IGES file\n";
    return 0;
  }

  Handle(TDocStd_Document) aDocument;
  DDocStd::GetDocument(theArgVec[1], aDocument);
  if (aDocument.IsNull())
  {
    theDI << theArgVec[1] << " is not a document\n";
    return 1;
  }

  XSDRAW::SetNorm("IGES");

  TCollection_AsciiString aFileName, aModelName;
  const Standard_Boolean isModified = XSDRAW::FileAndVar(theArgVec[2], theArgVec[1], "IGES", aFileName, aModelName);

  Handle(Draw_ProgressIndicator) aProgress = new Draw_ProgressIndicator(theDI);
  Message_ProgressScope aRootScope(aProgress->Start(), "IGES export", isModified ? 2 : 1);

  IGESCAFControl_Writer aWriter(XSDRAW::Session(), Standard_True);
  if (theNbArgs == 4)
  {
    Standard_Boolean mode = Standard_True;
    for (Standard_Integer i = 0; theArgVec[3][i]; i++)
      switch (theArgVec[3][i])
      {
      case '-': mode = Standard_False; break;
      case '+': mode = Standard_True; break;
      case 'c': aWriter.SetColorMode(mode); break;
      case 'n': aWriter.SetNameMode(mode); break;
      case 'l': aWriter.SetLayerMode(mode); break;
      }
  }
  aWriter.Transfer(aDocument, aRootScope.Next());

  if (isModified)
  {
    Message_ProgressScope aWriteScope(aRootScope.Next(), "File writing", 1);
    aWriteScope.Show();
    theDI << "Writing IGES model to file " << theArgVec[2] << "\n";
    if (aWriter.Write(aFileName.ToCString()))
    {
      theDI << " Write OK\n";
    }
    else
    {
      theDI << " Write failed\n";
    }
  }
  else
  {
    theDI << "Document has been translated into the session";
  }
  return 0;
}

//=======================================================================
//function : Factory
//purpose  :
//=======================================================================
void XSDRAWIGES::Factory(Draw_Interpretor& theDI)
{
  static Standard_Boolean aIsActivated = Standard_False;
  if (aIsActivated)
  {
    return;
  }
  aIsActivated = Standard_True;

  IGESControl_Controller::Init();

  const char* aGroup = "DE: IGES";

  theDI.Add("tplosttrim", "number of untrimmed faces during last transfer", __FILE__, XSDRAWIGES_tplosttrim, aGroup);
  theDI.Add("igesbrep", "igesbrep [file else already loaded model] [name DRAW]", __FILE__, igesbrep, aGroup);
  theDI.Add("testreadiges", "testreadiges [file else already loaded model] [name DRAW]", __FILE__, testread, aGroup);
  theDI.Add("igesparam", "igesparam ->list, + name ->one param, + name val->change", __FILE__, igesparam, aGroup);
  theDI.Add("TPSTAT", " ", __FILE__, XSDRAWIGES_TPSTAT, aGroup);
  theDI.Add("etest", "test of eviewer", __FILE__, etest, aGroup);

  theDI.Add("ReadIges", "Doc filename: Read IGES file to DECAF document", __FILE__, ReadIges, aGroup);
  theDI.Add("WriteIges", "Doc filename: Write DECAF document to IGES file", __FILE__, WriteIges, aGroup);
  theDI.Add("igesread", "igesread [file else already loaded model] [name DRAW]", __FILE__, igesbrep, aGroup);
  theDI.Add("igeswrite", "igesread [file else already loaded model] [name DRAW]", __FILE__, brepiges, aGroup);
  theDI.Add("brepiges", "brepiges sh1 [+sh2 [+sh3 ..]] filename.igs", __FILE__, brepiges, aGroup);
  theDI.Add("testwriteiges", "testwriteiges filename.igs shape", __FILE__, testwrite, aGroup);

  // Load XSDRAW session for pilot activation
  XSDRAW::LoadDraw(theDI);
}

// Declare entry point PLUGINFACTORY
DPLUGIN(XSDRAWIGES)
