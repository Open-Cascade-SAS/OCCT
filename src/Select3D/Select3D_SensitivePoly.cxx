// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

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
Select3D_SensitiveEntity(OwnerId),
mypolyg(ThePoints.Upper()-ThePoints.Lower()+1)
{
  for (Standard_Integer theIndex = 0 ; theIndex < mypolyg.Size(); ++theIndex)
    mypolyg.SetPnt(theIndex, ThePoints.Value(ThePoints.Lower()+theIndex));
}

//==================================================
// Function: Creation
// Purpose :
//==================================================

Select3D_SensitivePoly::
Select3D_SensitivePoly(const Handle(SelectBasics_EntityOwner)& OwnerId,
                       const Handle(TColgp_HArray1OfPnt)& ThePoints):
Select3D_SensitiveEntity(OwnerId),
mypolyg(ThePoints->Upper()-ThePoints->Lower()+1)
{
  for (Standard_Integer theIndex = 0; theIndex < mypolyg.Size(); theIndex++)
    mypolyg.SetPnt(theIndex, ThePoints->Value(ThePoints->Lower()+theIndex));
}

//==================================================
// Function: Creation
// Purpose :
//==================================================

Select3D_SensitivePoly::
Select3D_SensitivePoly(const Handle(SelectBasics_EntityOwner)& OwnerId, 
                       const Standard_Integer NbPoints):
Select3D_SensitiveEntity(OwnerId),
mypolyg(NbPoints)
{
}

//==================================================
// Function: Project
// Purpose :
//==================================================

void Select3D_SensitivePoly::Project(const Handle(Select3D_Projector)& aProj)
{
  mybox2d.SetVoid();

  Standard_Boolean hasloc = HasLocation();
  gp_Pnt2d aPnt2d;
  gp_Pnt aPnt;
  for (Standard_Integer theIndex = 0; theIndex < mypolyg.Size(); ++theIndex)
  {
    aPnt = mypolyg.Pnt(theIndex);
    if (hasloc)
    {
      aProj->Project(aPnt.Transformed(Location().Transformation()), aPnt2d);
    }
    else
    {
      aProj->Project(aPnt, aPnt2d);
    }
    mybox2d.Update(aPnt2d);
    mypolyg.SetPnt2d(theIndex, aPnt2d);
  }
}

//==================================================
// Function: Areas
// Purpose :
//==================================================
void Select3D_SensitivePoly
::Areas(SelectBasics_ListOfBox2d& aSeq)
{
  aSeq.Append(mybox2d);
}

