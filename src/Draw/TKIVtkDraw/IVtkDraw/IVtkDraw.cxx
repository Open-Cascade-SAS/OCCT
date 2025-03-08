// Created on: 2012-02-03
//
// Copyright (c) 2012-2014 OPEN CASCADE SAS
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

#ifdef _WIN32
  #include <windows.h>
  #include <WNT_WClass.hxx>
  #include <WNT_Window.hxx>
#endif

#include <Draw.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_PluginMacro.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopoDS_Shape.hxx>
#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#include <DBRep.hxx>
#include <NCollection_DoubleMap.hxx>
#include <NCollection_List.hxx>
#include <NCollection_DataMap.hxx>
#include <TopTools_DataMapOfIntegerShape.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <V3d.hxx>
#include <V3d_TypeOfOrientation.hxx>
#include <Aspect_DisplayConnection.hxx>

#include <IVtk_Types.hxx>
#include <IVtkVTK_ShapeData.hxx>
#include <IVtkOCC_Shape.hxx>
#include <IVtkOCC_ShapeMesher.hxx>
#include <IVtkTools_ShapeDataSource.hxx>
#include <IVtkTools_ShapeObject.hxx>
#include <IVtkTools_SubPolyDataFilter.hxx>
#include <IVtkTools_DisplayModeFilter.hxx>
#include <IVtkTools_ShapePicker.hxx>

#include <IVtkDraw.hxx>
#include <IVtkDraw_HighlightAndSelectionPipeline.hxx>
#include <IVtkDraw_Interactor.hxx>

// prevent disabling some MSVC warning messages by VTK headers
#include <Standard_WarningsDisable.hxx>
#include <vtkAlgorithmOutput.h>
#include <vtkAppendPolyData.h>
#include <vtkBMPWriter.h>
#include <vtkCamera.h>
#include <vtkCellData.h>
#include <vtkCommand.h>
#include <vtkGeometryFilter.h>
#include <vtkIdTypeArray.h>
#include <vtkImageResize.h>
#include <vtkImageWriter.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkJPEGWriter.h>
#include <vtkMatrix4x4.h>
#include <vtkPNGWriter.h>
#include <vtkPNMWriter.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>
#include <vtkTIFFWriter.h>
#include <vtkWindowToImageFilter.h>
#ifndef _WIN32
  #include <X11/X.h>
  #include <X11/Xlib.h>
  #include <X11/Xutil.h>
  #include <GL/glx.h>
  #include <Xw_Window.hxx>
  #include <vtkXRenderWindowInteractor.h>
  #include <vtkXOpenGLRenderWindow.h>
#endif
#include <Standard_WarningsRestore.hxx>

#if (VTK_MAJOR_VERSION > 8) || (VTK_MAJOR_VERSION == 8 && VTK_MINOR_VERSION >= 1)
  #define HAVE_VTK_SRGB
#endif

//================================================================
// TYPE DEFINITIONS
//================================================================
namespace
{
struct VtkPointerHasher
{
  std::size_t operator()(const vtkSmartPointer<vtkActor>& thePointer) const
  {
    return std::hash<vtkActor*>{}(thePointer.Get());
  }

  bool operator()(const vtkSmartPointer<vtkActor>& thePointer1,
                  const vtkSmartPointer<vtkActor>& thePointer2) const
  {
    return thePointer1 == thePointer2;
  }
};
} // namespace

typedef NCollection_DoubleMap<TopoDS_Shape, TCollection_AsciiString> DoubleMapOfShapesAndNames;
typedef NCollection_DoubleMap<vtkSmartPointer<vtkActor>, TCollection_AsciiString, VtkPointerHasher>
  DoubleMapOfActorsAndNames;

typedef IVtkDraw_HighlightAndSelectionPipeline PipelinePtr;

//================================================================
// GLOBAL VARIABLES
//================================================================

Standard_IMPORT Standard_Boolean Draw_VirtualWindows;

static Handle(Aspect_DisplayConnection)& GetDisplayConnection()
{
  static Handle(Aspect_DisplayConnection) aDisplayConnection;
  return aDisplayConnection;
}

static void SetDisplayConnection(const Handle(Aspect_DisplayConnection)& theDisplayConnection)
{
  GetDisplayConnection() = theDisplayConnection;
}

static DoubleMapOfShapesAndNames& GetMapOfShapes()
{
  static DoubleMapOfShapesAndNames aMap;
  return aMap;
}

static DoubleMapOfActorsAndNames& GetMapOfActors()
{
  static DoubleMapOfActorsAndNames aMap;
  return aMap;
}

static vtkSmartPointer<vtkRenderer>& GetRenderer()
{
  static vtkSmartPointer<vtkRenderer> aRenderer;
  return aRenderer;
}

static Handle(ShapePipelineMap)& GetPipelines()
{
  static Handle(ShapePipelineMap) aPLMap;
  if (aPLMap.IsNull())
  {
    aPLMap = new ShapePipelineMap();
  }

  return aPLMap;
}

static Handle(PipelinePtr) GetPipeline(const IVtk_IdType& theShapeID)
{
  Handle(PipelinePtr) aPtr;
  GetPipelines()->Find(theShapeID, aPtr);
  return aPtr;
}

//! Get VTK render pipeline with shape ID got from actor.
static Handle(PipelinePtr) PipelineByActor(const vtkSmartPointer<vtkActor>& theActor)
{
  IVtk_IdType aShapeID = IVtkTools_ShapeObject::GetShapeSource(theActor)->GetShape()->GetId();
  return GetPipeline(aShapeID);
}

//! Get VTK render pipeline with actor that has the input name.
static Handle(PipelinePtr) PipelineByActorName(const TCollection_AsciiString& theName)
{
  const vtkSmartPointer<vtkActor>& anActor = GetMapOfActors().Find2(theName);
  return PipelineByActor(anActor);
}

//! Create global presentation attributes.
static Handle(Prs3d_Drawer) createDefaultDrawer()
{
  Handle(Prs3d_Drawer) aGlobalDrawer = new Prs3d_Drawer();
  aGlobalDrawer->SetupOwnDefaults();
  aGlobalDrawer->SetTypeOfDeflection(Aspect_TOD_RELATIVE);
  aGlobalDrawer->SetDeviationCoefficient(0.0001);
  return aGlobalDrawer;
}

//! Get global presentation attributes (analog of AIS_InteractiveContext::DefaultDrawer()).
static const Handle(Prs3d_Drawer)& GetDefaultDrawer()
{
  static Handle(Prs3d_Drawer) aGlobalDrawer = createDefaultDrawer();
  return aGlobalDrawer;
}

#ifdef _WIN32

static Handle(WNT_Window)& GetWindow()
{
  static Handle(WNT_Window) aWindow;
  return aWindow;
}

#else

static Handle(Xw_Window)& GetWindow()
{
  static Handle(Xw_Window) aXWWin;
  return aXWWin;
}

#endif

static vtkSmartPointer<IVtkDraw_Interactor>& GetInteractor()
{
  static vtkSmartPointer<IVtkDraw_Interactor> anInteractor;
  return anInteractor;
}

static vtkSmartPointer<IVtkTools_ShapePicker>& GetPicker()
{
  static vtkSmartPointer<IVtkTools_ShapePicker> aPicker;
  return aPicker;
}

//! Generate identical number for shape
Standard_Integer GenerateId()
{
  static unsigned int aShapesCounter = (unsigned int)-1;
  return (Standard_Integer)++aShapesCounter;
}

//=================================================================================================

const Handle(WNT_WClass)& IVtkDraw::WClass()
{
  static Handle(WNT_WClass) aWindowClass;
#ifdef _WIN32
  if (aWindowClass.IsNull())
  {
    aWindowClass = new WNT_WClass("GWVTK_Class",
                                  NULL,
                                  CS_VREDRAW | CS_HREDRAW,
                                  0,
                                  0,
                                  ::LoadCursorW(NULL, IDC_ARROW));
  }
#endif
  return aWindowClass;
}

//=================================================================================================

