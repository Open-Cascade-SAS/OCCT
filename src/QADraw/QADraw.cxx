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
#include <Image_PixMap.hxx>
#include <V3d_View.hxx>
#include <ViewerTest.hxx>
#include <ViewerTest_EventManager.hxx>
#include <TColStd_StackOfInteger.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_HSequenceOfReal.hxx>
#include <AIS_InteractiveContext.hxx>
#include <Graphic3d_GraphicDevice.hxx>
#include <Draw_Window.hxx>
#include <Draw_Viewer.hxx>
#include <Aspect_WindowDriver.hxx>
#include <stdio.h>

#include <Viewer2dTest.hxx>
#include <V2d_View.hxx>
#include <Viewer2dTest_EventManager.hxx>

#if ! defined(WNT)
#include <Xw_Window.hxx>
//#include <Xm/Xm.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <Xw_GraphicDevice.hxx>
#include <Xw_Cextern.hxx>
#include <unistd.h>
#else
#include <WNT.h>
#include <WNT_GraphicDevice.hxx>
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
  if((adup != NULL) && (atof(adup) == 0)) {
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

static Standard_Integer QAUpdateLights(Draw_Interpretor& di, Standard_Integer argc, const char ** argv )
{
  if(argc != 1) {
    di << "Usage : " << argv[0] << "\n";
    return -1;
  }

  Handle(AIS_InteractiveContext) myAIScontext = ViewerTest::GetAISContext();
  if(myAIScontext.IsNull()) {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return -1;
  }
  ViewerTest::CurrentView()->UpdateLights();
  ViewerTest::CurrentView()->Update();
  return 0;
}

static Standard_Integer QAxwd_3d(Draw_Interpretor& di, Standard_Integer argc, const char** argv )
{
  if(argc != 2) {
    di << "Usage : " << argv[0] << " filename" << "\n";
    return -1;
  }

  Handle(AIS_InteractiveContext) myAIScontext = ViewerTest::GetAISContext();
  if(myAIScontext.IsNull()) {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return -1;
  }
  Handle(V3d_View) myV3dView = ViewerTest::CurrentView();
  //cout << myV3dView->Dump(argv[1]) << endl;
  myV3dView->Dump(argv[1]);
  return 0;
}


static Standard_Integer QAMoveTo(Draw_Interpretor& di, Standard_Integer argc, const char ** argv )
{
  if(argc != 3) {
    di << "Usage : " << argv[0] << " x y" << "\n";
    return -1;
  }

  Handle(AIS_InteractiveContext) myAIScontext = ViewerTest::GetAISContext();
  if(myAIScontext.IsNull()) {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return -1;
  }
  ViewerTest::CurrentEventManager()->MoveTo(atoi(argv[1]),atoi(argv[2]));
  return 0;
}

static Standard_Integer QASelect(Draw_Interpretor& di, Standard_Integer argc, const char ** argv )
{
  if(argc != 3) {
    di << "Usage : " << argv[0] << " x y" << "\n";
    return -1;
  }

  Handle(AIS_InteractiveContext) myAIScontext = ViewerTest::GetAISContext();
  if(myAIScontext.IsNull()) {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return -1;
  }
  ViewerTest::CurrentEventManager()->MoveTo(atoi(argv[1]),atoi(argv[2]));
  ViewerTest::CurrentEventManager()->Select();
  return 0;
}

static Standard_Integer QAShiftSelect(Draw_Interpretor& di, Standard_Integer argc, const char ** argv )
{
  if(argc != 3) {
    di << "Usage : " << argv[0] << " x y" << "\n";
    return -1;
  }

  Handle(AIS_InteractiveContext) myAIScontext = ViewerTest::GetAISContext();
  if(myAIScontext.IsNull()) {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return -1;
  }
  ViewerTest::CurrentEventManager()->MoveTo(atoi(argv[1]),atoi(argv[2]));
  ViewerTest::CurrentEventManager()->ShiftSelect();
  return 0;
}

static Standard_Integer QASetAntialiasing(Draw_Interpretor& di, Standard_Integer argc, const char ** argv )
{
  if(argc > 2) {
    di << "Usage : " << argv[0] << " [1/0]" << "\n";
    return -1;
  }

  Handle(AIS_InteractiveContext) myAIScontext = ViewerTest::GetAISContext();
  if(myAIScontext.IsNull()) {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return -1;
  }

  Handle(V3d_View) myV3dView = ViewerTest::CurrentView();

  if((argc == 2) && (atof(argv[1]) == 0))
    myV3dView->SetAntialiasingOff();
  else
    myV3dView->SetAntialiasingOn();
  myV3dView->Update();
  return 0;
}

static Standard_Integer QAvzfit(Draw_Interpretor& di, Standard_Integer /*argc*/, const char ** argv )
{
  Handle(V3d_View) V = ViewerTest::CurrentView();
  if ( V.IsNull() ) {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return -1;
  }
  V->ZFitAll();
  return 0;
}

Handle(TColStd_HSequenceOfReal) GetColorOfPixel (const Handle(Aspect_PixMap) theImage,
                                                 const Standard_Integer theCoordinateX,
                                                 const Standard_Integer theCoordinateY,
                                                 const Standard_Integer theRadius)
{
  Handle(TColStd_HSequenceOfReal) aSeq = new TColStd_HSequenceOfReal();
  if (theImage.IsNull()) {
    std::cerr << "The image is null.\n";
    return aSeq;
  }
  Standard_Integer aWidth = 0;
  Standard_Integer anHeight = 0;
  theImage->Size (aWidth, anHeight);

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
      aColorTmp = theImage->PixelColor (anXIter, anYIter);
      aSeq->Append (aColorTmp.Red());
      aSeq->Append (aColorTmp.Green());
      aSeq->Append (aColorTmp.Blue());
    }
  }
  return aSeq;
}

static Standard_Integer QAAISGetPixelColor (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if ( argc != 3 ) {
    if ( argc != 6 ) {
      di << "Usage : " << argv[0] << " coordinate_X coordinate_Y [color_R color_G color_B]" << "\n";
      return 1;
    }
  }
  Handle (V3d_View) QAAISView = ViewerTest::CurrentView ();
  if ( QAAISView.IsNull () ) {
    di << "You must initialize AISViewer before this command." << "\n";
    return 1;
  }
  Handle (Aspect_Window) QAAISWindow = QAAISView->Window ();
  Standard_Integer QAAISXWindowSize_X = 0;
  Standard_Integer QAAISXWindowSize_Y = 0;
  QAAISWindow->Size(QAAISXWindowSize_X, QAAISXWindowSize_Y);
  Standard_ShortReal QAAISCoordinateX = atoi (argv [1]);
  Standard_ShortReal QAAISCoordinateY = atoi (argv [2]);

  Standard_ShortReal QAAISColorRED_V = 0;
  Standard_ShortReal QAAISColorGRN_V = 0;
  Standard_ShortReal QAAISColorBLU_V = 0;

  if ( argc == 6 ) {
    QAAISColorRED_V = atof (argv [3]);
    QAAISColorGRN_V = atof (argv [4]);
    QAAISColorBLU_V = atof (argv [5]);

    di << "Begin aColorRED_User = " << QAAISColorRED_V << "\n";
    di << "Begin aColorGRN_User = " << QAAISColorRED_V << "\n";
    di << "Begin aColorBLU_User = " << QAAISColorRED_V << "\n";
  }

  Standard_Integer aRadius = 1;
  if ( argc == 3 ) {
    aRadius=0;
  }

  Handle(TColStd_HSequenceOfReal) aSeq = GetColorOfPixel (QAAISView->ToPixMap (QAAISXWindowSize_X, QAAISXWindowSize_Y, Image_TOI_RGB),
                                                          QAAISCoordinateX, QAAISCoordinateY,
                                                          aRadius);
  cout << "Length = " << aSeq->Length() << endl;

  Standard_Boolean IsNotEqual = Standard_True;
  Standard_Integer i;
  for(i=1; i<=aSeq->Length();i+=3) {
    // mkv 29.04.03
    Standard_ShortReal QAAISColorRED_R = (((Standard_ShortReal) ((Standard_Integer) (aSeq->Value(i+0) * 1000000))) / 1000000.);
    Standard_ShortReal QAAISColorGRN_R = (((Standard_ShortReal) ((Standard_Integer) (aSeq->Value(i+1) * 1000000))) / 1000000.);
    Standard_ShortReal QAAISColorBLU_R = (((Standard_ShortReal) ((Standard_Integer) (aSeq->Value(i+2) * 1000000))) / 1000000.);
    // mkv 29.04.03

    if ( argc == 3 ) {
      di << "RED : "    << QAAISColorRED_R
	<< " GREEN : " << QAAISColorGRN_R
	  << " BLUE : "  << QAAISColorBLU_R
	    << "\n";
      IsNotEqual = Standard_False;
      break;
    }

    if (   QAAISColorRED_R == QAAISColorRED_V
	&& QAAISColorGRN_R == QAAISColorGRN_V
	&& QAAISColorBLU_R == QAAISColorBLU_V
	) {
      IsNotEqual = Standard_False;
      break;
    }
  }
  if (IsNotEqual) {
    di << "Faulty : colors are not equal." << "\n";
    return 1;
  }
  return 0;
}

