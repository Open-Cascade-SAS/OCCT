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

#include <XSDRAWSTEP.hxx>

#include <DBRep.hxx>
#include <DDF.hxx>
#include <DDocStd.hxx>
#include <DDocStd_DrawDocument.hxx>
#include <Draw.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_PluginMacro.hxx>
#include <Draw_ProgressIndicator.hxx>
#include <Interface_Macros.hxx>
#include <Interface_Static.hxx>
#include <Message.hxx>
#include <OSD_OpenFile.hxx>
#include <OSD_Parallel.hxx>
#include <OSD_Path.hxx>
#include <STEPCAFControl_Reader.hxx>
#include <STEPCAFControl_Writer.hxx>
#include <STEPControl_ActorWrite.hxx>
#include <STEPControl_Controller.hxx>
#include <STEPControl_Reader.hxx>
#include <STEPControl_StepModelType.hxx>
#include <STEPControl_Writer.hxx>
#include <StepData_StepModel.hxx>
#include <StepGeom_Axis2Placement3d.hxx>
#include <StepSelect_Activator.hxx>
#include <STEPSelections_AssemblyExplorer.hxx>
#include <STEPSelections_Counter.hxx>
#include <StepToTopoDS_MakeTransformed.hxx>
#include <TDataStd_Name.hxx>
#include <TDocStd_Application.hxx>
#include <TopoDS_Shape.hxx>
#include <UnitsMethods.hxx>
#include <XSAlgo.hxx>
#include <XSAlgo_ShapeProcessor.hxx>
#include <XSControl_WorkSession.hxx>
#include <XSDRAW.hxx>

namespace
{
using ExternalFileMap =
  NCollection_DataMap<TCollection_AsciiString, Handle(STEPCAFControl_ExternFile)>;
}

//=================================================================================================

