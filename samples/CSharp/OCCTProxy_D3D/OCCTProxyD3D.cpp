#include <windows.h>

// include required OCCT headers
#include <Standard_Version.hxx>
#include <Message_ProgressIndicator.hxx>
//for OCC graphic
#include <WNT_Window.hxx>
#include <WNT_WClass.hxx>
#include <Graphic3d_CView.hxx>
#include <Graphic3d_Camera.hxx>
#include <Graphic3d_TextureParams.hxx>
#include <D3DHost_GraphicDriver.hxx>
#include <D3DHost_View.hxx>
//for object display
#include <V3d_Viewer.hxx>
#include <V3d_View.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_Shape.hxx>
//topology
#include <TopoDS_Shape.hxx>
#include <TopoDS_Compound.hxx>
//brep tools
#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
// iges I/E
#include <IGESControl_Reader.hxx>
#include <IGESControl_Controller.hxx>
#include <IGESControl_Writer.hxx>
#include <IFSelect_ReturnStatus.hxx>
#include <Interface_Static.hxx>
//step I/E
#include <STEPControl_Reader.hxx>
#include <STEPControl_Writer.hxx>
//for stl export
#include <StlAPI_Writer.hxx>
//for vrml export
#include <VrmlAPI_Writer.hxx>
//wrapper of pure C++ classes to ref classes
#include <NCollection_Haft.h>

// list of required OCCT libraries
#pragma comment(lib, "TKernel.lib")
#pragma comment(lib, "TKMath.lib")
#pragma comment(lib, "TKBRep.lib")
#pragma comment(lib, "TKXSBase.lib")
#pragma comment(lib, "TKService.lib")
#pragma comment(lib, "TKV3d.lib")
#pragma comment(lib, "TKOpenGl.lib")
#pragma comment(lib, "TKD3dHost.lib")
#pragma comment(lib, "TKIGES.lib")
#pragma comment(lib, "TKSTEP.lib")
#pragma comment(lib, "TKStl.lib")
#pragma comment(lib, "TKVrml.lib")

#pragma comment(lib, "D3D9.lib")

