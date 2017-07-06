// Created on: 2003-09-16
// Created by: Alexander SOLOVYOV
// Copyright (c) 2003-2014 OPEN CASCADE SAS
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


#include <Bnd_Box.hxx>
#include <Bnd_Box2d.hxx>
#include <gp.hxx>
#include <gp_Vec.hxx>
#include <MeshVS_Buffer.hxx>
#include <MeshVS_DataSource.hxx>
#include <MeshVS_Mesh.hxx>
#include <MeshVS_Tool.hxx>
#include <Standard_Type.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_HPackedMapOfInteger.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>

IMPLEMENT_STANDARD_RTTIEXT(MeshVS_DataSource,Standard_Transient)

//================================================================
// Function : Get3DGeom
// Purpose  :
//================================================================
Standard_Boolean MeshVS_DataSource::Get3DGeom( const Standard_Integer /*ID*/, 
                                               Standard_Integer& /*NbNodes*/,
                                               Handle( MeshVS_HArray1OfSequenceOfInteger )& /*Data*/ ) const
{
  return Standard_False;
}

//================================================================
// Function : GetNormal
// Purpose  :
//================================================================
Standard_Boolean MeshVS_DataSource::GetNormal ( const Standard_Integer Id,
                                                const Standard_Integer Max,
                                                Standard_Real &nx,
                                                Standard_Real &ny,
                                                Standard_Real &nz ) const
{
  if ( Max <= 0 )
    return Standard_False;

  MeshVS_Buffer aCoordsBuf (3*Max*sizeof(Standard_Real));
  TColStd_Array1OfReal Coords ( aCoordsBuf, 1, 3*Max );
  Standard_Integer nbNodes;
  MeshVS_EntityType Type;

  Standard_Boolean res = Standard_False;

  if ( !GetGeom ( Id, Standard_True, Coords, nbNodes, Type ) )
    return res;

  if ( Type == MeshVS_ET_Face && nbNodes >= 3 )
  {
    Standard_Real x1 = Coords( 1 );
    Standard_Real y1 = Coords( 2 );
    Standard_Real z1 = Coords( 3 );
    Standard_Real x2 = Coords( 4 );
    Standard_Real y2 = Coords( 5 );
    Standard_Real z2 = Coords( 6 );
    Standard_Real x3 = Coords( ( nbNodes - 1 ) * 3 + 1 );
    Standard_Real y3 = Coords( ( nbNodes - 1 ) * 3 + 2 );
    Standard_Real z3 = Coords( ( nbNodes - 1 ) * 3 + 3 );
    Standard_Real p1 = x2 - x1, p2 = y2 - y1, p3 = z2 - z1,
                  q1 = x3 - x1, q2 = y3 - y1, q3 = z3 - z1;
    nx = p2*q3 - p3*q2;
    ny = p3*q1 - p1*q3;
    nz = p1*q2 - p2*q1;
    Standard_Real len = sqrt ( nx*nx + ny*ny + nz*nz );
    if ( len <= gp::Resolution() )
    {
      nx = ny = nz = 0;
      return res;
    }
    nx /= len; ny /= len; nz /= len;
    res = Standard_True;
  }
  return res;
}

//================================================================
// Function : GetNodeNormal
// Purpose  :
//================================================================
Standard_Boolean MeshVS_DataSource::GetNodeNormal
                                ( const Standard_Integer /*ranknode*/,
                                  const Standard_Integer /*Id*/,
                                  Standard_Real &/*nx*/,
                                  Standard_Real &/*ny*/,
                                  Standard_Real &/*nz*/ ) const
{
  return Standard_False;
}

