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

#ifndef Draw_Window_HeaderFile
#define Draw_Window_HeaderFile

#if defined(_WIN32)
  #include <windows.h>
#endif

#include <Aspect_Drawable.hxx>
#include <NCollection_Vec2.hxx>
#include <TCollection_AsciiString.hxx>

#include <memory>

const int MAXCOLOR = 15;

//! Segment definition.
struct Draw_XSegment
{
  NCollection_Vec2<short> Points[2]; // same as XSegment

  Draw_XSegment() = default;

  Draw_XSegment(short theXStart, short theYStart, short theXEnd, short theYEnd)
      : Points{NCollection_Vec2<short>(theXStart, theYStart),
               NCollection_Vec2<short>(theXEnd, theYEnd)}
  {
  }

  NCollection_Vec2<short>& operator[](int theIndex) { return Points[theIndex]; }

  const NCollection_Vec2<short>& operator[](int theIndex) const { return Points[theIndex]; }

  void Init(int theXStart, int theYStart, int theXEnd, int theYEnd)
  {
    Points[0].SetValues((short)theXStart, (short)theYStart);
    Points[1].SetValues((short)theXEnd, (short)theYEnd);
  }
};

#if defined(_WIN32)

  #define DRAWCLASS L"DRAWWINDOW"
  #define DRAWTITLE L"Draw View"

enum console_semaphore_value
{
  STOP_CONSOLE,
  WAIT_CONSOLE_COMMAND,
  HAS_CONSOLE_COMMAND
};

// global variable describing console state
extern console_semaphore_value volatile console_semaphore;

  // Console command buffer
  #define DRAW_COMMAND_SIZE 1000
extern wchar_t console_command[DRAW_COMMAND_SIZE + 1];

// PROCEDURE DE DRAW WINDOW
Standard_EXPORT bool Init_Appli(HINSTANCE, HINSTANCE, int, HWND&);
Standard_EXPORT void Run_Appli(HWND);
Standard_EXPORT void Destroy_Appli(HINSTANCE);

#else

//! Run the application.
//! interp will be called to interpret a command and return True if the command is complete.
void Run_Appli(bool (*inteprete)(const char*));

//! Initialize application.
bool Init_Appli();

//! Destroy application.
void Destroy_Appli();

  #if defined(HAVE_XLIB)

typedef unsigned long Window;
typedef unsigned long Pixmap;
typedef unsigned long Drawable;

  #elif defined(__APPLE__)
    #ifdef __OBJC__
@class NSView;
@class NSWindow;
@class NSImage;
@class Draw_CocoaView;
    #else
struct NSView;
struct NSWindow;
struct NSImage;
struct Draw_CocoaView;
    #endif
  #endif

#endif

//! Draw window.
class Draw_Window
{
public:
  //! Type of the callback function that is to be passed to the method AddCallbackBeforeTerminate().
  typedef void (*FCallbackBeforeTerminate)();

  //! This method registers a callback function that will be called just before exit.
  //! This is useful especially for Windows platform, on which Draw is normally self-terminated
  //! instead of exiting.
  Standard_EXPORT static void AddCallbackBeforeTerminate(FCallbackBeforeTerminate theCB);

  //! Just in case method for un-registering a callback previously registered by
  //! AddCallbackBeforeTerminate().
  Standard_EXPORT static void RemoveCallbackBeforeTerminate(FCallbackBeforeTerminate theCB);

  //! @sa SetColor()
  Standard_EXPORT static bool DefineColor(const int theIndex, const char* theColorName);

  //! XFlush() wrapper (X11), has no effect on other platforms.
  Standard_EXPORT static void Flush();

public:
  //! Destructor.
  Standard_EXPORT virtual ~Draw_Window();

  //! Get window position.
  Standard_EXPORT void GetPosition(int& thePosX, int& thePosY);

  //! Set window position.
  Standard_EXPORT void SetPosition(int theNewXpos, int theNewYpos);

  //! Return window height.
  Standard_EXPORT int HeightWin() const;

  //! Return window width.
  Standard_EXPORT int WidthWin() const;

  //! Set window dimensions.
  Standard_EXPORT void SetDimension(int theNewDx, int theNewDy);

  //! Return window title.
  Standard_EXPORT TCollection_AsciiString GetTitle() const;

  //! Set window title.
  Standard_EXPORT void SetTitle(const TCollection_AsciiString& theTitle);

  //! Return true if window is displayed on the screen.
  Standard_EXPORT bool IsMapped() const;

