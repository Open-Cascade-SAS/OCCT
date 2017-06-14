//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"

using namespace uwp;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

using namespace Windows::UI::Popups;

#include <BRepAlgo.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRepTools.hxx>
#include <Geom2dAPI_PointsToBSpline.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <gp_Pnt.hxx>
#include <IGESControl_Controller.hxx>
#include <IGESControl_Reader.hxx>
#include <IGESControl_Writer.hxx>
#include <Interface_Static.hxx>
#include <OSD_Directory.hxx>
#include <OSD_File.hxx>
#include <OSD_Path.hxx>
#include <OSD_Protection.hxx>
#include <STEPControl_Reader.hxx>
#include <STEPControl_Writer.hxx>
#include <StlAPI_Writer.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <VrmlAPI_Writer.hxx>

#include <Strsafe.h>

//=======================================================================
//function : MainPage
//purpose  :
//=======================================================================
MainPage::MainPage()
{
  InitializeComponent();
  ApplicationView::PreferredLaunchViewSize = Size(1000, 500);
  ApplicationView::PreferredLaunchWindowingMode = ApplicationViewWindowingMode::PreferredLaunchViewSize;
}

//=======================================================================
//function : Throw
//purpose  : Test offset functionality
//=======================================================================
void MainPage::OnClickOffset(Platform::Object^  theSender,
  Windows::UI::Xaml::Input::PointerRoutedEventArgs^ theEvent)
{
  TColgp_Array1OfPnt2d array(1, 5); // sizing array
  array.SetValue(1, gp_Pnt2d(-4, 0)); array.SetValue(2, gp_Pnt2d(-7, 2));
  array.SetValue(3, gp_Pnt2d(-6, 3)); array.SetValue(4, gp_Pnt2d(-4, 3));
  array.SetValue(5, gp_Pnt2d(-3, 5));
  Handle(Geom2d_BSplineCurve) SPL1 = Geom2dAPI_PointsToBSpline(array);

  Standard_Real dist = 1;
  Handle(Geom2d_OffsetCurve) OC =
    new Geom2d_OffsetCurve(SPL1, dist);
  Standard_Real dist2 = 1.5;
  Handle(Geom2d_OffsetCurve) OC2 =
    new Geom2d_OffsetCurve(SPL1, dist2);

  Platform::String ^aMessage;

  if (OC.IsNull()) {
    aMessage = "Error: could not create offset curve with offset distance " + dist;
  } else if (OC2.IsNull()) {
    aMessage = "Error: could not create offset curve with offset distance 1.5" + dist2;
  } else {
    aMessage = "Result: Two offset curves OC and OC2 were successfully created from source curve SPL1. \n";
  }
  Output_TextBlock->Text = aMessage;
}

//=======================================================================
//function : OnClickMesh
//purpose  : Test mesh functionality
//=======================================================================
void MainPage::OnClickMesh(Platform::Object^  theSender,
  Windows::UI::Xaml::Input::PointerRoutedEventArgs^ theEvent)
{
  TopoDS_Shape theBox = BRepPrimAPI_MakeBox(200, 60, 60);
  TopoDS_Shape theSphere = BRepPrimAPI_MakeSphere(gp_Pnt(100, 20, 20), 80);
  TopoDS_Shape ShapeFused = BRepAlgoAPI_Fuse(theSphere, theBox);
  BRepMesh_IncrementalMesh(ShapeFused, 1);

  Standard_Integer result(0);

  for (TopExp_Explorer ex(ShapeFused, TopAbs_FACE); ex.More(); ex.Next()) {
    TopoDS_Face F = TopoDS::Face(ex.Current());
    TopLoc_Location L;
    Handle(Poly_Triangulation) facing = BRep_Tool::Triangulation(F, L);
    result = result + facing->NbTriangles();
  }

  Platform::String ^aMessage;
  if (ShapeFused.IsNull()) {
    aMessage = "Error: could not fuse source shapes";
  } else if (result == 0) {
    aMessage = "Error: mesh could not be created";
  } else {
    aMessage = "Result: Mesh was created\
--- Number of created triangles ---\n";

    aMessage += result;
    aMessage += ("\n\n");
  }
  Output_TextBlock->Text = aMessage;
}