void IVtkDraw::ViewerInit(const IVtkWinParams& theParams)
{
  Standard_Integer aPxLeft = 0, aPxTop = 460;
  Standard_Integer aPxWidth = 409, aPxHeight = 409;
  if (theParams.TopLeft.x() != 0)
  {
    aPxLeft = theParams.TopLeft.x();
  }
  if (theParams.TopLeft.y() != 0)
  {
    aPxTop = theParams.TopLeft.y();
  }
  if (theParams.Size.x() != 0)
  {
    aPxWidth = theParams.Size.x();
  }
  if (theParams.Size.y() != 0)
  {
    aPxHeight = theParams.Size.y();
  }

  if (!GetRenderer())
  {
    SetDisplayConnection(new Aspect_DisplayConnection());
#ifdef _WIN32
    if (GetWindow().IsNull())
    {
      GetWindow() = new WNT_Window("IVtkTest",
                                   WClass(),
                                   WS_OVERLAPPEDWINDOW,
                                   aPxLeft,
                                   aPxTop,
                                   aPxWidth,
                                   aPxHeight,
                                   Quantity_NOC_BLACK);
      GetWindow()->SetVirtual(Draw_VirtualWindows);
    }
#else

    if (GetWindow().IsNull())
    {
      GetWindow() =
        new Xw_Window(GetDisplayConnection(), "IVtkTest", aPxLeft, aPxTop, aPxWidth, aPxHeight);
      GetWindow()->SetVirtual(Draw_VirtualWindows);
    }
#endif
    // Init pipeline
    GetRenderer() = vtkSmartPointer<vtkRenderer>::New();

    vtkSmartPointer<vtkRenderWindow> aRenWin = vtkSmartPointer<vtkRenderWindow>::New();
    aRenWin->AddRenderer(GetRenderer());
    GetRenderer()->GetActiveCamera()->ParallelProjectionOn();
    aRenWin->SetSize(aPxWidth, aPxHeight);

    aRenWin->SetMultiSamples(theParams.NbMsaaSample);
    aRenWin->SetAlphaBitPlanes(1);
#ifdef HAVE_VTK_SRGB
    aRenWin->SetUseSRGBColorSpace(theParams.UseSRGBColorSpace);
#else
    if (theParams.UseSRGBColorSpace)
    {
      Message::SendWarning() << "Warning: skipped option -srgb unsupported by old VTK";
    }
#endif

#ifdef _WIN32
    aRenWin->SetWindowId((void*)GetWindow()->HWindow());
#else
    Window aWindowId = GetWindow()->XWindow();
    aRenWin->SetWindowId((void*)aWindowId);
    Display* aDisplayId = (Display*)GetDisplayConnection()->GetDisplayAspect();
    aRenWin->SetDisplayId(aDisplayId);

    // Setup XWindow
    XSynchronize(aDisplayId, 1);
    GetWindow()->Map();

    // X11 : For keyboard on SUN
    XWMHints wmhints;
    wmhints.flags = InputHint;
    wmhints.input = 1;

    XSetWMHints(aDisplayId, aWindowId, &wmhints);

    XSelectInput(aDisplayId,
                 aWindowId,
                 ExposureMask | KeyPressMask | ButtonPressMask | ButtonReleaseMask
                   | StructureNotifyMask | PointerMotionMask | Button1MotionMask | Button2MotionMask
                   | Button3MotionMask);

    XSynchronize(aDisplayId, 0);

#endif

    // Init interactor
    GetInteractor() = vtkSmartPointer<IVtkDraw_Interactor>::New();
    GetInteractor()->SetRenderWindow(aRenWin);
    GetInteractor()->SetOCCWindow(GetWindow());

    vtkSmartPointer<vtkInteractorStyleTrackballCamera> aStyle =
      vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    GetInteractor()->SetInteractorStyle(aStyle);

    // Init picker
    GetPicker() = vtkSmartPointer<IVtkTools_ShapePicker>::New();
    GetPicker()->SetTolerance(0.025f);
    GetPicker()->SetRenderer(GetRenderer());

    GetInteractor()->SetShapePicker(GetPicker());
    GetInteractor()->SetPipelines(GetPipelines());
    GetInteractor()->Initialize();

    aRenWin->SetOffScreenRendering(Draw_VirtualWindows);
    aRenWin->Render();
  }

  GetWindow()->Map();
}

//=================================================================================================

static Standard_Integer VtkInit(Draw_Interpretor&,
                                Standard_Integer theNbArgs,
                                const char**     theArgVec)
{
  bool                    hasSize = false;
  IVtkDraw::IVtkWinParams aParams;
  for (Standard_Integer anArgIter = 1; anArgIter < theNbArgs; ++anArgIter)
  {
    TCollection_AsciiString anArg(theArgVec[anArgIter]);
    anArg.LowerCase();
    if (anArg == "-msaa" && anArgIter + 1 < theNbArgs)
    {
      aParams.NbMsaaSample = Draw::Atoi(theArgVec[++anArgIter]);
    }
    else if (anArg == "-srgb")
    {
      aParams.UseSRGBColorSpace = true;
      if (anArgIter + 1 < theNbArgs
          && Draw::ParseOnOff(theArgVec[anArgIter + 1], aParams.UseSRGBColorSpace))
      {
        ++anArgIter;
      }
    }
    else if (!hasSize && anArgIter + 3 < theNbArgs)
    {
      aParams.TopLeft.SetValues(Draw::Atoi(theArgVec[anArgIter + 0]),
                                Draw::Atoi(theArgVec[anArgIter + 1]));
      aParams.Size.SetValues(Draw::Atoi(theArgVec[anArgIter + 2]),
                             Draw::Atoi(theArgVec[anArgIter + 3]));
    }
    else
    {
      Message::SendFail() << "Syntax error at '" << anArg << "'";
      return 1;
    }
  }

  IVtkDraw::ViewerInit(aParams);
  return 0;
}

//=================================================================================================

static Standard_Integer VtkClose(Draw_Interpretor&, Standard_Integer theNbArgs, const char**)
{
  if (theNbArgs > 1)
  {
    Message::SendFail() << "Syntax error: wrong number of arguments";
    return 1;
  }

  if (GetWindow())
  {
    GetWindow()->Unmap();
  }

  GetWindow().Nullify();
  if (GetInteractor())
  {
    GetInteractor()->GetRenderWindow()->Finalize();
    // GetInteractor()->SetRenderWindow (NULL);
    GetInteractor()->TerminateApp();
  }

  GetInteractor() = NULL;
  GetRenderer()   = NULL;
  GetPicker()     = NULL;
  return 0;
}

//=================================================================================================

static Standard_Integer VtkRenderParams(Draw_Interpretor&,
                                        Standard_Integer theNbArgs,
                                        const char**     theArgVec)
{
  if (!GetInteractor() || !GetInteractor()->IsEnabled())
  {
    Message::SendFail() << "Syntax error: call ivtkinit before";
    return 1;
  }
  else if (theNbArgs <= 1)
  {
    Message::SendFail() << "Syntax error: wrong number of arguments";
    return 1;
  }

  for (Standard_Integer anArgIter = 1; anArgIter < theNbArgs; ++anArgIter)
  {
    TCollection_AsciiString anArg(theArgVec[anArgIter]);
    anArg.LowerCase();
    if (anArg == "-depthpeeling" && anArgIter + 1 < theNbArgs)
    {
      Standard_Integer aNbLayers = Draw::Atoi(theArgVec[++anArgIter]);
      GetRenderer()->SetUseDepthPeeling(aNbLayers > 0);
      GetRenderer()->SetMaximumNumberOfPeels(aNbLayers);
    }
    else if (anArg == "-shadows")
    {
      bool toUseShadows = true;
      if (anArgIter + 1 < theNbArgs && Draw::ParseOnOff(theArgVec[anArgIter + 1], toUseShadows))
      {
        ++anArgIter;
      }
#if (VTK_MAJOR_VERSION >= 7)
      GetRenderer()->SetUseShadows(toUseShadows);
#else
      Message::SendWarning() << "Warning: skipped option -shadows unsupported by old VTK";
#endif
    }
    else
    {
      Message::SendFail() << "Syntax error at '" << anArg << "'";
      return 1;
    }
  }

  GetInteractor()->GetRenderWindow()->Render();
  return 0;
}

//=================================================================================================

vtkActor* CreateActor(const Standard_Integer theId, const TopoDS_Shape& theShape)
{
  if (theShape.IsNull())
  {
    return NULL;
  }

  Handle(PipelinePtr) aPL = new PipelinePtr(theShape, theId, GetDefaultDrawer());
  GetPipelines()->Bind(theId, aPL);

  aPL->Actor()->GetProperty()->SetInterpolationToPhong();
  return aPL->Actor();
}

//=================================================================================================

