// Created on: 2002-02-04
// Created by: QA Admin
// Copyright (c) 2002-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



#if defined(WNT)
#include <windows.h>
#endif
#include <QADraw.hxx>
#include <QADraw_DataMapOfAsciiStringOfAddress.hxx>
#include <Draw_Interpretor.hxx>
#include <Image_AlienPixMap.hxx>
#include <V3d_View.hxx>
#include <ViewerTest.hxx>
#include <ViewerTest_EventManager.hxx>
#include <TColStd_StackOfInteger.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_HSequenceOfReal.hxx>
#include <AIS_InteractiveContext.hxx>
#include <Draw.hxx>
#include <Draw_Window.hxx>
#include <Draw_Viewer.hxx>
#include <Aspect_WindowDriver.hxx>
#include <stdio.h>
#include <Aspect_DisplayConnection.hxx>
#include <Graphic3d.hxx>

#if ! defined(WNT)
#include <Xw_Window.hxx>
//#include <Xm/Xm.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <Xw_Cextern.hxx>
#include <unistd.h>
#else
#include <WNT.h>
#include <WNT_Window.hxx>
#include <io.h>
#endif

#include <tcl.h>

#if ! defined(STDOUT_FILENO)
#define STDOUT_FILENO fileno(stdout)
#endif

// mkv 15.07.03
#if ! defined(STDERR_FILENO)
#define STDERR_FILENO fileno(stderr)
#endif

Draw_Interpretor *thePCommands = NULL;

#if ! defined(WNT)
//extern Draw_Interpretor theCommands;

extern void (*Draw_BeforeCommand)();
extern void (*Draw_AfterCommand)(Standard_Integer);
#else
//Standard_EXPORT Draw_Interpretor theCommands;

Standard_EXPORT void (*Draw_BeforeCommand)();
Standard_EXPORT void (*Draw_AfterCommand)(Standard_Integer);
#endif


#include <Draw_PluginMacro.hxx>

// avoid warnings on 'extern "C"' functions returning C++ classes
#ifdef WNT
#pragma warning(4:4190)
#endif


int st_err = 0;

void (*Draw_BeforeCommand_old)();
void (*Draw_AfterCommand_old)(Standard_Integer);

static Standard_Boolean should_be_printed = Standard_True;

static Standard_Boolean shouldDUP() {
  // MKV 30.03.05
#if ((TCL_MAJOR_VERSION > 8) || ((TCL_MAJOR_VERSION == 8) && (TCL_MINOR_VERSION >= 4))) && !defined(USE_NON_CONST)
  const Standard_Character * adup = Tcl_GetVar(thePCommands->Interp(),
					 "QA_DUP",TCL_GLOBAL_ONLY);
#else
  Standard_Character * adup = Tcl_GetVar(thePCommands->Interp(),
					 "QA_DUP",TCL_GLOBAL_ONLY);
#endif
  Standard_Integer aDUP=1;
  if((adup != NULL) && (Draw::Atof(adup) == 0)) {
    aDUP = 0;
  }

  return aDUP;
}

static void before() {
  should_be_printed = Standard_True;
  if(Draw_BeforeCommand_old) (*Draw_BeforeCommand_old) ();
}

static void  after(Standard_Integer a)
{
  if(Draw_AfterCommand_old) (*Draw_AfterCommand_old) (a);
  if(!should_be_printed) {
//    Tcl_ResetResult(theCommands.Interp());
  }
}

static QADraw_DataMapOfAsciiStringOfAddress stFuncMap;

  // MKV 30.03.05
#if ((TCL_MAJOR_VERSION > 8) || ((TCL_MAJOR_VERSION == 8) && (TCL_MINOR_VERSION >= 4))) && !defined(USE_NON_CONST)
static Standard_Integer (*CommandCmd_Old)
(ClientData clientData, Tcl_Interp *interp,
 Standard_Integer argc, const char* argv[]) = NULL;
#else
static Standard_Integer (*CommandCmd_Old)
(ClientData clientData, Tcl_Interp *interp,
 Standard_Integer argc, char* argv[]) = NULL;
#endif

  // MKV 30.03.05
