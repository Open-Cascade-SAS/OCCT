// File:    Select3D_SensitiveCircle.cxx
// Created: Tue Feb  6 14:15:06 1996
// Author:  Robert COUBLANC
//      <rob@fidox>
// Modified    Tue Apr 14 1998 by rob : fix Bug : Case of Null Radius Circle...

#include <Select3D_SensitiveCircle.ixx>
#include <Precision.hxx>
#include <gp_Lin2d.hxx>

#include <CSLib_Class2d.hxx>
#include <Select3D_SensitiveTriangle.hxx>
#include <ElCLib.hxx>
#include <Select3D_Pnt.hxx>
#include <Select3D_Pnt2d.hxx>

//=======================================================================
//function : Select3D_SensitiveCircle (constructeur)
//purpose  : Definition of a sensitive circle
//=======================================================================
static Standard_Integer S3D_GetCircleNBPoints(const Handle(Geom_Circle)& C,
                                              const Standard_Integer anInputNumber)
{ 
  if(C->Radius()>Precision::Confusion())
    return 2*anInputNumber+1;
  return 1;
}
static Standard_Integer S3D_GetArcNBPoints(const Handle(Geom_Circle)& C,
                    const Standard_Integer anInputNumber)
{ 
  if(C->Radius()>Precision::Confusion())
    return 2*anInputNumber-1;
  return 1;
}
     
Select3D_SensitiveCircle::
Select3D_SensitiveCircle(const Handle(SelectBasics_EntityOwner)& OwnerId, 
             const Handle(Geom_Circle)& TheCircle, 
             const Standard_Boolean FilledCircle,
             const Standard_Integer NbPoints):
Select3D_SensitivePoly(OwnerId, S3D_GetCircleNBPoints(TheCircle,NbPoints)),
myFillStatus(FilledCircle),
myDetectedIndex(-1)
{
  if(mynbpoints!=1){
    gp_Pnt p1,p2;//,pmid;
    gp_Vec v1;//,v2;
    Standard_Real ustart = TheCircle->FirstParameter(),uend = TheCircle->LastParameter();
    Standard_Real du = (uend-ustart)/NbPoints;
    Standard_Real R = TheCircle->Radius();
    Standard_Integer rank = 1;
    Standard_Real curu =ustart;
    for(Standard_Integer i=1;i<=NbPoints;i++) 
    {
      TheCircle->D1(curu,p1,v1);
    
      v1.Normalize();
      ((Select3D_Pnt*)mypolyg3d)[rank-1] = p1;
      rank++;
      p2 = gp_Pnt(p1.X()+v1.X()*tan(du/2.)*R,
            p1.Y()+v1.Y()*tan(du/2.)*R,
            p1.Z()+v1.Z()*tan(du/2.)*R);
      ((Select3D_Pnt*)mypolyg3d)[rank-1] = p2;
      rank++;
      curu+=du;
    }
    ((Select3D_Pnt*)mypolyg3d)[NbPoints*2] = ((Select3D_Pnt*)mypolyg3d)[0];
  }
  // Radius = 0.0
  else
    ((Select3D_Pnt*)mypolyg3d)[0] = TheCircle->Location();
}

//=======================================================================
//function : Select3D_SensitiveCircle (constructeur)
//purpose  : Definition of a sensitive arc
//=======================================================================
Select3D_SensitiveCircle::
Select3D_SensitiveCircle(const Handle(SelectBasics_EntityOwner)& OwnerId, 
             const Handle(Geom_Circle)& TheCircle,
             const Standard_Real u1,
             const Standard_Real u2,
             const Standard_Boolean FilledCircle,
             const Standard_Integer NbPoints):
