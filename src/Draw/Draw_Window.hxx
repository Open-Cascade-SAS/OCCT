// Created on: 1994-07-27
// Created by: Remi LEQUETTE
// Copyright (c) 1994-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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


#ifndef Draw_Window_HeaderFile
#define Draw_Window_HeaderFile

#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>

#ifndef WNT

const Standard_Integer MAXCOLOR = 15;

typedef unsigned long Window;
typedef unsigned long Pixmap;
typedef unsigned long Drawable;

// Definition de la class Base_Window (Definie dans Draw_Window.cxx)
//===================================
class Base_Window;

// Definition de la classe Segment
//================================
struct Segment
{
  short xx1;
  short yy1;
  short xx2;
  short yy2;

  void Init(short x1, short y1, short x2, short y2) {
    xx1 = x1; yy1 = y1; xx2 = x2; yy2 = y2;
  }

};

// Definition de la structure Event
//=================================
typedef struct Event
{
  Standard_Integer type;
  Window window;
  Standard_Integer button;
  Standard_Integer x;
  Standard_Integer y;
} Event;

// Definition de la classe Draw_Window
//====================================
class Draw_Window
{

  public :

    Draw_Window (); // the window is not initialized
    Draw_Window (const char* title,
		 Standard_Integer X, Standard_Integer Y = 0,
		 Standard_Integer DX = 50, Standard_Integer DY = 50);

    Draw_Window (Window mother);
    Draw_Window (Window mother,char* title,
		 Standard_Integer X = 0, Standard_Integer Y = 0,
		 Standard_Integer DX = 50, Standard_Integer DY = 50);
    Draw_Window(const char *window);

    void Init (Standard_Integer X = 0, Standard_Integer Y = 0,
	       Standard_Integer DX = 50, Standard_Integer DY = 50);

    void Init (Window mother,
	       Standard_Integer X = 0, Standard_Integer Y = 0,
	       Standard_Integer DX = 50, Standard_Integer DY = 50);


    void StopWinManager();

    void SetPosition (Standard_Integer NewXpos,
		      Standard_Integer NewYpos);

    void SetDimension(Standard_Integer NewDx,
		      Standard_Integer NewDy);

    void GetPosition(Standard_Integer &PosX,
		     Standard_Integer &PosY);

    Standard_Integer HeightWin() const;
    Standard_Integer WidthWin() const;

    void SetTitle(const char* title);
    char* GetTitle();

    void DisplayWindow();
    void Hide();
    void Destroy();
    void Clear();
    void Wait(Standard_Boolean wait = Standard_True);

    Drawable GetDrawable() const;
    // Initializes off-screen image buffer according to current window size
    void InitBuffer();

    static Standard_Boolean DefineColor(const Standard_Integer, const char*);
    void SetColor(int);
    void SetMode(int);
    void DrawString(int, int, char*);
    void DrawSegments(Segment* ,int);
    void Redraw();
    static void Flush();

    // save snapshot
    Standard_Boolean Save(const char* theFileName) const;

    virtual ~Draw_Window ();

    // X Event management
    virtual void WExpose();
    virtual void WButtonPress(const Standard_Integer X,
			      const Standard_Integer Y,
			      const Standard_Integer& button);
    virtual void WButtonRelease(const Standard_Integer X,
				const Standard_Integer Y,
				const Standard_Integer& button);
    //virtual void WKeyPress(char, KeySym& );
    virtual void WMotionNotify(const Standard_Integer X,
			       const Standard_Integer Y);

    virtual void WConfigureNotify(const Standard_Integer X,
				  const Standard_Integer Y,
				  const Standard_Integer dx,
				  const Standard_Integer dy);

    virtual void WUnmapNotify();

    Base_Window& base;
    Window win;
    Window myMother; // default : myMother is the root window
    Pixmap myBuffer;

    static Draw_Window* firstWindow;
    Draw_Window* next;
    Draw_Window* previous;

    Standard_Boolean myUseBuffer;
    Standard_Boolean withWindowManager;

};

//======================================================
// funtion : Run_Appli
// purpose : run the application
//           interp will be called to interpret a command
//           and return True if the command is complete
//======================================================

void Run_Appli(Standard_Boolean (*inteprete) (const char*));

//======================================================
// funtion : Init_Appli
// purpose :
//======================================================
Standard_Boolean Init_Appli();

//======================================================
// funtion : Destroy_Appli()
// purpose :
//======================================================
void Destroy_Appli();

//======================================================
// funtion : GetNextEvent()
// purpose :
//======================================================
void GetNextEvent(Event&);

#else
// Specifique WNT

#include <windows.h>

#define DRAWCLASS "DRAWWINDOW"
#define DRAWTITLE "Draw View"
#define MAXCOLOR  15

