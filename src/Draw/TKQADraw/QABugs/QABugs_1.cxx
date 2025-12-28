// Created on: 2002-05-21
// Created by: QA Admin
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#include <QABugs.hxx>

#include <Draw_Interpretor.hxx>
#include <DBRep.hxx>
#include <ViewerTest.hxx>
#include <AIS_Shape.hxx>
#include <TopoDS_Shape.hxx>

#include <ViewerTest_DoubleMapOfInteractiveAndName.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Map.hxx>

#include <TDocStd_Application.hxx>
#include <DDocStd.hxx>
#include <TDocStd_Owner.hxx>
#include <TDF_Label.hxx>
#include <DDF.hxx>
#include <TPrsStd_AISViewer.hxx>
#include <TPrsStd_AISPresentation.hxx>

#include <Draw_Viewer.hxx>
#include <Draw.hxx>

#ifndef _WIN32
extern Draw_Viewer dout;
#else
Standard_IMPORT Draw_Viewer dout;
#endif

#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <TopoDS.hxx>

#if !defined(_WIN32)
extern NCollection_DoubleMap<occ::handle<AIS_InteractiveObject>, TCollection_AsciiString>&
  GetMapOfAIS();
#else
Standard_EXPORT NCollection_DoubleMap<occ::handle<AIS_InteractiveObject>, TCollection_AsciiString>&
                GetMapOfAIS();
#endif

#include <AIS_PlaneTrihedron.hxx>
#include <TopExp_Explorer.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <GC_MakePlane.hxx>

static int OCC159bug(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc != 2)
  {
    di << "ERROR : Usage : " << argv[0] << " Doc\n";
    return 1;
  }

  occ::handle<TDocStd_Document> D;
  if (!DDocStd::GetDocument(argv[1], D))
    return 1;

  int DocRefCount1 = D->GetRefCount();
  di << "DocRefCount1 = " << DocRefCount1 << "\n";

  occ::handle<TDocStd_Owner> Owner;
  if (!D->Main().Root().FindAttribute(TDocStd_Owner::GetID(), Owner))
    return 1;

  occ::handle<TDocStd_Document> OwnerD1 = Owner->GetDocument();
  if (OwnerD1.IsNull())
  {
    di << "DocOwner1 = NULL\n";
  }
  else
  {
    di << "DocOwner1 = NOTNULL\n";
  }
  OwnerD1.Nullify();

  occ::handle<TDocStd_Application> A = DDocStd::GetApplication();
  A->Close(D);

  if (occ::handle<Draw_Drawable3D> DD = Draw::GetExisting(argv[1]))
  {
    dout.RemoveDrawable(DD);
  }

  occ::handle<TDocStd_Document> OwnerD2 = Owner->GetDocument();
  if (OwnerD2.IsNull())
  {
    di << "DocOwner2 = NULL\n";
  }
  else
  {
    di << "DocOwner2 = NOTNULL\n";
  }

  int DocRefCount2 = D->GetRefCount();
  di << "DocRefCount2 = " << DocRefCount2 << "\n";

  return 0;
}

static int OCC145bug(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc != 3)
  {
    di << "ERROR : Usage : " << argv[0] << " Shape MaxNbr\n";
    return 1;
  }

  TCollection_AsciiString aFileName = argv[1];
  int                     aMaxNbr   = Draw::Atoi(argv[2]);

  BRep_Builder aBld;
  TopoDS_Shape aShape;

  if (!BRepTools::Read(aShape, aFileName.ToCString(), aBld))
  {
    di << "ERROR :Could not read a shape!!!\n";
    return 1;
  }

  int         i;
  TopoDS_Wire aWire = TopoDS::Wire(aShape);

  for (i = 1; i <= aMaxNbr; i++)
  {
    BRepBuilderAPI_MakeFace aMF(aWire);
    if (!aMF.IsDone())
    {
      di << "ERROR : Could not make a face\n";
      return 1;
    }
  }

  return 0;
}

