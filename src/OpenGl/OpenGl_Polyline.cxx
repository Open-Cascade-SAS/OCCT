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
  myVertices(NULL),
  myColors(NULL)
{
  myVertices = new TEL_POINT[myNbVertices];
  memcpy( myVertices, &AListVertex(AListVertex.Lower()), myNbVertices*sizeof(TEL_POINT) );
}

/*----------------------------------------------------------------------*/

OpenGl_Polyline::OpenGl_Polyline (const Graphic3d_Array1OfVertexC& AListVertex)
: myNbVertices(AListVertex.Length()),
  myVertices(NULL),
  myColors(NULL)
{
  myVertices = new TEL_POINT[myNbVertices];
  myColors = new TEL_COLOUR[myNbVertices];

  Standard_Integer i = 0, j = AListVertex.Lower();
  Standard_Real X, Y, Z;
  for ( ; i < myNbVertices; i++, j++)
  {
    AListVertex(j).Coord(X, Y, Z);
    myVertices[i].xyz[0] = float (X);
    myVertices[i].xyz[1] = float (Y);
    myVertices[i].xyz[2] = float (Z);
    AListVertex(j).Color().Values(X, Y, Z, Quantity_TOC_RGB);
    myColors[i].rgb[0] = float (X);
    myColors[i].rgb[1] = float (Y);
    myColors[i].rgb[2] = float (Z);
    myColors[i].rgb[3] = 1.0F;
  }
}

/*----------------------------------------------------------------------*/

OpenGl_Polyline::OpenGl_Polyline (const Standard_Real X1,const Standard_Real Y1,const Standard_Real Z1,
                                const Standard_Real X2,const Standard_Real Y2,const Standard_Real Z2)
: myNbVertices(2),
  myVertices(new TEL_POINT[2]),
  myColors(NULL)
{
  myVertices[0].xyz[0] = float (X1);
  myVertices[0].xyz[1] = float (Y1);
  myVertices[0].xyz[2] = float (Z1);
  myVertices[1].xyz[0] = float (X2);
  myVertices[1].xyz[1] = float (Y2);
  myVertices[1].xyz[2] = float (Z2);
}

/*----------------------------------------------------------------------*/

OpenGl_Polyline::~OpenGl_Polyline ()
{
  if ( myVertices )
    delete[] myVertices;
  if( myColors )
    delete[] myColors;
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
  if (!myColors || (theWorkspace->NamedStatus & OPENGL_NS_HIGHLIGHT))
  {
    // Use highlight colors
    glColor3fv ((theWorkspace->NamedStatus & OPENGL_NS_HIGHLIGHT) ? theWorkspace->HighlightColor->rgb : anAspectLine->Color().rgb);

    for (Tint i = 0; i < myNbVertices; ++i)
      glVertex3fv (myVertices[i].xyz);
  }
  else
  {
    for (Tint i = 0; i < myNbVertices; ++i)
    {
      glColor3fv (myColors[i].rgb);
      glVertex3fv (myVertices[i].xyz);
    }
  }
  glEnd();

  glPopAttrib();
}

/*----------------------------------------------------------------------*/
