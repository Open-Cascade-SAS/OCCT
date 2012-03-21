// Created on: 2000-06-16
// Copyright (c) 2000-2012 OPEN CASCADE SAS
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


#define TRACE 0

#include <Graphic3d_ArrayOfPrimitives.ixx>
#include <Standard.hxx>
#include <TCollection_AsciiString.hxx>
#include <OSD_Environment.hxx>

#include <stdio.h>
#include <stdlib.h>

static Standard_Integer enableArray = 1;
static Standard_Boolean interleavedArray = Standard_False;

Graphic3d_ArrayOfPrimitives :: Graphic3d_ArrayOfPrimitives (
                        const Graphic3d_TypeOfPrimitiveArray aType,
                        const Standard_Integer maxVertexs,
                        const Standard_Integer maxBounds,
                        const Standard_Integer maxEdges,
                        const Standard_Boolean hasVNormals,
                        const Standard_Boolean hasVColors,
                        const Standard_Boolean hasFColors,
                        const Standard_Boolean hasVTexels,
                        const Standard_Boolean hasEdgeInfos
) : myMaxBounds(0),myMaxVertexs(0),myMaxEdges(0) {
  Standard_Integer size = sizeof(CALL_DEF_PARRAY);
  Standard_Integer format = MVERTICE;
  if( hasVNormals ) format |= MVNORMAL;
  if( hasVColors ) format |= MVCOLOR;
  if( hasVTexels ) format |= MVTEXEL;
    
  myPrimitiveArray = (Graphic3d_PrimitiveArray) Standard::Allocate(size); 
  memset ( myPrimitiveArray, 0, size );

  if( maxVertexs > 0){
    myPrimitiveArray->vertices = (TEL_POINT*) Standard::Allocate(maxVertexs *sizeof(TEL_POINT));
    memset ( myPrimitiveArray->vertices, 0, maxVertexs *sizeof(TEL_POINT));
  }

  if( hasVNormals ){
    myPrimitiveArray->vnormals = (TEL_POINT*) Standard::Allocate(sizeof(TEL_POINT) * maxVertexs);
    memset ( myPrimitiveArray->vnormals, 0, sizeof(TEL_POINT) * maxVertexs);
  }

  if( hasVColors ){
    myPrimitiveArray->vcolours = (Tint*) Standard::Allocate(maxVertexs *sizeof(Tint));
    memset ( myPrimitiveArray->vcolours, 0, sizeof(Tint) * maxVertexs);
  }

  if( hasVTexels ){
    myPrimitiveArray->vtexels = (TEL_TEXTURE_COORD*) Standard::Allocate(maxVertexs *sizeof(TEL_TEXTURE_COORD));
    memset ( myPrimitiveArray->vtexels, 0, sizeof(TEL_TEXTURE_COORD) * maxVertexs);
  }

  if( hasFColors && (maxBounds > 0) ){
    myPrimitiveArray->fcolours = (TEL_COLOUR*) Standard::Allocate(maxBounds *sizeof(TEL_COLOUR));
    memset ( myPrimitiveArray->fcolours, 0, sizeof(TEL_COLOUR) * maxBounds);
  } 

  if( maxBounds > 0 ){
    myPrimitiveArray->bounds = (Tint*) Standard::Allocate(maxBounds *sizeof(Tint));
    memset ( myPrimitiveArray->bounds, 0, maxBounds *sizeof(Tint));
  }

  if( maxEdges > 0 ){
    myPrimitiveArray->edges = (Tint*) Standard::Allocate(maxEdges *sizeof(Tint));
    memset ( myPrimitiveArray->edges, 0,  maxEdges *sizeof(Tint));
  }

  if( hasEdgeInfos && (maxEdges > 0) ){
    myPrimitiveArray->edge_vis = (Tchar*)Standard::Allocate(maxEdges *sizeof(Tchar));
    memset ( myPrimitiveArray->edge_vis, 0, maxEdges *sizeof(Tchar));
  }

  myPrimitiveArray->keys = NULL;
  myMaxVertexs = maxVertexs;
  myMaxBounds = maxBounds;
  myMaxEdges = maxEdges;
  myPrimitiveArray->type          = (TelPrimitivesArrayType) aType;
  myPrimitiveArray->format        = format;
  myPrimitiveArray->num_bounds    = 0;
  myPrimitiveArray->num_vertexs   = 0;
  myPrimitiveArray->num_edges     = 0;
  myPrimitiveArray->VBOEnabled    = -1;
  myPrimitiveArray->flagBufferVBO = -1;
  myPrimitiveArray->contextId     = 0;

  for( int i =0 ; i < VBOMaxType ; i++){
    myPrimitiveArray->bufferVBO[i] = 0;
  }

}