static int OCC73_SelectionMode(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc < 3)
  {
    di << "ERROR : Usage : " << argv[0] << " DOC entry [SelectionMode]\n";
    return 1;
  }

  occ::handle<TDocStd_Document> D;
  // std::cout << "OCC73_SelectionMode  1" << std::endl;
  if (!DDocStd::GetDocument(argv[1], D))
    return 1;
  TDF_Label L;
  // std::cout << "OCC73_SelectionMode  2" << std::endl;
  if (!DDF::FindLabel(D->GetData(), argv[2], L))
    return 1;

  occ::handle<TPrsStd_AISViewer> viewer;
  // std::cout << "OCC73_SelectionMode  3" << std::endl;
  if (!TPrsStd_AISViewer::Find(L, viewer))
    return 1;

  occ::handle<TPrsStd_AISPresentation> prs;
  // std::cout << "OCC73_SelectionMode  4" << std::endl;
  if (L.FindAttribute(TPrsStd_AISPresentation::GetID(), prs))
  {
    if (argc == 4)
    {
      prs->SetSelectionMode((int)Draw::Atoi(argv[3]));
      TPrsStd_AISViewer::Update(L);
    }
    else
    {
      int SelectionMode = prs->SelectionMode();
      // std::cout << "SelectionMode = " << SelectionMode << std::endl;
      di << SelectionMode;
    }
  }
  // std::cout << "OCC73_SelectionMode  5" << std::endl;

  return 0;
}

static int OCC10bug(Draw_Interpretor& di, int argc, const char** argv)
{
  occ::handle<AIS_InteractiveContext> aContext = ViewerTest::GetAISContext();
  if (aContext.IsNull())
  {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return 1;
  }

  if (argc != 4)
  {
    di << "Usage : " << argv[0] << " name plane Length\n";
    return 1;
  }

  TopoDS_Shape S = DBRep::Get(argv[2]);
  if (S.IsNull())
  {
    di << "Shape is empty\n";
    return 1;
  }

  TCollection_AsciiString name(argv[1]);
  double                  Length = Draw::Atof(argv[3]);

  // Construction de l'AIS_PlaneTrihedron
  occ::handle<AIS_PlaneTrihedron> theAISPlaneTri;

  bool IsBound = GetMapOfAIS().IsBound2(name);
  if (IsBound)
  {
    // on recupere la shape dans la map des objets displayes
    occ::handle<AIS_InteractiveObject> aShape = GetMapOfAIS().Find2(name);

    // On verifie que l'AIS InteraciveObject est bien
    // un AIS_PlaneTrihedron
    if (aShape->Type() == AIS_KindOfInteractive_Datum && aShape->Signature() == 4)
    {
      // On downcast aShape de AIS_InteractiveObject a AIS_PlaneTrihedron
      theAISPlaneTri = occ::down_cast<AIS_PlaneTrihedron>(aShape);

      theAISPlaneTri->SetLength(Length);

      aContext->Redisplay(theAISPlaneTri, false);
      aContext->UpdateCurrentViewer();
    }
  }
  else
  {
    TopoDS_Face FaceB = TopoDS::Face(S);

    // Construction du Plane
    // recuperation des edges des faces.
    TopExp_Explorer FaceExpB(FaceB, TopAbs_EDGE);

    TopoDS_Edge EdgeB = TopoDS::Edge(FaceExpB.Current());
    // declarations
    gp_Pnt A, B, C;

    // si il y a plusieurs edges
    if (FaceExpB.More())
    {
      FaceExpB.Next();
      TopoDS_Edge       EdgeC = TopoDS::Edge(FaceExpB.Current());
      BRepAdaptor_Curve theCurveB(EdgeB);
      BRepAdaptor_Curve theCurveC(EdgeC);
      A = theCurveC.Value(0.1);
      B = theCurveC.Value(0.9);
      C = theCurveB.Value(0.5);
    }
    else
    {
      // FaceB a 1 unique edge courbe
      BRepAdaptor_Curve theCurveB(EdgeB);
      A = theCurveB.Value(0.1);
      B = theCurveB.Value(0.9);
      C = theCurveB.Value(0.5);
    }
    // Construction du Geom_Plane
    GC_MakePlane                   MkPlane(A, B, C);
    const occ::handle<Geom_Plane>& theGeomPlane = MkPlane.Value();

    // on le display & bind
    theAISPlaneTri = new AIS_PlaneTrihedron(theGeomPlane);

    theAISPlaneTri->SetLength(Length);

    GetMapOfAIS().Bind(theAISPlaneTri, name);
    aContext->Display(theAISPlaneTri, true);
  }

  double getLength = theAISPlaneTri->GetLength();
  di << "Length = " << Length << "\n";
  di << "getLength = " << getLength << "\n";

  if (getLength == Length)
  {
    di << "OCC10: OK\n";
  }
  else
  {
    di << "OCC10: ERROR\n";
  }

  return 0;
}

