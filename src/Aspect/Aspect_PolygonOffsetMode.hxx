/* File:      Aspect_PolygonOffsetMode.hxx
 * Created:   3/22/2004 15:55:39
 * Author:    Sergey ANIKIN
 * Copyright: Open CASCADE 2004
 */

/*
 * Created: 22/03/04 ; SAN : OCC4895 High-level interface for controlling polygon offsets
 */

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