static Standard_Boolean IsSelectionModeCurrentlyON (Standard_Integer theMode) {
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if(!aContext->HasOpenedContext()) {
    return Standard_False ;
  }
  const TColStd_ListOfInteger& List = aContext->ActivatedStandardModes();
  TColStd_ListIteratorOfListOfInteger it;
  Standard_Boolean Found=Standard_False;
  for (it.Initialize(List); it.More()&&!Found; it.Next() ) {
    if (it.Value()==theMode ) Found=Standard_True;
  }
  return Found;
}

static Standard_Integer QAAISSetChoiceMode (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if ( argc != 3 ) {
    di << "Usage : " << argv[0] << " mode switch" << "\n";
    di << "      mode:                         " << "\n";
    // cout << "              SHAPE                 " << endl;
    di << "              VERTEX                " << "\n";
    di << "              EDGE                  " << "\n";
    di << "              WIRE                  " << "\n";
    di << "              FACE                  " << "\n";
    di << "              SHELL                 " << "\n";
    di << "              SOLID                 " << "\n";
    di << "              COMPOUND              " << "\n";
    di << "      switch:                       " << "\n";
    di << "              ON                    " << "\n";
    di << "              OFF                   " << "\n";
    return 1;
  }
  Handle (V3d_View) QAAISView = ViewerTest::CurrentView ();
  if ( QAAISView.IsNull () ) {
    di << "You must initialize AISViewer before this command." << "\n";
    return 1;
  }
  Standard_Integer QAChoosingMode = -1;
  // if ( strcmp (sc [1], "SHAPE") == 0 ) {
  //   QAChoosingMode = 0;
  // }
  if ( strcmp (argv [1], "VERTEX") == 0 ) {
    QAChoosingMode = 1;
  }
  if ( strcmp (argv [1], "EDGE") == 0 ) {
    QAChoosingMode = 2;
  }
  if ( strcmp (argv [1], "WIRE") == 0 ) {
    QAChoosingMode = 3;
  }
  if ( strcmp (argv [1], "FACE") == 0 ) {
    QAChoosingMode = 4;
  }
  if ( strcmp (argv [1], "SHELL") == 0 ) {
    QAChoosingMode = 5;
  }
  if ( strcmp (argv [1], "SOLID") == 0 ) {
    QAChoosingMode = 6;
  }
  if ( strcmp (argv [1], "COMPOUND") == 0 ) {
    QAChoosingMode = 7;
  }
  if ( QAChoosingMode == -1 ) {
    di << "Use - QAAISSetChoiceMode mode switch" << "\n";
    return 1;
  }

  if ( strcmp (argv [2], "ON") == 0 ) {
    if ( IsSelectionModeCurrentlyON (QAChoosingMode)) {
      di << "Mode already ON." << "\n";
      return 1;
    }
    ViewerTest::StandardModeActivation (QAChoosingMode);
    return 0;
  }
  if ( strcmp (argv [2], "OFF") == 0 ) {
    if (!IsSelectionModeCurrentlyON (QAChoosingMode)) {
      di << "Mode already OFF." << "\n";
      return 1;
    }
    ViewerTest::StandardModeActivation (QAChoosingMode);
    return 0;
  }
  di << "Usage : " << argv[0] << " mode switch" << "\n";
  return 1;
}

#if ! defined(WNT)
extern int ViewerMainLoop (Standard_Integer argc, const char ** argv);
#else
Standard_EXPORT int ViewerMainLoop (Standard_Integer argc, const char ** argv);
#endif
#if ! defined(WNT)
extern int ViewerMainLoop2d (Standard_Integer argc, const char ** argv);
#else
Standard_EXPORT int ViewerMainLoop2d (Standard_Integer argc, const char ** argv);
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

static Standard_Integer QAAISGetViewCharac (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
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
  Quantity_Factor QAAISViewScale = QAAISView -> V3d_View::Scale ();
  Standard_Real QAAISViewCenterCoordinateX = 0.0;
  Standard_Real QAAISViewCenterCoordinateY = 0.0;
  QAAISView -> V3d_View::Center (QAAISViewCenterCoordinateX, QAAISViewCenterCoordinateY);
  Standard_Real QAAISViewProjX = 0.0;
  Standard_Real QAAISViewProjY = 0.0;
  Standard_Real QAAISViewProjZ = 0.0;
  QAAISView -> V3d_View::Proj (QAAISViewProjX, QAAISViewProjY, QAAISViewProjZ);
  Standard_Real QAAISViewUpX = 0.0;
  Standard_Real QAAISViewUpY = 0.0;
  Standard_Real QAAISViewUpZ = 0.0;
  QAAISView -> V3d_View::Up (QAAISViewUpX, QAAISViewUpY, QAAISViewUpZ);
  Standard_Real QAAISViewAtX = 0.0;
  Standard_Real QAAISViewAtY = 0.0;
  Standard_Real QAAISViewAtZ = 0.0;
  QAAISView -> V3d_View::At (QAAISViewAtX, QAAISViewAtY, QAAISViewAtZ);
  di << "Scale of current view: " << QAAISViewScale << "\n";
  di << "Center on X : "<< QAAISViewCenterCoordinateX << "; on Y: " << QAAISViewCenterCoordinateY << "\n";
  di << "Proj on X : " << QAAISViewProjX << "; on Y: " << QAAISViewProjY << "; on Z: " << QAAISViewProjZ << "\n";
  di << "Up on X : " << QAAISViewUpX << "; on Y: " << QAAISViewUpY << "; on Z: " << QAAISViewUpZ << "\n";
  di << "At on X : " << QAAISViewAtX << "; on Y: " << QAAISViewAtY << "; on Z: " << QAAISViewAtZ << "\n";
}

static Standard_Integer QAAISSetViewCharac (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if ( argc != 13 ) {
    di << "Usage : " << argv[0] << " scale center_X center_Y proj_X proj_Y proj_Z up_X up_Y up_Z at_X at_Y at_Z" << "\n";
    return 1;
  }
  Handle (V3d_View) QAAISView = ViewerTest::CurrentView ();
  if ( QAAISView.IsNull () ) {
    di << "You must initialize AISViewer before this command." << "\n";
    return 1;
  }
  Quantity_Factor QAAISViewScale = atof (argv [1]);
  Standard_Real QAAISViewCenterCoordinateX = atof (argv [2]);
  Standard_Real QAAISViewCenterCoordinateY = atof (argv [3]);
  Standard_Real QAAISViewProjX = atof (argv [4]);
  Standard_Real QAAISViewProjY = atof (argv [5]);
  Standard_Real QAAISViewProjZ = atof (argv [6]);
  Standard_Real QAAISViewUpX = atof (argv [7]);
  Standard_Real QAAISViewUpY = atof (argv [8]);
  Standard_Real QAAISViewUpZ = atof (argv [9]);
  Standard_Real QAAISViewAtX = atof (argv [10]);
  Standard_Real QAAISViewAtY = atof (argv [11]);
  Standard_Real QAAISViewAtZ = atof (argv [12]);
  QAAISView -> V3d_View::SetScale (QAAISViewScale);
  QAAISView -> V3d_View::SetCenter (QAAISViewCenterCoordinateX, QAAISViewCenterCoordinateY);
  QAAISView -> V3d_View::SetAt (QAAISViewAtX, QAAISViewAtY, QAAISViewAtZ);
  QAAISView -> V3d_View::SetProj (QAAISViewProjX, QAAISViewProjY, QAAISViewProjZ);
  QAAISView -> V3d_View::SetUp (QAAISViewUpX, QAAISViewUpY, QAAISViewUpZ);
  QAAISView -> V3d_View::SetProj (QAAISViewProjX, QAAISViewProjY, QAAISViewProjZ);
  return 0;
}

