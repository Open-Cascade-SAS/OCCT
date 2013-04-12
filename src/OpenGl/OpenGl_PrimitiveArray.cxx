// Created on: 2011-07-13
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2013 OPEN CASCADE SAS
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

#include <OpenGl_IndexBuffer.hxx>
#include <OpenGl_Context.hxx>

#include <OpenGl_PrimitiveArray.hxx>

#include <OpenGl_AspectFace.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <OpenGl_Structure.hxx>
#include <OpenGl_Workspace.hxx>

#include <InterfaceGraphic_PrimitiveArray.hxx>

// =======================================================================
// function : clearMemoryOwn
// purpose  :
// =======================================================================
void OpenGl_PrimitiveArray::clearMemoryOwn() const
{
  Standard::Free ((Standard_Address& )myPArray->edges);
  Standard::Free ((Standard_Address& )myPArray->vertices);
  Standard::Free ((Standard_Address& )myPArray->vcolours);
  Standard::Free ((Standard_Address& )myPArray->vnormals);
  Standard::Free ((Standard_Address& )myPArray->vtexels);
  Standard::Free ((Standard_Address& )myPArray->edge_vis); /// ???

  myPArray->edges    = NULL;
  myPArray->vertices = NULL;
  myPArray->vcolours = NULL;
  myPArray->vnormals = NULL;
  myPArray->vtexels  = NULL;
  myPArray->edge_vis = NULL;
}

// =======================================================================
// function : clearMemoryGL
// purpose  :
// =======================================================================
void OpenGl_PrimitiveArray::clearMemoryGL (const Handle(OpenGl_Context)& theGlCtx) const
{
  for (Standard_Integer anIter = 0; anIter < VBOMaxType; ++anIter)
  {
    if (!myVbos[anIter].IsNull())
    {
      myVbos[anIter]->Release (theGlCtx.operator->());
      myVbos[anIter].Nullify();
    }
  }
}

// =======================================================================
// function : BuildVBO
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_PrimitiveArray::BuildVBO (const Handle(OpenGl_Workspace)& theWorkspace) const
{
  const Handle(OpenGl_Context)& aGlCtx = theWorkspace->GetGlContext();
  if (myPArray->vertices == NULL)
  {
    // vertices should be always defined - others are optional
    return Standard_False;
  }
  myVbos[VBOVertices] = new OpenGl_VertexBuffer();
  if (!myVbos[VBOVertices]->Init (aGlCtx, 3, myPArray->num_vertexs, &myPArray->vertices[0].xyz[0]))
  {
    clearMemoryGL (aGlCtx);
    return Standard_False;
  }

  if (myPArray->edges != NULL)
  {
    myVbos[VBOEdges] = new OpenGl_IndexBuffer();
    if (!myVbos[VBOEdges]->Init (aGlCtx, 1, myPArray->num_edges, (GLuint* )myPArray->edges))
    {
      clearMemoryGL (aGlCtx);
      return Standard_False;
    }
  }
  if (myPArray->vcolours != NULL)
  {
    myVbos[VBOVcolours] = new OpenGl_VertexBuffer();
    if (!myVbos[VBOVcolours]->Init (aGlCtx, 4, myPArray->num_vertexs, (GLubyte* )myPArray->vcolours))
    {
      clearMemoryGL (aGlCtx);
      return Standard_False;
    }
  }
  if (myPArray->vnormals != NULL)
  {
    myVbos[VBOVnormals] = new OpenGl_VertexBuffer();
    if (!myVbos[VBOVnormals]->Init (aGlCtx, 3, myPArray->num_vertexs, &myPArray->vnormals[0].xyz[0]))
    {
      clearMemoryGL (aGlCtx);
      return Standard_False;
    }
  }
  if (myPArray->vtexels)
  {
    myVbos[VBOVtexels] = new OpenGl_VertexBuffer();
    if (!myVbos[VBOVtexels]->Init (aGlCtx, 2, myPArray->num_vertexs, &myPArray->vtexels[0].xy[0]))
    {
      clearMemoryGL (aGlCtx);
      return Standard_False;
    }
  }

  clearMemoryOwn();
  return Standard_True;
}