static int VtkDefaults(Draw_Interpretor& theDi, Standard_Integer theArgsNb, const char** theArgVec)
{
  const Handle(Prs3d_Drawer)& aDefParams = GetDefaultDrawer();
  if (theArgsNb < 2)
  {
    if (aDefParams->TypeOfDeflection() == Aspect_TOD_RELATIVE)
    {
      theDi << "DeflType:           relative\n"
            << "DeviationCoeff:     " << aDefParams->DeviationCoefficient() << "\n";
    }
    else
    {
      theDi << "DeflType:           absolute\n"
            << "AbsoluteDeflection: " << aDefParams->MaximalChordialDeviation() << "\n";
    }
    theDi << "AngularDeflection:  " << (180.0 * aDefParams->DeviationAngle() / M_PI) << "\n";
    theDi << "AutoTriangulation:  " << (aDefParams->IsAutoTriangulation() ? "on" : "off") << "\n";
    return 0;
  }

  for (Standard_Integer anArgIter = 1; anArgIter < theArgsNb; ++anArgIter)
  {
    TCollection_AsciiString anArg(theArgVec[anArgIter]);
    anArg.UpperCase();
    if (anArg == "-ABSDEFL" || anArg == "-ABSOLUTEDEFLECTION" || anArg == "-DEFL"
        || anArg == "-DEFLECTION")
    {
      if (++anArgIter >= theArgsNb)
      {
        theDi << "Syntax error at " << anArg;
        return 1;
      }
      aDefParams->SetTypeOfDeflection(Aspect_TOD_ABSOLUTE);
      aDefParams->SetMaximalChordialDeviation(Draw::Atof(theArgVec[anArgIter]));
    }
    else if (anArg == "-RELDEFL" || anArg == "-RELATIVEDEFLECTION" || anArg == "-DEVCOEFF"
             || anArg == "-DEVIATIONCOEFF" || anArg == "-DEVIATIONCOEFFICIENT")
    {
      if (++anArgIter >= theArgsNb)
      {
        theDi << "Syntax error at " << anArg;
        return 1;
      }
      aDefParams->SetTypeOfDeflection(Aspect_TOD_RELATIVE);
      aDefParams->SetDeviationCoefficient(Draw::Atof(theArgVec[anArgIter]));
    }
    else if (anArg == "-ANGDEFL" || anArg == "-ANGULARDEFL" || anArg == "-ANGULARDEFLECTION")
    {
      if (++anArgIter >= theArgsNb)
      {
        theDi << "Syntax error at " << anArg;
        return 1;
      }
      aDefParams->SetDeviationAngle(M_PI * Draw::Atof(theArgVec[anArgIter]) / 180.0);
    }
    else if (anArg == "-AUTOTR" || anArg == "-AUTOTRIANG" || anArg == "-AUTOTRIANGULATION")
    {
      ++anArgIter;
      bool toTurnOn = true;
      if (anArgIter >= theArgsNb || !Draw::ParseOnOff(theArgVec[anArgIter], toTurnOn))
      {
        theDi << "Syntax error at '" << anArg << "'";
        return 1;
      }
      aDefParams->SetAutoTriangulation(toTurnOn);
    }
    else
    {
      theDi << "Syntax error: unknown argument '" << anArg << "'";
      return 1;
    }
  }

  return 0;
}

//=================================================================================================

static Standard_Integer VtkDisplay(Draw_Interpretor&,
                                   Standard_Integer theArgNum,
                                   const char**     theArgs)
{
  if (!GetInteractor() || !GetInteractor()->IsEnabled())
  {
    Message::SendFail() << "Error : call ivtkinit before";
    return 1;
  }
  if (theArgNum < 2)
  {
    Message::SendFail() << "Error : expects at least 1 argument\n";
    return 1;
  }

  TCollection_AsciiString       aName;
  TopoDS_Shape                  anOldShape, aNewShape;
  vtkSmartPointer<vtkRenderer>& aRenderer = GetRenderer();
  for (Standard_Integer anIndex = 1; anIndex < theArgNum; ++anIndex)
  {
    // Get name of shape
    aName = theArgs[anIndex];
    // Get shape from DRAW
    aNewShape = DBRep::Get(theArgs[anIndex]);

    // The shape is already in the map
    if (GetMapOfShapes().IsBound2(aName))
    {
      // Get shape from map
      anOldShape = GetMapOfShapes().Find2(aName);
      // Equal shapes
      if (anOldShape.IsEqual(aNewShape))
      {
        // Get actor from map and display it
        PipelineByActorName(aName)->AddToRenderer(aRenderer);
      }
      // Different shapes
      else
      {
        if (aNewShape.IsNull())
          continue;
        // Create actor from DRAW shape
        vtkActor* anActor = CreateActor(GenerateId(), aNewShape);
        // Remove old actor from render
        PipelineByActorName(aName)->RemoveFromRenderer(aRenderer);
        // Update maps
        GetMapOfShapes().UnBind2(aName);
        GetMapOfShapes().Bind(aNewShape, aName);
        GetMapOfActors().UnBind2(aName);
        GetMapOfActors().Bind(anActor, aName);
        // Display new actor
        PipelineByActorName(aName)->AddToRenderer(aRenderer);
        // Compute selection for displayed actors
        GetPicker()->SetSelectionMode(SM_Shape, Standard_True);
      }
    }
    // There is no shape with given name in map
    else
    {
      if (aNewShape.IsNull())
        continue;
      // Create actor from DRAW shape
      Standard_Integer          anId    = GenerateId();
      vtkSmartPointer<vtkActor> anActor = CreateActor(anId, aNewShape);
      // Update maps
      GetMapOfShapes().Bind(aNewShape, aName);
      GetMapOfActors().Bind(anActor, aName);
      // Display actor
      GetPipeline(anId)->AddToRenderer(aRenderer);

      // Compute selection for displayed actors
      GetPicker()->SetSelectionMode(SM_Shape, Standard_True);
    }
  }

  // Redraw window
  aRenderer->ResetCamera();
  GetInteractor()->GetRenderWindow()->Render();

  return 0;
}

//=================================================================================================

static Standard_Integer VtkErase(Draw_Interpretor&,
                                 Standard_Integer theArgNum,
                                 const char**     theArgs)
{
  if (!GetInteractor() || !GetInteractor()->IsEnabled())
  {
    Message::SendFail() << "Error: call ivtkinit before";
    return 1;
  }

  vtkSmartPointer<vtkRenderer> aRenderer = GetRenderer();
  if (theArgNum == 1)
  {
    // Erase all objects
    DoubleMapOfActorsAndNames::Iterator anIterator(GetMapOfActors());
    while (anIterator.More())
    {
      PipelineByActor(anIterator.Key1())->RemoveFromRenderer(aRenderer);
      anIterator.Next();
    }
  }
  else
  {
    // Erase named objects
    for (Standard_Integer anIndex = 1; anIndex < theArgNum; ++anIndex)
    {
      TCollection_AsciiString   aName = theArgs[anIndex];
      vtkSmartPointer<vtkActor> anActor;
      if (!GetMapOfActors().Find2(aName, anActor))
      {
        Message::SendFail() << "Syntax error: object '" << aName << "' not found";
        return 1;
      }

      PipelineByActorName(aName)->RemoveFromRenderer(aRenderer);
    }
  }

  // Redraw window
  aRenderer->ResetCamera();
  GetInteractor()->GetRenderWindow()->Render();
  return 0;
}