static Standard_Integer stepread(Draw_Interpretor& theDI,
                                 Standard_Integer  theNbArgs,
                                 const char**      theArgVec)
{
  if (theNbArgs < 3)
  {
    theDI << "Use: stepread  [file] [f or r (type of model full or reduced)]\n";
    return 1;
  }

  // Progress indicator
  Handle(Draw_ProgressIndicator) progress = new Draw_ProgressIndicator(theDI, 1);
  Message_ProgressScope          aPSRoot(progress->Start(), "Reading", 100);

  STEPControl_Reader      sr(XSDRAW::Session(), Standard_False);
  TCollection_AsciiString fnom, rnom;
  Standard_Boolean modfic = XSDRAW::FileAndVar(theArgVec[1], theArgVec[2], "STEP", fnom, rnom);
  if (modfic)
    theDI << " File STEP to read : " << fnom.ToCString() << "\n";
  else
    theDI << " Model taken from the session : " << fnom.ToCString() << "\n";
  theDI << " -- Names of variables BREP-DRAW prefixed by : " << rnom.ToCString() << "\n";
  IFSelect_ReturnStatus readstat = IFSelect_RetVoid;

  aPSRoot.SetName("Loading");
  progress->Show(aPSRoot);

  Standard_Boolean fromtcl   = Standard_False;
  Standard_Boolean aFullMode = Standard_False;
  Standard_Integer k         = 3;
  if (theNbArgs > k)
  {
    if (theArgVec[k][0] == 'f' || theArgVec[3][0] == 'F')
    {
      aFullMode = Standard_True;
      k++;
    }
    else if (theArgVec[k][0] == 'r' || theArgVec[3][0] == 'R')
    {
      aFullMode = Standard_False;
      k++;
    }
    else
      fromtcl = Standard_True;
  }
  if (!fromtcl)
    fromtcl = theNbArgs > k;
  if (aFullMode)
    std::cout << "Full model for translation with additional info will be used \n" << std::flush;
  else
    std::cout << "Reduced model for translation without additional info will be used \n"
              << std::flush;

  sr.WS()->SetModeStat(aFullMode);

  if (modfic)
    readstat = sr.ReadFile(fnom.ToCString());
  else if (XSDRAW::Session()->NbStartingEntities() > 0)
    readstat = IFSelect_RetDone;

  aPSRoot.Next(20); // On average loading takes 20%
  if (aPSRoot.UserBreak())
    return 1;

  if (readstat != IFSelect_RetDone)
  {
    if (modfic)
      theDI << "Could not read file " << fnom.ToCString() << " , abandon\n";
    else
      theDI << "No model loaded\n";
    return 1;
  }

  sr.SetSystemLengthUnit(XSDRAW::GetLengthUnit());
  XSAlgo_ShapeProcessor::ProcessingData aProcessingData =
    XSAlgo_ShapeProcessor::ReadProcessingData("read.step.resource.name", "read.step.sequence");
  sr.SetShapeFixParameters(std::move(aProcessingData.first));
  sr.SetShapeProcessFlags(aProcessingData.second);

  //   nom = "." -> fichier deja lu
  Standard_Integer i, num, nbs, modepri = 1;
  if (fromtcl)
    modepri = 4;
  while (modepri)
  {
    num = sr.NbRootsForTransfer();
    if (!fromtcl)
    {
      theDI << "NbRootsForTransfer=" << num << " :\n";
      for (i = 1; i <= num; i++)
      {
        theDI << "Root." << i << ", Ent. ";
        Standard_SStream aTmpStream;
        sr.Model()->Print(sr.RootForTransfer(i), aTmpStream);
        theDI << aTmpStream.str().c_str();
        theDI << " Type:" << sr.RootForTransfer(i)->DynamicType()->Name() << "\n";
      }

      std::cout << "Mode (0 End, 1 root n0 1, 2 one root/n0, 3 one entity/n0, 4 Selection) : "
                << std::flush;
      std::cin >> modepri;
    }

    if (modepri == 0)
    {
      theDI << "End Reading STEP\n";
      return 0;
    }
    if (modepri <= 2)
    {
      num = 1;
      if (modepri == 2)
      {
        std::cout << "Root N0 : " << std::flush;
        std::cin >> num;
      }
      aPSRoot.SetName("Translation");
      progress->Show(aPSRoot);

      if (!sr.TransferRoot(num, aPSRoot.Next(80)))
        theDI << "Transfer root n0 " << num << " : no result\n";
      else
      {
        nbs = sr.NbShapes();
        char shname[30];
        Sprintf(shname, "%s_%d", rnom.ToCString(), nbs);
        theDI << "Transfer root n0 " << num << " OK  -> DRAW Shape: " << shname << "\n";
        theDI << "Now, " << nbs << " Shapes produced\n";
        TopoDS_Shape sh = sr.Shape(nbs);
        DBRep::Set(shname, sh);
      }
      if (aPSRoot.UserBreak())
        return 1;
    }
    else if (modepri == 3)
    {
      std::cout << "Entity : " << std::flush;
      num = XSDRAW::GetEntityNumber("");
      if (!sr.TransferOne(num))
        theDI << "Transfer entity n0 " << num << " : no result\n";
      else
      {
        nbs = sr.NbShapes();
        char shname[30];
        Sprintf(shname, "%s_%d", rnom.ToCString(), num);
        theDI << "Transfer entity n0 " << num << " OK  -> DRAW Shape: " << shname << "\n";
        theDI << "Now, " << nbs << " Shapes produced\n";
        TopoDS_Shape sh = sr.Shape(nbs);
        DBRep::Set(shname, sh);
      }
    }
    else if (modepri == 4)
    {
      //      char snm[100];  Standard_Integer answer = 1;
      Handle(TColStd_HSequenceOfTransient) list;

      //  Selection, nommee ou via tcl. tcl : raccourcis admis
      //   * donne xst-transferrable-roots
      if (fromtcl)
      {
        modepri = 0; // d ioffice une seule passe
        if (theArgVec[k][0] == '*' && theArgVec[k][1] == '\0')
        {
          theDI << "Transferrable Roots : ";
          list = XSDRAW::Session()->GiveList("xst-transferrable-roots");
          // list = new TColStd_HSequenceOfTransient;
          // for(Standard_Integer j=1; j<=num; j++)
          //   list->Append(sr.RootForTransfer(j));
        }
        else
        {
          theDI << "List given by " << theArgVec[k];
          if (theNbArgs > k + 1)
            theDI << " " << theArgVec[k + 1];
          theDI << " : ";
          list =
            XSDRAW::Session()->GiveList(theArgVec[k], (theNbArgs > (k + 1) ? theArgVec[k + 1] : 0));
        }
        if (list.IsNull())
        {
          theDI << "No list defined. Give a selection name or * for all transferrable roots\n";
          continue;
        }
      }
      else
      {
        std::cout << "Name of Selection :" << std::flush;
        list = XSDRAW::Session()->GiveList("");
        if (list.IsNull())
        {
          theDI << "No list defined\n";
          continue;
        }
      }

      Standard_Integer ill, nbl = list->Length();
      theDI << "Nb entities selected : " << nbl << "\n";
      if (nbl == 0)
        continue;

      aPSRoot.SetName("Translation");
      progress->Show(aPSRoot);

      Message_ProgressScope aPS(aPSRoot.Next(80), "Root", nbl);
      for (ill = 1; ill <= nbl && aPS.More(); ill++)
      {
        num = sr.Model()->Number(list->Value(ill));
        if (num == 0)
          continue;
        if (!sr.TransferOne(num, aPS.Next()))
          theDI << "Transfer entity n0 " << num << " : no result\n";
        else
        {
          nbs = sr.NbShapes();
          char shname[30];
          Sprintf(shname, "%s_%d", rnom.ToCString(), nbs);
          theDI << "Transfer entity n0 " << num << " OK  -> DRAW Shape: " << shname << "\n";
          theDI << "Now, " << nbs << " Shapes produced\n";
          TopoDS_Shape sh = sr.Shape(nbs);
          DBRep::Set(shname, sh);
        }
      }
      if (aPSRoot.UserBreak())
        return 1;
    }
    else
      theDI << "Unknown mode n0 " << modepri << "\n";
  }
  return 0;
}

//=================================================================================================

