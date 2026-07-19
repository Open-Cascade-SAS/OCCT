// Created on: 2012-03-06
// Created by: Kirill GAVRILOV
// Copyright (c) 2012-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef OpenGl_GlCore11_HeaderFile
#define OpenGl_GlCore11_HeaderFile

#include <OpenGl_GlCore11Fwd.hxx>

#ifndef GL_COMPILE
  #define GL_COMPILE 0x1300
  #define GL_COMPILE_AND_EXECUTE 0x1301

  #define GL_RENDER_MODE 0x0C40
  #define GL_RENDER 0x1C00
  #define GL_FEEDBACK 0x1C01
  #define GL_SELECT 0x1C02

  #define GL_SHADE_MODEL 0x0B54
  #define GL_FLAT 0x1D00
  #define GL_SMOOTH 0x1D01
  #define GL_POINT 0x1B00
  #define GL_LINE 0x1B01
  #define GL_FILL 0x1B02

  #define GL_LINE_STIPPLE 0x0B24
  #define GL_LINE_STIPPLE_PATTERN 0x0B25
  #define GL_LINE_STIPPLE_REPEAT 0x0B26

  #define GL_POLYGON_MODE 0x0B40
  #define GL_POLYGON_SMOOTH 0x0B41
  #define GL_POLYGON_STIPPLE 0x0B42

  #define GL_MATRIX_MODE 0x0BA0
  #define GL_NORMALIZE 0x0BA1

  #define GL_TEXTURE_1D 0x0DE0

  #define GL_MAX_CLIP_PLANES 0x0D32
  #define GL_CLIP_PLANE0 0x3000
  #define GL_CLIP_PLANE1 0x3001
  #define GL_CLIP_PLANE2 0x3002
  #define GL_CLIP_PLANE3 0x3003
  #define GL_CLIP_PLANE4 0x3004
  #define GL_CLIP_PLANE5 0x3005

  #define GL_QUADS 0x0007
  #define GL_QUAD_STRIP 0x0008
  #define GL_POLYGON 0x0009

  #define GL_LIGHTING 0x0B50
  #define GL_LIGHT_MODEL_LOCAL_VIEWER 0x0B51
  #define GL_LIGHT_MODEL_TWO_SIDE 0x0B52
  #define GL_LIGHT_MODEL_AMBIENT 0x0B53
  #define GL_COLOR_MATERIAL 0x0B57
  #define GL_LIGHT0 0x4000
  #define GL_LIGHT1 0x4001
  #define GL_LIGHT2 0x4002
  #define GL_LIGHT3 0x4003
  #define GL_LIGHT4 0x4004
  #define GL_LIGHT5 0x4005
  #define GL_LIGHT6 0x4006
  #define GL_LIGHT7 0x4007

  // LightParameter
  #define GL_AMBIENT 0x1200
  #define GL_DIFFUSE 0x1201
  #define GL_SPECULAR 0x1202
  #define GL_POSITION 0x1203
  #define GL_SPOT_DIRECTION 0x1204
  #define GL_SPOT_EXPONENT 0x1205
  #define GL_SPOT_CUTOFF 0x1206
  #define GL_CONSTANT_ATTENUATION 0x1207
  #define GL_LINEAR_ATTENUATION 0x1208
  #define GL_QUADRATIC_ATTENUATION 0x1209

  #define GL_EMISSION 0x1600
  #define GL_SHININESS 0x1601
  #define GL_AMBIENT_AND_DIFFUSE 0x1602
  #define GL_COLOR_INDEXES 0x1603

  // MatrixMode
  #define GL_MODELVIEW 0x1700
  #define GL_PROJECTION 0x1701
  #define GL_TEXTURE 0x1702

  #define GL_POINT_SMOOTH_HINT 0x0C51
  #define GL_LINE_SMOOTH_HINT 0x0C52
  #define GL_POLYGON_SMOOTH_HINT 0x0C53
  #define GL_FOG_HINT 0x0C54
  #define GL_FOG 0x0B60

  #define GL_LOGIC_OP_MODE 0x0BF0
  #define GL_INDEX_LOGIC_OP 0x0BF1
  #define GL_LOGIC_OP GL_INDEX_LOGIC_OP
  #define GL_COLOR_LOGIC_OP 0x0BF2
  #define GL_CLEAR 0x1500
  #define GL_AND 0x1501
  #define GL_AND_REVERSE 0x1502
  #define GL_COPY 0x1503
  #define GL_AND_INVERTED 0x1504
  #define GL_NOOP 0x1505
  #define GL_XOR 0x1506
  #define GL_OR 0x1507
  #define GL_NOR 0x1508
  #define GL_EQUIV 0x1509
  #define GL_INVERT 0x150A
  #define GL_OR_REVERSE 0x150B
  #define GL_COPY_INVERTED 0x150C
  #define GL_OR_INVERTED 0x150D
  #define GL_NAND 0x150E
  #define GL_SET 0x150F

  #define GL_VERTEX_ARRAY 0x8074
  #define GL_NORMAL_ARRAY 0x8075
  #define GL_COLOR_ARRAY 0x8076
  #define GL_INDEX_ARRAY 0x8077
  #define GL_TEXTURE_COORD_ARRAY 0x8078

  #define GL_PROXY_TEXTURE_1D 0x8063

  #define GL_ALPHA_TEST 0x0BC0
  #define GL_ALPHA_TEST_FUNC 0x0BC1
  #define GL_ALPHA_TEST_REF 0x0BC2

  // TextureCoordName
  #define GL_S 0x2000
  #define GL_T 0x2001
  #define GL_R 0x2002
  #define GL_Q 0x2003

  // TextureEnvMode
  #define GL_MODULATE 0x2100
  #define GL_DECAL 0x2101

  // TextureEnvParameter
  #define GL_TEXTURE_ENV_MODE 0x2200
  #define GL_TEXTURE_ENV_COLOR 0x2201

  // TextureEnvTarget
  #define GL_TEXTURE_ENV 0x2300

  // TextureGenMode
  #define GL_EYE_LINEAR 0x2400
  #define GL_OBJECT_LINEAR 0x2401
  #define GL_SPHERE_MAP 0x2402

  // TextureGenParameter
  #define GL_TEXTURE_GEN_MODE 0x2500
  #define GL_OBJECT_PLANE 0x2501
  #define GL_EYE_PLANE 0x2502

  #define GL_TEXTURE_GEN_S 0x0C60
  #define GL_TEXTURE_GEN_T 0x0C61
  #define GL_TEXTURE_GEN_R 0x0C62
  #define GL_TEXTURE_GEN_Q 0x0C63

  #define GL_MAP_COLOR 0x0D10
  #define GL_MAP_STENCIL 0x0D11
  #define GL_RED_SCALE 0x0D14
  #define GL_RED_BIAS 0x0D15
  #define GL_GREEN_SCALE 0x0D18
  #define GL_GREEN_BIAS 0x0D19
  #define GL_BLUE_SCALE 0x0D1A
  #define GL_BLUE_BIAS 0x0D1B
  #define GL_ALPHA_SCALE 0x0D1C
  #define GL_ALPHA_BIAS 0x0D1D
  #define GL_DEPTH_SCALE 0x0D1E
  #define GL_DEPTH_BIAS 0x0D1F

  #define GL_STACK_OVERFLOW 0x0503
  #define GL_STACK_UNDERFLOW 0x0504