#if !defined(__Draw_API) && !defined(HAVE_NO_DLL)
# ifdef __Draw_DLL
#  define __Draw_API __declspec( dllexport )
# else
#  define __Draw_API __declspec( dllimport )
# endif
#endif

// definition de la classe Segment

class DrawWindow;
class Segment
{
  friend class DrawWindow;
  public :
    //constructeur
    Segment () {}
    //destructeur
    ~Segment () {}

    //methods
    void Init(Standard_Integer,
	      Standard_Integer,
	      Standard_Integer,
	      Standard_Integer);
  private:
    //atributs :
    Standard_Integer x1;
    Standard_Integer y1;
    Standard_Integer x2;
    Standard_Integer y2;
};

//definition de la classe DRAWWINDOW

class DrawWindow
{
  //constructeur
public:
  __Draw_API DrawWindow();
  __Draw_API DrawWindow(char*, Standard_Integer, Standard_Integer,
			Standard_Integer, Standard_Integer);
  __Draw_API DrawWindow(char*, Standard_Integer, Standard_Integer,
			Standard_Integer, Standard_Integer, HWND);
  //destructeur
  __Draw_API ~DrawWindow();

  //methods
public:
  __Draw_API void Init(Standard_Integer, Standard_Integer,
		       Standard_Integer, Standard_Integer);

  __Draw_API void SetUseBuffer(Standard_Boolean);
  // Turns on/off usage of off-screen image buffer (can be used for redrawing optimization)

  __Draw_API Standard_Boolean GetUseBuffer() const { return myUseBuffer; }
  // Returns Standard_True if off-screen image buffer is being used

  //taille et position
  __Draw_API void SetPosition (Standard_Integer,Standard_Integer);
  __Draw_API void SetDimension(Standard_Integer,Standard_Integer);
  __Draw_API void GetPosition (Standard_Integer&,Standard_Integer&);
  __Draw_API Standard_Integer HeightWin() const;
  __Draw_API Standard_Integer WidthWin()  const;

  //Title
  __Draw_API void SetTitle(char*);
  __Draw_API char* GetTitle();

  //Affichage
  __Draw_API void DisplayWindow();
  __Draw_API void Hide();
  __Draw_API void Destroy();
  __Draw_API void Clear();
  __Draw_API static void Flush() {} ;

  // save snapshot
  __Draw_API Standard_Boolean Save(const char* theFileName) const;

  //Dessin
  __Draw_API void DrawString(int,int,char*);
  __Draw_API void DrawSegments(Segment*,int);

  __Draw_API void InitBuffer();
  // Initializes off-screen image buffer according to current window size

  __Draw_API void Redraw();
  // Copies an image from memory buffer to screen

  //Couleur
  __Draw_API void SetColor(Standard_Integer);
  __Draw_API void SetMode(int);
  __Draw_API static Standard_Boolean DefineColor ( const Standard_Integer,const char*);

  //Gestion des Messages
  __Draw_API virtual void WExpose ();
  __Draw_API virtual void WButtonPress(const Standard_Integer,const Standard_Integer,
				       const Standard_Integer&);
  __Draw_API virtual void WButtonRelease(const Standard_Integer,const Standard_Integer,
					 const Standard_Integer&);
  __Draw_API virtual void WMotionNotify(const Standard_Integer,const Standard_Integer);
  __Draw_API virtual void WConfigureNotify(const Standard_Integer,const Standard_Integer,
					   const Standard_Integer,const Standard_Integer);
  __Draw_API virtual void WUnmapNotify();

  //Gestion souris
  __Draw_API static void SelectWait   (HANDLE&,int&,int&,int&);
  __Draw_API static void SelectNoWait (HANDLE&,int&,int&,int&);

  // Procedure de fenetre
  __Draw_API static LONG APIENTRY DrawProc (HWND,UINT,WPARAM,LONG);

private:

  __Draw_API static HWND CreateDrawWindow(HWND,int);
  __Draw_API HDC  GetMemDC(HDC);
  __Draw_API void ReleaseMemDC(HDC);

  //atributs
public:
  HWND win;
  static HWND hWndClientMDI;

private:
  static DrawWindow* firstWindow;
  DrawWindow* next;
  DrawWindow* previous;
  HBITMAP myMemHbm;
  HBITMAP myOldHbm;
  Standard_Boolean myUseBuffer;
  Standard_Integer myCurrPen;
  Standard_Integer myCurrMode;
};

typedef DrawWindow Draw_Window;
typedef enum {
  STOP_CONSOLE,
  WAIT_CONSOLE_COMMAND,
  HAS_CONSOLE_COMMAND} console_semaphore_value;

// PROCEDURE DE DRAW WINDOW

__Draw_API Standard_Boolean Init_Appli(HINSTANCE,HINSTANCE,int,HWND&);
__Draw_API void Run_Appli(HWND);
__Draw_API void Destroy_Appli(HINSTANCE);

#endif

#endif