void Graphic3d_ArrayOfPrimitives::Destroy (  ){
  if( myPrimitiveArray ) {
    if( myPrimitiveArray->vertices ){
      Standard::Free( (Standard_Address&)myPrimitiveArray->vertices );
      myPrimitiveArray->vertices = 0;
    }

    if( myPrimitiveArray->vnormals ){
      Standard::Free( (Standard_Address&)myPrimitiveArray->vnormals );
      myPrimitiveArray->vnormals = 0;
    }

    if( myPrimitiveArray->vcolours ){
      Standard::Free( (Standard_Address&)myPrimitiveArray->vcolours );
      myPrimitiveArray->vcolours = 0;
    }

    if( myPrimitiveArray->vtexels ){
      Standard::Free( (Standard_Address&)myPrimitiveArray->vtexels );
      myPrimitiveArray->vtexels = 0;
    }

    if( myPrimitiveArray->fcolours ){
      Standard::Free( (Standard_Address&)myPrimitiveArray->fcolours );
      myPrimitiveArray->fcolours = 0;
    } 

    if( myPrimitiveArray->bounds ){
      Standard::Free( (Standard_Address&)myPrimitiveArray->bounds );
      myPrimitiveArray->bounds = 0;
    }

    if( myPrimitiveArray->edges ){
      Standard::Free( (Standard_Address&)myPrimitiveArray->edges );
      myPrimitiveArray->edges = 0;
    }

    if( myPrimitiveArray->edge_vis ){
      Standard::Free( (Standard_Address&)myPrimitiveArray->edge_vis );
      myPrimitiveArray->edge_vis = 0;
    }

    Standard::Free( (Standard_Address&)myPrimitiveArray );
#if TRACE > 0
    cout << " Graphic3d_ArrayOfPrimitives::Destroy()" << endl;
#endif
  }
}

void Graphic3d_ArrayOfPrimitives::Enable() {
  enableArray = 1;
}

void Graphic3d_ArrayOfPrimitives::Disable() {
  enableArray = -1;
}

Standard_Boolean Graphic3d_ArrayOfPrimitives::IsEnable() {

  if( enableArray == 0 ) {
    OSD_Environment csf(TCollection_AsciiString("CSF_USE_ARRAY_OF_PRIMITIVES"));
    TCollection_AsciiString value = csf.Value();
    enableArray = -1;
    if( value.Length() > 0 ) {
      if( value.IsIntegerValue() ) {
        enableArray = value.IntegerValue();
        if( enableArray > 1 ) {
          enableArray = 1;
        } else interleavedArray = Standard_False;
      }
    }
#if TRACE > 0
    if( enableArray > 0 ) {
      if( interleavedArray ) 
        cout << " ! ENABLE to use Interleaved arrays of primitives" << endl;
      else
        cout << " ! ENABLE to use Single arrays of primitives" << endl;
    } else
      cout << " ! DISABLE to use arrays of primitives" << endl;
#endif
  }
  if( enableArray > 0 ) return Standard_True;

  return Standard_False;
}

Standard_Integer Graphic3d_ArrayOfPrimitives::AddVertex(
                const gp_Pnt& aVertice) {
  Standard_Real x,y,z;
  aVertice.Coord(x,y,z);
  return AddVertex(x,y,z);
}

Standard_Integer Graphic3d_ArrayOfPrimitives::AddVertex(
        const Standard_Real X, const Standard_Real Y, const Standard_Real Z) {

  if( !myPrimitiveArray ) return 0;

  Standard_Integer index = myPrimitiveArray->num_vertexs + 1;
  if( index > myMaxVertexs ) {
    Standard_OutOfRange::Raise(" TOO many VERTEX");
  }
  SetVertice(index,X,Y,Z);
  return index;
}

Standard_Integer Graphic3d_ArrayOfPrimitives::AddVertex(
                                                        const gp_Pnt& aVertice,
                                                        const Quantity_Color& aColor) {
  Standard_Real x,y,z;
  aVertice.Coord(x,y,z);
  Standard_Integer index = AddVertex(x,y,z);
  Standard_Real r,g,b;
  aColor.Values(r,g,b,Quantity_TOC_RGB);
  SetVertexColor(index,r,g,b);
  return index;
}
Standard_Integer Graphic3d_ArrayOfPrimitives::AddVertex(
                                                        const gp_Pnt& aVertice,
                                                        const Standard_Integer aColor) {
  Standard_Real x,y,z;
  aVertice.Coord(x,y,z);
  Standard_Integer index = AddVertex(x,y,z);
  SetVertexColor(index,aColor);
  return index;
}


Standard_Integer Graphic3d_ArrayOfPrimitives::AddVertex(
                                                        const gp_Pnt& aVertice,
                                                        const gp_Dir& aNormal) {
  Standard_Real x,y,z;
  aVertice.Coord(x,y,z);
  Standard_Real nx,ny,nz;
  aNormal.Coord(nx,ny,nz);
  return AddVertex(x,y,z,nx,ny,nz);
}

Standard_Integer Graphic3d_ArrayOfPrimitives::AddVertex(
        const Standard_Real X, const Standard_Real Y, const Standard_Real Z,
        const Standard_Real NX, const Standard_Real NY, const Standard_Real NZ) {
  if( !myPrimitiveArray ) return 0;

  Standard_Integer index = myPrimitiveArray->num_vertexs + 1;
  if( index > myMaxVertexs ) {
    Standard_OutOfRange::Raise(" TOO many VERTEX");
  }
  SetVertice(index,X,Y,Z);
  SetVertexNormal(index,NX,NY,NZ);
  return index;
}