// =======================================================================
// function : DrawArray
// purpose  :
// =======================================================================
void OpenGl_PrimitiveArray::DrawArray (Tint theLightingModel,
                                       const Aspect_InteriorStyle theInteriorStyle,
                                       Tint theEdgeFlag,
                                       const TEL_COLOUR* theInteriorColour,
                                       const TEL_COLOUR* theLineColour,
                                       const TEL_COLOUR* theEdgeColour,
                                       const OPENGL_SURF_PROP* theFaceProp,
                                       const Handle(OpenGl_Workspace)& theWorkspace) const
{
  const Handle(OpenGl_Context)& aGlContext = theWorkspace->GetGlContext();

  Tint i,n;
  Tint transp = 0;
  // Following pointers have been provided for performance improvement
  tel_colour pfc = myPArray->fcolours;
  Tint* pvc = myPArray->vcolours;
  if (pvc != NULL)
  {
    for (i = 0; i < myPArray->num_vertexs; ++i)
    {
      transp = int(theFaceProp->trans * 255.0f);
    #if defined (sparc) || defined (__sparc__) || defined (__sparc)
      pvc[i] = (pvc[i] & 0xffffff00);
      pvc[i] += transp;
    #else
      pvc[i] = (pvc[i] & 0x00ffffff);
      pvc[i] += transp << 24;
    #endif
    }
  }

  switch (myPArray->type)
  {
    case TelPointsArrayType:
    case TelPolylinesArrayType:
    case TelSegmentsArrayType:
      glColor3fv (theLineColour->rgb);
      break;
    case TelPolygonsArrayType:
    case TelTrianglesArrayType:
    case TelQuadranglesArrayType:
    case TelTriangleStripsArrayType:
    case TelQuadrangleStripsArrayType:
    case TelTriangleFansArrayType:
      glColor3fv (theInteriorColour->rgb);
      break;
  }

  // Temporarily disable environment mapping
  if (myDrawMode <= GL_LINE_STRIP)
  {
    glPushAttrib (GL_ENABLE_BIT);
    glDisable (GL_TEXTURE_1D);
    glDisable (GL_TEXTURE_2D);
  }

  if ((myDrawMode >  GL_LINE_STRIP && theInteriorStyle != Aspect_IS_EMPTY) ||
      (myDrawMode <= GL_LINE_STRIP))
  {
    if (theWorkspace->NamedStatus & OPENGL_NS_HIGHLIGHT)
    {
      pfc = NULL;
      pvc = NULL;
    }

    if (theInteriorStyle == Aspect_IS_HIDDENLINE)
    {
      theEdgeFlag = 1;
      pfc = NULL;
      pvc = NULL;
    }

    // Sometimes the GL_LIGHTING mode is activated here
    // without glEnable(GL_LIGHTING) call for an unknown reason, so it is necessary
    // to call glEnable(GL_LIGHTING) to synchronize Light On/Off mechanism*
    if (theLightingModel == 0 || myDrawMode <= GL_LINE_STRIP)
      glDisable (GL_LIGHTING);
    else
      glEnable (GL_LIGHTING);

    if (!toDrawVbo())
    {
      if (myPArray->vertices != NULL)
      {
        glVertexPointer (3, GL_FLOAT, 0, myPArray->vertices); // array of vertices
        glEnableClientState (GL_VERTEX_ARRAY);
      }
      if (myPArray->vnormals != NULL)
      {
        glNormalPointer (GL_FLOAT, 0, myPArray->vnormals); // array of normals
        glEnableClientState (GL_NORMAL_ARRAY);
      }
      if (myPArray->vtexels != NULL)
      {
        glTexCoordPointer (2, GL_FLOAT, 0, myPArray->vtexels); // array of texture coordinates
        glEnableClientState (GL_TEXTURE_COORD_ARRAY);
      }

      if (pvc != NULL)
      {
        glColorPointer (4, GL_UNSIGNED_BYTE, 0, pvc);  // array of colors
        glEnableClientState (GL_COLOR_ARRAY);
        glColorMaterial (GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
        glEnable (GL_COLOR_MATERIAL);
      }
    }
    else
    {
      // Bindings concrete pointer in accordance with VBO buffer
      myVbos[VBOVertices]->BindFixed (aGlContext, GL_VERTEX_ARRAY);
      if (!myVbos[VBOVnormals].IsNull())
      {
        myVbos[VBOVnormals]->BindFixed (aGlContext, GL_NORMAL_ARRAY);
      }
      if (!myVbos[VBOVtexels].IsNull() && (theWorkspace->NamedStatus & OPENGL_NS_FORBIDSETTEX) == 0)
      {
        myVbos[VBOVtexels]->BindFixed (aGlContext, GL_TEXTURE_COORD_ARRAY);
      }
      if (!myVbos[VBOVcolours].IsNull())
      {
        myVbos[VBOVcolours]->BindFixed (aGlContext, GL_COLOR_ARRAY);
        glColorMaterial (GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        glEnable (GL_COLOR_MATERIAL);
      }
    }

    /// OCC22236 NOTE: draw for all situations:
    /// 1) draw elements from myPArray->bufferVBO[VBOEdges] indicies array
    /// 2) draw elements from vertice array, when bounds defines count of primitive's verts.
    /// 3) draw primitive by vertexes if no edges and bounds array is specified
    if (toDrawVbo())
    {
      if (!myVbos[VBOEdges].IsNull())
      {
        myVbos[VBOEdges]->Bind (aGlContext);
        if (myPArray->num_bounds > 0)
        {
          // draw primitives by vertex count with the indicies
          Tint* anOffset = NULL;
          for (i = 0; i < myPArray->num_bounds; ++i)
          {
            if (pfc != NULL) glColor3fv (pfc[i].rgb);
            glDrawElements (myDrawMode, myPArray->bounds[i], myVbos[VBOEdges]->GetDataType(), anOffset);
            anOffset += myPArray->bounds[i];
          }
        }
        else
        {
          // draw one (or sequential) primitive by the indicies
          glDrawElements (myDrawMode, myPArray->num_edges, myVbos[VBOEdges]->GetDataType(), NULL);
        }
        myVbos[VBOEdges]->Unbind (aGlContext);
      }
      else if (myPArray->num_bounds > 0)
      {
        for (i = n = 0; i < myPArray->num_bounds; ++i)
        {
          if (pfc != NULL) glColor3fv (pfc[i].rgb);
          glDrawArrays (myDrawMode, n, myPArray->bounds[i]);
          n += myPArray->bounds[i];
        }
      }
      else
      {
        glDrawArrays (myDrawMode, 0, myVbos[VBOVertices]->GetElemsNb());
      }

      // bind with 0
      myVbos[VBOVertices]->UnbindFixed (aGlContext, GL_VERTEX_ARRAY);
      if (!myVbos[VBOVnormals].IsNull())
      {
        myVbos[VBOVnormals]->UnbindFixed (aGlContext, GL_NORMAL_ARRAY);
      }
      if (!myVbos[VBOVtexels].IsNull() && (theWorkspace->NamedStatus & OPENGL_NS_FORBIDSETTEX) == 0)
      {
        myVbos[VBOVtexels]->UnbindFixed (aGlContext, GL_TEXTURE_COORD_ARRAY);
      }
      if (!myVbos[VBOVcolours].IsNull())
      {
        myVbos[VBOVcolours]->UnbindFixed (aGlContext, GL_COLOR_ARRAY);
        glDisable (GL_COLOR_MATERIAL);
        theWorkspace->NamedStatus |= OPENGL_NS_RESMAT; // Reset material
      }
    }
    else
    {
      if (myPArray->num_bounds > 0)
      {
        if (myPArray->num_edges > 0)
        {
          for (i = n = 0; i < myPArray->num_bounds; ++i)
          {
            if (pfc != NULL) glColor3fv (pfc[i].rgb);
            glDrawElements (myDrawMode, myPArray->bounds[i], GL_UNSIGNED_INT, (GLenum* )&myPArray->edges[n]);
            n += myPArray->bounds[i];
          }
        }
        else
        {
          for (i = n = 0; i < myPArray->num_bounds; ++i)
          {
            if (pfc != NULL) glColor3fv (pfc[i].rgb);
            glDrawArrays (myDrawMode, n, myPArray->bounds[i]);
            n += myPArray->bounds[i];
          }
        }
      }
      else if (myPArray->num_edges > 0)
      {
        glDrawElements (myDrawMode, myPArray->num_edges, GL_UNSIGNED_INT, (GLenum* )myPArray->edges);
      }
      else
      {
        glDrawArrays (myDrawMode, 0, myPArray->num_vertexs);
      }

      if (pvc != NULL)
      {
        glDisable (GL_COLOR_MATERIAL);
        theWorkspace->NamedStatus |= OPENGL_NS_RESMAT; // Reset material
      }

      glDisableClientState (GL_VERTEX_ARRAY);
      if (myPArray->vcolours != NULL)
        glDisableClientState (GL_COLOR_ARRAY);
      if (myPArray->vnormals != NULL)
        glDisableClientState (GL_NORMAL_ARRAY);
      if (myPArray->vtexels != NULL)
        glDisableClientState (GL_TEXTURE_COORD_ARRAY);
    }
  }

  // On some NVIDIA graphic cards, using glEdgeFlagPointer() in
  // combination with VBO (edge flag data put into a VBO buffer)
  // leads to a crash in a driver. Therefore, edge flags are simply
  // igonored when VBOs are enabled, so all the edges are drawn if
  // edge visibility is turned on. In order to draw edges selectively,
  // either disable VBO or turn off edge visibilty in the current
  // primitive array and create a separate primitive array (segments)
  // and put edges to be drawn into it.
  if (theEdgeFlag && myDrawMode > GL_LINE_STRIP)
  {
    DrawEdges (theEdgeColour, theWorkspace);
  }

  if (myDrawMode <= GL_LINE_STRIP)
    glPopAttrib();
}

// =======================================================================
// function : DrawEdges
// purpose  :
// =======================================================================
void OpenGl_PrimitiveArray::DrawEdges (const TEL_COLOUR*               theEdgeColour,
                                       const Handle(OpenGl_Workspace)& theWorkspace) const
{
  glDisable (GL_LIGHTING);

  const Handle(OpenGl_Context)& aGlContext = theWorkspace->GetGlContext();
  const OpenGl_AspectLine* anAspectLineOld = NULL;
  if (myDrawMode > GL_LINE_STRIP)
  {
    anAspectLineOld = theWorkspace->SetAspectLine (theWorkspace->AspectFace (Standard_True)->AspectEdge());
    theWorkspace->AspectLine (Standard_True);

    glPushAttrib (GL_POLYGON_BIT);
    glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
  }

  Tint i, j, n;

  /// OCC22236 NOTE: draw edges for all situations:
  /// 1) draw elements with GL_LINE style as edges from myPArray->bufferVBO[VBOEdges] indicies array
  /// 2) draw elements from vertice array, when bounds defines count of primitive's verts.
  /// 3) draw primitive's edges by vertexes if no edges and bounds array is specified
  if (toDrawVbo())
  {
    myVbos[VBOVertices]->BindFixed (aGlContext, GL_VERTEX_ARRAY);
    glColor3fv (theEdgeColour->rgb);
    if (!myVbos[VBOEdges].IsNull())
    {
      myVbos[VBOEdges]->Bind (aGlContext);

      // draw primitives by vertex count with the indicies
      if (myPArray->num_bounds > 0)
      {
        Tint* offset = 0;
        for (i = 0, offset = 0; i < myPArray->num_bounds; ++i)
        {
          glDrawElements (myDrawMode, myPArray->bounds[i], myVbos[VBOEdges]->GetDataType(), offset);
          offset += myPArray->bounds[i];
        }
      }
      // draw one (or sequential) primitive by the indicies
      else
      {
        glDrawElements (myDrawMode, myVbos[VBOEdges]->GetElemsNb(), myVbos[VBOEdges]->GetDataType(), NULL);
      }
      myVbos[VBOEdges]->Unbind (aGlContext);
    }
    else if (myPArray->num_bounds > 0)
    {
      for (i = n = 0; i < myPArray->num_bounds; ++i)
      {
        glDrawArrays (myDrawMode, n, myPArray->bounds[i]);
        n += myPArray->bounds[i];
      }
    }
    else
    {
      glDrawArrays (myDrawMode, 0, myPArray->num_vertexs);
    }

    // unbind buffers
    myVbos[VBOVertices]->UnbindFixed (aGlContext, GL_VERTEX_ARRAY);
  }
  else
  {
    glEnableClientState (GL_VERTEX_ARRAY);
    glVertexPointer (3, GL_FLOAT, 0, myPArray->vertices); // array of vertices

    glColor3fv (theEdgeColour->rgb);
    if (myPArray->num_bounds > 0)
    {
      if (myPArray->num_edges > 0)
      {
        for (i = n = 0; i < myPArray->num_bounds; ++i)
        {
          if (myPArray->edge_vis)
          {
            glBegin (myDrawMode);
            for (j = 0; j < myPArray->bounds[i]; ++j)
            {
              glEdgeFlag (myPArray->edge_vis[n+j]);
              glVertex3fv (&myPArray->vertices[myPArray->edges[n+j]].xyz[0]);
            }
            glEnd();
          }
          else
          {
            glDrawElements (myDrawMode, myPArray->bounds[i], GL_UNSIGNED_INT, (GLenum* )&myPArray->edges[n]);
          }
          n += myPArray->bounds[i];
        }
      }
      else
      {
        for (i = n = 0 ; i < myPArray->num_bounds; ++i)
        {
          glDrawArrays (myDrawMode, n, myPArray->bounds[i]);
          n += myPArray->bounds[i];
        }
      }
    }
    else if (myPArray->num_edges > 0)
    {
      if (myPArray->edge_vis)
      {
        glBegin (myDrawMode);
        for (i = 0; i < myPArray->num_edges; ++i)
        {
          glEdgeFlag (myPArray->edge_vis[i]);
          glVertex3fv (&myPArray->vertices[myPArray->edges[i]].xyz[0]);
        }
        glEnd();
      }
      else
      {
        glDrawElements (myDrawMode, myPArray->num_edges, GL_UNSIGNED_INT, (GLenum* )myPArray->edges);
      }
    }
    else
    {
      glDrawArrays (myDrawMode, 0, myPArray->num_vertexs);
    }
  }

  if (myDrawMode > GL_LINE_STRIP)
  {
    // Restore line context
    theWorkspace->SetAspectLine (anAspectLineOld);
    glPopAttrib();
  }
}

// =======================================================================
// function : OpenGl_PrimitiveArray
// purpose  :
// =======================================================================
OpenGl_PrimitiveArray::OpenGl_PrimitiveArray (CALL_DEF_PARRAY* thePArray)
: myPArray (thePArray),
  myDrawMode (DRAW_MODE_NONE),
  myIsVboInit (Standard_False)
{
  switch (myPArray->type)
  {
    case TelPointsArrayType:
      myDrawMode = GL_POINTS;
      break;
    case TelPolylinesArrayType:
      myDrawMode = GL_LINE_STRIP;
      break;
    case TelSegmentsArrayType:
      myDrawMode = GL_LINES;
      break;
    case TelPolygonsArrayType:
      myDrawMode = GL_POLYGON;
      break;
    case TelTrianglesArrayType:
      myDrawMode = GL_TRIANGLES;
      break;
    case TelQuadranglesArrayType:
      myDrawMode = GL_QUADS;
      break;
    case TelTriangleStripsArrayType:
      myDrawMode = GL_TRIANGLE_STRIP;
      break;
    case TelQuadrangleStripsArrayType:
      myDrawMode = GL_QUAD_STRIP;
      break;
    case TelTriangleFansArrayType:
      myDrawMode = GL_TRIANGLE_FAN;
      break;
  }
}

// =======================================================================
// function : ~OpenGl_PrimitiveArray
// purpose  :
// =======================================================================
OpenGl_PrimitiveArray::~OpenGl_PrimitiveArray()
{
  //
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void OpenGl_PrimitiveArray::Release (const Handle(OpenGl_Context)& theContext)
{
  for (Standard_Integer anIter = 0; anIter < VBOMaxType; ++anIter)
  {
    if (!myVbos[anIter].IsNull())
    {
      if (!theContext.IsNull())
      {
        theContext->DelayedRelease (myVbos[anIter]);
      }
      myVbos[anIter].Nullify();
    }
  }
}

// =======================================================================
// function : Render
// purpose  :
// =======================================================================
void OpenGl_PrimitiveArray::Render (const Handle(OpenGl_Workspace)& theWorkspace) const
{
  if (myPArray == NULL || myDrawMode == DRAW_MODE_NONE || myPArray->num_vertexs <= 0)
  {
    return;
  }

  // create VBOs on first render call
  if (!myIsVboInit && OpenGl_GraphicDriver::ToUseVBO() && theWorkspace->GetGlContext()->core15 != NULL)
  {
    BuildVBO (theWorkspace);
    myIsVboInit = Standard_True;
  }

  switch (myPArray->type)
  {
    case TelPointsArrayType:
    case TelPolylinesArrayType:
    case TelSegmentsArrayType:
    {
      glDisable (GL_LIGHTING);
      break;
    }
    default:
      break;
  }

  const OpenGl_AspectFace*   anAspectFace   = theWorkspace->AspectFace   (Standard_True);
  const OpenGl_AspectLine*   anAspectLine   = theWorkspace->AspectLine   (Standard_True);
  const OpenGl_AspectMarker* anAspectMarker = theWorkspace->AspectMarker (myPArray->type == TelPointsArrayType);

  Tint aFrontLightingModel = anAspectFace->IntFront.color_mask;
  const TEL_COLOUR* anInteriorColor = &anAspectFace->IntFront.matcol;
  const TEL_COLOUR* anEdgeColor = &anAspectFace->AspectEdge()->Color();
  const TEL_COLOUR* aLineColor = (myPArray->type == TelPointsArrayType) ? &anAspectMarker->Color() : &anAspectLine->Color();

  // Use highlight colors
  if (theWorkspace->NamedStatus & OPENGL_NS_HIGHLIGHT)
  {
    anEdgeColor = anInteriorColor = aLineColor = theWorkspace->HighlightColor;
    aFrontLightingModel = 0;
  }

  DrawArray (aFrontLightingModel,
             anAspectFace->InteriorStyle,
             anAspectFace->Edge,
             anInteriorColor,
             aLineColor,
             anEdgeColor,
             &anAspectFace->IntFront,
             theWorkspace);
}
