// Created on: 1995-03-13
// Created by: Robert COUBLANC
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




#include <Select3D_SensitiveCurve.ixx>
#include <SelectBasics_BasicTool.hxx>
#include <gp_Lin2d.hxx>
#include <Precision.hxx>
#include <ElCLib.hxx>
#include <CSLib_Class2d.hxx>


//==================================================
// Function: Creation
// Purpose :
//==================================================

Select3D_SensitiveCurve
::Select3D_SensitiveCurve(const Handle(SelectBasics_EntityOwner)& OwnerId,
                          const Handle(Geom_Curve)& C,
                          const Standard_Integer NbPoints):
Select3D_SensitivePoly(OwnerId, NbPoints),
mylastseg(0), 
myCurve(C)
{
  LoadPoints(C,NbPoints);
}

//==================================================
// Function: Creation
// Purpose :
//==================================================

Select3D_SensitiveCurve
::Select3D_SensitiveCurve(const Handle(SelectBasics_EntityOwner)& OwnerId,
                          const Handle(TColgp_HArray1OfPnt)& ThePoints):
Select3D_SensitivePoly(OwnerId, ThePoints),
mylastseg(0)
{
}

//==================================================
// Function: Creation
// Purpose :
//==================================================

Select3D_SensitiveCurve
::Select3D_SensitiveCurve(const Handle(SelectBasics_EntityOwner)& OwnerId,
                          const TColgp_Array1OfPnt& ThePoints):
Select3D_SensitivePoly(OwnerId, ThePoints),
mylastseg(0)
{
}

//==================================================
// Function: Matches
// Purpose :
//==================================================

Standard_Boolean Select3D_SensitiveCurve
::Matches(const Standard_Real X,
          const Standard_Real Y,
          const Standard_Real aTol,
          Standard_Real& DMin)
{
  Standard_Integer Rank;
  TColgp_Array1OfPnt2d aArrayOf2dPnt(1, mypolyg.Size());
  Points2D(aArrayOf2dPnt);
  if (SelectBasics_BasicTool::MatchPolyg2d (aArrayOf2dPnt,
                                            X, Y,
                                            aTol,
                                            DMin,
                                            Rank))
  {
    mylastseg = Rank;
    // compute and validate the depth (::Depth()) along the eyeline
    return Select3D_SensitiveEntity::Matches (X, Y, aTol, DMin);
  }
  return Standard_False;
}

//==================================================
// Function: Matches
// Purpose : know if a box touches the projected polygon
//           of the Curve.
//==================================================

Standard_Boolean Select3D_SensitiveCurve::
Matches (const Standard_Real XMin,
         const Standard_Real YMin,
         const Standard_Real XMax,
         const Standard_Real YMax,
         const Standard_Real aTol)
{
  Bnd_Box2d BoundBox;
  BoundBox.Update(XMin-aTol,YMin-aTol,XMax+aTol,YMax+aTol);
  
  for(Standard_Integer anIndex=0; anIndex<mypolyg.Size(); ++anIndex)
    {
      if(BoundBox.IsOut(mypolyg.Pnt2d(anIndex)))
        return Standard_False;
    }
  return Standard_True;
}

//=======================================================================
//function : Matches
//purpose  : 
//=======================================================================

Standard_Boolean Select3D_SensitiveCurve::
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

  for(Standard_Integer anIndex=0;anIndex<mypolyg.Size();++anIndex)
  {
    Standard_Integer RES = aClassifier2d.SiDans(mypolyg.Pnt2d(anIndex));
    if(RES!=1)
      return Standard_False;
  }
  return Standard_True;
}

//==================================================
// Function: LoadPoints
// Purpose :
//==================================================

void Select3D_SensitiveCurve
::LoadPoints (const Handle(Geom_Curve)& aCurve,const Standard_Integer NbP)
{
  /*this method is private and it used only inside of constructor. 
    That's why check !NbP==mypolyg3d->Length() was removed*/

  Standard_Real Step = (aCurve->LastParameter()- aCurve->FirstParameter())/(NbP-1);
  Standard_Real Curparam = aCurve->FirstParameter();
  for(Standard_Integer anIndex=0;anIndex<mypolyg.Size();++anIndex)
  {
    mypolyg.SetPnt(anIndex, aCurve->Value(Curparam));
    Curparam+=Step;
  }
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void Select3D_SensitiveCurve::Dump(Standard_OStream& S,const Standard_Boolean FullDump) const 
{
  S<<"\tSensitiveCurve 3D :"<<endl;
  if (HasLocation())
    S<<"\t\tExisting Location"<<endl;

  S<<"\t\tNumber Of Points :"<<mypolyg.Size()<<endl;

  if(FullDump)
  {
    Select3D_SensitiveEntity::DumpBox(S,mybox2d);
  }
}

//=======================================================================
//function : ComputeDepth
//purpose  : 
//=======================================================================

Standard_Real Select3D_SensitiveCurve::ComputeDepth(const gp_Lin& EyeLine) const
{
  Standard_Real aDepth = Precision::Infinite();

  // Not implemented
  if(mylastseg==0)
    return aDepth;

  gp_XYZ aCDG;
  // In case if mylastseg and mylastseg+1 are not valid
  // the depth will be infinite
  if (mylastseg < mypolyg.Size())
  {
    aCDG = mypolyg.Pnt(mylastseg);
    if (mylastseg+1 < mypolyg.Size())
    {
      aCDG += mypolyg.Pnt(mylastseg+1);
      aCDG /= 2.;
    }
    aDepth = ElCLib::Parameter(EyeLine,gp_Pnt(aCDG));
  }

  return aDepth;
}

//=======================================================================
//function : GetConnected
//purpose  : 
//======================================================================= 

Handle(Select3D_SensitiveEntity) Select3D_SensitiveCurve::GetConnected(const TopLoc_Location &theLocation) 
{
  // Create a copy of this 
  Handle(Select3D_SensitiveEntity) aNewEntity;
  // this was constructed using Handle(Geom_Curve) 
  if (!myCurve.IsNull()) 
  {
    aNewEntity = new Select3D_SensitiveCurve(myOwnerId, myCurve);
  }
  // this was constructed using TColgp_HArray1OfPnt
  else 
  {
    Standard_Integer aSize = mypolyg.Size();
    Handle(TColgp_HArray1OfPnt) aPoints = new TColgp_HArray1OfPnt(1, aSize);
    // Fill the array with points from mypolyg3d
    for (Standard_Integer anIndex = 1; anIndex <= aSize; ++anIndex) 
    {
      aPoints->SetValue(anIndex, mypolyg.Pnt(anIndex-1));
    }
     aNewEntity = new Select3D_SensitiveCurve(myOwnerId, aPoints);
  }
  
  if (HasLocation()) 
    aNewEntity->SetLocation(Location()); 

  aNewEntity->UpdateLocation(theLocation);

  return aNewEntity;
}