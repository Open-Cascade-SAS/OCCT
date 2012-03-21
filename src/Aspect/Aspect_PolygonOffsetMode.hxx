// Created on: 2004-03-22
// Created by: Sergey ANIKIN
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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

#ifndef ASPECT_POLYGONOFFSETMODE_HEADER
#define ASPECT_POLYGONOFFSETMODE_HEADER

typedef enum 
{
  Aspect_POM_Off   = 0x00,  /* all polygon offset modes disabled                     */
  Aspect_POM_Fill  = 0x01,  /* GL_POLYGON_OFFSET_FILL enabled (shaded polygons)      */
  Aspect_POM_Line  = 0x02,  /* GL_POLYGON_OFFSET_LINE enabled (polygons as outlines) */
  Aspect_POM_Point = 0x04,  /* GL_POLYGON_OFFSET_POINT enabled (polygons as vertices)*/
  Aspect_POM_All   = Aspect_POM_Fill | Aspect_POM_Line | Aspect_POM_Point,
  Aspect_POM_None  = 0x08,  /* do not change current polygon offset mode             */
  Aspect_POM_Mask  = Aspect_POM_All  | Aspect_POM_None
} Aspect_PolygonOffsetMode;

#endif