  //! Display window on the screen.
  Standard_EXPORT void DisplayWindow();

  //! Hide window.
  Standard_EXPORT void Hide();

  //! Destroy window.
  Standard_EXPORT void Destroy();

  //! Clear window content.
  Standard_EXPORT void Clear();

  //! Returns true if off-screen image buffer is being used
  bool GetUseBuffer() const { return myUseBuffer; }

  // Turns on/off usage of off-screen image buffer (can be used for redrawing optimization)
  Standard_EXPORT void SetUseBuffer(bool theToUse);

  //! Set active color index for further paintings.
  //! @sa DefineColor()
  Standard_EXPORT void SetColor(int theColor);

  //! Set active paint mode (3 for COPY; 6 for XOR).
  Standard_EXPORT void SetMode(int theMode);

  //! Draw the string.
  Standard_EXPORT void DrawString(int theX, int theY, const char* theText);

  //! Draw array of segments.
  Standard_EXPORT void DrawSegments(const Draw_XSegment* theSegments, int theNumberOfElements);

  //! Redraw window content.
  Standard_EXPORT void Redraw();

  //! Save snapshot.
  Standard_EXPORT bool Save(const char* theFileName) const;

  //! Perform window exposing.
  virtual void WExpose() = 0;

  //! (Re)initializes off-screen image buffer according to current window size.
  Standard_EXPORT void InitBuffer();

protected:
  //! Main constructor.
  //! @param[in] theTitle   window title
  //! @param[in] theXY      top-left position
  //! @param[in] theSize    window dimensions
  //! @param[in] theParent  optional native parent window
  //! @param[in] theWin     optional native window
  Standard_EXPORT Draw_Window(const char*                  theTitle,
                              const NCollection_Vec2<int>& theXY,
                              const NCollection_Vec2<int>& theSize,
                              Aspect_Drawable              theParent,
                              Aspect_Drawable              theWin);

  //! Initialize the window.
  Standard_EXPORT void init(const NCollection_Vec2<int>& theXY,
                            const NCollection_Vec2<int>& theSize);

public:
#if defined(_WIN32)
  bool IsEqualWindows(HANDLE theWindow) { return myWindow == theWindow; }

  Standard_EXPORT static void SelectWait(HANDLE& theWindow, int& theX, int& theY, int& theButton);
  Standard_EXPORT static void SelectNoWait(HANDLE& theWindow, int& theX, int& theY, int& theButton);
  // Procedure de fenetre
  Standard_EXPORT static LRESULT APIENTRY DrawProc(HWND, UINT, WPARAM, LPARAM);
  static HWND                             hWndClientMDI;
#elif defined(HAVE_XLIB)
  bool IsEqualWindows(Window theWindow) { return myWindow == theWindow; }

  //! Event structure.
  struct Draw_XEvent
  {
    int    type;
    Window window;
    int    button;
    int    x;
    int    y;
  };

  //! Retrieve event.
  static void GetNextEvent(Draw_XEvent& theEvent);

  void Wait(bool theToWait = true);
#elif defined(__APPLE__)
  bool IsEqualWindows(const long theWindowNumber);

  static void GetNextEvent(bool  theWait,
                           long& theWindowNumber,
                           int&  theX,
                           int&  theY,
                           int&  theButton);
#endif

private:
#if defined(_WIN32)
  Standard_EXPORT static HWND createDrawWindow(HWND, int);
  Standard_EXPORT HDC         getMemDC(HDC theWinDC);
  Standard_EXPORT void        releaseMemDC(HDC theMemDC);
#elif defined(HAVE_XLIB)
  Drawable GetDrawable() const { return myUseBuffer ? myImageBuffer : myWindow; }
  struct Base_Window; // opaque structure with extra Xlib parameters
#endif

private:
#if defined(_WIN32)
  HWND    myWindow; //!< native window
  HBITMAP myMemHbm;
  HBITMAP myOldHbm;
  int     myCurrPen;
  int     myCurrMode;
#elif defined(HAVE_XLIB)
  Window                       myWindow; //!< native window
  Window                       myMother; //!< parent native window
  Pixmap                       myImageBuffer;
  std::unique_ptr<Base_Window> myBase;
#elif defined(__APPLE__)
  NSWindow*       myWindow; //!< native window
  Draw_CocoaView* myView;
  NSImage*        myImageBuffer;
#else
  Aspect_Drawable myWindow;
#endif
  int  myCurrentColor;
  bool myUseBuffer;
};

#endif // Draw_Window_HeaderFile