static Standard_Integer testreadstep(Draw_Interpretor& theDI,
                                     Standard_Integer  theNbArgs,
                                     const char**      theArgVec)
{
  if (theNbArgs < 3)
  {
    theDI << "ERROR in " << theArgVec[0] << "Wrong Number of Arguments.\n";
    theDI << " Usage : " << theArgVec[0] << " file_name shape_name [-stream]\n";
    theDI << " Option -stream forces usage of API accepting stream\n";
    return 1;
  }

  Standard_Boolean useStream = (theNbArgs > 3 && !strcasecmp(theArgVec[theNbArgs - 1], "-stream"));
  Standard_CString aShName   = useStream ? theArgVec[theNbArgs - 2] : theArgVec[theNbArgs - 1];
  Standard_Integer aSize     = useStream ? (theNbArgs - 3) : (theNbArgs - 2);
  NCollection_Array1<TopoDS_Shape>                           aShapes(0, aSize);
  NCollection_Array1<TCollection_AsciiString>                aFileNames(0, aSize);
  NCollection_DataMap<TCollection_AsciiString, TopoDS_Shape> aShapesMap;
  for (int anInd = 1; anInd <= aSize; ++anInd)
  {
    aFileNames[anInd - 1] = theArgVec[anInd];
  }
  STEPControl_Controller::Init();
  XSAlgo_ShapeProcessor::PrepareForTransfer(); // update unit info
  IFSelect_ReturnStatus aReadStat;
  DESTEP_Parameters     aParameters;
  aParameters.InitFromStatic();
  int aNbSubShape = 0;
  OSD_Parallel::For(0, aSize, [&](const Standard_Integer theIndex) {
    STEPControl_Reader                    aReader;
    XSAlgo_ShapeProcessor::ProcessingData aProcessingData =
      XSAlgo_ShapeProcessor::ReadProcessingData("read.step.resource.name", "read.step.sequence");
    aReader.SetShapeFixParameters(std::move(aProcessingData.first));
    aReader.SetShapeProcessFlags(aProcessingData.second);
    aReader.SetSystemLengthUnit(UnitsMethods::GetCasCadeLengthUnit());
    if (useStream)
    {
      std::ifstream aStream;
      OSD_OpenStream(aStream, aFileNames[theIndex].ToCString(), std::ios::in | std::ios::binary);
      TCollection_AsciiString aFolder, aFileNameShort;
      OSD_Path::FolderAndFileFromPath(aFileNames[theIndex].ToCString(), aFolder, aFileNameShort);
      aReadStat = aReader.ReadStream(aFileNameShort.ToCString(), aParameters, aStream);
    }
    else
    {
      aReadStat = aReader.ReadFile(aFileNames[theIndex].ToCString(), aParameters);
    }
    if (aReadStat == IFSelect_RetDone)
    {
      aReader.TransferRoots();
      aShapes[theIndex] = aReader.OneShape();
      TCollection_AsciiString aName(aShName);
      if (aSize > 1)
      {
        aName += theIndex;
      }
      aShapesMap.Bind(aName, aShapes[theIndex]);
      aNbSubShape += aReader.NbShapes();
    }
    else
    {
      theDI << "Error: Problem with reading shape by file: " << "[" << aFileNames[theIndex] << "]";
    }
  });
  NCollection_DataMap<TCollection_AsciiString, TopoDS_Shape>::Iterator anIt(aShapesMap);
  for (; anIt.More(); anIt.Next())
  {
    DBRep::Set(anIt.Key().ToCString(), anIt.Value());
  }
  theDI << "Count of shapes produced : " << aNbSubShape << "\n";
  return 0;
}

//=================================================================================================

static Standard_Integer steptrans(Draw_Interpretor& theDI,
                                  Standard_Integer  theNbArgs,
                                  const char**      theArgVec)
{
  if (theNbArgs < 5)
  {
    theDI << "give shape-name new-shape + entity-n0 entity-n0: AXIS2\n";
    return 1;
  }
  Handle(XSControl_WorkSession) aWS   = XSDRAW::Session();
  TopoDS_Shape                  shape = DBRep::Get(theArgVec[1]);
  if (shape.IsNull())
  {
    theDI << "Not a shape : " << theArgVec[1] << "\n";
    return 1;
  }
  Handle(StepGeom_Axis2Placement3d) ax1, ax2;
  Standard_Integer                  n1 = 0, n2 = 0;
  n1 = XSDRAW::GetEntityNumber(theArgVec[3]);
  if (theNbArgs > 4)
    n2 = XSDRAW::GetEntityNumber(theArgVec[4]);
  if (n1 > 0)
    ax1 = Handle(StepGeom_Axis2Placement3d)::DownCast(aWS->StartingEntity(n1));
  if (n2 > 0)
    ax2 = Handle(StepGeom_Axis2Placement3d)::DownCast(aWS->StartingEntity(n2));
  StepData_Factors             aFactors;
  StepToTopoDS_MakeTransformed mktrans;
  if (mktrans.Compute(ax1, ax2, aFactors))
  {
    TopLoc_Location loc(mktrans.Transformation());
    shape.Move(loc);
    DBRep::Set(theArgVec[2], shape);
    theDI << "Transformed Shape as " << theArgVec[2] << "\n";
  }
  else
    theDI << "No transformation computed\n";
  return 0;
}

//=================================================================================================

