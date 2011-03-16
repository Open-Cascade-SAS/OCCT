// Copyright: 	Matra-Datavision 1995
// File:	Select3D_SensitivePoint.cxx
// Created:	Fri Mar 10 13:41:23 1995
// Author:	Mister rmi
//		<rmi>



#include <Select3D_SensitivePoint.ixx>

#include <Select3D_Projector.hxx>
#include <Bnd_Box2d.hxx>
#include <ElCLib.hxx>
#include <CSLib_Class2d.hxx>

//==================================================
// Function: 
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
// Function: 
// Purpose :
//==================================================
void Select3D_SensitivePoint
::Project (const Select3D_Projector& aProj)
{
  Select3D_SensitiveEntity::Project(aProj); // to set the field last proj...
  gp_Pnt2d aPoint2d;
  if(!HasLocation())
    aProj.Project(mypoint, aPoint2d);
  else{
    gp_Pnt aP(mypoint.x, mypoint.y, mypoint.z);
    aProj.Project(aP.Transformed(Location().Transformation()), aPoint2d);
  }
  myprojpt = aPoint2d;
}

//==================================================
// Function: 
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
// Function: 
// Purpose :
//==================================================
Standard_Boolean Select3D_SensitivePoint
::Matches(const Standard_Real X,
	  const Standard_Real Y,
	  const Standard_Real aTol,
	  Standard_Real& DMin)
{
  DMin = gp_Pnt2d(X,Y).Distance(myprojpt);
  if(DMin<=aTol*SensitivityFactor()) {
    Select3D_SensitiveEntity::Matches(X,Y,aTol,DMin);
    return Standard_True;
  }
  return Standard_False;
}

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


