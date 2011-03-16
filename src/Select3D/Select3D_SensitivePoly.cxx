#include <Select3D_SensitivePoly.ixx>
#include <gp_Pnt2d.hxx>
#include <gp_Pnt.hxx>
#include <Select3D_Pnt.hxx>
#include <Select3D_Pnt2d.hxx>
#include <Select3D_Box2d.hxx>
#include <TopLoc_Location.hxx>


//==================================================
// Function: faire disparaitre ce constructeur a la prochaine version...
// Purpose : simplement garde pour ne pas perturber la version update
//==================================================

Select3D_SensitivePoly::
Select3D_SensitivePoly(const Handle(SelectBasics_EntityOwner)& OwnerId,
               const TColgp_Array1OfPnt& ThePoints):
Select3D_SensitiveEntity(OwnerId)
{
  mynbpoints = ThePoints.Upper()-ThePoints.Lower()+1;
  mypolyg3d = new Select3D_Pnt[mynbpoints];
  mypolyg2d = new Select3D_Pnt2d[mynbpoints];
  for(Standard_Integer i=0;i<mynbpoints;i++)
    ((Select3D_Pnt*)mypolyg3d)[i] = ThePoints.Value(ThePoints.Lower()+i); 
}

//==================================================
// Function: 
// Purpose :
//==================================================

Select3D_SensitivePoly::
Select3D_SensitivePoly(const Handle(SelectBasics_EntityOwner)& OwnerId,
               const Handle(TColgp_HArray1OfPnt)& ThePoints):
Select3D_SensitiveEntity(OwnerId)
{
  mynbpoints = ThePoints->Upper()-ThePoints->Lower()+1;
  mypolyg3d = new Select3D_Pnt[mynbpoints];
  mypolyg2d = new Select3D_Pnt2d[mynbpoints];
  for(Standard_Integer i=0;i<mynbpoints;i++)
    ((Select3D_Pnt*)mypolyg3d)[i] = ThePoints->Value(ThePoints->Lower()+i); 
}

//==================================================
// Function: 
// Purpose :
//==================================================

Select3D_SensitivePoly::
Select3D_SensitivePoly(const Handle(SelectBasics_EntityOwner)& OwnerId, 
             const Standard_Integer NbPoints):
Select3D_SensitiveEntity(OwnerId)
{
  mynbpoints = NbPoints;
  mypolyg3d = new Select3D_Pnt[mynbpoints];
  mypolyg2d = new Select3D_Pnt2d[mynbpoints];
}

//==================================================
// Function: 
// Purpose :
//==================================================

void Select3D_SensitivePoly::Project(const Select3D_Projector& aProj)
{
  Select3D_SensitiveEntity::Project(aProj); // to set the field last proj...
  mybox2d.SetVoid();

  Standard_Boolean hasloc = HasLocation();
  gp_Pnt2d aPnt2d;
  for(Standard_Integer i=0;i<mynbpoints;i++)
    {
      gp_Pnt aPnt(((Select3D_Pnt*)mypolyg3d)[i].x, ((Select3D_Pnt*)mypolyg3d)[i].y, ((Select3D_Pnt*)mypolyg3d)[i].z);
      if(hasloc){
	aProj.Project(aPnt.Transformed(Location().Transformation()),aPnt2d);
      }
      else
	aProj.Project(aPnt,aPnt2d);
      mybox2d.Update(aPnt2d);
      ((Select3D_Pnt2d*)mypolyg2d)[i] = aPnt2d;
    }
}

//==================================================
// Function: 
// Purpose :
//==================================================
void Select3D_SensitivePoly
::Areas(SelectBasics_ListOfBox2d& aSeq)
{
  aSeq.Append(mybox2d);
}

//==================================================
// Function: 
// Purpose :
//==================================================
void Select3D_SensitivePoly::Destroy() 
{
  delete[] (Select3D_Pnt*)mypolyg3d;
  delete[] (Select3D_Pnt2d*)mypolyg2d;
}

