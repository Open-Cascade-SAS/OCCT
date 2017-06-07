// Created on: 1994-07-27
// Created by: Remi LEQUETTE
// Copyright (c) 1994-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

// include windows.h first to have all definitions available
#ifdef _WIN32
#include <windows.h>
#endif

#include <Standard_ErrorHandler.hxx>

#include <tcl.h>
#include <Draw_Interpretor.hxx>
#include <Draw_Window.hxx>
#include <Draw_Appli.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <Image_AlienPixMap.hxx>
#include <NCollection_List.hxx>

extern Standard_Boolean Draw_VirtualWindows;
static Tcl_Interp *interp;        /* Interpreter for this application. */
static NCollection_List<Draw_Window::FCallbackBeforeTerminate> MyCallbacks;

void Draw_Window::AddCallbackBeforeTerminate(FCallbackBeforeTerminate theCB)
{
  MyCallbacks.Append(theCB);
}

void Draw_Window::RemoveCallbackBeforeTerminate(FCallbackBeforeTerminate theCB)
{
  NCollection_List<Draw_Window::FCallbackBeforeTerminate>::Iterator Iter(MyCallbacks);
  for(; Iter.More(); Iter.Next())
  {
    if (Iter.Value() == theCB)
    {
      MyCallbacks.Remove(Iter);
      break;
    }
  }
}

/*
 *----------------------------------------------------------------------
 *
 * Prompt --
 *
 *        Issue a prompt on standard output, or invoke a script
 *        to issue the prompt.
 *
 * Results:
 *        None.
 *
 * Side effects:
 *        A prompt gets output, and a Tcl script may be evaluated
 *        in interp.
 *
 *----------------------------------------------------------------------
 */

static void Prompt(Tcl_Interp *Interp, int partial)
{

  // MKV 29.03.05
#if ((TCL_MAJOR_VERSION > 8) || ((TCL_MAJOR_VERSION == 8) && (TCL_MINOR_VERSION >= 4))) && !defined(USE_NON_CONST)
    const char *promptCmd;
#else
    char *promptCmd;
#endif
    int code;
    Tcl_Channel  outChannel, errChannel;
    outChannel = Tcl_GetStdChannel(TCL_STDOUT);
    promptCmd = Tcl_GetVar(Interp,(char*)
        (partial ? "tcl_prompt2" : "tcl_prompt1"), TCL_GLOBAL_ONLY);

    if (promptCmd == NULL) {
defaultPrompt:
      if (!partial && outChannel) {
        Tcl_Write(outChannel, "% ", 2);
      }
    } else {
      code = Tcl_Eval(Interp, promptCmd);
      outChannel = Tcl_GetStdChannel(TCL_STDOUT);
      errChannel = Tcl_GetStdChannel(TCL_STDERR);
      if (code != TCL_OK) {
        if (errChannel) {
#if ((TCL_MAJOR_VERSION > 8) || ((TCL_MAJOR_VERSION == 8) && (TCL_MINOR_VERSION >= 5)))
          Tcl_Write(errChannel, Tcl_GetStringResult(Interp), -1);
#else
          Tcl_Write(errChannel, Interp->result, -1);
#endif
          Tcl_Write(errChannel, "\n", 1);
        }
        Tcl_AddErrorInfo(Interp,
                         "\n    (script that generates prompt)");
        goto defaultPrompt;
      }
    }
    if (outChannel) {
      Tcl_Flush(outChannel);
    }
}

#if !defined(_WIN32) && !defined(__WIN32__)

#include <OSD_Timer.hxx>
#include <Draw_Window.hxx>
#include <unistd.h>

#if defined(__APPLE__) && !defined(MACOSX_USE_GLX)
  // use forward declaration for small subset of used Tk functions
  // to workaround broken standard Tk framework installation within OS X SDKs
  // which *HAS* X11 headers in Tk.framework but doesn't install them appropriately
  #define _TK
  typedef struct Tk_Window_* Tk_Window;
  typedef const char* Tk_Uid;

  extern "C" int Tk_Init (Tcl_Interp* interp);
  extern "C" void Tk_MainLoop();
  extern "C" Tk_Window Tk_MainWindow (Tcl_Interp* interp) ;
  extern "C" Tk_Uid Tk_GetUid (const char* str);
  extern "C" const char* Tk_SetAppName (Tk_Window tkwin, const char* name) ;
  extern "C" void Tk_GeometryRequest (Tk_Window tkwin, int reqWidth, int reqHeight);

#else
  #include <tk.h>
#endif

/*
 * Global variables used by the main program:
 */

char *tcl_RcFileName = NULL;    /* Name of a user-specific startup script
                                 * to source if the application is being run
                                 * interactively (e.g. "~/.wishrc").  Set
                                 * by Tcl_AppInit.  NULL means don't source
                                 * anything ever. */

static Tcl_DString command;     /* Used to assemble lines of terminal input
                                 * into Tcl commands. */
static Tcl_DString line;        /* Used to read the next line from the
                                 * terminal input. */
//static char errorExitCmd[] = "exit 1";

/*
 * Forward declarations for procedures defined later in this file:
 */

static void StdinProc (ClientData clientData, int mask);

static void Prompt (Tcl_Interp *Interp, int partial);

static Standard_Boolean tty;        /* Non-zero means standard input is a
                                 * terminal-like device.  Zero means it's
                                 * a file. */

Standard_Integer Draw_WindowScreen = 0;
Standard_Boolean Draw_BlackBackGround = Standard_True;


// Initialization of static variables of Draw_Window
//======================================================
Draw_Window* Draw_Window::firstWindow = NULL;

// X11 specific part
#if !defined(__APPLE__) || defined(MACOSX_USE_GLX)
#include <X11/Xutil.h>
#include <Aspect_DisplayConnection.hxx>

static unsigned long thePixels[MAXCOLOR];

Display* Draw_WindowDisplay = NULL;
Colormap Draw_WindowColorMap;
static Handle(Aspect_DisplayConnection) Draw_DisplayConnection;

// Base_Window struct definition
//===================================
struct Base_Window
{
  GC gc;
  XSetWindowAttributes xswa;
};

//=======================================================================
//function : Draw_Window
//purpose  :
//=======================================================================
Draw_Window::Draw_Window() :
       base(*new Base_Window()),
       win(0),
       myBuffer(0),
       next(firstWindow),
       previous(NULL),
       myUseBuffer(Standard_False),
       withWindowManager(Standard_True)
{
  myMother = RootWindow(Draw_WindowDisplay,
                        Draw_WindowScreen);

  if (firstWindow) firstWindow->previous = this;
  firstWindow = this;
}

//=======================================================================
//function : Draw_Window
//purpose  :
//=======================================================================
Draw_Window::Draw_Window(Window mother) :
       base(*new Base_Window()),
       win(0),
       myBuffer(0),
       next(firstWindow),
       previous(NULL),
       myUseBuffer(Standard_False),
       withWindowManager(Standard_True)
{
  myMother = mother;

  if (firstWindow) firstWindow->previous = this;
  firstWindow = this;
}

//=======================================================================
//function : Draw_Window
//purpose  :
//=======================================================================
Draw_Window::Draw_Window (const char* title,
                          Standard_Integer X, Standard_Integer Y,
                          Standard_Integer DX, Standard_Integer DY) :
       base(*new Base_Window()),
       win(0),
       myBuffer(0),
       next(firstWindow),
       previous(NULL),
       myUseBuffer(Standard_False),
       withWindowManager(Standard_True)
{
  myMother = RootWindow(Draw_WindowDisplay,
                        Draw_WindowScreen);

  if (firstWindow) firstWindow->previous = this;
  firstWindow = this;
  Init(X,Y,DX,DY);
  SetTitle(title);
}

//=======================================================================
//function : Draw_Window
//purpose  :
//=======================================================================
Draw_Window::Draw_Window (const char* window ) :
       base(*new Base_Window()),
       win(0),
       myBuffer(0),
       next(firstWindow),
       previous(NULL),
       myUseBuffer(Standard_False),
       withWindowManager(Standard_True)
{
  sscanf(window,"%lx",&win);
  Standard_Integer X,Y,DX,DY;

  if (firstWindow) firstWindow->previous = this;
  firstWindow = this;
  GetPosition(X,Y);
  DX=HeightWin();
  DY=WidthWin();

  Init(X,Y,DX,DY);
}

//=======================================================================
//function : Draw_Window
//purpose  :
//=======================================================================
Draw_Window::Draw_Window (Window mother,
                          char* title,
                          Standard_Integer X, Standard_Integer Y,
                          Standard_Integer DX, Standard_Integer DY) :
       base(*new Base_Window()),
       win(0),
       myBuffer(0),
       next(firstWindow),
       previous(NULL),
       myUseBuffer(Standard_False),
       withWindowManager(Standard_True)
{
  myMother = mother;

  if (firstWindow) firstWindow->previous = this;
  firstWindow = this;
  Init(X,Y,DX,DY);
  SetTitle(title);
}

//=======================================================================
//function : ~Draw_Window
//purpose  :
//=======================================================================
Draw_Window::~Draw_Window()
{
  if (previous)
    previous->next = next;
  else
    firstWindow = next;
  if (next)
    next->previous = previous;

  if (myBuffer != 0)
  {
    XFreePixmap(Draw_WindowDisplay, myBuffer);
    myBuffer = 0;
  }
  // Liberation pointer on Base_Window
  delete &base;
}