Standard_Integer Graphic3d_ArrayOfPrimitives::AddVertex(
                                                        const gp_Pnt& aVertice,
                                                        const gp_Dir& aNormal,
                                                        const Quantity_Color& aColor) {
  Standard_Real x,y,z;
  aVertice.Coord(x,y,z);
  Standard_Real nx,ny,nz;
  aNormal.Coord(nx,ny,nz);
  Standard_Integer index = AddVertex(x,y,z,nx,ny,nz);
  Standard_Real r,g,b;
  aColor.Values(r,g,b,Quantity_TOC_RGB);
  SetVertexColor(index,r,g,b);
  return index;
}

Standard_Integer Graphic3d_ArrayOfPrimitives::AddVertex(
                                                        const gp_Pnt& aVertice,
                                                        const gp_Dir& aNormal,
                                                        const Standard_Integer aColor) {
  Standard_Real x,y,z;
  aVertice.Coord(x,y,z);
  Standard_Real nx,ny,nz;
  aNormal.Coord(nx,ny,nz);
  Standard_Integer index = AddVertex(x,y,z,nx,ny,nz);
  SetVertexColor(index,aColor);
  return index;
}

Standard_Integer Graphic3d_ArrayOfPrimitives::AddVertex(
                                                        const gp_Pnt& aVertice,
                                                        const gp_Pnt2d& aTexel) {
  Standard_Real x,y,z;
  aVertice.Coord(x,y,z);
  Standard_Real tx,ty;
  aTexel.Coord(tx,ty);
  return AddVertex(x,y,z,tx,ty);
}

Standard_Integer Graphic3d_ArrayOfPrimitives::AddVertex(
        const Standard_Real X, const Standard_Real Y, const Standard_Real Z,
        const Standard_Real TX, const Standard_Real TY) {
  if( !myPrimitiveArray ) return 0;

  Standard_Integer index = myPrimitiveArray->num_vertexs + 1;
  if( index > myMaxVertexs ) {
    Standard_OutOfRange::Raise(" TOO many VERTEX");
  }
  SetVertice(index,X,Y,Z);
  SetVertexTexel(index,TX,TY);
  return index;
}

Standard_Integer Graphic3d_ArrayOfPrimitives::AddVertex(
                                                        const gp_Pnt& aVertice,
                                                        const gp_Dir& aNormal,
                                                        const gp_Pnt2d& aTexel) {
  Standard_Real x,y,z;
  aVertice.Coord(x,y,z);
  Standard_Real nx,ny,nz;
  aNormal.Coord(nx,ny,nz);
  Standard_Real tx,ty;
  aTexel.Coord(tx,ty);
  return AddVertex(x,y,z,nx,ny,nz,tx,ty);
}

Standard_Integer Graphic3d_ArrayOfPrimitives::AddVertex(
        const Standard_Real X, const Standard_Real Y, const Standard_Real Z,
        const Standard_Real NX, const Standard_Real NY, const Standard_Real NZ,
        const Standard_Real TX, const Standard_Real TY) {
  if( !myPrimitiveArray ) return 0;

  Standard_Integer index = myPrimitiveArray->num_vertexs + 1;
  if( index > myMaxVertexs ) {
    Standard_OutOfRange::Raise(" TOO many VERTEX");
  }
  SetVertice(index,X,Y,Z);
  SetVertexNormal(index,NX,NY,NZ);
  SetVertexTexel(index,TX,TY);
  return index;
}

Standard_Integer Graphic3d_ArrayOfPrimitives::AddBound( const Standard_Integer edgeNumber) {
  Standard_Integer index = 0;
  if( myPrimitiveArray && myPrimitiveArray->bounds ) {
    index = myPrimitiveArray->num_bounds;
    if( index < myMaxBounds ) {
      myPrimitiveArray->bounds[index] = edgeNumber;
      myPrimitiveArray->num_bounds = ++index;
    } else {
      Standard_OutOfRange::Raise(" TOO many BOUNDS");
    }
  }

  return index;
}

Standard_Integer Graphic3d_ArrayOfPrimitives::AddBound(
                                                        const Standard_Integer edgeNumber,
                                                        const Quantity_Color& aFColor
) {
  Standard_Real r,g,b;
  aFColor.Values(r,g,b,Quantity_TOC_RGB);
  return AddBound(edgeNumber,r,g,b);
}

Standard_Integer Graphic3d_ArrayOfPrimitives::AddBound(
                                                        const Standard_Integer edgeNumber,
                                                        const Standard_Real R, 
                                                        const Standard_Real G,
                                                        const Standard_Real B) { 
  if( !myPrimitiveArray ) return 0;

  Standard_Integer index = myPrimitiveArray->num_bounds;
  if( index >= myMaxBounds ) {
    Standard_OutOfRange::Raise(" TOO many BOUND");
  }
  myPrimitiveArray->bounds[index] = edgeNumber;
  myPrimitiveArray->num_bounds = ++index;
  SetBoundColor(index,R,G,B);
  return index;
}