//================================================================
// Function : VtkRemove
// Purpose  : Remove the actor from memory.
//================================================================
static Standard_Integer VtkRemove(Draw_Interpretor&,
                                  Standard_Integer theArgNum,
                                  const char**     theArgs)
{
  if (!GetInteractor() || !GetInteractor()->IsEnabled())
  {
    Message::SendFail() << "Error: call ivtkinit before";
    return 1;
  }

  vtkSmartPointer<vtkRenderer> aRenderer = GetRenderer();
  if (theArgNum == 1)
  {
    // Remove all actors from the renderer
    DoubleMapOfActorsAndNames::Iterator anIterator(GetMapOfActors());
    while (anIterator.More())
    {
      vtkSmartPointer<IVtkTools_ShapeDataSource> aSrc =
        IVtkTools_ShapeObject::GetShapeSource(anIterator.Key1());
      if (aSrc.GetPointer() != NULL && !aSrc->GetShape().IsNull())
      {
        GetPicker()->RemoveSelectableObject(aSrc->GetShape());
      }
      else
      {
        aRenderer->RemoveActor(anIterator.Key1());
      }
      anIterator.Next();
    }
    // Remove all pipelines from the renderer
    for (ShapePipelineMap::Iterator anIt(*GetPipelines()); anIt.More(); anIt.Next())
    {
      anIt.Value()->RemoveFromRenderer(aRenderer);
    }
    // Clear maps and remove all TopoDS_Shapes, actors and pipelines
    GetMapOfShapes().Clear();
    GetMapOfActors().Clear();
    GetPipelines()->Clear();
  }
  else
  {
    // Remove named objects
    for (Standard_Integer anIndex = 1; anIndex < theArgNum; ++anIndex)
    {
      TCollection_AsciiString   aName = theArgs[anIndex];
      vtkSmartPointer<vtkActor> anActor;
      if (!GetMapOfActors().Find2(aName, anActor))
      {
        Message::SendFail() << "Syntax error: object '" << aName << "' not found";
        return 1;
      }

      // Remove the actor and its pipeline (if found) from the renderer
      vtkSmartPointer<IVtkTools_ShapeDataSource> aSrc =
        IVtkTools_ShapeObject::GetShapeSource(anActor);
      if (aSrc.GetPointer() != NULL && !aSrc->GetShape().IsNull())
      {
        IVtk_IdType aShapeID = aSrc->GetShape()->GetId();
        GetPicker()->RemoveSelectableObject(aSrc->GetShape());
        GetPipeline(aSrc->GetShape()->GetId())->RemoveFromRenderer(aRenderer);
        GetPipelines()->UnBind(aShapeID); // Remove a pipeline
      }
      else
      {
        aRenderer->RemoveActor(anActor);
      }
      // Remove the TopoDS_Shape and the actor
      GetMapOfShapes().UnBind2(aName); // Remove a TopoDS shape
      GetMapOfActors().UnBind2(aName); // Remove an actor
    }
  }

  // Redraw window
  aRenderer->ResetCamera();
  GetInteractor()->GetRenderWindow()->Render();
  return 0;
}

//=================================================================================================

static Standard_Integer VtkSetDisplayMode(Draw_Interpretor& theDI,
                                          Standard_Integer  theArgNum,
                                          const char**      theArgs)
{
  if (!GetInteractor() || !GetInteractor()->IsEnabled())
  {
    theDI << "Error: call ivtkinit before";
    return 1;
  }

  Standard_Integer                                aDispMode          = -1;
  Standard_Integer                                isFaceBoundaryDraw = -1, isSmoothShading = -1;
  Graphic3d_TypeOfShadingModel                    aShadingModel = Graphic3d_TOSM_DEFAULT;
  NCollection_Sequence<vtkSmartPointer<vtkActor>> anActors;
  for (Standard_Integer anArgIter = 1; anArgIter < theArgNum; ++anArgIter)
  {
    TCollection_AsciiString anArgCase(theArgs[anArgIter]);
    anArgCase.LowerCase();
    if (anArgCase == "-faceboundarydraw" || anArgCase == "-drawfaceboundary"
        || anArgCase == "-faceboundary")
    {
      bool toDraw        = Draw::ParseOnOffNoIterator(theArgNum, theArgs, anArgIter);
      isFaceBoundaryDraw = toDraw ? 1 : 0;
    }
    else if (anArgCase == "-smoothshading" || anArgCase == "-smooth")
    {
      bool toEnable   = Draw::ParseOnOffNoIterator(theArgNum, theArgs, anArgIter);
      isSmoothShading = toEnable ? 1 : 0;
    }
    else if (anArgIter + 1 < theArgNum && (anArgCase == "-shadingmodel"))
    {
      TCollection_AsciiString aModelName(theArgs[++anArgIter]);
      aModelName.LowerCase();
      if (aModelName == "fragment" || aModelName == "frag" || aModelName == "phong")
      {
        aShadingModel = Graphic3d_TOSM_FRAGMENT;
      }
      else if (aModelName == "vertex" || aModelName == "vert" || aModelName == "gouraud")
      {
        aShadingModel = Graphic3d_TOSM_VERTEX;
      }
      else if (aModelName == "facet" || aModelName == "flat")
      {
        aShadingModel = Graphic3d_TOSM_FACET;
      }
      else
      {
        theDI << "Syntax error: unknown shading model '" << theArgs[anArgIter] << "'";
        return 1;
      }
    }
    else if (aDispMode == -1 && (anArgCase == "0" || anArgCase == "1"))
    {
      aDispMode = Draw::Atoi(theArgs[anArgIter]);
    }
    else if (aDispMode == -1 && (anArgCase == "-shaded" || anArgCase == "-shading"))
    {
      aDispMode = DM_Shading;
    }
    else if (aDispMode == -1 && anArgCase == "-wireframe")
    {
      aDispMode = DM_Wireframe;
    }
    else
    {
      TCollection_AsciiString   aName = theArgs[anArgIter];
      vtkSmartPointer<vtkActor> anActor;
      if (!GetMapOfActors().Find2(aName, anActor))
      {
        theDI << "Syntax error: object '" << aName << "' not found";
        return 1;
      }
      anActors.Append(anActor);
    }
  }
  if (aDispMode == -1)
  {
    theDI << "Syntax error: wrong number of arguments";
    return 1;
  }

  if (anActors.IsEmpty())
  {
    // update all objects
    for (DoubleMapOfActorsAndNames::Iterator anIter(GetMapOfActors()); anIter.More(); anIter.Next())
    {
      anActors.Append(anIter.Key1());
    }
  }

  for (NCollection_Sequence<vtkSmartPointer<vtkActor>>::Iterator anActorIter(anActors);
       anActorIter.More();
       anActorIter.Next())
  {
    vtkSmartPointer<vtkActor>  anActor = anActorIter.Value();
    IVtkTools_ShapeDataSource* aSrc    = IVtkTools_ShapeObject::GetShapeSource(anActor);
    if (aSrc == NULL)
    {
      continue;
    }

    IVtkOCC_Shape::Handle anOccShape = aSrc->GetShape();
    if (!anOccShape.IsNull())
    {
      IVtkTools_DisplayModeFilter* aFilter =
        GetPipeline(anOccShape->GetId())->GetDisplayModeFilter();
      aFilter->SetDisplayMode((IVtk_DisplayMode)aDispMode);
      if (isFaceBoundaryDraw != -1)
      {
        // Set Red color for boundary edges
        vtkLookupTable* aTable = (vtkLookupTable*)anActor->GetMapper()->GetLookupTable();
        IVtkTools::SetLookupTableColor(aTable, MT_SharedEdge, 1., 0., 0., 1.);
        aFilter->SetFaceBoundaryDraw(isFaceBoundaryDraw == 1);
      }
      if (isSmoothShading != -1)
      {
        aFilter->SetSmoothShading(isSmoothShading == 1);
      }
      switch (aShadingModel)
      {
        case Graphic3d_TOSM_FACET: {
          anActor->GetProperty()->SetInterpolationToFlat();
          break;
        }
        case Graphic3d_TOSM_VERTEX: {
          anActor->GetProperty()->SetInterpolationToGouraud();
          break;
        }
        case Graphic3d_TOSM_FRAGMENT: {
          anActor->GetProperty()->SetInterpolationToPhong();
          break;
        }
        default:
          break;
      }

      aFilter->Modified();
      aFilter->Update();
    }
  }

  // Redraw window
  GetInteractor()->Render();
  return 0;
}

//=================================================================================================

