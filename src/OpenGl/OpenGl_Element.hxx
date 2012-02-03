// File:      OpenGl_Element.hxx
// Created:   5 August 2011
// Author:    Sergey ZERCHANINOV
// Copyright: OPEN CASCADE 2011

#ifndef OpenGl_Element_Header
#define OpenGl_Element_Header

#include <OpenGl_Workspace.hxx>

class OpenGl_Element
{
 public:
  OpenGl_Element () {}
  virtual ~OpenGl_Element () {}

  virtual void Render (const Handle(OpenGl_Workspace) &AWorkspace) const = 0;

 public:
  IMPLEMENT_MEMORY_OPERATORS
};

#endif //OpenGl_Element_Header
