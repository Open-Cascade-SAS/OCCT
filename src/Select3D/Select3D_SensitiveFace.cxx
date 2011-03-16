// Copyright:   Matra-Datavision 1995
// File:    Select3D_SensitiveFace.cxx
// Created: Mon Mar 27 10:15:15 1995
// Author:  Robert COUBLANC
//      <rob>
//Modif on jun-24-97 : introduction de CSLib_Class2d de LBR
//                     pour teste si on est dedans ou dehors...
//Modif on jul-21-97 : changement en harray1 pour eventuelles connexions ...

#include <Select3D_SensitiveFace.ixx>
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
// Function: faire disparaitre ce constructeur a la prochaine version...
// Purpose : simplement garde pour ne pas perturber la version update
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
// Function: 
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
// Function: 
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
  if(!Bnd_Box2d(mybox2d).IsVoid()){
    Bnd_Box2d(mybox2d).Get(Xmin,Ymin,Xmax,Ymax);
    DMin2 = gp_XY(Xmax-Xmin,Ymax-Ymin).SquareModulus();
  }
  // calcul d'un critere de distance mini...
  // au depart Dmin = taille de la boite englobante 2D,
  // ensuite distance mini du polyedre ou du cdg...

  Standard_Real aTol2 = aTol*aTol;
  gp_XY CDG(0.,0.);
//  for(Standard_Integer I=1;I<=Nbp-1;I++){
  Standard_Integer I;
  for(I=1;I<mynbpoints-1;I++){
    CDG+=((Select3D_Pnt2d*)mypolyg2d)[I-1];
  }
  
  if(mynbpoints>1){
    CDG/= (mynbpoints-1);
  }
  DMin2=Min(DMin2,gp_XY(CDG.X()-X,CDG.Y()-Y).SquareModulus());
  DMin = Sqrt(DMin2);
  
  
  Standard_Boolean isplane2d(Standard_True);
  
  for( I=1;I<mynbpoints-1;I++){
    gp_XY V1(((Select3D_Pnt2d*)mypolyg2d)[I]),V(X,Y);
    V1-=((Select3D_Pnt2d*)mypolyg2d)[I-1];
    V-=((Select3D_Pnt2d*)mypolyg2d)[I-1];
    Standard_Real Vector = V1^V;
    Standard_Real V1V1 = V1.SquareModulus();
    DMin2 = 
      (V1V1 <=aTol2) ? 
    Min(DMin2,V.SquareModulus()): // si le segment est trop petit...
      Min(DMin2,Vector*Vector/V1V1);
    //cdg ...
    gp_XY PlaneTest(CDG);PlaneTest-=((Select3D_Pnt2d*)mypolyg2d)[I-1];
    Standard_Real valtst = PlaneTest^V1;
    if(isplane2d && Abs(valtst)>aTol) isplane2d=Standard_False;
  }

  if(isplane2d) {
    Select3D_SensitiveEntity::Matches(X,Y,aTol,DMin);
    
    return Standard_True;
  }
  //detection d'une auto - intersection dans le polygon 2D; si oui on sort
//    if (!AutoComputeFlag(myautointer)) {
//      if(mypolyg2d.Length()>4) {
//        if (SelectBasics_BasicTool::AutoInter(mypolyg2d)) {
//      SetAutoInterFlag(myautointer);
//        }
//      }
//      SetAutoComputeFlag(myautointer);
//    }
//   if (AutoInterFlag(myautointer)) return Standard_True;
// //  

  //sinon on regarde si le point est dans la face...
  TColgp_Array1OfPnt2d aArrayOf2dPnt(1, mynbpoints);
  Points2D(aArrayOf2dPnt);
  CSLib_Class2d TheInOutTool(aArrayOf2dPnt,aTol,aTol,Xmin,Ymin,Xmax,Ymax);
  Standard_Integer TheStat = TheInOutTool.SiDans(gp_Pnt2d(X,Y));
  
  Standard_Boolean res(Standard_False);
  switch(TheStat){
  case 0:
    res = Standard_True;
  case 1:
    {
      if(mytype!=Select3D_TOS_BOUNDARY)
	res = Standard_True;
    }
  }
  if(res)
    Select3D_SensitiveEntity::Matches(X,Y,aTol,DMin);
    
  return res;
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
  
  for(Standard_Integer j=1;j<=mynbpoints-1;j++){
    if(BoundBox.IsOut(((Select3D_Pnt2d*)mypolyg2d)[j-1])) return Standard_False;
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

  for(Standard_Integer j=1;j<=mynbpoints;j++){
    Standard_Integer RES = aClassifier2d.SiDans(((Select3D_Pnt2d*)mypolyg2d)[j-1]);
    if(RES!=1) return Standard_False;
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
  
  if(FullDump){
    S<<"\t\tNumber Of Points :"<<mynbpoints<<endl;
    
//    S<<"\t\t\tOwner:"<<myOwnerId<<endl;
    Select3D_SensitiveEntity::DumpBox(S,mybox2d);
  }
}

//=======================================================================
//function : ComputeDepth
//purpose  : 
//=======================================================================
Standard_Real Select3D_SensitiveFace::ComputeDepth(const gp_Lin& EyeLine) const
{
  Standard_Real val(Precision::Infinite());
  for(Standard_Integer i=0;i<mynbpoints-1;i++)
    val = Min(val,ElCLib::Parameter(EyeLine,((Select3D_Pnt*)mypolyg3d)[i]));
  return val;
}
