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
#include <Select3D_Projector.hxx>

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
    // Get myCenter3D 
    myCenter3D = TheCircle->Location();
  }
  // Radius = 0.0
  else
  {
    ((Select3D_Pnt*)mypolyg3d)[0] = TheCircle->Location();
    // Get myCenter3D
    myCenter3D = ((Select3D_Pnt*)mypolyg3d)[0];
  }
    
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
    // Get myCenter3D 
    myCenter3D = TheCircle->Location();
  }
  else
  {
    ((Select3D_Pnt*)mypolyg3d)[0] = TheCircle->Location();  
    // Get myCenter3D
    myCenter3D = ((Select3D_Pnt*)mypolyg3d)[0];
  }
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
  if (mynbpoints > 1) 
    ComputeCenter3D();
  else 
    myCenter3D = ((Select3D_Pnt*)mypolyg3d)[0];
}

Select3D_SensitiveCircle::Select3D_SensitiveCircle(const Handle(SelectBasics_EntityOwner)& OwnerId,
                           const TColgp_Array1OfPnt& Thepolyg3d,
                           const Standard_Boolean FilledCircle):
Select3D_SensitivePoly(OwnerId, Thepolyg3d),
myFillStatus(FilledCircle),
myDetectedIndex(-1)
{ 
  if (mynbpoints > 1) 
    ComputeCenter3D();
  else 
    myCenter3D = ((Select3D_Pnt*)mypolyg3d)[0];
}

Standard_Boolean Select3D_SensitiveCircle::
Matches(const Standard_Real X, 
    const Standard_Real Y, 
    const Standard_Real aTol, 
    Standard_Real& DMin)
{

  if(mynbpoints>1)
  {
    Standard_Boolean Found = Standard_False;
    Standard_Integer i = 0;
    
    if(!myFillStatus)
    {
      while(i < mynbpoints-2 && !Found) 
      { 
        Standard_Integer TheStat = 
          Select3D_SensitiveTriangle::Status(((Select3D_Pnt2d*)mypolyg2d)[i],
                         ((Select3D_Pnt2d*)mypolyg2d)[i+1],
                         ((Select3D_Pnt2d*)mypolyg2d)[i+2],
                         gp_XY(X,Y),aTol,DMin); 
        Found = (TheStat != 2); 
        if(Found) myDetectedIndex = i; 
        i += 2;
      }
    }
    else 
    {
      myDetectedIndex =-1;
      Standard_Real Xmin,Ymin,Xmax,Ymax;

      // Get coordinates of the bounding box
      Bnd_Box2d(mybox2d).Get(Xmin,Ymin,Xmax,Ymax); 
      TColgp_Array1OfPnt2d anArrayOf2dPnt(1, mynbpoints); 
      
      // Fill anArrayOf2dPnt with points from mypolig2d
      Points2D(anArrayOf2dPnt); 
      
      CSLib_Class2d anInOutTool(anArrayOf2dPnt,aTol,aTol,Xmin,Ymin,Xmax,Ymax);

      // Method SiDans returns the status :
      //  1 - the point is inside the circle 
      //  0 - the point is on the circle
      // -1 - the point is outside the circle
      Standard_Integer aStat = anInOutTool.SiDans(gp_Pnt2d(X,Y));
      if(aStat != -1) 
      {
        // Compute DMin (a distance between the center and the point)
        DMin = gp_XY(myCenter2D.x - X, myCenter2D.y - Y).Modulus();
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

void Select3D_SensitiveCircle::Project(const Handle_Select3D_Projector &aProjector) 
{
  Select3D_SensitivePoly::Project(aProjector); 
  // Project the center of the circle 
  gp_Pnt2d aCenter;
  aProjector->Project(myCenter3D, aCenter);
  myCenter2D = aCenter;
}

void Select3D_SensitiveCircle::ComputeCenter3D() 
{
  gp_XYZ aCenter(0., 0., 0.);
  if (mynbpoints > 1) 
  {
    // The mass of points system
    Standard_Integer aMass = mynbpoints - 1; 
    // Find the circle barycenter
    for (Standard_Integer i = 0; i < mynbpoints-1; ++i) 
    {
      aCenter += ((Select3D_Pnt*)mypolyg3d)[i];
    }
    myCenter3D = aCenter / aMass;
  }
  else if (mynbpoints == 1) 
  {
    myCenter3D = ((Select3D_Pnt*)mypolyg3d)[0];
  }
  // bad case! there are no points in mypolyg3d 
  // It can lead to incorrect computation of 
  // parameter DMin in method Matches. 
  // In spite of this myCenter3D isn't left uninitialized
  else 
  { 
    myCenter3D = aCenter;
  }
}