//=======================================================================
//function : OnClickBoolean
//purpose  : Test boolean operations
//=======================================================================
void MainPage::OnClickBoolean(Platform::Object^  theSender,
  Windows::UI::Xaml::Input::PointerRoutedEventArgs^ theEvent)
{
  TCollection_AsciiString asd;
  gp_Pnt P(-5, 5, -5);
  TopoDS_Shape theBox1 = BRepPrimAPI_MakeBox(60, 200, 70).Shape();

  TopoDS_Shape theBox2 = BRepPrimAPI_MakeBox(P, 20, 150, 110).Shape();

  TopoDS_Shape FusedShape = BRepAlgoAPI_Fuse(theBox1, theBox2);

  Platform::String ^aMessage;

  if (FusedShape.IsNull()) {
    aMessage = "Error: could not fuse shapes theBox1 and theBox2";
  } else {
    aMessage = "Result: Shapes were successfully fused. \n";
  }
  Output_TextBlock->Text = aMessage;
}

//=======================================================================
//function : OnClickBuildTemporary
//purpose  : Test OSD_File::BuildTemporary() method
//=======================================================================
void MainPage::OnClickBuildTemporary(Platform::Object^  theSender,
  Windows::UI::Xaml::Input::PointerRoutedEventArgs^ theEvent)
{
  OSD_File theTmpFile;
  theTmpFile.BuildTemporary();
  Standard_Boolean fKO = theTmpFile.Failed();
  if (fKO)
  {
    Output_TextBlock->Text = "Error: cannot create temporary file";
  } else {
    OSD_Path theTmpFilepath;
    theTmpFile.Path(theTmpFilepath);
    TCollection_AsciiString theTmpFilepathTrek;
    theTmpFilepath.SystemName(theTmpFilepathTrek);
    wchar_t wchar_str[MAX_PATH];
    StringCchCopy(wchar_str, _countof(wchar_str), L"Result: ");
    TCollection_ExtendedString aWName(theTmpFilepathTrek);
    StringCchCat(wchar_str, _countof(wchar_str), (const wchar_t*)aWName.ToExtString());
    Platform::String^ p_string = ref new Platform::String(wchar_str);
    Output_TextBlock->Text = p_string;
  }
}

