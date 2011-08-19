// Copyright:   Matra-Datavision 1995
// File:    Select3D_SensitiveCurve.cxx
// Created: Mon Mar 13 09:57:58 1995
// Author:  Robert COUBLANC
//      <rob>



#include <Select3D_SensitiveCurve.ixx>
#include <SelectBasics_BasicTool.hxx>
#include <gp_Lin2d.hxx>
#include <Precision.hxx>
#include <ElCLib.hxx>
#include <CSLib_Class2d.hxx>


//==================================================
// Function: 
// Purpose :
//==================================================

Select3D_SensitiveCurve
::Select3D_SensitiveCurve(const Handle(SelectBasics_EntityOwner)& OwnerId,
              const Handle(Geom_Curve)& C,
              const Standard_Integer NbPoints):
Select3D_SensitivePoly(OwnerId, NbPoints),
mylastseg(0)     
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
  TColgp_Array1OfPnt2d aArrayOf2dPnt(1, mynbpoints);
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
  
  for(Standard_Integer j=0; j<mynbpoints; j++)
    {
      if(BoundBox.IsOut(((Select3D_Pnt2d*)mypolyg2d)[j])) return Standard_False;
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

  for(Standard_Integer j=0;j<mynbpoints;j++){
    Standard_Integer RES = aClassifier2d.SiDans(((Select3D_Pnt2d*)mypolyg2d)[j]);
    if(RES!=1) return Standard_False;
  }
  return Standard_True;
}



//==================================================
// Function: 
// Purpose :
//==================================================

void Select3D_SensitiveCurve
::LoadPoints (const Handle(Geom_Curve)& aCurve,const Standard_Integer NbP)
{
  /*this method is private and it used only inside of constructor. 
    That's why check !NbP==mypolyg3d->Length() was removed*/

  Standard_Real Step = (aCurve->LastParameter()- aCurve->FirstParameter())/(NbP-1);
  Standard_Real Curparam = aCurve->FirstParameter();
  for(Standard_Integer i=0;i<mynbpoints;i++)
    {
      ((Select3D_Pnt*)mypolyg3d)[i] = aCurve->Value(Curparam);
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
  if(HasLocation())
    S<<"\t\tExisting Location"<<endl;

  S<<"\t\tNumber Of Points :"<<mynbpoints<<endl;

  if(FullDump){
//    S<<"\t\t\tOwner:"<<myOwnerId<<endl;
    Select3D_SensitiveEntity::DumpBox(S,mybox2d);
  }
}
//=======================================================================
//function : ComputeDepth
//purpose  : 
//=======================================================================

Standard_Real Select3D_SensitiveCurve::ComputeDepth(const gp_Lin& EyeLine) const
{

  if(mylastseg==0) return Precision::Infinite(); // non implemente actuellement...
  gp_XYZ TheCDG(((Select3D_Pnt*)mypolyg3d)[mylastseg]);
  TheCDG+=((Select3D_Pnt*)mypolyg3d)[mylastseg+1];
  TheCDG/=2.;
  return ElCLib::Parameter(EyeLine,gp_Pnt(TheCDG));
}