#if ((TCL_MAJOR_VERSION > 8) || ((TCL_MAJOR_VERSION == 8) && (TCL_MINOR_VERSION >= 4))) && !defined(USE_NON_CONST)
static Standard_Integer CommandCmd
(ClientData clientData, Tcl_Interp *interp,
 Standard_Integer argc, const char* argv[])
#else
static Standard_Integer CommandCmd
(ClientData clientData, Tcl_Interp *interp,
 Standard_Integer argc, char* argv[])
#endif
{
  Standard_Integer old_out = 0;
  Standard_Integer old_err = 0;
  Standard_Integer fd_out = 0;
  Standard_Integer fd_err = 0;

  FILE * aFile_out = NULL;
  FILE * aFile_err = NULL;

  char *nameo = NULL;
  char *namee = NULL;
#ifdef WNT
  if(strlen(getenv("TEMP")) == 0) {
    cerr << "The TEMP variable is not set." << endl;
    aFile_out = tmpfile();
    aFile_err = tmpfile();
  } else {
    nameo = _tempnam(getenv("TEMP"),"tmpo");
    namee = _tempnam(getenv("TEMP"),"tmpe");
    aFile_out=fopen(nameo, "w+");
    aFile_err=fopen(namee, "w+");
  }
#else
    aFile_out = tmpfile();
    aFile_err = tmpfile();
#endif
  fd_out = fileno(aFile_out);
  fd_err = fileno(aFile_err);
  if(fd_out !=-1 && fd_err !=-1) {
    old_err = dup(STDERR_FILENO);
    old_out = dup(STDOUT_FILENO);
    dup2(fd_err,STDERR_FILENO);
    dup2(fd_out,STDOUT_FILENO);
  } else
    cout << "Faulty : Can not create temporary file."   << endl;

  Standard_Integer clen = sizeof(Standard_Character);

  Standard_Character * QA = getenv("QA_print_command");
  if((QA != NULL) && (!strcmp(QA,"1"))) {
    for(Standard_Integer i=0;i<argc;i++) {
      write(st_err,argv[i],clen*strlen(argv[i]));
      write(st_err," ",clen);
    }
    write(st_err,"\n",clen);
  }

  // MKV 30.03.05
#if ((TCL_MAJOR_VERSION > 8) || ((TCL_MAJOR_VERSION == 8) && (TCL_MINOR_VERSION >= 4))) && !defined(USE_NON_CONST)
  TCollection_AsciiString aName((char *) argv[0]);
#else
  TCollection_AsciiString aName(argv[0]);
#endif

  Standard_Integer res = 0;

  if(stFuncMap.IsBound(aName)) {
    // MKV 30.03.05
#if ((TCL_MAJOR_VERSION > 8) || ((TCL_MAJOR_VERSION == 8) && (TCL_MINOR_VERSION >= 4))) && !defined(USE_NON_CONST)
    CommandCmd_Old  = (int(*)(void*, Tcl_Interp*, int, const char**))  stFuncMap((char *) argv[0]);
#else
    CommandCmd_Old  = (int(*)(void*, Tcl_Interp*, int, char**))  stFuncMap(argv[0]);
#endif
    res = (*CommandCmd_Old) (clientData,interp,argc,argv);
  }

  fflush(stderr);
  fflush(stdout);
  close(STDERR_FILENO);
  close(STDOUT_FILENO);
  dup2(old_err,STDERR_FILENO);
  dup2(old_out,STDOUT_FILENO);
  close(old_err);
  close(old_out);

  Standard_Character buf[256];
  Standard_Integer len = 0;

  rewind(aFile_err);
  while((len = read(fd_err,buf,clen*255)/clen) > 0) {
    buf[len]='\0';
    if(shouldDUP()) {
      (*thePCommands) << buf;
    } else {
      write(st_err,buf,clen*len);
    }
  }
  close(fd_err);

  rewind(aFile_out);
  buf[0] = '\0';
  len = 0;
  while((len = read(fd_out,buf,clen*255)/clen) > 0) {
    buf[len]='\0';
    if(shouldDUP()) {
      (*thePCommands) << buf;
    } else {
      write(st_err,buf,clen*len);
    }
  }
  close(fd_out);

  if(shouldDUP()) {
    Standard_Character *Result = (Standard_Character *)thePCommands->Result();
    if(Result[0] != '\0') {
      Standard_Integer pos = 0;
      Standard_Integer rlen = strlen(Result);
      while(pos < rlen) {
	Standard_Integer nb = 256;
	if((rlen - pos) <= 256) {
	  nb = rlen - pos;
	}
	write(st_err,Result+pos,nb*clen);
	pos += nb;
      }
      write(st_err,"\n",clen);
      should_be_printed = Standard_False  ;
    }
  } //else {
  if(nameo)
    free(nameo);
  if(namee)
    free(namee);
  return res;
}