//=======================================================================
//function : OnClickDataExchange
//purpose  : Test data exchange functionality
//=======================================================================
void MainPage::OnClickDataExchange(Platform::Object^  theSender,
    Windows::UI::Xaml::Input::PointerRoutedEventArgs^ theEvent)
{
  Output_TextBlock->Text = "";

  // Create box for export
  TopoDS_Shape theBox = BRepPrimAPI_MakeBox(200, 60, 60);

  // Create temporary directory
  wchar_t tmpPath[MAX_PATH];
  wchar_t filePath[MAX_PATH];
  char tmpPathA[MAX_PATH];

  if (!GetTempPathW(_countof(tmpPath), tmpPath)) {
    StringCchCopyW(tmpPath, _countof(tmpPath), L"./");
  }

  WideCharToMultiByte(CP_UTF8, 0, tmpPath, -1, tmpPathA, sizeof(tmpPathA), NULL, NULL);
  OSD_Path tmpDirPath(tmpPathA);
  OSD_Directory tmpDir(tmpDirPath);

  OSD_Protection srt;
  tmpDir.Build(srt);

  // Export box to .brep
  StringCchCopyW(filePath, _countof(filePath), tmpPath);
  StringCchCatW(filePath, _countof(filePath), L"/box.brep");
  if (SaveBREP(filePath, theBox))
    Output_TextBlock->Text += L"OK: export to .brep\n";
  // Import from .brep
  TopoDS_Shape theBoxFromBrep;
  if (ReadBREP(filePath, theBoxFromBrep))
    Output_TextBlock->Text += L"OK: import from .brep\n";
  else
    Output_TextBlock->Text += L"Error: import from .brep\n";

  // Export box to .iges
  StringCchCopyW(filePath, _countof(filePath), tmpPath);
  StringCchCatW(filePath, _countof(filePath), L"/box.iges");
  if (SaveIGES(filePath, theBox))
    Output_TextBlock->Text += L"OK: export to .iges\n";
  // Import from .iges
  TopoDS_Shape theBoxFromIges;
  if (ReadIGES(filePath, theBoxFromIges))
    Output_TextBlock->Text += L"OK: import from .iges\n";
  else
    Output_TextBlock->Text += L"Error: import from .iges\n";

  // Export box to .step
  StringCchCopyW(filePath, _countof(filePath), tmpPath);
  StringCchCatW(filePath, _countof(filePath), L"/box.step");

  STEPControl_StepModelType aSelection = STEPControl_AsIs;
  if (SaveSTEP(filePath, theBox, aSelection))
    Output_TextBlock->Text += L"OK: export to .iges\n";
  // Import from .iges
  TopoDS_Shape theBoxFromStep;
  if (ReadSTEP(filePath, theBoxFromStep))
    Output_TextBlock->Text += L"OK: import from .step\n";
  else
    Output_TextBlock->Text += L"Error: import from .step\n";

  // Export box to .stl
  StringCchCopyW(filePath, _countof(filePath), tmpPath);
  StringCchCatW(filePath, _countof(filePath), L"/box.stl");

  if (SaveSTL(filePath, theBox))
    Output_TextBlock->Text += L"OK: export to .stl\n";

  // Export box to .vrml
  StringCchCopyW(filePath, _countof(filePath), tmpPath);
  StringCchCatW(filePath, _countof(filePath), L"/box.vrml");

  if (SaveVRML(filePath, theBox))
    Output_TextBlock->Text += L"OK: export to .vrml\n";
}

//=======================================================================
//function : SaveBREP
//purpose  : Export shape to .brep
//=======================================================================
Standard_Boolean MainPage::SaveBREP(const wchar_t* theFilePath, const TopoDS_Shape& theShape)
{
  std::filebuf aFileBuf;
  std::ostream aStream(&aFileBuf);

  if (!aFileBuf.open(theFilePath, ios::out)) {
    Output_TextBlock->Text += L"Error: cannot open file for export (brep)\n";
    return Standard_False;
  }

  BRepTools::Write(theShape, aStream);
  return Standard_True;
}

//=======================================================================
//function : SaveIGES
//purpose  : Export shape to .iges
//=======================================================================
Standard_Boolean MainPage::SaveIGES(const wchar_t* theFilePath, const TopoDS_Shape& theShape)
{
  std::filebuf aFileBuf;
  std::ostream aStream(&aFileBuf);

  if (!aFileBuf.open(theFilePath, ios::out)) {
    Output_TextBlock->Text += L"Error: cannot open file for export (iges)\n";
    return Standard_False;
  }

  IGESControl_Controller::Init();
  IGESControl_Writer ICW(Interface_Static::CVal("XSTEP.iges.unit"), Interface_Static::IVal("XSTEP.iges.writebrep.mode"));

  ICW.AddShape(theShape);
  ICW.ComputeModel();

  if (!ICW.Write(aStream)) {
    Output_TextBlock->Text += L"Error: cannot export box to .iges\n";
    return Standard_False;
  }

  return Standard_True;
}