//=======================================================================
//function : Init
//purpose  :
//=======================================================================
void Draw_Window::Init(Standard_Integer X, Standard_Integer Y,
                       Standard_Integer DX, Standard_Integer DY)
{
  unsigned long setmask;

  if (Draw_BlackBackGround)
  {
    base.xswa.background_pixel = BlackPixel(Draw_WindowDisplay,Draw_WindowScreen);
    base.xswa.border_pixel     = WhitePixel(Draw_WindowDisplay,Draw_WindowScreen);
  }
  else
  {
    base.xswa.background_pixel = WhitePixel(Draw_WindowDisplay,Draw_WindowScreen);
    base.xswa.border_pixel     = BlackPixel(Draw_WindowDisplay,Draw_WindowScreen);
  }
  base.xswa.colormap         = Draw_WindowColorMap;
  setmask               = CWBackPixel | CWBorderPixel ;

  XSizeHints myHints;
  myHints.flags = USPosition;
  myHints.x = (int) X;
  myHints.y = (int) Y;

  if (win == 0)
  {
    win = XCreateWindow(Draw_WindowDisplay,
                        myMother,
                        (int) X,(int) Y,
                        (unsigned int) DX,(unsigned int) DY,
                        5,
                        DefaultDepth(Draw_WindowDisplay,Draw_WindowScreen),
                        InputOutput,
                        DefaultVisual(Draw_WindowDisplay,Draw_WindowScreen),
                        setmask,&base.xswa);
    XSelectInput(Draw_WindowDisplay, win, ButtonPressMask|ExposureMask|
                                          StructureNotifyMask);

    // advise to the window manager to place it where I need
    XSetWMNormalHints(Draw_WindowDisplay,win,&myHints);

    Atom aDeleteWindowAtom = Draw_DisplayConnection->GetAtom (Aspect_XA_DELETE_WINDOW);
    XSetWMProtocols (Draw_WindowDisplay, win, &aDeleteWindowAtom, 1);

    if (Draw_VirtualWindows)
    {
      myUseBuffer = Standard_True;
      InitBuffer();
    }
  }

  base.gc = XCreateGC(Draw_WindowDisplay, win, 0, NULL);

  XSetPlaneMask(Draw_WindowDisplay,base.gc,AllPlanes);
  XSetForeground(Draw_WindowDisplay,
                 base.gc, WhitePixel(Draw_WindowDisplay,Draw_WindowScreen));
  XSetBackground(Draw_WindowDisplay,
                 base.gc, BlackPixel(Draw_WindowDisplay,Draw_WindowScreen));
  // save in case of window recovery

  base.xswa.backing_store = Always;
  XChangeWindowAttributes(Draw_WindowDisplay, win,
                          CWBackingStore, &base.xswa);

  XSetLineAttributes (Draw_WindowDisplay, base.gc,
                      0, LineSolid, CapButt, JoinMiter);
}

//=======================================================================
//function : InitBuffer
//purpose  :
//=======================================================================
void Draw_Window::InitBuffer()
{
  if (myUseBuffer) {
    if (myBuffer != 0) {
      XFreePixmap (Draw_WindowDisplay, myBuffer);
    }
    XWindowAttributes winAttr;
    XGetWindowAttributes (Draw_WindowDisplay, win, &winAttr);
    myBuffer = XCreatePixmap (Draw_WindowDisplay, win, winAttr.width, winAttr.height, winAttr.depth);
  }
  else if (myBuffer != 0)
  {
    XFreePixmap (Draw_WindowDisplay, myBuffer);
    myBuffer = 0;
  }
}

//=======================================================================
//function : StopWinManager
//purpose  :
//=======================================================================
void Draw_Window::StopWinManager()
{
//  XGCValues winGc;
  XWindowAttributes winAttr;
  XGetWindowAttributes (Draw_WindowDisplay, win, &winAttr);
  Destroy();

  XSizeHints myHints;
  myHints.flags = USPosition;
  myHints.x = (int) 30;
  myHints.y = (int) 100;

  base.xswa.override_redirect = 1;
  base.xswa.border_pixel = BlackPixel(Draw_WindowDisplay,
                                 Draw_WindowScreen);
  base.xswa.background_pixel = WhitePixel(Draw_WindowDisplay,
                               Draw_WindowScreen);

  withWindowManager = Standard_False;

  win = XCreateWindow(Draw_WindowDisplay, myMother,
                      winAttr.x, winAttr.y,
                      winAttr.width, winAttr.height,
                      2,
                      CopyFromParent, InputOutput, CopyFromParent,
                      CWBorderPixel|CWOverrideRedirect|CWBackPixel, &base.xswa);


  // adwise to the window manager to place it where I wish
  XSetWMNormalHints(Draw_WindowDisplay,win,&myHints);

  // all masks of the old window are reassigned to the new one.
  XSelectInput(Draw_WindowDisplay,win,winAttr.your_event_mask);
}

//=======================================================================
//function : SetPosition
//purpose  :
//=======================================================================
void Draw_Window::SetPosition(Standard_Integer NewXpos,
                              Standard_Integer NewYpos)
{
  Standard_Integer x,y;
  GetPosition(x, y);

  if ( (x != NewXpos) || (y != NewYpos) )
    XMoveWindow(Draw_WindowDisplay, win, NewXpos, NewYpos);
}

//=======================================================================
//function : SetDimension
//purpose  :
//=======================================================================
void Draw_Window::SetDimension(Standard_Integer NewDx,
                               Standard_Integer NewDy)
{
  if ( (NewDx != WidthWin() ) || (NewDy != HeightWin() ) )
    XResizeWindow(Draw_WindowDisplay, win, NewDx, NewDy);
}

//=======================================================================
//function : GetPosition
//purpose  :
//=======================================================================
void Draw_Window::GetPosition(Standard_Integer &PosX,
                              Standard_Integer &PosY)
{
  XWindowAttributes winAttr;
  XGetWindowAttributes(Draw_WindowDisplay, win, &winAttr);

  PosX = winAttr.x;
  PosY = winAttr.y;
}

//=======================================================================
//function : HeightWin
//purpose  :
//=======================================================================
Standard_Integer Draw_Window::HeightWin() const
{
  Standard_Integer DY;
  XWindowAttributes winAttr;
  XGetWindowAttributes(Draw_WindowDisplay, win, &winAttr);

  DY = winAttr.height;
  return DY;
}

//=======================================================================
//function : WidthWin
//purpose  :
//=======================================================================
Standard_Integer Draw_Window::WidthWin() const
{
  Standard_Integer DX;
  XWindowAttributes winAttr;
  XGetWindowAttributes(Draw_WindowDisplay, win, &winAttr);

  DX = winAttr.width;
  return DX;
}

//=======================================================================
//function : SetTitle
//purpose  :
//=======================================================================
void Draw_Window::SetTitle(const TCollection_AsciiString& theTitle)
{
  XStoreName (Draw_WindowDisplay, win, theTitle.ToCString());
}

//=======================================================================
//function : GetTitle
//purpose  :
//=======================================================================
TCollection_AsciiString Draw_Window::GetTitle() const
{
  char* aTitle = NULL;
  XFetchName (Draw_WindowDisplay, win, &aTitle);
  return TCollection_AsciiString (aTitle);
}

//=======================================================================
//function : GetDrawable
//purpose  :
//=======================================================================
Drawable Draw_Window::GetDrawable() const
{
  return myUseBuffer ? myBuffer : win;
}

//=======================================================================
//function :DefineColor
//purpose  :
//=======================================================================
Standard_Boolean Draw_Window::DefineColor(const Standard_Integer i, const char* colorName)
{
  XColor color;

  if (!XParseColor(Draw_WindowDisplay,Draw_WindowColorMap,colorName,&color))
    return Standard_False;
  if (!XAllocColor(Draw_WindowDisplay,Draw_WindowColorMap,&color))
    return Standard_False;
  thePixels[i % MAXCOLOR] = color.pixel;
  return Standard_True;
}

//=======================================================================
//function : IsMapped
//purpose  :
//=======================================================================
bool Draw_Window::IsMapped() const
{
  if (Draw_VirtualWindows
   || win == 0)
  {
    return false;
  }

  XFlush (Draw_WindowDisplay);
  XWindowAttributes aWinAttr;
  XGetWindowAttributes (Draw_WindowDisplay, win, &aWinAttr);
  return aWinAttr.map_state == IsUnviewable
      || aWinAttr.map_state == IsViewable;
}

//=======================================================================
//function : DisplayWindow
//purpose  :
//=======================================================================
void Draw_Window::DisplayWindow()
{
  if (Draw_VirtualWindows)
  {
    return;
  }
  else
  {
    XMapRaised(Draw_WindowDisplay, win);
  }
  XFlush(Draw_WindowDisplay);
}

//=======================================================================
//function : Hide
//purpose  :
//=======================================================================
void Draw_Window::Hide()
{
   XUnmapWindow(Draw_WindowDisplay, win);
}

//=======================================================================
//function : Destroy
//purpose  :
//=======================================================================
void Draw_Window::Destroy()
{
  XFreeGC (Draw_WindowDisplay, base.gc);
  XDestroyWindow(Draw_WindowDisplay, win);
  win = 0;
  if (myBuffer != 0)
  {
    XFreePixmap(Draw_WindowDisplay, myBuffer);
    myBuffer = 0;
  }
}