static Standard_Integer VtkSetSelectionMode(Draw_Interpretor&,
                                            Standard_Integer theArgNum,
                                            const char**     theArgs)
{
  if (!GetInteractor() || !GetInteractor()->IsEnabled())
  {
    Message::SendFail() << "Error: call ivtkinit before";
    return 1;
  }
  else if (theArgNum != 3 && theArgNum != 4)
  {
    Message::SendFail() << "Syntax error: expects 2 or 3 arguments";
    return 1;
  }

  if (theArgNum == 3)
  {
    // Set sel mode for all objects
    const Standard_Integer aMode    = Draw::Atoi(theArgs[1]);
    Standard_Boolean       isTurnOn = true;
    if (aMode < 0 || aMode > 8 || !Draw::ParseOnOff(theArgs[2], isTurnOn))
    {
      Message::SendFail() << "Syntax error: only 0-8 selection modes are supported";
      return 1;
    }
    DoubleMapOfActorsAndNames::Iterator anIter(GetMapOfActors());
    while (anIter.More())
    {
      vtkSmartPointer<vtkActor> anActor = anIter.Key1();
      if (aMode == SM_Shape && isTurnOn)
      {
        IVtk_SelectionModeList           aList = GetPicker()->GetSelectionModes(anActor);
        IVtk_SelectionModeList::Iterator anIt(aList);
        // Turn off all sel modes differed from SM_Shape
        while (anIt.More())
        {
          IVtk_SelectionMode aCurMode = anIt.Value();
          if (SM_Shape != aCurMode)
          {
            GetPicker()->SetSelectionMode(anActor, aCurMode, Standard_False);
          }
          anIt.Next();
        }
        GetPicker()->SetSelectionMode(anActor, SM_Shape);
      }

      if (aMode != SM_Shape)
      {
        if (isTurnOn)
        {
          GetPicker()->SetSelectionMode(anActor, (IVtk_SelectionMode)aMode);
          GetPicker()->SetSelectionMode(anActor, SM_Shape, Standard_False);
          if (aMode == SM_Vertex)
          {
            GetPipeline(IVtkTools_ShapeObject::GetShapeSource(anActor)->GetShape()->GetId())
              ->SharedVerticesSelectionOn();
          }
        }
        else
        {
          GetPicker()->SetSelectionMode(anActor, (IVtk_SelectionMode)aMode, Standard_False);
          IVtk_SelectionModeList aList = GetPicker()->GetSelectionModes(anActor);
          if (!aList.Size())
          {
            GetPicker()->SetSelectionMode(anActor, SM_Shape);
          }
          if (aMode == SM_Vertex)
          {
            GetPipeline(IVtkTools_ShapeObject::GetShapeSource(anActor)->GetShape()->GetId())
              ->SharedVerticesSelectionOff();
          }
        }
      }
      anIter.Next();
    }
  }

  if (theArgNum == 4)
  {
    // Set sel mode for named object
    const Standard_Integer aMode    = Draw::Atoi(theArgs[2]);
    Standard_Boolean       isTurnOn = true;
    if (aMode < 0 || aMode > 8 || !Draw::ParseOnOff(theArgs[3], isTurnOn))
    {
      Message::SendFail() << "Syntax error: only 0-8 selection modes are supported";
      return 1;
    }

    TCollection_AsciiString aName = theArgs[1];
    if (GetMapOfActors().IsBound2(aName))
    {
      vtkSmartPointer<vtkActor> anActor = GetMapOfActors().Find2(aName);
      if (aMode == SM_Shape && isTurnOn)
      {
        IVtk_SelectionModeList           aList = GetPicker()->GetSelectionModes(anActor);
        IVtk_SelectionModeList::Iterator anIt(aList);
        // Turn off all sel modes differed from SM_Shape
        while (anIt.More())
        {
          IVtk_SelectionMode aCurMode = anIt.Value();
          if (SM_Shape != aCurMode)
          {
            GetPicker()->SetSelectionMode(anActor, aCurMode, Standard_False);
          }
          anIt.Next();
        }
        GetPicker()->SetSelectionMode(anActor, SM_Shape);
      }

      if (aMode != SM_Shape)
      {
        if (isTurnOn)
        {
          GetPicker()->SetSelectionMode(anActor, (IVtk_SelectionMode)aMode);
          GetPicker()->SetSelectionMode(anActor, SM_Shape, Standard_False);
          if (aMode == SM_Vertex)
          {
            GetPipeline(IVtkTools_ShapeObject::GetShapeSource(anActor)->GetShape()->GetId())
              ->SharedVerticesSelectionOn();
          }
        }
        else
        {
          GetPicker()->SetSelectionMode(anActor, (IVtk_SelectionMode)aMode, Standard_False);
          IVtk_SelectionModeList aList = GetPicker()->GetSelectionModes(anActor);
          if (!aList.Size())
          {
            GetPicker()->SetSelectionMode(anActor, SM_Shape);
          }
          if (aMode == SM_Vertex)
          {
            GetPipeline(IVtkTools_ShapeObject::GetShapeSource(anActor)->GetShape()->GetId())
              ->SharedVerticesSelectionOff();
          }
        }
      }
    }
  }

  // Redraw window
  GetInteractor()->Render();
  return 0;
}

//=================================================================================================

static Standard_Integer VtkSetColor(Draw_Interpretor&,
                                    Standard_Integer theArgNb,
                                    const char**     theArgVec)
{
  if (!GetInteractor() || !GetInteractor()->IsEnabled())
  {
    Message::SendFail() << "Error: call ivtkinit before\n";
    return 1;
  }

  NCollection_Sequence<vtkSmartPointer<vtkActor>> anActorSeq;
  Quantity_Color                                  aQColor;
  bool                                            hasColor = false;
  for (Standard_Integer anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    TCollection_AsciiString   anArg(theArgVec[anArgIter]);
    vtkSmartPointer<vtkActor> anActor;
    if (hasColor)
    {
      Message::SendFail() << "Syntax error at '" << anArg << "'";
      return 1;
    }
    else if (GetMapOfActors().Find2(anArg, anActor))
    {
      anActorSeq.Append(anActor);
    }
    else
    {
      Standard_Integer aNbParsed =
        Draw::ParseColor(theArgNb - anArgIter, theArgVec + anArgIter, aQColor);
      if (aNbParsed == 0)
      {
        Message::SendFail() << "Syntax error at '" << anArg << "'";
        return 1;
      }
      anArgIter += aNbParsed - 1;
      hasColor = true;
    }
  }
  if (!hasColor || anActorSeq.IsEmpty())
  {
    Message::SendFail() << "Syntax error: wrong number of arguments";
    return 1;
  }

  bool isSRGBAware = false;
#ifdef HAVE_VTK_SRGB
  isSRGBAware = GetRenderer()->GetRenderWindow()->GetUseSRGBColorSpace();
#endif
  const Graphic3d_Vec3 aColor =
    isSRGBAware ? (Graphic3d_Vec3)aQColor
                : Quantity_Color::Convert_LinearRGB_To_sRGB((Graphic3d_Vec3)aQColor);
  for (NCollection_Sequence<vtkSmartPointer<vtkActor>>::Iterator anActorIter(anActorSeq);
       anActorIter.More();
       anActorIter.Next())
  {
    const vtkSmartPointer<vtkActor>& anActor = anActorIter.Value();
    vtkLookupTable* aTable = (vtkLookupTable*)anActor->GetMapper()->GetLookupTable();
    IVtkTools::SetLookupTableColor(aTable, MT_ShadedFace, aColor.r(), aColor.g(), aColor.b(), 1.0);
  }

  GetInteractor()->Render();
  return 0;
}

//=================================================================================================

static Standard_Integer VtkSetTransparency(Draw_Interpretor&,
                                           Standard_Integer theArgNb,
                                           const char**     theArgVec)
{
  if (!GetInteractor() || !GetInteractor()->IsEnabled())
  {
    Message::SendFail() << "Error: call ivtkinit before\n";
    return 1;
  }

  NCollection_Sequence<vtkSmartPointer<vtkActor>> anActorSeq;
  Standard_Real                                   aTransparency = -1.0;
  for (Standard_Integer anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    TCollection_AsciiString   anArg(theArgVec[anArgIter]);
    vtkSmartPointer<vtkActor> anActor;
    if (aTransparency >= 0.0)
    {
      Message::SendFail() << "Syntax error at '" << anArg << "'";
      return 1;
    }
    else if (GetMapOfActors().Find2(anArg, anActor))
    {
      anActorSeq.Append(anActor);
    }
    else if (!Draw::ParseReal(theArgVec[anArgIter], aTransparency) || aTransparency < 0.0
             || aTransparency >= 1.0)
    {
      Message::SendFail() << "Syntax error at '" << anArg << "'";
      return 1;
    }
  }
  if (aTransparency < 0.0 || aTransparency >= 1)
  {
    Message::SendFail() << "Syntax error: wrong number of arguments";
    return 1;
  }

  for (NCollection_Sequence<vtkSmartPointer<vtkActor>>::Iterator anActorIter(anActorSeq);
       anActorIter.More();
       anActorIter.Next())
  {
    const vtkSmartPointer<vtkActor>& anActor = anActorIter.Value();
    anActor->GetProperty()->SetOpacity(1.0 - aTransparency);
  }

  GetInteractor()->Render();
  return 0;
}

