// File:      OpenGl_Workspace_1.cxx
// Created:   20 September 2011
// Author:    Sergey ZERCHANINOV
// Copyright: OPEN CASCADE 2011

#include <OpenGl_Workspace.hxx>

//call_togl_begin_animation
void OpenGl_Workspace::BeginAnimation (const Standard_Boolean UseDegeneration, const Standard_Boolean UpdateAM)
{
  NamedStatus   |= OPENGL_NS_ANIMATION;
  if (UseDegeneration)
    NamedStatus |= OPENGL_NS_DEGENERATION;
  else
    NamedStatus &= ~OPENGL_NS_DEGENERATION;
  if (UpdateAM)
    NamedStatus |= OPENGL_NS_UPDATEAM;
  else
    NamedStatus &= ~OPENGL_NS_UPDATEAM;
  NamedStatus &= ~OPENGL_NS_FLIST;
}

/*----------------------------------------------------------------------*/

//call_togl_end_animation
void OpenGl_Workspace::EndAnimation ()
{
  if( (NamedStatus & OPENGL_NS_ANIMATION) == 0 ) return;

  NamedStatus &= ~(OPENGL_NS_ANIMATION | OPENGL_NS_DEGENERATION | OPENGL_NS_UPDATEAM);
  myIsUpdated = Standard_False;
}

/*----------------------------------------------------------------------*/
/*
* Permet de terminer effectivement le mode Animation.
* Ceci est realise des qu'une structure a ete modifiee.
*/

//call_togl_erase_animation_mode
void OpenGl_Workspace::EraseAnimation ()
{
  NamedStatus &= ~(OPENGL_NS_ANIMATION | OPENGL_NS_DEGENERATION | OPENGL_NS_UPDATEAM | OPENGL_NS_FLIST);
}

/*----------------------------------------------------------------------*/