Standard_Integer Graphic3d_ArrayOfPrimitives::AddEdge(
                                                      const Standard_Integer vertexIndex,
                                                      const Standard_Boolean isVisible) {
  if( !myPrimitiveArray ) return 0;

  Standard_Integer index = myPrimitiveArray->num_edges;
  if( index >= myMaxEdges ) {
    Standard_OutOfRange::Raise(" TOO many EDGE");
  }
  Standard_Integer vindex = vertexIndex-1;
  if( vertexIndex > 0 && vindex < myMaxVertexs ) {
    myPrimitiveArray->edges[index] = vindex;
    if( myPrimitiveArray->edge_vis ) {
      myPrimitiveArray->edge_vis[index] = (Tchar) (isVisible ? 1 : 0);
    }
    myPrimitiveArray->num_edges = ++index;
  } else {
    Standard_OutOfRange::Raise(" BAD EDGE vertex index");
  }

  return index;
}

Standard_Boolean Graphic3d_ArrayOfPrimitives::Orientate(
                        const gp_Dir& aNormal) {
  return Orientate(1,Max(VertexNumber(),EdgeNumber()),aNormal);
}

Standard_Boolean Graphic3d_ArrayOfPrimitives::Orientate(
                                                        const Standard_Integer aVertexIndex,
                                                        const Standard_Integer aVertexNumber,
                                                        const gp_Dir& aNormal) {
  Standard_Boolean somethingHasChange = Standard_False;
  if( myPrimitiveArray && (myPrimitiveArray->num_vertexs > 2) ) {
    Standard_Integer i,j,k=aVertexNumber,n=aVertexIndex-1;
    Standard_ShortReal x,y,z;
    if( myPrimitiveArray->edges ) {
      if( n >= 0 && (n+k) <= myPrimitiveArray->num_edges ) {
        Standard_Integer i1 = myPrimitiveArray->edges[n];
        Standard_Integer i2 = myPrimitiveArray->edges[n+1];
        Standard_Integer i3 = myPrimitiveArray->edges[n+2];
        gp_Pnt p1(myPrimitiveArray->vertices[i1].xyz[0],
                  myPrimitiveArray->vertices[i1].xyz[1],
                  myPrimitiveArray->vertices[i1].xyz[2]);
        gp_Pnt p2(myPrimitiveArray->vertices[i2].xyz[0],
                  myPrimitiveArray->vertices[i2].xyz[1],
                  myPrimitiveArray->vertices[i2].xyz[2]);
        gp_Pnt p3(myPrimitiveArray->vertices[i3].xyz[0],
                  myPrimitiveArray->vertices[i3].xyz[1],
                  myPrimitiveArray->vertices[i3].xyz[2]);
        gp_Vec v21(p1,p2),v31(p1,p3),fn = v21.Crossed(v31);
        if( aNormal.IsOpposite(fn, M_PI / 4.) ) {
          Standard_Integer e; char v;
          for( i=0,j=k-1 ; i<k/2 ; i++,j-- ) {
            e = myPrimitiveArray->edges[n+i];
            myPrimitiveArray->edges[n+i] = myPrimitiveArray->edges[n+j];
            myPrimitiveArray->edges[n+j] = e;
            if( myPrimitiveArray->edge_vis ) {
              v = myPrimitiveArray->edge_vis[n+i];
              myPrimitiveArray->edge_vis[n+i] = myPrimitiveArray->edge_vis[n+j];
              myPrimitiveArray->edge_vis[n+j] = v;
            }
            if( myPrimitiveArray->vnormals ) {
              e = myPrimitiveArray->edges[n+i];
              x = myPrimitiveArray->vnormals[e].xyz[0];
              y = myPrimitiveArray->vnormals[e].xyz[1];
              z = myPrimitiveArray->vnormals[e].xyz[2];
              gp_Vec vn(x,y,z);
              if( aNormal.IsOpposite(vn, M_PI / 4.) ) {
                myPrimitiveArray->vnormals[e].xyz[0] = -x;
                myPrimitiveArray->vnormals[e].xyz[1] = -y;
                myPrimitiveArray->vnormals[e].xyz[2] = -z;
              }
            }
          }
          somethingHasChange = Standard_True;
        }
      } else {
        Standard_OutOfRange::Raise(" BAD EDGE index or number");
      }
      return somethingHasChange;
    } 

    if( n >= 0 && (n+k) <= myPrimitiveArray->num_vertexs ) {
      gp_Pnt p1(myPrimitiveArray->vertices[n].xyz[0],
                  myPrimitiveArray->vertices[n].xyz[1],
                  myPrimitiveArray->vertices[n].xyz[2]);
      gp_Pnt p2(myPrimitiveArray->vertices[n+1].xyz[0],
                  myPrimitiveArray->vertices[n+1].xyz[1],
                  myPrimitiveArray->vertices[n+1].xyz[2]);
      gp_Pnt p3(myPrimitiveArray->vertices[n+2].xyz[0],
                  myPrimitiveArray->vertices[n+2].xyz[1],
                  myPrimitiveArray->vertices[n+2].xyz[2]);
      gp_Vec v21(p1,p2),v31(p1,p3),fn = v21.Crossed(v31);
      if( aNormal.IsOpposite(fn, M_PI / 4.) ) {
        for( i=0,j=k-1 ; i<k/2 ; i++,j-- ) {
          x = myPrimitiveArray->vertices[n+i].xyz[0];
          y = myPrimitiveArray->vertices[n+i].xyz[1];
          z = myPrimitiveArray->vertices[n+i].xyz[2];
          myPrimitiveArray->vertices[n+i].xyz[0] = myPrimitiveArray->vertices[n+j].xyz[0];
          myPrimitiveArray->vertices[n+i].xyz[1] = myPrimitiveArray->vertices[n+j].xyz[1];
          myPrimitiveArray->vertices[n+i].xyz[2] = myPrimitiveArray->vertices[n+j].xyz[2];
          myPrimitiveArray->vertices[n+j].xyz[0] = x;
          myPrimitiveArray->vertices[n+j].xyz[1] = y;
          myPrimitiveArray->vertices[n+j].xyz[2] = z;
          if( myPrimitiveArray->vnormals ) {
            x = myPrimitiveArray->vnormals[n+i].xyz[0];
            y = myPrimitiveArray->vnormals[n+i].xyz[1];
            z = myPrimitiveArray->vnormals[n+i].xyz[2];
            myPrimitiveArray->vnormals[n+i].xyz[0] = myPrimitiveArray->vnormals[n+j].xyz[0];
            myPrimitiveArray->vnormals[n+i].xyz[1] = myPrimitiveArray->vnormals[n+j].xyz[1];
            myPrimitiveArray->vnormals[n+i].xyz[2] = myPrimitiveArray->vnormals[n+j].xyz[2];
            myPrimitiveArray->vnormals[n+j].xyz[0] = x;
            myPrimitiveArray->vnormals[n+j].xyz[1] = y;
            myPrimitiveArray->vnormals[n+j].xyz[2] = z;

            x = myPrimitiveArray->vnormals[n+i].xyz[0];
            y = myPrimitiveArray->vnormals[n+i].xyz[1];
            z = myPrimitiveArray->vnormals[n+i].xyz[2];
            gp_Vec vn(x,y,z);
            if( aNormal.IsOpposite(vn, M_PI / 4.) ) {
              myPrimitiveArray->vnormals[n+i].xyz[0] = -x;
              myPrimitiveArray->vnormals[n+i].xyz[1] = -y;
              myPrimitiveArray->vnormals[n+i].xyz[2] = -z;
            }
          }
          if( myPrimitiveArray->vcolours ) {
            x = (Standard_ShortReal)myPrimitiveArray->vcolours[n+i];
            myPrimitiveArray->vcolours[n+i] = myPrimitiveArray->vcolours[n+j];
            myPrimitiveArray->vcolours[n+j] = (Tint)x;
          }
          if( myPrimitiveArray->vtexels ) {
            x = myPrimitiveArray->vtexels[n+i].xy[0];
            y = myPrimitiveArray->vtexels[n+i].xy[1];
            myPrimitiveArray->vtexels[n+i].xy[0] = myPrimitiveArray->vtexels[n+j].xy[0];
            myPrimitiveArray->vtexels[n+i].xy[1] = myPrimitiveArray->vtexels[n+j].xy[1];
            myPrimitiveArray->vtexels[n+j].xy[0] = x;
            myPrimitiveArray->vtexels[n+j].xy[1] = y;
          }
        }
        somethingHasChange = Standard_True;
      }
    }
  }
  return somethingHasChange;
}