static Standard_Integer stepwrite(Draw_Interpretor& theDI,
                                  Standard_Integer  theNbArgs,
                                  const char**      theArgVec)
{
  Handle(XSControl_WorkSession)  aWS = XSDRAW::Session();
  Handle(STEPControl_Controller) aCtl =
    Handle(STEPControl_Controller)::DownCast(aWS->NormAdaptor());
  if (aCtl.IsNull())
  {
    aWS->SelectNorm("STEP");
  }
  if (theNbArgs < 3)
  {
    theDI << "Error: Give mode[1-4] and Shape name + optional file. Mode possible\n";
    theDI << "f ou 1 : FacettedBRep        s ou 2 : ShellBasedSurfaceModel\n"
          << "m ou 3 : ManifoldSolidBrep   w ou 4 : GeometricCurveSet/WireFrame\n";
    return 1;
  }
  STEPControl_StepModelType mode;
  switch (theArgVec[1][0])
  {
    case 'a':
    case '0':
      mode = STEPControl_AsIs;
      break;
    case 'f':
    case '1':
      mode = STEPControl_FacetedBrep;
      break;
    case 's':
    case '2':
      mode = STEPControl_ShellBasedSurfaceModel;
      break;
    case 'm':
    case '3':
      mode = STEPControl_ManifoldSolidBrep;
      break;
    case 'w':
    case '4':
      mode = STEPControl_GeometricCurveSet;
      break;
    default:
      theDI << "Error: 1st arg = mode, incorrect [give fsmw]\n";
      return 1;
  }
  Handle(STEPControl_ActorWrite) ActWrite =
    Handle(STEPControl_ActorWrite)::DownCast(aWS->NormAdaptor()->ActorWrite());
  if (!ActWrite.IsNull())
    ActWrite->SetGroupMode(Interface_Static::IVal("write.step.assembly"));

  TopoDS_Shape                     shape = DBRep::Get(theArgVec[2]);
  STEPControl_Writer               sw(aWS, Standard_False);
  Handle(Interface_InterfaceModel) stepmodel = sw.Model();
  Standard_Integer                 nbavant   = (stepmodel.IsNull() ? 0 : stepmodel->NbEntities());

  Handle(Draw_ProgressIndicator) progress = new Draw_ProgressIndicator(theDI, 1);
  Message_ProgressScope          aPSRoot(progress->Start(), "Translating", 100);
  progress->Show(aPSRoot);

  XSAlgo_ShapeProcessor::ProcessingData aProcessingData =
    XSAlgo_ShapeProcessor::ReadProcessingData("write.step.resource.name", "write.step.sequence");
  sw.SetShapeFixParameters(std::move(aProcessingData.first));
  sw.SetShapeProcessFlags(aProcessingData.second);
  Standard_Integer stat = sw.Transfer(shape, mode, Standard_True, aPSRoot.Next(90));
  if (stat == IFSelect_RetDone)
  {
    theDI << "Translation: OK\n";
  }
  else
  {
    theDI << "Error: translation failed, status = " << stat << "\n";
  }
  if (aPSRoot.UserBreak())
    return 1;
  aPSRoot.SetName("Writing");
  progress->Show(aPSRoot);

  //   Que s est-il passe
  stepmodel                = sw.Model();
  Standard_Integer nbapres = (stepmodel.IsNull() ? 0 : stepmodel->NbEntities());
  if (nbavant > 0)
    theDI << "Beware : Model not empty before transferring\n";
  if (nbapres <= nbavant)
    theDI << "Beware : No data produced by this transfer\n";
  if (nbapres == 0)
  {
    theDI << "No data to write\n";
    return 0;
  }

  if (theNbArgs <= 3)
  {
    theDI << " Now, to write a file, command : writeall filename\n";
    return 0;
  }
  const char* nomfic = theArgVec[3];
  stat               = sw.Write(nomfic);
  switch (stat)
  {
    case IFSelect_RetVoid:
      theDI << "Error: No file written\n";
      return 1;
    case IFSelect_RetDone:
      theDI << "File " << nomfic << " written\n";
      break;
    case IFSelect_RetStop:
      theDI << "Error on writing file: no space on disk or destination is write protected\n";
      return 1;
    default:
      theDI << "Error: File " << nomfic << " written with fail messages\n";
      break;
  }
  XSDRAW::CollectActiveWorkSessions(aWS, nomfic, XSDRAW::WorkSessionList());
  return 0;
}

//=================================================================================================

