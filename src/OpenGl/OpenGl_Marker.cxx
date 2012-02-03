// File:      OpenGl_Marker.cxx
// Created:   13 July 2011
// Author:    Sergey ZERCHANINOV
// Copyright: OPEN CASCADE 2011

#include <OpenGl_Marker.hxx>

#include <OpenGl_tgl_all.hxx>
#include <GL/gl.h>

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
      GLint mode;
      glGetIntegerv( GL_RENDER_MODE, &mode );
      if( mode==GL_FEEDBACK )
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