Standard_Boolean Graphic3d_ArrayOfPrimitives::Orientate(
                                                        const Standard_Integer aBoundIndex,
                                                        const gp_Dir& aNormal) {
  Standard_Boolean somethingHasChange = Standard_False;
  if( myPrimitiveArray && myPrimitiveArray->vertices ) {
    if( myPrimitiveArray->bounds && 
        (aBoundIndex > 0) && (aBoundIndex <= myPrimitiveArray->num_bounds) ) {
      Standard_Integer k,n;
      for( k=n=1 ; k<aBoundIndex ; k++ )
        n += myPrimitiveArray->bounds[k];
      k = myPrimitiveArray->bounds[aBoundIndex-1];
      somethingHasChange = Orientate(n,k,aNormal);
    } else if( myPrimitiveArray->bounds ) {
      Standard_OutOfRange::Raise(" BAD BOUND index");
    } else if( (aBoundIndex > 0) && (aBoundIndex <= ItemNumber()) ) {
      switch( myPrimitiveArray->type ) {
        case TelPointsArrayType:
        case TelPolylinesArrayType:
        case TelSegmentsArrayType:
          break;
        case TelPolygonsArrayType:
        case TelTriangleStripsArrayType:
        case TelTriangleFansArrayType:
        case TelQuadrangleStripsArrayType:
          somethingHasChange = Orientate(1,VertexNumber(),aNormal);
          break;
        case TelTrianglesArrayType:
          somethingHasChange = Orientate(aBoundIndex*3-2,3,aNormal);
          break;
        case TelQuadranglesArrayType:
          somethingHasChange = Orientate(aBoundIndex*4-3,4,aNormal);
          break;
        default:
          break;
      }
    } else {
      Standard_OutOfRange::Raise(" BAD ITEM index");
    }
  }
  return somethingHasChange;
}

