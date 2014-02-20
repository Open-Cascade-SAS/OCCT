// Created on: 2011-09-20
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2014 OPEN CASCADE SAS
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

#ifndef _OpenGl_Display_Header
#define _OpenGl_Display_Header

#include <Handle_OpenGl_Display.hxx>
#include <MMgt_TShared.hxx>

#include <Aspect_Display.hxx>
#include <Aspect_DisplayConnection.hxx>
#include <Aspect_TypeOfLine.hxx>
#include <Aspect_TypeOfMarker.hxx>

#include <Font_FontAspect.hxx>

#include <InterfaceGraphic_telem.hxx>

#include <Handle_OpenGl_Window.hxx>

struct OpenGl_Facilities
{
  int AntiAliasing;
  int DepthCueing;
  int DoubleBuffer;
  int ZBuffer;
  int MaxLights;
  int MaxViews;
};

class OpenGl_Context;

class OpenGl_Display : public MMgt_TShared
{
public:

  OpenGl_Display (const Handle(Aspect_DisplayConnection)& theDisplayConnection);
  virtual ~OpenGl_Display ();

  Aspect_Display GetDisplay () const { return myDisplay; }

  const OpenGl_Facilities & Facilities () const { return myFacilities; }

  Standard_Boolean DBuffer () const { return myDBuffer; }
  Standard_Boolean Dither () const { return myDither; }
  Standard_Boolean BackDither () const { return myBackDither; }
  Standard_Boolean Walkthrough () const { return myWalkthrough; }
  Standard_Boolean SymPerspective() const { return mySymPerspective; }
  Standard_Boolean PolygonOffset (Standard_ShortReal &AFactor, Standard_ShortReal &AUnits) const
  {
    AFactor = myOffsetFactor;
    AUnits = myOffsetUnits;
    return Standard_True;
  }
  Standard_Integer AntiAliasingMode () const { return myAntiAliasingMode; }

  // System attributes

  void InitAttributes();
  void ReleaseAttributes (const OpenGl_Context* theGlCtx);

  void SetTypeOfLine (const Aspect_TypeOfLine AType) const;

  void SetTypeOfHatch (const int AType) const;

  friend class OpenGl_Window;

  // Type definition
  //
  DEFINE_STANDARD_RTTI(OpenGl_Display)

 protected:

  void Init ();

  Aspect_Display   myDisplay;
  OpenGl_Facilities myFacilities;

  Standard_Boolean myDBuffer;
  Standard_Boolean myDither;
  Standard_Boolean myBackDither;
  Standard_Boolean myWalkthrough;
  Standard_Boolean mySymPerspective;
  Standard_ShortReal myOffsetFactor;
  Standard_ShortReal myOffsetUnits;
  Standard_Integer myAntiAliasingMode;

  unsigned int myLinestyleBase;
  unsigned int myPatternBase;

 public:
  DEFINE_STANDARD_ALLOC
};

#endif // _OpenGl_Display_Header
