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


#include <OpenGl_GlCore11.hxx>

#include <OpenGl_Polyline.hxx>

#include <OpenGl_AspectLine.hxx>
#include <OpenGl_Structure.hxx>

/*----------------------------------------------------------------------*/

OpenGl_Polyline::OpenGl_Polyline (const Graphic3d_Array1OfVertex& AListVertex)
: myNbVertices(AListVertex.Length()),
  myVertices(NULL)
{
  myVertices = new TEL_POINT[myNbVertices];
  memcpy( myVertices, &AListVertex(AListVertex.Lower()), myNbVertices*sizeof(TEL_POINT) );
}

/*----------------------------------------------------------------------*/

OpenGl_Polyline::~OpenGl_Polyline()
{
  delete[] myVertices;
}

void OpenGl_Polyline::Release (const Handle(OpenGl_Context)& theContext)
{
  //
}

/*----------------------------------------------------------------------*/

void OpenGl_Polyline::Render (const Handle(OpenGl_Workspace)& theWorkspace) const
{
  // Apply line aspect
  const OpenGl_AspectLine* anAspectLine = theWorkspace->AspectLine (Standard_True);

  // Temporarily disable environment mapping
  glPushAttrib (GL_ENABLE_BIT);
  glDisable (GL_TEXTURE_1D);
  glDisable (GL_TEXTURE_2D);

  glBegin (theWorkspace->DegenerateModel != 3 ? GL_LINE_STRIP : GL_POINTS);

  // Use highlight colors
  glColor3fv ((theWorkspace->NamedStatus & OPENGL_NS_HIGHLIGHT) ? theWorkspace->HighlightColor->rgb : anAspectLine->Color().rgb);

  for (Tint i = 0; i < myNbVertices; ++i)
    glVertex3fv (myVertices[i].xyz);

  glEnd();

  glPopAttrib();
}