void Graphic3d_ArrayOfPrimitives::SetVertice(
                                              const Standard_Integer anIndex,
                                              const gp_Pnt& aVertice) {
  Standard_Real x,y,z;
  aVertice.Coord(x,y,z);
  SetVertice(anIndex,x,y,z);
}

void Graphic3d_ArrayOfPrimitives::SetVertexColor(
                                                  const Standard_Integer anIndex,
                                                  const Quantity_Color& aColor) {
  Standard_Real r,g,b;
  aColor.Values(r,g,b,Quantity_TOC_RGB);
  SetVertexColor(anIndex,r,g,b);
}

void Graphic3d_ArrayOfPrimitives::SetVertexColor(
                                                  const Standard_Integer anIndex,
                                                  const Standard_Integer aColor) {

  if( !myPrimitiveArray ) return;
  if( anIndex < 1 || anIndex > myMaxVertexs ) {
    Standard_OutOfRange::Raise(" BAD VERTEX index");
  }
  Standard_Integer index = anIndex - 1;
  if( myPrimitiveArray->vcolours ) {
#if defined (sparc) || defined (__sparc__) || defined (__sparc)
    /* 
      Well known processor(x86) architectures that use the little-endian format. 
      Processors use big-endian format is SPARC. In this case use platform with 
      SPARC architecture(SUNOS). Byte order could have little-endian format.
    */
    const char* p_ch = (const char*)&aColor;
    myPrimitiveArray->vcolours[index] += p_ch[0];
    myPrimitiveArray->vcolours[index] += p_ch[1] << 8 ;
    myPrimitiveArray->vcolours[index] += p_ch[2] << 16;
    myPrimitiveArray->vcolours[index] += p_ch[3] << 24;
#else
    myPrimitiveArray->vcolours[index] = aColor;
#endif

  }
}
void Graphic3d_ArrayOfPrimitives::SetVertexNormal(
                                                  const Standard_Integer anIndex,
                                                  const gp_Dir& aNormal) {
  Standard_Real x,y,z;
  aNormal.Coord(x,y,z);
  SetVertexNormal(anIndex,x,y,z);
}

void Graphic3d_ArrayOfPrimitives::SetVertexTexel(
                                                  const Standard_Integer anIndex,
                                                  const gp_Pnt2d& aTexel) {
  Standard_Real x,y;
  aTexel.Coord(x,y);
  SetVertexTexel(anIndex,x,y);
}

void Graphic3d_ArrayOfPrimitives::SetBoundColor(
                                                const Standard_Integer anIndex,
                                                const Quantity_Color& aColor) {
  Standard_Real r,g,b;
  aColor.Values(r,g,b,Quantity_TOC_RGB);
  SetBoundColor(anIndex,r,g,b);
}

Standard_CString Graphic3d_ArrayOfPrimitives::StringType() const {
  TCollection_AsciiString name("UndefinedArray");
  switch( myPrimitiveArray->type ) {
    case TelPointsArrayType:
      name = "ArrayOfPoints";
      break;
    case TelPolylinesArrayType:
      name = "ArrayOfPolylines";
      break;
    case TelSegmentsArrayType:
      name = "ArrayOfSegments";
      break;
    case TelPolygonsArrayType:
      name = "ArrayOfPolygons";
      break;
    case TelTrianglesArrayType:
      name = "ArrayOfTriangles";
      break;
    case TelQuadranglesArrayType:
      name = "ArrayOfQuadrangles";
      break;
    case TelTriangleStripsArrayType:
      name = "ArrayOfTriangleStrips";
      break;
    case TelQuadrangleStripsArrayType:
      name = "ArrayOfQuadrangleStrips";
      break;
    case TelTriangleFansArrayType:
      name = "ArrayOfTriangleFans";
      break;
    default:
      break;
  }

  return name.ToCString();
}

gp_Pnt Graphic3d_ArrayOfPrimitives::Vertice(const Standard_Integer aRank) const {
  Standard_Real x,y,z;
  Vertice(aRank,x,y,z);
  return gp_Pnt(x,y,z);
}

Quantity_Color Graphic3d_ArrayOfPrimitives::VertexColor(const Standard_Integer aRank) const {
  Standard_Real r,g,b;
  VertexColor(aRank,r,g,b);
  return Quantity_Color(r,g,b,Quantity_TOC_RGB);
}

