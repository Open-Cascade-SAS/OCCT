// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _Graphic3d_CGroup_HeaderFile
#define _Graphic3d_CGroup_HeaderFile

#include <Aspect_TypeOfMarker.hxx>
#include <InterfaceGraphic_Visual3d.hxx>
#include <Graphic3d_CTexture.hxx>
#include <Graphic3d_MarkerImage_Handle.hxx>
#include <Graphic3d_ShaderProgram_Handle.hxx>

class Graphic3d_CStructure;

class CALL_DEF_CONTEXTFILLAREA
{

public:

  CALL_DEF_CONTEXTFILLAREA()
  : IsDef (0),
    IsSet (0),
    Style (0),
    LineType (0),
    Width (0.0f),
    Hatch (0),
    Distinguish (0),
    BackFace (0),
    Edge (0),
    PolygonOffsetMode   (0),
    PolygonOffsetFactor (0.0f),
    PolygonOffsetUnits  (0.0f)
  {
    memset(&IntColor,0,sizeof(IntColor));
	memset(&BackIntColor,0,sizeof(BackIntColor));
	memset(&EdgeColor,0,sizeof(EdgeColor));
	memset(&Front,0,sizeof(Front));
	memset(&Back,0,sizeof(Back));
  }

public:

  int IsDef;

  int IsSet;

  int Style;

  CALL_DEF_COLOR IntColor;
  CALL_DEF_COLOR BackIntColor;
  CALL_DEF_COLOR EdgeColor;

  int LineType;

  float Width;

  int Hatch;

  int Distinguish;
  int BackFace;

  int Edge;

  CALL_DEF_MATERIAL Front;
  CALL_DEF_MATERIAL Back;

  Graphic3d_CTexture Texture;

  int   PolygonOffsetMode;
  float PolygonOffsetFactor;
  float PolygonOffsetUnits;

  Handle(Graphic3d_ShaderProgram) ShaderProgram;
};

class CALL_DEF_CONTEXTMARKER
{
public:

  CALL_DEF_CONTEXTMARKER()
  : IsDef (0),
    IsSet (0),
    MarkerType (Aspect_TOM_POINT),
    Scale (0),
    MarkerImage (NULL)
  {
	  memset(&Color,0,sizeof(Color));
  }

public:

  Standard_Integer IsDef;
  Standard_Integer IsSet;
  CALL_DEF_COLOR Color;
  Aspect_TypeOfMarker MarkerType;
  Standard_ShortReal Scale;
  Handle(Graphic3d_MarkerImage) MarkerImage;
  Handle(Graphic3d_ShaderProgram) ShaderProgram;

};

class Graphic3d_CGroup
{

public:

  void* ptrGroup;

  CALL_DEF_CONTEXTLINE     ContextLine;
  CALL_DEF_CONTEXTFILLAREA ContextFillArea;
  CALL_DEF_CONTEXTMARKER   ContextMarker;
  CALL_DEF_CONTEXTTEXT     ContextText;

  Graphic3d_CStructure* Struct;

  CALL_DEF_PICKID PickId;

};

const Handle(Standard_Type)& TYPE(Graphic3d_CGroup);

#endif // Graphic3d_CGroup_HeaderFile
