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
  #include <X11/Shell.h>
  #include <X11/Xlib.h>
  #include <X11/Xutil.h>
  #include <GL/glx.h>
  #include <Xw_Window.hxx>
  #include <vtkXRenderWindowInteractor.h>
  #include <vtkXOpenGLRenderWindow.h>
  #include <tk.h>
#endif
#include <Standard_WarningsRestore.hxx>

#if (VTK_MAJOR_VERSION > 8) || (VTK_MAJOR_VERSION == 8 && VTK_MINOR_VERSION >= 1)
  #define HAVE_VTK_SRGB
#endif

//================================================================
// TYPE DEFINITIONS
//================================================================

typedef NCollection_DoubleMap<TopoDS_Shape, TCollection_AsciiString> DoubleMapOfShapesAndNames;
typedef NCollection_DoubleMap<vtkSmartPointer<vtkActor>, TCollection_AsciiString> DoubleMapOfActorsAndNames;

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

static void SetDisplayConnection (const Handle(Aspect_DisplayConnection)& theDisplayConnection)
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

static Handle(PipelinePtr) GetPipeline (const IVtk_IdType& theShapeID)
{
  Handle(PipelinePtr) aPtr;
  GetPipelines()->Find (theShapeID, aPtr);
  return aPtr;
}

//! Get VTK render pipeline with shape ID got from actor.
static Handle(PipelinePtr) PipelineByActor (const vtkSmartPointer<vtkActor>& theActor)
{
  IVtk_IdType aShapeID = IVtkTools_ShapeObject::GetShapeSource (theActor)->GetShape()->GetId();
  return GetPipeline (aShapeID);
}

//! Get VTK render pipeline with actor that has the input name.
static Handle(PipelinePtr) PipelineByActorName (const TCollection_AsciiString& theName)
{
  const vtkSmartPointer<vtkActor>& anActor = GetMapOfActors().Find2 (theName);
  return PipelineByActor (anActor);
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
  static unsigned int aShapesCounter = (unsigned int )-1;
  return (Standard_Integer )++aShapesCounter;
}

//=========================================================
// Function : WClass
// Purpose  :
//=========================================================
const Handle(WNT_WClass)& IVtkDraw::WClass()
{
  static Handle(WNT_WClass) aWindowClass;
#ifdef _WIN32
  if (aWindowClass.IsNull())
  {
    aWindowClass = new WNT_WClass ("GWVTK_Class", NULL,
                                   CS_VREDRAW | CS_HREDRAW, 0, 0,
                                   ::LoadCursorW (NULL, IDC_ARROW));
  }
#endif
  return aWindowClass;
}

//==============================================================
// Function : ViewerInit
// Purpose  :
//==============================================================
void IVtkDraw::ViewerInit (const IVtkWinParams& theParams)
{
  Standard_Integer aPxLeft  =   0, aPxTop    = 460;
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
    SetDisplayConnection (new Aspect_DisplayConnection ());
#ifdef _WIN32
    if (GetWindow().IsNull())
    {
      GetWindow() = new WNT_Window ("IVtkTest", WClass(),
                                    WS_OVERLAPPEDWINDOW,
                                    aPxLeft, aPxTop,
                                    aPxWidth, aPxHeight,
                                    Quantity_NOC_BLACK);
      GetWindow()->SetVirtual (Draw_VirtualWindows);
    }
#else

    if (GetWindow().IsNull())
    {
      GetWindow() = new Xw_Window (GetDisplayConnection(),
                                   "IVtkTest",
                                   aPxLeft, aPxTop,
                                   aPxWidth, aPxHeight);
      GetWindow()->SetVirtual (Draw_VirtualWindows);
    }
#endif
    // Init pipeline
    GetRenderer() = vtkSmartPointer<vtkRenderer>::New();

    vtkSmartPointer<vtkRenderWindow> aRenWin = vtkSmartPointer<vtkRenderWindow>::New();
    aRenWin->AddRenderer (GetRenderer());
    GetRenderer()->GetActiveCamera()->ParallelProjectionOn();
    aRenWin->SetSize (aPxWidth, aPxHeight);

    aRenWin->SetMultiSamples (theParams.NbMsaaSample);
    aRenWin->SetAlphaBitPlanes (1);
  #ifdef HAVE_VTK_SRGB
    aRenWin->SetUseSRGBColorSpace (theParams.UseSRGBColorSpace);
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
    aRenWin->SetWindowId ((void*)aWindowId);
    Display *aDisplayId = GetDisplayConnection()->GetDisplay();
    aRenWin->SetDisplayId (aDisplayId);

    // Setup XWindow
    XSynchronize (aDisplayId, 1);
    GetWindow()->Map();

    // X11 : For keyboard on SUN
    XWMHints wmhints;
    wmhints.flags = InputHint;
    wmhints.input = 1;

    XSetWMHints (aDisplayId, aWindowId, &wmhints);

    XSelectInput (aDisplayId, aWindowId,  ExposureMask | KeyPressMask |
      ButtonPressMask | ButtonReleaseMask |
      StructureNotifyMask |
      PointerMotionMask |
      Button1MotionMask | Button2MotionMask |
      Button3MotionMask
      );

    XSynchronize (aDisplayId, 0);

#endif

    // Init interactor
    GetInteractor() = vtkSmartPointer<IVtkDraw_Interactor>::New();
    GetInteractor()->SetRenderWindow (aRenWin);
    GetInteractor()->SetOCCWindow (GetWindow());

    vtkSmartPointer<vtkInteractorStyleTrackballCamera>
      aStyle = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    GetInteractor()->SetInteractorStyle (aStyle);

    // Init picker
    GetPicker() = vtkSmartPointer<IVtkTools_ShapePicker>::New();
    GetPicker()->SetTolerance (0.025f);
    GetPicker()->SetRenderer (GetRenderer());

    GetInteractor()->SetShapePicker (GetPicker());
    GetInteractor()->SetPipelines (GetPipelines());
    GetInteractor()->Initialize();

    aRenWin->SetOffScreenRendering(Draw_VirtualWindows);
    aRenWin->Render();
  }

  GetWindow()->Map();
}

