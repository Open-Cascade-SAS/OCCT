// Created on: 2003-09-29
// Created by: Alexander SOLOVYOV and Sergey LITONIN
// Copyright (c) 2003-2012 OPEN CASCADE SAS
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


#include <MeshVS_SensitiveFace.ixx>

#include <TColgp_Array1OfPnt.hxx>
#include <Select3D_Projector.hxx>
#include <CSLib_Class2d.hxx>
#include <TopLoc_Location.hxx>

//=======================================================================
// name    : MeshVS_SensitiveFace::MeshVS_SensitiveFace
// Purpose :
//=======================================================================
MeshVS_SensitiveFace::MeshVS_SensitiveFace (
                   const Handle(SelectBasics_EntityOwner)& theOwnerId,
                   const TColgp_Array1OfPnt&               thePnts,
                   const Select3D_TypeOfSensitivity        theSensitivity )
: Select3D_SensitiveFace( theOwnerId, thePnts, theSensitivity )
{
  gp_XYZ c( 0, 0, 0 );
  Standard_Integer nbPnts = thePnts.Upper() - thePnts.Lower() + 1;
  for ( Standard_Integer i = thePnts.Lower(); i <= thePnts.Upper(); i++ )
    c += thePnts( i ).XYZ();
  myCentre.SetXYZ( c / nbPnts );
}

//=======================================================================
// name    : MeshVS_SensitiveFace::Project
// Purpose :
//=======================================================================
void MeshVS_SensitiveFace::Project( const Handle(Select3D_Projector)& aProj )
{
  Select3D_SensitiveFace::Project( aProj );
  if ( HasLocation() )
    aProj->Project( myCentre.Transformed( Location().Transformation() ), myProjCentre );
  else
    aProj->Project( myCentre, myProjCentre );
}

//=======================================================================
// name    : MeshVS_SensitiveFace::Matches
// Purpose :
//=======================================================================
Standard_Boolean MeshVS_SensitiveFace::Matches( const Standard_Real XMin,
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
// name    : MeshVS_SensitiveFace::Matches
// Purpose :
//=======================================================================
Standard_Boolean MeshVS_SensitiveFace::Matches( const TColgp_Array1OfPnt2d& Polyline,
                                                const Bnd_Box2d&            aBox,
                                                const Standard_Real         aTol )
{
  Standard_Real Umin, Vmin, Umax, Vmax;
  aBox.Get ( Umin,Vmin,Umax,Vmax );
  CSLib_Class2d aClassifier2d( Polyline, aTol, aTol, Umin, Vmin, Umax, Vmax );
  Standard_Integer aRes = aClassifier2d.SiDans( myProjCentre );

  return ( aRes == 1) ;
}