static Standard_Integer testwrite(Draw_Interpretor& theDI,
                                  Standard_Integer  theNbArgs,
                                  const char**      theArgVec)
{
  if (theNbArgs < 3)
  {
    theDI << "ERROR in " << theArgVec[0] << "Wrong Number of Arguments.\n";
    theDI << " Usage : " << theArgVec[0] << " file_name shape_name [-stream]\n";
    theDI << " Option -stream forces usage of API accepting stream\n";
    return 1;
  }

  Standard_Boolean useStream = (theNbArgs > 3 && !strcasecmp(theArgVec[theNbArgs - 1], "-stream"));
  Standard_CString aShName   = useStream ? theArgVec[theNbArgs - 2] : theArgVec[theNbArgs - 1];
  Standard_Integer aSize     = useStream ? (theNbArgs - 3) : (theNbArgs - 2);
  NCollection_Array1<TCollection_AsciiString>                aFileNames(0, aSize);
  NCollection_DataMap<TCollection_AsciiString, TopoDS_Shape> aShapesMap;
  for (int anInd = 1; anInd <= aSize; ++anInd)
  {
    aFileNames[anInd - 1] = theArgVec[anInd];
  }
  TopoDS_Shape aShape = DBRep::Get(aShName);
  if (aShape.IsNull())
  {
    theDI << "Syntax error: wrong number of arguments";
    return 1;
  }

  DESTEP_Parameters aParameters;
  aParameters.InitFromStatic();

  OSD_Parallel::For(0, aSize, [&](const Standard_Integer theIndex) {
    STEPControl_Writer                    aWriter;
    XSAlgo_ShapeProcessor::ProcessingData aProcessingData =
      XSAlgo_ShapeProcessor::ReadProcessingData("write.step.resource.name", "write.step.sequence");
    aWriter.SetShapeFixParameters(std::move(aProcessingData.first));
    aWriter.SetShapeProcessFlags(aProcessingData.second);

    if (aWriter.Transfer(aShape, STEPControl_AsIs, aParameters) != IFSelect_RetDone)
    {
      theDI << "Error: Can't transfer input shape";
      return;
    }
    IFSelect_ReturnStatus aStat = IFSelect_RetDone;
    if (useStream)
    {
      std::ofstream aStream;
      OSD_OpenStream(aStream, aFileNames[theIndex], std::ios::out | std::ios::binary);
      if (!aStream.good())
      {
        theDI << "Error: Problem with opening stream by file: " << "[" << aFileNames[theIndex]
              << "]";
        return;
      }
      aStat = aWriter.WriteStream(aStream);
    }
    else
    {
      aStat = aWriter.Write(aFileNames[theIndex].ToCString());
    }
    if (aStat != IFSelect_RetDone)
    {
      theDI << "Error on writing file: " << "[" << aFileNames[theIndex] << "]";
      return;
    }
  });
  theDI << "File(s) are Written";
  return 0;
}

//=================================================================================================

static Standard_Integer countexpected(Draw_Interpretor& theDI,
                                      Standard_Integer /*theNbArgs*/,
                                      const char** /*theArgVec*/)
{
  Handle(XSControl_WorkSession) WS    = XSDRAW::Session();
  const Interface_Graph&        graph = WS->Graph();

  Handle(TColStd_HSequenceOfTransient) roots = WS->GiveList("xst-transferrable-roots", "");
  STEPSelections_Counter               cnt;

  for (Standard_Integer i = 1; i <= roots->Length(); i++)
  {
    cnt.Count(graph, roots->Value(i));
  }

  theDI << "Instances of Faces \t: " << cnt.NbInstancesOfFaces() << "\n";
  theDI << "Instances of Shells\t: " << cnt.NbInstancesOfShells() << "\n";
  theDI << "Instances of Solids\t: " << cnt.NbInstancesOfSolids() << "\n";
  theDI << "Instances of Wires in GS\t: " << cnt.NbInstancesOfWires() << "\n";
  theDI << "Instances of Edges in GS\t: " << cnt.NbInstancesOfEdges() << "\n";

  theDI << "Source Faces \t: " << cnt.NbSourceFaces() << "\n";
  theDI << "Source Shells\t: " << cnt.NbSourceShells() << "\n";
  theDI << "Source Solids\t: " << cnt.NbSourceSolids() << "\n";
  theDI << "Source Wires in GS\t: " << cnt.NbSourceWires() << "\n";
  theDI << "Source Edges in GS\t: " << cnt.NbSourceEdges() << "\n";

  return 1;
}

//=================================================================================================

static Standard_Integer dumpassembly(Draw_Interpretor& /*theDI*/,
                                     Standard_Integer /*theNbArgs*/,
                                     const char** /*theArgVec*/)
{
  Handle(XSControl_WorkSession) WS    = XSDRAW::Session();
  const Interface_Graph&        graph = WS->Graph();

  STEPSelections_AssemblyExplorer exp(graph);
  exp.Dump(std::cout);
  return 0;
}

//=================================================================================================

static Standard_Integer stepfileunits(Draw_Interpretor& theDI,
                                      Standard_Integer  theNbArgs,
                                      const char**      theArgVec)
{
  if (theNbArgs < 2)
  {
    theDI << "Error: Invalid number of parameters. Should be: getfileunits name_file\n";
    return 1;
  }
  STEPControl_Reader    aStepReader;
  IFSelect_ReturnStatus readstat = IFSelect_RetVoid;
  readstat                       = aStepReader.ReadFile(theArgVec[1]);
  if (readstat != IFSelect_RetDone)
  {
    theDI << "No model loaded\n";
    return 1;
  }

  TColStd_SequenceOfAsciiString anUnitLengthNames;
  TColStd_SequenceOfAsciiString anUnitAngleNames;
  TColStd_SequenceOfAsciiString anUnitSolidAngleNames;
  aStepReader.FileUnits(anUnitLengthNames, anUnitAngleNames, anUnitSolidAngleNames);

  Standard_Integer i = 1;
  theDI << "=====================================================\n";
  theDI << "LENGTH Unit\n";
  for (; i <= anUnitLengthNames.Length(); i++)
    theDI << anUnitLengthNames(i).ToCString() << "\n";

  theDI << "=====================================================\n";
  theDI << "Angle Unit\n";
  for (i = 1; i <= anUnitAngleNames.Length(); i++)
    theDI << anUnitAngleNames(i).ToCString() << "\n";

  theDI << "=====================================================\n";
  theDI << "Solid Angle Unit\n";
  for (i = 1; i <= anUnitSolidAngleNames.Length(); i++)
    theDI << anUnitSolidAngleNames(i).ToCString() << "\n";

  return 0;
}

