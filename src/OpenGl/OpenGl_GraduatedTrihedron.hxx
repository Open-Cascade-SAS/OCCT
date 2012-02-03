// File:      OpenGl_GraduatedTrihedron.hxx
// Created:   20 September 2011
// Author:    Sergey ZERCHANINOV
// Copyright: OPEN CASCADE 2011

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
  OSD_FontAspect myStyleOfNames;
  int mySizeOfNames;
  const char* myFontOfValues;
  OSD_FontAspect myStyleOfValues;
  int mySizeOfValues;
  minMaxValuesCallback myCbCubicAxes;
  void* myPtrVisual3dView;

 public:
  IMPLEMENT_MEMORY_OPERATORS
};

#endif //_OpenGl_GraduatedTrihedron_Header