gp_Dir Graphic3d_ArrayOfPrimitives::VertexNormal(const Standard_Integer aRank) const {
  Standard_Real x,y,z;
  VertexNormal(aRank,x,y,z);
  return gp_Dir(x,y,z);
}

gp_Pnt2d Graphic3d_ArrayOfPrimitives::VertexTexel(const Standard_Integer aRank) const {
  Standard_Real x,y;
  VertexTexel(aRank,x,y);
  return gp_Pnt2d(x,y);
}

Quantity_Color Graphic3d_ArrayOfPrimitives::BoundColor(const Standard_Integer aRank) const {
  Standard_Real r,g,b;
  BoundColor(aRank,r,g,b);
  return Quantity_Color(r,g,b,Quantity_TOC_RGB);
}

Standard_Integer Graphic3d_ArrayOfPrimitives::ItemNumber() const {
  Standard_Integer number=-1;
  if( myPrimitiveArray ) switch( myPrimitiveArray->type ) {
    case TelPointsArrayType:
      number = myPrimitiveArray->num_vertexs;
      break;
    case TelPolylinesArrayType:
    case TelPolygonsArrayType:
      if( myPrimitiveArray->num_bounds > 0 ) 
        number = myPrimitiveArray->num_bounds;
      else number = 1;
      break;
    case TelSegmentsArrayType:
      if( myPrimitiveArray->num_edges > 0 ) 
        number = myPrimitiveArray->num_edges/2;
      else number = myPrimitiveArray->num_vertexs/2;
      break;
    case TelTrianglesArrayType:
      if( myPrimitiveArray->num_edges > 0 ) 
        number = myPrimitiveArray->num_edges/3;
      else number = myPrimitiveArray->num_vertexs/3;
      break;
    case TelQuadranglesArrayType:
      if( myPrimitiveArray->num_edges > 0 ) 
        number = myPrimitiveArray->num_edges/4;
      else number = myPrimitiveArray->num_vertexs/4;
      break;
    case TelTriangleStripsArrayType:
      if( myPrimitiveArray->num_bounds > 0 ) 
        number = myPrimitiveArray->num_vertexs-2*myPrimitiveArray->num_bounds;
      else number = myPrimitiveArray->num_vertexs-2;
      break;
    case TelQuadrangleStripsArrayType:
      if( myPrimitiveArray->num_bounds > 0 ) 
        number = myPrimitiveArray->num_vertexs/2-myPrimitiveArray->num_bounds;
      else number = myPrimitiveArray->num_vertexs/2-1;
      break;
    case TelTriangleFansArrayType:
      if( myPrimitiveArray->num_bounds > 0 ) 
        number = myPrimitiveArray->num_vertexs-2*myPrimitiveArray->num_bounds;
      else number = myPrimitiveArray->num_vertexs-2;
      break;
    default:
      break;
  }

  return number;
}

void Graphic3d_ArrayOfPrimitives::ComputeVNormals(
                                                  const Standard_Integer from,
                                                  const Standard_Integer to) {
  Standard_Integer next = from+1;
  Standard_Integer last = to+1;
  gp_Pnt p1,p2,p3;

  if( myMaxEdges > 0 ) {
    p1 = Vertice(Edge(next++));
    p2 = Vertice(Edge(next++));
  } else {
    p1 = Vertice(next++);
    p2 = Vertice(next++);
  }

  gp_Vec vn;
  
  while ( next <= last ) {
    if( myMaxEdges > 0 ) {
      p3 = Vertice(Edge(next));
    } else {
      p3 = Vertice(next);
    }
    gp_Vec v21(p2,p1);
    gp_Vec v31(p3,p1);
    vn = v21 ^ v31;
    if( vn.SquareMagnitude() > 0. ) break;
    next++;
  }

  if( next > last ) {
#if TRACE > 0
    cout << " An item has a NULL computed facet normal" << endl;
#endif
    return;
  }

  vn.Normalize();
  if( myMaxEdges > 0 ) {
    for( int i=from+1 ; i<=to+1 ; i++ ) {
      SetVertexNormal(Edge(i),vn);
    }
  } else {
    for( int i=from+1 ; i<=to+1 ; i++ ) {
      SetVertexNormal(i,vn);
    }
  }
}

