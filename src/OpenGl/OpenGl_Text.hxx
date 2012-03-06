// File:      OpenGl_Text.hxx
// Created:   13 July 2011
// Author:    Sergey ZERCHANINOV
// Copyright: OPEN CASCADE 2011

#ifndef OpenGl_Text_Header
#define OpenGl_Text_Header

#include <OpenGl_Element.hxx>

#include <OpenGl_TextParam.hxx>

#include <TCollection_ExtendedString.hxx>
#include <Graphic3d_Vertex.hxx>
#include <Graphic3d_HorizontalTextAlignment.hxx>
#include <Graphic3d_VerticalTextAlignment.hxx>

class OpenGl_Text : public OpenGl_Element
{
 public:

  OpenGl_Text (const TCollection_ExtendedString& AText,
              const Graphic3d_Vertex& APoint,
              const Standard_Real AHeight,
              const Graphic3d_HorizontalTextAlignment AHta,
              const Graphic3d_VerticalTextAlignment AVta);
  virtual ~OpenGl_Text ();

  virtual void Render (const Handle(OpenGl_Workspace) &AWorkspace) const;

 protected:

  OpenGl_TextParam myParam;
  TEL_POINT       myAttachPnt;
  const wchar_t  *myString;

 public:
  DEFINE_STANDARD_ALLOC
};

#endif //OpenGl_Text_Header