//=======================================================================
// function : ReadStep
// purpose  : Read STEP file to DECAF document
//=======================================================================
static Standard_Integer ReadStep(Draw_Interpretor& theDI,
                                 Standard_Integer  theNbArgs,
                                 const char**      theArgVec)
{
  DeclareAndCast(STEPControl_Controller, aController, XSDRAW::Controller());
  if (aController.IsNull())
  {
    XSDRAW::SetNorm("STEP");
  }

  Standard_CString        aDocumentName = NULL;
  TCollection_AsciiString aFilePath, aModeStr;
  bool                    toTestStream = false;

  for (Standard_Integer anArgIter = 1; anArgIter < theNbArgs; ++anArgIter)
  {
    TCollection_AsciiString anArgCase(theArgVec[anArgIter]);
    anArgCase.LowerCase();

    if (anArgCase == "-stream")
    {
      toTestStream = true;
    }
    else if (aDocumentName == NULL)
    {
      aDocumentName = theArgVec[anArgIter];
    }
    else if (aFilePath.IsEmpty())
    {
      aFilePath = theArgVec[anArgIter];
    }
    else if (aModeStr.IsEmpty())
    {
      aModeStr = theArgVec[anArgIter];
    }
    else
    {
      Message::SendFail() << "Syntax error at '" << theArgVec[anArgIter] << "'";
      return 1;
    }
  }

  TCollection_AsciiString aFileName, anOldVarName;
  Standard_Boolean        isFileMode =
    XSDRAW::FileAndVar(aFilePath.ToCString(), aDocumentName, "STEP", aFileName, anOldVarName);

  if (isFileMode)
  {
    theDI << " File STEP to read : " << aFileName << "\n";
  }
  else
  {
    theDI << " Model taken from the session : " << aFileName << "\n";
  }

  STEPCAFControl_Reader aReader(XSDRAW::Session(), isFileMode);
  if (!aModeStr.IsEmpty())
  {
    Standard_Boolean aMode = Standard_True;

    for (Standard_Integer i = 1; i <= aModeStr.Length(); ++i)
    {
      switch (aModeStr.Value(i))
      {
        case '-':
          aMode = Standard_False;
          break;
        case '+':
          aMode = Standard_True;
          break;
        case 'c':
          aReader.SetColorMode(aMode);
          break;
        case 'n':
          aReader.SetNameMode(aMode);
          break;
        case 'l':
          aReader.SetLayerMode(aMode);
          break;
        case 'v':
          aReader.SetPropsMode(aMode);
          break;
        case 'm':
          aReader.SetMetaMode(aMode);
          break;
        default:
          Message::SendFail() << "Syntax error at '" << aModeStr << "'\n";
          return 1;
      }
    }
  }

  Handle(Draw_ProgressIndicator) aProgress = new Draw_ProgressIndicator(theDI);
  Message_ProgressScope          aRootScope(aProgress->Start(), "STEP import", isFileMode ? 2 : 1);

  IFSelect_ReturnStatus aReadStat = IFSelect_RetVoid;

  if (isFileMode)
  {
    Message_ProgressScope aReadScope(aRootScope.Next(), "File reading", 1);
    aReadScope.Show();

    if (toTestStream)
    {
      std::ifstream aStream;
      OSD_OpenStream(aStream, aFileName.ToCString(), std::ios::in | std::ios::binary);
      TCollection_AsciiString aFolder, aFileNameShort;
      OSD_Path::FolderAndFileFromPath(aFileName, aFolder, aFileNameShort);
      aReadStat = aReader.ReadStream(aFileNameShort.ToCString(), aStream);
    }
    else
    {
      aReadStat = aReader.ReadFile(aFileName.ToCString());
    }
  }
  else if (XSDRAW::Session()->NbStartingEntities() > 0)
  {
    aReadStat = IFSelect_RetDone;
  }

  if (aReadStat != IFSelect_RetDone)
  {
    if (isFileMode)
    {
      theDI << "Could not read file " << aFileName << " , abandon\n";
    }
    else
    {
      theDI << "No model loaded\n";
    }
    return 1;
  }

  Handle(TDocStd_Document) aDocument;

  if (!DDocStd::GetDocument(aDocumentName, aDocument, Standard_False))
  {
    Handle(TDocStd_Application) anApp = DDocStd::GetApplication();
    anApp->NewDocument("BinXCAF", aDocument);
    TDataStd_Name::Set(aDocument->GetData()->Root(), aDocumentName);
    Handle(DDocStd_DrawDocument) aDrawDoc = new DDocStd_DrawDocument(aDocument);
    Draw::Set(aDocumentName, aDrawDoc);
  }

  XSAlgo_ShapeProcessor::ProcessingData aProcessingData =
    XSAlgo_ShapeProcessor::ReadProcessingData("read.step.resource.name", "read.step.sequence");
  aReader.SetShapeFixParameters(std::move(aProcessingData.first));
  aReader.SetShapeProcessFlags(aProcessingData.second);

  if (!aReader.Transfer(aDocument, aRootScope.Next()))
  {
    theDI << "Cannot read any relevant data from the STEP file\n";
    return 1;
  }

  Handle(DDocStd_DrawDocument) aDrawDoc = new DDocStd_DrawDocument(aDocument);
  Draw::Set(aDocumentName, aDrawDoc);

  theDI << "Document saved with name " << aDocumentName;

  XSDRAW::CollectActiveWorkSessions(aFilePath);
  for (ExternalFileMap::Iterator anIter(aReader.ExternFiles()); anIter.More(); anIter.Next())
  {
    XSDRAW::CollectActiveWorkSessions(anIter.Value()->GetWS(),
                                      anIter.Key(),
                                      XSDRAW::WorkSessionList());
  }

  return 0;
}

