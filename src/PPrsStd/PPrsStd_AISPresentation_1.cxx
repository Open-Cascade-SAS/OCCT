// Created on: 1999-07-08
// Created by: srn
// Copyright (c) 1999-1999 Matra Datavision
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

#include <PPrsStd_AISPresentation_1.ixx>


//=======================================================================
//function : PPrsStd_AISPresentation
//purpose  : 
//=======================================================================

PPrsStd_AISPresentation_1::PPrsStd_AISPresentation_1()
{
  myMode=0;
}


//=======================================================================
//function : SetDisplayed
//purpose  : 
//=======================================================================

void PPrsStd_AISPresentation_1::SetDisplayed (const Standard_Boolean B)       
{
  myIsDisplayed = B ;
}

//=======================================================================
//function : IsDisplayed
//purpose  : 
//=======================================================================

Standard_Boolean PPrsStd_AISPresentation_1::IsDisplayed() const 
{
    return myIsDisplayed;
}


//=======================================================================
//function : SetDriverGUID
//purpose  : 
//=======================================================================
void PPrsStd_AISPresentation_1::SetDriverGUID( const Handle(PCollection_HExtendedString)&  guid)
{
  myDriverGUID = guid;
}

//=======================================================================
//function : GetDriverGUID
//purpose  : 
//=======================================================================
Handle(PCollection_HExtendedString) PPrsStd_AISPresentation_1::GetDriverGUID( void ) const
{
  return myDriverGUID;
}

//=======================================================================
//function : Color
//purpose  : 
//=======================================================================
Standard_Integer PPrsStd_AISPresentation_1::Color() const
{
  return myColor;
}

//=======================================================================
//function : SetColor
//purpose  : 
//=======================================================================
void PPrsStd_AISPresentation_1::SetColor(const Standard_Integer C) 
{
  myColor = C;
}

//=======================================================================
//function : Transparency
//purpose  : 
//=======================================================================
Standard_Real PPrsStd_AISPresentation_1::Transparency() const
{
  return myTransparency;
}

//=======================================================================
//function : SetTransparency
//purpose  : 
//=======================================================================
void PPrsStd_AISPresentation_1::SetTransparency(const Standard_Real T) 
{
  myTransparency = T;
}

//=======================================================================
//function : Material
//purpose  : 
//=======================================================================
Standard_Integer PPrsStd_AISPresentation_1::Material() const
{
  return myMaterial;
}

//=======================================================================
//function : SetMaterial
//purpose  : 
//=======================================================================
void PPrsStd_AISPresentation_1::SetMaterial(const Standard_Integer M) 
{
   myMaterial = M;
}

//=======================================================================
//function : Width 
//purpose  : 
//=======================================================================
Standard_Real PPrsStd_AISPresentation_1::Width() const
{
  return myWidth;  
}




//=======================================================================
//function : SetWidth
//purpose  : 
//=======================================================================

void PPrsStd_AISPresentation_1::SetWidth(const Standard_Real W) 
{
  myWidth = W;
}



//=======================================================================
//function : Mode
//purpose  : 
//=======================================================================

Standard_Integer PPrsStd_AISPresentation_1::Mode() const
{
  return myMode;
}


//=======================================================================
//function : SetMode
//purpose  : 
//=======================================================================

void PPrsStd_AISPresentation_1::SetMode(const Standard_Integer M) 
{
   myMode = M;
}