static Standard_Integer QARebuild (Draw_Interpretor& di, Standard_Integer /*n*/, const char ** a)
{

  Tcl_CmdInfo *infoPtr = new Tcl_CmdInfo;

  if(!strcmp(a[0],a[1]))
    return 0;

  char* var_a = (char*)a[1];
  Standard_Integer res = Tcl_GetCommandInfo(di.Interp(),var_a,infoPtr);
  if(res && (infoPtr->proc != &CommandCmd)) {
    TCollection_AsciiString aName(var_a);
    if (!stFuncMap.IsBound(aName)) {
      stFuncMap.Bind(aName, (Standard_Address) infoPtr->proc);

      infoPtr->proc = &CommandCmd;
#if TCL_MAJOR_VERSION == 8 && TCL_MINOR_VERSION == 3
      infoPtr->isNativeObjectProc = 0;
      infoPtr->objProc = NULL;
#endif
      Tcl_SetCommandInfo(di.Interp(),var_a,infoPtr);
    }
  }

  return 0;
}

Handle(TColStd_HSequenceOfReal) GetColorOfPixel (const Image_PixMap&    theImage,
                                                 const Standard_Integer theCoordinateX,
                                                 const Standard_Integer theCoordinateY,
                                                 const Standard_Integer theRadius)
{
  Handle(TColStd_HSequenceOfReal) aSeq = new TColStd_HSequenceOfReal();
  if (theImage.IsEmpty()) {
    std::cerr << "The image is null.\n";
    return aSeq;
  }
  Standard_Integer aWidth   = (Standard_Integer )theImage.SizeX();
  Standard_Integer anHeight = (Standard_Integer )theImage.SizeY();

  Quantity_Color aColorTmp;
  for (Standard_Integer anXIter = theCoordinateX - theRadius;
       anXIter <= theCoordinateX + theRadius; ++anXIter)
  {
    if (anXIter < 0 || anXIter >= aWidth)
    {
      continue;
    }
    for (Standard_Integer anYIter = theCoordinateY - theRadius;
         anYIter <= theCoordinateY + theRadius; ++anYIter)
    {
      if (anYIter < 0 || anYIter >= anHeight)
      {
        continue;
      }
      // Image_PixMap stores image upside-down in memory!
      aColorTmp = theImage.PixelColor (anXIter, anYIter);
      aSeq->Append (aColorTmp.Red());
      aSeq->Append (aColorTmp.Green());
      aSeq->Append (aColorTmp.Blue());
    }
  }
  return aSeq;
}