//================================================================
// Function  : VtkMoveTo
// Purpose   :
// Draw args : ivtkmoveto x y
//================================================================
static Standard_Integer VtkMoveTo(Draw_Interpretor& theDI,
                                  Standard_Integer  theArgNum,
                                  const char**      theArgs)
{
  if (!GetInteractor() || !GetInteractor()->IsEnabled())
  {
    Message::SendFail() << "Error: call ivtkinit before";
    return 1;
  }
  else if (theArgNum != 3)
  {
    Message::SendFail() << "Syntax error: expects 2 arguments";
    return 1;
  }

  Standard_Integer anY = GetInteractor()->GetRenderWindow()->GetSize()[1] - atoi(theArgs[2]) - 1;
  GetInteractor()->MoveTo(atoi(theArgs[1]), anY);

  gp_XYZ aPickPnt;
  GetInteractor()->Selector()->GetPickPosition(aPickPnt.ChangeData());
  theDI << aPickPnt.X() << " " << aPickPnt.Y() << " " << aPickPnt.Z();
  return 0;
}

//=================================================================================================

static Standard_Integer VtkSelect(Draw_Interpretor&,
                                  Standard_Integer theArgNum,
                                  const char**     theArgs)
{
  if (!GetInteractor() || !GetInteractor()->IsEnabled())
  {
    Message::SendFail() << "Error: call ivtkinit before";
    return 1;
  }
  else if (theArgNum != 3)
  {
    Message::SendFail() << "Syntax error: expects 3 arguments";
    return 1;
  }

  Standard_Integer anY = GetInteractor()->GetRenderWindow()->GetSize()[1] - atoi(theArgs[2]) - 1;
  GetInteractor()->MoveTo(atoi(theArgs[1]), anY);
  GetInteractor()->OnSelection();
  return 0;
}

//===================================================================
// Fubction  : VtkViewProj
// Purpose   :
//===================================================================
static Standard_Integer VtkViewProj(Draw_Interpretor&,
                                    Standard_Integer theNbArgs,
                                    const char**     theArgVec)
{
  if (!GetInteractor() || !GetInteractor()->IsEnabled())
  {
    Message::SendFail() << "Error: call ivtkinit before";
    return 1;
  }
  else if (theNbArgs != 1)
  {
    Message::SendFail() << "Syntax error: wrong number of arguments";
    return 1;
  }

  TCollection_AsciiString aCmd(theArgVec[0]);
  aCmd.LowerCase();

  V3d_TypeOfOrientation aProj      = V3d_Xpos;
  bool                  hasProjDir = false;
  if (aCmd == "ivtkaxo")
  {
    hasProjDir = true;
    aProj      = V3d_TypeOfOrientation_Zup_AxoRight;
  }
  else if (aCmd == "ivtktop")
  {
    hasProjDir = true;
    aProj      = V3d_TypeOfOrientation_Zup_Top;
  }
  else if (aCmd == "ivtkbottom")
  {
    hasProjDir = true;
    aProj      = V3d_TypeOfOrientation_Zup_Bottom;
  }
  else if (aCmd == "ivtkfront")
  {
    hasProjDir = true;
    aProj      = V3d_TypeOfOrientation_Zup_Front;
  }
  else if (aCmd == "ivtkback")
  {
    hasProjDir = true;
    aProj      = V3d_TypeOfOrientation_Zup_Back;
  }
  else if (aCmd == "ivtkleft")
  {
    hasProjDir = true;
    aProj      = V3d_TypeOfOrientation_Zup_Left;
  }
  else if (aCmd == "ivtkright")
  {
    hasProjDir = true;
    aProj      = V3d_TypeOfOrientation_Zup_Right;
  }

  if (hasProjDir)
  {
    const gp_Dir    aBck = V3d::GetProjAxis(aProj);
    Graphic3d_Vec3d anUp(0.0, 0.0, 1.0);
    if (aProj == V3d_Zpos)
    {
      anUp.SetValues(0.0, 1.0, 0.0);
    }
    else if (aProj == V3d_Zneg)
    {
      anUp.SetValues(0.0, -1.0, 0.0);
    }

    vtkCamera*   aCam  = GetRenderer()->GetActiveCamera();
    const double aDist = aCam->GetDistance();

    Graphic3d_Vec3d aNewEye = Graphic3d_Vec3d(aBck.X(), aBck.Y(), aBck.Z()) * aDist;
    aCam->SetPosition(aNewEye.x(), aNewEye.y(), aNewEye.z());
    aCam->SetFocalPoint(0.0, 0.0, 0.0);
    aCam->SetViewUp(anUp.x(), anUp.y(), anUp.z());
    aCam->OrthogonalizeViewUp();
  }

  GetRenderer()->ResetCamera();
  GetInteractor()->Render();
  return 0;
}

//=================================================================================================

static int VtkViewParams(Draw_Interpretor& theDI,
                         Standard_Integer  theArgsNb,
                         const char** /*theArgVec*/)
{
  if (!GetInteractor() || !GetInteractor()->IsEnabled())
  {
    Message::SendFail() << "Error: call ivtkinit before";
    return 1;
  }
  else if (theArgsNb != 1)
  {
    Message::SendFail() << "Syntax error: wrong number of arguments";
    return 1;
  }

  vtkCamera* aCam = GetRenderer()->GetActiveCamera();

  gp_XYZ aViewUp, aViewProj, aViewEye, aViewAt;
  aCam->GetViewUp(aViewUp.ChangeCoord(1), aViewUp.ChangeCoord(2), aViewUp.ChangeCoord(3));
  aCam->GetDirectionOfProjection(aViewProj.ChangeCoord(1),
                                 aViewProj.ChangeCoord(2),
                                 aViewProj.ChangeCoord(3));
  aViewProj.Reverse();
  aCam->GetPosition(aViewEye.ChangeCoord(1), aViewEye.ChangeCoord(2), aViewEye.ChangeCoord(3));
  aCam->GetFocalPoint(aViewAt.ChangeCoord(1), aViewAt.ChangeCoord(2), aViewAt.ChangeCoord(3));
  const Standard_Real aViewScale  = aCam->GetParallelScale();
  const Standard_Real aViewAspect = GetRenderer()->GetTiledAspectRatio();
  vtkMatrix4x4*       aProjMat =
    aCam->GetProjectionTransformMatrix(GetRenderer()->GetTiledAspectRatio(), -1, 1);
  vtkMatrix4x4* aViewMat = aCam->GetViewTransformMatrix();
  // print all of the available view parameters
  char aText[4096];
  Sprintf(aText,
          "Scale: %g\n"
          "Aspect:  %g\n"
          "Proj:  %12g %12g %12g\n"
          "Up:    %12g %12g %12g\n"
          "At:    %12g %12g %12g\n"
          "Eye:   %12g %12g %12g\n"
          "ViewMat:       %12g %12g %12g %12g\n"
          "               %12g %12g %12g %12g\n"
          "               %12g %12g %12g %12g\n"
          "               %12g %12g %12g %12g\n"
          "ProjMat:       %12g %12g %12g %12g\n"
          "               %12g %12g %12g %12g\n"
          "               %12g %12g %12g %12g\n"
          "               %12g %12g %12g %12g\n",
          aViewScale,
          aViewAspect,
          aViewProj.X(),
          aViewProj.Y(),
          aViewProj.Z(),
          aViewUp.X(),
          aViewUp.Y(),
          aViewUp.Z(),
          aViewAt.X(),
          aViewAt.Y(),
          aViewAt.Z(),
          aViewEye.X(),
          aViewEye.Y(),
          aViewEye.Z(),
          aViewMat->GetElement(0, 0),
          aViewMat->GetElement(0, 1),
          aViewMat->GetElement(0, 2),
          aViewMat->GetElement(0, 3),
          aViewMat->GetElement(1, 0),
          aViewMat->GetElement(1, 1),
          aViewMat->GetElement(1, 2),
          aViewMat->GetElement(1, 3),
          aViewMat->GetElement(2, 0),
          aViewMat->GetElement(2, 1),
          aViewMat->GetElement(2, 2),
          aViewMat->GetElement(2, 3),
          aViewMat->GetElement(3, 0),
          aViewMat->GetElement(3, 1),
          aViewMat->GetElement(3, 2),
          aViewMat->GetElement(3, 3),
          aProjMat->GetElement(0, 0),
          aProjMat->GetElement(0, 1),
          aProjMat->GetElement(0, 2),
          aProjMat->GetElement(0, 3),
          aProjMat->GetElement(1, 0),
          aProjMat->GetElement(1, 1),
          aProjMat->GetElement(1, 2),
          aProjMat->GetElement(1, 3),
          aProjMat->GetElement(2, 0),
          aProjMat->GetElement(2, 1),
          aProjMat->GetElement(2, 2),
          aProjMat->GetElement(2, 3),
          aProjMat->GetElement(3, 0),
          aProjMat->GetElement(3, 1),
          aProjMat->GetElement(3, 2),
          aProjMat->GetElement(3, 3));
  theDI << aText;
  return 0;
}

