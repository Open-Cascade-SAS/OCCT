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


#ifdef NOMINMAX
#undef NOMINMAX    /* avoid #define min() and max() */
#endif
#ifdef NOMSG
#undef NOMSG       /* avoid #define SendMessage etc. */
#endif
#ifdef NODRAWTEXT
#undef NODRAWTEXT  /* avoid #define DrawText etc. */
#endif
#ifdef NONLS
#undef NONLS       /* avoid #define CompareString etc. */
#endif
#ifdef NOGDI
#undef NOGDI       /* avoid #define SetPrinter (winspool.h) etc. */
#endif
#ifdef NOSERVICE
#undef NOSERVICE    
#endif
#ifdef NOKERNEL
#undef NOKERNEL 
#endif
#ifdef NOUSER
#undef NOUSER 
#endif
#ifdef NOMCX
#undef NOMCX 
#endif
#ifdef NOIME
#undef NOIME 
#endif

#include <stdio.h>
#ifdef HAVE_STRINGS_H
# include <strings.h>
#endif

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
#include <GL/glx.h>
#include <Xw_Window.hxx>
#include <vtkXRenderWindowInteractor.h>
#include <vtkXOpenGLRenderWindow.h>
#include <X11/Xutil.h>
#include <tk.h>
#endif

// workaround name conflicts with OCCT methods (in class TopoDS_Shape for example)
#ifdef Convex
  #undef Convex
