// File:      OpenGl_MarkerSet.cxx
// Created:   13 July 2011
// Author:    Sergey ZERCHANINOV
// Copyright: OPEN CASCADE 2011

#include <OpenGl_GlCore11.hxx>

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
      glPointSize( aspect_marker->Scale() );
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
      GLint mode;
      glGetIntegerv( GL_RENDER_MODE, &mode );
      if( mode==GL_FEEDBACK )
      {
        glBegin( GL_POINTS );
        for( i = 0, ptr = myPoints; i < myNbPoints; i++, ptr++ )
          glVertex3fv( ptr->xyz );
        glEnd();
      }
    }
  }
}

/*----------------------------------------------------------------------*/