//=======================================================================
//function : Clear
//purpose  :
//=======================================================================
void Draw_Window::Clear()
{
  if (myUseBuffer)
  {
    // XClearArea only applicable for windows
    XGCValues currValues;
    XGetGCValues(Draw_WindowDisplay, base.gc, GCBackground | GCForeground, &currValues);
    XSetForeground(Draw_WindowDisplay, base.gc, currValues.background);
    XFillRectangle(Draw_WindowDisplay, myBuffer, base.gc, 0, 0, WidthWin(), HeightWin());
    XSetForeground(Draw_WindowDisplay, base.gc, currValues.foreground);
  }
  else
  {
    XClearArea(Draw_WindowDisplay, win, 0, 0, 0, 0, False);
  }
}

//=======================================================================
//function : Flush
//purpose  :
//=======================================================================
void Draw_Window::Flush()
{
  XFlush(Draw_WindowDisplay);
}

//=======================================================================
//function : DrawString
//purpose  :
//=======================================================================
void Draw_Window::DrawString(int X, int Y, char *text)
{
  XDrawString(Draw_WindowDisplay, GetDrawable(), base.gc, X, Y, text, strlen(text));
}

//=======================================================================
//function : DrawSegments
//purpose  :
//=======================================================================
void Draw_Window::DrawSegments(Segment *tab, int nbElem)
{
  XDrawSegments(Draw_WindowDisplay, GetDrawable(), base.gc, (XSegment*) tab, nbElem);
}

//=======================================================================
//function : Redraw
//purpose  :
//=======================================================================
void Draw_Window::Redraw()
{
  if (myUseBuffer) {
    XCopyArea (Draw_WindowDisplay,
               myBuffer, win, // source, destination Drawables
               base.gc,
               0, 0,  // source x, y
               WidthWin(), HeightWin(),
               0, 0); // destination x, y
  }
}

//=======================================================================
//function : SetColor
//purpose  :
//=======================================================================
void Draw_Window::SetColor(Standard_Integer color)
{
  XSetForeground(Draw_WindowDisplay, base.gc, thePixels[color]);
}

//=======================================================================
//function : SetMode
//purpose  :
//=======================================================================
void Draw_Window::SetMode( int mode)
{
  XSetFunction(Draw_WindowDisplay, base.gc, mode);
}

//=======================================================================
//function : Save
//purpose  :
//=======================================================================
Standard_Boolean Draw_Window::Save (const char* theFileName) const
{
  // make sure all draw operations done
  XSync (Draw_WindowDisplay, True);

  // the attributes
  XWindowAttributes winAttr;
  XGetWindowAttributes (Draw_WindowDisplay, win, &winAttr);

  if (!myUseBuffer)
  {
    // make sure that the whole window fit on display to prevent BadMatch error
    XWindowAttributes winAttrRoot;
    XGetWindowAttributes (Draw_WindowDisplay, XRootWindowOfScreen (winAttr.screen), &winAttrRoot);

    Window winChildDummy;
    int winLeft = 0;
    int winTop = 0;
    XTranslateCoordinates (Draw_WindowDisplay, win, XRootWindowOfScreen (winAttr.screen),
                           0, 0, &winLeft, &winTop, &winChildDummy);

    if (((winLeft + winAttr.width) > winAttrRoot.width)  || winLeft < winAttrRoot.x ||
        ((winTop + winAttr.height) > winAttrRoot.height) || winTop  < winAttrRoot.y)
    {
      std::cerr << "The window not fully visible! Can't create the snapshot.\n";
      return Standard_False;
    }
  }

  XVisualInfo aVInfo;
  if (XMatchVisualInfo (Draw_WindowDisplay, Draw_WindowScreen, 32, TrueColor, &aVInfo) == 0
   && XMatchVisualInfo (Draw_WindowDisplay, Draw_WindowScreen, 24, TrueColor, &aVInfo) == 0)
  {
    std::cerr << "24-bit TrueColor visual is not supported by server!\n";
    return Standard_False;
  }

  Image_AlienPixMap anImage;
  bool isBigEndian = Image_PixMap::IsBigEndianHost();
  const Standard_Size aSizeRowBytes = Standard_Size(winAttr.width) * 4;
  if (!anImage.InitTrash (isBigEndian ? Image_Format_RGB32 : Image_Format_BGR32,
                          Standard_Size(winAttr.width), Standard_Size(winAttr.height), aSizeRowBytes))
  {
    return Standard_False;
  }
  anImage.SetTopDown (true);

  XImage* anXImage = XCreateImage (Draw_WindowDisplay, aVInfo.visual,
                                   32, ZPixmap, 0, (char* )anImage.ChangeData(), winAttr.width, winAttr.height, 32, int(aSizeRowBytes));
  anXImage->bitmap_bit_order = anXImage->byte_order = (isBigEndian ? MSBFirst : LSBFirst);
  if (XGetSubImage (Draw_WindowDisplay, GetDrawable(),
                    0, 0, winAttr.width, winAttr.height,
                    AllPlanes, ZPixmap, anXImage, 0, 0) == NULL)
  {
    anXImage->data = NULL;
    XDestroyImage (anXImage);
    return Standard_False;
  }

  // destroy the image
  anXImage->data = NULL;
  XDestroyImage (anXImage);

  // save the image
  return anImage.Save (theFileName);
}

//=======================================================================
//function : Wait
//purpose  :
//=======================================================================

void Draw_Window::Wait (Standard_Boolean wait)
{
  Flush();
  if (!wait) {
    XSelectInput(Draw_WindowDisplay,win,
                 ButtonPressMask|ExposureMask | StructureNotifyMask |
                 PointerMotionMask);
  }
  else {
    XSelectInput(Draw_WindowDisplay,win,
                 ButtonPressMask|ExposureMask | StructureNotifyMask);
  }
}

//=======================================================================
//function : ProcessEvent
//purpose  :
//=======================================================================

void ProcessEvent(Draw_Window& win, XEvent& xev)
{
  Standard_Integer X,Y,button;
  KeySym keysym;
  XComposeStatus stat;
  char chainekey[10];

  switch (xev.type)
  {
  case ClientMessage:
  {
    if (xev.xclient.data.l[0] == (int )Draw_DisplayConnection->GetAtom (Aspect_XA_DELETE_WINDOW))
    {
      // just hide the window
      win.Hide();
    }
    return;
  }
  case Expose :
    win.WExpose();
    break;

  case ButtonPress :
    X = xev.xbutton.x;
    Y = xev.xbutton.y;
    button = xev.xbutton.button;
    win.WButtonPress(X,Y,button);
    break;

  case ButtonRelease :
    X = xev.xbutton.x;
    Y = xev.xbutton.y;
    button = xev.xbutton.button;
    win.WButtonRelease(X,Y,button);
    break;

  case KeyPress :
    XLookupString(&(xev.xkey),
                         chainekey,
                         10,
                         &keysym,
                         &stat);
    break;

  case MotionNotify :
    X = xev.xmotion.x;
    Y = xev.xmotion.y;
    win.WMotionNotify(X,Y);
    break;

  case ConfigureNotify :
    if (win.withWindowManager)
      win.WConfigureNotify(xev.xconfigure.x, xev.xconfigure.y,
                           xev.xconfigure.width,
                           xev.xconfigure.height);
    break;

  case UnmapNotify :

    win.WUnmapNotify();
    break;
  }
}

//=======================================================================
//function : WExpose
//purpose  :
//=======================================================================
void Draw_Window::WExpose()
{
}

//=======================================================================
//function : WButtonPress
//purpose  :
//=======================================================================
void Draw_Window::WButtonPress(const Standard_Integer,
                               const Standard_Integer,
                               const Standard_Integer&)
{
}

//=======================================================================
//function : WButtonRelease
//purpose  :
//=======================================================================
void Draw_Window::WButtonRelease(const Standard_Integer,
                                 const Standard_Integer,
                                 const Standard_Integer&)
{
}

/**************************
//=======================================================================
//function : WKeyPress
//purpose  :
//=======================================================================

void Draw_Window::WKeyPress(char, KeySym&)
{
}
***************************/

//=======================================================================
//function : WMotionNotify
//purpose  :
//=======================================================================
void Draw_Window::WMotionNotify(const Standard_Integer ,
                                const Standard_Integer )
{
}

//=======================================================================
//function : WConfigureNotify
//purpose  :
//=======================================================================

void Draw_Window::WConfigureNotify(const Standard_Integer,
                                   const Standard_Integer,
                                   const Standard_Integer,
                                   const Standard_Integer)
{
}

//=======================================================================
//function : WUnmapNotify
//purpose  :
//=======================================================================

void Draw_Window::WUnmapNotify()
{
}


//======================================================
// funtion : ProcessEvents
// purpose : process pending X events
//======================================================

static void ProcessEvents(ClientData,int)
{
  // test for X Event

  while (XPending(Draw_WindowDisplay)) {

    XEvent xev;
    xev.type = 0;

    XNextEvent(Draw_WindowDisplay,&xev);

    /* search the window in the window list */
    Draw_Window* w = Draw_Window::firstWindow;
    Standard_Integer found=0;
    while (w) {
      if (xev.xany.window == w->win) {
        ProcessEvent(*w, xev);
        found=1;
        break;
      }
      w = w->next;
    }
    if (found==0) {
      Tk_HandleEvent(&xev);
    }
  }
}