//=================================================================================================

static int VtkCamera(Draw_Interpretor& theDI, Standard_Integer theArgsNb, const char** theArgVec)
{
  if (!GetInteractor() || !GetInteractor()->IsEnabled())
  {
    Message::SendFail() << "Error: call ivtkinit before";
    return 1;
  }

  vtkCamera* aCamera = GetRenderer()->GetActiveCamera();

  if (theArgsNb < 2)
  {
    Standard_Real aZNear = 0.0, aZFar = 0.0;
    aCamera->GetClippingRange(aZNear, aZFar);
    theDI << "ProjType:   " << (aCamera->GetParallelProjection() ? "orthographic" : "perspective")
          << "\n";
    theDI << "FOVy:       " << aCamera->GetViewAngle() << "\n";
    theDI << "Distance:   " << aCamera->GetDistance() << "\n";
    theDI << "ZNear:      " << aZNear << "\n";
    theDI << "ZFar:       " << aZFar << "\n";
    return 0;
  }

  for (Standard_Integer anArgIter = 1; anArgIter < theArgsNb; ++anArgIter)
  {
    Standard_CString        anArg = theArgVec[anArgIter];
    TCollection_AsciiString anArgCase(anArg);
    anArgCase.LowerCase();
    if (anArgCase == "-ortho" || anArgCase == "-orthographic")
    {
      aCamera->SetParallelProjection(true);
    }
    else if (anArgCase == "-persp" || anArgCase == "-perspective")
    {
      aCamera->SetParallelProjection(false);
    }
    else
    {
      Message::SendFail() << "Error: unknown argument '" << anArg << "'";
      return 1;
    }
  }
  GetRenderer()->ResetCamera();
  GetInteractor()->Render();
  return 0;
}

//===================================================================
// Fubction  : VtkDump
// Purpose   :
//===================================================================
static Standard_Integer VtkDump(Draw_Interpretor&, Standard_Integer theArgNum, const char** theArgs)
{
  if (!GetInteractor() || !GetInteractor()->IsEnabled())
  {
    Message::SendFail() << "Error : call ivtkinit before";
    return 1;
  }
  else if (theArgNum < 2)
  {
    Message::SendFail() << "Syntax error: wrong number of parameters";
    return 1;
  }

  vtkSmartPointer<vtkWindowToImageFilter> anImageFilter =
    vtkSmartPointer<vtkWindowToImageFilter>::New();
  anImageFilter->SetInput(GetInteractor()->GetRenderWindow());
  // Set custom buffer type
  if (theArgNum > 2)
  {
    TCollection_AsciiString aBufferType(theArgs[2]);
    aBufferType.LowerCase();
    if (aBufferType.IsEqual("rgb"))
    {
      anImageFilter->SetInputBufferTypeToRGB();
    }
    else if (aBufferType.IsEqual("rgba"))
    {
      anImageFilter->SetInputBufferTypeToRGBA();
    }
    else if (aBufferType.IsEqual("depth"))
    {
      anImageFilter->SetInputBufferTypeToZBuffer();
    }
  }
  anImageFilter->Update();

  // Set custom stereo projection options
  if (theArgNum > 5 && GetRenderer()->GetRenderWindow()->GetStereoRender())
  {
    Standard_CString aStereoProjStr = theArgs[5];

    Standard_Integer aStereoType = GetRenderer()->GetRenderWindow()->GetStereoType();
    if (strcasecmp(aStereoProjStr, "L"))
    {
      GetRenderer()->GetRenderWindow()->SetStereoTypeToLeft();
      GetRenderer()->GetRenderWindow()->StereoUpdate();
      anImageFilter->Update();
      GetRenderer()->GetRenderWindow()->SetStereoType(aStereoType);
    }
    else if (strcasecmp(aStereoProjStr, "R"))
    {
      GetRenderer()->GetRenderWindow()->SetStereoTypeToRight();
      GetRenderer()->GetRenderWindow()->StereoUpdate();
      anImageFilter->Update();
      GetRenderer()->GetRenderWindow()->SetStereoType(aStereoType);
    }
    else
    {
      Message::SendFail() << "Syntax error: unknown value for stereo projection";
      return 1;
    }
  }

  // Set parameters for image writer
  vtkSmartPointer<vtkImageWriter> anImageWriter;
  TCollection_AsciiString         aFilename(theArgs[1]);
  Standard_Integer        anExtStart = aFilename.SearchFromEnd(TCollection_AsciiString("."));
  TCollection_AsciiString aFormat    = (anExtStart == -1)
                                         ? TCollection_AsciiString("")
                                         : aFilename.SubString(anExtStart + 1, aFilename.Length());
  aFormat.LowerCase();

  if (aFormat.IsEqual("png"))
  {
    anImageWriter = vtkSmartPointer<vtkPNGWriter>::New();
  }
  else if (aFormat.IsEqual("bmp"))
  {
    anImageWriter = vtkSmartPointer<vtkBMPWriter>::New();
  }
  else if (aFormat.IsEqual("jpeg"))
  {
    anImageWriter = vtkSmartPointer<vtkJPEGWriter>::New();
  }
  else if (aFormat.IsEqual("tiff"))
  {
    anImageWriter = vtkSmartPointer<vtkTIFFWriter>::New();
  }
  else if (aFormat.IsEqual("pnm"))
  {
    anImageWriter = vtkSmartPointer<vtkPNMWriter>::New();
  }
  else // aFormat is unsupported or not set.
  {
    if (aFormat.IsEmpty())
    {
      Message::SendWarning() << "Warning: the image format is not set.\n"
                             << "The image will be saved into PNG format.";
      anImageWriter = vtkSmartPointer<vtkPNGWriter>::New();
      aFormat       = TCollection_AsciiString("png");
      if (anExtStart != -1)
      {
        aFilename.Split(anExtStart);
      }
      else
      {
        aFilename += ".";
      }
      aFilename += aFormat;
    }
    else
    {
      Message::SendFail() << "Error: the image format " << aFormat << " is not supported";
      return 1;
    }
  }

  anImageWriter->SetFileName(aFilename.ToCString());

  Standard_Integer aWidth  = (theArgNum > 3) ? atoi(theArgs[3]) : 0;
  Standard_Integer aHeight = (theArgNum > 4) ? atoi(theArgs[4]) : 0;
  if (aWidth >= 0 || aHeight >= 0)
  {
    // Scale image
    vtkSmartPointer<vtkImageResize> anImageResize = vtkSmartPointer<vtkImageResize>::New();
#if VTK_MAJOR_VERSION <= 5
    anImageResize->SetInput(anImageFilter->GetOutput());
#else
    anImageResize->SetInputData(anImageFilter->GetOutput());
#endif

    anImageResize->SetOutputDimensions(aWidth, aHeight, 1);
    anImageResize->Update();
    anImageWriter->SetInputConnection(anImageResize->GetOutputPort());
  }
  else
  {
    anImageWriter->SetInputConnection(anImageFilter->GetOutputPort());
  }
  anImageWriter->Write();
  return 0;
}

//===================================================================
// Fubction  : VtkBackgroundColor
// Purpose   :
//===================================================================
static Standard_Integer VtkBackgroundColor(Draw_Interpretor&,
                                           Standard_Integer theArgNum,
                                           const char**     theArgs)
{
  if (!GetInteractor() || !GetInteractor()->IsEnabled())
  {
    Message::SendFail() << "Error: call ivtkinit before";
    return 1;
  }

  Quantity_Color         aQColor1;
  const Standard_Integer aNbParsed1 = Draw::ParseColor(theArgNum - 1, theArgs + 1, aQColor1);
  if (aNbParsed1 == 0)
  {
    Message::SendFail() << "Syntax error: wrong number of parameters";
    return 1;
  }

  bool isSRGBAware = false;
#ifdef HAVE_VTK_SRGB
  isSRGBAware = GetRenderer()->GetRenderWindow()->GetUseSRGBColorSpace();
#endif
  const Graphic3d_Vec3 aColor1 =
    isSRGBAware ? (Graphic3d_Vec3)aQColor1
                : Quantity_Color::Convert_LinearRGB_To_sRGB((Graphic3d_Vec3)aQColor1);
  GetRenderer()->SetGradientBackground(false);
  GetRenderer()->SetBackground(aColor1.r(), aColor1.g(), aColor1.b());
  if (theArgNum - 1 > aNbParsed1)
  {
    Quantity_Color         aQColor2;
    const Standard_Integer aNbParsed2 =
      Draw::ParseColor(theArgNum - 1 - aNbParsed1, theArgs + 1 + aNbParsed1, aQColor2);
    if (aNbParsed2 == 0)
    {
      Message::SendFail() << "Syntax error: wrong number of parameters";
      return 1;
    }

    const Graphic3d_Vec3 aColor2 =
      isSRGBAware ? (Graphic3d_Vec3)aQColor2
                  : Quantity_Color::Convert_LinearRGB_To_sRGB((Graphic3d_Vec3)aQColor2);
    GetRenderer()->SetBackground2(aColor2.r(), aColor2.g(), aColor2.b());
    GetRenderer()->SetGradientBackground(true);
  }

  GetInteractor()->Render();
  return 0;
}

