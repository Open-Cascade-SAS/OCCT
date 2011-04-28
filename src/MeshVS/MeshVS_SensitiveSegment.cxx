// File:	MeshVS_SensitiveSegment.cxx
// Created:	Mon Sep 29 2003
// Author:	Alexander SOLOVYOV and Sergey LITONIN
// Copyright:	 Open CASCADE 2003

#include <MeshVS_SensitiveSegment.ixx>
#include <Select3D_Projector.hxx>
#include <Bnd_Box2d.hxx>
#include <CSLib_Class2d.hxx>
#include <TopLoc_Location.hxx>

//=======================================================================
// name    : MeshVS_SensitiveSegment::MeshVS_SensitiveSegment
// Purpose :
//=======================================================================
MeshVS_SensitiveSegment::MeshVS_SensitiveSegment (
             const Handle(SelectBasics_EntityOwner)& theOwnerId,
             const gp_Pnt&                           theFirstP,
             const gp_Pnt&                           theLastP,
             const Standard_Integer                  theMaxRect )
: Select3D_SensitiveSegment( theOwnerId, theFirstP, theLastP, theMaxRect )
{
  myCentre.SetXYZ( ( theFirstP.XYZ() + theLastP.XYZ() ) / 2 );
}

//=======================================================================
// name    : MeshVS_SensitiveSegment::Project
// Purpose :
//=======================================================================
void MeshVS_SensitiveSegment::Project( const Handle(Select3D_Projector)& aProj )
{
  Select3D_SensitiveSegment::Project( aProj );
  if ( HasLocation() )
    aProj->Project( myCentre.Transformed( Location().Transformation() ), myProjCentre );
  else
    aProj->Project( myCentre, myProjCentre );
}

//=======================================================================
// name    : MeshVS_SensitiveSegment::Matches
// Purpose :
//=======================================================================
Standard_Boolean MeshVS_SensitiveSegment::Matches( const Standard_Real XMin,
                                                   const Standard_Real YMin,
                                                   const Standard_Real XMax,
                                                   const Standard_Real YMax,
                                                   const Standard_Real aTol )
{
  Bnd_Box2d aBox;
  aBox.Update( XMin-aTol, YMin-aTol, XMax+aTol, YMax+aTol );
  return !aBox.IsOut( myProjCentre );
}

//=======================================================================
// name    : MeshVS_SensitiveSegment::Matches
// Purpose :
//=======================================================================
Standard_Boolean MeshVS_SensitiveSegment::Matches( const TColgp_Array1OfPnt2d& Polyline,
                                                   const Bnd_Box2d&            aBox,
                                                   const Standard_Real         aTol )
{
  Standard_Real Umin, Vmin, Umax, Vmax;
  aBox.Get ( Umin,Vmin,Umax,Vmax );
  CSLib_Class2d aClassifier2d( Polyline, aTol, aTol, Umin, Vmin, Umax, Vmax );
  Standard_Integer aRes = aClassifier2d.SiDans( myProjCentre );

  return ( aRes == 1) ;
}