//======================================================
// funtion : GetNextEvent()
// purpose :
//======================================================
void GetNextEvent(Event& ev)
{
  XEvent xev;
  XNextEvent(Draw_WindowDisplay, &xev);
  switch(xev.type)
  {
    case ButtonPress :
      ev.type = 4;
      ev.window = xev.xbutton.window;
      ev.button = xev.xbutton.button;
      ev.x = xev.xbutton.x;
      ev.y = xev.xbutton.y;
      break;

    case MotionNotify :
      ev.type = 6;
      ev.window = xev.xmotion.window;
      ev.button = 0;
      ev.x = xev.xmotion.x;
      ev.y = xev.xmotion.y;
      break;
  }
}
#endif //__APPLE__

//======================================================
// funtion :Run_Appli
// purpose :
//======================================================


static Standard_Boolean(*Interprete) (const char*);

void Run_Appli(Standard_Boolean (*interprete) (const char*))
{
  Tcl_Channel outChannel, inChannel ;
  Interprete = interprete;

#ifdef _TK

    /*
     * Commands will come from standard input, so set up an event
     * handler for standard input.  If the input device is aEvaluate the
     * .rc file, if one has been specified, set up an event handler
     * for standard input, and print a prompt if the input
     * device is a terminal.
     */
  inChannel = Tcl_GetStdChannel(TCL_STDIN);
  if (inChannel) {
            Tcl_CreateChannelHandler(inChannel, TCL_READABLE, StdinProc,
                    (ClientData) inChannel);
        }

  // Create a handler for the draw display

  // Adding of the casting into void* to be able to compile on AO1
  // ConnectionNumber(Draw_WindowDisplay) is an int 32 bits
  //                    (void*) is a pointer      64 bits ???????

#if !defined(__APPLE__) || defined(MACOSX_USE_GLX)
#if TCL_MAJOR_VERSION  < 8
    Tk_CreateFileHandler((void*) ConnectionNumber(Draw_WindowDisplay),
                         TK_READABLE, ProcessEvents,(ClientData) 0 );
#else
    Tk_CreateFileHandler(ConnectionNumber(Draw_WindowDisplay),
                         TK_READABLE, ProcessEvents,(ClientData) 0 );
#endif
#endif // __APPLE__

#endif

  Draw_Interpretor& aCommands = Draw::GetInterpretor();

  if (tty) Prompt(aCommands.Interp(), 0);
  Prompt(aCommands.Interp(), 0);

  outChannel = Tcl_GetStdChannel(TCL_STDOUT);
  if (outChannel) {
        Tcl_Flush(outChannel);
    }
  Tcl_DStringInit(&command);

  /*
   * Loop infinitely, waiting for commands to execute.  When there
   * are no windows left, Tk_MainLoop returns and we exit.
   */

#ifdef _TK

  if (Draw_VirtualWindows) {
    // main window will never shown
    // but main loop will parse all Xlib messages
    Tcl_Eval(aCommands.Interp(), "wm withdraw .");
  }
  Tk_MainLoop();

#else

  fd_set readset;
  Standard_Integer count = ConnectionNumber(Draw_WindowDisplay);
  Standard_Integer numfd;
  while (1) {
      FD_ZERO(&readset);
      FD_SET(0,&readset);
      FD_SET(count,&readset);
#ifdef HPUX
      numfd = select(count+1,(Integer*)&readset,NULL,NULL,NULL);
#else
      numfd = select(count+1,&readset,NULL,NULL,NULL);
#endif
      if (FD_ISSET(0,&readset))     StdinProc((ClientData)0,0);
      if (FD_ISSET(count,&readset)) ProcessEvents((ClientData)0,0);
    }

#endif
  NCollection_List<Draw_Window::FCallbackBeforeTerminate>::Iterator Iter(MyCallbacks);
  for(; Iter.More(); Iter.Next())
  {
      (*Iter.Value())();
  }
}

//======================================================
// funtion : Init_Appli()
// purpose :
//======================================================
Standard_Boolean Init_Appli()
{
  Draw_Interpretor& aCommands = Draw::GetInterpretor();
  aCommands.Init();
  interp = aCommands.Interp();

  Tcl_Init(interp) ;
  try {
    OCC_CATCH_SIGNALS
    Tk_Init(interp) ;
  } catch  (Standard_Failure) {
    cout <<" Pb au lancement de TK_Init "<<endl;
  }

  Tcl_StaticPackage(interp, "Tk", Tk_Init, (Tcl_PackageInitProc *) NULL);

  Tk_Window aMainWindow = Tk_MainWindow(interp) ;
  if (aMainWindow == NULL) {
#if ((TCL_MAJOR_VERSION > 8) || ((TCL_MAJOR_VERSION == 8) && (TCL_MINOR_VERSION >= 5)))
    fprintf(stderr, "%s\n", Tcl_GetStringResult(interp));
#else
    fprintf(stderr, "%s\n", interp->result);
#endif
    exit(1);
  }
#if defined(__APPLE__) && !defined(MACOSX_USE_GLX)
  Tk_SetAppName(aMainWindow, "Draw");
#else
  Tk_Name(aMainWindow) = Tk_GetUid(Tk_SetAppName(aMainWindow, "Draw"));
#endif

  Tk_GeometryRequest (aMainWindow, 200, 200);

#if !defined(__APPLE__) || defined(MACOSX_USE_GLX)
  if (Draw_DisplayConnection.IsNull())
  {
    try
    {
      Draw_DisplayConnection = new Aspect_DisplayConnection();
    }
    catch (Standard_Failure)
    {
      std::cout << "Cannot open display. Interpret commands in batch mode." << std::endl;
      return Standard_False;
    }
  }
  if (Draw_WindowDisplay == NULL)
  {
    Draw_WindowDisplay = Draw_DisplayConnection->GetDisplay();
  }
  //
  // synchronize the display server : could be done within Tk_Init
  //
  XSynchronize(Draw_WindowDisplay, True);
  XSetInputFocus(Draw_WindowDisplay,
                 PointerRoot,
                 RevertToPointerRoot,
                 CurrentTime);

  Draw_WindowScreen   = DefaultScreen(Draw_WindowDisplay);
  Draw_WindowColorMap = DefaultColormap(Draw_WindowDisplay,
                                        Draw_WindowScreen);
#endif // __APPLE__

  tty = isatty(0);
  Tcl_SetVar(interp,"tcl_interactive",(char*)(tty ? "1" : "0"), TCL_GLOBAL_ONLY);
//  Tcl_SetVar(interp,"tcl_interactive",tty ? "1" : "0", TCL_GLOBAL_ONLY);
  return Standard_True;
}

//======================================================
// funtion : Destroy_Appli()
// purpose :
//======================================================
void Destroy_Appli()
{
  //XCloseDisplay(Draw_WindowDisplay);
}

/*
 *----------------------------------------------------------------------
 *
 * StdinProc --
 *
 *        This procedure is invoked by the event dispatcher whenever
 *        standard input becomes readable.  It grabs the next line of
 *        input characters, adds them to a command being assembled, and
 *        executes the command if it's complete.
 *
 * Results:
 *        None.
 *
 * Side effects:
 *        Could be almost arbitrary, depending on the command that's
 *        typed.
 *
 *----------------------------------------------------------------------
 */

    /* ARGSUSED */
//static void StdinProc(ClientData clientData, int mask)
static void StdinProc(ClientData clientData, int )
{
  static int gotPartial = 0;
  char *cmd;
//  int code, count;
  int count;
  Tcl_Channel chan = (Tcl_Channel) clientData;

  // MSV Nov 2, 2001: patch for TCL 8.3: initialize line to avoid exception
  //                  when first user input is an empty string
  Tcl_DStringFree(&line);
  count = Tcl_Gets(chan, &line);

  // MKV 26.05.05
#if ((TCL_MAJOR_VERSION > 8) || ((TCL_MAJOR_VERSION == 8) && (TCL_MINOR_VERSION >= 4)))
  Tcl_DString linetmp;
  Tcl_DStringInit(&linetmp);
  Tcl_UniChar * UniCharString;
  UniCharString = Tcl_UtfToUniCharDString(Tcl_DStringValue(&line),-1,&linetmp);
  Standard_Integer l = Tcl_UniCharLen(UniCharString);
  TCollection_AsciiString AsciiString("");
  Standard_Character Character;
  Standard_Integer i;
  for (i=0; i<l; i++) {
    Character = UniCharString[i];
    AsciiString.AssignCat(Character);
  }
  Tcl_DStringInit(&line);
  Tcl_DStringAppend(&line, AsciiString.ToCString(), -1);
#endif
  if (count < 0) {
    if (!gotPartial) {
      if (tty) {
        Tcl_Exit(0);
      } else {
        Tcl_DeleteChannelHandler(chan, StdinProc, (ClientData) chan);
      }
      return;
    } else {
      count = 0;
    }
  }

  (void) Tcl_DStringAppend(&command, Tcl_DStringValue(&line), -1);
  cmd = Tcl_DStringAppend(&command, "\n", -1);
  Tcl_DStringFree(&line);
  try {
    OCC_CATCH_SIGNALS
  if (!Tcl_CommandComplete(cmd)) {
    gotPartial = 1;
    goto prompt;
  }
  gotPartial = 0;

  /*
   * Disable the stdin channel handler while evaluating the command;
   * otherwise if the command re-enters the event loop we might
   * process commands from stdin before the current command is
   * finished.  Among other things, this will trash the text of the
   * command being evaluated.
   */

  Tcl_CreateChannelHandler(chan, 0, StdinProc, (ClientData) chan);


  /*
   * Disable the stdin file handler while evaluating the command;
   * otherwise if the command re-enters the event loop we might
   * process commands from stdin before the current command is
   * finished.  Among other things, this will trash the text of the
   * command being evaluated.
   */

#ifdef _TK
   //  Tk_CreateFileHandler(0, 0, StdinProc, (ClientData) 0);
#endif
    //
    // xab average to avoid an output SIGBUS of DRAW
    // to ultimately prescise or remove once
    // the problem of free on the global variable at the average
    //
    //

  Interprete(cmd);


  Tcl_CreateChannelHandler(chan, TCL_READABLE, StdinProc,
                           (ClientData) chan);
  Tcl_DStringFree(&command);

  /*
   * Output a prompt.
   */

prompt:
  if (tty) Prompt(interp, gotPartial);

 } catch (Standard_Failure) {}

}