//=======================================================================
// function : WriteStep
// purpose  : Write DECAF document to STEP
//=======================================================================
static Standard_Integer WriteStep(Draw_Interpretor& theDI,
                                  Standard_Integer  theNbArgs,
                                  const char**      theArgVec)
{
  DeclareAndCast(STEPControl_Controller, aController, XSDRAW::Controller());
  if (aController.IsNull())
  {
    XSDRAW::SetNorm("STEP");
  }

  STEPCAFControl_Writer aWriter(XSDRAW::Session(), Standard_True);

  Handle(TDocStd_Document)  aDocument;
  TCollection_AsciiString   aDocumentName, aFilePath;
  STEPControl_StepModelType aMode      = STEPControl_AsIs;
  bool                      hasModeArg = false, toTestStream = false;
  TCollection_AsciiString   aMultiFilePrefix, aLabelName;
  TDF_Label                 aLabel;

  for (Standard_Integer anArgIter = 1; anArgIter < theNbArgs; ++anArgIter)
  {
    TCollection_AsciiString anArgCase(theArgVec[anArgIter]);
    anArgCase.LowerCase();

    if (anArgCase == "-stream")
    {
      toTestStream = true;
    }
    else if (aDocumentName.IsEmpty())
    {
      Standard_CString aDocNameStr = theArgVec[anArgIter];
      DDocStd::GetDocument(aDocNameStr, aDocument);

      if (aDocument.IsNull())
      {
        theDI << "Syntax error: '" << theArgVec[anArgIter] << "' is not a document";
        return 1;
      }

      aDocumentName = aDocNameStr;
    }
    else if (aFilePath.IsEmpty())
    {
      aFilePath = theArgVec[anArgIter];
    }
    else if (!hasModeArg)
    {
      hasModeArg = true;

      switch (anArgCase.Value(1))
      {
        case 'a':
        case '0':
          aMode = STEPControl_AsIs;
          break;
        case 'f':
        case '1':
          aMode = STEPControl_FacetedBrep;
          break;
        case 's':
        case '2':
          aMode = STEPControl_ShellBasedSurfaceModel;
          break;
        case 'm':
        case '3':
          aMode = STEPControl_ManifoldSolidBrep;
          break;
        case 'w':
        case '4':
          aMode = STEPControl_GeometricCurveSet;
          break;
        default: {
          theDI << "Syntax error: mode '" << theArgVec[anArgIter] << "' is incorrect [give fsmw]";
          return 1;
        }
      }

      Standard_Boolean wrmode = Standard_True;

      for (Standard_Integer i = 1; i <= anArgCase.Length(); ++i)
      {
        switch (anArgCase.Value(i))
        {
          case '-':
            wrmode = Standard_False;
            break;
          case '+':
            wrmode = Standard_True;
            break;
          case 'c':
            aWriter.SetColorMode(wrmode);
            break;
          case 'n':
            aWriter.SetNameMode(wrmode);
            break;
          case 'l':
            aWriter.SetLayerMode(wrmode);
            break;
          case 'v':
            aWriter.SetPropsMode(wrmode);
            break;
        }
      }
    }
    else if (aMultiFilePrefix.IsEmpty() && anArgCase.Search(":") == -1)
    {
      aMultiFilePrefix = theArgVec[anArgIter];
    }
    else if (aLabel.IsNull())
    {
      if (!DDF::FindLabel(aDocument->Main().Data(), theArgVec[anArgIter], aLabel)
          || aLabel.IsNull())
      {
        theDI << "Syntax error: No label for entry '" << theArgVec[anArgIter] << "'";
        return 1;
      }

      aLabelName = theArgVec[anArgIter];
    }
    else
    {
      theDI << "Syntax error: unknown argument '" << theArgVec[anArgIter] << "'";
      return 1;
    }
  }

  if (aFilePath.IsEmpty())
  {
    theDI << "Syntax error: wrong number of arguments";
    return 1;
  }

  TCollection_AsciiString aFileName, anOldVarName;
  const Standard_Boolean  isFileMode = XSDRAW::FileAndVar(aFilePath.ToCString(),
                                                         aDocumentName.ToCString(),
                                                         "STEP",
                                                         aFileName,
                                                         anOldVarName);

  Handle(Draw_ProgressIndicator) aProgress = new Draw_ProgressIndicator(theDI);
  Message_ProgressScope          aRootScope(aProgress->Start(), "STEP export", isFileMode ? 2 : 1);

  XSAlgo_ShapeProcessor::ProcessingData aProcessingData =
    XSAlgo_ShapeProcessor::ReadProcessingData("write.step.resource.name", "write.step.sequence");
  aWriter.SetShapeFixParameters(std::move(aProcessingData.first));
  aWriter.SetShapeProcessFlags(aProcessingData.second);

  if (!aLabel.IsNull())
  {
    theDI << "Translating label " << aLabelName << " of document " << aDocumentName << " to STEP\n";

    if (!aWriter.Transfer(aLabel,
                          aMode,
                          !aMultiFilePrefix.IsEmpty() ? aMultiFilePrefix.ToCString() : NULL,
                          aRootScope.Next()))
    {
      theDI << "Error: the label of document cannot be translated or gives no result";
      return 1;
    }
  }
  else
  {
    theDI << "Translating document " << aDocumentName << " to STEP\n";

    if (!aWriter.Transfer(aDocument,
                          aMode,
                          !aMultiFilePrefix.IsEmpty() ? aMultiFilePrefix.ToCString() : NULL,
                          aRootScope.Next()))
    {
      theDI << "Error: The document cannot be translated or gives no result\n";
    }
  }

  if (!isFileMode)
  {
    theDI << "Document has been translated into the session";
    return 0;
  }

  Message_ProgressScope aWriteScope(aRootScope.Next(), "File writing", 1);
  aWriteScope.Show();
  theDI << "Writing STEP file " << aFilePath << "\n";

  IFSelect_ReturnStatus aStat = IFSelect_RetVoid;

  if (toTestStream)
  {
    std::ofstream aStream;
    OSD_OpenStream(aStream, aFilePath, std::ios::out | std::ios::binary);
    aStat = aWriter.WriteStream(aStream);
    aStream.close();

    if (!aStream.good() && aStat == IFSelect_RetDone)
    {
      aStat = IFSelect_RetFail;
    }
  }
  else
  {
    aStat = aWriter.Write(aFilePath.ToCString());
  }

  switch (aStat)
  {
    case IFSelect_RetVoid: {
      theDI << "Error: no file written";
      break;
    }
    case IFSelect_RetDone: {
      theDI << "File " << aFilePath << " written\n";

      XSDRAW::CollectActiveWorkSessions(aFilePath);
      for (ExternalFileMap::Iterator anIter(aWriter.ExternFiles()); anIter.More(); anIter.Next())
      {
        XSDRAW::CollectActiveWorkSessions(anIter.Value()->GetWS(),
                                          anIter.Key(),
                                          XSDRAW::WorkSessionList());
      }
      break;
    }
    default: {
      theDI << "Error on writing file";
      break;
    }
  }
  return 0;
}

