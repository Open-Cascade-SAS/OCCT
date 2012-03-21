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



#include <MeshVS_SensitivePolyhedron.ixx>

#include <MeshVS_Tool.hxx>
#include <CSLib_Class2d.hxx>
#include <ElCLib.hxx>

//================================================================
// Function : Constructor MeshVS_SensitivePolyhedron
// Purpose  :
//================================================================
MeshVS_SensitivePolyhedron::
MeshVS_SensitivePolyhedron( const Handle( SelectBasics_EntityOwner )& Owner,
                            const TColgp_Array1OfPnt& Nodes,
                            const Handle( MeshVS_HArray1OfSequenceOfInteger )& Topo )
: Select3D_SensitiveEntity( Owner ),  
  myTopo( Topo )
{
  Standard_Integer low = Nodes.Lower(), up = Nodes.Upper(), i;

  myNodes   = new TColgp_HArray1OfPnt  ( low, up );
  for( i=low; i<=up; i++ )
    myNodes->SetValue( i, Nodes.Value( i ) );

  myNodes2d = new TColgp_HArray1OfPnt2d( low, up );
}

//================================================================
// Function : Project
// Purpose  :
//================================================================
void MeshVS_SensitivePolyhedron::Project( const Handle(Select3D_Projector)& aProjector )
{
  Select3D_SensitiveEntity::Project( aProjector );

  if( myNodes.IsNull() || myNodes2d.IsNull() )
    return;

  Standard_Integer low = myNodes->Lower(),
                   up  = myNodes->Upper();

  gp_Pnt   pnt;
  gp_Pnt2d proj;

  Standard_Boolean hasLoc = HasLocation();

  myCenter = gp_XY( 0, 0 );

  for( Standard_Integer i=low; i<=up; i++ )
  {
    pnt = myNodes->Value( i );
    if( !hasLoc )
      aProjector->Project( pnt, proj );
    else
      aProjector->Project( pnt.Transformed( Location().Transformation() ), proj );

    myNodes2d->SetValue( i, proj.XY() );
    myCenter += proj.XY();
  }

  myCenter /= ( up-low+1 );
}

//================================================================
// Function : GetConnected
// Purpose  :
//================================================================
Handle( Select3D_SensitiveEntity ) MeshVS_SensitivePolyhedron::GetConnected
    ( const TopLoc_Location& aLocation )
{
  Handle( MeshVS_SensitivePolyhedron ) NewEnt = new
    MeshVS_SensitivePolyhedron( myOwnerId, myNodes->Array1(), myTopo );

  if( HasLocation() )
    NewEnt->SetLocation( Location() );

  NewEnt->UpdateLocation( aLocation );

  return NewEnt;
}

//================================================================
// Function : sort
// Purpose  :
//================================================================
void sort( Standard_Real& a, Standard_Real& b )
{
  if( a>b )
  {
    Standard_Real temp = a; a = b; b = temp;
  }
}

//================================================================
// Function : Matches
// Purpose  :
//================================================================
Standard_Boolean MeshVS_SensitivePolyhedron::Matches( const Standard_Real X,
                                                      const Standard_Real Y,
                                                      const Standard_Real aTol,
                                                      Standard_Real& DMin )
{
  if( myNodes2d.IsNull() || myTopo.IsNull() )
    return Standard_False;

  Standard_Integer R1  = myTopo->Lower(),
                   R2  = myTopo->Upper(),
                   low = myNodes2d->Lower();

  Standard_Real rTol = aTol*SensitivityFactor();

  Standard_Boolean inside = Standard_False;

  // "odd-even" algorithm: with ray parallel axis of absciss and toward positive
  for( Standard_Integer i=R1; i<=R2 && !inside; i++ )
  {
    Standard_Integer intersect = 0, cur, next, C1 = 1, C2 = myTopo->Value( i ).Length();
    Standard_Real k, b,                         // y=kx+b -- equation of polygon's edge
                  x1, y1, x2, y2, xp;           // auxiliary points

    for( Standard_Integer j=C1; j<=C2; j++ )
    {
      cur = myTopo->Value( i ).Value( j );
      next = myTopo->Value( i ).Value( j<C2 ? j+1 : C1 );

      x1 = myNodes2d->Value( low+cur ).X(),
      y1 = myNodes2d->Value( low+cur ).Y(),
      x2 = myNodes2d->Value( low+next ).X(),
      y2 = myNodes2d->Value( low+next ).Y();

      if( Abs( x2-x1 )<Precision::Confusion() )
      {  
        //vertical edge!!!

        sort( y1, y2 );
        if( Y>=y1-rTol && Y<=y2+rTol && x1>X-rTol )
          intersect++;
      }
      else
      {  
        //inclined edge!!!

        k = ( y2-y1 ) / ( x2-x1 );
        b = y1 - k*x1;

        if( Abs( k )>Precision::Confusion() )
        {
          xp = ( Y-b ) / k; // absciss of point of intersection
          sort( x1, x2 );
          if( xp>=x1 && xp<=x2 && xp>X-rTol )
            intersect++;
        }
      }
    }
    inside = ( intersect%2 ) == 1;
  }

  if( inside )
  {
    return Select3D_SensitiveEntity::Matches( X, Y, aTol, DMin );
  }
  return Standard_False;
}