#else

// Source Specifique WNT

/****************************************************\
*  Draw_Window.cxx :
*
\****************************************************/

#include "Draw_Window.hxx"
#include "DrawRessource.h"
#include "init.h"

#include <Draw_Appli.hxx>
#include <OSD.hxx>

#include <tk.h>

#define PENWIDTH 1
#define CLIENTWND 0
// Position of information in the extra memory

// indicates SUBSYSTEM:CONSOLE linker option, to be set to True in main()
Standard_EXPORT
Standard_Boolean Draw_IsConsoleSubsystem = Standard_False;


Standard_Boolean Draw_BlackBackGround = Standard_True;

// Creation of color stylos
HPEN colorPenTab[MAXCOLOR] = {CreatePen(PS_SOLID, PENWIDTH, RGB(255,255,255)),
                              CreatePen(PS_SOLID, PENWIDTH, RGB(255,0,0)),
                              CreatePen(PS_SOLID, PENWIDTH, RGB(0,255,0)),
                              CreatePen(PS_SOLID, PENWIDTH, RGB(0,0,255)),
                              CreatePen(PS_SOLID, PENWIDTH, RGB(0,255,255)),
                              CreatePen(PS_SOLID, PENWIDTH, RGB(255,215,0)),
                              CreatePen(PS_SOLID, PENWIDTH, RGB(255,0,255)),
                              CreatePen(PS_SOLID, PENWIDTH, RGB(255,52,179)),
                              CreatePen(PS_SOLID, PENWIDTH, RGB(255,165,0)),
                              CreatePen(PS_SOLID, PENWIDTH, RGB(255,228,225)),
                              CreatePen(PS_SOLID, PENWIDTH, RGB(255,160,122)),
                              CreatePen(PS_SOLID, PENWIDTH, RGB(199,21,133)),
                              CreatePen(PS_SOLID, PENWIDTH, RGB(255,255,0)),
                              CreatePen(PS_SOLID, PENWIDTH, RGB(240,230,140)),
                              CreatePen(PS_SOLID, PENWIDTH, RGB(255,127,80))};

// Correspondance mode X11 and WINDOWS NT
int modeTab[16] = {R2_BLACK, R2_MASKPEN, R2_MASKPENNOT, R2_COPYPEN,
                   R2_MASKNOTPEN, R2_NOP, R2_XORPEN, R2_MERGEPEN,
                   R2_NOTMASKPEN, R2_NOTXORPEN, R2_NOT, R2_MERGEPENNOT,
                   R2_NOTCOPYPEN, R2_MERGENOTPEN, R2_NOTMERGEPEN, R2_WHITE};

/*--------------------------------------------------------*\
|  CREATE DRAW WINDOW PROCEDURE
\*--------------------------------------------------------*/
HWND DrawWindow::CreateDrawWindow(HWND hWndClient, int nitem)
{
  if (Draw_IsConsoleSubsystem) {
    HWND aWin = CreateWindowW (DRAWCLASS, DRAWTITLE,
                              WS_OVERLAPPEDWINDOW,
                              1,1,1,1,
                              NULL, NULL,::GetModuleHandle(NULL), NULL);
    if (!Draw_VirtualWindows)
    {
      SetWindowPos(aWin, HWND_TOPMOST, 1,1,1,1, SWP_NOMOVE);
      SetWindowPos(aWin, HWND_NOTOPMOST, 1,1,1,1, SWP_NOMOVE);
    }
    return aWin;
  }
  else {
    HANDLE hInstance = (HANDLE )GetWindowLongPtrW (hWndClient, GWLP_HINSTANCE);

    return CreateMDIWindowW(DRAWCLASS, DRAWTITLE,
                           WS_CAPTION | WS_CHILD | WS_THICKFRAME,
                           1,1,0,0,
                           hWndClient, (HINSTANCE)hInstance, nitem);
  }
}


/*--------------------------------------------------------*\
|  DRAW WINDOW PROCEDURE
\*--------------------------------------------------------*/
LRESULT APIENTRY DrawWindow::DrawProc(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam )
{
  DrawWindow* localObjet = (DrawWindow* )GetWindowLongPtrW (hWnd, CLIENTWND);
  if (!localObjet)
  {
    return Draw_IsConsoleSubsystem
         ? DefWindowProcW   (hWnd, wMsg, wParam, lParam)
         : DefMDIChildProcW (hWnd, wMsg, wParam, lParam);
  }

  switch (wMsg)
  {
    case WM_CLOSE:
    {
      localObjet->Hide();
      return 0; // do nothing - window destruction should be performed by application
    }
    case WM_PAINT:
    {
      PAINTSTRUCT ps;
      BeginPaint (hWnd, &ps);
      if (localObjet->GetUseBuffer())
      {
        localObjet->Redraw();
      }
      else
      {
        localObjet->WExpose();
      }
      EndPaint (hWnd, &ps);
      return 0;
    }
    case WM_SIZE:
    {
      if (localObjet->GetUseBuffer())
      {
        localObjet->InitBuffer();
        localObjet->WExpose();
        localObjet->Redraw();
        return 0;
      }
      break;
    }
  }
  return Draw_IsConsoleSubsystem
       ? DefWindowProcW   (hWnd, wMsg, wParam, lParam)
       : DefMDIChildProcW (hWnd, wMsg, wParam, lParam);
}



/*
**  IMPLEMENTATION of the CLASS DRAWWINDOW
 */

/*--------------------------------------------------------*\
| Initialization of static variables of DrawWindow
\*--------------------------------------------------------*/

DrawWindow* DrawWindow::firstWindow = NULL;
HWND DrawWindow::hWndClientMDI = 0;

/*--------------------------------------------------------*\
| Constructors of Draw_Window
\*--------------------------------------------------------*/

// Default Constructor
//________________________
DrawWindow::DrawWindow() :
        win(0),
        next(firstWindow),
        previous(NULL),
        myMemHbm(NULL),
        myUseBuffer(Standard_False)
{
  if (firstWindow) firstWindow->previous = this;
  firstWindow = this;
}

//________________________
DrawWindow::DrawWindow(const char* title,
                       Standard_Integer X, Standard_Integer Y,
                       Standard_Integer dX,Standard_Integer dY) :
       win(0),        next(firstWindow), previous(NULL), myMemHbm(NULL), myUseBuffer(Standard_False)
{
  if (firstWindow) firstWindow->previous = this;
  firstWindow = this;
  Init(X, Y, dX, dY);
  SetTitle(title);
}
DrawWindow::DrawWindow(const char* title,
                       Standard_Integer X, Standard_Integer Y,
                       Standard_Integer dX,Standard_Integer dY,
                       HWND theWin) :
       win(theWin),next(firstWindow), previous(NULL), myMemHbm(NULL), myUseBuffer(Standard_False)
{
  if (firstWindow) firstWindow->previous = this;
  firstWindow = this;
  Init(X, Y, dX, dY);
  SetTitle(title);
}



/*--------------------------------------------------------*\
| Destructor of DrawWindow
\*--------------------------------------------------------*/
DrawWindow::~DrawWindow()
{
  if (previous)
    previous->next = next;
  else
    firstWindow = next;
  if (next)
    next->previous = previous;

  // Delete 'off-screen drawing'-related objects
  if (myMemHbm) {
    DeleteObject(myMemHbm);
    myMemHbm = NULL;
  }
}



