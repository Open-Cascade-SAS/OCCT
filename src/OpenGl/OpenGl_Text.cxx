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
#include <OpenGl_Text.hxx>
#include <OpenGl_AspectText.hxx>
#include <OpenGl_Structure.hxx>

#include <GL/glu.h> // gluUnProject()

/*----------------------------------------------------------------------*/

OpenGl_Text::OpenGl_Text (const TCollection_ExtendedString& AText,
                        const Graphic3d_Vertex& APoint,
                        const Standard_Real AHeight,
                        const Graphic3d_HorizontalTextAlignment AHta,
                        const Graphic3d_VerticalTextAlignment AVta)
: myString(NULL)
{
  const Techar *str = (const Techar *) AText.ToExtString();

  //szv: instead of strlen + 1
  int i = 0; while (str[i++]);

  wchar_t *wstr = new wchar_t[i];

  //szv: instead of memcpy
  i = 0; while (wstr[i++] = (wchar_t)(*str++));
  if (myString) delete[] myString;
  myString = wstr;

  Standard_Real X, Y, Z;
  APoint.Coord(X, Y, Z);
  myAttachPnt.xyz[0] = float (X);
  myAttachPnt.xyz[1] = float (Y);
  myAttachPnt.xyz[2] = float (Z);

  myParam.Height = int (AHeight);

  myParam.HAlign = AHta;
  myParam.VAlign = AVta;
}

/*----------------------------------------------------------------------*/

OpenGl_Text::~OpenGl_Text ()
{
  if (myString)
    delete[] myString;
}

/*----------------------------------------------------------------------*/

