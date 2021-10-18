// Created on: 1992-08-21
// Created by: Christophe MARION
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _HLRTest_ShapeData_HeaderFile
#define _HLRTest_ShapeData_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Draw_Color.hxx>
#include <Standard_Transient.hxx>


class HLRTest_ShapeData;
DEFINE_STANDARD_HANDLE(HLRTest_ShapeData, Standard_Transient)

//! Contains the colors of a shape.
class HLRTest_ShapeData : public Standard_Transient
{

public:

  
  Standard_EXPORT HLRTest_ShapeData(const Draw_Color& CVis, const Draw_Color& COVis, const Draw_Color& CIVis, const Draw_Color& CHid, const Draw_Color& COHid, const Draw_Color& CIHid);
  
    void VisibleColor (const Draw_Color& CVis);
  
    void VisibleOutLineColor (const Draw_Color& COVis);
  
    void VisibleIsoColor (const Draw_Color& CIVis);
  
    void HiddenColor (const Draw_Color& CHid);
  
    void HiddenOutLineColor (const Draw_Color& COHid);
  
    void HiddenIsoColor (const Draw_Color& CIHid);
  
    Draw_Color VisibleColor() const;
  
    Draw_Color VisibleOutLineColor() const;
  
    Draw_Color VisibleIsoColor() const;
  
    Draw_Color HiddenColor() const;
  
    Draw_Color HiddenOutLineColor() const;
  
    Draw_Color HiddenIsoColor() const;




  DEFINE_STANDARD_RTTIEXT(HLRTest_ShapeData,Standard_Transient)

protected:




private:


  Draw_Color myVColor;
  Draw_Color myVOColor;
  Draw_Color myVIColor;
  Draw_Color myHColor;
  Draw_Color myHOColor;
  Draw_Color myHIColor;


};


#include <HLRTest_ShapeData.lxx>





#endif // _HLRTest_ShapeData_HeaderFile