static Standard_Integer QAAISGetPixelColor (Draw_Interpretor& theDi,
                                            Standard_Integer  theArgsNb,
                                            const char**      theArgs)
{
  if (theArgsNb != 3 && theArgsNb != 6)
  {
    theDi << "Usage : " << theArgs[0] << " coordinate_X coordinate_Y [color_R color_G color_B]" << "\n";
    return 1; // TCL_ERROR
  }

  Handle(V3d_View) aView3d = ViewerTest::CurrentView();
  if (aView3d.IsNull())
  {
    theDi << "You must initialize AISViewer before this command.\n";
    return 1; // TCL_ERROR
  }

  const Handle(Aspect_Window) anAISWindow = aView3d->Window();
  Standard_Integer aWindowSizeX = 0;
  Standard_Integer aWindowSizeY = 0;
  anAISWindow->Size (aWindowSizeX, aWindowSizeY);

  Standard_Integer anArgIter = 1;
  const Standard_Integer aPickCoordX = Draw::Atoi (theArgs[anArgIter++]);
  const Standard_Integer aPickCoordY = Draw::Atoi (theArgs[anArgIter++]);
  const Standard_Integer aRadius = (theArgsNb == 3) ? 0 : 1;

  Image_ColorRGBF aColorInput = {{ 0.0f, 0.0f, 0.0f }};
  if (theArgsNb == 6)
  {
    aColorInput.r() = (Standard_ShortReal )Draw::Atof (theArgs[anArgIter++]);
    aColorInput.g() = (Standard_ShortReal )Draw::Atof (theArgs[anArgIter++]);
    aColorInput.b() = (Standard_ShortReal )Draw::Atof (theArgs[anArgIter++]);
  }

  Image_PixMap anImage;
  aView3d->ToPixMap (anImage, aWindowSizeX, aWindowSizeY);
  const Handle(TColStd_HSequenceOfReal) aSeq = GetColorOfPixel (anImage, aPickCoordX, aPickCoordY, aRadius);
  cout << "Length = " << aSeq->Length() << endl;

  Image_ColorRGBF aColorPicked = {{ 0.0f, 0.0f, 0.0f }};
  Standard_Boolean isNotEqual = Standard_True;
  for (Standard_Integer i = 1; i <= aSeq->Length(); i += 3)
  {
    aColorPicked.r() = (Standard_ShortReal )aSeq->Value (i + 0);
    aColorPicked.g() = (Standard_ShortReal )aSeq->Value (i + 1);
    aColorPicked.b() = (Standard_ShortReal )aSeq->Value (i + 2);

    if (theArgsNb == 3 ||
        ((Abs (aColorPicked.r() - aColorInput.r()) <= Precision::Confusion())
      && (Abs (aColorPicked.g() - aColorInput.g()) <= Precision::Confusion())
      && (Abs (aColorPicked.b() - aColorInput.b()) <= Precision::Confusion())))
    {
      isNotEqual = Standard_False;
      break;
    }
  }

  theDi << "RED :   " << aColorPicked.r() << " "
        << "GREEN : " << aColorPicked.g() << " "
        << "BLUE :  " << aColorPicked.b() << "\n";

  if (theArgsNb == 6)
  {
    theDi << "User color: \n"
          << "RED :   " << aColorInput.r() << " "
          << "GREEN : " << aColorInput.g() << " "
          << "BLUE :  " << aColorInput.b() << "\n";
  }

  if (isNotEqual)
  {
    theDi << "Faulty : colors are not equal.\n";
    return 1; // TCL_ERROR
  }
  return 0;
}

#if ! defined(WNT)
extern int ViewerMainLoop (Standard_Integer argc, const char ** argv);
#else
Standard_EXPORT int ViewerMainLoop (Standard_Integer argc, const char ** argv);
#endif

static Standard_Integer QAAISGetMousePoint (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if ( argc != 1 ) {
    di << "Usage : " << argv[0] << "\n";
    return 1;
  }
  Handle (V3d_View) QAAISView = ViewerTest::CurrentView ();
  if ( QAAISView.IsNull () ) {
    di << "You must initialize AISViewer before this command." << "\n";
    return 1;
  }
  Standard_Integer QAAISMouseCoordinateX = 0;
  Standard_Integer QAAISMouseCoordinateY = 0;
  Standard_Integer argccc = 5;
  const char *bufff[] = { "A", "B", "C", "D", "E" };
  const char **argvvv = (const char **) bufff;
  while ( ViewerMainLoop (argccc, argvvv) ) {
    ViewerTest::GetMousePosition (QAAISMouseCoordinateX, QAAISMouseCoordinateY);
  }
  ViewerTest::GetMousePosition (QAAISMouseCoordinateX, QAAISMouseCoordinateY);
  di << "X-coordinate: " << QAAISMouseCoordinateX << "; Y-coordinate: " << QAAISMouseCoordinateY << "\n";
  return 0;
}

