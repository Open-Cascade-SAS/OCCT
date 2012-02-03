// File:      OpenGl_AspectFace.hxx
// Created:   13 July 2011
// Author:    Sergey ZERCHANINOV
// Copyright: OPEN CASCADE 2011

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
  IMPLEMENT_MEMORY_OPERATORS
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
  IMPLEMENT_MEMORY_OPERATORS
};

#include <OpenGl_Element.hxx>

class OpenGl_AspectFace : public OpenGl_Element
{
 public:

  OpenGl_AspectFace ();
  virtual ~OpenGl_AspectFace () {}

  void SetContext (const CALL_DEF_CONTEXTFILLAREA &AContext);

  void SetContext (const TEL_CONTEXT_FACE &AContext) { myContext = AContext; }
  void SetAspectEdge (const OpenGl_AspectLine * AnAspectEdge) { myAspectEdge = *AnAspectEdge; }

  const TEL_CONTEXT_FACE & Context() const { return myContext; }
  const OpenGl_AspectLine * AspectEdge() const { return &myAspectEdge; }

  virtual void Render (const Handle(OpenGl_Workspace) &AWorkspace) const;

 protected:

  TEL_CONTEXT_FACE   myContext;
  OpenGl_AspectLine  myAspectEdge;

 public:
  IMPLEMENT_MEMORY_OPERATORS
};

#endif //_OpenGl_AspectFace_Header