/*--------------------------------------------------------*\
|  Init
\*--------------------------------------------------------*/
void DrawWindow::Init(Standard_Integer theXLeft, Standard_Integer theYTop,
                      Standard_Integer theWidth, Standard_Integer theHeight)
{
  if (win == NULL)
  {
    win = CreateDrawWindow(hWndClientMDI, 0);
  }

  // include decorations in the window dimensions
  // to reproduce same behaviour of Xlib window.
  DWORD aWinStyle   = GetWindowLongW (win, GWL_STYLE);
  DWORD aWinStyleEx = GetWindowLongW (win, GWL_EXSTYLE);
  HMENU aMenu       = GetMenu (win);

  RECT aRect;
  aRect.top    = theYTop;
  aRect.bottom = theYTop + theHeight;
  aRect.left   = theXLeft;
  aRect.right  = theXLeft + theWidth;
  AdjustWindowRectEx (&aRect, aWinStyle, aMenu != NULL ? TRUE : FALSE, aWinStyleEx);

  SetPosition  (aRect.left, aRect.top);
  SetDimension (aRect.right - aRect.left, aRect.bottom - aRect.top);
  // Save the pointer at the instance associated to the window
  SetWindowLongPtrW (win, CLIENTWND, (LONG_PTR)this);
  HDC hDC = GetDC(win);
  SetBkColor(hDC, RGB(0, 0, 0));
  myCurrPen  = 3;
  myCurrMode = 3;
  SelectObject(hDC, colorPenTab[myCurrPen]); // Default pencil
  SelectObject(hDC, GetStockObject(BLACK_BRUSH));
  SetTextColor(hDC, RGB(0,0,255));
  ReleaseDC(win, hDC);

  if (Draw_VirtualWindows)
  {
    // create a virtual window
    SetUseBuffer (Standard_True);
  }
}

/*--------------------------------------------------------*\
|  SetUseBuffer
\*--------------------------------------------------------*/
void DrawWindow::SetUseBuffer(Standard_Boolean use)
{
  myUseBuffer = use;
  InitBuffer();
}

/*--------------------------------------------------------*\
|  InitBuffer
\*--------------------------------------------------------*/
void DrawWindow::InitBuffer()
{
  if (myUseBuffer) {
    RECT rc;
    HDC hDC = GetDC(win);
    GetClientRect(win, &rc);
    if (myMemHbm) {
      BITMAP aBmp;
      GetObjectW (myMemHbm, sizeof(BITMAP), &aBmp);
      if (rc.right-rc.left == aBmp.bmWidth && rc.bottom-rc.top == aBmp.bmHeight) return;
      DeleteObject(myMemHbm);
    }
    myMemHbm = (HBITMAP)CreateCompatibleBitmap(hDC,
                                      rc.right-rc.left,
                                      rc.bottom-rc.top);
    HDC aMemDC      = GetMemDC(hDC);
    FillRect(aMemDC, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));
    ReleaseMemDC(aMemDC);
    ReleaseDC(win, hDC);
  }
  else {
    if (myMemHbm) {
      DeleteObject(myMemHbm);
      myMemHbm = NULL;
    }
  }
}

/*--------------------------------------------------------*\
|  GetMemDC
\*--------------------------------------------------------*/
HDC DrawWindow::GetMemDC(HDC theWinDC)
{
  if (!myUseBuffer) return NULL;

  HDC aWorkDC = CreateCompatibleDC(theWinDC);
  myOldHbm = (HBITMAP)SelectObject(aWorkDC, myMemHbm);
  SetROP2(aWorkDC, modeTab[myCurrMode]);
  SelectObject(aWorkDC, colorPenTab[myCurrPen]);
  SetBkColor(aWorkDC, RGB(0, 0, 0));
  SelectObject(aWorkDC, GetStockObject(BLACK_BRUSH));
  SetTextColor(aWorkDC, RGB(0,0,255));
  return aWorkDC;
}


/*--------------------------------------------------------*\
|  ReleaseMemDC
\*--------------------------------------------------------*/
void DrawWindow::ReleaseMemDC(HDC theMemDC)
{
  if (!myUseBuffer || !theMemDC) return;

  if (myOldHbm) SelectObject(theMemDC, myOldHbm);
  DeleteDC(theMemDC);
}