//=================================================================================================

void IVtkDraw::Commands(Draw_Interpretor& theCommands)
{
  const char* group = "VtkViewer";

  theCommands.Add("ivtkinit",
                  "ivtkinit [leftPx topPx widthPx heightPx] [-srgb {on|off}] [-msaa NbSamples]"
                  "\n\t\t: Creates the Vtk window."
                  "\n\t\t:  -srgb Enable/disable sRGB colorspace; OFF by default."
                  "\n\t\t:  -msaa Requests desired number of samples for multisampling buffer;"
                  "\n\t\t:        0 by default meaning no MSAA.",
                  __FILE__,
                  VtkInit,
                  group);

  theCommands.Add("ivtkclose", "ivtkclose : Closes the Vtk window.", __FILE__, VtkClose, group);

  theCommands.Add("ivtkdefaults",
                  "ivtkdefaults [-absDefl value]"
                  "\n\t\t:              [-devCoeff value]"
                  "\n\t\t:              [-angDefl value]"
                  "\n\t\t:              [-autoTriang {off/on | 0/1}]"
                  "\n\t\t: Sets default VTK meshing parameters.",
                  __FILE__,
                  VtkDefaults,
                  group);

  theCommands.Add(
    "ivtkrenderparams",
    "ivtkrenderparams [-depthPeeling NbLayers] [-shadows {on|off}]"
    "\n\t\t: Sets Vtk rendering parameters."
    "\n\t\t:  -shadows      Turn shadows on/off; OFF by default."
    "\n\t\t:  -depthPeeling Enable/disable depth peeling for rendering transparent objects"
    "\n\t\t:                with specified number of layers;"
    "\n\t\t:                0 by default meaning order-dependent transparency without depth "
    "peeling.",
    __FILE__,
    VtkRenderParams,
    group);

  theCommands.Add("ivtkdisplay",
                  "ivtkdisplay name1 [name2 ...]"
                  "\n\t\t: Displays named objects in Vtk view.",
                  __FILE__,
                  VtkDisplay,
                  group);

  theCommands.Add("ivtkerase",
                  "ivtkerase [name1 name2 ...]"
                  "\n\t\t: Hides in Vtk renderer named objects or all objects.",
                  __FILE__,
                  VtkErase,
                  group);

  theCommands.Add("ivtkremove",
                  "ivtkremove name1 [name2 ...]"
                  "\n\t\t: Removes from Vtk renderer named objects.",
                  __FILE__,
                  VtkRemove,
                  group);

  theCommands.Add("ivtkclear",
                  "ivtkclear : Removes all objects from Vtk renderer.",
                  __FILE__,
                  VtkRemove,
                  group);

  theCommands.Add(
    "ivtksetdispmode",
    "ivtksetdispmode [name] mode={0|1} [-faceBoundaryDraw {0|1}] [-smoothShading {0|1}]"
    "\n\t\t:                                   [-shadingModel {phong|gouraud|flat}]"
    "\n\t\t: Sets or unsets display mode to the object with the given name or to all objects."
    "\n\t\t:   -faceBoundaryDraw show/hide boundaries within shading display mode"
    "\n\t\t:   -smoothShading    enable/disable vertex normals for smooth shading"
    "\n\t\t:   -shadingModel     sets specified shading model",
    __FILE__,
    VtkSetDisplayMode,
    group);

  theCommands.Add("ivtksetselmode",
                  "ivtksetselmode [name] mode {on|off}"
                  "\n\t\t: Sets or unsets selection mode 'mode' to the object with name 'name'"
                  "\n\t\t: or to the all displayed objects.",
                  __FILE__,
                  VtkSetSelectionMode,
                  group);

  theCommands.Add("ivtkmoveto",
                  "ivtkmoveto x y"
                  "\n\t\t: Moves position to the pixel with coordinates (x,y). The object on this "
                  "position is highlighted.",
                  __FILE__,
                  VtkMoveTo,
                  group);

  theCommands.Add(
    "ivtkselect",
    "ivtkselect x y"
    "\n\t\t: Selects object which correspond to the pixel with input coordinates (x,y).",
    __FILE__,
    VtkSelect,
    group);

  theCommands.Add("ivtkfit",
                  "ivtkfit : Fits all displayed objects into Vtk view.",
                  __FILE__,
                  VtkViewProj,
                  group);

  theCommands.Add("ivtkaxo",
                  "ivtkaxo : Resets Vtk view orientation to axo",
                  __FILE__,
                  VtkViewProj,
                  group);

  theCommands.Add("ivtkfront",
                  "ivtkfront : Resets Vtk view orientation to front",
                  __FILE__,
                  VtkViewProj,
                  group);

  theCommands.Add("ivtkback",
                  "ivtkback : Resets Vtk view orientation to back",
                  __FILE__,
                  VtkViewProj,
                  group);

  theCommands.Add("ivtktop",
                  "ivtktop : Resets Vtk view orientation to top",
                  __FILE__,
                  VtkViewProj,
                  group);

  theCommands.Add("ivtkbottom",
                  "ivtkbottom : Resets Vtk view orientation to bottom",
                  __FILE__,
                  VtkViewProj,
                  group);

  theCommands.Add("ivtkleft",
                  "ivtkleft : Resets Vtk view orientation to left",
                  __FILE__,
                  VtkViewProj,
                  group);

  theCommands.Add("ivtkright",
                  "ivtkright : Resets Vtk view orientation to right",
                  __FILE__,
                  VtkViewProj,
                  group);

  theCommands.Add("ivtkdump",
                  "ivtkdump <FullFilename>.{png|bmp|jpeg|tiff|pnm} [buffer={rgb|rgba|depth}]"
                  "\n\t\t:          [width height] [stereoproj={L|R}]"
                  "\n\t\t: Dumps contents of viewer window to PNG, BMP, JPEG, TIFF or PNM file.",
                  __FILE__,
                  VtkDump,
                  group);

  theCommands.Add(
    "ivtkbgcolor",
    "ivtkbgcolor Color1 [Color2]"
    "\n\t\t: Sets uniform background color or gradient one if second triple of parameters is set.",
    __FILE__,
    VtkBackgroundColor,
    group);

  theCommands.Add("ivtksetcolor",
                  "ivtksetcolor name {ColorName|R G B}"
                  "\n\t\t: Sets color to the object with name 'name'.",
                  __FILE__,
                  VtkSetColor,
                  group);

  theCommands.Add("ivtksettransparency",
                  "ivtksettransparency name 0..1"
                  "\n\t\t: Sets transparency to the object with name 'name'.",
                  __FILE__,
                  VtkSetTransparency,
                  group);

  theCommands.Add("ivtkviewparams",
                  "ivtkviewparams: Prints all current view parameters.",
                  __FILE__,
                  VtkViewParams,
                  group);

  theCommands.Add("ivtkcamera",
                  "ivtkcamera [-ortho] [-persp]"
                  "\n\t\t: Manages camera parameters."
                  "\n\t\t: Prints current value when option called without argument."
                  "\n\t\t: Orthographic camera:"
                  "\n\t\t:   -ortho      activate orthographic projection"
                  "\n\t\t: Perspective camera:"
                  "\n\t\t:   -persp      activate perspective  projection",
                  __FILE__,
                  VtkCamera,
                  group);
}

//=================================================================================================

void IVtkDraw::Factory(Draw_Interpretor& theDI)
{
  // definition of Viewer Commands
  IVtkDraw::Commands(theDI);
}

// Declare entry point PLUGINFACTORY
DPLUGIN(IVtkDraw)