Select3D_SensitivePoly(OwnerId, S3D_GetArcNBPoints(TheCircle,NbPoints)),
myFillStatus(FilledCircle),
myDetectedIndex(-1)
{

  if(mynbpoints > 1){
    gp_Pnt p1,p2;//,pmid;
    gp_Vec v1;//,v2;
    
    Standard_Real ustart = u1;
    Standard_Real uend = u2;
    
    if (u1 > u2) {ustart=u1;uend=u2;}
    
    Standard_Real du = (uend-ustart)/(NbPoints-1);
    Standard_Real R = TheCircle->Radius();
    Standard_Integer rank = 1;
    Standard_Real curu =ustart;
    
    
    
    for(Standard_Integer i=1;i<=NbPoints-1;i++)
    {
      TheCircle->D1(curu,p1,v1);
      v1.Normalize();
      ((Select3D_Pnt*)mypolyg3d)[rank-1] = p1;
      rank++;
      p2 = gp_Pnt(p1.X()+v1.X()*tan(du/2.)*R,
            p1.Y()+v1.Y()*tan(du/2.)*R,
            p1.Z()+v1.Z()*tan(du/2.)*R);
      ((Select3D_Pnt*)mypolyg3d)[rank-1] = p2;            
      rank++;
      curu+=du;
    }
    TheCircle->D0(uend,p1);
    ((Select3D_Pnt*)mypolyg3d)[NbPoints*2-2] = p1;            
  }
  else
    ((Select3D_Pnt*)mypolyg3d)[0] = TheCircle->Location();                
}

//=======================================================================
//function : Select3D_SensitiveCircle
//purpose  : 
//=======================================================================
Select3D_SensitiveCircle::Select3D_SensitiveCircle(const Handle(SelectBasics_EntityOwner)& OwnerId,
                           const Handle(TColgp_HArray1OfPnt)& Thepolyg3d,
                           const Standard_Boolean FilledCircle):
Select3D_SensitivePoly(OwnerId, Thepolyg3d),
myFillStatus(FilledCircle),
myDetectedIndex(-1)
{
  
}
Select3D_SensitiveCircle::Select3D_SensitiveCircle(const Handle(SelectBasics_EntityOwner)& OwnerId,
                           const TColgp_Array1OfPnt& Thepolyg3d,
                           const Standard_Boolean FilledCircle):
Select3D_SensitivePoly(OwnerId, Thepolyg3d),
myFillStatus(FilledCircle),
myDetectedIndex(-1)
{
}

Standard_Boolean Select3D_SensitiveCircle::
Matches(const Standard_Real X, 
    const Standard_Real Y, 
    const Standard_Real aTol, 
    Standard_Real& DMin)
{
  

  // in case of Edge (for the face it is only checked if 
  // the mouse point X,Y is found inside the triangle
  // pi,pi+1,pi+2 with close tolerance... if yes, finish...
  if(mynbpoints>1){
    Standard_Boolean Found =Standard_False;
    Standard_Integer i = 0;
    //gp_Pnt2d p1,p2,p3,pg;
    
    if(!myFillStatus){
      while(i < mynbpoints-2 && !Found) {
    Standard_Integer TheStat = 
      Select3D_SensitiveTriangle::Status(((Select3D_Pnt2d*)mypolyg2d)[i],
                         ((Select3D_Pnt2d*)mypolyg2d)[i+1],
                         ((Select3D_Pnt2d*)mypolyg2d)[i+2],
                         gp_XY(X,Y),aTol,DMin);
    Found = (TheStat!=2);
    if(Found) myDetectedIndex=i;
    i+=2;
    
      }
    }
    else {
      myDetectedIndex =-1;
#ifndef DEB
      Standard_Real DMin2 = 0.;
#else
      Standard_Real DMin2;
#endif
      Standard_Real Xmin,Ymin,Xmax,Ymax;
      Bnd_Box2d(mybox2d).Get(Xmin,Ymin,Xmax,Ymax);
      if(!Bnd_Box2d(mybox2d).IsVoid())
        DMin2 = gp_XY(Xmax-Xmin,Ymax-Ymin).SquareModulus();
      TColgp_Array1OfPnt2d aArrayOf2dPnt(1, mynbpoints);
      Points2D(aArrayOf2dPnt);
      CSLib_Class2d TheInOutTool(aArrayOf2dPnt,aTol,aTol,Xmin,Ymin,Xmax,Ymax);
      Standard_Integer TheStat = TheInOutTool.SiDans(gp_Pnt2d(X,Y));
      Standard_Real aTol2 = aTol*aTol;
      if(TheStat!=1) {
	for(Standard_Integer I=0;i<mynbpoints-1;i+=2){
	  gp_XY V1(((Select3D_Pnt2d*)mypolyg2d)[I+1]),V(X,Y);
	  V1-=((Select3D_Pnt2d*)mypolyg2d)[I];
	  V-=((Select3D_Pnt2d*)mypolyg2d)[I];
	  Standard_Real Vector = V1^V;
	  Standard_Real V1V1 = V1.SquareModulus();
	  DMin2 = 
	    (V1V1 <=aTol2) ? 
	      Min(DMin2,V.SquareModulus()): // if the segment is too small...
		Min(DMin2,Vector*Vector/V1V1);
	  
	}
	Select3D_SensitiveEntity::Matches(X,Y,aTol,DMin);
	return Standard_True;
      }
      return Standard_False;
      
    }
    if(!Found) 
      myDetectedIndex=-1;
    else
      Select3D_SensitiveEntity::Matches(X,Y,aTol,DMin);
    
    return Found;
  }
  return Standard_True;

}
Standard_Boolean Select3D_SensitiveCircle::
Matches(const Standard_Real XMin,
    const Standard_Real YMin,
    const Standard_Real XMax,
    const Standard_Real YMax,
    const Standard_Real aTol)
{
  myDetectedIndex =-1;
  Bnd_Box2d abox;
  abox.Update(Min(XMin,XMax),Min(YMin,YMax),Max(XMin,XMax),Max(YMin,YMax));
  abox.Enlarge(aTol);
  for(Standard_Integer i=0;i<mynbpoints;i++)
    if(abox.IsOut(((Select3D_Pnt2d*)mypolyg2d)[i])) return Standard_False;

  return Standard_True;
}


