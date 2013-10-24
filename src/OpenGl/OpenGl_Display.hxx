// Created on: 2011-09-20
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2012 OPEN CASCADE SAS
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
