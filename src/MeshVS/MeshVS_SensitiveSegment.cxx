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
