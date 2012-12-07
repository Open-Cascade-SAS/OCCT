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
#include <TCollection_AsciiString.hxx>
#include <Handle_Graphic3d_TextureParams.hxx>

#include <OpenGl_AspectLine.hxx>
#include <OpenGl_Element.hxx>
#include <Handle_OpenGl_Texture.hxx>

#define OPENGL_AMBIENT_MASK  (1<<0)
#define OPENGL_DIFFUSE_MASK  (1<<1)
#define OPENGL_SPECULAR_MASK (1<<2)
#define OPENGL_EMISSIVE_MASK (1<<3)

class CALL_DEF_CONTEXTFILLAREA;

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

class OpenGl_AspectFace : public OpenGl_Element
{

public:

  OpenGl_AspectFace();

  void Init (const Handle(OpenGl_Context)&   theContext,
             const CALL_DEF_CONTEXTFILLAREA& theAspect);

  void SetAspectEdge (const OpenGl_AspectLine* theAspectEdge) { myAspectEdge = *theAspectEdge; }

  const OpenGl_AspectLine* AspectEdge() const { return &myAspectEdge; }

  virtual void Render  (const Handle(OpenGl_Workspace)& theWorkspace) const;
  virtual void Release (const Handle(OpenGl_Context)&   theContext);

private:

  void convertMaterial (const CALL_DEF_MATERIAL& theMat,
                        OPENGL_SURF_PROP&        theSurf);

public:

  Aspect_InteriorStyle    InteriorStyle;
  int                     Edge;
  int                     Hatch;
  int                     DistinguishingMode;
  int                     CullingMode;
  OPENGL_SURF_PROP        IntFront;
  OPENGL_SURF_PROP        IntBack;
  TEL_POFFSET_PARAM       PolygonOffset;

  int                     doTextureMap;
  Handle(OpenGl_Texture)  TextureRes;
  Handle(Graphic3d_TextureParams) TextureParams;

protected:

  TCollection_AsciiString myTextureId;
  OpenGl_AspectLine       myAspectEdge;

public:

  DEFINE_STANDARD_ALLOC

};

#endif //_OpenGl_AspectFace_Header