static int OCC74bug_set(Draw_Interpretor& di, int argc, const char** argv)
{
  occ::handle<AIS_InteractiveContext> aContext = ViewerTest::GetAISContext();
  if (aContext.IsNull())
  {
    di << argv[0] << "ERROR : use 'vinit' command before \n";
    return 1;
  }

  if (argc != 3)
  {
    di << "ERROR : Usage : " << argv[0] << " shape mode; set selection mode\n";
    return 1;
  }

  bool updateviewer = true;

  NCollection_DoubleMap<occ::handle<AIS_InteractiveObject>, TCollection_AsciiString>& aMap =
    GetMapOfAIS();

  TCollection_AsciiString            aName(argv[1]);
  occ::handle<AIS_InteractiveObject> AISObj;

  int SelectMode = Draw::Atoi(argv[2]);
  if (!aMap.Find2(aName, AISObj) || AISObj.IsNull())
  {
    di << "Use 'vdisplay' before\n";
    return 1;
  }

  aContext->Erase(AISObj, updateviewer);
  aContext->UpdateCurrentViewer();
  aContext->SetAutoActivateSelection(false);
  aContext->Display(AISObj, updateviewer);
  aContext->Activate(AISObj, SelectMode);
  aContext->UpdateCurrentViewer();
  return 0;
}

static int OCC74bug_get(Draw_Interpretor& di, int argc, const char** argv)
{
  occ::handle<AIS_InteractiveContext> aContext = ViewerTest::GetAISContext();
  if (aContext.IsNull())
  {
    di << argv[0] << "ERROR : use 'vinit' command before \n";
    return 1;
  }

  if (argc != 2)
  {
    di << "ERROR : Usage : " << argv[0] << " shape; get selection mode\n";
    return 1;
  }

  NCollection_DoubleMap<occ::handle<AIS_InteractiveObject>, TCollection_AsciiString>& aMap =
    GetMapOfAIS();

  TCollection_AsciiString            aName(argv[1]);
  occ::handle<AIS_InteractiveObject> AISObj;
  if (!aMap.Find2(aName, AISObj) || AISObj.IsNull())
  {
    di << "Use 'vdisplay' before\n";
    return 1;
  }

  NCollection_List<int> anActivatedModes;
  aContext->ActivatedModes(AISObj, anActivatedModes);
  int aMode = anActivatedModes.IsEmpty() ? -1 : anActivatedModes.Last();
  di << aMode << "\n";
  return 0;
}

#include <BRepPrimAPI_MakeBox.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_NamedShape.hxx>
#include <AIS_InteractiveObject.hxx>

#include <Graphic3d_Texture2D.hxx>
#include <Image_AlienPixMap.hxx>
#include <OSD_FileSystem.hxx>
#include <Prs3d_ShadingAspect.hxx>