//=================================================================================================

void XSDRAWSTEP::Factory(Draw_Interpretor& theDI)
{
  static Standard_Boolean aIsActivated = Standard_False;
  if (aIsActivated)
  {
    return;
  }
  aIsActivated = Standard_True;

  const char* aGroup = "DE: STEP"; // Step transfer file commands
  theDI.Add("stepwrite", "stepwrite mode[0-4 afsmw] shape", __FILE__, stepwrite, aGroup);
  theDI.Add("testwritestep",
            "testwritestep [file_1.stp ... file_n.stp] shape [-stream]",
            __FILE__,
            testwrite,
            aGroup);
  theDI.Add("stepread",
            "stepread  [file] [f or r (type of model full or reduced)]",
            __FILE__,
            stepread,
            aGroup);
  theDI.Add("testreadstep",
            "testreadstep [file_1 ... file_n] shape [-stream]",
            __FILE__,
            testreadstep,
            aGroup);
  theDI.Add("steptrans", "steptrans shape stepax1 stepax2", __FILE__, steptrans, aGroup);
  theDI.Add("countexpected", "TEST", __FILE__, countexpected, aGroup);
  theDI.Add("dumpassembly", "TEST", __FILE__, dumpassembly, aGroup);
  theDI.Add("stepfileunits", "stepfileunits name_file", __FILE__, stepfileunits, aGroup);
  theDI.Add("ReadStep",
            "Doc filename [mode] [-stream]"
            "\n\t\t: Read STEP file to a document."
            "\n\t\t:  -stream read using istream reading interface (testing)",
            __FILE__,
            ReadStep,
            aGroup);
  theDI.Add(
    "WriteStep",
    "Doc filename [mode=a [multifile_prefix] [label]] [-stream]"
    "\n\t\t: Write DECAF document to STEP file"
    "\n\t\t:   mode can be: a or 0 : AsIs (default)"
    "\n\t\t:                f or 1 : FacettedBRep        s or 2 : ShellBasedSurfaceModel"
    "\n\t\t:                m or 3 : ManifoldSolidBrep   w or 4 : GeometricCurveSet/WireFrame"
    "\n\t\t:   multifile_prefix: triggers writing assembly components as separate files,"
    "\n\t\t:                     and defines common prefix for their names"
    "\n\t\t:   label  tag of the sub-assembly label to save only that sub-assembly"
    "\n\t\t:  -stream read using ostream writing interface (testing)",
    __FILE__,
    WriteStep,
    aGroup);

  // Load XSDRAW session for pilot activation
  XSDRAW::LoadDraw(theDI);
}

// Declare entry point PLUGINFACTORY
DPLUGIN(XSDRAWSTEP)
