// Created on: 1999-07-08
// Created by: srn
// Copyright (c) 1999-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <PPrsStd_AISPresentation.ixx>


//=======================================================================
//function : PPrsStd_AISPresentation
//purpose  : 
//=======================================================================

PPrsStd_AISPresentation::PPrsStd_AISPresentation() :
    myIsDisplayed(Standard_False),
    myTransparency(0.),
    myColor(0),
    myMaterial(0),
    myWidth(0.)
{}


//=======================================================================
//function : SetDisplayed
//purpose  : 
//=======================================================================

void PPrsStd_AISPresentation::SetDisplayed (const Standard_Boolean B)       
{
  myIsDisplayed = B ;
}

//=======================================================================
//function : IsDisplayed
//purpose  : 
//=======================================================================

Standard_Boolean PPrsStd_AISPresentation::IsDisplayed() const 
{
    return myIsDisplayed;
}


//=======================================================================
//function : SetDriverGUID
//purpose  : 
//=======================================================================
void PPrsStd_AISPresentation::SetDriverGUID( const Handle(PCollection_HExtendedString)&  guid)
{
  myDriverGUID = guid;
}

//=======================================================================
//function : GetDriverGUID
//purpose  : 
//=======================================================================
Handle(PCollection_HExtendedString) PPrsStd_AISPresentation::GetDriverGUID( void ) const
{
  return myDriverGUID;
}

//=======================================================================
//function : Color
//purpose  : 
//=======================================================================
Standard_Integer PPrsStd_AISPresentation::Color() const
{
  return myColor;
}

//=======================================================================
//function : SetColor
//purpose  : 
//=======================================================================
void PPrsStd_AISPresentation::SetColor(const Standard_Integer C) 
{
  myColor = C;
}

//=======================================================================
//function : Transparency
//purpose  : 
//=======================================================================
Standard_Real PPrsStd_AISPresentation::Transparency() const
{
  return myTransparency;
}

//=======================================================================
//function : SetTransparency
//purpose  : 
//=======================================================================
void PPrsStd_AISPresentation::SetTransparency(const Standard_Real T) 
{
  myTransparency = T;
}

//=======================================================================
//function : Material
//purpose  : 
//=======================================================================
Standard_Integer PPrsStd_AISPresentation::Material() const
{
  return myMaterial;
}

//=======================================================================
//function : SetMaterial
//purpose  : 
//=======================================================================
void PPrsStd_AISPresentation::SetMaterial(const Standard_Integer M) 
{
   myMaterial = M;
}

//=======================================================================
//function : Width 
//purpose  : 
//=======================================================================
Standard_Real PPrsStd_AISPresentation::Width() const
{
  return myWidth;  
}

//=======================================================================
//function : SetWidth
//purpose  : 
//=======================================================================
void PPrsStd_AISPresentation::SetWidth(const Standard_Real W) 
{
  myWidth = W;
}