//=======================================================================
// function : OCC30182
// purpose  : Testing different interfaces of Image_AlienPixMap::Load()
//=======================================================================
static int OCC30182(Draw_Interpretor& di, int theNbArgs, const char** theArgVec)
{
  if (ViewerTest::CurrentView().IsNull())
  {
    di << "Error: no active view\n";
    return 1;
  }

  TCollection_AsciiString aPrsName, anImgPath;
  int                     anOffset = 0;
  int                     aSrc     = 0; // 0 - file name, 1 - file stream, 2 - memory buffer
  for (int anArgIter = 1; anArgIter < theNbArgs; ++anArgIter)
  {
    TCollection_AsciiString anArg(theArgVec[anArgIter]);
    anArg.LowerCase();
    if (anArg == "-offset" && anArgIter + 1 < theNbArgs)
    {
      anOffset = Draw::Atoi(theArgVec[++anArgIter]);
    }
    else if (anArg == "-filename")
    {
      aSrc = 0;
    }
    else if (anArg == "-stream")
    {
      aSrc = 1;
    }
    else if (anArg == "-mem" || anArg == "-memory")
    {
      aSrc = 2;
    }
    else if (aPrsName.IsEmpty())
    {
      aPrsName = theArgVec[anArgIter];
    }
    else if (anImgPath.IsEmpty())
    {
      anImgPath = theArgVec[anArgIter];
    }
    else
    {
      di << "Syntax error at '" << anArg << "'\n";
      return 1;
    }
  }
  if (anImgPath.IsEmpty())
  {
    di << "Syntax error: wrong number of arguments\n";
    return 1;
  }

  occ::handle<Image_AlienPixMap> anImage = new Image_AlienPixMap();
  if (aSrc == 0)
  {
    if (!anImage->Load(anImgPath))
    {
      return 0;
    }
  }
  else
  {
    const occ::handle<OSD_FileSystem>& aFileSystem = OSD_FileSystem::DefaultFileSystem();
    std::shared_ptr<std::istream>      aFile =
      aFileSystem->OpenIStream(anImgPath, std::ios::in | std::ios::binary);
    if (aFile.get() == NULL)
    {
      di << "Syntax error: image file '" << anImgPath << "' cannot be found\n";
      return 1;
    }
    if (anOffset != 0)
    {
      aFile->seekg(anOffset);
    }

    if (aSrc == 2)
    {
      aFile->seekg(0, std::ios::end);
      int aLen = (int)aFile->tellg() - anOffset;
      aFile->seekg(anOffset);
      if (aLen <= 0)
      {
        di << "Syntax error: wrong offset\n";
        return 1;
      }
      NCollection_Array1<uint8_t> aBuff(1, aLen);
      if (!aFile->read((char*)&aBuff.ChangeFirst(), aBuff.Size()))
      {
        di << "Error: unable to read file\n";
        return 1;
      }
      if (!anImage->Load(&aBuff.ChangeFirst(), aBuff.Size(), anImgPath))
      {
        return 0;
      }
    }
    else
    {
      if (!anImage->Load(*aFile, anImgPath))
      {
        return 0;
      }
    }
  }

  TopoDS_Shape           aShape = BRepPrimAPI_MakeBox(100.0 * anImage->Ratio(), 100.0, 1.0).Shape();
  occ::handle<AIS_Shape> aPrs   = new AIS_Shape(aShape);
  aPrs->SetDisplayMode(AIS_Shaded);
  aPrs->Attributes()->SetupOwnShadingAspect();
  const occ::handle<Graphic3d_AspectFillArea3d>& anAspect =
    aPrs->Attributes()->ShadingAspect()->Aspect();
  anAspect->SetShadingModel(Graphic3d_TypeOfShadingModel_Unlit);
  anAspect->SetTextureMapOn(true);
  anAspect->SetTextureMap(new Graphic3d_Texture2D(anImage));
  if (anImage->IsTopDown())
  {
    anAspect->TextureMap()->GetParams()->SetTranslation(NCollection_Vec2<float>(0.0f, -1.0f));
    anAspect->TextureMap()->GetParams()->SetScale(NCollection_Vec2<float>(1.0f, -1.0f));
  }

  ViewerTest::Display(aPrsName, aPrs, true, true);
  return 0;
}

