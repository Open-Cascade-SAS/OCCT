// File:      OpenGl_Window.hxx
// Created:   20 September 2011
// Author:    Sergey ZERCHANINOV
// Copyright: OPEN CASCADE 2011

#ifndef _OpenGl_Window_Header
#define _OpenGl_Window_Header

#include <Handle_OpenGl_Window.hxx>
#include <MMgt_TShared.hxx>

#include <InterfaceGraphic.hxx>
#include <InterfaceGraphic_Aspect.hxx>
#include <InterfaceGraphic_telem.hxx>

#include <Handle_OpenGl_Display.hxx>
#include <Handle_OpenGl_Context.hxx>

//! This class represents low-level wrapper over window with GL context.
//! The window itself should be provided to constructor.
class OpenGl_Window : public MMgt_TShared
{
public:

  //! Main constructor - prepare GL context for specified window.
  OpenGl_Window (const Handle(OpenGl_Display)& theDisplay,
                 const CALL_DEF_WINDOW&        theCWindow,
                 Aspect_RenderingContext       theGContext);

  //! Destructor
  virtual ~OpenGl_Window();

  //! Resizes the window.
  void Resize (const CALL_DEF_WINDOW& theCWindow);

  //! Reads depth component for current scene into specified buffer.
  void ReadDepths (const Standard_Integer theX,     const Standard_Integer theY,
                   const Standard_Integer theWidth, const Standard_Integer theHeight,
                   float* theDepths);

  void SetBackgroundColor (const Standard_ShortReal theR,
                           const Standard_ShortReal theG,
                           const Standard_ShortReal theB);

  void EnablePolygonOffset() const;
  void DisablePolygonOffset() const;

  Standard_Integer Width()  const { return myWidth; }
  Standard_Integer Height() const { return myHeight; }
  const TEL_COLOUR& BackgroundColor() const { return myBgColor; }
  Standard_Boolean Dither()     const { return myDither; }
  Standard_Boolean BackDither() const { return myBackDither; }

  const Handle(OpenGl_Display)& GetDisplay() const { return myDisplay; }

  const Handle(OpenGl_Context)& GetGlContext() const { return myGlContext; }

  WINDOW    GetWindow()   const { return myWindow; }
  GLCONTEXT GetGContext() const { return myGContext; }

protected:

  //! Activates GL context and setup viewport.
  void Init();

  //! Makes GL context for this window active in current thread
  virtual Standard_Boolean Activate();

  void EnableFeatures() const;
  void DisableFeatures() const;

  //! Draw directly to the FRONT buffer. Can cause artifacts on the screen.
  void MakeFrontBufCurrent() const;

  //! Draw to BACK buffer. Normal and default state.
  void MakeBackBufCurrent() const;

  //! Draw simultaneously to BACK and FRONT buffers. Abnormal usage.
  void MakeFrontAndBackBufCurrent() const;

protected:

  Handle(OpenGl_Display) myDisplay;
  WINDOW                 myWindow;      //!< native window handle, system-specific
  Handle(OpenGl_Context) myGlContext;
  GLCONTEXT              myGContext;    //!< native GL context bound to this window, system-specific
  Standard_Boolean       myOwnGContext; //!< set to TRUE if GL context was not created by this class
#if (defined(_WIN32) || defined(__WIN32__))
  HDC                    myWindowDC;
  BOOL                   mySysPalInUse;
#endif

  Standard_Integer       myWidth;       // WSWidth
  Standard_Integer       myHeight;      // WSHeight
  TEL_COLOUR             myBgColor;     // WSBackground
  Standard_Boolean       myDither;
  Standard_Boolean       myBackDither;

public:

  DEFINE_STANDARD_RTTI(OpenGl_Window) // Type definition
  DEFINE_STANDARD_ALLOC

};

#endif //_OpenGl_Window_Header