/// <summary>
/// Proxy class encapsulating calls to OCCT C++ classes within
/// C++/CLI class visible from .Net (CSharp)
/// </summary>
public ref class OCCTProxyD3D
{
public:

  OCCTProxyD3D() {}

  // ============================================
  // Viewer functionality
  // ============================================

  /// <summary>
  ///Initialize a viewer
  /// </summary>
  /// <param name="theWnd">System.IntPtr that contains the window handle (HWND) of the control</param>
  bool InitViewer()
  {
    myGraphicDriver() = new D3DHost_GraphicDriver();
    myGraphicDriver()->ChangeOptions().buffersNoSwap = true;
    //myGraphicDriver()->ChangeOptions().contextDebug = true;

    TCollection_ExtendedString a3DName ("Visu3D");
    myViewer() = new V3d_Viewer (myGraphicDriver(), a3DName.ToExtString(), "", 1000.0,
                                 V3d_XposYnegZpos, Quantity_NOC_GRAY30,
                                 V3d_ZBUFFER, V3d_GOURAUD, V3d_WAIT,
                                 Standard_True, Standard_False);

    myViewer()->SetDefaultLights();
    myViewer()->SetLightOn();
    myView() = myViewer()->CreateView();

    static Handle(WNT_WClass) aWClass = new WNT_WClass ("OCC_Viewer", NULL, CS_OWNDC);
    Handle(WNT_Window) aWNTWindow = new WNT_Window ("OCC_Viewer", aWClass, WS_POPUP, 64, 64, 64, 64);
    aWNTWindow->SetVirtual (Standard_True);
    myView()->SetWindow(aWNTWindow);
    myAISContext() = new AIS_InteractiveContext (myViewer());
    myAISContext()->UpdateCurrentViewer();
    myView()->MustBeResized();
    return true;
  }

  /// <summary> Resizes custom FBO for Direct3D output. </summary>
  System::IntPtr ResizeBridgeFBO (int theWinSizeX,
                                  int theWinSizeY)
  {
    Handle(WNT_Window) aWNTWindow = Handle(WNT_Window)::DownCast (myView()->Window());
    aWNTWindow->SetPos (0, 0, theWinSizeX, theWinSizeY);
    myView()->MustBeResized();
    myView()->Invalidate();
    return System::IntPtr(Handle(D3DHost_View)::DownCast (myView()->View())->D3dColorSurface());
  }

  /// <summary>
  /// Make dump of current view to file
  /// </summary>
  /// <param name="theFileName">Name of dump file</param>
  bool Dump (const char* theFileName)
  {
    if (myView().IsNull())
    {
      return false;
    }
    myView()->Redraw();
    return myView()->Dump (theFileName) != Standard_False;
  }

  /// <summary>
  ///Redraw view
  /// </summary>
  void RedrawView()
  {
    if (!myView().IsNull())
    {
      myView()->Redraw();
    }
  }

  /// <summary>
  ///Update view
  /// </summary>
  void UpdateView(void)
  {
    if (!myView().IsNull())
    {
      myView()->MustBeResized();
    }
  }

  /// <summary>
  ///Set computed mode in false
  /// </summary>
  void SetDegenerateModeOn()
  {
    if (!myView().IsNull())
    {
      myView()->SetComputedMode (Standard_False);
    }
  }

  /// <summary>
  ///Set computed mode in true
  /// </summary>
  void SetDegenerateModeOff()
  {
    if (!myView().IsNull())
    {
      myView()->SetComputedMode (Standard_True);
    }
  }

  /// <summary>
  ///Fit all
  /// </summary>
  void WindowFitAll (int theXmin, int theYmin,
                     int theXmax, int theYmax)
  {
    if (!myView().IsNull())
    {
      myView()->WindowFitAll (theXmin, theYmin, theXmax, theYmax);
    }
  }

  /// <summary>
  ///Current place of window
  /// </summary>
  /// <param name="theZoomFactor">Current zoom</param>
  void Place (int theX, int theY, float theZoomFactor)
  {	
    Quantity_Factor aZoomFactor = theZoomFactor;
    if (!myView().IsNull())
    {
      myView()->Place (theX, theY, aZoomFactor);
    }
  }

  /// <summary>
  ///Set Zoom
  /// </summary>
  void Zoom (int theX1, int theY1, int theX2, int theY2)
  {
    if (!myView().IsNull())
    {
      myView()->Zoom (theX1, theY1, theX2, theY2);
    }
  }

  /// <summary>
  ///Set Pan
  /// </summary>
  void Pan (int theX, int theY)
  {
    if (!myView().IsNull())
    {
      myView()->Pan (theX, theY);
    }
  }

  /// <summary>
  ///Rotation
  /// </summary>
  void Rotation (int theX, int theY)
  {
    if (!myView().IsNull())
    {
      myView()->Rotation (theX, theY);
    }
  }

  /// <summary>
  ///Start rotation
  /// </summary>
  void StartRotation (int theX, int theY)
  {
    if (!myView().IsNull())
    {
      myView()->StartRotation (theX, theY);
    }
  }

  /// <summary>
  ///Select by rectangle
  /// </summary>
  void Select (int theX1, int theY1, int theX2, int theY2)
  {
    if (!myAISContext().IsNull())
    {
      myAISContext()->Select (theX1, theY1, theX2, theY2, myView());
    }
  }

  /// <summary>
  ///Select by click
  /// </summary>
  void Select()
  {
    if (!myAISContext().IsNull())
    {
      myAISContext()->Select();
    }
  }

  /// <summary>
  ///Move view
  /// </summary>
  void MoveTo (int theX, int theY)
  {
    if (!myAISContext().IsNull() && !myView().IsNull())
    {
      myAISContext()->MoveTo (theX, theY, myView());
    }
  }

  /// <summary>
  ///Select by rectangle with pressed "Shift" key
  /// </summary>
  void ShiftSelect (int theX1, int theY1, int theX2, int theY2)
  {
    if (!myAISContext().IsNull() && !myView().IsNull())
    {
      myAISContext()->ShiftSelect (theX1, theY1, theX2, theY2, myView());
    }
  }

  /// <summary>
  ///Select by "Shift" key
  /// </summary>
  void ShiftSelect()
  {
    if (!myAISContext().IsNull())
    {
      myAISContext()->ShiftSelect();
    }
  }

  /// <summary>
  ///Set background color
  /// </summary>
  void BackgroundColor (int& theRed, int& theGreen, int& theBlue)
  {
    if (!myView().IsNull())
    {
      Quantity_Color aColor = myView()->BackgroundColor();
      theRed   = (int )aColor.Red()   * 255;
      theGreen = (int )aColor.Green() * 255;
      theBlue  = (int )aColor.Blue()  * 255;
    }
  }

  /// <summary>
  ///Get background color Red
  /// </summary>
  int GetBGColR()
  {
    int anRgb[3];
    BackgroundColor (anRgb[0], anRgb[1], anRgb[2]);
    return anRgb[0];
  }

  /// <summary>
  ///Get background color Green
  /// </summary>
  int GetBGColG()
  {
    int anRgb[3];
    BackgroundColor (anRgb[0], anRgb[1], anRgb[2]);
    return anRgb[1];
  }

  /// <summary>
  ///Get background color Blue
  /// </summary>
  int GetBGColB()
  {
    int anRgb[3];
    BackgroundColor (anRgb[0], anRgb[1], anRgb[2]);
    return anRgb[2];
  }

  /// <summary>
  ///Update current viewer
  /// </summary>
  void UpdateCurrentViewer()
  {
    if (!myAISContext().IsNull())
    {
      myAISContext()->UpdateCurrentViewer();
    }
  }

  /// <summary>
  ///Front side
  /// </summary>
  void FrontView()
  {
    if (!myView().IsNull())
    {
      myView()->SetProj (V3d_Xpos);
    }
  }

  /// <summary>
  ///Top side
  /// </summary>
  void TopView()
  {
    if (!myView().IsNull())
    {
      myView()->SetProj (V3d_Zpos);
    }
  }

  /// <summary>
  ///Left side
  /// </summary>
  void LeftView()
  {
    if (!myView().IsNull())
    {
      myView()->SetProj (V3d_Ypos);
    }
  }

  /// <summary>
  ///Back side
  /// </summary>
  void BackView()
  {
    if (!myView().IsNull())
    {
      myView()->SetProj (V3d_Xneg);
    }
  }

  /// <summary>
  ///Right side
  /// </summary>
  void RightView()
  {
    if (!myView().IsNull())
    {
      myView()->SetProj (V3d_Yneg);
    }
  }

  /// <summary>
  ///Bottom side
  /// </summary>
  void BottomView()
  {
    if (!myView().IsNull())
    {
      myView()->SetProj (V3d_Zneg);
    }
  }

  /// <summary>
  ///Axo side
  /// </summary>
  void AxoView()
  {
    if (!myView().IsNull())
    {
      myView()->SetProj (V3d_XposYnegZpos);
    }
  }

  /// <summary>
  ///Scale
  /// </summary>
  float Scale()
  {
    return myView().IsNull()
         ? -1.0f
         : float(myView()->Scale());
  }

  /// <summary>
  ///Zoom in all view
  /// </summary>
  void ZoomAllView()
  {
    if (!myView().IsNull())
    {
      myView()->FitAll();
      myView()->ZFitAll();
    }
  }

  /// <summary>
  ///Reset view
  /// </summary>
  void Reset()
  {
    if (!myView().IsNull())
    {
      myView()->Reset();
    }
  }

  /// <summary>
  ///Set display mode of objects
  /// </summary>
  /// <param name="theMode">Set current mode</param>
  void SetDisplayMode (int theMode)
  {
    if (myAISContext().IsNull())
    {
      return;
    }

    AIS_DisplayMode aCurrentMode = theMode == 0
                                 ? AIS_WireFrame
                                 : AIS_Shaded;
    if (myAISContext()->NbCurrents() == 0
     || myAISContext()->NbSelected() == 0)
    {
       myAISContext()->SetDisplayMode (aCurrentMode);
    }
    else
    {
       for (myAISContext()->InitCurrent(); myAISContext()->MoreCurrent(); myAISContext()->NextCurrent())
       {
         myAISContext()->SetDisplayMode (myAISContext()->Current(), theMode, Standard_False);
       }
    }
    myAISContext()->UpdateCurrentViewer();
  }

  /// <summary>
  ///Set color
  /// </summary>
  void SetColor (int theR, int theG, int theB)
  {
    if (myAISContext().IsNull())
    {
      return;
    }

    Quantity_Color aCol (theR / 255.0, theG / 255.0, theB / 255.0, Quantity_TOC_RGB);
    for (; myAISContext()->MoreCurrent(); myAISContext()->NextCurrent())
    {
      myAISContext()->SetColor (myAISContext()->Current(), aCol.Name());
    }
  }

  /// <summary>
  ///Get object color red
  /// </summary>
  int GetObjColR()
  {
    int anRgb[3];
    ObjectColor (anRgb[0], anRgb[1], anRgb[2]);
    return anRgb[0];
  }

  /// <summary>
  ///Get object color green
  /// </summary>
  int GetObjColG()
  {
    int anRgb[3];
    ObjectColor (anRgb[0], anRgb[1], anRgb[2]);
    return anRgb[1];
  }

  /// <summary>
  ///Get object color blue
  /// </summary>
  int GetObjColB()
  {
    int anRgb[3];
    ObjectColor (anRgb[0], anRgb[1], anRgb[2]);
    return anRgb[2];
  }

  /// <summary>
  ///Get object color R/G/B
  /// </summary>
  void ObjectColor (int& theRed, int& theGreen, int& theBlue)
  {
    if (myAISContext().IsNull())
    {
      return;
    }

    theRed   = 255;
    theGreen = 255;
    theBlue  = 255;
    myAISContext()->InitCurrent();
    if (!myAISContext()->MoreCurrent())
    {
      return;
    }

    Handle(AIS_InteractiveObject) aCurrent = myAISContext()->Current();
    if (aCurrent->HasColor())
    {
      Quantity_Color anObjCol = myAISContext()->Color (myAISContext()->Current());
      theRed   = int(anObjCol.Red()   * 255.0);
      theGreen = int(anObjCol.Green() * 255.0);
      theBlue  = int(anObjCol.Blue()  * 255.0);
    }
  }

  /// <summary>
  ///Set background color R/G/B
  /// </summary>
  void SetBackgroundColor (int theRed, int theGreen, int theBlue)
  {
    if (!myView().IsNull())
    {
      myView()->SetBackgroundColor (Quantity_TOC_RGB, theRed / 255.0, theGreen / 255.0, theBlue / 255.0);
    }
  }

  /// <summary>
  ///Erase objects
  /// </summary>
  void EraseObjects()
  {
    if (myAISContext().IsNull())
    {
      return;
    }

    myAISContext()->EraseSelected (Standard_True);
    myAISContext()->ClearCurrents();
  }

  /// <summary>
  ///Get version
  /// </summary>
  float GetOCCVersion()
  {
    return (float )OCC_VERSION;
  }

  /// <summary>
  ///set material
  /// </summary>
  void SetMaterial (int theMaterial)
  {
    if (myAISContext().IsNull())
    {
      return;
    }
    for (myAISContext()->InitCurrent(); myAISContext()->MoreCurrent(); myAISContext()->NextCurrent())
    {
      myAISContext()->SetMaterial (myAISContext()->Current(), (Graphic3d_NameOfMaterial )theMaterial);
    }
    myAISContext()->UpdateCurrentViewer();
  }

  /// <summary>
  ///set transparency
  /// </summary>
  void SetTransparency (int theTrans)
  {
    if (myAISContext().IsNull())
    {
      return;
    }
    for (myAISContext()->InitCurrent(); myAISContext()->MoreCurrent(); myAISContext()->NextSelected())
    {
      myAISContext()->SetTransparency (myAISContext()->Current(), ((Standard_Real )theTrans) / 10.0);
    }
  }

  /// <summary>
  ///Return true if object is selected
  /// </summary>
  bool IsObjectSelected()
  {
    if (myAISContext().IsNull())
    {
      return false;
    }
    myAISContext()->InitCurrent();
    return myAISContext()->MoreCurrent() != Standard_False;
  }

  /// <summary>
  ///Return display mode
  /// </summary>
  int DisplayMode()
  {
    if (myAISContext().IsNull())
    {
      return -1;
    }

    bool isOneOrMoreInShading   = false;
    bool isOneOrMoreInWireframe = false;
    for (myAISContext()->InitCurrent(); myAISContext()->MoreCurrent(); myAISContext()->NextCurrent())
    {
      if (myAISContext()->IsDisplayed (myAISContext()->Current(), AIS_Shaded))
      {
        isOneOrMoreInShading = true;
      }
      if (myAISContext()->IsDisplayed (myAISContext()->Current(), AIS_WireFrame))
      {
        isOneOrMoreInWireframe = true;
      }
    }
    if (isOneOrMoreInShading
     && isOneOrMoreInWireframe)
    {
      return 10;
    }
    else if (isOneOrMoreInShading)
    {
      return 1;
    }
    else if (isOneOrMoreInWireframe)
    {
      return 0;
    }
    return -1;
  }

  /// <summary>
  ///Set AISContext
  /// </summary>
  bool SetAISContext (OCCTProxyD3D^ theViewer)
  {
     this->myAISContext() = theViewer->GetContext();
     if (myAISContext().IsNull())
     {
       return false;
     }
    return true;
  }

  /// <summary>
  ///Get AISContext
  /// </summary>
  Handle_AIS_InteractiveContext GetContext()
  {
    return myAISContext();
  }

public:
  // ============================================
  // Import / export functionality
  // ============================================

  /// <summary>
  ///Import BRep file
  /// </summary>
  /// <param name="theFileName">Name of import file</param>
  bool ImportBrep (System::String^ theFileName)
  {
    bool  isResult  = false;
    int   aLength   = theFileName->Length;
    char* aFilename = new char[aLength + 1];
    for(int i = 0; i < aLength; ++i)
    {
      aFilename[i] = (char )theFileName->ToCharArray()[i];
    }
    aFilename[aLength] = '\0';
    isResult = ImportBrep (aFilename);
    delete[] aFilename;
    return isResult;
  }

  /// <summary>
  ///Import BRep file
  /// </summary>
  /// <param name="theFileName">Name of import file</param>
  bool ImportBrep (char* theFileName)
  {
    TopoDS_Shape aShape;
    BRep_Builder aBuilder;
    if (!BRepTools::Read (aShape, theFileName, aBuilder))
    {
      return false;
    }
    if (myAISContext()->HasOpenedContext())
    {
      myAISContext()->CloseLocalContext();
    }
    Handle(AIS_Shape) aPrs = new AIS_Shape (aShape);
    myAISContext()->SetMaterial   (aPrs, Graphic3d_NOM_GOLD);
    myAISContext()->SetDisplayMode(aPrs, AIS_Shaded, Standard_False);
    myAISContext()->Display (aPrs);
    return true;
  }

  /// <summary>
  ///Import Step file
  /// </summary>
  /// <param name="theFileName">Name of import file</param>
  bool ImportStep (char* theFileName)
  {
    STEPControl_Reader aReader;
    if (aReader.ReadFile (theFileName) != IFSelect_RetDone)
    {
      return false;
    }

    bool isFailsonly = false;
    aReader.PrintCheckLoad( isFailsonly, IFSelect_ItemsByEntity );

    int aNbRoot = aReader.NbRootsForTransfer();
    aReader.PrintCheckTransfer (isFailsonly, IFSelect_ItemsByEntity);
    for (Standard_Integer aRootIter = 1; aRootIter <= aNbRoot; ++aRootIter)
    {
      aReader.TransferRoot (aRootIter);
      int aNbShap = aReader.NbShapes();
      if (aNbShap > 0)
      {
        for (int aShapeIter = 1; aShapeIter <= aNbShap; ++aShapeIter)
        {
          myAISContext()->Display (new AIS_Shape (aReader.Shape (aShapeIter)), Standard_True);
        }
      }
    }
    return true;
  }

  /// <summary>
  ///Import Iges file
  /// </summary>
  /// <param name="theFileName">Name of import file</param>
  bool ImportIges (char* theFileName)
  {
    IGESControl_Reader aReader;
    if (aReader.ReadFile (theFileName) != IFSelect_RetDone)
    {
      return false;
    }

    aReader.TransferRoots();
    TopoDS_Shape aShape = aReader.OneShape();
    myAISContext()->Display (new AIS_Shape (aShape), Standard_False);
    myAISContext()->UpdateCurrentViewer();
    return true;
  }

  /// <summary>
  ///Export BRep file
  /// </summary>
  /// <param name="theFileName">Name of export file</param>
  bool ExportBRep (char* theFileName)
  {
    myAISContext()->InitCurrent();
    if (!myAISContext()->MoreCurrent())
    {
      return false;
    }

    Handle(AIS_Shape) anIS = Handle(AIS_Shape)::DownCast (myAISContext()->Current());
    return !anIS.IsNull()
         && BRepTools::Write (anIS->Shape(), theFileName);
  }

  /// <summary>
  ///Export Step file
  /// </summary>
  /// <param name="theFileName">Name of export file</param>
  bool ExportStep (char* theFileName)
  {
    STEPControl_StepModelType aType = STEPControl_AsIs;
    STEPControl_Writer        aWriter;
    for (myAISContext()->InitCurrent(); myAISContext()->MoreCurrent(); myAISContext()->NextCurrent())
    {
      Handle(AIS_Shape) anIS = Handle(AIS_Shape)::DownCast (myAISContext()->Current());
      if (anIS.IsNull())
      {
        return false;
      }

      TopoDS_Shape aShape = anIS->Shape();
      if (aWriter.Transfer (aShape, aType) != IFSelect_RetDone)
      {
        return false;
      }
    }
    return aWriter.Write (theFileName) == IFSelect_RetDone;
  }

  /// <summary>
  ///Export Iges file
  /// </summary>
  /// <param name="theFileName">Name of export file</param>
  bool ExportIges (char* theFileName)
  {
    IGESControl_Controller::Init();
    IGESControl_Writer aWriter (Interface_Static::CVal ("XSTEP.iges.unit"),
                                Interface_Static::IVal ("XSTEP.iges.writebrep.mode"));
    for (myAISContext()->InitCurrent(); myAISContext()->MoreCurrent(); myAISContext()->NextCurrent())
    {
      Handle(AIS_Shape) anIS = Handle(AIS_Shape)::DownCast (myAISContext()->Current());
      if (anIS.IsNull())
      {
        return false;
      }

      aWriter.AddShape (anIS->Shape());
    }

    aWriter.ComputeModel();
    return aWriter.Write (theFileName) != Standard_False;
  }

  /// <summary>
  ///Export Vrml file
  /// </summary>
  /// <param name="theFileName">Name of export file</param>
  bool ExportVrml (char* theFileName)
  {
    TopoDS_Compound aRes;
    BRep_Builder    aBuilder;
    aBuilder.MakeCompound (aRes);
    for (myAISContext()->InitCurrent(); myAISContext()->MoreCurrent(); myAISContext()->NextCurrent())
    {
      Handle(AIS_Shape) anIS = Handle(AIS_Shape)::DownCast (myAISContext()->Current());
      if (anIS.IsNull())
      {
        return false;
      }
      aBuilder.Add (aRes, anIS->Shape());
    }

    VrmlAPI_Writer aWriter;
    aWriter.Write (aRes, theFileName);
    return true;
  }

  /// <summary>
  ///Export Stl file
  /// </summary>
  /// <param name="theFileName">Name of export file</param>
  bool ExportStl (char* theFileName)
  {
    TopoDS_Compound aComp;
    BRep_Builder    aBuilder;
    aBuilder.MakeCompound (aComp);
    for (myAISContext()->InitCurrent(); myAISContext()->MoreCurrent(); myAISContext()->NextCurrent())
    {
      Handle(AIS_Shape) anIS = Handle(AIS_Shape)::DownCast (myAISContext()->Current());
      if (anIS.IsNull())
      {
        return false;
      }
      aBuilder.Add (aComp, anIS->Shape());
    }

    StlAPI_Writer aWriter;
    aWriter.Write (aComp, theFileName);
    return true;
  }

  /// <summary>
  ///Define which Import/Export function must be called
  /// </summary>
  /// <param name="theFileName">Name of Import/Export file</param>
  /// <param name="theFormat">Determines format of Import/Export file</param>
  /// <param name="theIsImport">Determines is Import or not</param>
  bool TranslateModel (System::String^ theFileName, int theFormat, bool theIsImport)
  {
    bool  isResult  = false;
    int   aLength   = theFileName->Length;
    char* aFilename = new char[aLength + 1];
    for (int aCharIter = 0; aCharIter < aLength; ++aCharIter)
    {
      aFilename[aCharIter] = (char)theFileName->ToCharArray()[aCharIter];
    }
    aFilename[aLength] = '\0';

    if (theIsImport)
    {
      switch (theFormat)
      {
        case 0: isResult = ImportBrep  (aFilename); break;
        case 1: isResult = ImportStep  (aFilename); break;
        case 2: isResult = ImportIges  (aFilename); break;
      }
    }
    else 
    {
      switch (theFormat)
      {
        case 0: isResult = ExportBRep (aFilename); break;
        case 1: isResult = ExportStep (aFilename); break;
        case 2: isResult = ExportIges (aFilename); break;
        case 3: isResult = ExportVrml (aFilename); break;
        case 4: isResult = ExportStl  (aFilename); break;
        case 5: isResult = Dump (aFilename);      break;
      }
    }
    delete[] aFilename;
    return isResult;
  }

  /// <summary>
  ///Initialize OCCTProxyD3D
  /// </summary>
  void InitOCCTProxy()
  {
    myGraphicDriver().Nullify();
    myViewer().Nullify();
    myView().Nullify();
    myAISContext().Nullify();
  }

private:

  NCollection_Haft<Handle_V3d_Viewer>             myViewer;
  NCollection_Haft<Handle_V3d_View>               myView;
  NCollection_Haft<Handle_AIS_InteractiveContext> myAISContext;
  NCollection_Haft<Handle_D3DHost_GraphicDriver>  myGraphicDriver;

};