//=======================================================================
//function : Matches
//purpose  : 
//=======================================================================

Standard_Boolean Select3D_SensitiveCircle::
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




void Select3D_SensitiveCircle::
ArrayBounds(Standard_Integer & Low,
          Standard_Integer & Up) const
{
  Low = 0;
  Up  = mynbpoints-1;
}


gp_Pnt Select3D_SensitiveCircle::
GetPoint3d(const Standard_Integer Rank) const
{
  if(Rank>=0&& Rank<mynbpoints)
    return ((Select3D_Pnt*)mypolyg3d)[Rank];
  return ((Select3D_Pnt*)mypolyg3d)[0];
}


//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void Select3D_SensitiveCircle::Dump(Standard_OStream& S,const Standard_Boolean FullDump) const
{
  //  Standard_Integer rank(1);
  gp_XYZ CDG(0.,0.,0.);
  
  S<<"\tSensitiveCircle 3D :";
  
  Standard_Boolean isclosed = 1== mynbpoints;
  if(isclosed)
    S<<"(Closed Circle)"<<endl;
  else
    S<<"(Arc Of Circle)"<<endl;
  
  if(HasLocation())
    S<<"\t\tExisting Location"<<endl;
  

  if(FullDump){
    Standard_Integer EndIndex = isclosed? mynbpoints-2 : mynbpoints-1, nbpt(0);
    for(Standard_Integer i=0;i<EndIndex;i+=2){
      CDG +=((Select3D_Pnt*)mypolyg3d)[i];
      nbpt++;
    }
    
    CDG/=nbpt;
    
    Standard_Real R = (CDG-((Select3D_Pnt*)mypolyg3d)[0]).Modulus();
    
    S<<"\t\t Center : ("<<CDG.X()<<" , "<<CDG.Y()<<" , "<<CDG.Z()<<" )"<<endl;
    S<<"\t\t Radius :"<<R<<endl;
    
  }
}
Standard_Real Select3D_SensitiveCircle::ComputeDepth(const gp_Lin& EyeLine) const
{
  gp_Pnt CDG;
  if(myDetectedIndex==-1){
    gp_XYZ CurCoord(((Select3D_Pnt*)mypolyg3d)[0]);
    Standard_Boolean isclosed = 1==mynbpoints;
    Standard_Integer EndIndex = isclosed ? mynbpoints-2 : mynbpoints-1, nbpt(0);
    for(Standard_Integer i=1;i<EndIndex;i+=2){
      CurCoord +=((Select3D_Pnt*)mypolyg3d)[i];
      nbpt++;
    }
    CDG.SetXYZ(CurCoord);
  }
  else{
    gp_XYZ CurCoord(((Select3D_Pnt*)mypolyg3d)[myDetectedIndex]);
    CurCoord+=((Select3D_Pnt*)mypolyg3d)[myDetectedIndex+1];
    CurCoord+=((Select3D_Pnt*)mypolyg3d)[myDetectedIndex+2];
    CDG.SetXYZ(CurCoord);
  }

  return  ElCLib::Parameter(EyeLine,CDG);
  
}
