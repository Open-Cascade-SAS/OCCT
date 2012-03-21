// Created on: 1995-03-10
// Created by: Mister rmi
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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




#include <Select3D_SensitivePoint.ixx>

#include <Select3D_Projector.hxx>
#include <Bnd_Box2d.hxx>
#include <ElCLib.hxx>
#include <CSLib_Class2d.hxx>

//==================================================
// Function: Creation
// Purpose :
//==================================================

Select3D_SensitivePoint
::Select3D_SensitivePoint(const Handle(SelectBasics_EntityOwner)& anOwner,
                          const gp_Pnt& aPoint):
Select3D_SensitiveEntity(anOwner)
{
  SetSensitivityFactor(4.);
  mypoint = aPoint;
}

//==================================================
// Function: Project
// Purpose :
//==================================================

void Select3D_SensitivePoint
::Project (const Handle(Select3D_Projector)& aProj)
{
  Select3D_SensitiveEntity::Project(aProj); // to set the field last proj...
  gp_Pnt2d aPoint2d;
  if(!HasLocation())
    aProj->Project(mypoint, aPoint2d);
  else
  {
    gp_Pnt aP(mypoint.x, mypoint.y, mypoint.z);
    aProj->Project(aP.Transformed(Location().Transformation()), aPoint2d);
  }
  myprojpt = aPoint2d;
}

//==================================================
// Function: Areas
// Purpose :
//==================================================

void Select3D_SensitivePoint
::Areas(SelectBasics_ListOfBox2d& boxes)
{
  Bnd_Box2d abox;
  abox.Set(myprojpt);
  boxes.Append(abox);
}

//==================================================
// Function: Matches
// Purpose :
//==================================================

Standard_Boolean Select3D_SensitivePoint
::Matches(const Standard_Real X,
          const Standard_Real Y,
          const Standard_Real aTol,
          Standard_Real& DMin)
{
  DMin = gp_Pnt2d(X,Y).Distance(myprojpt);
  if(DMin<=aTol*SensitivityFactor())
  {
    // compute and validate the depth (::Depth()) along the eyeline
    return Select3D_SensitiveEntity::Matches(X,Y,aTol,DMin);
  }
  return Standard_False;
}

//==================================================
// Function: Matches
// Purpose :
//==================================================

Standard_Boolean Select3D_SensitivePoint::
Matches (const Standard_Real XMin,
         const Standard_Real YMin,
         const Standard_Real XMax,
         const Standard_Real YMax,
         const Standard_Real aTol)
{
  Bnd_Box2d B;
  B.Update(Min(XMin,XMax),Min(YMin,YMax),Max(XMin,XMax),Max(YMin,YMax));
  B.Enlarge(aTol);
  return !B.IsOut(myprojpt);
}

//=======================================================================
//function : Matches
//purpose  : 
//=======================================================================

Standard_Boolean Select3D_SensitivePoint::
Matches (const TColgp_Array1OfPnt2d& aPoly,
         const Bnd_Box2d& aBox,
         const Standard_Real aTol)
{ 
  Standard_Real Umin,Vmin,Umax,Vmax;
  aBox.Get(Umin,Vmin,Umax,Vmax);
  Standard_Real Tolu,Tolv;
  Tolu = 1e-7;
  Tolv = 1e-7;
  CSLib_Class2d aClassifier2d(aPoly,aTol,aTol,Umin,Vmin,Umax,Vmax);

  Standard_Integer RES = aClassifier2d.SiDans(myprojpt);
  if(RES==1) return Standard_True;
  
  return Standard_False;
}

//=======================================================================
//function : Point
//purpose  : 
//=======================================================================

gp_Pnt Select3D_SensitivePoint::Point() const
{return mypoint;}

//=======================================================================
//function : GetConnected
//purpose  : 
//=======================================================================

Handle(Select3D_SensitiveEntity) Select3D_SensitivePoint::GetConnected(const TopLoc_Location& aLoc)  
{
  Handle(Select3D_SensitivePoint) NiouEnt = new Select3D_SensitivePoint(myOwnerId,mypoint);
  if(HasLocation()) NiouEnt->SetLocation(Location());
  NiouEnt->UpdateLocation(aLoc);
  return NiouEnt;
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void Select3D_SensitivePoint::Dump(Standard_OStream& S,const Standard_Boolean FullDump) const
{
  S<<"\tSensitivePoint 3D :";
  if(HasLocation())
    S<<"\t\tExisting Location"<<endl;

  S<<"\t\t P3d [ "<<mypoint.x<<" , "<<mypoint.y<<" , "<<mypoint.z<<" ]"<<endl;
  S<<"\t\t P2d [ "<<myprojpt.x<<" , "<<myprojpt.y<<" ]"<<endl;
}

//=======================================================================
//function : ComputeDepth
//purpose  : 
//=======================================================================

Standard_Real Select3D_SensitivePoint::ComputeDepth(const gp_Lin& EyeLine) const
{
  return ElCLib::Parameter(EyeLine,mypoint);
}