//================================================================
// Function : VtkInit
// Purpose  : 
//================================================================
static Standard_Integer VtkInit (Draw_Interpretor& ,
                                 Standard_Integer theNbArgs,
                                 const char** theArgVec)
{
  bool hasSize = false;
  IVtkDraw::IVtkWinParams aParams;
  for (Standard_Integer anArgIter = 1; anArgIter < theNbArgs; ++anArgIter)
  {
    TCollection_AsciiString anArg (theArgVec[anArgIter]);
    anArg.LowerCase();
    if (anArg == "-msaa"
     && anArgIter + 1 < theNbArgs)
    {
      aParams.NbMsaaSample = Draw::Atoi (theArgVec[++anArgIter]);
    }
    else if (anArg == "-srgb")
    {
      aParams.UseSRGBColorSpace = true;
      if (anArgIter + 1 < theNbArgs
       && Draw::ParseOnOff (theArgVec[anArgIter + 1], aParams.UseSRGBColorSpace))
      {
        ++anArgIter;
      }
    }
    else if (!hasSize
          && anArgIter + 3 < theNbArgs)
    {
      aParams.TopLeft.SetValues (Draw::Atoi (theArgVec[anArgIter + 0]),
                                 Draw::Atoi (theArgVec[anArgIter + 1]));
      aParams.Size   .SetValues (Draw::Atoi (theArgVec[anArgIter + 2]),
                                 Draw::Atoi (theArgVec[anArgIter + 3]));
    }
    else
    {
      Message::SendFail() << "Syntax error at '" << anArg << "'";
      return 1;
    }
  }

  IVtkDraw::ViewerInit (aParams);
  return 0;
}

//================================================================
// Function : VtkClose
// Purpose  :
//================================================================
static Standard_Integer VtkClose (Draw_Interpretor& ,
                                  Standard_Integer theNbArgs,
                                  const char** )
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
    //GetInteractor()->SetRenderWindow (NULL);
    GetInteractor()->TerminateApp();
  }

  GetInteractor() = NULL;
  GetRenderer() = NULL;
  GetPicker() = NULL;
  return 0;
}