//================================================================
// Function : GetNormalsByElement
// Purpose  :
//================================================================
Standard_Boolean MeshVS_DataSource::GetNormalsByElement(const Standard_Integer Id,
                                                        const Standard_Boolean IsNodal,
                                                        const Standard_Integer MaxNodes,
                                                        Handle(TColStd_HArray1OfReal)& Normals) const
{
  MeshVS_Buffer aCoordsBuf (3*MaxNodes*sizeof(Standard_Real));
  TColStd_Array1OfReal Coords ( aCoordsBuf, 1, 3*MaxNodes );
  Standard_Integer NbNodes;
  MeshVS_EntityType Type;

  Standard_Boolean res = Standard_False;
  if ( MaxNodes <= 0 )
    return res;

  if ( !GetGeom ( Id, Standard_True, Coords, NbNodes, Type ) )
    return res;

  Standard_Integer aNbNormals = NbNodes;

  Handle( MeshVS_HArray1OfSequenceOfInteger ) aTopo;
  if ( Type == MeshVS_ET_Volume )
  {
    if( !Get3DGeom( Id, NbNodes, aTopo ) )
          return res;
    // calculate number of normals for faces of volume 
    aNbNormals = aTopo->Upper() - aTopo->Lower() + 1;
  }

  Handle(TColStd_HArray1OfReal) aNormals = new TColStd_HArray1OfReal(1, 3 * aNbNormals );

  Standard_Boolean allNormals = ( Type == MeshVS_ET_Face && IsNodal );
  // Try returning nodal normals if possible
  for( Standard_Integer k=1; k<=NbNodes && allNormals; k++ )
    allNormals = GetNodeNormal( k, 
                                Id, 
                                aNormals->ChangeValue(3 * k - 2), 
                                aNormals->ChangeValue(3 * k - 1), 
                                aNormals->ChangeValue(3 * k    ) );

  // Nodal normals not available or not needed
  if ( !allNormals )
  {
    switch ( Type )
    {
    // Compute a face normal and duplicate it for all element`s nodes
    case MeshVS_ET_Face:
      res = GetNormal( Id, 
                       MaxNodes, 
                       aNormals->ChangeValue(1), 
                       aNormals->ChangeValue(2), 
                       aNormals->ChangeValue(3) );
      if ( res )
      {
        for( Standard_Integer k=2; k<=NbNodes; k++ )
        {
          aNormals->ChangeValue(3 * k - 2) = aNormals->Value(1);
          aNormals->ChangeValue(3 * k - 1) = aNormals->Value(2);
          aNormals->ChangeValue(3 * k    ) = aNormals->Value(3);
        }
      }
      break;

    // Compute normals for each of volum`s faces - not for each node!
    case MeshVS_ET_Volume:
      {
        gp_Vec norm;
        Standard_Integer low = Coords.Lower();
        for ( Standard_Integer k = aTopo->Lower(), last = aTopo->Upper(), i = 1; k <= last; k++, i++ )
        {
          const TColStd_SequenceOfInteger& aSeq = aTopo->Value( k );
          Standard_Integer m = aSeq.Length(), ind;

          norm.SetCoord( 0, 0, 0 );
          MeshVS_Buffer PolyNodesBuf (3*m*sizeof(Standard_Real));
          TColStd_Array1OfReal PolyNodes( PolyNodesBuf, 0, 3*m );
          PolyNodes.SetValue( 0, m );
          for( Standard_Integer j=1; j<=m; j++ )
          {          
            ind = aSeq.Value( j );
            PolyNodes.SetValue( 3*j-2, Coords( low+3*ind   ) );
            PolyNodes.SetValue( 3*j-1, Coords( low+3*ind+1 ) );
            PolyNodes.SetValue( 3*j,   Coords( low+3*ind+2 ) );
          }

          MeshVS_Tool::GetAverageNormal( PolyNodes, norm );

          aNormals->ChangeValue(i * 3 - 2) = norm.X();
          aNormals->ChangeValue(i * 3 - 1) = norm.Y();
          aNormals->ChangeValue(i * 3    ) = norm.Z();
        }
        res = Standard_True;
      }
      break;

    default:
      return res;
    } // switch ( Type )
  } // if ( !allNormals )

  if ( res || allNormals )
    Normals = aNormals;

  return ( res || allNormals );
}

//================================================================
// Function : GetAllGroups
// Purpose  :
//================================================================
void MeshVS_DataSource::GetAllGroups( TColStd_PackedMapOfInteger& /*Ids*/ ) const
{
}