/*--------------------------------------------------------*\
|  SetPosition
\*--------------------------------------------------------*/
void DrawWindow::SetPosition(Standard_Integer posX, Standard_Integer posY)
{
  SetWindowPos(win, 0,
               posX, posY,
               0, 0,
               SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
}


/*--------------------------------------------------------*\
|  SetDimension
\*--------------------------------------------------------*/
void DrawWindow::SetDimension(Standard_Integer dimX, Standard_Integer dimY)
{
  SetWindowPos(win, 0,
               0, 0,
               dimX, dimY,
               SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
}


/*--------------------------------------------------------*\
|  GetPosition
\*--------------------------------------------------------*/
void DrawWindow::GetPosition(Standard_Integer &dimX,
                             Standard_Integer &dimY)
{
  RECT rect;
  GetWindowRect(win, &rect);

  POINT point;
  point.x = rect.left;
  point.y = rect.top;

  ScreenToClient(hWndClientMDI, &point);
  dimX = point.x;
  dimY = point.y;
}


/*--------------------------------------------------------*\
|  HeightWin
\*--------------------------------------------------------*/
Standard_Integer DrawWindow::HeightWin() const
{
  RECT rect;
  GetClientRect(win, &rect);
  return(rect.bottom-rect.top);
}


/*--------------------------------------------------------*\
|  WidthWin
\*--------------------------------------------------------*/
Standard_Integer DrawWindow::WidthWin() const
{
  RECT rect;
  GetClientRect(win, &rect);
  return(rect.right-rect.left);
}


/*--------------------------------------------------------*\
|  SetTitle
\*--------------------------------------------------------*/
void DrawWindow::SetTitle (const TCollection_AsciiString& theTitle)
{
  const TCollection_ExtendedString aTitleW (theTitle);
  SetWindowTextW (win, aTitleW.ToWideString());
}


/*--------------------------------------------------------*\
|  GetTitle
\*--------------------------------------------------------*/
TCollection_AsciiString DrawWindow::GetTitle() const
{
  wchar_t aTitleW[32];
  GetWindowTextW (win, aTitleW, 30);
  return TCollection_AsciiString (aTitleW);
}

//=======================================================================
//function : IsMapped
//purpose  :
//=======================================================================
bool Draw_Window::IsMapped() const
{
  if (Draw_VirtualWindows
   || win == NULL)
  {
    return false;
  }

  LONG aWinStyle = GetWindowLongW (win, GWL_STYLE);
  return (aWinStyle & WS_VISIBLE)  != 0
      && (aWinStyle & WS_MINIMIZE) == 0;
}

/*--------------------------------------------------------*\
|  DisplayWindow
\*--------------------------------------------------------*/
void DrawWindow::DisplayWindow()
{
  if (Draw_VirtualWindows)
  {
    return;
  }
  ShowWindow (win, SW_SHOW);
  UpdateWindow (win);
}


/*--------------------------------------------------------*\
|  Hide
\*--------------------------------------------------------*/
void DrawWindow::Hide()
{
  ShowWindow(win, SW_HIDE);
}


/*--------------------------------------------------------*\
|  Destroy
\*--------------------------------------------------------*/
void DrawWindow::Destroy()
{
  DestroyWindow(win);
}



/*--------------------------------------------------------*\
|  Clear
\*--------------------------------------------------------*/
void DrawWindow::Clear()
{
  HDC hDC = GetDC(win);
  HDC aWorkDC = myUseBuffer ? GetMemDC(hDC) : hDC;

  SaveDC(aWorkDC);
  SelectObject(aWorkDC,GetStockObject(BLACK_PEN));
  Rectangle(aWorkDC, 0, 0, WidthWin(), HeightWin());
  RestoreDC(aWorkDC,-1);

  if (myUseBuffer) ReleaseMemDC(aWorkDC);
  ReleaseDC(win,hDC);
}

/*--------------------------------------------------------*\
|  SaveBitmap
\*--------------------------------------------------------*/
static Standard_Boolean SaveBitmap (HBITMAP     theHBitmap,
                                    const char* theFileName)
{
  // Get informations about the bitmap
  BITMAP aBitmap;
  if (GetObjectW (theHBitmap, sizeof(BITMAP), &aBitmap) == 0)
  {
    return Standard_False;
  }

  Image_AlienPixMap anImage;
  const Standard_Size aSizeRowBytes = Standard_Size(aBitmap.bmWidth) * 4;
  if (!anImage.InitTrash (Image_Format_BGR32, Standard_Size(aBitmap.bmWidth), Standard_Size(aBitmap.bmHeight), aSizeRowBytes))
  {
    return Standard_False;
  }
  anImage.SetTopDown (false);

  // Setup image data
  BITMAPINFOHEADER aBitmapInfo;
  memset (&aBitmapInfo, 0, sizeof(BITMAPINFOHEADER));
  aBitmapInfo.biSize        = sizeof(BITMAPINFOHEADER);
  aBitmapInfo.biWidth       = aBitmap.bmWidth;
  aBitmapInfo.biHeight      = aBitmap.bmHeight; // positive means bottom-up!
  aBitmapInfo.biPlanes      = 1;
  aBitmapInfo.biBitCount    = 32; // use 32bit for automatic word-alignment per row
  aBitmapInfo.biCompression = BI_RGB;

  // Copy the pixels
  HDC aDC = GetDC (NULL);
  Standard_Boolean isSuccess = GetDIBits (aDC, theHBitmap,
                                          0,                           // first scan line to set
                                          aBitmap.bmHeight,            // number of scan lines to copy
                                          anImage.ChangeData(),        // array for bitmap bits
                                          (LPBITMAPINFO )&aBitmapInfo, // bitmap data info
                                          DIB_RGB_COLORS) != 0;
  ReleaseDC (NULL, aDC);
  return isSuccess && anImage.Save (theFileName);
}

/*--------------------------------------------------------*\
|  Save
\*--------------------------------------------------------*/
Standard_Boolean DrawWindow::Save (const char* theFileName) const
{
  if (myUseBuffer)
  {
    return SaveBitmap (myMemHbm, theFileName);
  }

  RECT aRect;
  GetClientRect (win, &aRect);
  int aWidth  = aRect.right  - aRect.left;
  int aHeight = aRect.bottom - aRect.top;

  // Prepare the DCs
  HDC aDstDC = GetDC (NULL);
  HDC aSrcDC = GetDC (win); // we copy only client area
  HDC aMemDC = CreateCompatibleDC (aDstDC);

  // Copy the screen to the bitmap
  HBITMAP anHBitmapDump = CreateCompatibleBitmap (aDstDC, aWidth, aHeight);
  HBITMAP anHBitmapOld = (HBITMAP )SelectObject (aMemDC, anHBitmapDump);
  BitBlt (aMemDC, 0, 0, aWidth, aHeight, aSrcDC, 0, 0, SRCCOPY);

  Standard_Boolean isSuccess = SaveBitmap (anHBitmapDump, theFileName);

  // Free objects
  DeleteObject (SelectObject (aMemDC, anHBitmapOld));
  DeleteDC (aMemDC);

  return isSuccess;
}

/*--------------------------------------------------------*\
|  DrawString
\*--------------------------------------------------------*/
void DrawWindow::DrawString(int x,int y, char* text)
{
  HDC hDC = GetDC(win);
  HDC aWorkDC = myUseBuffer ? GetMemDC(hDC) : hDC;

  TCollection_ExtendedString textW (text);
  TextOutW(aWorkDC, x, y, (const wchar_t*)textW.ToExtString(), (int )strlen(text));

  if (myUseBuffer) ReleaseMemDC(aWorkDC);
  ReleaseDC(win,hDC);
}

/*--------------------------------------------------------*\
|  DrawSegments
\*--------------------------------------------------------*/
void DrawWindow::DrawSegments(Segment *tab, int nbElem)
{
  HDC hDC = GetDC(win);
  HDC aWorkDC = myUseBuffer ? GetMemDC(hDC) : hDC;

  for(int i = 0 ; i < nbElem ; i++)
  {
    MoveToEx(aWorkDC, tab[i].x1, tab[i].y1, NULL);
    LineTo(aWorkDC, tab[i].x2, tab[i].y2);
  }

  if (myUseBuffer) ReleaseMemDC(aWorkDC);
  ReleaseDC(win,hDC);
}

/*--------------------------------------------------------*\
|  Redraw
\*--------------------------------------------------------*/
void DrawWindow::Redraw()
{
  if (myUseBuffer) {
    HDC hDC = GetDC(win);
    RECT rc;
    GetClientRect(win, &rc);
    HDC aMemDC = GetMemDC(hDC);
    BitBlt(hDC,
           rc.left, rc.top,
           rc.right-rc.left, rc.bottom-rc.top,
           aMemDC,
           0, 0, SRCCOPY);
    ReleaseMemDC(aMemDC);
    ReleaseDC(win,hDC);
  }
}

/*--------------------------------------------------------*\
|  SetMode
\*--------------------------------------------------------*/
void DrawWindow::SetMode(int mode)
{
  HDC hDC = GetDC(win);
  myCurrMode = mode;
  SetROP2(hDC, modeTab[mode]);
  ReleaseDC(win,hDC);
}


/*--------------------------------------------------------*\
|  SetColor
\*--------------------------------------------------------*/
void DrawWindow::SetColor(Standard_Integer color)
{
  HDC hDC = GetDC(win);
  myCurrPen = color;
  SelectObject(hDC,colorPenTab[color]);
  ReleaseDC(win,hDC);
}


/*--------------------------------------------------------*\
|  WExpose
\*--------------------------------------------------------*/
void DrawWindow::WExpose()
{
}


/*--------------------------------------------------------*\
|  WButtonPress
\*--------------------------------------------------------*/
void DrawWindow::WButtonPress(const Standard_Integer,
                               const Standard_Integer,
                               const Standard_Integer&)
{
}


/*--------------------------------------------------------*\
|  WButtonRelease
\*--------------------------------------------------------*/
void DrawWindow::WButtonRelease(const Standard_Integer,
                                 const Standard_Integer,
                                 const Standard_Integer&)
{
}


/*--------------------------------------------------------*\
|  WMotionNotify
\*--------------------------------------------------------*/
void Draw_Window::WMotionNotify(const Standard_Integer ,
                                const Standard_Integer )
{
}


/*--------------------------------------------------------*\
|  WConfigureNotify
\*--------------------------------------------------------*/
void DrawWindow::WConfigureNotify(const Standard_Integer,
                                   const Standard_Integer,
                                   const Standard_Integer,
                                   const Standard_Integer)
{
}


/*--------------------------------------------------------*\
|  WUnmapNotify
\*--------------------------------------------------------*/
void DrawWindow::WUnmapNotify()
{
}



/*
**  IMPLEMENTATION of the CLASS SEGMENT
 */

/*--------------------------------------------------------*\
|  Init
\*--------------------------------------------------------*/

void Segment::Init(Standard_Integer a1, Standard_Integer a2,
                   Standard_Integer a3, Standard_Integer a4)
{
  x1=a1;
  y1=a2;
  x2=a3;
  y2=a4;
}

static DWORD WINAPI tkLoop(VOID);
#ifdef _TK
static Tk_Window mainWindow;
#endif

//* threads sinchronization *//
DWORD  dwMainThreadId;
console_semaphore_value volatile console_semaphore = WAIT_CONSOLE_COMMAND;
#define THE_COMMAND_SIZE 1000     /* Console Command size */
wchar_t console_command[THE_COMMAND_SIZE];
bool volatile isTkLoopStarted = false;

/*--------------------------------------------------------*\
|  Init_Appli
\*--------------------------------------------------------*/
Standard_Boolean Init_Appli(HINSTANCE hInst,
                            HINSTANCE hPrevInst, int nShow, HWND& hWndFrame )
{
  Draw_Interpretor& aCommands = Draw::GetInterpretor();

  DWORD IDThread;
  HANDLE hThread;
  console_semaphore = STOP_CONSOLE;
  aCommands.Init();
  interp = aCommands.Interp();
  Tcl_Init(interp) ;

  dwMainThreadId = GetCurrentThreadId();

  //necessary for normal Tk operation
  hThread = CreateThread(NULL, // no security attributes
                           0,                           // use default stack size
                           (LPTHREAD_START_ROUTINE) tkLoop, // thread function
                           NULL,                    // no thread function argument
                           0,                       // use default creation flags
                           &IDThread);
  if (!hThread) {
    cout << "Tcl/Tk main loop thread not created. Switching to batch mode..." << endl;
#ifdef _TK
    try {
      OCC_CATCH_SIGNALS
      Tk_Init(interp) ;
    } catch  (Standard_Failure) {
      cout <<" Pb au lancement de TK_Init "<<endl;
    }

    Tcl_StaticPackage(interp, "Tk", Tk_Init, (Tcl_PackageInitProc *) NULL);
#endif
    //since the main Tcl/Tk loop wasn't created --> switch to batch mode
    return Standard_False;
  }

  // san - 06/08/2002 - Time for tkLoop to start; Tk fails to initialize otherwise
  while (!isTkLoopStarted)
    Sleep(10);

  // Saving of window classes
  if(!hPrevInst)
    if(!RegisterAppClass(hInst))
      return(Standard_False);

  /*
   ** Enter the application message-polling loop.  This is the anchor for
   ** the application.
  */
  hWndFrame = !Draw_IsConsoleSubsystem ? CreateAppWindow (hInst) : NULL;
  if (hWndFrame != NULL)
  {
    ShowWindow(hWndFrame,nShow);
    UpdateWindow(hWndFrame);
  }

  return Standard_True;
}

Standard_Boolean Draw_Interprete (const char*);

/*--------------------------------------------------------*\
|  readStdinThreadFunc
\*--------------------------------------------------------*/
static DWORD WINAPI readStdinThreadFunc()
{
  if (!Draw_IsConsoleSubsystem)
  {
    return 1;
  }

  // Console locale could be set to the system codepage .OCP (UTF-8 is not properly supported on Windows).
  // However, to use it, we have to care using std::wcerr/fwprintf/WriteConsoleW for non-ascii strings everywhere (including Tcl itself),
  // or otherwise we can have incomplete output issues
  // (e.g. UNICODE string will be NOT just corrupted as in case when we don't set setlocale()
  // but will break further normal output to console due to special characters being accidentally handled by console in the wrong way).
  //setlocale (LC_ALL, ".OCP");

  // _O_U16TEXT can be used with fgetws() to get similar result as ReadConsoleW() without affecting setlocale(),
  // however it would break pipe input
  //_setmode (_fileno(stdin), _O_U16TEXT);

  bool isConsoleInput = true;
  for (;;)
  {
    while (console_semaphore != WAIT_CONSOLE_COMMAND)
    {
      Sleep (100);
    }

    const HANDLE anStdIn = ::GetStdHandle (STD_INPUT_HANDLE);
    if (anStdIn != NULL
     && anStdIn != INVALID_HANDLE_VALUE
     && isConsoleInput)
    {
      DWORD aNbRead = 0;
      if (ReadConsoleW (anStdIn, console_command, THE_COMMAND_SIZE, &aNbRead, NULL))
      {
        console_command[aNbRead] = L'\0';
        console_semaphore = HAS_CONSOLE_COMMAND;
        continue;
      }
      else
      {
        const DWORD anErr = GetLastError();
        if (anErr != ERROR_SUCCESS)
        {
          // fallback using fgetws() which would work with pipes
          // but supports Unicode only through multi-byte encoding (which is not UTF-8)
          isConsoleInput = false;
          continue;
        }
      }
    }

    // fgetws() works only for characters within active locale (see setlocale())
    if (fgetws (console_command, THE_COMMAND_SIZE, stdin))
    {
      console_semaphore = HAS_CONSOLE_COMMAND;
    }
  }
}

/*--------------------------------------------------------*\
|  exitProc: finalization handler for Tcl/Tk thread. Forces parent process to die
\*--------------------------------------------------------*/
void exitProc(ClientData /*dc*/)
{
  NCollection_List<Draw_Window::FCallbackBeforeTerminate>::Iterator Iter(MyCallbacks);
  for(; Iter.More(); Iter.Next())
  {
      (*Iter.Value())();
  }
  HANDLE proc = GetCurrentProcess();
  TerminateProcess(proc, 0);
}

/*--------------------------------------------------------*\
|  tkLoop: implements Tk_Main()-like behaviour in a separate thread
\*--------------------------------------------------------*/
static DWORD WINAPI tkLoop(VOID)
{
  Draw_Interpretor& aCommands = Draw::GetInterpretor();

  Tcl_CreateExitHandler(exitProc, 0);
#if (TCL_MAJOR_VERSION > 8) || ((TCL_MAJOR_VERSION == 8) && (TCL_MINOR_VERSION >= 5))
  {
    Tcl_Channel aChannelIn  = Tcl_GetStdChannel (TCL_STDIN);
    Tcl_Channel aChannelOut = Tcl_GetStdChannel (TCL_STDOUT);
    Tcl_Channel aChannelErr = Tcl_GetStdChannel (TCL_STDERR);
    if (aChannelIn != NULL)
    {
      Tcl_RegisterChannel (aCommands.Interp(), aChannelIn);
    }
    if (aChannelOut != NULL)
    {
      Tcl_RegisterChannel (aCommands.Interp(), aChannelOut);
    }
    if (aChannelErr != NULL)
    {
      Tcl_RegisterChannel (aCommands.Interp(), aChannelErr);
    }
  }
#endif

#ifdef _TK
  // initialize the Tk library if not in 'virtual windows' mode
  // (virtual windows are created by OCCT with native APIs,
  // thus Tk will be useless)
  if (!Draw_VirtualWindows)
  {
    try
    {
      OCC_CATCH_SIGNALS
      Standard_Integer res = Tk_Init (interp);
      if (res != TCL_OK)
      {
#if ((TCL_MAJOR_VERSION > 8) || ((TCL_MAJOR_VERSION == 8) && (TCL_MINOR_VERSION >= 5)))
        cout << "tkLoop: error in Tk initialization. Tcl reported: " << Tcl_GetStringResult(interp) << endl;
#else
        cout << "tkLoop: error in Tk initialization. Tcl reported: " << interp->result << endl;
#endif
      }
    }
    catch (Standard_Failure)
    {
      cout << "tkLoop: exception in TK_Init\n";
    }
    Tcl_StaticPackage (interp, "Tk", Tk_Init, (Tcl_PackageInitProc* ) NULL);
    mainWindow = Tk_MainWindow (interp);
    if (mainWindow == NULL)
    {
#if ((TCL_MAJOR_VERSION > 8) || ((TCL_MAJOR_VERSION == 8) && (TCL_MINOR_VERSION >= 5)))
      fprintf (stderr, "%s\n", Tcl_GetStringResult(interp));
#else
      fprintf (stderr, "%s\n", interp->result);
#endif
      cout << "tkLoop: Tk_MainWindow() returned NULL. Exiting...\n";
      Tcl_Exit (0);
    }
    Tk_Name(mainWindow) = Tk_GetUid (Tk_SetAppName (mainWindow, "Draw"));
  }
#endif //#ifdef _TK

  // set signal handler in the new thread
  OSD::SetSignal(Standard_False);

  // inform the others that we have started
  isTkLoopStarted = true;

  while (console_semaphore == STOP_CONSOLE)
    Tcl_DoOneEvent(TCL_ALL_EVENTS | TCL_DONT_WAIT);

  if (Draw_IsConsoleSubsystem && console_semaphore == WAIT_CONSOLE_COMMAND)
    Prompt(interp, 0);

  //process a command
  Standard_Boolean toLoop = Standard_True;
  while (toLoop)
  {
    while(Tcl_DoOneEvent(TCL_ALL_EVENTS | TCL_DONT_WAIT));
    if (console_semaphore == HAS_CONSOLE_COMMAND)
    {
      TCollection_AsciiString aCmdUtf8 (console_command);
      if (Draw_Interprete (aCmdUtf8.ToCString()))
      {
        if (Draw_IsConsoleSubsystem) Prompt (interp, 0);
      }
      else
      {
        if (Draw_IsConsoleSubsystem) Prompt (interp, 1);
      }
      console_semaphore = WAIT_CONSOLE_COMMAND;
    }
    else
    {
      Sleep(100);
    }
  #ifdef _TK
    // We should not exit until the Main Tk window is closed
    toLoop = (Tk_GetNumMainWindows() > 0) || Draw_VirtualWindows;
  #endif
  }
  Tcl_Exit(0);
  return 0;
}


/*--------------------------------------------------------*\
|  Run_Appli
\*--------------------------------------------------------*/
void Run_Appli(HWND hWnd)
{
  MSG msg;
  HACCEL hAccel = NULL;

  msg.wParam = 1;

//  if (!(hAccel = LoadAccelerators (hInstance, MAKEINTRESOURCE(ACCEL_ID))))
//        MessageBox(hWnd, "MDI: Load Accel failure!", "Error", MB_OK);
  DWORD IDThread;
  HANDLE hThread;
  if (Draw_IsConsoleSubsystem) {
    hThread = CreateThread(NULL, // no security attributes
                           0,                           // use default stack size
                           (LPTHREAD_START_ROUTINE) readStdinThreadFunc, // thread function
                           NULL,                    // no thread function argument
                           0,                       // use default creation flags
                           &IDThread);              // returns thread identifier
    if (!hThread) {
      cout << "pb in creation of the thread reading stdin" << endl;
      Draw_IsConsoleSubsystem = Standard_False;
      Init_Appli (GetModuleHandleW (NULL),
                  GetModuleHandleW (NULL),
                  1, hWnd); // reinit => create MDI client wnd
    }
  }

  //turn on the command interpretation mechanism (regardless of the mode)
  if (console_semaphore == STOP_CONSOLE)
    console_semaphore = WAIT_CONSOLE_COMMAND;

  //simple Win32 message loop
  while (GetMessageW (&msg, NULL, 0, 0) > 0)
  {
    if (!TranslateAcceleratorW (hWnd, hAccel, &msg))
    {
      TranslateMessage (&msg);
      DispatchMessageW (&msg);
    }
  }
  ExitProcess(0);
}


/*--------------------------------------------------------*\
|  Destroy_Appli
\*--------------------------------------------------------*/
void Destroy_Appli(HINSTANCE hInst)
{
  UnregisterAppClass(hInst);
  for (int i = 0 ; i < MAXCOLOR ; i++)
    DeleteObject(colorPenTab[i]);
}

/*--------------------------------------------------------*\
|  SelectWait
\*--------------------------------------------------------*/
void DrawWindow::SelectWait(HANDLE& hWnd, int& x, int& y, int& button)
{
  MSG msg;

  msg.wParam = 1;

  GetMessageW (&msg, NULL, 0, 0);
  while((msg.message != WM_RBUTTONDOWN && msg.message != WM_LBUTTONDOWN) ||
        ! ( Draw_IsConsoleSubsystem || IsChild(DrawWindow::hWndClientMDI,msg.hwnd)) )
  {
    GetMessageW (&msg, NULL, 0, 0);
  }

  hWnd = msg.hwnd;
  x = LOWORD(msg.lParam);
  y = HIWORD(msg.lParam);
  if (msg.message == WM_LBUTTONDOWN)
    button = 1;
  else
    button = 3;
}

/*--------------------------------------------------------*\
|  SelectNoWait
\*--------------------------------------------------------*/
void DrawWindow::SelectNoWait(HANDLE& hWnd, int& x, int& y, int& button)
{
  MSG msg;

  msg.wParam = 1;

  GetMessageW (&msg,NULL,0,0);
  while((msg.message != WM_RBUTTONDOWN && msg.message != WM_LBUTTONDOWN &&
        msg.message != WM_MOUSEMOVE) ||
        ! ( Draw_IsConsoleSubsystem || IsChild(DrawWindow::hWndClientMDI,msg.hwnd) ) )
  {
    GetMessageW(&msg,NULL,0,0);
  }
  hWnd = msg.hwnd;
  x = LOWORD(msg.lParam);
  y = HIWORD(msg.lParam);
  switch (msg.message)
  {
    case WM_LBUTTONDOWN :
                    button = 1;
                    break;

    case WM_RBUTTONDOWN :
                    button = 3;
                    break;

    case WM_MOUSEMOVE :
                    button = 0;
                    break;
  }
}

Standard_Boolean DrawWindow::DefineColor (const Standard_Integer, const char*)
{
  return Standard_True;
};

#endif