//=======================================================================
// function : OCC31956
// purpose  : Testing Image_AlienPixMap::Save() overload for saving into a memory buffer or stream
//=======================================================================
static int OCC31956(Draw_Interpretor& di, int theNbArgs, const char** theArgVec)
{
  if (ViewerTest::CurrentView().IsNull())
  {
    di << "Error: no active view\n";
    return 1;
  }
  if (theNbArgs != 3 && theNbArgs != 5)
  {
    di << "Syntax error: wrong number of arguments\n";
    return 1;
  }

  bool                    useStream = false;
  TCollection_AsciiString aTempImgPath;
  if (theNbArgs == 5)
  {
    TCollection_AsciiString anArg(theArgVec[3]);
    anArg.LowerCase();
    if (anArg == "-stream")
    {
      useStream    = true;
      aTempImgPath = theArgVec[4];
    }
    else
    {
      di << "Syntax error at '" << anArg << "'\n";
      return 1;
    }
  }

  TCollection_AsciiString aPrsName, anImgPath;
  aPrsName                                               = theArgVec[1];
  anImgPath                                              = theArgVec[2];
  occ::handle<Image_AlienPixMap>             anImage     = new Image_AlienPixMap();
  const occ::handle<OSD_FileSystem>&         aFileSystem = OSD_FileSystem::DefaultFileSystem();
  opencascade::std::shared_ptr<std::istream> aFile =
    aFileSystem->OpenIStream(anImgPath, std::ios::in | std::ios::binary);
  if (aFile.get() == NULL)
  {
    di << "Syntax error: image file '" << anImgPath << "' cannot be found\n";
    return 1;
  }

  aFile->seekg(0, std::ios::end);
  int aLen = (int)aFile->tellg();
  aFile->seekg(0);
  if (!anImage->Load(*aFile, anImgPath))
  {
    return 0;
  }

  occ::handle<Image_AlienPixMap> aControlImg = new Image_AlienPixMap();
  if (useStream)
  {
    opencascade::std::shared_ptr<std::ostream> aTempFile =
      aFileSystem->OpenOStream(aTempImgPath, std::ios::out | std::ios::binary);
    if (aTempFile.get() == NULL)
    {
      di << "Error: image file '" << aTempImgPath << "' cannot be open\n";
      return 0;
    }
    if (!anImage->Save(*aTempFile, aTempImgPath))
    {
      di << "Error: failed saving file using stream '" << aTempImgPath << "'\n";
      return 0;
    }
    aTempFile.reset();
    aControlImg->Load(aTempImgPath);
  }
  else
  {
    NCollection_Array1<uint8_t> aBuff(1, aLen + 2048);
    if (!anImage->Save(&aBuff.ChangeFirst(), aBuff.Size(), anImgPath))
    {
      di << "Error: failed saving file using buffer'" << anImgPath << "'\n";
      return 0;
    }
    aControlImg->Load(&aBuff.ChangeFirst(), aBuff.Size(), anImgPath);
  }

  TopoDS_Shape aShape = BRepPrimAPI_MakeBox(100.0 * aControlImg->Ratio(), 100.0, 1.0).Shape();
  occ::handle<AIS_Shape> aPrs = new AIS_Shape(aShape);
  aPrs->SetDisplayMode(AIS_Shaded);
  aPrs->Attributes()->SetupOwnShadingAspect();
  const occ::handle<Graphic3d_AspectFillArea3d>& anAspect =
    aPrs->Attributes()->ShadingAspect()->Aspect();
  anAspect->SetShadingModel(Graphic3d_TOSM_UNLIT);
  anAspect->SetTextureMapOn(true);
  anAspect->SetTextureMap(new Graphic3d_Texture2D(aControlImg));
  if (aControlImg->IsTopDown())
  {
    anAspect->TextureMap()->GetParams()->SetTranslation(NCollection_Vec2<float>(0.0f, -1.0f));
    anAspect->TextureMap()->GetParams()->SetScale(NCollection_Vec2<float>(1.0f, -1.0f));
  }

  ViewerTest::Display(aPrsName, aPrs, true, true);
  return 0;
}

void QABugs::Commands_1(Draw_Interpretor& theCommands)
{
  const char* group = "QABugs";

  theCommands.Add("OCC159", "OCC159 Doc", __FILE__, OCC159bug, group);
  theCommands.Add("OCC145", "OCC145 Shape MaxNbr", __FILE__, OCC145bug, group);

  theCommands.Add("OCC73_SelectionMode",
                  "OCC73_SelectionMode DOC entry [SelectionMode]",
                  __FILE__,
                  OCC73_SelectionMode,
                  group);

  theCommands.Add("OCC10", "OCC10 Shape MaxNbr", __FILE__, OCC10bug, group);

  theCommands.Add("OCC74_set",
                  "OCC74_set shape mode;   set selection mode",
                  __FILE__,
                  OCC74bug_set,
                  group);
  theCommands.Add("OCC74_get",
                  "OCC74_get shape;   get selection mode",
                  __FILE__,
                  OCC74bug_get,
                  group);

  theCommands.Add("OCC30182",
                  "OCC30182 name image [-offset Start] [-fileName] [-stream] [-memory]\n"
                  "Decodes image either by passing file name, file stream or memory stream",
                  __FILE__,
                  OCC30182,
                  group);
  theCommands.Add("OCC31956",
                  "OCC31956 name image [-stream tempImage]\n"
                  "Loads image and saves it into memory buffer or stream then loads it back",
                  __FILE__,
                  OCC31956,
                  group);
  return;
}
