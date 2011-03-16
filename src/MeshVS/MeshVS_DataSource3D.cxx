// File:	MeshVS_DataSource3D.cxx
// Created:	Fri Jan 21 2005
// Author:	Alexander SOLOVYOV
// Copyright:	 Open CASCADE 2005

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
