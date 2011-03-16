// Copyright: 	Matra-Datavision 1995
// File:	Select3D_SensitiveBox.cxx
// Created:	Thu Apr 13 10:28:17 1995
// Author:	Robert COUBLANC
//		<rob>



#include <Select3D_SensitiveBox.ixx>
#include <gp_Pnt2d.hxx>
#include <gp_Pnt.hxx>
#include <Bnd_Box.hxx>
#include <ElCLib.hxx>



//==================================================
// Function: Constructor
// Purpose :
//==================================================

Select3D_SensitiveBox::Select3D_SensitiveBox(const Handle(SelectBasics_EntityOwner)& OwnerId,
					     const Bnd_Box& BBox):
Select3D_SensitiveEntity(OwnerId),
mybox3d(BBox){}


//==================================================
// Function: Constructor
// Purpose :
//==================================================

Select3D_SensitiveBox::
Select3D_SensitiveBox(const Handle(SelectBasics_EntityOwner)& OwnerId,
		      const Standard_Real XMin,
		      const Standard_Real YMin,
		      const Standard_Real ZMin,
		      const Standard_Real XMax,
		      const Standard_Real YMax,
		      const Standard_Real ZMax):
Select3D_SensitiveEntity(OwnerId)
{
  mybox3d.Update(XMin,YMin,ZMin,XMax,YMax,ZMax);
}


//==================================================
// Function: Project
// Purpose :
//==================================================
void Select3D_SensitiveBox::
Project(const Select3D_Projector& aProj)
{
  Select3D_SensitiveEntity::Project(aProj); // to set the field last proj...
  
  if(HasLocation()){
    Bnd_Box B = mybox3d.Transformed(Location().Transformation());
    ProjectBox(aProj,B);
  }
  else
    ProjectBox(aProj,mybox3d);
}

//==================================================
// Function: 
// Purpose :
//==================================================
void Select3D_SensitiveBox::
Areas(SelectBasics_ListOfBox2d& aSeq)
{  aSeq.Append(mybox2d);}


//=======================================================================
//function : GetConnected
//purpose  : 
//=======================================================================

Handle(Select3D_SensitiveEntity) Select3D_SensitiveBox::GetConnected(const TopLoc_Location& aLoc) 
{
  Handle(Select3D_SensitiveBox) NiouEnt =  new Select3D_SensitiveBox(myOwnerId,mybox3d);
  
  if(HasLocation()) NiouEnt->SetLocation(Location());
  NiouEnt->UpdateLocation(aLoc);
  return NiouEnt;
}



//==================================================
// Function: 
// Purpose :
//==================================================
Standard_Boolean Select3D_SensitiveBox::
Matches(const Standard_Real X,
	const Standard_Real Y,
	const Standard_Real aTol,
	Standard_Real& DMin)
{
  Select3D_SensitiveEntity::Matches(X,Y,aTol,DMin);
  DMin=0.;
  
  return Standard_True;
  
}

//==================================================
// Function: 
// Purpose :
//==================================================

Standard_Boolean Select3D_SensitiveBox::
Matches (const Standard_Real XMin,
	 const Standard_Real YMin,
	 const Standard_Real XMax,
	 const Standard_Real YMax,
	 const Standard_Real aTol)
{
  Bnd_Box2d BoundBox;
  BoundBox.Update(XMin-aTol,YMin-aTol,XMax+aTol,YMax+aTol);
  return(!BoundBox.IsOut(mybox2d));
}


//=======================================================================
//function : Matches
//purpose  : 
//=======================================================================

Standard_Boolean Select3D_SensitiveBox::
Matches (const TColgp_Array1OfPnt2d& aPoly,
	 const Bnd_Box2d& aBox,
	 const Standard_Real aTol)
{ 
  return(!aBox.IsOut(mybox2d));
}


//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void Select3D_SensitiveBox::Dump(Standard_OStream& S,const Standard_Boolean FullDump) const
{
  S<<"\tSensitiveBox 3D :\n";
  if(HasLocation())
    S<<"\t\tExisting Location"<<endl;
  
  Standard_Real XMin,YMin,ZMin,XMax,YMax,ZMax;
  mybox3d.Get(XMin,YMin,ZMin,XMax,YMax,ZMax);
  
  S<<"\t\t PMin [ "<<XMin<<" , "<<YMin<<" , "<<ZMin<<" ]";
  S<<"\t\t PMax [ "<<XMax<<" , "<<YMax<<" , "<<ZMax<<" ]"<<endl;

  if(FullDump){
//    S<<"\t\t\tOwner:"<<myOwnerId<<endl;
    Select3D_SensitiveEntity::DumpBox(S,mybox2d);}
  
}


//=======================================================================
//function : ProjectBox
//purpose  : 
//=======================================================================

void Select3D_SensitiveBox::ProjectBox(const Select3D_Projector& aPrj,
				       const Bnd_Box& aBox) 
{
  mybox2d.SetVoid();
  gp_Pnt2d curp2d;
  Standard_Real XMin,YMin,ZMin,XMax,YMax,ZMax;
  aBox.Get(XMin,YMin,ZMin,XMax,YMax,ZMax);
  
  aPrj.Project(gp_Pnt(XMin,YMin,ZMin),curp2d);
  mybox2d.Update(curp2d.X(),curp2d.Y());
  aPrj.Project(gp_Pnt(XMax,YMin,ZMin),curp2d);
  mybox2d.Update(curp2d.X(),curp2d.Y());
  aPrj.Project(gp_Pnt(XMax,YMax,ZMin),curp2d);
  mybox2d.Update(curp2d.X(),curp2d.Y());
  aPrj.Project(gp_Pnt(XMin,YMax,ZMin),curp2d);
  mybox2d.Update(curp2d.X(),curp2d.Y());
  aPrj.Project(gp_Pnt(XMin,YMin,ZMax),curp2d);
  mybox2d.Update(curp2d.X(),curp2d.Y());
  aPrj.Project(gp_Pnt(XMax,YMin,ZMax),curp2d);
  mybox2d.Update(curp2d.X(),curp2d.Y());
  aPrj.Project(gp_Pnt(XMax,YMax,ZMax),curp2d);
  mybox2d.Update(curp2d.X(),curp2d.Y());
  aPrj.Project(gp_Pnt(XMin,YMax,ZMax),curp2d);
  mybox2d.Update(curp2d.X(),curp2d.Y());
}

//=======================================================================
//function : ComputeDepth
//purpose  : 
//=======================================================================

Standard_Real Select3D_SensitiveBox::ComputeDepth(const gp_Lin& EyeLine) const
{
  Standard_Real XMin,YMin,ZMin,XMax,YMax,ZMax;
  mybox3d.Get(XMin,YMin,ZMin,XMax,YMax,ZMax);
  gp_Pnt PMid((XMin+XMax)/2.,(YMin+YMax)/2.,(ZMin+ZMax)/2.);
  return ElCLib::Parameter(EyeLine,PMid);
}
