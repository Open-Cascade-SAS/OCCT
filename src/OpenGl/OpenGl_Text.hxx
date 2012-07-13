// Created on: 2011-07-13
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

  virtual void Render  (const Handle(OpenGl_Workspace)& theWorkspace) const;
  virtual void Release (const Handle(OpenGl_Context)&   theContext);

protected:

  virtual ~OpenGl_Text();

protected:

  OpenGl_TextParam myParam;
  TEL_POINT       myAttachPnt;
  const wchar_t  *myString;

public:

  DEFINE_STANDARD_ALLOC

};

#endif //OpenGl_Text_Header
