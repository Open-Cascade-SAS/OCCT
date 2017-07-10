// include required OCCT headers
#include <Standard_Version.hxx>
#include <Message_ProgressIndicator.hxx>
//for OCC graphic
#include <Aspect_DisplayConnection.hxx>
#include <WNT_Window.hxx>
#include <OpenGl_GraphicDriver.hxx>
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
#pragma comment(lib, "TKIGES.lib")
#pragma comment(lib, "TKSTEP.lib")
#pragma comment(lib, "TKStl.lib")
#pragma comment(lib, "TKVrml.lib")

/// <summary>
/// Proxy class encapsulating calls to OCCT C++ classes within 
/// C++/CLI class visible from .Net (CSharp)
/// </summary>
public ref class OCCTProxy
{
public:
  // ============================================
  // Viewer functionality
  // ============================================

  /// <summary>
  ///Initialize a viewer
  /// </summary>
  /// <param name="theWnd">System.IntPtr that contains the window handle (HWND) of the control</param>
  bool InitViewer(System::IntPtr theWnd)
  {
    try
    {
      Handle(Aspect_DisplayConnection) aDisplayConnection;
      myGraphicDriver() = new OpenGl_GraphicDriver (aDisplayConnection);
    }
    catch (Standard_Failure)
    {
      return false;
    }

    myViewer() = new V3d_Viewer (myGraphicDriver());
    myViewer()->SetDefaultLights();
    myViewer()->SetLightOn();
    myView() = myViewer()->CreateView();
    Handle(WNT_Window) aWNTWindow = new WNT_Window (reinterpret_cast<HWND> (theWnd.ToPointer()));
    myView()->SetWindow(aWNTWindow);
    if (!aWNTWindow->IsMapped())
    {
      aWNTWindow->Map();
    }
    myAISContext() = new AIS_InteractiveContext( myViewer() );
    myAISContext()->UpdateCurrentViewer();
    myView()->Redraw();
    myView()->MustBeResized();
    return true;
  }

  /// <summary>
  /// Make dump of current view to file
  /// </summary>
  /// <param name="theFileName">Name of dump file</param>
  bool Dump(char *theFileName)
  {
    if (myView().IsNull())
    {
      return false;
    }
    myView()->Redraw();
    return myView()->Dump(theFileName) != Standard_False;
  }

  /// <summary>
  ///Redraw view
  /// </summary>
  void RedrawView(void)
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
  void SetDegenerateModeOn(void)
  {
    if (!myView().IsNull())
    {
      myView()->SetComputedMode (Standard_False);
      myView()->Redraw();
    }
  }

  /// <summary>
  ///Set computed mode in true
  /// </summary>
  void SetDegenerateModeOff(void)
  {
    if (!myView().IsNull())
    {
      myView()->SetComputedMode (Standard_True);
      myView()->Redraw();
    }
  }

  /// <summary>
  ///Fit all
  /// </summary>
  void WindowFitAll(int theXmin, int theYmin, int theXmax, int theYmax)
  {
    if (!myView().IsNull())
    {
      myView()->WindowFitAll(theXmin, theYmin, theXmax, theYmax);
    }
  }

  /// <summary>
  ///Current place of window
  /// </summary>
  /// <param name="theZoomFactor">Current zoom</param>
  void Place(int theX, int theY, float theZoomFactor)
  {	
    Standard_Real aZoomFactor = theZoomFactor;
    if (!myView().IsNull())
    {
      myView()->Place(theX, theY, aZoomFactor);
    }
  }

  /// <summary>
  ///Set Zoom
  /// </summary>
  void Zoom(int theX1, int theY1, int theX2, int theY2)
  {
    if (!myView().IsNull())
    {
      myView()->Zoom(theX1, theY1, theX2, theY2);
    }
  }

  /// <summary>
  ///Set Pan
  /// </summary>
  void Pan(int theX, int theY)
  {
    if (!myView().IsNull())
    {
      myView()->Pan(theX, theY);
    }
  }

  /// <summary>
  ///Rotation
  /// </summary>
  void Rotation(int theX, int theY)
  {
    if (!myView().IsNull())
    {
      myView()->Rotation(theX, theY);
    }
  }

  /// <summary>
  ///Start rotation
  /// </summary>
  void StartRotation(int theX, int theY)
  {
    if (!myView().IsNull())
    {
      myView()->StartRotation(theX, theY);
    }
  }

  /// <summary>
  ///Select by rectangle
  /// </summary>
  void Select(int theX1, int theY1, int theX2, int theY2)
  {
    if (!myAISContext().IsNull())
    {
      myAISContext()->Select (theX1, theY1, theX2, theY2, myView(), Standard_True);
    }
  }

  /// <summary>
  ///Select by click
  /// </summary>
  void Select(void)
  {
    if (!myAISContext().IsNull())
    {
      myAISContext()->Select (Standard_True);
    }
  }

  /// <summary>
  ///Move view
  /// </summary>
  void MoveTo(int theX, int theY)
  {
    if ((!myAISContext().IsNull()) && (!myView().IsNull()))
    {
      myAISContext()->MoveTo (theX, theY, myView(), Standard_True);
    }
  }

  /// <summary>
  ///Select by rectangle with pressed "Shift" key
  /// </summary>
  void ShiftSelect(int theX1, int theY1, int theX2, int theY2)
  {
    if ((!myAISContext().IsNull()) && (!myView().IsNull()))
    {
      myAISContext()->ShiftSelect (theX1, theY1, theX2, theY2, myView(), Standard_True);
    }
  }

  /// <summary>
  ///Select by "Shift" key
  /// </summary>
  void ShiftSelect(void)
  {
    if (!myAISContext().IsNull())
    {
      myAISContext()->ShiftSelect (Standard_True);
    }
  }

  /// <summary>
  ///Set background color
  /// </summary>
  void BackgroundColor(int& theRed, int& theGreen, int& theBlue)
  {
    Standard_Real R1;
    Standard_Real G1;
    Standard_Real B1;
    if (!myView().IsNull())
    {
      myView()->BackgroundColor(Quantity_TOC_RGB,R1,G1,B1);
    }
    theRed = (int)R1*255;
    theGreen = (int)G1*255;
    theBlue = (int)B1*255;
  }

  /// <summary>
  ///Get background color Red
  /// </summary>
  int GetBGColR(void)
  {
    int aRed, aGreen, aBlue;
    BackgroundColor(aRed, aGreen, aBlue);
    return aRed;
  }

  /// <summary>
  ///Get background color Green
  /// </summary>
  int GetBGColG(void)
  {
    int aRed, aGreen, aBlue;
    BackgroundColor(aRed, aGreen, aBlue);
    return aGreen;
  }

  /// <summary>
  ///Get background color Blue
  /// </summary>
  int GetBGColB(void)
  {
    int aRed, aGreen, aBlue;
    BackgroundColor(aRed, aGreen, aBlue);
    return aBlue;
  }

  /// <summary>
  ///Update current viewer
  /// </summary>
  void UpdateCurrentViewer(void)
  {
    if (!myAISContext().IsNull())
    {
      myAISContext()->UpdateCurrentViewer();
    }
  }

  /// <summary>
  ///Front side
  /// </summary>
  void FrontView(void)
  {
    if (!myView().IsNull())
    {
      myView()->SetProj(V3d_Yneg);
    }
  }

  /// <summary>
  ///Top side
  /// </summary>
  void TopView(void)
  {
    if (!myView().IsNull())
    {
      myView()->SetProj(V3d_Zpos);
    }
  }

  /// <summary>
  ///Left side
  /// </summary>
  void LeftView(void)
  {
    if (!myView().IsNull())
    {
      myView()->SetProj(V3d_Xneg);
    }
  }

  /// <summary>
  ///Back side
  /// </summary>
  void BackView(void)
  {
    if (!myView().IsNull())
    {
      myView()->SetProj(V3d_Ypos);
    }
  }

  /// <summary>
  ///Right side
  /// </summary>
  void RightView(void)
  {
    if (!myView().IsNull())
    {
      myView()->SetProj(V3d_Xpos);
    }
  }

  /// <summary>
  ///Bottom side
  /// </summary>
  void BottomView(void)
  {
    if (!myView().IsNull())
    {
      myView()->SetProj(V3d_Zneg);
    }
  }

  /// <summary>
  ///Axo side
  /// </summary>
  void AxoView(void)
  {
    if (!myView().IsNull())
    {
      myView()->SetProj(V3d_XposYnegZpos);
    }
  }

  /// <summary>
  ///Scale
  /// </summary>
  float Scale(void)
  {
    if (myView().IsNull())
    {
      return -1;
    }
    else
    {
      return (float)myView()->Scale();
    }
  }

  /// <summary>
  ///Zoom in all view
  /// </summary>
  void ZoomAllView(void)
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
  void Reset(void)
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
  void SetDisplayMode(int theMode)
  {
    if (myAISContext().IsNull())
    {
      return;
    }
    AIS_DisplayMode aCurrentMode;
    if (theMode == 0)
    {
      aCurrentMode=AIS_WireFrame;
    }
    else
    {
      aCurrentMode=AIS_Shaded;
    }

    if(myAISContext()->NbSelected()==0)
    {
       myAISContext()->SetDisplayMode (aCurrentMode, Standard_False);
    }
    else 
    {
       for(myAISContext()->InitSelected(); myAISContext()->MoreSelected(); myAISContext()->NextSelected())
       {
         myAISContext()->SetDisplayMode (myAISContext()->SelectedInteractive(), theMode, Standard_False);
       }
    }
    myAISContext()->UpdateCurrentViewer();
  }

  /// <summary>
  ///Set color
  /// </summary>
  void SetColor(int theR, int theG, int theB)
  {
    if (myAISContext().IsNull())
    {
      return;
    }
    Quantity_Color aCol =  Quantity_Color(theR/255.,theG/255.,theB/255.,Quantity_TOC_RGB);
    for (; myAISContext()->MoreSelected(); myAISContext()->NextSelected())
    {
      myAISContext()->SetColor (myAISContext()->SelectedInteractive(), aCol, Standard_False);
    }
    myAISContext()->UpdateCurrentViewer();
  }

  /// <summary>
  ///Get object color red
  /// </summary>
  int GetObjColR(void)
  {
    int aRed, aGreen, aBlue;
    ObjectColor(aRed, aGreen, aBlue);
    return aRed;
  }

  /// <summary>
  ///Get object color green
  /// </summary>
  int GetObjColG(void)
  {
    int aRed, aGreen, aBlue;
    ObjectColor(aRed, aGreen, aBlue);
    return aGreen;
  }

  /// <summary>
  ///Get object color R/G/B
  /// </summary>
  void ObjectColor(int& theRed, int& theGreen, int& theBlue)
  {
    if (myAISContext().IsNull())
    {
      return;
    }
    theRed=255;
    theGreen=255;
    theBlue=255;
    Handle(AIS_InteractiveObject) aCurrent ;
    myAISContext()->InitSelected();
    if (!myAISContext()->MoreSelected())
    {
      return;
    }
    aCurrent = myAISContext()->SelectedInteractive();
    if ( aCurrent->HasColor () ) 
    {
      Quantity_Color anObjCol;
      myAISContext()->Color (aCurrent, anObjCol);
      Standard_Real r1, r2, r3;
      anObjCol.Values(r1, r2, r3, Quantity_TOC_RGB);
      theRed=(int)r1*255;
      theGreen=(int)r2*255;
      theBlue=(int)r3*255;
    }
  }

  /// <summary>
  ///Get object color blue
  /// </summary>
  int GetObjColB(void)
  {
    int aRed, aGreen, aBlue;
    ObjectColor(aRed, aGreen, aBlue);
    return aBlue;
  }

  /// <summary>
  ///Set background color R/G/B
  /// </summary>
  void SetBackgroundColor(int theRed, int theGreen, int theBlue)
  {
    if (!myView().IsNull())
    {
      myView()->SetBackgroundColor(Quantity_TOC_RGB, theRed/255.,theGreen/255.,theBlue/255.);
    }
  }

  /// <summary>
  ///Erase objects
  /// </summary>
  void EraseObjects(void)
  {
    if (myAISContext().IsNull())
    {
      return;
    }

    myAISContext()->EraseSelected (Standard_False);
    myAISContext()->ClearSelected (Standard_True);
  }

  /// <summary>
  ///Get version
  /// </summary>
  float GetOCCVersion(void)
  {
    return (float)OCC_VERSION;
  }

  /// <summary>
  ///set material
  /// </summary>
  void SetMaterial(int theMaterial)
  {
    if (myAISContext().IsNull())
    {
      return;
    }
    for (myAISContext()->InitSelected(); myAISContext()->MoreSelected(); myAISContext()->NextSelected())
    {
      myAISContext()->SetMaterial (myAISContext()->SelectedInteractive(), (Graphic3d_NameOfMaterial)theMaterial, Standard_False);
    }
    myAISContext()->UpdateCurrentViewer();
  }

  /// <summary>
  ///set transparency
  /// </summary>
  void SetTransparency(int theTrans)
  {
    if (myAISContext().IsNull())
    {
      return;
    }
    for( myAISContext()->InitSelected(); myAISContext()->MoreSelected(); myAISContext()->NextSelected() )
    {
      myAISContext()->SetTransparency (myAISContext()->SelectedInteractive(), ((Standard_Real)theTrans) / 10.0, Standard_False);
    }
    myAISContext()->UpdateCurrentViewer();
  }

  /// <summary>
  ///Return true if object is selected
  /// </summary>
  bool IsObjectSelected(void)
  {
    if (myAISContext().IsNull())
    {
      return false;
    }
    myAISContext()->InitSelected();
    return myAISContext()->MoreSelected() != Standard_False;
  }

  /// <summary>
  ///Return display mode
  /// </summary>
  int DisplayMode(void)
  {
    if (myAISContext().IsNull())
    {
      return -1;
    }
    int aMode = -1;
    bool OneOrMoreInShading = false;
    bool OneOrMoreInWireframe = false;
    for (myAISContext()->InitSelected(); myAISContext()->MoreSelected(); myAISContext()->NextSelected())
    {
      if ( myAISContext()->IsDisplayed( myAISContext()->SelectedInteractive(), 1 ) )
      {
        OneOrMoreInShading = true;
      }
      if ( myAISContext()->IsDisplayed( myAISContext()->SelectedInteractive(), 0 ) )
      {
        OneOrMoreInWireframe = true;
      }
    }
    if (OneOrMoreInShading && OneOrMoreInWireframe)
    {
      aMode=10;
    }
    else if(OneOrMoreInShading)
    {
      aMode=1;
    }
    else if (OneOrMoreInWireframe)
    {
      aMode=0;
    }

    return aMode;
  }

  /// <summary>
  ///Create new view
  /// </summary>
  /// <param name="theWnd">System.IntPtr that contains the window handle (HWND) of the control</param>
  void CreateNewView(System::IntPtr theWnd)
  {
    if (myAISContext().IsNull())
    {
      return;
    }
    myView() = myAISContext()->CurrentViewer()->CreateView();
    if (myGraphicDriver().IsNull())
    {
      myGraphicDriver() = new OpenGl_GraphicDriver (Handle(Aspect_DisplayConnection)());
    }
    Handle(WNT_Window) aWNTWindow = new WNT_Window (reinterpret_cast<HWND> (theWnd.ToPointer()));
    myView()->SetWindow(aWNTWindow);
    Standard_Integer w=100, h=100;
    aWNTWindow->Size(w,h);
    if (!aWNTWindow->IsMapped())
    {
      aWNTWindow->Map();
    }
  }

  /// <summary>
  ///Set AISContext
  /// </summary>
  bool SetAISContext(OCCTProxy^ theViewer)
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
  Handle(AIS_InteractiveContext) GetContext(void)
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
  bool ImportBrep(System::String^ theFileName)
  {
    bool isResult = false;
    int aLength = theFileName->Length;
    char* aFilename = new char[aLength+1];
    for(int i = 0; i<aLength; i++)
    {
      aFilename[i] = (char)theFileName->ToCharArray()[i];
    }
    aFilename[aLength] = '\0';
    isResult = ImportBrep(aFilename);
    return isResult;
  }

  /// <summary>
  ///Import BRep file
  /// </summary>
  /// <param name="theFileName">Name of import file</param>
  bool ImportBrep(char* theFileName)
  {
    Standard_CString aFileName = (Standard_CString) theFileName;
    TopoDS_Shape aShape;
    BRep_Builder aBuilder;
    Standard_Boolean isResult = BRepTools::Read(aShape,aFileName,aBuilder);
    if (!isResult)
    {
      return false;
    }

    myAISContext()->Display (new AIS_Shape (aShape), Standard_True);
    return true;
  }

  /// <summary>
  ///Import Step file
  /// </summary>
  /// <param name="theFileName">Name of import file</param>
  bool ImportStep(char* theFileName)
  {
    Standard_CString aFileName = (Standard_CString) theFileName;
    STEPControl_Reader aReader;
    IFSelect_ReturnStatus aStatus = aReader.ReadFile(aFileName);
    if ( aStatus == IFSelect_RetDone )
    {
      bool isFailsonly = false;
      aReader.PrintCheckLoad( isFailsonly, IFSelect_ItemsByEntity );

      int aNbRoot = aReader.NbRootsForTransfer();
      aReader.PrintCheckTransfer( isFailsonly, IFSelect_ItemsByEntity );
      for ( Standard_Integer n = 1; n <= aNbRoot; n++ )
      {
        Standard_Boolean ok = aReader.TransferRoot( n );
        int aNbShap = aReader.NbShapes();
        if ( aNbShap > 0 )
        {
          for ( int i = 1; i <= aNbShap; i++ )
          {
            TopoDS_Shape aShape = aReader.Shape( i );
            myAISContext()->Display (new AIS_Shape (aShape), Standard_False);
          }
          myAISContext()->UpdateCurrentViewer();
        }
      }
    }
    else
    {
      return false;
    }

    return true;
  }

  /// <summary>
  ///Import Iges file
  /// </summary>
  /// <param name="theFileName">Name of import file</param>
  bool ImportIges(char* theFileName)
  {
    Standard_CString aFileName = (Standard_CString) theFileName;
    IGESControl_Reader aReader;
    int aStatus = aReader.ReadFile( aFileName );

    if ( aStatus == IFSelect_RetDone )
    {
      aReader.TransferRoots();
      TopoDS_Shape aShape = aReader.OneShape();
      myAISContext()->Display (new AIS_Shape (aShape), Standard_False);
    }
    else
    {
      return false;
    }

    myAISContext()->UpdateCurrentViewer();
    return true;
  }

  /// <summary>
  ///Export BRep file
  /// </summary>
  /// <param name="theFileName">Name of export file</param>
  bool ExportBRep(char* theFileName)
  {
    myAISContext()->InitSelected();
    if (!myAISContext()->MoreSelected())
    {
      return false;
    }

    Handle(AIS_InteractiveObject) anIO = myAISContext()->SelectedInteractive();
    Handle(AIS_Shape) anIS = Handle(AIS_Shape)::DownCast(anIO);
    return BRepTools::Write (anIS->Shape(), (Standard_CString)theFileName) != Standard_False;
  }

  /// <summary>
  ///Export Step file
  /// </summary>
  /// <param name="theFileName">Name of export file</param>
  bool ExportStep(char* theFileName)
  {
    STEPControl_StepModelType aType = STEPControl_AsIs;
    IFSelect_ReturnStatus aStatus;
    STEPControl_Writer aWriter;
    for ( myAISContext()->InitSelected(); myAISContext()->MoreSelected(); myAISContext()->NextSelected() )
    {
      Handle(AIS_InteractiveObject) anIO = myAISContext()->SelectedInteractive();
      Handle(AIS_Shape) anIS=Handle(AIS_Shape)::DownCast(anIO);
      TopoDS_Shape aShape = anIS->Shape();
      aStatus = aWriter.Transfer( aShape , aType );
      if ( aStatus != IFSelect_RetDone )
      {
        return false;
      }
    }

    aStatus = aWriter.Write( (Standard_CString)theFileName );
    if ( aStatus != IFSelect_RetDone )
    {
      return false;
    }

    return true;
  }

  /// <summary>
  ///Export Iges file
  /// </summary>
  /// <param name="theFileName">Name of export file</param>
  bool ExportIges(char* theFileName)
  {
    IGESControl_Controller::Init();
    IGESControl_Writer aWriter( Interface_Static::CVal( "XSTEP.iges.unit" ),
      Interface_Static::IVal( "XSTEP.iges.writebrep.mode" ) );

    for ( myAISContext()->InitSelected(); myAISContext()->MoreSelected(); myAISContext()->NextSelected() )
    {
      Handle(AIS_InteractiveObject) anIO = myAISContext()->SelectedInteractive();
      Handle(AIS_Shape) anIS=Handle(AIS_Shape)::DownCast(anIO);
      TopoDS_Shape aShape = anIS->Shape();
      aWriter.AddShape ( aShape );
    }

    aWriter.ComputeModel();
    return aWriter.Write( (Standard_CString)theFileName) != Standard_False;
  }

  /// <summary>
  ///Export Vrml file
  /// </summary>
  /// <param name="theFileName">Name of export file</param>
  bool ExportVrml(char* theFileName)
  {
    TopoDS_Compound aRes;
    BRep_Builder aBuilder;
    aBuilder.MakeCompound( aRes );

    for ( myAISContext()->InitSelected(); myAISContext()->MoreSelected(); myAISContext()->NextSelected() )
    {
      Handle(AIS_InteractiveObject) anIO = myAISContext()->SelectedInteractive();
      Handle(AIS_Shape) anIS=Handle(AIS_Shape)::DownCast(anIO);
      TopoDS_Shape aShape = anIS->Shape();
      if ( aShape.IsNull() )
      {
        return false;
      }

      aBuilder.Add( aRes, aShape );
    }

    VrmlAPI_Writer aWriter;
    aWriter.Write( aRes, (Standard_CString)theFileName );

    return true;
  }

  /// <summary>
  ///Export Stl file
  /// </summary>
  /// <param name="theFileName">Name of export file</param>
  bool ExportStl(char* theFileName)
  {
    TopoDS_Compound aComp;
    BRep_Builder aBuilder;
    aBuilder.MakeCompound( aComp );

    for ( myAISContext()->InitSelected(); myAISContext()->MoreSelected(); myAISContext()->NextSelected() )
    {
      Handle(AIS_InteractiveObject) anIO = myAISContext()->SelectedInteractive();
      Handle(AIS_Shape) anIS=Handle(AIS_Shape)::DownCast(anIO);
      TopoDS_Shape aShape = anIS->Shape();
      if ( aShape.IsNull() ) 
      {
        return false;
      }
      aBuilder.Add( aComp, aShape );
    }

    StlAPI_Writer aWriter;
    aWriter.Write( aComp, (Standard_CString)theFileName );
    return true;
  }

  /// <summary>
  ///Define which Import/Export function must be called
  /// </summary>
  /// <param name="theFileName">Name of Import/Export file</param>
  /// <param name="theFormat">Determines format of Import/Export file</param>
  /// <param name="theIsImport">Determines is Import or not</param>
  bool TranslateModel(System::String^ theFileName, int theFormat, bool theIsImport)
  {
    bool isResult;
    int aLength = theFileName->Length;
    char* aFilename = new char[aLength+1];
    for(int i = 0; i<aLength; i++)
    {
      aFilename[i] = (char)theFileName->ToCharArray()[i];
    }
    aFilename[aLength] = '\0';

    if (theIsImport)
    {
      switch(theFormat)
      {
      case 0:
        isResult = ImportBrep(aFilename);
        break;
      case 1:
        isResult = ImportStep(aFilename);
        break;
      case 2:
        isResult = ImportIges(aFilename);
        break;
      default:
        isResult = false;
      }
    }
    else 
    {
      switch(theFormat)
      {
      case 0:
        isResult = ExportBRep(aFilename);
        break;
      case 1:
        isResult = ExportStep(aFilename);
        break;
      case 2:
        isResult = ExportIges(aFilename);
        break;
      case 3:
        isResult = ExportVrml(aFilename);
        break;
      case 4:
        isResult = ExportStl(aFilename);
        break;
      case 5:
        isResult = Dump(aFilename);
        break;
      default:
        isResult = false;
      }
    }
    return isResult;
  }

  /// <summary>
  ///Initialize OCCTProxy
  /// </summary>
  void InitOCCTProxy(void)
  {
    myGraphicDriver()=NULL;
    myViewer()=NULL;
    myView()=NULL;
    myAISContext()=NULL;
  }

private:
  // fields
  NCollection_Haft<Handle(V3d_Viewer)> myViewer;
  NCollection_Haft<Handle(V3d_View)> myView;
  NCollection_Haft<Handle(AIS_InteractiveContext)> myAISContext;
  NCollection_Haft<Handle(OpenGl_GraphicDriver)> myGraphicDriver;
};