static Standard_Integer QAAISGetColorCoord (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
#if ! defined(WNT)
  if ( argc > 1 ) {
    di << "Usage : " << argv[0] << "\n";
    return 1;
  }
  Handle(Xw_Window) QAAISWindow;

  Handle (V3d_View) QAAIS_MainView = ViewerTest::CurrentView ();
  if ( QAAIS_MainView.IsNull () ) {
    di << "You must initialize AISViewer before this command." << "\n";
    return 1;
  }
  QAAISWindow = Handle(Xw_Window)::DownCast (QAAIS_MainView->V3d_View::Window());

  Standard_Integer QAAIS_WindowSize_X = 0;
  Standard_Integer QAAIS_WindowSize_Y = 0;
  QAAISWindow->Size(QAAIS_WindowSize_X, QAAIS_WindowSize_Y);
  Handle(Aspect_DisplayConnection) aDisplayConnection = new Aspect_DisplayConnection();
  Handle(Graphic3d_GraphicDriver)  aGraphicDriver = Graphic3d::InitGraphicDriver (aDisplayConnection);

  Draw_Window QAAIS_CoordWindow ("coordinate", 421, 205, 200, 60);
  QAAIS_CoordWindow.DisplayWindow ();
  QAAIS_CoordWindow.SetColor (12);

  Handle (Xw_Window) QAAIS_ColorWindow = new Xw_Window (aDisplayConnection, "color", 0.4074, 0.678, 0.1962, 0.06, Quantity_NOC_BLACK);
  Handle (V3d_Viewer) QAAIS_ColorViewer = new V3d_Viewer (aGraphicDriver, Standard_ExtString ("COLOR"));
  Handle (V3d_View) QAAIS_ColorView = QAAIS_ColorViewer -> CreateView ();
  QAAIS_ColorWindow -> Map ();
  QAAIS_ColorView -> SetWindow (QAAIS_ColorWindow);
  QAAIS_ColorView -> SetBackgroundColor (Quantity_NOC_BLACK);
  QAAIS_ColorView -> Redraw ();

  Standard_Integer QAAIS_MousePoint_X = 0;
  Standard_Integer QAAIS_MousePoint_Y = 0;
  Standard_Character QAAIS_MousePointX[32];
  Sprintf (QAAIS_MousePointX, "X : %d", QAAIS_MousePoint_X);
  Standard_Character QAAIS_MousePointY[32];
  Sprintf (QAAIS_MousePointY, "Y : %d", QAAIS_MousePoint_Y);
  Standard_ShortReal QAAIS_ColorRED = 0;
  Standard_ShortReal QAAIS_ColorGRN = 0;
  Standard_ShortReal QAAIS_ColorBLU = 0;
  Quantity_Color QAAIS_ShowingColor;
  QAAIS_ShowingColor.SetValues (QAAIS_ColorRED, QAAIS_ColorGRN, QAAIS_ColorBLU, Quantity_TOC_RGB);
  Standard_Integer argccc = 5;
  const char *bufff[] = { "A", "B", "C", "D", "E" };
  const char **argvvv = (const char **) bufff;
  while (ViewerMainLoop (argccc, argvvv)) {
    Handle(TColStd_HSequenceOfReal) aSeq;
    Image_PixMap anImage;

    ViewerTest::GetMousePosition (QAAIS_MousePoint_X, QAAIS_MousePoint_Y);
    Handle (V3d_View) QAAIS_MainView = ViewerTest::CurrentView();
    QAAIS_MainView->ToPixMap (anImage, QAAIS_WindowSize_X, QAAIS_WindowSize_Y);

    aSeq = GetColorOfPixel (anImage, QAAIS_MousePoint_X, QAAIS_MousePoint_Y, 0);

    QAAIS_ColorRED = aSeq->Value(1);
    QAAIS_ColorGRN = aSeq->Value(2);
    QAAIS_ColorBLU = aSeq->Value(3);
    QAAIS_ShowingColor.SetValues (QAAIS_ColorRED, QAAIS_ColorGRN, QAAIS_ColorBLU, Quantity_TOC_RGB);
    QAAIS_ColorView -> SetBackgroundColor (QAAIS_ShowingColor);
    QAAIS_ColorView -> Redraw ();
    QAAIS_CoordWindow.Clear();
    Sprintf (QAAIS_MousePointX, "X : %d", QAAIS_MousePoint_X);
    Sprintf (QAAIS_MousePointY, "Y : %d", QAAIS_MousePoint_Y);
    QAAIS_CoordWindow.DrawString (30, 35, QAAIS_MousePointX);
    QAAIS_CoordWindow.DrawString (125, 35, QAAIS_MousePointY);
  }
  QAAIS_CoordWindow.Destroy ();
#endif //WNT
    return 0;
}

