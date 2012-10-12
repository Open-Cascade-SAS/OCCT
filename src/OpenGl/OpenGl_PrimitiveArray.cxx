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

#include <OpenGl_IndexBuffer.hxx>
#include <OpenGl_Context.hxx>

#include <OpenGl_PrimitiveArray.hxx>

#include <OpenGl_AspectFace.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <OpenGl_Structure.hxx>
#include <OpenGl_TextureBox.hxx>

#include <InterfaceGraphic_PrimitiveArray.hxx>

namespace
{
  static unsigned long vRand = 1L;
  #define OGL_Rand() (vRand = vRand * 214013L + 2531011L)
};

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

  if (theWorkspace->DegenerateModel < 2 &&
     ((myDrawMode >  GL_LINE_STRIP && theInteriorStyle != Aspect_IS_EMPTY) ||
      (myDrawMode <= GL_LINE_STRIP)))
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
            if (pfc != NULL)
            {
              glColor3fv (pfc[i].rgb);
            }
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

    if (theWorkspace->DegenerateModel)
    {
      if (myDrawMode <= GL_LINE_STRIP)
      {
        glPopAttrib();
      }
      return;
    }
  }

  if (theEdgeFlag || theWorkspace->DegenerateModel)
  {
    switch (theWorkspace->DegenerateModel)
    {
      default: // XXX_TDM_NODE or TINY
        //  On some NVIDIA graphic cards, using glEdgeFlagPointer() in 
        //  combination with VBO ( edge flag data put into a VBO buffer)
        //  leads to a crash in a driver. Therefore, edge flags are simply 
        //  igonored when VBOs are enabled, so all the edges are drawn if 
        //  edge visibility is turned on. In order to draw edges selectively, 
        //  either disable VBO or turn off edge visibilty in the current 
        //  primitive array and create a separate primitive array (segments) 
        //  and put edges to be drawn into it.
        if (myDrawMode > GL_LINE_STRIP)
        {
          DrawEdges (theEdgeFlag ? theEdgeColour : theInteriorColour, theWorkspace);
        }
        break;
      // DegenerateModel(as Lines, Points, BBoxs) are used only without VBO
      case 2: // XXX_TDM_WIREFRAME
        if (!toDrawVbo())
          DrawDegeneratesAsLines ((theEdgeFlag ? theEdgeColour : theInteriorColour), theWorkspace);
        break;
      case 3: // XXX_TDM_MARKER
        if (!toDrawVbo())
          DrawDegeneratesAsPoints ((theEdgeFlag ? theEdgeColour : theInteriorColour), theWorkspace->SkipRatio);
        break;
      case 4: // XXX_TDM_BBOX
        if (!toDrawVbo())
          DrawDegeneratesAsBBoxs (theEdgeFlag ? theEdgeColour : theInteriorColour);
        break;
    }
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
// function : DrawDegeneratesPointsAsPoints
// purpose  :
// =======================================================================
void OpenGl_PrimitiveArray::DrawDegeneratesPointsAsPoints() const
{ 
  tel_point pv = myPArray->vertices;
  for (Tint aVertId = 0; aVertId < myPArray->num_vertexs; ++aVertId)
  {
    glVertex3fv (&pv[aVertId].xyz[0]);
  }
}

// =======================================================================
// function : DrawDegeneratesLinesAsPoints
// purpose  :
// =======================================================================
void OpenGl_PrimitiveArray::DrawDegeneratesLinesAsPoints() const
{
  GLfloat pt[3];
  tel_point pv = myPArray->vertices;

  Tint j = 0;
  while (j < myPArray->num_vertexs)
  {
    pt[0] = pv[j].xyz[0];
    pt[1] = pv[j].xyz[1];
    pt[2] = pv[j].xyz[2]; ++j;
    pt[0] += pv[j].xyz[0];
    pt[1] += pv[j].xyz[1];
    pt[2] += pv[j].xyz[2]; ++j;
    pt[0] *= 0.5f;
    pt[1] *= 0.5f;
    pt[2] *= 0.5f;
    glVertex3fv (pt);
  }
}  

// =======================================================================
// function : DrawDegeneratesTrianglesAsPoints
// purpose  :
// =======================================================================
void OpenGl_PrimitiveArray::DrawDegeneratesTrianglesAsPoints() const
{
  Tint      i, j, iv;
  GLfloat   pt[ 3 ];
  tel_point pv = myPArray->vertices;

  if (myPArray->num_edges > 0)
  {
    for (j = 0; j < myPArray->num_edges; j += 3)
    {
      iv = myPArray->edges[j];
      pt[0] = pv[iv].xyz[0];
      pt[1] = pv[iv].xyz[1];
      pt[2] = pv[iv].xyz[2];
      for (i = 1; i < 3; ++i)
      {
        iv = myPArray->edges[j+i];
        pt[0] += pv[iv].xyz[0];
        pt[1] += pv[iv].xyz[1];
        pt[2] += pv[iv].xyz[2];
      }
      pt[0] /= 3.f;
      pt[1] /= 3.f;
      pt[2] /= 3.f;
      glVertex3fv (pt);
    }
  }
  else
  {
    for (j = 0; j < myPArray->num_vertexs; j += 3)
    {
      pt[0] = pv[j].xyz[0];
      pt[1] = pv[j].xyz[1];
      pt[2] = pv[j].xyz[2];
      for (i = 1; i < 3; ++i)
      {
        pt[0] += pv[j+i].xyz[0];
        pt[1] += pv[j+i].xyz[1];
        pt[2] += pv[j+i].xyz[2];
      }
      pt[0] /= 3.f;
      pt[1] /= 3.f;
      pt[2] /= 3.f;
      glVertex3fv (pt);
    }
  }
}

// =======================================================================
// function : DrawDegeneratesTrianglesAsPoints
// purpose  :
// =======================================================================
void OpenGl_PrimitiveArray::DrawDegeneratesTrianglestripsAsPoints() const
{ 
  Tint      i, j, k, n;
  GLfloat   pt[ 3 ];
  tel_point pv = myPArray->vertices;

  if (myPArray->num_bounds > 0)
  {
    for (k = n = 0; k < myPArray->num_bounds; ++k)
    {
      for (j = 0; j < myPArray->bounds[k] - 2; ++j)
      {
        pt[0] = pv[n+j].xyz[0];
        pt[1] = pv[n+j].xyz[1];
        pt[2] = pv[n+j].xyz[2];
        for (i = 1; i < 3; ++i)
        {
          pt[0] += pv[n+j+i].xyz[0];
          pt[1] += pv[n+j+i].xyz[1];
          pt[2] += pv[n+j+i].xyz[2];
        }
        pt[0] /= 3.f;
        pt[1] /= 3.f;
        pt[2] /= 3.f;
        glVertex3fv (pt);
      }
      n += myPArray->bounds[k];
    }
  }
  else
  {
    for (j = 0; j < myPArray->num_vertexs - 2; ++j)
    {
      pt[0] = pv[j].xyz[0];
      pt[1] = pv[j].xyz[1];
      pt[2] = pv[j].xyz[2];
      for (i = 1; i < 3; ++i)
      {
        pt[0] += pv[j+i].xyz[0];
        pt[1] += pv[j+i].xyz[1];
        pt[2] += pv[j+i].xyz[2];
      }
      pt[0] /= 3.f;
      pt[1] /= 3.f;
      pt[2] /= 3.f;
      glVertex3fv (pt);
    }
  }
}

// =======================================================================
// function : DrawDegeneratesPolygonsAsPoints
// purpose  :
// =======================================================================
void OpenGl_PrimitiveArray::DrawDegeneratesPolygonsAsPoints() const
{ 
  Tint      j, k, n, iv;
  GLfloat   pt[3];
  tel_point pv = myPArray->vertices;

  if (myPArray->num_bounds > 0)
  {
    if (myPArray->num_edges > 0)
    {
      for (k = n = 0; k < myPArray->num_bounds; ++k)
      {
        pt[0] = pt[1] = pt[2] = 0.0;
        for (j = 0; j < myPArray->bounds[k]; ++j)
        {
          iv = myPArray->edges[n+j];
          pt[0] += pv[iv].xyz[0];
          pt[1] += pv[iv].xyz[1];
          pt[2] += pv[iv].xyz[2];
        }
        pt[0] /= myPArray->bounds[k];
        pt[1] /= myPArray->bounds[k];
        pt[2] /= myPArray->bounds[k];
        glVertex3fv (pt);
        n += myPArray->bounds[k];
      }
    }
    else
    {
      for (k = n = 0; k < myPArray->num_bounds; ++k)
      {
        pt[0] = pt[1] = pt[2] = 0.0;
        for (j = 0; j < myPArray->bounds[k]; ++j)
        {
          pt[0] += pv[n+j].xyz[0];
          pt[1] += pv[n+j].xyz[1];
          pt[2] += pv[n+j].xyz[2];
        }
        pt[0] /= myPArray->bounds[k];
        pt[1] /= myPArray->bounds[k];
        pt[2] /= myPArray->bounds[k];
        glVertex3fv (pt);
        n += myPArray->bounds[k];
      }
    }
  }
  else if (myPArray->num_edges > 0)
  {
    pt[0] = pt[1] = pt[2] = 0.0;
    for (j = 0; j < myPArray->num_edges; ++j)
    {
      iv = myPArray->edges[j];
      pt[0] += pv[iv].xyz[0];
      pt[1] += pv[iv].xyz[1];
      pt[2] += pv[iv].xyz[2];
    }
    pt[0] /= myPArray->num_edges;
    pt[1] /= myPArray->num_edges;
    pt[2] /= myPArray->num_edges;
    glVertex3fv (pt);
  }
  else
  {
    pt[0] = pt[1] = pt[2] = 0.0;
    for (j = 0; j < myPArray->num_vertexs; ++j)
    {
      pt[0] += pv[j].xyz[0];
      pt[1] += pv[j].xyz[1];
      pt[2] += pv[j].xyz[2];
    }
    pt[0] /= myPArray->num_vertexs;
    pt[1] /= myPArray->num_vertexs;
    pt[2] /= myPArray->num_vertexs;
    glVertex3fv (pt);
  }
}

// =======================================================================
// function : DrawDegeneratesQuadranglesAsPoints
// purpose  :
// =======================================================================
void OpenGl_PrimitiveArray::DrawDegeneratesQuadranglesAsPoints() const
{
  Tint      i, j, iv;
  GLfloat   pt[ 3 ];
  tel_point pv = myPArray->vertices;

  if (myPArray->num_edges > 0)
  {
    for (j = 0; j < myPArray->num_edges; j += 4)
    {
      pt[0] = pt[1] = pt[2] = 0.0;
      for (i = 0; i < 4; ++i)
      {
        iv = myPArray->edges[j+i];
        pt[0] += pv[iv].xyz[0];
        pt[1] += pv[iv].xyz[1];
        pt[2] += pv[iv].xyz[2];
      }
      pt[0] /= 4;
      pt[1] /= 4;
      pt[2] /= 4;
      glVertex3fv ( pt );
    }
  }
  else
  {
    for (j = 0; j < myPArray->num_vertexs; j += 4)
    {
      pt[0] = pt[1] = pt[2] = 0.0;
      for (i = 0; i < 4; ++i)
      {
        pt[0] += pv[j+i].xyz[0];
        pt[1] += pv[j+i].xyz[1];
        pt[2] += pv[j+i].xyz[2];
      }
      pt[0] /= 4;
      pt[1] /= 4;
      pt[2] /= 4;
      glVertex3fv (pt);
    }
  }
}  

// =======================================================================
// function : DrawDegeneratesAsPoints
// purpose  :
// =======================================================================
void OpenGl_PrimitiveArray::DrawDegeneratesQuadranglestripsAsPoints() const
{ 
  Tint      i, j, k, n;
  GLfloat   pt[3];
  tel_point pv = myPArray->vertices;

  if (myPArray->num_bounds > 0)
  {
    for (k = n = 0; k < myPArray->num_bounds; ++k)
    {
      for (j = 0; j < myPArray->bounds[k] - 2; j += 2)
      {
        pt[0] = pt[1] = pt[2] = 0.;
        for (i = 0; i < 4; ++i)
        {
          pt[0] += pv[n+j+i].xyz[0];
          pt[1] += pv[n+j+i].xyz[1];
          pt[2] += pv[n+j+i].xyz[2];
        }
        pt[0] /= 4;
        pt[1] /= 4;
        pt[2] /= 4;
        glVertex3fv (pt);
      }
      n += myPArray->bounds[k];
    }
  }
  else
  {
    for (j = 0; j < myPArray->num_vertexs - 2; j += 2)
    {
      pt[0] = pt[1] = pt[2] = 0.;
      for (i = 0; i < 4; ++i)
      {
        pt[0] += pv[j+i].xyz[0];
        pt[1] += pv[j+i].xyz[1];
        pt[2] += pv[j+i].xyz[2];
      }
      pt[0] /= 4;
      pt[1] /= 4;
      pt[2] /= 4;
      glVertex3fv (pt);
    }
  }
}

// =======================================================================
// function : DrawDegeneratesAsPoints
// purpose  :
// =======================================================================
void OpenGl_PrimitiveArray::DrawDegeneratesAsPoints (const TEL_COLOUR* theEdgeColour,
                                                     const float       theSkipRatio) const
{
  if (theSkipRatio >= 1.0f)
    return;

  GLboolean zbuff_state = glIsEnabled (GL_DEPTH_TEST);
  glDisable (GL_LIGHTING);
  if (zbuff_state)
    glDisable (GL_DEPTH_TEST); 
  glColor3fv (theEdgeColour->rgb);

  glBegin (GL_POINTS);
  switch (myDrawMode)
  {
    case GL_POINTS:
      DrawDegeneratesPointsAsPoints();
      break;
    case GL_LINES:
      DrawDegeneratesLinesAsPoints();
      break;
    case GL_LINE_STRIP:
    case GL_POLYGON:
      DrawDegeneratesPolygonsAsPoints();
      break;
    case GL_TRIANGLES:
      DrawDegeneratesTrianglesAsPoints();
      break;
    case GL_QUADS:
      DrawDegeneratesQuadranglesAsPoints();
      break;
    case GL_TRIANGLE_FAN:
    case GL_TRIANGLE_STRIP:
      DrawDegeneratesTrianglestripsAsPoints();
      break;
    case GL_QUAD_STRIP:
      DrawDegeneratesQuadranglestripsAsPoints();
      break;
    default:
      break;
  }
  glEnd();
  if (zbuff_state)
    glEnable (GL_DEPTH_TEST);
}

// =======================================================================
// function : DrawDegeneratesLinesAsLines
// purpose  :
// =======================================================================
void OpenGl_PrimitiveArray::DrawDegeneratesLinesAsLines (const float theSkipRatio) const
{
  Tint i, iv;
  tel_point  pv = myPArray->vertices;

  Tint n = myPArray->num_vertexs;
  Tint j = int((1.0f - theSkipRatio) * n);
  for (; j > 0; --j)
  {
    i = OGL_Rand() % n;
    myPArray->keys[i] = -myPArray->keys[i];
  }

  if (myPArray->num_bounds > 0)
  {
    if (myPArray->num_edges > 0)
    {
      for (i = n = 0; i < myPArray->num_bounds; ++i)
      {
        glBegin (GL_LINES);
        for (j = 0; j < myPArray->bounds[i]; ++j)
        {
          iv = myPArray->edges[n + j];
          if (myPArray->keys[iv] < 0)
          {
            myPArray->keys[iv] = -myPArray->keys[iv];
            glVertex3fv (pv[iv].xyz);
          }
        }
        glEnd();
        n += myPArray->bounds[i];
      }
    }
    else
    {
      for (i = n = 0; i < myPArray->num_bounds; ++i)
      {
        glBegin (GL_LINES);
        for (j = 0; j < myPArray->bounds[i]; ++j)
        {
          if (myPArray->keys[n+j] < 0)
          {
            myPArray->keys[n+j] = -myPArray->keys[n+j];
            glVertex3fv (pv[n+j].xyz);
          }
        }
        glEnd();
        n += myPArray->bounds[i];
      }
    }
  }
  else if (myPArray->num_edges > 0)
  {
    glBegin (GL_LINES);
    for (j = 0; j < myPArray->num_edges; ++j)
    {
      iv = myPArray->edges[j];
      if (myPArray->keys[iv] < 0)
      {
        myPArray->keys[iv] = -myPArray->keys[iv];
        glVertex3fv (pv[iv].xyz);
      }
    }
    glEnd();
  }
  else
  {
    glBegin (GL_LINES);
    for (j = 0; j < myPArray->num_vertexs; ++j)
    {
      if (myPArray->keys[j] < 0)
      {
        myPArray->keys[j] = -myPArray->keys[j];
        glVertex3fv (pv[j].xyz);
      }
    }
    glEnd();
  }
}

// =======================================================================
// function : DrawDegeneratesTrianglesAsLines
// purpose  :
// =======================================================================
void OpenGl_PrimitiveArray::DrawDegeneratesTrianglesAsLines (const float theSkipRatio) const
{ 
  Tint i, iv;
  tel_point pv = myPArray->vertices;

  Tint n = myPArray->num_vertexs / 3;
  Tint j = int((1.0f - theSkipRatio) * n);
  for (; j > 0; --j)
  {
    i = OGL_Rand() % n; i *= 3;
    myPArray->keys[i] = -myPArray->keys[i];
  }

  if (myPArray->num_edges > 0)
  {
    for (j = 0; j < myPArray->num_edges; j += 3)
    {
      iv = myPArray->edges[j];
      if (myPArray->keys[iv] < 0)
      { 
        myPArray->keys[iv] = -myPArray->keys[iv];
        glBegin (GL_LINE_LOOP);
        for (i = 0; i < 3; ++i)
        {
          iv = myPArray->edges[j+i];
          glVertex3fv (pv[iv].xyz);
        }
        glEnd();
      }
    }
  }
  else
  {
    for (j = 0; j < myPArray->num_vertexs; j += 3)
    {
      if (myPArray->keys[j] < 0)
      {
        myPArray->keys[j] = -myPArray->keys[j];
        glBegin (GL_LINE_LOOP);
        for (i = 0; i < 3; ++i)
        {
          glVertex3fv (pv[j+i].xyz);
        }
        glEnd();
      }
    }
  }
}

// =======================================================================
// function : DrawDegeneratesTrianglesAsLines
// purpose  :
// =======================================================================
void OpenGl_PrimitiveArray::DrawDegeneratesTrianglestripsAsLines (const float theSkipRatio) const
{ 
  Tint i, j, k, n, ni;
  tel_point pv = myPArray->vertices;

  if (myPArray->num_bounds > 0)
  {
    for (i = n = 0; i < myPArray->num_bounds; ++i)
    {
      ni = myPArray->bounds[i] - 2;
      k = int((1.0f - theSkipRatio) * ni);
      for (; k > 0; --k)
      {
        j = OGL_Rand() % ni; j += 2;
        myPArray->keys[n+j] = -myPArray->keys[n+j];
      }
      for (j = 2; j < myPArray->bounds[i]; ++j)
      {
        if (myPArray->keys[n+j] < 0)
        {
          myPArray->keys[n+j] = -myPArray->keys[n+j];
          glBegin (GL_LINE_LOOP);
          glVertex3fv (pv[n+j-2].xyz);
          glVertex3fv (pv[n+j-1].xyz);
          glVertex3fv (pv[n+j].xyz);
          glEnd();
        }
      }
      n += myPArray->bounds[i];
    }
  }
  else
  {
    ni = myPArray->num_vertexs - 2;
    k = int((1.0f - theSkipRatio) * ni);
    for (; k > 0; --k)
    {
      j = OGL_Rand() % ni; j += 2;
      myPArray->keys[j] = -myPArray->keys[j];
    }
    for (j = 2; j < myPArray->num_vertexs; ++j)
    {
      if (myPArray->keys[j] < 0)
      {
        myPArray->keys[j] = -myPArray->keys[j];
        glBegin (GL_LINE_LOOP);
        glVertex3fv (pv[j-2].xyz);
        glVertex3fv (pv[j-1].xyz);
        glVertex3fv (pv[j].xyz);
        glEnd();
      }
    }
  }
}

// =======================================================================
// function : DrawDegeneratesPolygonsAsLines
// purpose  :
// =======================================================================
void OpenGl_PrimitiveArray::DrawDegeneratesPolygonsAsLines (const float theSkipRatio) const
{ 
  Tint i, iv;
  tel_point pv = myPArray->vertices;

  Tint n = myPArray->num_vertexs;
  Tint j = int((1.0f - theSkipRatio) * n);
  for (; j > 0; --j)
  {
    i = OGL_Rand() % n;
    myPArray->keys[i] = -myPArray->keys[i];
  }

  if (myPArray->num_bounds > 0)
  {
    if (myPArray->num_edges > 0)
    {
      for (i = n = 0; i < myPArray->num_bounds; ++i)
      {
        glBegin (GL_LINE_LOOP);
        for (j = 0; j < myPArray->bounds[i]; ++j)
        {
          iv = myPArray->edges[n+j];
          if (myPArray->keys[iv] < 0)
          {
            myPArray->keys[iv] = -myPArray->keys[iv];
            glVertex3fv (pv[iv].xyz);
          }
        }
        glEnd();
        n += myPArray->bounds[i];
      }
    }
    else
    {
      for (i = n = 0; i < myPArray->num_bounds; ++i)
      {
        glBegin (GL_LINE_LOOP);
        for (j = 0; j < myPArray->bounds[i]; ++j)
        {
          if (myPArray->keys[n+j] < 0)
          {
            myPArray->keys[n+j] = -myPArray->keys[n+j];
            glVertex3fv (pv[n+j].xyz);
          }
        }
        glEnd();
        n += myPArray->bounds[i];
      }
    }
  }
  else if (myPArray->num_edges > 0)
  {
    glBegin (GL_LINE_LOOP);
    for (j = 0; j < myPArray->num_edges; ++j)
    {
      iv = myPArray->edges[j];
      if (myPArray->keys[iv] < 0)
      {
        myPArray->keys[iv] = -myPArray->keys[iv];
        glVertex3fv (pv[iv].xyz);
      }
    }
    glEnd();
  }
  else
  {
    glBegin (GL_LINE_LOOP);
    for (j = 0; j < myPArray->num_vertexs; ++j)
    {
      if (myPArray->keys[j] < 0)
      {
        myPArray->keys[j] = -myPArray->keys[j];
        glVertex3fv (pv[j].xyz);
      }
    }
    glEnd();
  }
}  

// =======================================================================
// function : DrawDegeneratesQuadranglesAsLines
// purpose  :
// =======================================================================
void OpenGl_PrimitiveArray::DrawDegeneratesQuadranglesAsLines (const float theSkipRatio) const
{
  Tint i, iv;
  tel_point pv = myPArray->vertices;

  Tint n = myPArray->num_vertexs / 4;
  Tint j = int((1.0f - theSkipRatio) * n);
  for (; j > 0; --j)
  {
    i = OGL_Rand() % n; i *= 4;
    myPArray->keys[i] = -myPArray->keys[i];
  }

  if (myPArray->num_edges > 0)
  {
    for (j = 0; j < myPArray->num_edges; j += 4)
    {
      iv = myPArray->edges[j];
      if (myPArray->keys[iv] < 0)
      {
        myPArray->keys[iv] = -myPArray->keys[iv];
        glBegin (GL_LINE_LOOP);
        for (i = 0; i < 4; ++i)
        {
          iv = myPArray->edges[j+i];
          glVertex3fv (pv[iv].xyz);
        }
        glEnd();
      }
    }
  }
  else
  {
    for (j = 0; j < myPArray->num_vertexs; j += 4)
    {
      if (myPArray->keys[j] < 0)
      {
        myPArray->keys[j] = -myPArray->keys[j];
        glBegin (GL_LINE_LOOP);
        for (i = 0; i < 4; ++i)
        {
          glVertex3fv (pv[j+i].xyz);
        }
        glEnd();
      }
    }
  }
}

// =======================================================================
// function : DrawDegeneratesQuadranglesAsLines
// purpose  :
// =======================================================================
void OpenGl_PrimitiveArray::DrawDegeneratesQuadranglestripsAsLines (const float theSkipRatio) const
{ 
  Tint i, j, k, n, ni;
  tel_point pv = myPArray->vertices;

  if (myPArray->num_bounds > 0)
  {
    for (i = n = 0; i < myPArray->num_bounds; ++i)
    {
      ni = myPArray->bounds[i] / 2 - 2;
      k = int((1.0f - theSkipRatio) * ni);
      for (; k > 0; --k)
      {
        j = OGL_Rand() % ni; j = j * 2 + 2;
        myPArray->keys[n+j] = -myPArray->keys[n+j];
      }
      for (j = 3; j < myPArray->bounds[i]; j += 2)
      {
        if (myPArray->keys[n+j] < 0)
        {
          myPArray->keys[n+j] = -myPArray->keys[n+j];
          glBegin (GL_LINE_LOOP);
          glVertex3fv (pv[n+j-3].xyz);
          glVertex3fv (pv[n+j-2].xyz);
          glVertex3fv (pv[n+j-1].xyz);
          glVertex3fv (pv[n+j].xyz);
          glEnd();
        }
      }
      n += myPArray->bounds[i];
    }
  }
  else
  {
    ni = myPArray->num_vertexs / 2 - 2;
    k = int((1.0f - theSkipRatio) * ni);
    for (; k > 0; --k)
    {
      j = OGL_Rand() % ni; j = j * 2 + 2;
      myPArray->keys[j] = -myPArray->keys[j];
    }
    for (j = 3; j < myPArray->num_vertexs; j += 2)
    {
      if (myPArray->keys[j] < 0)
      {
        myPArray->keys[j] = -myPArray->keys[j];
        glBegin (GL_LINE_LOOP);
        glVertex3fv (pv[j-3].xyz);
        glVertex3fv (pv[j-2].xyz);
        glVertex3fv (pv[j-1].xyz);
        glVertex3fv (pv[j].xyz);
        glEnd();
      }
    }
  }
}

// =======================================================================
// function : DrawDegeneratesAsLines
// purpose  :
// =======================================================================
void OpenGl_PrimitiveArray::DrawDegeneratesAsLines (const TEL_COLOUR*               theEdgeColour,
                                                    const Handle(OpenGl_Workspace)& theWorkspace) const
{
  const float aSkipRatio = theWorkspace->SkipRatio;

  GLboolean zbuff_state = glIsEnabled (GL_DEPTH_TEST);

  glDisable (GL_LIGHTING);

  if (zbuff_state)
    glDisable (GL_DEPTH_TEST); 

  glColor3fv (theEdgeColour->rgb);

  if (aSkipRatio != 0.0f)
  {
    switch (myDrawMode)
    {
      case GL_POINTS:
        if (aSkipRatio < 1.0f)
          DrawDegeneratesPointsAsPoints();
        break;
      case GL_LINES:
        DrawDegeneratesLinesAsLines (aSkipRatio);
        break;
      case GL_LINE_STRIP:
      case GL_POLYGON:
        DrawDegeneratesPolygonsAsLines (aSkipRatio);
        break;
      case GL_TRIANGLES:
        DrawDegeneratesTrianglesAsLines (aSkipRatio);
        break;
      case GL_QUADS:
        DrawDegeneratesQuadranglesAsLines (aSkipRatio);
        break;
      case GL_TRIANGLE_FAN:
      case GL_TRIANGLE_STRIP:
        DrawDegeneratesTrianglestripsAsLines (aSkipRatio);
        break;
      case GL_QUAD_STRIP:
        DrawDegeneratesQuadranglestripsAsLines (aSkipRatio);
        break;
      default:
        break;
    }
  }
  else
  {
    int i,n;
    glPushAttrib (GL_POLYGON_BIT);
    glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);

    GLboolean color_array_mode         = glIsEnabled (GL_COLOR_ARRAY);
    GLboolean edge_flag_array_mode     = glIsEnabled (GL_EDGE_FLAG_ARRAY);
    GLboolean index_array_mode         = glIsEnabled (GL_INDEX_ARRAY);
    GLboolean normal_array_mode        = glIsEnabled (GL_NORMAL_ARRAY);
    GLboolean texture_coord_array_mode = glIsEnabled (GL_TEXTURE_COORD_ARRAY);
    GLboolean vertex_array_mode        = glIsEnabled (GL_VERTEX_ARRAY);

    glDisableClientState (GL_COLOR_ARRAY);
    glDisableClientState (GL_EDGE_FLAG_ARRAY);
    glDisableClientState (GL_INDEX_ARRAY);
    glDisableClientState (GL_NORMAL_ARRAY);
    glDisableClientState (GL_TEXTURE_COORD_ARRAY);

    if (!vertex_array_mode)
      glEnableClientState (GL_VERTEX_ARRAY);

    glVertexPointer (3, GL_FLOAT, 0, myPArray->vertices); // array of vertices

    if (myPArray->num_bounds > 0)
    {
      if (myPArray->num_edges > 0)
      {
        for (i = n = 0; i < myPArray->num_bounds; ++i)
        {
          glDrawElements (myDrawMode, myPArray->bounds[i], GL_UNSIGNED_INT, (GLenum* )&myPArray->edges[n]);
          n += myPArray->bounds[i];
        }
      }
      else
      {
        for (i = n = 0; i < myPArray->num_bounds; ++i)
        {
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

    if (!vertex_array_mode) glDisableClientState (GL_VERTEX_ARRAY);

    if (color_array_mode)         glEnableClientState (GL_COLOR_ARRAY);
    if (edge_flag_array_mode)     glEnableClientState (GL_EDGE_FLAG_ARRAY);
    if (index_array_mode)         glEnableClientState (GL_INDEX_ARRAY);
    if (normal_array_mode)        glEnableClientState (GL_NORMAL_ARRAY);
    if (texture_coord_array_mode) glEnableClientState (GL_TEXTURE_COORD_ARRAY);

    glPopAttrib();
  }

  if (zbuff_state)
    glEnable(GL_DEPTH_TEST); 
}  

// =======================================================================
// function : DrawDegeneratesAsBBoxs
// purpose  :
// =======================================================================
void OpenGl_PrimitiveArray::DrawDegeneratesAsBBoxs (const TEL_COLOUR* theEdgeColour) const
{
  GLfloat minp[3] = { FLT_MAX, FLT_MAX, FLT_MAX };
  GLfloat maxp[3] = { FLT_MIN, FLT_MIN, FLT_MIN };
  tel_point pv = myPArray->vertices;

  glDisable (GL_LIGHTING);

  glColor3fv (theEdgeColour->rgb);

  for (Tint i = 0; i < myPArray->num_vertexs; ++i)
  {
    if (pv[i].xyz[0] < minp[0])
      minp[0] = pv[i].xyz[0];
    if (pv[i].xyz[1] < minp[1])
      minp[1] = pv[i].xyz[1];
    if (pv[i].xyz[2] < minp[2])
      minp[2] = pv[i].xyz[2];

    if (pv[i].xyz[0] > maxp[0])
      maxp[0] = pv[i].xyz[0];
    if (pv[i].xyz[1] > maxp[1])
      maxp[1] = pv[i].xyz[1];
    if (pv[i].xyz[2] > maxp[2])
      maxp[2] = pv[i].xyz[2];
  }

  glBegin (GL_LINE_STRIP);

  glVertex3fv (minp);
  glVertex3f (minp[0], maxp[1], minp[2]);
  glVertex3f (minp[0], maxp[1], maxp[2]);
  glVertex3f (minp[0], minp[1], maxp[2]);
  glVertex3f (minp[0], minp[1], minp[2]);

  glVertex3f (maxp[0], minp[1], minp[2]);
  glVertex3f (maxp[0], maxp[1], minp[2]);
  glVertex3f (maxp[0], maxp[1], maxp[2]);
  glVertex3f (maxp[0], minp[1], maxp[2]);
  glVertex3f (maxp[0], minp[1], minp[2]);

  glVertex3f (maxp[0], minp[1], maxp[2]);
  glVertex3f (minp[0], minp[1], maxp[2]);
  glVertex3f (minp[0], maxp[1], maxp[2]);
  glVertex3fv (maxp);
  glVertex3f (maxp[0], maxp[1], minp[2]);
  glVertex3f (minp[0], maxp[1], minp[2]);

  glEnd();
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
    return;

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

      if ((theWorkspace->NamedStatus & OPENGL_NS_ANIMATION) != 0 &&
          (theWorkspace->NamedStatus & OPENGL_NS_WIREFRAME) == 0 &&
           theWorkspace->DegenerateModel)
      {
        glDisable (GL_DEPTH_TEST);
        if (theWorkspace->NamedStatus & OPENGL_NS_TEXTURE)
          DisableTexture();
        theWorkspace->NamedStatus |= OPENGL_NS_WIREFRAME;
      }
      break;
    }
    case TelPolygonsArrayType:
    case TelTrianglesArrayType:
    case TelQuadranglesArrayType:
    case TelTriangleStripsArrayType:
    case TelTriangleFansArrayType:
    case TelQuadrangleStripsArrayType:
    {
      if ((theWorkspace->NamedStatus & OPENGL_NS_ANIMATION) != 0 &&
          (theWorkspace->NamedStatus & OPENGL_NS_WIREFRAME) != 0 &&
           theWorkspace->DegenerateModel < 2)
      {
        if (theWorkspace->NamedStatus & OPENGL_NS_TEXTURE)
          EnableTexture();
        glEnable (GL_DEPTH_TEST);
        theWorkspace->NamedStatus &= ~OPENGL_NS_WIREFRAME;
      }
      break;
    }
    default:
      break;
  }

  const OpenGl_AspectFace*   anAspectFace   = theWorkspace->AspectFace   (Standard_True);
  const OpenGl_AspectLine*   anAspectLine   = theWorkspace->AspectLine   (Standard_True);
  const OpenGl_AspectMarker* anAspectMarker = theWorkspace->AspectMarker (myPArray->type == TelPointsArrayType);

  Tint aFrontLightingModel = anAspectFace->Context().IntFront.color_mask;
  const TEL_COLOUR* anInteriorColor = &anAspectFace->Context().IntFront.matcol;
  const TEL_COLOUR* anEdgeColor = &anAspectFace->AspectEdge()->Color();
  const TEL_COLOUR* aLineColor = (myPArray->type == TelPointsArrayType) ? &anAspectMarker->Color() : &anAspectLine->Color();

  // Use highlight colors
  if (theWorkspace->NamedStatus & OPENGL_NS_HIGHLIGHT)
  {                         
    anEdgeColor = anInteriorColor = aLineColor = theWorkspace->HighlightColor;
    aFrontLightingModel = 0;
  }

  DrawArray (aFrontLightingModel,
             anAspectFace->Context().InteriorStyle,
             anAspectFace->Context().Edge,
             anInteriorColor,
             aLineColor,
             anEdgeColor,
             &anAspectFace->Context().IntFront,
             theWorkspace);

  switch (myPArray->type)
  {
    case TelPointsArrayType:
    case TelPolylinesArrayType:
    case TelSegmentsArrayType:
    {
      if (theWorkspace->NamedStatus & OPENGL_NS_TEXTURE)
        EnableTexture();
    }
  }
}