//=======================================================================
//function : SaveSTEP
//purpose  : Export shape to .step
//=======================================================================
Standard_Boolean MainPage::SaveSTEP(const wchar_t* theFilePath, const TopoDS_Shape& theShape, const STEPControl_StepModelType theValue)
{
  std::filebuf aFileBuf;
  std::ostream aStream(&aFileBuf);

  if (!aFileBuf.open(theFilePath, ios::out)) {
    Output_TextBlock->Text += L"Error: cannot open file for export (step)\n";
    return Standard_False;
  }

  STEPControl_Writer aWriter;

  if (aWriter.Transfer(theShape, theValue) != IFSelect_RetDone) {
    Output_TextBlock->Text += L"Error: cannot translate shape to STEP\n";
    return Standard_False;
  }

  const TCollection_AsciiString aFilePath (theFilePath);
  switch (aWriter.Write (aFilePath.ToCString()))
  {
  case IFSelect_RetError:
    Output_TextBlock->Text += L"Error: Incorrect Data\n";
    break;
  case IFSelect_RetFail:
    Output_TextBlock->Text += L"Error: Writing has failed\n";
    break;
  case IFSelect_RetVoid:
    Output_TextBlock->Text += L"Error: Nothing to transfer\n";
    break;
  default:
    return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
//function : SaveSTL
//purpose  : Export shape to .stl
//=======================================================================
Standard_Boolean MainPage::SaveSTL(const wchar_t* theFilePath, const TopoDS_Shape& theShape)
{
  StlAPI_Writer myStlWriter;
  const TCollection_AsciiString aFilePath (theFilePath);
  return myStlWriter.Write (theShape, aFilePath.ToCString());
}

//=======================================================================
//function : SaveVRML
//purpose  : Export shape to .vrml
//=======================================================================
Standard_Boolean MainPage::SaveVRML(const wchar_t* theFilePath, const TopoDS_Shape& theShape)
{
  VrmlAPI_Writer aWriter;
  const TCollection_AsciiString aFilePath (theFilePath);
  aWriter.Write (theShape, aFilePath.ToCString());
  return Standard_True;
}

//=======================================================================
//function : ReadBREP
//purpose  : Import shape from .brep
//=======================================================================
Standard_Boolean MainPage::ReadBREP(const wchar_t* theFilePath, TopoDS_Shape& theShape)
{
  theShape.Nullify();

  BRep_Builder aBuilder;
  const TCollection_AsciiString aFilePath (theFilePath);
  if (!BRepTools::Read(theShape, aFilePath.ToCString(), aBuilder))
    return Standard_False;

  return !theShape.IsNull() && BRepAlgo::IsValid(theShape);
}

//=======================================================================
//function : ReadIGES
//purpose  : Import shape from .iges
//=======================================================================
Standard_Boolean MainPage::ReadIGES(const wchar_t* theFilePath, TopoDS_Shape& theShape)
{
  theShape.Nullify();

  IGESControl_Reader Reader;

  const TCollection_AsciiString aFilePath (theFilePath);
  if (Reader.ReadFile (aFilePath.ToCString()) != IFSelect_RetDone)
    return Standard_False;

  Reader.TransferRoots();
  theShape = Reader.OneShape();

  return !theShape.IsNull();
}

//=======================================================================
//function : ReadSTEP
//purpose  : Import shape from .step
//=======================================================================
Standard_Boolean MainPage::ReadSTEP(const wchar_t* theFilePath, TopoDS_Shape& theShape)
{
  theShape.Nullify();

  STEPControl_Reader aReader;
  const TCollection_AsciiString aFilePath (theFilePath);
  switch (aReader.ReadFile (aFilePath.ToCString()))
  {
  case IFSelect_RetError:
    Output_TextBlock->Text += L"Error: Not a valid Step file\n";
    break;
  case IFSelect_RetFail:
    Output_TextBlock->Text += L"Error: Reading has failed\n";
    break;
  case IFSelect_RetVoid:
    Output_TextBlock->Text += L"Error: Nothing to transfer\n";
    break;
  default:
    return Standard_True;
  }
  return Standard_False;
}