//================================================================
// Function : Matches
// Purpose  :
//================================================================
Standard_Boolean MeshVS_SensitivePolyhedron::Matches( const Standard_Real XMin,
                                                      const Standard_Real YMin,
                                                      const Standard_Real XMax,
                                                      const Standard_Real YMax,
                                                      const Standard_Real aTol )
{
  Standard_Real rTol = aTol*SensitivityFactor();

  return myCenter.X()>=XMin-rTol && myCenter.X()<=XMax+rTol &&
         myCenter.Y()>=YMin-rTol && myCenter.Y()<=YMax+rTol;
}

//================================================================
// Function : Matches
// Purpose  :
//================================================================
Standard_Boolean MeshVS_SensitivePolyhedron::Matches
    ( const TColgp_Array1OfPnt2d& Polyline,
      const Bnd_Box2d& aBox,
      const Standard_Real aTol )
{
  Standard_Real Umin, Vmin, Umax, Vmax;
  aBox.Get( Umin, Vmin, Umax, Vmax );

  CSLib_Class2d aClassifier2d( Polyline, aTol, aTol, Umin, Vmin, Umax, Vmax );

  Standard_Integer res = aClassifier2d.SiDans( myCenter );
  return( res==1 );
}

//================================================================
// Function : FindIntersection
// Purpose  :
//================================================================
Standard_Real MeshVS_SensitivePolyhedron::FindIntersection
    ( const TColStd_SequenceOfInteger& NodesIndices,
      const gp_Lin& EyeLine) const
{
  Standard_Real val( Precision::Infinite() );
  for( Standard_Integer i=1, n=NodesIndices.Length(); i<=n; i++ )
    val = Min( val, ElCLib::Parameter( 
      EyeLine, myNodes->Value( myNodes->Lower()+NodesIndices.Value( i ) ) ) );

  return val;
}

//================================================================
// Function : ComputeDepth
// Purpose  :
//================================================================
Standard_Real MeshVS_SensitivePolyhedron::ComputeDepth( const gp_Lin& EyeLine ) const
{
  Standard_Real val = Precision::Infinite();

  if( !myTopo.IsNull() )
    for( Standard_Integer i=myTopo->Lower(), up=myTopo->Upper(); i<=up; i++ )
      val = Min( val, FindIntersection( myTopo->Value( i ), EyeLine ) );

  return val;
}

//================================================================
// Function : ComputeSize
// Purpose  :
//================================================================
/*Standard_Real MeshVS_SensitivePolyhedron::ComputeSize() const
{
  Bnd_Box2d aBox;
  GetBox2d( aBox );

  Standard_Real aXmin, aYmin, aXmax, aYmax;
  aBox.Get( aXmin, aYmin, aXmax, aYmax );
  return Abs( ( aXmax-aXmin ) * ( aYmax-aYmin ) );
} */

//================================================================
// Function : Areas
// Purpose  :
//================================================================
void MeshVS_SensitivePolyhedron::Areas( SelectBasics_ListOfBox2d& aResult ) 
{
  Bnd_Box2d aBox;
  GetBox2d( aBox );
  aResult.Append( aBox );
}

//================================================================
// Function : GetBox2d
// Purpose  :
//================================================================
void MeshVS_SensitivePolyhedron::GetBox2d( Bnd_Box2d& aBox ) const
{
  aBox.SetVoid();

  Standard_Real xmin = 0., ymin = 0., xmax = 0., ymax = 0., x, y;
  Standard_Integer low = myNodes2d->Lower(),
                   up  = myNodes2d->Upper();

  if( !myNodes2d.IsNull() )
  {
    xmin = xmax = myNodes2d->Value( low ).X();
    ymin = ymax = myNodes2d->Value( low ).Y();
    for( Standard_Integer i=low+1; i<=up; i++ )
    {
      x = myNodes2d->Value( i ).X();
      y = myNodes2d->Value( i ).Y();
      if( x>xmax )
        xmax = x;
      else if( x<xmin )
        xmin = x;
      if( y>ymax )
        ymax = y;
      else if( y<ymin )
        ymin = y;
    }
  }

  aBox.Update( xmin, ymin, xmax, ymax );
}
