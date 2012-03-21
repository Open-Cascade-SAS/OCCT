// Created on: 1995-03-27
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

//Modif on jun-24-97 : introduction de CSLib_Class2d de LBR
//                     pour teste si on est dedans ou dehors...
//Modif on jul-21-97 : changement en harray1 pour eventuelles connexions ...

#include <Select3D_SensitiveFace.ixx>
#include <Select3D_Projector.hxx>
#include <SelectBasics_BasicTool.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <ElCLib.hxx>

#include <CSLib_Class2d.hxx>


#define AutoInterMask  0x01
#define AutoComputeMask  0x02
// Standard_True if the flag is one
#define AutoInterFlag(aflag)  ( aflag & AutoInterMask )
#define AutoComputeFlag(aflag)  ( aflag & AutoComputeMask )
// set the flag to one
#define SetAutoInterFlag(aflag)  ( aflag = aflag & AutoInterMask)
#define SetAutoComputeFlag(aflag)  ( aflag = aflag & AutoComputeMask)
// Initialize flags
#define AutoInitFlags(aflag) (aflag = 0)

//==================================================
// Function: Hide this constructor to the next version...
// Purpose : simply avoid interfering with the version update
//==================================================

Select3D_SensitiveFace::
Select3D_SensitiveFace(const Handle(SelectBasics_EntityOwner)& OwnerId,
                       const TColgp_Array1OfPnt& ThePoints,
                       const Select3D_TypeOfSensitivity aType):
Select3D_SensitivePoly(OwnerId, ThePoints),
mytype (aType),
myDetectedIndex(-1)
{
  AutoInitFlags(myautointer);
}

//==================================================
// Function: Creation
// Purpose :
//==================================================

Select3D_SensitiveFace::
Select3D_SensitiveFace(const Handle(SelectBasics_EntityOwner)& OwnerId,
                       const Handle(TColgp_HArray1OfPnt)& ThePoints,
                       const Select3D_TypeOfSensitivity aType):
Select3D_SensitivePoly(OwnerId, ThePoints),
mytype (aType),
myDetectedIndex(-1)
{
  AutoInitFlags(myautointer);
}

//==================================================
// Function: Matches
// Purpose :
//==================================================

Standard_Boolean Select3D_SensitiveFace::
Matches(const Standard_Real X,
        const Standard_Real Y,
        const Standard_Real aTol,
        Standard_Real& DMin)
{
#ifndef DEB
  Standard_Real DMin2 = 0.;
#else
  Standard_Real DMin2;
#endif
  Standard_Real Xmin,Ymin,Xmax,Ymax;
  if(!Bnd_Box2d(mybox2d).IsVoid())
  {
    Bnd_Box2d(mybox2d).Get(Xmin,Ymin,Xmax,Ymax);
    DMin2 = gp_XY(Xmax-Xmin,Ymax-Ymin).SquareModulus();
  }
  // calculation of a criterion of minimum distance...
  // from start Dmin = size of the bounding box 2D,
  // then min. distance of the polyhedron or cdg...

  Standard_Real aTol2 = aTol*aTol;
  Standard_Integer aSize = mypolyg.Size(), anIndex;
  gp_XY CDG;
  for(anIndex=0;anIndex<aSize;++anIndex)
  {
    CDG+=mypolyg.Pnt2d(anIndex);
  }

  if(aSize>1)
  {
    CDG/=(aSize-1);
  }
  DMin2=Min(DMin2,gp_XY(CDG.X()-X,CDG.Y()-Y).SquareModulus());
  DMin = Sqrt(DMin2);

  Standard_Boolean isplane2d(Standard_True);

  for(anIndex=1;anIndex<aSize;++anIndex)
  {
    gp_XY V1(mypolyg.Pnt2d(anIndex)),V(X,Y);
    V1-=mypolyg.Pnt2d(anIndex-1);
    V-=mypolyg.Pnt2d(anIndex-1);
    Standard_Real Vector = V1^V;
    Standard_Real V1V1 = V1.SquareModulus();
    DMin2 = 
      (V1V1 <=aTol2) ? 
    Min(DMin2,V.SquareModulus()): // if the segment is too small...
      Min(DMin2,Vector*Vector/V1V1);
    //cdg ...
    gp_XY PlaneTest(CDG);
    PlaneTest-=mypolyg.Pnt2d(anIndex-1);
    Standard_Real valtst = PlaneTest^V1;
    if(isplane2d && Abs(valtst)>aTol) isplane2d=Standard_False;
  }
  if (isplane2d)
  {
    return Select3D_SensitiveEntity::Matches(X,Y,aTol,DMin);
  }

  //otherwise it is checked if the point is in the face...
  TColgp_Array1OfPnt2d aArrayOf2dPnt(1, aSize);
  Points2D(aArrayOf2dPnt);
  CSLib_Class2d TheInOutTool(aArrayOf2dPnt,aTol,aTol,Xmin,Ymin,Xmax,Ymax);
  Standard_Integer TheStat = TheInOutTool.SiDans(gp_Pnt2d(X,Y));

  Standard_Boolean res(Standard_False);
  switch(TheStat)
  {
  case 0:
    res = Standard_True;
  case 1:
    {
      if(mytype!=Select3D_TOS_BOUNDARY)
        res = Standard_True;
    }
  }
  if (res)
  {
    return Select3D_SensitiveEntity::Matches(X,Y,aTol,DMin);
  }
  return Standard_False;
}