static Standard_Integer QAAISGetColorCoord (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
#if ! defined(WNT)
  if ( argc > 2 ) {
    di << "Usage : " << argv[0] << " [3d|2d]" << "\n";
    return 1;
  }
  Handle (Aspect_Window) QAAISWindow;

  Standard_Boolean is3d = 1;

  if(argc == 1 || !strcmp(argv[1],"3d")) {

    Handle (V3d_View) QAAIS_MainView = ViewerTest::CurrentView ();
    if ( QAAIS_MainView.IsNull () ) {
      di << "You must initialize AISViewer before this command." << "\n";
      return 1;
    }
    QAAISWindow = QAAIS_MainView -> V3d_View::Window ();
    is3d = 1;
  }

  if(argc == 2 && !strcmp(argv[1],"2d")) {
    Handle(V2d_View) V = Viewer2dTest::CurrentView();
    if (V.IsNull()) {
      di << "You must initialize AIS 2D Viewer before this command." << "\n";
      return 1;
    }
    QAAISWindow = V->Driver()->Window();
    is3d = 0;
  }

  Standard_Integer QAAIS_WindowSize_X = 0;
  Standard_Integer QAAIS_WindowSize_Y = 0;
  QAAISWindow->Size(QAAIS_WindowSize_X, QAAIS_WindowSize_Y);
  Handle (Graphic3d_GraphicDevice) QAAIS_GraphicDevice = new Graphic3d_GraphicDevice (getenv ("DISPLAY"));

  Draw_Window QAAIS_CoordWindow ("coordinate", 421, 205, 200, 60);
  QAAIS_CoordWindow.DisplayWindow ();
  QAAIS_CoordWindow.SetColor (12);

  Handle (Xw_Window) QAAIS_ColorWindow = new Xw_Window (QAAIS_GraphicDevice, "color", 0.4074, 0.678, 0.1962, 0.06, Xw_WQ_3DQUALITY, Quantity_NOC_BLACK);
  Handle (V3d_Viewer) QAAIS_ColorViewer = new V3d_Viewer (QAAIS_GraphicDevice, Standard_ExtString ("COLOR"));
  Handle (V3d_View) QAAIS_ColorView = QAAIS_ColorViewer -> CreateView ();
  QAAIS_ColorWindow -> Map ();
  QAAIS_ColorView -> SetWindow (QAAIS_ColorWindow);
  QAAIS_ColorView -> SetBackgroundColor (Quantity_NOC_BLACK);
  QAAIS_ColorView -> Redraw ();

  Standard_Integer QAAIS_MousePoint_X = 0;
  Standard_Integer QAAIS_MousePoint_Y = 0;
  Standard_Character QAAIS_MousePointX[32];
  sprintf (QAAIS_MousePointX, "X : %d", QAAIS_MousePoint_X);
  Standard_Character QAAIS_MousePointY[32];
  sprintf (QAAIS_MousePointY, "Y : %d", QAAIS_MousePoint_Y);
  Standard_ShortReal QAAIS_ColorRED = 0;
  Standard_ShortReal QAAIS_ColorGRN = 0;
  Standard_ShortReal QAAIS_ColorBLU = 0;
  Quantity_Color QAAIS_ShowingColor;
  QAAIS_ShowingColor.SetValues (QAAIS_ColorRED, QAAIS_ColorGRN, QAAIS_ColorBLU, Quantity_TOC_RGB);
  Standard_Integer argccc = 5;
  const char *bufff[] = { "A", "B", "C", "D", "E" };
  const char **argvvv = (const char **) bufff;
  while ( is3d ? ViewerMainLoop (argccc, argvvv) : ViewerMainLoop2d (argccc, argvvv)) {
    Handle(TColStd_HSequenceOfReal) aSeq;
    if(is3d)
    {
      ViewerTest::GetMousePosition (QAAIS_MousePoint_X, QAAIS_MousePoint_Y);
      Handle (V3d_View) QAAIS_MainView = ViewerTest::CurrentView();
      aSeq = GetColorOfPixel (QAAIS_MainView->ToPixMap (QAAIS_WindowSize_X, QAAIS_WindowSize_Y, Image_TOI_RGB),
                              QAAIS_MousePoint_X, QAAIS_MousePoint_Y, 0);
    }
    else
    {
      Viewer2dTest::GetMousePosition (QAAIS_MousePoint_X, QAAIS_MousePoint_Y);
      aSeq = GetColorOfPixel (QAAISWindow->ToPixMap(),
                              QAAIS_MousePoint_X, QAAIS_MousePoint_Y, 0);
    }
    QAAIS_ColorRED = aSeq->Value(1);
    QAAIS_ColorGRN = aSeq->Value(2);
    QAAIS_ColorBLU = aSeq->Value(3);
    QAAIS_ShowingColor.SetValues (QAAIS_ColorRED, QAAIS_ColorGRN, QAAIS_ColorBLU, Quantity_TOC_RGB);
    QAAIS_ColorView -> SetBackgroundColor (QAAIS_ShowingColor);
    QAAIS_ColorView -> Redraw ();
    QAAIS_CoordWindow.Clear();
    sprintf (QAAIS_MousePointX, "X : %d", QAAIS_MousePoint_X);
    sprintf (QAAIS_MousePointY, "Y : %d", QAAIS_MousePoint_Y);
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
#if ! defined(WNT)
extern int V2dPickGrid (Draw_Interpretor& , Standard_Integer argc, const char ** argv);
#else
Standard_EXPORT int V2dPickGrid (Draw_Interpretor& , Standard_Integer argc, const char ** argv);
#endif
//==============================================================================
//function : V2dSetHighlightMode
//purpose  : QAv2dSetHighlightMode mode
//==============================================================================
static int V2dSetHighlightMode (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if (argc != 2)
  {
    di << "Usage: QAv2dSetHighlightMode mode" << "\n";
    return 1;
  }

  Viewer2dTest::StandardModeActivation(atoi(argv[1]));
  return 0;
}

#ifndef WNT
//==============================================================================
//function : QAAISGetPixelColor2d
//purpose  : QAAISGetPixelColor2d coord_X coord_Y Red Green Blue
//==============================================================================

static int QAAISGetPixelColor2d (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if (argc != 6 && argc != 3)
  {
    di << "Args: coord_X coord_Y [Red Green Blue]" << "\n";
    return 1;
  }

  Handle(V2d_View) V = Viewer2dTest::CurrentView();
  if (V.IsNull())
  {
    di << "You must initialize AIS 2D Viewer before this command." << "\n";
    return 1;
  }
  // Get Color
  Handle(Aspect_Window) QAAISWindow = V->Driver()->Window();

  Standard_ShortReal aCoordinateX = atoi(argv[1]);
  Standard_ShortReal aCoordinateY = atoi(argv[2]);

  // Get Color
  Standard_ShortReal aColorRED_V = 0;
  Standard_ShortReal aColorGRN_V = 0;
  Standard_ShortReal aColorBLU_V = 0;

  if ( argc == 6 ) {
    aColorRED_V = atof (argv [3]);
    aColorGRN_V = atof (argv [4]);
    aColorBLU_V = atof (argv [5]);

    di << "Begin aColorRED_User = " << aColorRED_V << "\n";
    di << "Begin aColorGRN_User = " << aColorGRN_V << "\n";
    di << "Begin aColorBLU_User = " << aColorBLU_V << "\n";
  }

  Standard_Integer aRadius = 1;
  if ( argc == 3 ) {
    aRadius=0;
  }

  Handle(TColStd_HSequenceOfReal) aSeq = GetColorOfPixel (QAAISWindow->ToPixMap(),
                                                          aCoordinateX, aCoordinateY,
                                                          aRadius);

  Standard_Boolean IsNotEqual = Standard_True;
  Standard_Integer i;
  for(i=1; i<=aSeq->Length();i+=3) {
    // mkv 29.04.03
    Standard_ShortReal aColorRED_R = (((Standard_ShortReal) ((Standard_Integer) (aSeq->Value(i+0) * 1000000))) / 1000000.);
    Standard_ShortReal aColorGRN_R = (((Standard_ShortReal) ((Standard_Integer) (aSeq->Value(i+1) * 1000000))) / 1000000.);
    Standard_ShortReal aColorBLU_R = (((Standard_ShortReal) ((Standard_Integer) (aSeq->Value(i+2) * 1000000))) / 1000000.);
    // mkv 29.04.03

    if ( argc == 3 ) {
      di << "RED : "    << aColorRED_R << " GREEN : " << aColorGRN_R  << " BLUE : "  << aColorBLU_R << "\n";
      IsNotEqual = Standard_False;
      break;
    }

    if (   aColorRED_R == aColorRED_V
	&& aColorGRN_R == aColorGRN_V
	&& aColorBLU_R == aColorBLU_V
	) {
      IsNotEqual = Standard_False;
      break;
    }
  }
  if (IsNotEqual) {
    di << "Faulty : colors are not equal." << "\n";
    return 1;
  }
  return 0;
}
#endif // !WNT

//==============================================================================
//function : QAMoveTo2d
//purpose  : QAMoveTo2d x y
//==============================================================================
static int QAMoveTo2d (Draw_Interpretor& di, int argc, const char ** argv)
{
  if (argc != 3)
  {
    di << "Usage : " << argv[0] << " x y" << "\n";
    return -1;
  }

  Handle(AIS2D_InteractiveContext) myAIScontext = Viewer2dTest::GetAIS2DContext();
  if (myAIScontext.IsNull())
  {
    di << "use 'v2dinit' command before " << argv[0] << "\n";
    return -1;
  }
  Viewer2dTest::CurrentEventManager()->MoveTo
    (atoi(argv[1]), atoi(argv[2]), Viewer2dTest::CurrentView());
  return 0;
}

//==============================================================================
//function : QASelect2d
//purpose  : QASelect2d x y
//==============================================================================
static int QASelect2d (Draw_Interpretor& di, int argc, const char ** argv)
{
  if (argc != 3)
  {
    di << "Usage : " << argv[0] << " x y" << "\n";
    return -1;
  }

  Handle(AIS2D_InteractiveContext) myAIScontext = Viewer2dTest::GetAIS2DContext();
  if (myAIScontext.IsNull())
  {
    di << "use 'v2dinit' command before " << argv[0] << "\n";
    return -1;
  }
  Viewer2dTest::CurrentEventManager()->MoveTo
    (atoi(argv[1]), atoi(argv[2]), Viewer2dTest::CurrentView());
  Viewer2dTest::CurrentEventManager()->Select();
  return 0;
}

//==============================================================================
//function : QAShiftSelect2d
//purpose  : QAShiftSelect2d x y
//==============================================================================
static int QAShiftSelect2d (Draw_Interpretor& di, int argc, const char ** argv)
{
  if (argc != 3)
  {
    di << "Usage : " << argv[0] << " x y" << "\n";
    return -1;
  }

  Handle(AIS2D_InteractiveContext) myAIScontext = Viewer2dTest::GetAIS2DContext();
  if (myAIScontext.IsNull())
  {
    di << "use 'v2dinit' command before " << argv[0] << "\n";
    return -1;
  }
  Viewer2dTest::CurrentEventManager()->MoveTo
    (atoi(argv[1]), atoi(argv[2]), Viewer2dTest::CurrentView());
  Viewer2dTest::CurrentEventManager()->ShiftSelect();
  return 0;
}

//==============================================================================
//function : V2dZoom
//purpose  : QAv2dzoom zoom_factor
//==============================================================================
static int V2dZoom (Draw_Interpretor& di, int argc, const char ** argv)
{
  if (argc != 2)
  {
    di << "Usage : " << argv[0] << " zoom_factor" << "\n";
    return -1;
  }

  Handle(AIS2D_InteractiveContext) myAIScontext = Viewer2dTest::GetAIS2DContext();
  if (myAIScontext.IsNull())
  {
    di << "use 'v2dinit' command before " << argv[0] << "\n";
    return -1;
  }
  Viewer2dTest::CurrentView()->Zoom(atof(argv[1]));
  return 0;
}

//==============================================================================
//function : V2dPan
//purpose  : QAv2dpan dx dy
//==============================================================================
static int V2dPan (Draw_Interpretor& di, int argc, const char ** argv)
{
  if (argc != 3)
  {
    di << "Usage : " << argv[0] << " dx dy" << "\n";
    return -1;
  }

  Handle(AIS2D_InteractiveContext) myAIScontext = Viewer2dTest::GetAIS2DContext();
  if (myAIScontext.IsNull())
  {
    di << "use 'v2dinit' command before " << argv[0] << "\n";
    return -1;
  }
  Viewer2dTest::CurrentView()->Pan(atof(argv[1]), atof(argv[2]));
  return 0;
}

//==============================================================================
//function : V2dGetViewCharac
//purpose  : v2dGetViewCharac
//==============================================================================
static int V2dGetViewCharac (Draw_Interpretor& di, int si, const char ** /*sc*/)
{
  if (si != 1)
  {
    di << "Use - v2dGetViewCharac" << "\n";
    return 1;
  }

  Handle(V2d_View) V = Viewer2dTest::CurrentView();
  if (V.IsNull())
  {
    di << "You must initialize AIS 2D Viewer before this command." << "\n";
    return 1;
  }

//  Quantity_Factor aViewScale = V->Scale();

  Standard_Real aCenterCoordX = 0.0;
  Standard_Real aCenterCoordY = 0.0;
  V->Center(aCenterCoordX, aCenterCoordY);

  Standard_Real aViewProjX = 0.0;
  Standard_Real aViewProjY = 0.0;
  Standard_Real aViewProjZ = 0.0;
//  V->Proj(aViewProjX, aViewProjY, aViewProjZ);

  Standard_Real aViewUpX = 0.0;
  Standard_Real aViewUpY = 0.0;
  Standard_Real aViewUpZ = 0.0;
//  V->Up(aViewUpX, aViewUpY, aViewUpZ);

  Standard_Real aViewAtX = 0.0;
  Standard_Real aViewAtY = 0.0;
  Standard_Real aViewAtZ = 0.0;
//  V->At(aViewAtX, aViewAtY, aViewAtZ);

//  cout << "Scale of current view: " << aViewScale << endl;
//  cout << "Center on X : "<< aViewCenterCoordX << "; on Y: " << aViewCenterCoordY << endl;
//  cout << "Proj on X : " << aViewProjX << "; on Y: " << aViewProjY << "; on Z: " << aViewProjZ << endl;
//  cout << "Up on X : " << aViewUpX << "; on Y: " << aViewUpY << "; on Z: " << aViewUpZ << endl;
//  cout << "At on X : " << aViewAtX << "; on Y: " << aViewAtY << "; on Z: " << aViewAtZ << endl;

//  cout << aViewScale << " " << aViewCenterCoordX << " " << aViewCenterCoordY << " ";
  di << aViewProjX << " " << aViewProjY << " " << aViewProjZ << " ";
  di << aViewUpX << " " << aViewUpY << " " << aViewUpZ << " ";
  di << aViewAtX << " " << aViewAtY << " " << aViewAtZ << "\n";
  return 0;
}

//==============================================================================
//function : V2dSetViewCharac
//purpose  : v2dSetViewCharac
//==============================================================================
static int V2dSetViewCharac (Draw_Interpretor& di, int si, const char ** sc)
{
  if (si != 13)
  {
    di << "Use - v2dSetViewCharac scale center(X Y) proj(X Y Z) up(X Y Z) at(X Y Z)" << "\n";
    return 1;
  }

  Handle(V2d_View) V = Viewer2dTest::CurrentView();
  if (V.IsNull())
  {
    di << "You must initialize AIS 2D Viewer before this command." << "\n";
    return 1;
  }

  Quantity_Factor aViewScale = atof(sc[1]);

  Standard_Real aViewCenterCoordX = atof(sc[2]);
  Standard_Real aViewCenterCoordY = atof(sc[3]);

  Standard_Real aViewProjX = atof(sc[4]);
  Standard_Real aViewProjY = atof(sc[5]);
  Standard_Real aViewProjZ = atof(sc[6]);

  Standard_Real aViewUpX = atof(sc[7]);
  Standard_Real aViewUpY = atof(sc[8]);
  Standard_Real aViewUpZ = atof(sc[9]);

  Standard_Real aViewAtX = atof(sc[10]);
  Standard_Real aViewAtY = atof(sc[11]);
  Standard_Real aViewAtZ = atof(sc[12]);

//  V->SetScale(aViewScale);
//  V->SetCenter(aViewCenterCoordX, aViewCenterCoordY);
//  V->SetAt(aViewAtX, aViewAtY, aViewAtZ);
//  V->SetProj(aViewProjX, aViewProjY, aViewProjZ);
//  V->SetUp(aViewUpX, aViewUpY, aViewUpZ);
//  V->SetProj(aViewProjX, aViewProjY, aViewProjZ);
  return 0;
}

//=======================================================================
//function : QAxwd_2d
//purpose  :
//=======================================================================
static int QAxwd_2d (Draw_Interpretor& di, int argc, const char ** argv)
{
  if (argc != 2)
  {
    di << "Usage : " << argv[0] << " filename" << "\n";
    return -1;
  }

  Handle(AIS2D_InteractiveContext) myAIScontext = Viewer2dTest::GetAIS2DContext();
  if (myAIScontext.IsNull())
  {
    di << "use 'v2dinit' command before " << argv[0] << "\n";
    return -1;
  }
  Handle(V2d_View) V = Viewer2dTest::CurrentView();
  V->Dump(argv[1]);
  return 0;
}

#ifndef WNT
extern Draw_Viewer dout;
extern XW_STATUS Xw_save_image_adv (Display *aDisplay,Window aWindow,XWindowAttributes aWinAttr,XImage *aPximage,Colormap aColormap,int aNcolors,char *filename);
extern Display*         Draw_WindowDisplay;
extern Colormap         Draw_WindowColorMap;
#else
Standard_IMPORT Draw_Viewer dout;
int __WNT_API SaveWindowToFile (Handle( WNT_GraphicDevice )& gDev,
				HWND hWnd, char* fName, int x, int y, int w, int h);
#endif

//=======================================================================
//function : QAxwd
//purpose  :
//=======================================================================
static int QAxwd (Draw_Interpretor& di, int argc, const char ** argv)
{
  if (argc < 2)
  {
    di << "Usage : " << argv[0] << " [id=1] filename" << "\n";
    return -1;
  }

  // enforce repaint if necessary
  Standard_Integer id = 1;
  const char* file = argv[1];
  if (argc > 2) {
    id  = atoi(argv[1]);
    file = argv[2];
  }

  after(id);

  dout.Flush();
  if(dout.HasView(id)) {
#if defined (WNT)
    Handle(WNT_GraphicDevice) aGd = new WNT_GraphicDevice(Standard_False);
    RECT rc;
    GetClientRect((HWND)dout.GetWindow(id), &rc);
    SaveWindowToFile(aGd,(HWND)dout.GetWindow(id),(char*)file,rc.left, rc.top,rc.right-rc.left, rc.bottom-rc.top);
#else
    XSync(Draw_WindowDisplay,True);

    XWindowAttributes winAttr;
    XGetWindowAttributes (Draw_WindowDisplay, dout.GetWindow(id), &winAttr);

    XImage *pximage = XGetImage(Draw_WindowDisplay,dout.GetWindow(id),
				0,0,winAttr.width,winAttr.height,
				AllPlanes,ZPixmap);

    Xw_save_image_adv(Draw_WindowDisplay,dout.GetWindow(id),winAttr,pximage,Draw_WindowColorMap,256,(char*)file);
#endif
  }

  return 0;
}

//=======================================================================
//function : QA2dGetIndexes
//purpose  :
//=======================================================================
static int QA2dGetIndexes (Draw_Interpretor& di, int /*argc*/, const char ** argv)
{
  Handle(AIS2D_InteractiveContext) myAIScontext = Viewer2dTest::GetAIS2DContext();
  if (myAIScontext.IsNull())
  {
    di << "use 'v2dinit' command before " << argv[0] << "\n";
    return -1;
  }
  Handle(Aspect_WindowDriver) aWindowDriver = Viewer2dTest::CurrentView()->Driver();

  Standard_Integer aFontMin, aFontMax, aColorMin, aColorMax;
  aWindowDriver->FontBoundIndexs(aFontMin, aFontMax);
  di << "Available font  indexes are " << aFontMin << " - " << aFontMax << "\n";
  aWindowDriver->ColorBoundIndexs(aColorMin, aColorMax);
  di << "Available color indexes are " << aColorMin << " - " << aColorMax << "\n";
}

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

//=======================================================================
//function : QAAddOrRemoveSelected
//purpose  :
//=======================================================================
static Standard_Integer QAAddOrRemoveSelected (Draw_Interpretor& di, Standard_Integer n, const char ** a)
{
  if( n != 2)
  {
    di<<"Usage : QAAddOrRemoveSelected shape \n";
    return 1;
  }
  //get AIS_Shape:
  Handle(AIS_InteractiveContext) anAISCtx = ViewerTest::GetAISContext();

  //ViewerTest_DoubleMapOfInteractiveAndName& aMap =
  //                       ViewerTest::GetDataMapOfAIS ();
  ViewerTest_DoubleMapOfInteractiveAndName& aMap = GetMapOfAIS();

  TCollection_AsciiString aName(a[1]);
  Handle(AIS_InteractiveObject) AISObj;

  if(aMap.IsBound2(aName)){
    AISObj = Handle(AIS_InteractiveObject)::DownCast(aMap.Find2(aName));
    if(AISObj.IsNull()){
      di<<"No interactive object \n";
      return 1;
    }

    if(anAISCtx->HasOpenedContext()){
      anAISCtx->InitSelected();
      anAISCtx->AddOrRemoveSelected(AISObj);
    }
    else {
      anAISCtx->InitCurrent();
      anAISCtx->AddOrRemoveCurrentObject(AISObj);
    }
    return 0;
  }
  //select this shape:
  else {
    di<<"Use 'vdisplay' before";
    return 1;
  }
}

//=======================================================================
//function : QASetZClippingMode
//purpose  :
//=======================================================================
static Standard_Integer QASetZClippingMode (Draw_Interpretor& di, int argc, const char ** argv)
{
  if (argc != 2) {
    di << "Usage : " << argv[0] << " mode(OFF/BACK/FRONT/SLICE)" << "\n";
    return -1;
  }

  Handle(AIS_InteractiveContext) AIScontext = ViewerTest::GetAISContext();
  if(AIScontext.IsNull()) {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return -1;
  }

  Standard_Integer aStatus = 0;
  V3d_TypeOfZclipping ZClippingMode;
  if ( strcmp (argv [1], "OFF") == 0 ) {
    aStatus = 1;
    ZClippingMode = V3d_OFF;
  }
  if ( strcmp (argv [1], "BACK") == 0 ) {
    aStatus = 1;
    ZClippingMode = V3d_BACK;
  }
  if ( strcmp (argv [1], "FRONT") == 0 ) {
    aStatus = 1;
    ZClippingMode = V3d_FRONT;
  }
  if ( strcmp (argv [1], "SLICE") == 0 ) {
    aStatus = 1;
    ZClippingMode = V3d_SLICE;
  }
  if (aStatus != 1)
  {
    di << "Bad mode; Usage : " << argv[0] << " mode(OFF/BACK/FRONT/SLICE)" << "\n";
    return -1;
  }

  Handle(V3d_View) aView = ViewerTest::CurrentView();
  aView->SetZClippingType(ZClippingMode);
  aView->Redraw();

  return 0;
}

//=======================================================================
//function : QAGetZClippingMode
//purpose  :
//=======================================================================
static Standard_Integer QAGetZClippingMode (Draw_Interpretor& di, int /*argc*/, const char ** argv)
{
  Handle(AIS_InteractiveContext) AIScontext = ViewerTest::GetAISContext();
  if(AIScontext.IsNull()) {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return -1;
  }

  Handle(V3d_View) aView = ViewerTest::CurrentView();
  TCollection_AsciiString ZClippingModeString;
  Quantity_Length Depth, Width;
  V3d_TypeOfZclipping ZClippingMode = aView->ZClipping(Depth, Width);
  switch (ZClippingMode)
  {
  case V3d_OFF:
    ZClippingModeString.Copy("OFF");
    break;
  case V3d_BACK:
    ZClippingModeString.Copy("BACK");
    break;
  case V3d_FRONT:
    ZClippingModeString.Copy("FRONT");
    break;
  case V3d_SLICE:
    ZClippingModeString.Copy("SLICE");
    break;
  default:
    ZClippingModeString.Copy(TCollection_AsciiString(ZClippingMode));
    break;
  }
  di << "ZClippingMode = " << ZClippingModeString.ToCString() << "\n";

  return 0;
}

#include <V2d_View.hxx>
#include <AIS2D_InteractiveObject.hxx>
#include <Graphic2d_Circle.hxx>
#include <Graphic2d_TypeOfPolygonFilling.hxx>
#include <V2d_Viewer.hxx>
#include <Viewer2dTest_DoubleMapOfInteractiveAndName.hxx>

#if ! defined(WNT)
extern Viewer2dTest_DoubleMapOfInteractiveAndName& GetMapOfAIS2D();
#else
Standard_EXPORT Viewer2dTest_DoubleMapOfInteractiveAndName& GetMapOfAIS2D();
#endif

//=======================================================================
//function :  QAv2dcircle
//purpose  :
//=======================================================================
static Standard_Integer QAv2dcircle (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  Handle(AIS2D_InteractiveContext) aContext = Viewer2dTest::GetAIS2DContext();
  if(aContext.IsNull()) {
    di << "ERROR: Use 'v2dinit' command before " << argv[0] << "\n";
    return -1;
  }
  if(argc < 7){
    di<<"Usage : " << argv[0] << " CircleName X Y Radius Alpha Beta [Color_index]\n";
    return -1;
  }
  Handle(V2d_View) V = Viewer2dTest::CurrentView();

  TCollection_AsciiString name = argv[1];
  Standard_Real x = atof(argv[2]);
  Standard_Real y = atof(argv[3]);
  Standard_Real radius = atof(argv[4]);
  Standard_Real alpha = atof(argv[5]);
  Standard_Real beta = atof(argv[6]);

  if (GetMapOfAIS2D().IsBound2(name)) {
    di << "There is already an object with name " << name.ToCString() << "\n";
    return -1;
  }
  Handle(AIS2D_InteractiveObject) aisobj = new AIS2D_InteractiveObject();
  aisobj->SetView(V->View());
  Handle(Graphic2d_Circle) circle = new Graphic2d_Circle(aisobj,x,y,radius,alpha,beta);
  if(argc > 7){
    Standard_Integer color_index = atoi(argv[7]);
    circle->SetTypeOfPolygonFilling(Graphic2d_TOPF_FILLED);
    circle->SetInteriorColorIndex(color_index);
  }
  GetMapOfAIS2D().Bind(aisobj, name);
  aisobj->Display();
  V->Viewer()->Update();
  return 0;
}

#include <Draw_Viewer.hxx>
#include <Draw.hxx>

#ifndef WNT
extern Draw_Viewer dout;
#else
Standard_IMPORT Draw_Viewer dout;
#endif

static Standard_Integer ViewId(const Standard_CString a)
{
  Standard_Integer id = atoi(a);
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

//=======================================================================
//function : QAwzoom
//purpose  :
//=======================================================================

static Standard_Integer QAwzoom(Draw_Interpretor& di, Standard_Integer argc, const char ** argv) {
  if(argc < 6){
    di<<"Usage : " << argv[0] << " view-id X1 Y1 X2 Y2\n";
    return -1;
  }

  Standard_Integer id = ViewId(argv [1]);
  if (id < 0) {
    return -1;
  }

  Standard_Integer X1 = atoi (argv [2]);
  Standard_Integer Y1 = atoi (argv [3]);
  Standard_Integer X2 = atoi (argv [4]);
  Standard_Integer Y2 = atoi (argv [5]);

  Standard_Real zx,zy;

  Standard_Integer X,Y,W,H;
  dout.GetPosSize(id,X,Y,W,H);

  if ((X1 == X2) || (Y1 == Y2)) return 0;

  zx = (Standard_Real) Abs(X2-X1) / (Standard_Real) W;
  zy = (Standard_Real) Abs(Y2-Y1) / (Standard_Real) H;
  if (zy > zx) zx = zy;
  zx = 1/zx;
  if (X2 < X1) X1 = X2;
  if (Y2 > Y1) Y1 = Y2;
  X1 = (Standard_Integer ) (X1*zx);
  Y1 = (Standard_Integer ) (Y1*zx);
  dout.SetZoom(id,zx*dout.Zoom(id));
  dout.SetPan(id,-X1,-Y1);
  dout.RepaintView(id);
  if (dout.HasView(id)) {
    char title[255];
    sprintf(title,"%d : %s - Zoom %f",id,dout.GetType(id),dout.Zoom(id));
    dout.SetTitle(id,title);
  }
  return 0;
}

#include <Draw_Display.hxx>

//=======================================================================
//function : QAGetCoordinatesWzoom
//purpose  :
//=======================================================================

static Standard_Integer QAGetCoordinatesWzoom(Draw_Interpretor& di, Standard_Integer, const char **)
{
  Standard_Integer id1,X1,Y1,b;
  Standard_Integer X2,Y2;
  Standard_Real dX1,dY1,dX2,dY2;
  di << "Pick first corner"<<"\n";
  dout.Select(id1,X1,Y1,b);

  gp_Trsf T;
  gp_Pnt P0(0,0,0);
  dout.GetTrsf(id1,T);
  T.Invert();
  P0.Transform(T);
  Standard_Real z = dout.Zoom(id1);

  dX1=X1;       dY1=Y1;
  dX1-=P0.X();  dY1-=P0.Y();
  dX1/=z;       dY1/=z;

  if (b != 1) return 0;
  if (id1 < 0) return 0;
  Draw_Display d = dout.MakeDisplay(id1);
  d.SetColor(Draw_blanc);
  d.SetMode(10);
  Standard_Real dOX2 = dX1;
  Standard_Real dOY2 = dY1;
  d.Draw(gp_Pnt2d(dX1,dY1),gp_Pnt2d(dX1,dOY2));
  d.Draw(gp_Pnt2d(dX1,dOY2),gp_Pnt2d(dOX2,dOY2));
  d.Draw(gp_Pnt2d(dOX2,dOY2),gp_Pnt2d(dOX2,dY1));
  d.Draw(gp_Pnt2d(dOX2,dY1),gp_Pnt2d(dX1,dY1));
  d.Flush();
  Standard_Real zx,zy;
  Standard_Integer X,Y,W,H;
  dout.GetPosSize(id1,X,Y,W,H);
  di << "Pick second corner"<<"\n";
  b = 0;
  while (b == 0) {
    dout.Select(id1,X2,Y2,b,Standard_False);
    dX2=X2;          dY2=Y2;
    dX2-=P0.X();     dY2-=P0.Y();
    dX2/=z;          dY2/=z;

    d.Draw(gp_Pnt2d(dX1,dY1),gp_Pnt2d(dX1,dOY2));
    d.Draw(gp_Pnt2d(dX1,dOY2),gp_Pnt2d(dOX2,dOY2));
    d.Draw(gp_Pnt2d(dOX2,dOY2),gp_Pnt2d(dOX2,dY1));
    d.Draw(gp_Pnt2d(dOX2,dY1),gp_Pnt2d(dX1,dY1));
    d.Draw(gp_Pnt2d(dX1,dY1),gp_Pnt2d(dX1,dY2));
    d.Draw(gp_Pnt2d(dX1,dY2),gp_Pnt2d(dX2,dY2));
    d.Draw(gp_Pnt2d(dX2,dY2),gp_Pnt2d(dX2,dY1));
    d.Draw(gp_Pnt2d(dX2,dY1),gp_Pnt2d(dX1,dY1));
    d.Flush();
    dOX2 = dX2;
    dOY2 = dY2;
  }
  d.Draw(gp_Pnt2d(dX1,dY1),gp_Pnt2d(dX1,dOY2));
  d.Draw(gp_Pnt2d(dX1,dOY2),gp_Pnt2d(dOX2,dOY2));
  d.Draw(gp_Pnt2d(dOX2,dOY2),gp_Pnt2d(dOX2,dY1));
  d.Draw(gp_Pnt2d(dOX2,dY1),gp_Pnt2d(dX1,dY1));
  d.Flush();
  if (b != 1) return 0;

  if ((X1 == X2) || (Y1 == Y2)) return 0;

  di << "X1=" << X1 << " Y1=" << Y1 <<"\n";
  di << "X2=" << X2 << " Y2=" << Y2 <<"\n";

  zx = (Standard_Real) Abs(X2-X1) / (Standard_Real) W;
  zy = (Standard_Real) Abs(Y2-Y1) / (Standard_Real) H;
  if (zy > zx) zx = zy;
  zx = 1/zx;
  if (X2 < X1) X1 = X2;
  if (Y2 > Y1) Y1 = Y2;
  X1 = (Standard_Integer ) (X1*zx);
  Y1 = (Standard_Integer ) (Y1*zx);
  dout.SetZoom(id1,zx*dout.Zoom(id1));
  dout.SetPan(id1,-X1,-Y1);
  dout.RepaintView(id1);
  //SetTitle(id1);
  char title[255];
  sprintf(title,"%d : %s - Zoom %f",id1,dout.GetType(id1),dout.Zoom(id1));
  dout.SetTitle(id1,title);
  return 0;
}

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

static Standard_Integer QANbSelected (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if(argc != 1) {
    di << "Usage : " << argv[0] << "\n";
    return 1;
  }
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if(aContext.IsNull()) {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return 1;
  }
  di << aContext->NbSelected() << "\n";
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

static Standard_Integer QAPurgeDisplay (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if (argc > 2) {
    di << "Usage : " << argv[0] << " [CollectorToo=0/1]" << "\n";
    return 1;
  }
  Standard_Boolean CollectorTooBoolean = Standard_False;
  if (argc == 2) {
    Standard_Integer CollectorTooInteger = atoi (argv [1]);
    if (CollectorTooInteger != 0)
      CollectorTooBoolean = Standard_True;
  }
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if (aContext.IsNull()) {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return 1;
  }
  aContext->CloseAllContexts(Standard_False);
  di << aContext->PurgeDisplay(CollectorTooBoolean) << "\n";
  return 0;
}

static Standard_Integer QACloseLocalContext (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if (argc > 3) {
    di << "Usage : " << argv[0] << " [Index [updateviewer=1/0]]" << "\n";
    return 1;
  }
  Standard_Integer Index = -1;
  if (argc > 1) {
    Index = atoi (argv [1]);
  }
  //cout << "Index = " << Index << endl;
  Standard_Boolean updateviewerBoolean = Standard_True;
  if (argc == 3) {
    Standard_Integer updateviewerInteger = atoi (argv [2]);
    if (updateviewerInteger == 0)
      updateviewerBoolean = Standard_False;
  }
  //if (updateviewerBoolean)
  //  cout << "updateviewer = Standard_True" << endl;
  //else
  //  cout << "updateviewer = Standard_False" << endl;
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if (aContext.IsNull()) {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return 1;
  }
  aContext->CloseLocalContext(Index, updateviewerBoolean);
  return 0;
}

static Standard_Integer QACloseAllContexts (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if (argc > 2) {
    di << "Usage : " << argv[0] << " [updateviewer=1/0]" << "\n";
    return 1;
  }
  Standard_Boolean updateviewerBoolean = Standard_True;
  if (argc == 2) {
    Standard_Integer updateviewerInteger = atoi (argv [1]);
    if (updateviewerInteger == 0)
      updateviewerBoolean = Standard_False;
  }
  //if (updateviewerBoolean)
  //  cout << "updateviewer = Standard_True" << endl;
  //else
  //  cout << "updateviewer = Standard_False" << endl;
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if (aContext.IsNull()) {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return 1;
  }
  aContext->CloseAllContexts(updateviewerBoolean);
  return 0;
}

static Standard_Integer QAIndexOfCurrentLocal (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if (argc > 1) {
    di << "Usage : " << argv[0] << "\n";
    return 1;
  }
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if (aContext.IsNull()) {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return 1;
  }
  di << aContext->IndexOfCurrentLocal() << "\n";
  return 0;
}

#include <AIS_ListOfInteractive.hxx>
static Standard_Integer QADisplayedObjects (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if (argc > 2) {
    di << "Usage : " << argv[0] << " [OnlyFromNeutral=0/1]" << "\n";
    return 1;
  }

  Standard_Boolean OnlyFromNeutralBoolean = Standard_False;
  if (argc == 2) {
    Standard_Integer OnlyFromNeutralInteger = atoi (argv [1]);
    if (OnlyFromNeutralInteger != 0)
      OnlyFromNeutralBoolean = Standard_True;
  }
  //if (OnlyFromNeutralBoolean)
  //  cout << "OnlyFromNeutral = Standard_True" << endl;
  //else
  //  cout << "OnlyFromNeutral = Standard_False" << endl;

  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if (aContext.IsNull()) {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return 1;
  }
  Standard_Integer ListOfInteractiveExtent = 0;
  if(!aContext->HasOpenedContext()) {
    ListOfInteractiveExtent = 0;
  } else {
    AIS_ListOfInteractive ListOfInteractive;
    aContext->DisplayedObjects(ListOfInteractive, OnlyFromNeutralBoolean);
    ListOfInteractiveExtent = ListOfInteractive.Extent();
  }
  di << ListOfInteractiveExtent << "\n";
  return 0;
}

static Standard_Integer QASelectRectangle(Draw_Interpretor& di, Standard_Integer argc, const char ** argv )
{
  if(argc != 5) {
    di << "Usage : " << argv[0] << " x1 y1 x2 y2" << "\n";
    return -1;
  }

  Handle(AIS_InteractiveContext) myAIScontext = ViewerTest::GetAISContext();
  if(myAIScontext.IsNull()) {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return -1;
  }

  Standard_Integer x1 = atoi(argv[1]);
  Standard_Integer y1 = atoi(argv[2]);
  Standard_Integer x2 = atoi(argv[3]);
  Standard_Integer y2 = atoi(argv[4]);

  Handle(ViewerTest_EventManager) aCurrentEventManager = ViewerTest::CurrentEventManager();
//  Handle(V3d_View) aCurrentView = ViewerTest::CurrentView();

  aCurrentEventManager->MoveTo(x1,y1);
  aCurrentEventManager->Select(x1,y1,x2,y2);
  aCurrentEventManager->MoveTo(x2,y2);

  return 0;
}

static Standard_Integer QAShiftSelectRectangle(Draw_Interpretor& di, Standard_Integer argc, const char ** argv )
{
  if(argc != 5) {
    di << "Usage : " << argv[0] << " x1 y1 x2 y2" << "\n";
    return -1;
  }

  Handle(AIS_InteractiveContext) myAIScontext = ViewerTest::GetAISContext();
  if(myAIScontext.IsNull()) {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return -1;
  }
  Standard_Integer x1 = atoi(argv[1]);
  Standard_Integer y1 = atoi(argv[2]);
  Standard_Integer x2 = atoi(argv[3]);
  Standard_Integer y2 = atoi(argv[4]);

  Handle(ViewerTest_EventManager) aCurrentEventManager = ViewerTest::CurrentEventManager();
//  Handle(V3d_View) aCurrentView = ViewerTest::CurrentView();

  aCurrentEventManager->MoveTo(x1,y1);
  aCurrentEventManager->ShiftSelect(x1,y1,x2,y2);
  aCurrentEventManager->MoveTo(x2,y2);

  return 0;
}

static int QASelect2dRectangle (Draw_Interpretor& di, int argc, const char ** argv)
{
  if (argc != 5)
  {
    di << "Usage : " << argv[0] << " x1 y1 x2 y2" << "\n";
    return -1;
  }

  Handle(AIS2D_InteractiveContext) myAIScontext = Viewer2dTest::GetAIS2DContext();
  if (myAIScontext.IsNull())
  {
    di << "use 'v2dinit' command before " << argv[0] << "\n";
    return -1;
  }

  Standard_Integer x1 = atoi(argv[1]);
  Standard_Integer y1 = atoi(argv[2]);
  Standard_Integer x2 = atoi(argv[3]);
  Standard_Integer y2 = atoi(argv[4]);

  Handle(Viewer2dTest_EventManager) aCurrentEventManager = Viewer2dTest::CurrentEventManager();
  Handle(V2d_View) aCurrentView = Viewer2dTest::CurrentView();

  aCurrentEventManager->MoveTo(x1,y1,aCurrentView);
  aCurrentEventManager->Select(x1,y1,x2,y2,aCurrentView);
  aCurrentEventManager->MoveTo(x2,y2,aCurrentView);

  return 0;
}

static int QAShiftSelect2dRectangle (Draw_Interpretor& di, int argc, const char ** argv)
{
  if (argc != 5)
  {
    di << "Usage : " << argv[0] << " x1 y1 x2 y2" << "\n";
    return -1;
  }

  Handle(AIS2D_InteractiveContext) myAIScontext = Viewer2dTest::GetAIS2DContext();
  if (myAIScontext.IsNull())
  {
    di << "use 'v2dinit' command before " << argv[0] << "\n";
    return -1;
  }

  Standard_Integer x1 = atoi(argv[1]);
  Standard_Integer y1 = atoi(argv[2]);
  Standard_Integer x2 = atoi(argv[3]);
  Standard_Integer y2 = atoi(argv[4]);

  Handle(Viewer2dTest_EventManager) aCurrentEventManager = Viewer2dTest::CurrentEventManager();
  Handle(V2d_View) aCurrentView = Viewer2dTest::CurrentView();

  aCurrentEventManager->MoveTo(x1,y1,aCurrentView);
  aCurrentEventManager->ShiftSelect(x1,y1,x2,y2,aCurrentView);
  aCurrentEventManager->MoveTo(x2,y2,aCurrentView);

  return 0;
}

//=======================================================================
//function : QARotateV3dView
//purpose  :
//=======================================================================
static Standard_Integer QARotateV3dView (Draw_Interpretor& di, int argc, const char ** argv)
{
  Handle(AIS_InteractiveContext) AIScontext = ViewerTest::GetAISContext();
  if(AIScontext.IsNull()) {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return -1;
  }
  if(argc < 4 || argc > 5){
    di<<"Usage : " << argv[0] << " Ax Ay Az [Start(1/0)]\n";
    return -1;
  }
  Standard_Real Ax = atof(argv[1]);
  Standard_Real Ay = atof(argv[2]);
  Standard_Real Az = atof(argv[3]);
  Standard_Boolean Start = Standard_True;
  if (argc == 5) {
    Standard_Integer StartInteger = atoi(argv[4]);
    if (StartInteger > 0) {
      Start = Standard_True;
    } else {
      Start = Standard_False;
    }
  }

  Handle(V3d_View) aView = ViewerTest::CurrentView();
  aView->Rotate(Ax,Ay,Az,Start);
  return 0;
}

//=======================================================================
//function : QAMoveV3dView
//purpose  :
//=======================================================================
static Standard_Integer QAMoveV3dView (Draw_Interpretor& di, int argc, const char ** argv)
{
  Handle(AIS_InteractiveContext) AIScontext = ViewerTest::GetAISContext();
  if(AIScontext.IsNull()) {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return -1;
  }
  if(argc < 4 || argc > 5){
    di<<"Usage : " << argv[0] << " Dx Dy Dz [Start(1/0)]\n";
    return -1;
  }
  Standard_Real Dx = atof(argv[1]);
  Standard_Real Dy = atof(argv[2]);
  Standard_Real Dz = atof(argv[3]);
  Standard_Boolean Start = Standard_True;
  if (argc == 5) {
    Standard_Integer StartInteger = atoi(argv[4]);
    if (StartInteger > 0) {
      Start = Standard_True;
    } else {
      Start = Standard_False;
    }
  }

  Handle(V3d_View) aView = ViewerTest::CurrentView();
  aView->Move(Dx,Dy,Dz,Start);
  return 0;
}

//=======================================================================
//function : QATranslateV3dView
//purpose  :
//=======================================================================
static Standard_Integer QATranslateV3dView (Draw_Interpretor& di, int argc, const char ** argv)
{
  Handle(AIS_InteractiveContext) AIScontext = ViewerTest::GetAISContext();
  if(AIScontext.IsNull()) {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return -1;
  }
  if(argc < 4 || argc > 5){
    di<<"Usage : " << argv[0] << " Dx Dy Dz [Start(1/0)]\n";
    return -1;
  }
  Standard_Real Dx = atof(argv[1]);
  Standard_Real Dy = atof(argv[2]);
  Standard_Real Dz = atof(argv[3]);
  Standard_Boolean Start = Standard_True;
  if (argc == 5) {
    Standard_Integer StartInteger = atoi(argv[4]);
    if (StartInteger > 0) {
      Start = Standard_True;
    } else {
      Start = Standard_False;
    }
  }

  Handle(V3d_View) aView = ViewerTest::CurrentView();
  aView->Translate(Dx,Dy,Dz,Start);
  return 0;
}

//=======================================================================
//function : QATurnV3dView
//purpose  :
//=======================================================================
static Standard_Integer QATurnV3dView (Draw_Interpretor& di, int argc, const char ** argv)
{
  Handle(AIS_InteractiveContext) AIScontext = ViewerTest::GetAISContext();
  if(AIScontext.IsNull()) {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return -1;
  }
  if(argc < 4 || argc > 5){
    di<<"Usage : " << argv[0] << " Ax Ay Az [Start(1/0)]\n";
    return -1;
  }
  Standard_Real Ax = atof(argv[1]);
  Standard_Real Ay = atof(argv[2]);
  Standard_Real Az = atof(argv[3]);
  Standard_Boolean Start = Standard_True;
  if (argc == 5) {
    Standard_Integer StartInteger = atoi(argv[4]);
    if (StartInteger > 0) {
      Start = Standard_True;
    } else {
      Start = Standard_False;
    }
  }

  Handle(V3d_View) aView = ViewerTest::CurrentView();
  aView->Turn(Ax,Ay,Az,Start);
  return 0;
}

//=======================================================================
//function : QAPanningV3dView
//purpose  :
//=======================================================================
static Standard_Integer QAPanningV3dView (Draw_Interpretor& di, int argc, const char ** argv)
{
  Handle(AIS_InteractiveContext) AIScontext = ViewerTest::GetAISContext();
  if(AIScontext.IsNull()) {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return -1;
  }
  if(argc < 3 || argc > 5){
    di<<"Usage : " << argv[0] << " Dx Dy [Zoom [Start(1/0)]]\n";
    return -1;
  }
  Standard_Real Dx = atof(argv[1]);
  Standard_Real Dy = atof(argv[2]);
  Standard_Real Zoom = 1.;
  if (argc > 3) {
    Zoom = atof(argv[3]);
    if (Zoom <= 0.) {
      di<<"Bad Zoom value  : " << Zoom << "\n";
      return -1;
    }
  }
  Standard_Boolean Start = Standard_True;
  if (argc == 5) {
    Standard_Integer StartInteger = atoi(argv[4]);
    if (StartInteger > 0) {
      Start = Standard_True;
    } else {
      Start = Standard_False;
    }
  }

  Handle(V3d_View) aView = ViewerTest::CurrentView();
  aView->Panning(Dx,Dy,Zoom,Start);
  return 0;
}

//=======================================================================
//function : QASetZoomV3dView
//purpose  :
//=======================================================================
static Standard_Integer QASetZoomV3dView (Draw_Interpretor& di, int argc, const char ** argv)
{
  Handle(AIS_InteractiveContext) AIScontext = ViewerTest::GetAISContext();
  if(AIScontext.IsNull()) {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return -1;
  }
  if(argc < 2 || argc > 3){
    di<<"Usage : " << argv[0] << " Zoom [Start(1/0)]\n";
    return -1;
  }
  Standard_Real Zoom = atof(argv[1]);
  if (Zoom <= 0.) {
    di<<"Bad Zoom value  : " << Zoom << "\n";
    return -1;
  }
  Standard_Boolean Start = Standard_True;
  if (argc == 3) {
    Standard_Integer StartInteger = atoi(argv[2]);
    if (StartInteger > 0) {
      Start = Standard_True;
    } else {
      Start = Standard_False;
    }
  }

  Handle(V3d_View) aView = ViewerTest::CurrentView();
  aView->SetZoom(Zoom,Start);
  return 0;
}

//=======================================================================
//function : QASetSizeV3dView
//purpose  :
//=======================================================================
static Standard_Integer QASetSizeV3dView (Draw_Interpretor& di, int argc, const char ** argv)
{
  Handle(AIS_InteractiveContext) AIScontext = ViewerTest::GetAISContext();
  if(AIScontext.IsNull()) {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return -1;
  }
  if(argc != 2){
    di<<"Usage : " << argv[0] << " Size\n";
    return -1;
  }
  Standard_Real Size = atof(argv[1]);
  if (Size <= 0.) {
    di<<"Bad Size value  : " << Size << "\n";
    return -1;
  }

  Handle(V3d_View) aView = ViewerTest::CurrentView();
  aView->SetSize(Size);
  return 0;
}

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
  theCommands.Add("QAxwd_3d","QAxwd_3d filename",__FILE__,QAxwd_3d,group);
  theCommands.Add("QAMoveTo","QAMoveTo x y",__FILE__,QAMoveTo,group);
  theCommands.Add("QASelect","QASelect x y",__FILE__,QASelect,group);
  theCommands.Add("QAShiftSelect","QAShiftSelect x y",__FILE__,QAShiftSelect,group);
  theCommands.Add("QAUpdateLights","QAUpdateLights",__FILE__,QAUpdateLights,group);
  theCommands.Add("QASetAntialiasing","QASetAntialiasing [1/0]",__FILE__,QASetAntialiasing,group);
  theCommands.Add("QAvzfit","QAvzfit",__FILE__,QAvzfit,group);
  theCommands.Add("QAGetPixelColor", "QAGetPixelColor coordinate_X coordinate_Y [color_R color_G color_B]", __FILE__,QAAISGetPixelColor, group);
  theCommands.Add("QASetChoiceMode", "QASetChoiceMode mode switch ; Use without params to see a help ", __FILE__,QAAISSetChoiceMode, group);
  theCommands.Add("QAGetMousePoint", "QAGetMousePoint", __FILE__,QAAISGetMousePoint, group);
  theCommands.Add("QAGetViewCharac", "QAGetViewCharac", __FILE__,QAAISGetViewCharac, group);
  theCommands.Add("QASetViewCharac", "QASetViewCharac scale center_X center_Y proj_X proj_Y proj_Z up_X up_Y up_Z at_X at_Y at_Z", __FILE__,QAAISSetViewCharac, group);
  theCommands.Add("QAGetColorCoord", "QAGetColorCoord [3d|2d]", __FILE__,QAAISGetColorCoord, group);
#ifndef WNT
  theCommands.Add("QAAISGetPixelColor2d",
                  "QAAISGetPixelColor2d coord_X coord_Y [Red Green Blue] : Check a color of pixel",
                  __FILE__, QAAISGetPixelColor2d, group);
#endif

  theCommands.Add("v2dgetgrid",
		  "v2dgetgrid coord_X coord_Y [grid_X grid_Y] : Get/print coordinates of a grid point near to (coord_X, coord_Y)",
		  __FILE__, V2dPickGrid, group);

  theCommands.Add("QAv2dzoom",
		  "QAv2dzoom zoom_factor         : Set Scale Factor",
		  __FILE__, V2dZoom, group);

  theCommands.Add("QAv2dpan",
		  "QAv2dpan dx dy                : script analog of Ctrl+MB2",
		  __FILE__, V2dPan, group);

//  theCommands.Add("QAGetViewCharac2d",
//                  "QAGetViewCharac2d - dumps viewer characteristics",
//                  V2dGetViewCharac, group);

//  theCommands.Add("QASetViewCharac2d",
//                  "QASetViewCharac2d scale center(X Y) proj(X Y Z) up(X Y Z) at(X Y Z)",
//                  V2dSetViewCharac, group);

  theCommands.Add("QAMoveTo2d", "QAMoveTo2d x y", __FILE__, QAMoveTo2d, group);
  theCommands.Add("QASelect2d", "QASelect2d x y", __FILE__, QASelect2d, group);
  theCommands.Add("QAShiftSelect2d", "QAShiftSelect2d x y", __FILE__, QAShiftSelect2d, group);

  theCommands.Add("QAv2dSetHighlightMode",
                  "QAv2dSetHighlightMode mode", __FILE__, V2dSetHighlightMode, group);

  theCommands.Add("QAxwd_2d", "QAxwd_2d filename", __FILE__, QAxwd_2d, group);

  theCommands.Add("QAxwd", "QAxwd [id=1] filename", __FILE__, QAxwd, group);

  theCommands.Add("QA2dGetIndexes", "QA2dGetIndexes", __FILE__, QA2dGetIndexes, group);

  theCommands.Add("vtri_orig",
		  "vtri_orig         : vtri_orig trihedron_name  -  draws axis origin lines",
		  __FILE__,VTrihedronOrigins,group);

  theCommands.Add("QAAddOrRemoveSelected",
                  "QAAddOrRemoveSelected shape : selects the shape by AddOrRemoveSelected method",
		  __FILE__, QAAddOrRemoveSelected, group);

  theCommands.Add("QASetZClippingMode","QASetZClippingMode mode(OFF/BACK/FRONT/SLICE)",__FILE__,QASetZClippingMode,group);
  theCommands.Add("QAGetZClippingMode","QAGetZClippingMode",__FILE__,QAGetZClippingMode,group);
  theCommands.Add("QAv2dcircle", "QAv2dcircle CircleName X Y Radius Alpha Beta [Color_index]", __FILE__, QAv2dcircle, group);

  theCommands.Add("QAwzoom","QAwzoom view-id X1 Y1 X2 Y2; zoom on a window",__FILE__,QAwzoom,group);
  theCommands.Add("QAGetCoordinatesWzoom","QAGetCoordinatesWzoom ; Get coordinates for zoom on a window",__FILE__,QAGetCoordinatesWzoom,group);

  theCommands.Add("rename","rename name1 toname1 name2 toname2 ...",__FILE__,QArename,group);

  theCommands.Add ("QANbSelected", "QANbSelected", __FILE__, QANbSelected, group);
//#if defined(V2D)
//  theCommands.Add ("QANbSelected2d", "QANbSelected2d", __FILE__, QANbSelected2d, group);
//#endif

  theCommands.Add ("QAPurgeDisplay", "QAPurgeDisplay [CollectorToo=0/1]", __FILE__, QAPurgeDisplay, group);
  theCommands.Add ("QACloseLocalContext", "QACloseLocalContext [Index [updateviewer=1/0]]", __FILE__, QACloseLocalContext, group);
  theCommands.Add ("QACloseAllContexts", "QACloseAllContexts [updateviewer=1/0]", __FILE__, QACloseAllContexts, group);
  theCommands.Add ("QAIndexOfCurrentLocal", "QAIndexOfCurrentLocal", __FILE__, QAIndexOfCurrentLocal, group);
  theCommands.Add ("QADisplayedObjects", "QADisplayedObjects [OnlyFromNeutral=0/1]", __FILE__, QADisplayedObjects, group);

  theCommands.Add("QASelectRectangle","QASelectRectangle x1 y1 x2 y2",__FILE__,QASelectRectangle,group);
  theCommands.Add("QAShiftSelectRectangle","QAShiftSelectRectangle x1 y1 x2 y2",__FILE__,QAShiftSelectRectangle,group);
  theCommands.Add("QASelect2dRectangle","QASelect2dRectangle x1 y1 x2 y2",__FILE__,QASelect2dRectangle,group);
  theCommands.Add("QAShiftSelect2dRectangle","QAShiftSelect2dRectangle x1 y1 x2 y2",__FILE__,QAShiftSelect2dRectangle,group);

  theCommands.Add("QARotateV3dView","QARotateV3dView Ax Ay Az [Start(1/0)]",__FILE__,QARotateV3dView,group);
  theCommands.Add("QAMoveV3dView","QAMoveV3dView Dx Dy Dz [Start(1/0)]",__FILE__,QAMoveV3dView,group);
  theCommands.Add("QATranslateV3dView","QATranslateV3dView Dx Dy Dz [Start(1/0)]",__FILE__,QATranslateV3dView,group);
  theCommands.Add("QATurnV3dView","QATurnV3dView Ax Ay Az [Start(1/0)]",__FILE__,QATurnV3dView,group);
  theCommands.Add("QAPanningV3dView","QAPanningV3dView Dx Dy [Zoom [Start(1/0)]]",__FILE__,QAPanningV3dView,group);
  theCommands.Add("QASetZoomV3dView","QASetZoomV3dView Zoom [Start(1/0)]",__FILE__,QASetZoomV3dView,group);
  theCommands.Add("QASetSizeV3dView","QASetSizeV3dView Size",__FILE__,QASetSizeV3dView,group);
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