//================================================================
// Function : GetGroup
// Purpose  :
//================================================================
Standard_Boolean MeshVS_DataSource::GetGroup( const Standard_Integer /*Id*/,
                                              MeshVS_EntityType& Type,
                                              TColStd_PackedMapOfInteger& /*Ids*/ )  const
{
  Type = MeshVS_ET_NONE;
  return Standard_False;
}


//================================================================
// Function : GetGroupAddr
// Purpose  :
//================================================================
Standard_Address MeshVS_DataSource::GetGroupAddr(const Standard_Integer /*ID*/) const
{
  return NULL;
}

//================================================================
// Function : IsAdvancedSelectionEnabled
// Purpose  :
//================================================================
Standard_Boolean MeshVS_DataSource::IsAdvancedSelectionEnabled() const
{
  return Standard_False;
}

//================================================================
// Function : GetDetectedEntities
// Purpose  :
//================================================================
Standard_Boolean MeshVS_DataSource::GetDetectedEntities(const Handle(MeshVS_Mesh)& /*theMesh*/, 
                                                        const Standard_Real     /*X*/,
                                                        const Standard_Real     /*Y*/,
                                                        const Standard_Real     /*aTol*/,
                                                        Handle(TColStd_HPackedMapOfInteger)& /*Nodes*/,
                                                        Handle(TColStd_HPackedMapOfInteger)& /*Elements*/,
                                                        Standard_Real& /*DMin*/)
{
  return Standard_False;
}

//================================================================
// Function : GetDetectedEntities
// Purpose  :
//================================================================
Standard_Boolean MeshVS_DataSource::GetDetectedEntities(const Handle(MeshVS_Mesh)& /*theMesh*/, 
							const Standard_Real     /*XMin*/,
							const Standard_Real     /*YMin*/,
							const Standard_Real     /*XMax*/,
							const Standard_Real     /*YMax*/,
							const Standard_Real     /*aTol*/,
							Handle(TColStd_HPackedMapOfInteger)& /*Nodes*/,
							Handle(TColStd_HPackedMapOfInteger)& /*Elements*/) 
{
  return Standard_False;
}

//================================================================
// Function : GetDetectedEntities
// Purpose  :
//================================================================
Standard_Boolean MeshVS_DataSource::GetDetectedEntities(const Handle(MeshVS_Mesh)& /*theMesh*/, 
							const TColgp_Array1OfPnt2d& /*Polyline*/,
							const Bnd_Box2d&            /*aBox*/,
							const Standard_Real         /*aTol*/,
							Handle(TColStd_HPackedMapOfInteger)& /*Nodes*/,
							Handle(TColStd_HPackedMapOfInteger)& /*Elements*/)
{
  return Standard_False;
}

//================================================================
// Function : GetDetectedEntities
// Purpose  :
//================================================================
Standard_Boolean MeshVS_DataSource::GetDetectedEntities(const Handle(MeshVS_Mesh)& /*theMesh*/,
                                                        Handle(TColStd_HPackedMapOfInteger)& /*Nodes*/,
                                                        Handle(TColStd_HPackedMapOfInteger)& /*Elements*/)
{
  return Standard_False;
}

//================================================================
// Function : GetBoundingBox
// Purpose  :
//================================================================
Bnd_Box MeshVS_DataSource::GetBoundingBox() const
{
  // Compute the 3D bounding box for mesh
  Bnd_Box aBox;
  const TColStd_PackedMapOfInteger& aNodes = GetAllNodes();
  if( aNodes.Extent() )
  {
    Standard_Real aCoordsBuf[ 3 ];
    TColStd_Array1OfReal aCoords (*aCoordsBuf, 1, 3);
    Standard_Integer nbNodes;
    MeshVS_EntityType aType;
    TColStd_MapIteratorOfPackedMapOfInteger anIter (aNodes);
    for ( ; anIter.More(); anIter.Next() )
    {
      Standard_Integer aKey = anIter.Key();
      if ( !GetGeom ( aKey, Standard_False, aCoords, nbNodes, aType ) )
        continue;
      aBox.Add( gp_Pnt( aCoordsBuf[0], aCoordsBuf[1], aCoordsBuf[2] ) );
    }
  }
  return aBox;
}
