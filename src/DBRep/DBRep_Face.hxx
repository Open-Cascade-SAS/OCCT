// Created on: 1993-07-15
// Created by: Remi LEQUETTE
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

#ifndef _DBRep_Face_HeaderFile
#define _DBRep_Face_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TopoDS_Face.hxx>
#include <Draw_Color.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Integer.hxx>
#include <GeomAbs_IsoType.hxx>
#include <Standard_Real.hxx>
class TopoDS_Face;
class Draw_Color;


class DBRep_Face;
DEFINE_STANDARD_HANDLE(DBRep_Face, Standard_Transient)

//! Display of a face. Face + Array of iso + color.
class DBRep_Face : public Standard_Transient
{

public:

  
  //! N is the number of iso intervals.
  Standard_EXPORT DBRep_Face(const TopoDS_Face& F, const Standard_Integer N, const Draw_Color& C);
  
    const TopoDS_Face& Face() const;
  
    void Face (const TopoDS_Face& F);
  
    Standard_Integer NbIsos() const;
  
    void Iso (const Standard_Integer I, const GeomAbs_IsoType T, const Standard_Real Par, const Standard_Real T1, const Standard_Real T2);
  
    void GetIso (const Standard_Integer I, GeomAbs_IsoType& T, Standard_Real& Par, Standard_Real& T1, Standard_Real& T2) const;
  
    const Draw_Color& Color() const;
  
    void Color (const Draw_Color& C);




  DEFINE_STANDARD_RTTIEXT(DBRep_Face,Standard_Transient)

protected:




private:


  TopoDS_Face myFace;
  Draw_Color myColor;
  TColStd_Array1OfInteger myTypes;
  TColStd_Array1OfReal myParams;


};


#include <DBRep_Face.lxx>





#endif // _DBRep_Face_HeaderFile