//==============================================================================
//  VIEWER GLOBALs
//==============================================================================
#ifndef WNT
extern Draw_Viewer dout;
extern Display*         Draw_WindowDisplay;
extern Colormap         Draw_WindowColorMap;
#else
Standard_IMPORT Draw_Viewer dout;
#endif

//=======================================================================
#if ! defined(WNT)
extern ViewerTest_DoubleMapOfInteractiveAndName& GetMapOfAIS();
extern Handle(AIS_InteractiveContext)& TheAISContext();
#else
Standard_EXPORT ViewerTest_DoubleMapOfInteractiveAndName& GetMapOfAIS();
Standard_EXPORT Handle(AIS_InteractiveContext)& TheAISContext();
#endif
#include <ViewerTest_DoubleMapOfInteractiveAndName.hxx>
#include <AIS_Trihedron.hxx>
#include <AIS_Axis.hxx>
#include <Geom_Line.hxx>
#include <AIS_Line.hxx>

//==============================================================================
// function : VTrihedronOrigins
// author   : ota
// purpose  : draws triheron axis origin lines.
// Draw arg : vtri_orig trihedron_name
//==============================================================================
static int VTrihedronOrigins(Draw_Interpretor& di,
			      Standard_Integer argc,
			      const char ** argv)
{
  if(argc != 2){
    di <<"Usage : vtri_orig tri_name"<<"\n";
    return 1;
  }

  if(TheAISContext().IsNull()){
    di<<"Make 'vinit' before this method call"<<"\n";
    return 1;
  }

  //get trihedron from AIS map.
  TCollection_AsciiString aName(argv[1]);
  if(!GetMapOfAIS().IsBound2(aName)){
    di<<"No object named '"<<argv[1]<<"'"<<"\n";
    return 1;
  }

  Handle(AIS_Trihedron) aTrih =
    Handle(AIS_Trihedron)::DownCast(GetMapOfAIS().Find2(aName));
  if(aTrih.IsNull()){
    di<<"Trihedron is not found, try another name"<<"\n";
    return 1;
  }

  //get axis
  Handle(AIS_Axis) XAxis = aTrih->XAxis();
  Handle(AIS_Axis) YAxis = aTrih->YAxis();
  Handle(AIS_Axis) ZAxis = aTrih->Axis();

  //get geometrical lines
  Handle(Geom_Line) XGLine = XAxis->Component();
  Handle(Geom_Line) YGLine = YAxis->Component();
  Handle(Geom_Line) ZGLine = ZAxis->Component();

  //make AIS_Lines
  Handle(AIS_Line) XLine = new AIS_Line(XGLine);
  Handle(AIS_Line) YLine = new AIS_Line(YGLine);
  Handle(AIS_Line) ZLine = new AIS_Line(ZGLine);

  //put them into AIS map:
  GetMapOfAIS().Bind(XLine,aName+"_X");
  GetMapOfAIS().Bind(YLine,aName+"_Y");
  GetMapOfAIS().Bind(ZLine,aName+"_Z");
  //print names of created objects:
  di<<argv[1]<<"_X  "<<argv[1]<<"_Y  "<<argv[1]<<"_Z"<<"\n";

  //try to draw them:
  TheAISContext()->Display(XLine);
  TheAISContext()->Display(YLine);
  TheAISContext()->Display(ZLine);

  return 0;
}

#include <Draw_Viewer.hxx>
#include <Draw.hxx>

static Standard_Integer ViewId(const Standard_CString a)
{
  Standard_Integer id = Draw::Atoi(a);
  if ((id < 0) || (id >= MAXVIEW)) {
    cout << "Incorrect view-id, must be in 0.."<<MAXVIEW-1<<endl;
    return -1;
  }
  if (!dout.HasView(id)) {
    cout <<"View "<<id<<" does not exist."<<endl;
    return -1;
  }
  return id;
}

