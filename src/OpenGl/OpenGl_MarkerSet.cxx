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
#include <OpenGl_Context.hxx>

#include <OpenGl_MarkerSet.hxx>

#include <OpenGl_AspectMarker.hxx>
#include <OpenGl_Structure.hxx>
#include <OpenGl_Display.hxx>

/*----------------------------------------------------------------------*/

OpenGl_MarkerSet::OpenGl_MarkerSet (const Standard_Integer ANbPoints, const Graphic3d_Vertex *APoints)
: myNbPoints(ANbPoints),
  myPoints(new TEL_POINT[ANbPoints])
{
  memcpy( myPoints, APoints, ANbPoints*sizeof(TEL_POINT) );
}

/*----------------------------------------------------------------------*/

OpenGl_MarkerSet::~OpenGl_MarkerSet ()
{
  if (myPoints)
    delete[] myPoints;
}

/*----------------------------------------------------------------------*/

void OpenGl_MarkerSet::Render (const Handle(OpenGl_Workspace) &AWorkspace) const
{
  const OpenGl_AspectMarker *aspect_marker = AWorkspace->AspectMarker( Standard_True );

  // Use highlight colors
  glColor3fv( (AWorkspace->NamedStatus & OPENGL_NS_HIGHLIGHT)? AWorkspace->HighlightColor->rgb : aspect_marker->Color().rgb );

  const TEL_POINT *ptr;
  Tint i;

  switch ( aspect_marker->Type() )
  {
    case Aspect_TOM_O_POINT :
    {
      const char *str = AWorkspace->GetDisplay()->GetStringForMarker( Aspect_TOM_O, aspect_marker->Scale() );
      AWorkspace->GetDisplay()->SetBaseForMarker();
      for( i = 0, ptr = myPoints; i < myNbPoints; i++, ptr++ )
      {
        glRasterPos3fv( ptr->xyz );
        glCallLists( strlen( str ), GL_UNSIGNED_BYTE, (const GLubyte *) str );
      }
    }
    case Aspect_TOM_POINT :
    {
      glBegin( GL_POINTS );
      for( i = 0, ptr = myPoints; i < myNbPoints; i++, ptr++ )
        glVertex3fv( ptr->xyz );
      glEnd();
      break;
    }
    default:
    {
      switch ( aspect_marker->Type() )
      {
        case Aspect_TOM_RING1 :
        case Aspect_TOM_RING2 :
        case Aspect_TOM_RING3 :
        {
          const float ADelta = 0.1f;
          float AScale = aspect_marker->Scale();
          float ALimit = 0.f;
          if (aspect_marker->Type() == Aspect_TOM_RING1)
            ALimit = AScale * 0.2f;
          else if (aspect_marker->Type() == Aspect_TOM_RING2)
            ALimit = AScale * 0.5f;
          else
            ALimit = AScale * 0.8f;
          AWorkspace->GetDisplay()->SetBaseForMarker();
          for( i = 0, ptr = myPoints; i < myNbPoints; i++, ptr++ )
          {
            glRasterPos3fv( ptr->xyz );
            AScale = aspect_marker->Scale();
            while (AScale > ALimit && AScale >= 1.f)
            {
              const char *str = AWorkspace->GetDisplay()->GetStringForMarker( Aspect_TOM_O, AScale );
              glCallLists( strlen( str ), GL_UNSIGNED_BYTE, (const GLubyte *) str );
              AScale -= ADelta;
            }
          }
          break;
        }
        case Aspect_TOM_USERDEFINED :
        {
          for( i = 0, ptr = myPoints; i < myNbPoints; i++, ptr++ )
          {
            glRasterPos3fv( ptr->xyz );
            glCallList( openglDisplay->GetUserMarkerListIndex( (int)aspect_marker->Scale() ) );
          }
          break;
        }
        default :
        {
          AWorkspace->GetDisplay()->SetBaseForMarker();
          const char *str = AWorkspace->GetDisplay()->GetStringForMarker( aspect_marker->Type(), aspect_marker->Scale() );
          for( i = 0, ptr = myPoints; i < myNbPoints; i++, ptr++ )
          {
            glRasterPos3fv( ptr->xyz );
            glCallLists( strlen( str ), GL_UNSIGNED_BYTE, (const GLubyte *) str );
          }
        }
      }

      if (AWorkspace->GetGlContext()->IsFeedback())
      {
        glBegin( GL_POINTS );
        for( i = 0, ptr = myPoints; i < myNbPoints; i++, ptr++ )
          glVertex3fv( ptr->xyz );
        glEnd();
      }
    }
  }
}

void OpenGl_MarkerSet::Release (const Handle(OpenGl_Context)& theContext)
{
  //
}
