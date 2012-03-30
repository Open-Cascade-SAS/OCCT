// Created on: 2011-09-20
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
