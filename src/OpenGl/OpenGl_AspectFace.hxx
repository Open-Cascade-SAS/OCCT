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


#ifndef _OpenGl_AspectFace_Header
#define _OpenGl_AspectFace_Header

#include <InterfaceGraphic_telem.hxx>
#include <Aspect_InteriorStyle.hxx>

#include <OpenGl_AspectLine.hxx>

#define OPENGL_AMBIENT_MASK  (1<<0)
#define OPENGL_DIFFUSE_MASK  (1<<1)
#define OPENGL_SPECULAR_MASK (1<<2)
#define OPENGL_EMISSIVE_MASK (1<<3)

struct OPENGL_SURF_PROP
{
  float        amb, diff, spec, emsv;
  float        trans, shine;
  float        env_reflexion;
  int          isphysic;
  unsigned int color_mask;
  TEL_COLOUR speccol, difcol, ambcol, emscol, matcol;
  DEFINE_STANDARD_ALLOC
};

struct TEL_CONTEXT_FACE
{
  Aspect_InteriorStyle InteriorStyle;
  int                  Edge;
  int                  Hatch;
  int                  DistinguishingMode;
  int                  CullingMode;
  OPENGL_SURF_PROP     IntFront;
  OPENGL_SURF_PROP     IntBack;
  int                  doTextureMap;
  int                  TexId;
  TEL_POFFSET_PARAM PolygonOffset;
  DEFINE_STANDARD_ALLOC
};

#include <OpenGl_Element.hxx>

class OpenGl_AspectFace : public OpenGl_Element
{
 public:

  OpenGl_AspectFace ();

  void SetContext (const CALL_DEF_CONTEXTFILLAREA &AContext);

  void SetContext (const TEL_CONTEXT_FACE &AContext) { myContext = AContext; }
  void SetAspectEdge (const OpenGl_AspectLine * AnAspectEdge) { myAspectEdge = *AnAspectEdge; }

  const TEL_CONTEXT_FACE & Context() const { return myContext; }
  const OpenGl_AspectLine * AspectEdge() const { return &myAspectEdge; }

  virtual void Render  (const Handle(OpenGl_Workspace)& theWorkspace) const;
  virtual void Release (const Handle(OpenGl_Context)&   theContext);

 protected:

  TEL_CONTEXT_FACE   myContext;
  OpenGl_AspectLine  myAspectEdge;

 public:
  DEFINE_STANDARD_ALLOC
};

#endif //_OpenGl_AspectFace_Header