#endif

//! OpenGL 1.1 core.
//! Notice that all functions within this structure are actually exported by system GL library.
//! The main purpose for these hint - to control visibility of functions per GL version
//! (global functions should not be used directly to achieve this effect!).
struct OpenGl_GlCore11 : protected OpenGl_GlFunctions
{

public:
  using OpenGl_GlFunctions::glGetTexEnviv;
  using OpenGl_GlFunctions::glTexEnvi;

public: //! @name Begin/End primitive specification (removed since 3.1)
  using OpenGl_GlFunctions::glColor4fv;

public: //! @name Matrix operations (removed since 3.1)
  using OpenGl_GlFunctions::glLoadIdentity;
  using OpenGl_GlFunctions::glLoadMatrixf;
  using OpenGl_GlFunctions::glMatrixMode;

public: //! @name Line and Polygon stipple (removed since 3.1)
  using OpenGl_GlFunctions::glLineStipple;
  using OpenGl_GlFunctions::glPolygonStipple;

public: //! @name Fixed pipeline lighting (removed since 3.1)
  using OpenGl_GlFunctions::glColorMaterial;
  using OpenGl_GlFunctions::glLightf;
  using OpenGl_GlFunctions::glLightfv;
  using OpenGl_GlFunctions::glLightModelfv;
  using OpenGl_GlFunctions::glLightModeli;
  using OpenGl_GlFunctions::glMaterialf;
  using OpenGl_GlFunctions::glMaterialfv;
  using OpenGl_GlFunctions::glShadeModel;

public: //! @name clipping plane (removed since 3.1)
  using OpenGl_GlFunctions::glClipPlane;

public: //! @name Display lists (removed since 3.1)
  using OpenGl_GlFunctions::glCallList;
  using OpenGl_GlFunctions::glCallLists;
  using OpenGl_GlFunctions::glDeleteLists;
  using OpenGl_GlFunctions::glEndList;
  using OpenGl_GlFunctions::glGenLists;
  using OpenGl_GlFunctions::glListBase;
  using OpenGl_GlFunctions::glNewList;

public: //! @name Current raster position and Rectangles (removed since 3.1)
  using OpenGl_GlFunctions::glRasterPos2i;
  using OpenGl_GlFunctions::glRasterPos3fv;

public: //! @name Texture mapping (removed since 3.1)
  using OpenGl_GlFunctions::glTexGenfv;
  using OpenGl_GlFunctions::glTexGeni;

public: //! @name Pixel copying (removed since 3.1)
  using OpenGl_GlFunctions::glBitmap;
  using OpenGl_GlFunctions::glCopyPixels;
  using OpenGl_GlFunctions::glDrawPixels;

public: //! @name Edge flags and fixed-function vertex processing (removed since 3.1)
  using OpenGl_GlFunctions::glColorPointer;
  using OpenGl_GlFunctions::glDisableClientState;
  using OpenGl_GlFunctions::glEnableClientState;
  using OpenGl_GlFunctions::glIndexPointer;
  using OpenGl_GlFunctions::glNormalPointer;
  using OpenGl_GlFunctions::glPixelTransferi;
  using OpenGl_GlFunctions::glTexCoordPointer;
  using OpenGl_GlFunctions::glVertexPointer;
};

#endif // _OpenGl_GlCore11_Header