Standard_Boolean Graphic3d_ArrayOfPrimitives::IsValid() {

  if( !myPrimitiveArray ) return Standard_False;

  Standard_Integer nvertexs = myPrimitiveArray->num_vertexs;
  Standard_Integer nbounds = myPrimitiveArray->num_bounds;
  Standard_Integer nedges = myPrimitiveArray->num_edges;
  Standard_Integer i,n;

#if TRACE > 0
  Standard_CString name = StringType();
  cout << " !!! An " << name << " has " << ItemNumber() << " items" << endl;
#endif

  switch( myPrimitiveArray->type ) {
    case TelPointsArrayType:
      if( nvertexs < 1 ) {
#if TRACE > 0
        cout << " *** An " << name << " is unavailable with a too lower number of vertex " << nvertexs << endl;
#endif
        return Standard_False;
      }
      break;
    case TelPolylinesArrayType:
      if( nedges > 0 && nedges < 2 ) {
#if TRACE > 0
        cout << " *** An " << name << " is unavailable with a too lower number of edges " << nedges << endl;
#endif
        return Standard_False;
      }
      if( nvertexs < 2 ) {
#if TRACE > 0
        cout << " *** An " << name << " is unavailable with a too lower number of vertex " << nvertexs << endl;
#endif
        return Standard_False;
      }
      break;
    case TelSegmentsArrayType:
      if( nvertexs < 2 ) {
#if TRACE > 0
        cout << " *** An " << name << " is unavailable with a too lower number of vertex " << nvertexs << endl;
#endif
        return Standard_False;
      }
      break;
    case TelPolygonsArrayType:
      if( nedges > 0 && nedges < 3 ) {
#if TRACE > 0
        cout << " *** An " << name << " is unavailable with a too lower number of edges " << nedges << endl;
#endif
        return Standard_False;
      }
      if( nvertexs < 3 ) {
#if TRACE > 0
        cout << " *** An " << name << " is unavailable with a too lower number of vertex " << nvertexs << endl;
#endif
        return Standard_False;
      }
      break;
    case TelTrianglesArrayType:
      if( nedges > 0 ) {
        if( nedges < 3 || nedges % 3 != 0 ) {
#if TRACE > 0
          cout << " *** An " << name << " is unavailable with a too lower number of edges " << nedges << endl;
#endif
          if( nedges > 3 ) myPrimitiveArray->num_edges = 3 * (nedges / 3);
          else return Standard_False;
        }
      } else if( nvertexs < 3 || nvertexs % 3 != 0 ) {
#if TRACE > 0
        cout << " *** An " << name << " is unavailable with a too lower number of vertex " << nvertexs << endl;
#endif
        if( nvertexs > 3 ) myPrimitiveArray->num_vertexs = 3 * (nvertexs / 3);
        else return Standard_False;
      }
      break;
    case TelQuadranglesArrayType:
      if( nedges > 0 ) {
        if( nedges < 4 || nedges % 4 != 0 ) {
#if TRACE > 0
          cout << " *** An " << name << " is unavailable with a too lower number of edges " << nedges << endl;
#endif
          if( nedges > 4 ) myPrimitiveArray->num_edges = 4 * (nedges / 4);
          else return Standard_False;
        }
      } else if( nvertexs < 4 || nvertexs % 4 != 0 ) {
#if TRACE > 0
        cout << " *** An " << name << " is unavailable with a too lower number of vertex " << nvertexs << endl;
#endif
        if( nvertexs > 4 ) myPrimitiveArray->num_vertexs = 4 * (nvertexs / 4);
        else return Standard_False;
      }
      break;
    case TelTriangleFansArrayType:
    case TelTriangleStripsArrayType:
      if( nvertexs < 3 ) {
#if TRACE > 0
        cout << " *** An " << name << " is unavailable with a too lower number of vertex " << nvertexs << endl;
#endif
        return Standard_False;
      }
      break;
    case TelQuadrangleStripsArrayType:
      if( nvertexs < 4 ) {
#if TRACE > 0
        cout << " *** An " << name << " is unavailable with a too lower number of vertex " << nvertexs << endl;
#endif
        return Standard_False;
      }
      break;
    default:
#if TRACE > 0
      cout << " *** UNKNOWN Array of primitives type found" << endl;
#endif
      return Standard_False;
  }

  // total number of edges(verticies) in bounds should be the same as variable
  // of total number of defined edges(verticies); if no edges - only verticies 
  // could be in bounds.
  if( nbounds > 0 ) {
    for( i=n=0 ; i<nbounds ; i++ ) {
      n += myPrimitiveArray->bounds[i];
    }
    if( nedges > 0 && n != nedges ) {
#if TRACE > 0
      cout << " *** An " << name << " has an incoherent number of edges " << nedges << endl;
#endif
      if( nedges > n ) myPrimitiveArray->num_edges = n;
      else return Standard_False;
    } else if ( nedges == 0 && n != nvertexs ) {
#if TRACE > 0
      cout << " *** An " << name << " has an incoherent number of vertexs " << nvertexs << endl;
#endif
      if( nvertexs > n ) myPrimitiveArray->num_vertexs = n;
      else return Standard_False;
    }
  }

  // check that edges (indexes to an array of verticies) are in range.
  if( nedges > 0 ) {
    for( i=0 ; i<nedges ; i++ ) {
      if( myPrimitiveArray->edges[i] >= myPrimitiveArray->num_vertexs ) {
#if TRACE > 0
        cout << " *** An " << name << " has a vertex index " << myPrimitiveArray->edges[i] << " greater than the number of defined vertexs " << myPrimitiveArray->num_vertexs << endl;
#endif
        myPrimitiveArray->edges[i] = myPrimitiveArray->num_vertexs-1;
      }
    }
  }

  return Standard_True;
}