#include <Draw_Display.hxx>

//=======================================================================
// QArename
//=======================================================================

static Standard_Integer QArename(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n < 3) return 1;
  Standard_Boolean cop = !strcasecmp(a[0],"copy");

  Handle(Draw_Drawable3D) D;
  for (Standard_Integer i = 1; i < n; i += 2) {
    if (i+1 >= n) return 0;
    D = Draw::Get(a[i]);
    if (!D.IsNull()) {
      if (cop)
	D = D->Copy();
      else
	// clear old name
	Draw::Set(a[i],Handle(Draw_Drawable3D()));

      Draw::Set(a[i+1],D);
    }
  }
  return 0;
}

//#if defined(V2D)
//#include <AIS2D_InteractiveContext.hxx>
//static Standard_Integer QANbSelected2d (Draw_Interpretor& /*di*/, Standard_Integer argc, char** argv)
//{
//  if(argc != 1) {
//    cout << "Usage : " << argv[0] << endl;
//    return 1;
//  }
//  Handle(AIS2D_InteractiveContext) aContext = Viewer2dTest::GetAIS2DContext();
//  if(aContext.IsNull()) {
//    cerr << "use 'v2dinit' command before " << argv[0] << endl;
//    return 1;
//  }
//  cout << aContext->NbSelected() << endl;
//  return 0;
//}
//#endif

void QADraw::CommonCommands(Draw_Interpretor& theCommands)
{
  ios::sync_with_stdio();

  st_err = dup(STDERR_FILENO);

  Draw_BeforeCommand_old = Draw_BeforeCommand;
  Draw_AfterCommand_old  = Draw_AfterCommand;

  Draw_BeforeCommand = &before;
  Draw_AfterCommand  = &after;

  const char* group = "QA_Commands";

  theCommands.Add("QARebuild","QARebuild command_name",__FILE__,QARebuild,group);
  theCommands.Add("QAGetPixelColor", "QAGetPixelColor coordinate_X coordinate_Y [color_R color_G color_B]", __FILE__,QAAISGetPixelColor, group);
  theCommands.Add("QAGetMousePoint", "QAGetMousePoint", __FILE__,QAAISGetMousePoint, group);
  theCommands.Add("QAGetColorCoord", "QAGetColorCoord", __FILE__,QAAISGetColorCoord, group);
  theCommands.Add("vtri_orig",
		  "vtri_orig         : vtri_orig trihedron_name  -  draws axis origin lines",
		  __FILE__,VTrihedronOrigins,group);

// adding commands "rename" leads to the fact that QA commands doesn't work properly OCC23410, use function "renamevar"
// theCommands.Add("rename","rename name1 toname1 name2 toname2 ...",__FILE__,QArename,group);
}
/*
extern "C" int Tkqadraw_Init(Tcl_Interp *);

int Tkqadraw_Init(Tcl_Interp * ) {

  ios::sync_with_stdio();

  QADraw::CommonCommands(theCommands);
  QADraw::AdditionalCommands(theCommands);

  ViewerTest::Commands (theCommands);
  ViewerTest::ViewerCommands (theCommands);

  Viewer2dTest::Commands(theCommands);
//   if (Tcl_PkgProvide(theCommands.Interp(), "Qa", "1.0") != TCL_OK) {
//     return TCL_ERROR;
//   }

  return TCL_OK;
}
*/
//==============================================================================
// QADraw::Factory
//==============================================================================
void QADraw::Factory(Draw_Interpretor& theCommands)
{
  thePCommands = &theCommands;

  // definition of QA Command
  QADraw::CommonCommands(theCommands);
  QADraw::AdditionalCommands(theCommands);

  //ViewerTest::Commands (theCommands);
  //ViewerTest::ViewerCommands (theCommands);

//#if defined(V2D)
//    Viewer2dTest::Commands(theCommands);
//#endif

//#ifdef DEB
//      cout << "Draw Plugin : QA commands are loaded" << endl;
//#endif
}

// Declare entry point PLUGINFACTORY
DPLUGIN(QADraw)
