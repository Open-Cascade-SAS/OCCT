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

#include <OpenGl_Marker.hxx>

#include <OpenGl_AspectMarker.hxx>
#include <OpenGl_Structure.hxx>
#include <OpenGl_Display.hxx>

/*----------------------------------------------------------------------*/

void OpenGl_Marker::Render (const Handle(OpenGl_Workspace) &AWorkspace) const
{
  const OpenGl_AspectMarker *aspect_marker = AWorkspace->AspectMarker( Standard_True );

  // Use highlight colours
  glColor3fv( (AWorkspace->NamedStatus & OPENGL_NS_HIGHLIGHT)? AWorkspace->HighlightColor->rgb : aspect_marker->Color().rgb );

  switch ( aspect_marker->Type() )
  {
    case Aspect_TOM_O_POINT :
    {
      const char *str = AWorkspace->GetDisplay()->GetStringForMarker( Aspect_TOM_O, aspect_marker->Scale() );
      glRasterPos3fv( myPoint.xyz );
      AWorkspace->GetDisplay()->SetBaseForMarker();
      glCallLists( strlen( str ), GL_UNSIGNED_BYTE, (const GLubyte *) str );
    }
    case Aspect_TOM_POINT :
    {
      glPointSize( aspect_marker->Scale() );
      glBegin( GL_POINTS );
      glVertex3fv( myPoint.xyz );
      glEnd();
      break;
    }
	default:
    {
      glRasterPos3fv( myPoint.xyz );
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
          while (AScale > ALimit && AScale >= 1.f)
          {
            const char *str = AWorkspace->GetDisplay()->GetStringForMarker( Aspect_TOM_O, AScale );
            glCallLists( strlen( str ), GL_UNSIGNED_BYTE, (const GLubyte *) str );
            AScale -= ADelta;
          }
          break;
        }
        case Aspect_TOM_USERDEFINED :
        {       
          glCallList( openglDisplay->GetUserMarkerListIndex( (int)aspect_marker->Scale() ) );
          break;
        }
        default :
        {
          AWorkspace->GetDisplay()->SetBaseForMarker();
          const char *str = AWorkspace->GetDisplay()->GetStringForMarker( aspect_marker->Type(), aspect_marker->Scale() );
          glCallLists( strlen( str ), GL_UNSIGNED_BYTE, (const GLubyte *)str );
        }
      }

      if (AWorkspace->GetGlContext()->IsFeedback())
      {
        glBegin( GL_POINTS );
        glVertex3fv( myPoint.xyz );
        glEnd();
        /* it is necessary to indicate end of marker information*/
      }
    }
  }
}

/*----------------------------------------------------------------------*/