#endif
#ifdef Status
  #undef Status
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
const Handle(Standard_Transient)& IVtkDraw::WClass()
{
  static Handle(Standard_Transient) aWindowClass;
#ifdef _WIN32
  if (aWindowClass.IsNull())
  {
    aWindowClass = new WNT_WClass ("GWVTK_Class", DefWindowProc,
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
void IVtkDraw::ViewerInit (Standard_Integer thePxLeft,
                           Standard_Integer thePxTop,
                           Standard_Integer thePxWidth,
                           Standard_Integer thePxHeight)
{
  static Standard_Boolean isFirst = Standard_True;

  Standard_Integer aPxLeft   = 0;
  Standard_Integer aPxTop    = 460;
  Standard_Integer aPxWidth  = 409;
  Standard_Integer aPxHeight = 409;

  if (thePxLeft != 0)
  {
    aPxLeft = thePxLeft;
  }
  if (thePxTop != 0)
  {
    aPxTop = thePxTop;
  }
  if (thePxWidth != 0)
  {
    aPxWidth = thePxWidth;
  }
  if (thePxHeight != 0)
  {
    aPxHeight = thePxHeight;
  }
  
  if (isFirst)
  {
    SetDisplayConnection (new Aspect_DisplayConnection ());
#ifdef _WIN32
    if (GetWindow().IsNull())
    {
      GetWindow() = new WNT_Window ("IVtkTest",
                                    Handle(WNT_WClass)::DownCast (WClass()),
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

    isFirst = Standard_False;
  }

  GetWindow()->Map();
}

//================================================================
// Function : VtkInit
// Purpose  : 
//================================================================
static Standard_Integer VtkInit (Draw_Interpretor& ,
                                 Standard_Integer theArgNum,
                                 const char** theArgs)
{
  Standard_Integer aPxLeft   = (theArgNum > 1) ? atoi(theArgs[1]) : 0;
  Standard_Integer aPxTop    = (theArgNum > 2) ? atoi(theArgs[2]) : 0;
  Standard_Integer aPxWidth  = (theArgNum > 3) ? atoi(theArgs[3]) : 0;
  Standard_Integer aPxHeight = (theArgNum > 4) ? atoi(theArgs[4]) : 0;

  IVtkDraw::ViewerInit (aPxLeft, aPxTop, aPxWidth, aPxHeight);
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

static Standard_Integer VtkDisplay (Draw_Interpretor& theDI,
                                    Standard_Integer theArgNum,
                                    const char** theArgs)
{
  // Check viewer
  if (!GetInteractor()->IsEnabled())
  {
    theDI << theArgs[0] << " error : call ivtkinit before\n";
    return 1; // TCL_ERROR
  }

  // Check arguments
  if (theArgNum < 2)
  {
    theDI << theArgs[0] << " error : expects at least 1 argument\n";
    return 1; // TCL_ERROR
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
static Standard_Integer VtkErase (Draw_Interpretor& theDI,
                                  Standard_Integer theArgNum,
                                  const char** theArgs)
{
  // Check viewer
  if (!GetInteractor()->IsEnabled())
  {
    theDI << theArgs[0] << " error : call ivtkinit before\n";
    return 1; // TCL_ERROR
  }

  vtkSmartPointer<vtkRenderer> aRenderer = GetRenderer();
  // Erase all objects
  if (theArgNum == 1)
  {
    DoubleMapOfActorsAndNames::Iterator anIterator (GetMapOfActors());
    while (anIterator.More())
    {
      PipelineByActor (anIterator.Key1())->RemoveFromRenderer (aRenderer);
      anIterator.Next();
    }
  }
  // Erase named objects
  else
  {
    TCollection_AsciiString aName;
    for (Standard_Integer anIndex = 1; anIndex < theArgNum; ++anIndex)
    {
      aName = theArgs[anIndex];
      if (GetMapOfActors().IsBound2 (aName))
      {
        PipelineByActorName (aName)->RemoveFromRenderer (aRenderer);
      }
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
static Standard_Integer VtkRemove(Draw_Interpretor& theDI,
  Standard_Integer theArgNum,
  const char** theArgs)
{
  // Check viewer
  if (!GetInteractor()->IsEnabled())
  {
    theDI << theArgs[0] << " error : call ivtkinit before\n";
    return 1; // TCL_ERROR
  }

  vtkSmartPointer<vtkRenderer> aRenderer = GetRenderer();

  // Remove all objects
  if (theArgNum == 1)
  {
    // Remove all actors from the renderer
    DoubleMapOfActorsAndNames::Iterator anIterator(GetMapOfActors());
    while (anIterator.More())
    {
      vtkSmartPointer<IVtkTools_ShapeDataSource> aSrc =
        IVtkTools_ShapeObject::GetShapeSource(anIterator.Key1());
      if (aSrc.GetPointer() != NULL && !(aSrc->GetShape().IsNull()))
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
  // Remove named objects
  else
  {
    TCollection_AsciiString aName;
    for (Standard_Integer anIndex = 1; anIndex < theArgNum; ++anIndex)
    {
      aName = theArgs[anIndex];
      if (GetMapOfActors().IsBound2(aName))
      {
        // Remove the actor and its pipeline (if found) from the renderer
        vtkSmartPointer<vtkActor> anActor = GetMapOfActors().Find2(aName);
        vtkSmartPointer<IVtkTools_ShapeDataSource> aSrc = 
          IVtkTools_ShapeObject::GetShapeSource(anActor);
        if (aSrc.GetPointer() != NULL && !(aSrc->GetShape().IsNull()))
        {
          IVtk_IdType aShapeID = aSrc->GetShape()->GetId();
          GetPicker()->RemoveSelectableObject(aSrc->GetShape());
          GetPipeline(aSrc->GetShape()->GetId())->RemoveFromRenderer(aRenderer);
          GetPipelines()->UnBind(aShapeID); // Remove a pipepline
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
static Standard_Integer VtkSetDisplayMode (Draw_Interpretor& theDI,
                                           Standard_Integer theArgNum,
                                           const char** theArgs)
{
  // Check viewer
  if (!GetInteractor()->IsEnabled())
  {
    theDI << theArgs[0] << " error: call ivtkinit before\n";
    return 1; // TCL_ERROR
  }

  // Check arguments
  if (theArgNum != 2 && theArgNum != 3)
  {
    theDI << theArgs[0] << " error: expects 1 or 2 arguments\n";
    return 1; // TCL_ERROR
  }

  vtkSmartPointer<vtkActor> anActor;
  // Set disp mode for all objects
  if (theArgNum == 2)
  {
    // Get mode
    Standard_Integer aMode =  Draw::Atoi (theArgs[1]);
    DoubleMapOfActorsAndNames::Iterator anIter (GetMapOfActors());
    while (anIter.More())
    {
      anActor = anIter.Key1();
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
    Standard_Integer aMode = atoi(theArgs[2]);
    TCollection_AsciiString aName = theArgs[1];
    if (GetMapOfActors().IsBound2 (aName))
    {
      anActor = GetMapOfActors().Find2 (aName);
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
  }

  // Redraw window
  GetInteractor()->Render();
  return 0;
}

//================================================================
// Function  : VtkSetSelectionMode
// Purpose   : 
// Draw args : ivtksetselmode [name] mode on/off(0,1)
//================================================================
static Standard_Integer VtkSetSelectionMode (Draw_Interpretor& theDI,
                                             Standard_Integer theArgNum,
                                             const char** theArgs)
{
  // Check viewer
  if (!GetInteractor()->IsEnabled())
  {
    theDI << theArgs[0] << " error: call ivtkinit before\n";
    return 1; // TCL_ERROR
  }

  // Check arguments
  if (theArgNum != 3 && theArgNum != 4)
  {
    theDI << theArgs[0] << " error: expects 2 or 3 arguments\n";
    return 1; // TCL_ERROR
  }

  vtkSmartPointer<vtkActor> anActor;
  Standard_Integer aMode;
  Standard_Boolean isTurnOn;
  // Set sel mode for all objects
  if (theArgNum == 3)
  {
    aMode = atoi (theArgs[1]);
    isTurnOn = (atoi (theArgs[2]) != 0);
    if (aMode < 0 || aMode > 8)
    {
      theDI << theArgs[0] << " error: only 0-8 selection modes are supported\n";
      return 1; // TCL_ERROR
    }
    DoubleMapOfActorsAndNames::Iterator anIter (GetMapOfActors());
    while (anIter.More())
    {
      anActor = anIter.Key1();

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

  // Set sel mode for named object
  if (theArgNum == 4)
  {
    aMode = atoi (theArgs[2]);
    isTurnOn = (atoi (theArgs[3]) != 0);

    if (aMode < 0 || aMode > 8)
    {
      theDI << theArgs[0] << " error: only 0-8 selection modes are supported\n";
      return 1; // TCL_ERROR
    }

    TCollection_AsciiString aName = theArgs[1];
    if (GetMapOfActors().IsBound2 (aName))
    {
      anActor = GetMapOfActors().Find2 (aName);

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
// Function  : VtkMoveTo
// Purpose   : 
// Draw args : ivtkmoveto x y
//================================================================
static Standard_Integer VtkMoveTo(Draw_Interpretor& theDI,
                                  Standard_Integer theArgNum,
                                  const char** theArgs)
{
  // Check viewer
  if (!GetInteractor()->IsEnabled())
  {
    theDI << theArgs[0] << " error: call ivtkinit before\n";
    return 1; // TCL_ERROR
  }

  // Check args
  if (theArgNum != 3)
  {
    theDI << theArgs[0] << " error: expects 2 arguments\n";
    return 1; // TCL_ERROR
  }

  Standard_Integer anY = GetInteractor()->GetRenderWindow()->GetSize()[1] - atoi (theArgs[2]) - 1;
  GetInteractor()->MoveTo (atoi (theArgs[1]), anY);
  return 0;
}

//================================================================
// Function  : VtkSelect
// Purpose   : 
// Draw args : ivtkselect x y
//================================================================
static Standard_Integer VtkSelect (Draw_Interpretor& theDI,
                                   Standard_Integer theArgNum,
                                   const char** theArgs)
{
  // Check viewer
  if (!GetInteractor()->IsEnabled())
  {
    theDI << theArgs[0] << " error: call ivtkinit before\n";
    return 1; // TCL_ERROR
  }

  // Check args
  if (theArgNum != 3)
  {
    theDI << theArgs[0] << " error: expects 3 arguments\n";
    return 1; // TCL_ERROR
  }

  Standard_Integer anY = GetInteractor()->GetRenderWindow()->GetSize()[1] - atoi (theArgs[2]) - 1;
  GetInteractor()->MoveTo (atoi (theArgs[1]), anY);
  GetInteractor()->OnSelection();
  return 0;

}

//===================================================================
// Fubction  : VtkFit
// Purpose   :
// Draw args : ivtkfit
//===================================================================

static Standard_Integer VtkFit (Draw_Interpretor& theDI,
                                Standard_Integer,
                                const char** theArgs)
{
  // Check viewer
  if (!GetInteractor()->IsEnabled())
  {
    theDI << theArgs[0] << " error : call ivtkinit before \n";
    return 1; //TCL_ERROR
  }

  GetRenderer()->ResetCamera();
  GetInteractor()->Render();
  return 0;
}

//===================================================================
// Fubction  : VtkDump
// Purpose   :
// Draw args : ivtkdump FullFilename.{png|bmp|jpeg|tiff|pnm} 
//                      [buffer={rgb|rgba|depth}] [width height]
//                      [stereoproj={L|R}]
//===================================================================
static Standard_Integer VtkDump (Draw_Interpretor& theDI,
                                 Standard_Integer theArgNum,
                                 const char** theArgs)
{
  // Check viewer
  if (!GetInteractor()->IsEnabled())
  {
    std::cout << theArgs[0] << " error : call ivtkinit before \n";
    return 1;
  }

  if (theArgNum < 2)
  {
    theDI << theArgs[0] << " error : wrong number of parameters. Type 'help"
          << theArgs[0] << "' for more information.\n";
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
      theDI << theArgs[0] << " error: unknown value for stereo projection.\n";
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
      theDI << theArgs[0] << " warning: the image format is not set.\n"
            << "The image will be saved into PNG format.\n";
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
      theDI << theArgs[0] << " error: the image format "
            << aFormat.ToCString() <<" is not supported.\n";
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
// Draw args : ivtkbgcolor r g b
//             r,g,b = [0..255]
//===================================================================
static Standard_Integer VtkBackgroundColor (Draw_Interpretor& theDI,
                                            Standard_Integer theArgNum,
                                            const char** theArgs)
{
  if (theArgNum != 4 && theArgNum != 7)
  {
    theDI << theArgs[0] << " error : wrong number of parameters.\n"
          << "Type 'help " << theArgs[0] << "' for more information.\n";
    return 1;
  }

  // Check viewer
  if (!GetInteractor()->IsEnabled())
  {
    std::cout << theArgs[0] << " error : call ivtkinit before \n";
    return 1;
  }

  Standard_Real aR = Draw::Atof(theArgs[1])/255.0;
  Standard_Real aG = Draw::Atof(theArgs[2])/255.0;
  Standard_Real aB = Draw::Atof(theArgs[3])/255.0;

  GetRenderer()->SetGradientBackground(false);
  GetRenderer()->SetBackground (aR, aG, aB);

  if (theArgNum == 7)
  {
    Standard_Real aR2 = Draw::Atof(theArgs[4])/255.0;
    Standard_Real aG2 = Draw::Atof(theArgs[5])/255.0;
    Standard_Real aB2 = Draw::Atof(theArgs[6])/255.0;

    GetRenderer()->SetBackground2(aR2, aG2, aB2);
    GetRenderer()->SetGradientBackground(true);
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
    "ivtkinit usage:\n"
    "ivtkinit [leftPx topPx widthPx heightPx]"
    "\n\t\t: Creates the Vtk window",
    __FILE__, VtkInit, group);

  theCommands.Add("ivtkdisplay",
    "ivtkdisplay usage:\n"
    "ivtkdisplay name1 name2 ..."
    "\n\t\t: Displayes named objects in current view.",
    __FILE__, VtkDisplay, group);

  theCommands.Add("ivtkerase",
    "ivtkerase usage:\n"
    "ivtkerase [name1 name2 ...]"
    "\n\t\t: Removes from renderer named objects or all objects.",
    __FILE__, VtkErase, group);

  theCommands.Add("ivtkremove",
    "ivtkremove usage:\n"
    "ivtkremove [name1 name2 ...]"
    "\n\t\t: Removes from renderer and from memory named objects or all objects.",
    __FILE__, VtkRemove, group);

  theCommands.Add("ivtksetdispmode",
    "ivtksetdispmode usage:\n"
    "ivtksetdispmode [name] mode (0,1)"
    "\n\t\t: Sets or unsets display mode 'mode' to the object with name 'name' or to all objects"
    "if name is not defined.",
    __FILE__, VtkSetDisplayMode, group);

  theCommands.Add("ivtksetselmode",
    "ivtksetselmode usage:\n"
    " ivtksetselmode [name] mode on/off(0,1)"
    "\n\t\t: Sets or unsets selection mode 'mode' to the object with name 'name' or to the all displayed objects.",
    __FILE__, VtkSetSelectionMode, group);

  theCommands.Add("ivtkmoveto",
    "ivtkmoveto usage:\n"
    "ivtkmoveto x y"
    "\n\t\t: Moves position to the pixel with coordinates (x,y). The object on this position is highlighted.",
    __FILE__, VtkMoveTo, group);

  theCommands.Add("ivtkselect",
    "ivtkselect usage:\n"
    "ivtkselect x y"
    "\n\t\t: Selects object which correspond to the pixel with input coordinates (x,y).",
    __FILE__, VtkSelect, group);

  theCommands.Add("ivtkfit",
    "ivtkfit usage:\n"
    "ivtkfit"
    "\n\t\t: Fits view according all displayed objects.",
    __FILE__, VtkFit, group);

  theCommands.Add("ivtkdump",
    "ivtkdump usage:\n"
    "ivtkdump <FullFilename>.{png|bmp|jpeg|tiff|pnm} [buffer={rgb|rgba|depth}] [width height] [stereoproj={L|R}]"
    "\n\t\t: Dumps contents of viewer window to PNG, BMP, JPEG, TIFF or PNM file",
    __FILE__, VtkDump, group);

  theCommands.Add("ivtkbgcolor",
    "ivtkbgcolor usage:\n"
    "ivtkbgcolor r g b [r2 g2 b2]\n"
    "\n\t\t: Sets uniform  background color or gradient one if second triple of paramers is set."
    "Color parameters r,g,b = [0..255].",
    __FILE__, VtkBackgroundColor, group);
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