//================================================================
// Function : VtkRenderParams
// Purpose  :
//================================================================
static Standard_Integer VtkRenderParams (Draw_Interpretor& ,
                                         Standard_Integer theNbArgs,
                                         const char** theArgVec)
{
  if (!GetInteractor()
   || !GetInteractor()->IsEnabled())
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
    TCollection_AsciiString anArg (theArgVec[anArgIter]);
    anArg.LowerCase();
    if (anArg == "-depthpeeling"
     && anArgIter + 1 < theNbArgs)
    {
      Standard_Integer aNbLayers = Draw::Atoi (theArgVec[++anArgIter]);
      GetRenderer()->SetUseDepthPeeling (aNbLayers > 0);
      GetRenderer()->SetMaximumNumberOfPeels (aNbLayers);
    }
    else if (anArg == "-shadows")
    {
      bool toUseShadows = true;
      if (anArgIter + 1 < theNbArgs
       && Draw::ParseOnOff (theArgVec[anArgIter + 1], toUseShadows))
      {
        ++anArgIter;
      }
    #if (VTK_MAJOR_VERSION >= 7)
      GetRenderer()->SetUseShadows (toUseShadows);
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


//================================================================
// Function : CreateActor
// Purpose  : 
//================================================================
vtkActor* CreateActor (const Standard_Integer theId,
                       const TopoDS_Shape& theShape)
{
  if ( theShape.IsNull() )
  {
    return NULL;
  }

  Handle(PipelinePtr) aPL = new PipelinePtr (theShape, theId);
  GetPipelines()->Bind (theId, aPL);

  return aPL->Actor();
}

//================================================================
// Function : VtkDisplay
// Purpose  : 
//================================================================

static Standard_Integer VtkDisplay (Draw_Interpretor& ,
                                    Standard_Integer theArgNum,
                                    const char** theArgs)
{
  if (!GetInteractor()
   || !GetInteractor()->IsEnabled())
  {
    Message::SendFail() << "Error : call ivtkinit before";
    return 1;
  }
  if (theArgNum < 2)
  {
    Message::SendFail() << "Error : expects at least 1 argument\n";
    return 1;
  }

  TCollection_AsciiString aName;
  TopoDS_Shape anOldShape, aNewShape;
  vtkSmartPointer<vtkRenderer>& aRenderer = GetRenderer();
  for (Standard_Integer anIndex = 1; anIndex < theArgNum; ++anIndex)
  {
    // Get name of shape
    aName = theArgs[anIndex];
    // Get shape from DRAW
    aNewShape = DBRep::Get (theArgs[anIndex]);

    // The shape is already in the map
    if (GetMapOfShapes().IsBound2 (aName))
    {
      // Get shape from map
      anOldShape = GetMapOfShapes().Find2 (aName);
      // Equal shapes
      if (anOldShape.IsEqual (aNewShape))
      {
        // Get actor from map and display it
        PipelineByActorName (aName)->AddToRenderer (aRenderer);
      }
      // Different shapes
      else
      {
        if (aNewShape.IsNull()) continue;
        // Create actor from DRAW shape
        vtkActor* anActor = CreateActor (GenerateId(), aNewShape);
        // Remove old actor from render
        PipelineByActorName (aName)->RemoveFromRenderer (aRenderer);
        // Update maps
        GetMapOfShapes().UnBind2 (aName);
        GetMapOfShapes().Bind (aNewShape, aName);
        GetMapOfActors().UnBind2 (aName);
        GetMapOfActors().Bind (anActor, aName);
        // Display new actor
        PipelineByActorName (aName)->AddToRenderer (aRenderer);
        // Compute selection for displayed actors
        GetPicker()->SetSelectionMode (SM_Shape, Standard_True);
      }
    }
    // There is no shape with given name in map
    else
    {
      if (aNewShape.IsNull()) continue;
      // Create actor from DRAW shape
      Standard_Integer anId = GenerateId();
      vtkSmartPointer<vtkActor> anActor = CreateActor (anId, aNewShape);
      // Update maps
      GetMapOfShapes().Bind (aNewShape, aName);
      GetMapOfActors().Bind (anActor, aName);
      // Display actor
      GetPipeline(anId)->AddToRenderer(aRenderer);

      // Compute selection for displayed actors
      GetPicker()->SetSelectionMode (SM_Shape, Standard_True);
    }
  }

  // Redraw window
  aRenderer->ResetCamera();
  GetInteractor()->GetRenderWindow()->Render();

  return 0;
}

//================================================================
// Function : VtkErase
// Purpose  : 
//================================================================
static Standard_Integer VtkErase (Draw_Interpretor& ,
                                  Standard_Integer theArgNum,
                                  const char** theArgs)
{
  if (!GetInteractor()
   || !GetInteractor()->IsEnabled())
  {
    Message::SendFail() << "Error: call ivtkinit before";
    return 1;
  }

  vtkSmartPointer<vtkRenderer> aRenderer = GetRenderer();
  if (theArgNum == 1)
  {
    // Erase all objects
    DoubleMapOfActorsAndNames::Iterator anIterator (GetMapOfActors());
    while (anIterator.More())
    {
      PipelineByActor (anIterator.Key1())->RemoveFromRenderer (aRenderer);
      anIterator.Next();
    }
  }
  else
  {
    // Erase named objects
    for (Standard_Integer anIndex = 1; anIndex < theArgNum; ++anIndex)
    {
      TCollection_AsciiString aName = theArgs[anIndex];
      vtkSmartPointer<vtkActor> anActor;
      if (!GetMapOfActors().Find2 (aName, anActor))
      {
        Message::SendFail() << "Syntax error: object '" << aName << "' not found";
        return 1;
      }

      PipelineByActorName (aName)->RemoveFromRenderer (aRenderer);
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
static Standard_Integer VtkRemove (Draw_Interpretor& ,
                                   Standard_Integer theArgNum,
                                   const char** theArgs)
{
  if (!GetInteractor()
   || !GetInteractor()->IsEnabled())
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
      vtkSmartPointer<IVtkTools_ShapeDataSource> aSrc = IVtkTools_ShapeObject::GetShapeSource (anIterator.Key1());
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
      TCollection_AsciiString aName = theArgs[anIndex];
      vtkSmartPointer<vtkActor> anActor;
      if (!GetMapOfActors().Find2 (aName, anActor))
      {
        Message::SendFail() << "Syntax error: object '" << aName << "' not found";
        return 1;
      }

      // Remove the actor and its pipeline (if found) from the renderer
      vtkSmartPointer<IVtkTools_ShapeDataSource> aSrc = IVtkTools_ShapeObject::GetShapeSource (anActor);
      if (aSrc.GetPointer() != NULL && !aSrc->GetShape().IsNull())
      {
        IVtk_IdType aShapeID = aSrc->GetShape()->GetId();
        GetPicker()->RemoveSelectableObject (aSrc->GetShape());
        GetPipeline (aSrc->GetShape()->GetId())->RemoveFromRenderer (aRenderer);
        GetPipelines()->UnBind (aShapeID); // Remove a pipepline
      }
      else
      {
        aRenderer->RemoveActor (anActor);
      }
      // Remove the TopoDS_Shape and the actor
      GetMapOfShapes().UnBind2 (aName); // Remove a TopoDS shape
      GetMapOfActors().UnBind2 (aName); // Remove an actor
    }
  }

  // Redraw window
  aRenderer->ResetCamera();
  GetInteractor()->GetRenderWindow()->Render();
  return 0;
}

//================================================================
// Function  : VtkSetDisplayMode
// Purpose   : 
// Draw args : ivtksetdispmode [name] mode(0,1)
//================================================================
static Standard_Integer VtkSetDisplayMode (Draw_Interpretor& ,
                                           Standard_Integer theArgNum,
                                           const char** theArgs)
{
  if (!GetInteractor()
   || !GetInteractor()->IsEnabled())
  {
    Message::SendFail() << "Error: call ivtkinit before";
    return 1;
  }
  else if (theArgNum != 2 && theArgNum != 3)
  {
    Message::SendFail() << "Syntax error: expects 1 or 2 arguments";
    return 1;
  }

  if (theArgNum == 2)
  {
    // Set disp mode for all objects
    Standard_Integer aMode =  Draw::Atoi (theArgs[1]); // Get mode
    DoubleMapOfActorsAndNames::Iterator anIter (GetMapOfActors());
    while (anIter.More())
    {
      vtkSmartPointer<vtkActor> anActor = anIter.Key1();
      IVtkTools_ShapeDataSource* aSrc = IVtkTools_ShapeObject::GetShapeSource (anActor);
      if (aSrc)
      {
        IVtkOCC_Shape::Handle anOccShape = aSrc->GetShape();
        if (!anOccShape.IsNull())
        {
          IVtkTools_DisplayModeFilter* aFilter = GetPipeline ( anOccShape->GetId() )->GetDisplayModeFilter();
          aFilter->SetDisplayMode((IVtk_DisplayMode)aMode);
          aFilter->Modified();
          aFilter->Update();
        }
      }
      anIter.Next();
    }
  }
  // Set disp mode for named object
  else 
  {
    TCollection_AsciiString aName = theArgs[1];
    vtkSmartPointer<vtkActor> anActor;
    if (!GetMapOfActors().Find2 (aName, anActor))
    {
      Message::SendFail() << "Syntax error: object '" << aName << "' not found";
      return 1;
    }

    Standard_Integer aMode = atoi(theArgs[2]);
    vtkSmartPointer<IVtkTools_ShapeDataSource> aSrc = IVtkTools_ShapeObject::GetShapeSource (anActor);
    if (aSrc)
    {
      IVtkOCC_Shape::Handle anOccShape = aSrc->GetShape();
      if (!anOccShape.IsNull())
      {
        IVtkTools_DisplayModeFilter* aFilter = GetPipeline (anOccShape->GetId())->GetDisplayModeFilter();
        aFilter->SetDisplayMode ((IVtk_DisplayMode)aMode);
        aFilter->Modified();
        aFilter->Update();
      }
    }
  }

  // Redraw window
  GetInteractor()->Render();
  return 0;
}

//================================================================
// Function  : VtkSetBoundaryDraw
// Purpose   :
//================================================================
static Standard_Integer VtkSetBoundaryDraw (Draw_Interpretor& ,
                                            Standard_Integer theArgNum,
                                            const char** theArgs)
{
  if (!GetInteractor()
   || !GetInteractor()->IsEnabled())
  {
    Message::SendFail() << "Error: call ivtkinit before";
    return 1;
  }
  else if (theArgNum != 2 && theArgNum != 3)
  {
    Message::SendFail() << "Syntax error: expects 1 or 2 arguments";
    return 1;
  }

  if (theArgNum == 2)
  {
    // Set disp mode for all objects
    Standard_Boolean toDraw = true;
    Draw::ParseOnOff (theArgs[1], toDraw);
    DoubleMapOfActorsAndNames::Iterator anIter(GetMapOfActors());
    while (anIter.More())
    {
      vtkSmartPointer<vtkActor> anActor = anIter.Key1();
      // Set Red color for boundary edges
      vtkLookupTable* aTable = (vtkLookupTable*)anActor->GetMapper()->GetLookupTable();
      IVtkTools::SetLookupTableColor(aTable, MT_SharedEdge, 1., 0., 0., 1.);

      IVtkTools_ShapeDataSource* aSrc = IVtkTools_ShapeObject::GetShapeSource(anActor);
      if (aSrc)
      {
        IVtkOCC_Shape::Handle anOccShape = aSrc->GetShape();
        if (!anOccShape.IsNull())
        {
          IVtkTools_DisplayModeFilter* aFilter = GetPipeline(anOccShape->GetId())->GetDisplayModeFilter();
          aFilter->SetDisplayMode(DM_Shading);
          aFilter->SetFaceBoundaryDraw(toDraw != 0);
          aFilter->Modified();
          aFilter->Update();
        }
      }
      anIter.Next();
    }
  }
  else
  {
    // Set disp mode for named object
    TCollection_AsciiString aName = theArgs[1];
    vtkSmartPointer<vtkActor> anActor;
    if (!GetMapOfActors().Find2 (aName, anActor))
    {
      Message::SendFail() << "Syntax error: object '" << aName << "' not found";
      return 1;
    }

    Standard_Boolean toDraw = true;
    Draw::ParseOnOff (theArgs[2], toDraw);

    // Set Red color for boundary edges
    vtkLookupTable* aTable = (vtkLookupTable*)anActor->GetMapper()->GetLookupTable();
    IVtkTools::SetLookupTableColor (aTable, MT_SharedEdge, 1., 0., 0., 1.);

    vtkSmartPointer<IVtkTools_ShapeDataSource> aSrc = IVtkTools_ShapeObject::GetShapeSource (anActor);
    if (aSrc)
    {
      IVtkOCC_Shape::Handle anOccShape = aSrc->GetShape();
      if (!anOccShape.IsNull())
      {
        IVtkTools_DisplayModeFilter* aFilter = GetPipeline (anOccShape->GetId())->GetDisplayModeFilter();
        aFilter->SetDisplayMode (DM_Shading);
        aFilter->SetFaceBoundaryDraw (toDraw != 0);
        aFilter->Modified();
        aFilter->Update();
      }
    }
  }

  // Redraw window
  GetInteractor()->Render();
  return 0;
}

//================================================================
// Function  : VtkSetSelectionMode
// Purpose   :
//================================================================
static Standard_Integer VtkSetSelectionMode (Draw_Interpretor& ,
                                             Standard_Integer theArgNum,
                                             const char** theArgs)
{
  if (!GetInteractor()
   || !GetInteractor()->IsEnabled())
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
    const Standard_Integer aMode = Draw::Atoi (theArgs[1]);
    Standard_Boolean isTurnOn = true;
    if (aMode < 0 || aMode > 8 || !Draw::ParseOnOff (theArgs[2], isTurnOn))
    {
      Message::SendFail() << "Syntax error: only 0-8 selection modes are supported";
      return 1;
    }
    DoubleMapOfActorsAndNames::Iterator anIter (GetMapOfActors());
    while (anIter.More())
    {
      vtkSmartPointer<vtkActor> anActor = anIter.Key1();
      if (aMode == SM_Shape && isTurnOn)
      {
        IVtk_SelectionModeList aList = GetPicker()->GetSelectionModes (anActor);
        IVtk_SelectionModeList::Iterator anIt (aList);
        // Turn off all sel modes differed from SM_Shape
        while (anIt.More())
        {
          IVtk_SelectionMode aCurMode = anIt.Value();
          if (SM_Shape != aCurMode)
          {
            GetPicker()->SetSelectionMode (anActor, aCurMode, Standard_False);
          }
          anIt.Next();
        }
        GetPicker()->SetSelectionMode (anActor, SM_Shape);
      }

      if (aMode != SM_Shape)
      {
        if (isTurnOn)
        {
          GetPicker()->SetSelectionMode (anActor, (IVtk_SelectionMode)aMode);
          GetPicker()->SetSelectionMode (anActor, SM_Shape, Standard_False);
          if (aMode == SM_Vertex)
          {
            GetPipeline( IVtkTools_ShapeObject::GetShapeSource(anActor)->GetShape()->GetId() )->SharedVerticesSelectionOn();
          }
        }
        else
        {
          GetPicker()->SetSelectionMode (anActor, (IVtk_SelectionMode)aMode, Standard_False);
          IVtk_SelectionModeList aList = GetPicker()->GetSelectionModes (anActor);
          if (!aList.Size())
          {
            GetPicker()->SetSelectionMode(anActor, SM_Shape);
          }
          if (aMode == SM_Vertex)
          {
            GetPipeline( IVtkTools_ShapeObject::GetShapeSource(anActor)->GetShape()->GetId() )->SharedVerticesSelectionOff();
          }
        }
      }
      anIter.Next();
    }
  }

  if (theArgNum == 4)
  {
    // Set sel mode for named object
    const Standard_Integer aMode = Draw::Atoi (theArgs[2]);
    Standard_Boolean isTurnOn = true;
    if (aMode < 0 || aMode > 8 || !Draw::ParseOnOff (theArgs[3], isTurnOn))
    {
      Message::SendFail() << "Syntax error: only 0-8 selection modes are supported";
      return 1;
    }

    TCollection_AsciiString aName = theArgs[1];
    if (GetMapOfActors().IsBound2 (aName))
    {
      vtkSmartPointer<vtkActor> anActor = GetMapOfActors().Find2 (aName);
      if (aMode == SM_Shape && isTurnOn)
      {
        IVtk_SelectionModeList aList = GetPicker()->GetSelectionModes (anActor);
        IVtk_SelectionModeList::Iterator anIt (aList);
        // Turn off all sel modes differed from SM_Shape
        while (anIt.More())
        {
          IVtk_SelectionMode aCurMode = anIt.Value();
          if (SM_Shape != aCurMode)
          {
            GetPicker()->SetSelectionMode (anActor, aCurMode, Standard_False);
          }
          anIt.Next();
        }
        GetPicker()->SetSelectionMode (anActor, SM_Shape);
      }

      if (aMode != SM_Shape)
      {
        if (isTurnOn)
        {
          GetPicker()->SetSelectionMode (anActor, (IVtk_SelectionMode)aMode);
          GetPicker()->SetSelectionMode (anActor, SM_Shape, Standard_False);
          if (aMode == SM_Vertex)
          {
            GetPipeline( IVtkTools_ShapeObject::GetShapeSource(anActor)->GetShape()->GetId() )->SharedVerticesSelectionOn();
          }
        }
        else
        {
          GetPicker()->SetSelectionMode (anActor, (IVtk_SelectionMode)aMode, Standard_False);
          IVtk_SelectionModeList aList = GetPicker()->GetSelectionModes (anActor);
          if (!aList.Size())
          {
            GetPicker()->SetSelectionMode(anActor, SM_Shape);
          }
          if (aMode == SM_Vertex)
          {
            GetPipeline( IVtkTools_ShapeObject::GetShapeSource(anActor)->GetShape()->GetId() )->SharedVerticesSelectionOff();
          }
        }
      }
    }
  }

  // Redraw window
  GetInteractor()->Render();
  return 0;
}

//================================================================
// Function  : VtkSetColor
// Purpose   :
//================================================================
static Standard_Integer VtkSetColor (Draw_Interpretor& ,
                                     Standard_Integer theArgNb,
                                     const char** theArgVec)
{
  if (!GetInteractor()
   || !GetInteractor()->IsEnabled())
  {
    Message::SendFail() << "Error: call ivtkinit before\n";
    return 1;
  }

  NCollection_Sequence< vtkSmartPointer<vtkActor> > anActorSeq;
  Quantity_Color aQColor;
  bool hasColor = false;
  for (Standard_Integer anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    TCollection_AsciiString anArg (theArgVec[anArgIter]);
    vtkSmartPointer<vtkActor> anActor;
    if (hasColor)
    {
      Message::SendFail() << "Syntax error at '" << anArg << "'";
      return 1;
    }
    else if (GetMapOfActors().Find2 (anArg, anActor))
    {
      anActorSeq.Append (anActor);
    }
    else
    {
      Standard_Integer aNbParsed = Draw::ParseColor (theArgNb  - anArgIter,
                                                     theArgVec + anArgIter,
                                                     aQColor);
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
  const Graphic3d_Vec3 aColor = isSRGBAware ? (Graphic3d_Vec3 )aQColor : Quantity_Color::Convert_LinearRGB_To_sRGB ((Graphic3d_Vec3 )aQColor);
  for (NCollection_Sequence< vtkSmartPointer<vtkActor> >::Iterator anActorIter (anActorSeq); anActorIter.More(); anActorIter.Next())
  {
    const vtkSmartPointer<vtkActor>& anActor = anActorIter.Value();
    vtkLookupTable* aTable = (vtkLookupTable* )anActor->GetMapper()->GetLookupTable();
    IVtkTools::SetLookupTableColor (aTable, MT_ShadedFace, aColor.r(), aColor.g(), aColor.b(), 1.0);
  }

  GetInteractor()->Render();
  return 0;
}

//================================================================
// Function  : VtkSetTransparency
// Purpose   :
//================================================================
static Standard_Integer VtkSetTransparency (Draw_Interpretor& ,
                                            Standard_Integer theArgNb,
                                            const char** theArgVec)
{
  if (!GetInteractor()
   || !GetInteractor()->IsEnabled())
  {
    Message::SendFail() << "Error: call ivtkinit before\n";
    return 1;
  }

  NCollection_Sequence< vtkSmartPointer<vtkActor> > anActorSeq;
  Standard_Real aTransparency = -1.0;
  for (Standard_Integer anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    TCollection_AsciiString anArg (theArgVec[anArgIter]);
    vtkSmartPointer<vtkActor> anActor;
    if (aTransparency >= 0.0)
    {
      Message::SendFail() << "Syntax error at '" << anArg << "'";
      return 1;
    }
    else if (GetMapOfActors().Find2 (anArg, anActor))
    {
      anActorSeq.Append (anActor);
    }
    else if (!Draw::ParseReal (theArgVec[anArgIter], aTransparency)
           || aTransparency < 0.0
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

  for (NCollection_Sequence< vtkSmartPointer<vtkActor> >::Iterator anActorIter (anActorSeq); anActorIter.More(); anActorIter.Next())
  {
    const vtkSmartPointer<vtkActor>& anActor = anActorIter.Value();
    anActor->GetProperty()->SetOpacity (1.0 - aTransparency);
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
                                  Standard_Integer theArgNum,
                                  const char** theArgs)
{
  if (!GetInteractor()
   || !GetInteractor()->IsEnabled())
  {
    Message::SendFail() << "Error: call ivtkinit before";
    return 1;
  }
  else if (theArgNum != 3)
  {
    Message::SendFail() << "Syntax error: expects 2 arguments";
    return 1;
  }

  Standard_Integer anY = GetInteractor()->GetRenderWindow()->GetSize()[1] - atoi (theArgs[2]) - 1;
  GetInteractor()->MoveTo (atoi (theArgs[1]), anY);

  gp_XYZ aPickPnt;
  GetInteractor()->Selector()->GetPickPosition (aPickPnt.ChangeData());
  theDI << aPickPnt.X() << " " << aPickPnt.Y() << " " << aPickPnt.Z();
  return 0;
}

//================================================================
// Function  : VtkSelect
// Purpose   :
//================================================================
static Standard_Integer VtkSelect (Draw_Interpretor& ,
                                   Standard_Integer theArgNum,
                                   const char** theArgs)
{
  if (!GetInteractor()
   || !GetInteractor()->IsEnabled())
  {
    Message::SendFail() << "Error: call ivtkinit before";
    return 1;
  }
  else if (theArgNum != 3)
  {
    Message::SendFail() << "Syntax error: expects 3 arguments";
    return 1;
  }

  Standard_Integer anY = GetInteractor()->GetRenderWindow()->GetSize()[1] - atoi (theArgs[2]) - 1;
  GetInteractor()->MoveTo (atoi (theArgs[1]), anY);
  GetInteractor()->OnSelection();
  return 0;

}

//===================================================================
// Fubction  : VtkViewProj
// Purpose   :
//===================================================================
static Standard_Integer VtkViewProj (Draw_Interpretor& ,
                                     Standard_Integer theNbArgs,
                                     const char** theArgVec)
{
  if (!GetInteractor()
   || !GetInteractor()->IsEnabled())
  {
    Message::SendFail() << "Error: call ivtkinit before";
    return 1;
  }
  else if (theNbArgs != 1)
  {
    Message::SendFail() << "Syntax error: wrong number of arguments";
    return 1;
  }

  TCollection_AsciiString aCmd (theArgVec[0]);
  aCmd.LowerCase();

  V3d_TypeOfOrientation aProj = V3d_Xpos;
  bool hasProjDir = false;
  if (aCmd == "ivtkaxo")
  {
    hasProjDir = true;
    aProj = V3d_TypeOfOrientation_Zup_AxoRight;
  }
  else if (aCmd == "ivtktop")
  {
    hasProjDir = true;
    aProj = V3d_TypeOfOrientation_Zup_Top;
  }
  else if (aCmd == "ivtkbottom")
  {
    hasProjDir = true;
    aProj = V3d_TypeOfOrientation_Zup_Bottom;
  }
  else if (aCmd == "ivtkfront")
  {
    hasProjDir = true;
    aProj = V3d_TypeOfOrientation_Zup_Front;
  }
  else if (aCmd == "ivtkback")
  {
    hasProjDir = true;
    aProj = V3d_TypeOfOrientation_Zup_Back;
  }
  else if (aCmd == "ivtkleft")
  {
    hasProjDir = true;
    aProj = V3d_TypeOfOrientation_Zup_Left;
  }
  else if (aCmd == "ivtkright")
  {
    hasProjDir = true;
    aProj = V3d_TypeOfOrientation_Zup_Right;
  }

  if (hasProjDir)
  {
    const gp_Dir aBck = V3d::GetProjAxis (aProj);
    Graphic3d_Vec3d anUp (0.0, 0.0, 1.0);
    if (aProj == V3d_Zpos)
    {
      anUp.SetValues (0.0, 1.0, 0.0);
    }
    else if (aProj == V3d_Zneg)
    {
      anUp.SetValues (0.0, -1.0, 0.0);
    }

    vtkCamera* aCam = GetRenderer()->GetActiveCamera();
    const double aDist = aCam->GetDistance();

    Graphic3d_Vec3d aNewEye = Graphic3d_Vec3d (aBck.X(), aBck.Y(), aBck.Z()) * aDist;
    aCam->SetPosition (aNewEye.x(), aNewEye.y(), aNewEye.z());
    aCam->SetFocalPoint (0.0, 0.0, 0.0);
    aCam->SetViewUp (anUp.x(), anUp.y(), anUp.z());
    aCam->OrthogonalizeViewUp();
  }

  GetRenderer()->ResetCamera();
  GetInteractor()->Render();
  return 0;
}

//===================================================================
// Fubction  : VtkDump
// Purpose   :
//===================================================================
static Standard_Integer VtkDump (Draw_Interpretor& ,
                                 Standard_Integer theArgNum,
                                 const char** theArgs)
{
  if (!GetInteractor()
   || !GetInteractor()->IsEnabled())
  {
    Message::SendFail() << "Error : call ivtkinit before";
    return 1;
  }
  else if (theArgNum < 2)
  {
    Message::SendFail() << "Syntax error: wrong number of parameters";
    return 1;
  }

  vtkSmartPointer<vtkWindowToImageFilter> anImageFilter = vtkSmartPointer<vtkWindowToImageFilter>::New();
  anImageFilter->SetInput (GetInteractor()->GetRenderWindow());
  // Set custom buffer type
  if (theArgNum > 2)
  {
    TCollection_AsciiString aBufferType (theArgs[2]);
    aBufferType.LowerCase();
    if (aBufferType.IsEqual ("rgb"))
    {
      anImageFilter->SetInputBufferTypeToRGB();
    }
    else if (aBufferType.IsEqual ("rgba"))
    {
      anImageFilter->SetInputBufferTypeToRGBA();
    }
    else if (aBufferType.IsEqual ("depth"))
    {
      anImageFilter->SetInputBufferTypeToZBuffer();
    }
  }
  anImageFilter->Update();

  // Set custom stereo projection options
  if (theArgNum > 5 && GetRenderer()->GetRenderWindow()->GetStereoRender())
  {
    Standard_CString aStereoProjStr = theArgs[5];

    Standard_Integer aStereoType =  GetRenderer()->GetRenderWindow()->GetStereoType();
    if (strcasecmp (aStereoProjStr, "L"))
    {
      GetRenderer()->GetRenderWindow()->SetStereoTypeToLeft();
      GetRenderer()->GetRenderWindow()->StereoUpdate();
      anImageFilter->Update();
      GetRenderer()->GetRenderWindow()->SetStereoType (aStereoType);
    }
    else if (strcasecmp (aStereoProjStr, "R"))
    {
      GetRenderer()->GetRenderWindow()->SetStereoTypeToRight();
      GetRenderer()->GetRenderWindow()->StereoUpdate();
      anImageFilter->Update();
      GetRenderer()->GetRenderWindow()->SetStereoType (aStereoType);
    }
    else
    {
      Message::SendFail() << "Syntax error: unknown value for stereo projection";
      return 1;
    }
  }

  // Set parameters for image writer
  vtkSmartPointer<vtkImageWriter> anImageWriter;
  TCollection_AsciiString aFilename (theArgs[1]);
  Standard_Integer anExtStart = aFilename.SearchFromEnd (TCollection_AsciiString("."));
  TCollection_AsciiString aFormat = (anExtStart == -1) ? TCollection_AsciiString("")
                                    : aFilename.SubString (anExtStart + 1, aFilename.Length());
  aFormat.LowerCase();

  if (aFormat.IsEqual ("png"))
  {
    anImageWriter = vtkSmartPointer<vtkPNGWriter>::New();
  }
  else if (aFormat.IsEqual ("bmp"))
  {
    anImageWriter = vtkSmartPointer<vtkBMPWriter>::New();
  }
  else if (aFormat.IsEqual ("jpeg"))
  {
    anImageWriter = vtkSmartPointer<vtkJPEGWriter>::New();
  }
  else if (aFormat.IsEqual ("tiff"))
  {
    anImageWriter = vtkSmartPointer<vtkTIFFWriter>::New();
  }
  else if (aFormat.IsEqual ("pnm"))
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
      aFormat = TCollection_AsciiString ("png");
      if (anExtStart != -1)
      {
        aFilename.Split (anExtStart);
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

  anImageWriter->SetFileName (aFilename.ToCString());

  Standard_Integer aWidth = (theArgNum > 3) ? atoi (theArgs[3]) : 0;
  Standard_Integer aHeight = (theArgNum > 4) ? atoi (theArgs[4]) : 0;
  if (aWidth >= 0 || aHeight >= 0)
  {
    // Scale image
    vtkSmartPointer<vtkImageResize> anImageResize = vtkSmartPointer<vtkImageResize>::New();
#if VTK_MAJOR_VERSION <= 5
    anImageResize->SetInput (anImageFilter->GetOutput());
#else
    anImageResize->SetInputData (anImageFilter->GetOutput());
#endif

    anImageResize->SetOutputDimensions (aWidth, aHeight, 1);
    anImageResize->Update();
    anImageWriter->SetInputConnection (anImageResize->GetOutputPort());
  }
  else
  {
    anImageWriter->SetInputConnection (anImageFilter->GetOutputPort());
  }
  anImageWriter->Write();
  return 0;
}

//===================================================================
// Fubction  : VtkBackgroundColor
// Purpose   :
//===================================================================
static Standard_Integer VtkBackgroundColor (Draw_Interpretor& ,
                                            Standard_Integer theArgNum,
                                            const char** theArgs)
{
  if (!GetInteractor()
   || !GetInteractor()->IsEnabled())
  {
    Message::SendFail() << "Error: call ivtkinit before";
    return 1;
  }

  Quantity_Color aQColor1;
  const Standard_Integer aNbParsed1 = Draw::ParseColor (theArgNum - 1, theArgs + 1, aQColor1);
  if (aNbParsed1 == 0)
  {
    Message::SendFail() << "Syntax error: wrong number of parameters";
    return 1;
  }

  bool isSRGBAware = false;
#ifdef HAVE_VTK_SRGB
  isSRGBAware = GetRenderer()->GetRenderWindow()->GetUseSRGBColorSpace();
#endif
  const Graphic3d_Vec3 aColor1 = isSRGBAware ? (Graphic3d_Vec3 )aQColor1 : Quantity_Color::Convert_LinearRGB_To_sRGB ((Graphic3d_Vec3 )aQColor1);
  GetRenderer()->SetGradientBackground(false);
  GetRenderer()->SetBackground (aColor1.r(), aColor1.g(), aColor1.b());
  if (theArgNum - 1 > aNbParsed1)
  {
    Quantity_Color aQColor2;
    const Standard_Integer aNbParsed2 = Draw::ParseColor (theArgNum - 1 - aNbParsed1, theArgs + 1 + aNbParsed1, aQColor2);
    if (aNbParsed2 == 0)
    {
      Message::SendFail() << "Syntax error: wrong number of parameters";
      return 1;
    }

    const Graphic3d_Vec3 aColor2 = isSRGBAware ? (Graphic3d_Vec3 )aQColor2 : Quantity_Color::Convert_LinearRGB_To_sRGB ((Graphic3d_Vec3 )aQColor2);
    GetRenderer()->SetBackground2 (aColor2.r(), aColor2.g(), aColor2.b());
    GetRenderer()->SetGradientBackground (true);
  }

  GetInteractor()->Render();
  return 0;
}

//================================================================
// Function : Commands
// Purpose  : 
//================================================================
void IVtkDraw::Commands (Draw_Interpretor& theCommands)
{
  const char *group = "VtkViewer";

  theCommands.Add("ivtkinit",
              "ivtkinit [leftPx topPx widthPx heightPx] [-srgb {on|off}] [-msaa NbSamples]"
      "\n\t\t: Creates the Vtk window."
      "\n\t\t:  -srgb Enable/disable sRGB colorspace; OFF by default."
      "\n\t\t:  -msaa Requests desired number of samples for multisampling buffer;"
      "\n\t\t:        0 by default meaning no MSAA.",
    __FILE__, VtkInit, group);

  theCommands.Add("ivtkclose",
              "ivtkclose : Closes the Vtk window.",
    __FILE__, VtkClose, group);

  theCommands.Add("ivtkrenderparams",
              "ivtkrenderparams [-depthPeeling NbLayers] [-shadows {on|off}]"
      "\n\t\t: Sets Vtk rendering parameters."
      "\n\t\t:  -shadows      Turn shadows on/off; OFF by default."
      "\n\t\t:  -depthPeeling Enable/disable depth peeling for rendering transparent objects"
      "\n\t\t:                with specified number of layers;"
      "\n\t\t:                0 by default meaning order-dependent transparency without depth peeling.",
    __FILE__, VtkRenderParams, group);

  theCommands.Add("ivtkdisplay",
              "ivtkdisplay name1 [name2 ...]"
      "\n\t\t: Displays named objects in Vtk view.",
    __FILE__, VtkDisplay, group);

  theCommands.Add("ivtkerase",
              "ivtkerase [name1 name2 ...]"
      "\n\t\t: Hides in Vtk renderer named objects or all objects.",
    __FILE__, VtkErase, group);

  theCommands.Add("ivtkremove",
              "ivtkremove name1 [name2 ...]"
      "\n\t\t: Removes from Vtk renderer named objects.",
    __FILE__, VtkRemove, group);

  theCommands.Add("ivtkclear",
              "ivtkclear : Removes all objects from Vtk renderer.",
    __FILE__, VtkRemove, group);

  theCommands.Add("ivtksetdispmode",
              "ivtksetdispmode [name] mode={0|1}"
      "\n\t\t: Sets or unsets display mode 'mode' to the object with name 'name' or to all objects.",
    __FILE__, VtkSetDisplayMode, group);

  theCommands.Add("ivtksetboundingdraw",
              "ivtksetboundingdraw [name] {on|off}"
      "\n\t\t: Sets or unsets boundaries drawing for shading display mode"
      "\n\t\t: to the object with name 'name' or to all objects.",
    __FILE__, VtkSetBoundaryDraw, group);

  theCommands.Add("ivtksetselmode",
              "ivtksetselmode [name] mode {on|off}"
      "\n\t\t: Sets or unsets selection mode 'mode' to the object with name 'name'"
      "\n\t\t: or to the all displayed objects.",
    __FILE__, VtkSetSelectionMode, group);

  theCommands.Add("ivtkmoveto",
              "ivtkmoveto x y"
      "\n\t\t: Moves position to the pixel with coordinates (x,y). The object on this position is highlighted.",
    __FILE__, VtkMoveTo, group);

  theCommands.Add("ivtkselect",
              "ivtkselect x y"
      "\n\t\t: Selects object which correspond to the pixel with input coordinates (x,y).",
    __FILE__, VtkSelect, group);

  theCommands.Add("ivtkfit",
              "ivtkfit : Fits all displayed objects into Vtk view.",
    __FILE__, VtkViewProj, group);

  theCommands.Add("ivtkaxo",
              "ivtkaxo : Resets Vtk view orientation to axo",
    __FILE__, VtkViewProj, group);

  theCommands.Add("ivtkfront",
              "ivtkfront : Resets Vtk view orientation to front",
    __FILE__, VtkViewProj, group);

  theCommands.Add("ivtkback",
              "ivtkback : Resets Vtk view orientation to back",
    __FILE__, VtkViewProj, group);

  theCommands.Add("ivtktop",
              "ivtktop : Resets Vtk view orientation to top",
    __FILE__, VtkViewProj, group);

  theCommands.Add("ivtkbottom",
              "ivtkbottom : Resets Vtk view orientation to bottom",
    __FILE__, VtkViewProj, group);

  theCommands.Add("ivtkleft",
              "ivtkleft : Resets Vtk view orientation to left",
    __FILE__, VtkViewProj, group);

  theCommands.Add("ivtkright",
              "ivtkright : Resets Vtk view orientation to right",
    __FILE__, VtkViewProj, group);

  theCommands.Add("ivtkdump",
              "ivtkdump <FullFilename>.{png|bmp|jpeg|tiff|pnm} [buffer={rgb|rgba|depth}]"
      "\n\t\t:          [width height] [stereoproj={L|R}]"
      "\n\t\t: Dumps contents of viewer window to PNG, BMP, JPEG, TIFF or PNM file.",
    __FILE__, VtkDump, group);

  theCommands.Add("ivtkbgcolor",
              "ivtkbgcolor Color1 [Color2]"
      "\n\t\t: Sets uniform background color or gradient one if second triple of parameters is set.",
    __FILE__, VtkBackgroundColor, group);

  theCommands.Add("ivtksetcolor",
              "ivtksetcolor name {ColorName|R G B}"
      "\n\t\t: Sets color to the object with name 'name'.",
    __FILE__, VtkSetColor, group);

  theCommands.Add("ivtksettransparency",
              "ivtksettransparency name 0..1"
      "\n\t\t: Sets transparency to the object with name 'name'.",
    __FILE__, VtkSetTransparency, group);
}

//================================================================
// Function : Factory
// Purpose  : 
//================================================================
void IVtkDraw::Factory (Draw_Interpretor& theDI)
{
  // definition of Viewer Commands
  IVtkDraw::Commands (theDI);
}

// Declare entry point PLUGINFACTORY
DPLUGIN(IVtkDraw)