//=======================================================================
//function : Matches
//purpose  : 
//=======================================================================

Standard_Boolean Select3D_SensitiveFace::
Matches (const Standard_Real XMin,
         const Standard_Real YMin,
         const Standard_Real XMax,
         const Standard_Real YMax,
         const Standard_Real aTol)
{
  Bnd_Box2d BoundBox;
  BoundBox.Update(XMin-aTol,YMin-aTol,XMax+aTol,YMax+aTol);

  for(Standard_Integer anIndex=0;anIndex<mypolyg.Size();++anIndex)
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

Standard_Boolean Select3D_SensitiveFace::
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

  gp_Pnt2d aPnt2d;
  for(Standard_Integer anIndex=0;anIndex<mypolyg.Size();++anIndex)
  {
    Standard_Integer RES = aClassifier2d.SiDans(mypolyg.Pnt2d(anIndex));
    if(RES!=1)
      return Standard_False;
  }
  return Standard_True;
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void Select3D_SensitiveFace::Dump(Standard_OStream& S,const Standard_Boolean FullDump) const
{
  S<<"\tSensitiveFace 3D :"<<endl;;
  if(HasLocation())
    S<<"\t\tExisting Location"<<endl;
  
  if(mytype==Select3D_TOS_BOUNDARY) 
    S<<"\t\tSelection Of Bounding Polyline Only"<<endl;
  
  if(FullDump)
  {
    S<<"\t\tNumber Of Points :"<<mypolyg.Size()<<endl;
    Select3D_SensitiveEntity::DumpBox(S,mybox2d);
  }
}

//=======================================================================
//function : ComputeDepth
//purpose  : 
//=======================================================================

Standard_Real Select3D_SensitiveFace::ComputeDepth(const gp_Lin& EyeLine) const
{
  Standard_Real aDepth = Precision::Infinite();

  Standard_Real aDepthMin = !mylastprj.IsNull() ? mylastprj->DepthMin() : -Precision::Infinite();
  Standard_Real aDepthMax = !mylastprj.IsNull() ? mylastprj->DepthMax() :  Precision::Infinite();
  Standard_Real aDepthTest;

  for (Standard_Integer anIndex = 0; anIndex < mypolyg.Size()-1; ++anIndex)
  {
    aDepthTest = ElCLib::Parameter (EyeLine, mypolyg.Pnt(anIndex));
    if (aDepthTest < aDepth && (aDepthTest > aDepthMin) && (aDepthTest < aDepthMax))
    {
      aDepth = aDepthTest;
    }
  }
  return aDepth;
}

//=======================================================================
//function : GetConnected
//purpose  : 
//=======================================================================

Handle(Select3D_SensitiveEntity) Select3D_SensitiveFace::GetConnected(const TopLoc_Location &theLocation) 
{
  // Create a copy of this
  Standard_Integer aSize = mypolyg.Size();
  TColgp_Array1OfPnt aPoints(1, aSize);
  for (Standard_Integer anIndex = 1; anIndex <= aSize; ++anIndex)
  {
    aPoints.SetValue(anIndex, mypolyg.Pnt(anIndex-1));
  }

  Handle(Select3D_SensitiveEntity) aNewEntity =
    new Select3D_SensitiveFace(myOwnerId, aPoints, mytype);

  if (HasLocation())
    aNewEntity->SetLocation(Location());

  aNewEntity->UpdateLocation(theLocation);

  return aNewEntity;
}