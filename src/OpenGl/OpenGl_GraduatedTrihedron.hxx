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


#ifndef _OpenGl_GraduatedTrihedron_Header
#define _OpenGl_GraduatedTrihedron_Header

#include <Handle_OpenGl_GraduatedTrihedron.hxx>

#include <Graphic3d_CGraduatedTrihedron.hxx>

class OpenGl_View;

class OpenGl_GraduatedTrihedron : public MMgt_TShared
{
 public:
  OpenGl_GraduatedTrihedron (const Graphic3d_CGraduatedTrihedron &AData);
  virtual ~OpenGl_GraduatedTrihedron ();

  static void SetMinMax (const Standard_ShortReal xMin, const Standard_ShortReal yMin, const Standard_ShortReal zMin,
                         const Standard_ShortReal xMax, const Standard_ShortReal yMax, const Standard_ShortReal zMax);

  void Render (const Handle(OpenGl_Workspace) &AWorkspace) const;

  // Type definition
  //
  DEFINE_STANDARD_RTTI(OpenGl_GraduatedTrihedron)

 protected:

  const wchar_t *myXName;
  const wchar_t *myYName;
  const wchar_t *myZName;
  unsigned char myDrawXName, myDrawYName, myDrawZName;
  unsigned char myDrawXValues, myDrawYValues, myDrawZValues;
  unsigned char myDrawGrid;
  unsigned char myDrawAxes;
  unsigned int myNbX, myNbY, myNbZ;
  int myXOffset, myYOffset, myZOffset;
  int myXAxisOffset, myYAxisOffset, myZAxisOffset;
  unsigned char myDrawXTickmarks, myDrawYTickmarks, myDrawZTickmarks;
  unsigned int myXTickmarkLength, myYTickmarkLength, myZTickmarkLength;
  float myGridColor[3];
  float myXNameColor[3];
  float myYNameColor[3];
  float myZNameColor[3];
  float myXColor[3];
  float myYColor[3];
  float myZColor[3];
  const char *myFontOfNames;
  Font_FontAspect myStyleOfNames;
  int mySizeOfNames;
  const char* myFontOfValues;
  Font_FontAspect myStyleOfValues;
  int mySizeOfValues;
  minMaxValuesCallback myCbCubicAxes;
  void* myPtrVisual3dView;

 public:
  DEFINE_STANDARD_ALLOC
};

#endif //_OpenGl_GraduatedTrihedron_Header
