// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
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

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <gp_GTrsf.hxx>
#include <gp_Pnt.hxx>
#include <gp_XYZ.hxx>
#include <IGESGraph_TextDisplayTemplate.hxx>
#include <IGESGraph_TextFontDef.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESGraph_TextDisplayTemplate,IGESData_IGESEntity)

IGESGraph_TextDisplayTemplate::IGESGraph_TextDisplayTemplate ()    {  }


    void IGESGraph_TextDisplayTemplate::Init
  (const Standard_Real                  aWidth,
   const Standard_Real                  aHeight,
   const Standard_Integer               aFontCode,
   const Handle(IGESGraph_TextFontDef)& aFontEntity,
   const Standard_Real                  aSlantAngle,
   const Standard_Real                  aRotationAngle,
   const Standard_Integer               aMirrorFlag,
   const Standard_Integer               aRotationFlag,
   const gp_XYZ&                        aCorner)
{
  theBoxWidth      = aWidth;
  theBoxHeight     = aHeight;    
  theFontCode      = aFontCode;    
  theFontEntity    = aFontEntity;      
  theSlantAngle    = aSlantAngle;   
  theRotationAngle = aRotationAngle;  
  theMirrorFlag    = aMirrorFlag; 
  theRotateFlag    = aRotationFlag;     
  theCorner        = aCorner;             
  InitTypeAndForm(312,FormNumber());  // FormNumber 0-1 : Incremental status
}

    void  IGESGraph_TextDisplayTemplate::SetIncremental (const Standard_Boolean F)
{
  InitTypeAndForm(312, (F ? 1 : 0));
}


    Standard_Real IGESGraph_TextDisplayTemplate::BoxWidth () const
{
  return theBoxWidth;
}

    Standard_Real IGESGraph_TextDisplayTemplate::BoxHeight () const
{
  return theBoxHeight;
}

    Standard_Boolean IGESGraph_TextDisplayTemplate::IsFontEntity () const
{
  return (! theFontEntity.IsNull());
}

    Standard_Integer IGESGraph_TextDisplayTemplate::FontCode () const
{
  return theFontCode;
}

    Handle(IGESGraph_TextFontDef) IGESGraph_TextDisplayTemplate::FontEntity () const
{
  return theFontEntity;
}

    Standard_Real IGESGraph_TextDisplayTemplate::SlantAngle () const
{
  return theSlantAngle;
}

    Standard_Real IGESGraph_TextDisplayTemplate::RotationAngle () const
{
  return theRotationAngle;
}

    Standard_Integer IGESGraph_TextDisplayTemplate::MirrorFlag () const
{
  return theMirrorFlag;
}

    Standard_Integer IGESGraph_TextDisplayTemplate::RotateFlag () const
{
  return theRotateFlag;
}

    Standard_Boolean IGESGraph_TextDisplayTemplate::IsIncremental () const
{
  return ( FormNumber() == 1 );
}

    gp_Pnt IGESGraph_TextDisplayTemplate::StartingCorner () const
{
  return ( gp_Pnt(theCorner) );
}

    gp_Pnt IGESGraph_TextDisplayTemplate::TransformedStartingCorner () const
{
  gp_XYZ TempXYZ = theCorner;
  if (HasTransf()) Location().Transforms(TempXYZ);
  return ( gp_Pnt(TempXYZ) );
}
