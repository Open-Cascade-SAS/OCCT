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


#ifndef _OpenGl_AspectLine_Header
#define _OpenGl_AspectLine_Header

#include <InterfaceGraphic_telem.hxx>
#include <Aspect_TypeOfLine.hxx>

#include <OpenGl_Element.hxx>

class OpenGl_AspectLine : public OpenGl_Element
{
 public:

  OpenGl_AspectLine ();
  OpenGl_AspectLine (const OpenGl_AspectLine &AnOther);

  void SetContext (const CALL_DEF_CONTEXTLINE &AContext);

  const TEL_COLOUR & Color() const { return myColor; }
  Aspect_TypeOfLine  Type() const { return myType; }
  float              Width() const { return myWidth; }

  virtual void Render  (const Handle(OpenGl_Workspace)& theWorkspace) const;
  virtual void Release (const Handle(OpenGl_Context)&   theContext);

 protected:

  TEL_COLOUR        myColor;
  Aspect_TypeOfLine myType;
  float             myWidth;

 public:
  DEFINE_STANDARD_ALLOC
};

#endif //_OpenGl_AspectLine_Header