void OpenGl_Text::Render (const Handle(OpenGl_Workspace) &AWorkspace) const
{
  if ( AWorkspace->DegenerateModel > 0 && AWorkspace->SkipRatio >= 1.f )
    return;

  const OpenGl_AspectText *aspect_text = AWorkspace->AspectText( Standard_True );

  const TEL_COLOUR *tcolor, *scolor;

  // Use highlight colours
  if( AWorkspace->NamedStatus & OPENGL_NS_HIGHLIGHT )
  {                         
    tcolor = scolor = AWorkspace->HighlightColor;
  }
  else
  {
    tcolor = &aspect_text->Color();
    scolor = &aspect_text->SubtitleColor();
  }

  // Handle annotation style
  GLboolean flag_zbuffer = GL_FALSE;
  if (aspect_text->StyleType() == Aspect_TOST_ANNOTATION)
  {
    flag_zbuffer = glIsEnabled(GL_DEPTH_TEST);
    if (flag_zbuffer) glDisable(GL_DEPTH_TEST);
  }

  AWorkspace->SetTextParam(&myParam);

  GLdouble        modelMatrix[16], projMatrix[16];
  GLint           viewport[4];
  GLdouble        objrefX, objrefY, objrefZ;
  GLdouble        objX, objY, objZ;
  GLdouble        obj1X, obj1Y, obj1Z;
  GLdouble        obj2X, obj2Y, obj2Z;
  GLdouble        obj3X, obj3Y, obj3Z;
  GLdouble        winx1, winy1, winz1;
  GLdouble        winx, winy, winz;
  GLint           status;

  /* display type of text */
  if (aspect_text->DisplayType() != Aspect_TODT_NORMAL)
  {
    /* Optimisation: il faudrait ne faire le Get qu'une fois par Redraw */
    glGetIntegerv (GL_VIEWPORT, viewport);
    glGetDoublev (GL_MODELVIEW_MATRIX, modelMatrix);
    glGetDoublev (GL_PROJECTION_MATRIX, projMatrix);

    switch (aspect_text->DisplayType())
    {
    case Aspect_TODT_BLEND:
      glEnable(GL_COLOR_LOGIC_OP);
      glLogicOp(GL_XOR);
      break;
    case Aspect_TODT_SUBTITLE:
    {
      int sWidth, sAscent, sDescent;
      AWorkspace->StringSize(myString, sWidth, sAscent, sDescent);

      objrefX = (float)myAttachPnt.xyz[0];   
      objrefY = (float)myAttachPnt.xyz[1];   
      objrefZ = (float)myAttachPnt.xyz[2];
      status = gluProject (objrefX, objrefY, objrefZ, modelMatrix, projMatrix, viewport,
        &winx1, &winy1, &winz1);

      winx = winx1;
      winy = winy1-sDescent;
      winz = winz1+0.00001;     
      status = gluUnProject (winx, winy, winz, modelMatrix, projMatrix, viewport,
        &objX, &objY, &objZ);

      winx = winx1 + sWidth;
      winy = winy1-sDescent;
      winz = winz1+0.00001; /* il vaut mieux F+B / 1000000 ? */     
      status = gluUnProject (winx, winy, winz, modelMatrix, projMatrix, viewport,
        &obj1X, &obj1Y, &obj1Z);

      winx = winx1 + sWidth;
      winy = winy1 + sAscent;
      winz = winz1+0.00001;     
      status = gluUnProject (winx, winy, winz, modelMatrix, projMatrix, viewport,
        &obj2X, &obj2Y, &obj2Z);

      winx = winx1;
      winy = winy1+ sAscent;
      winz = winz1+0.00001;   
      status = gluUnProject (winx, winy, winz, modelMatrix, projMatrix, viewport,
        &obj3X, &obj3Y, &obj3Z);

      glColor3fv( scolor->rgb );
      glBegin(GL_POLYGON);
      glVertex3d(objX, objY, objZ);
      glVertex3d(obj1X, obj1Y, obj1Z);
      glVertex3d(obj2X, obj2Y, obj2Z);
      glVertex3d(obj3X, obj3Y, obj3Z);
      glEnd();
      break;
    }

    case Aspect_TODT_DEKALE:
      objrefX = (float)myAttachPnt.xyz[0];   
      objrefY = (float)myAttachPnt.xyz[1];   
      objrefZ = (float)myAttachPnt.xyz[2];
      status = gluProject (objrefX, objrefY, objrefZ, modelMatrix, projMatrix, viewport,
        &winx1, &winy1, &winz1);

      winx = winx1+1;
      winy = winy1+1;
      winz = winz1+0.00001;     
      status = gluUnProject (winx, winy, winz, modelMatrix, projMatrix, viewport,
        &objX, &objY, &objZ);

      glColor3fv( scolor->rgb );
      AWorkspace->RenderText( myString, 0, (float)objX, (float)objY,(float)objZ );
      winx = winx1-1;
      winy = winy1-1;
      status = gluUnProject (winx, winy, winz, modelMatrix, projMatrix, viewport,
        &objX, &objY, &objZ);

      AWorkspace->RenderText( myString, 0, (float)objX, (float)objY,(float)objZ );
      winx = winx1-1;
      winy = winy1+1;
      status = gluUnProject (winx, winy, winz, modelMatrix, projMatrix, viewport,
        &objX, &objY, &objZ); 

      AWorkspace->RenderText( myString, 0, (float)objX, (float)objY,(float)objZ );
      winx = winx1+1;
      winy = winy1-1;
      status = gluUnProject (winx, winy, winz, modelMatrix, projMatrix, viewport,
        &objX, &objY, &objZ);
      AWorkspace->RenderText( myString, 0, (float)objX, (float)objY,(float)objZ );
      break;
    }
  }

  glColor3fv( tcolor->rgb );
  AWorkspace->RenderText( myString, 0, (float)myAttachPnt.xyz[0], (float)myAttachPnt.xyz[1],(float)myAttachPnt.xyz[2] );
  /* maj attributs */
  if (flag_zbuffer) glEnable(GL_DEPTH_TEST);
  if (aspect_text->DisplayType() == Aspect_TODT_BLEND)
  {
    glDisable(GL_COLOR_LOGIC_OP);
  }
}

/*----------------------------------------------------------------------*/
