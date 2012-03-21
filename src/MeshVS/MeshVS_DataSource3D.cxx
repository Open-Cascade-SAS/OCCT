// Created on: 2005-01-21
// Created by: Alexander SOLOVYOV
// Copyright (c) 2005-2012 OPEN CASCADE SAS
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


#include <MeshVS_DataSource3D.ixx>

//================================================================
// Function : GetPrismTopology
// Purpose  :
//================================================================
Handle( MeshVS_HArray1OfSequenceOfInteger ) 
    MeshVS_DataSource3D::GetPrismTopology( const Standard_Integer BasePoints ) const
{
  if( myPrismTopos.IsBound( BasePoints ) )
    return myPrismTopos.Find( BasePoints );
  else
  {
    Handle( MeshVS_HArray1OfSequenceOfInteger ) result = CreatePrismTopology( BasePoints );
    if( !result.IsNull() )
      ( ( MeshVS_DataSource3D* )this )->myPrismTopos.Bind( BasePoints, result );
    return result;
  }
}

//================================================================
// Function : GetPyramidTopology
// Purpose  :
//================================================================
Handle( MeshVS_HArray1OfSequenceOfInteger ) 
    MeshVS_DataSource3D::GetPyramidTopology( const Standard_Integer BasePoints ) const
{
  if( myPyramidTopos.IsBound( BasePoints ) )
    return myPyramidTopos.Find( BasePoints );
  else
  {
    Handle( MeshVS_HArray1OfSequenceOfInteger ) result = CreatePyramidTopology( BasePoints );
    if( !result.IsNull() )
      ( ( MeshVS_DataSource3D* )this )->myPyramidTopos.Bind( BasePoints, result );
    return result;
  }
}

//================================================================
// Function : CreatePrismTopology
// Purpose  :
//================================================================
Handle( MeshVS_HArray1OfSequenceOfInteger ) 
    MeshVS_DataSource3D::CreatePrismTopology( const Standard_Integer BasePoints )
{
  Handle( MeshVS_HArray1OfSequenceOfInteger ) result;

  if( BasePoints>=3 )
  {
    result = new MeshVS_HArray1OfSequenceOfInteger( 1, BasePoints+2 );
    Standard_Integer i, next;

    for( i=0; i<BasePoints; i++ )
    {
      result->ChangeValue( 1 ).Prepend( i );
      result->ChangeValue( 2 ).Append( i+BasePoints );

      result->ChangeValue( 3+i ).Prepend( i );
      result->ChangeValue( 3+i ).Prepend( i+BasePoints );
      next = ( i+1 ) % BasePoints;
      result->ChangeValue( 3+i ).Prepend( next+BasePoints );
      result->ChangeValue( 3+i ).Prepend( next );
    }
  }

  return result;
}

//================================================================
// Function : CreatePyramidTopology
// Purpose  :
//================================================================
Handle( MeshVS_HArray1OfSequenceOfInteger ) 
    MeshVS_DataSource3D::CreatePyramidTopology( const Standard_Integer BasePoints )
{
  Handle( MeshVS_HArray1OfSequenceOfInteger ) result;

  if( BasePoints>=3 )
  {
    result = new MeshVS_HArray1OfSequenceOfInteger( 1, BasePoints+1 );

    for( Standard_Integer i=1; i<=BasePoints; i++ )
    {
      result->ChangeValue( 1 ).Prepend( i );
      result->ChangeValue( 1+i ).Append( 0 );
      result->ChangeValue( 1+i ).Append( i );
      result->ChangeValue( 1+i ).Append( i%BasePoints + 1 );
    }
  }

  return result;
}
