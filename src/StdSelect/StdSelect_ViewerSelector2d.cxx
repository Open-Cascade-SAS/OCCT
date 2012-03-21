// Created on: 1995-02-23
// Created by: Mister rmi
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




#include <StdSelect_ViewerSelector2d.ixx>
#include <Graphic2d_Array1OfVertex.hxx>
#include <Graphic2d_GraphicObject.hxx>
#include <SelectMgr_DataMapIteratorOfDataMapOfIntegerSensitive.hxx>
#include <Select2D_SensitiveEntity.hxx>
#include <StdSelect_TextProjector2d.hxx>
#include <SelectBasics_ListOfBox2d.hxx>
#include <Bnd_Box2d.hxx>
#include <Graphic2d_Vertex.hxx>
#include <Graphic2d_Polyline.hxx>
#include <Graphic2d_Array1OfVertex.hxx>
#include <SelectBasics_ListIteratorOfListOfBox2d.hxx>

//==================================================
// Function: 
// Purpose :
//==================================================

StdSelect_ViewerSelector2d::StdSelect_ViewerSelector2d():
mypixtol(2)
{}

StdSelect_ViewerSelector2d::StdSelect_ViewerSelector2d(const Handle(Select2D_Projector)& aPrj):
myprj(aPrj),
mypixtol(2)
{}

//==================================================
// Function: Set
// Purpose :
//==================================================

void StdSelect_ViewerSelector2d::Set(const Standard_Integer apixtol)
{mypixtol = apixtol;
 tosort = Standard_True;
}

//==================================================
// Function: Set
// Purpose :
//==================================================

void StdSelect_ViewerSelector2d::Set(const Handle(Select2D_Projector)& aPrj)
{myprj = aPrj;
 UpdateConversion();
}

//==================================================
// Function: Convert
// Purpose :
//==================================================

void StdSelect_ViewerSelector2d::Convert(const Handle(SelectMgr_Selection)& aSel) 
{
  for(aSel->Init();aSel->More();aSel->Next())
    {
      if(aSel->Sensitive()->NeedsConversion()) 
	Handle(Select2D_SensitiveEntity)::
      DownCast(aSel->Sensitive())->Convert(myprj);
    }
  tosort = Standard_True;
}


void StdSelect_ViewerSelector2d::Pick(const Standard_Integer XPix,
				      const Standard_Integer YPix,
				      const Handle(V2d_View)& aViou)
{
  Standard_Real Xr,Yr;
  aViou->Convert(XPix,YPix,Xr,Yr);
  InitSelect(Xr,Yr);
}



void StdSelect_ViewerSelector2d::Pick(const Standard_Integer XPMin,
				      const Standard_Integer YPMin,
				      const Standard_Integer XPMax,
				      const Standard_Integer YPMax,
				      const Handle(V2d_View)& aViou)
{
  if(mypixtol!=0) mytolerance = aViou->Convert(mypixtol);
  Standard_Real X1,Y1,X2,Y2;
  aViou->Convert(XPMin,YPMin,X1,Y1);
  aViou->Convert(XPMax,YPMax,X2,Y2);
  InitSelect(Min(X1,X2),
	     Min(Y1,Y2),
	     Max(X1,X2),
	     Max(Y1,Y2));
}

//==================================================
// Function: DisplayAreas
// Purpose :
//==================================================
void StdSelect_ViewerSelector2d::
DisplayAreas(const Handle(V2d_View)& aViou)
{
  mygo = new Graphic2d_GraphicObject(aViou->View());
  
  Graphic2d_Array1OfVertex Av1 (1,5);
  SelectMgr_DataMapIteratorOfDataMapOfIntegerSensitive It(myentities);  
  SelectBasics_ListOfBox2d BoxList;
  Standard_Real xmin,ymin,xmax,ymax;
  Handle(Graphic2d_Polyline) pol;

  for(;It.More();It.Next())
    {
      It.Value()->Areas(BoxList);
      // DEN 
      SelectBasics_ListIteratorOfListOfBox2d anIterator(BoxList); 
      for (;anIterator.More();anIterator.Next()) 
        { 
          anIterator.Value().Get(xmin,ymin,xmax,ymax); 

      // FIN DEN 
      // BY DEN BoxList.Last().Get(xmin,ymin,xmax,ymax); 
      // construction des vertex graphiques 
      Av1.SetValue(1,Graphic2d_Vertex(xmin-mytolerance,ymin-mytolerance)); 
      Av1.SetValue(2,Graphic2d_Vertex(xmax+mytolerance,ymin-mytolerance)); 
      Av1.SetValue(3,Graphic2d_Vertex(xmax+mytolerance,ymax+mytolerance)); 
      Av1.SetValue(4,Graphic2d_Vertex(xmin-mytolerance,ymax+mytolerance)); 
      Av1.SetValue(5,Graphic2d_Vertex(xmin-mytolerance,ymin-mytolerance)); 
  
      pol = new Graphic2d_Polyline(mygo,Av1); 
      pol ->SetColorIndex(4); //vert 
      pol->SetTypeIndex(2); 
      // DEN 
      } 
    // FIN DEN 
    }
  mygo->Display();
  aViou->Update();
  
}

//================================================
// function : ClearAreas
// purpose  :
//================================================

void StdSelect_ViewerSelector2d::ClearAreas ()
{
  if (mygo.IsNull()) return; 
  mygo->RemovePrimitives(); 
  mygo->Display();
  mygo->Remove();
}


