// Created on: 1995-05-29
// Created by: Jacques GOUSSARD
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _LocOpe_PntFace_HeaderFile
#define _LocOpe_PntFace_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <gp_Pnt.hxx>
#include <TopoDS_Face.hxx>
#include <TopAbs_Orientation.hxx>
class gp_Pnt;
class TopoDS_Face;



class LocOpe_PntFace 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Empty constructor. Useful only for the list.
  LocOpe_PntFace()
  : myPar(0.0),
    myUPar(0.0),
    myVPar(0.0)
  {
  }
  
  LocOpe_PntFace (const gp_Pnt& P, const TopoDS_Face& F, const TopAbs_Orientation Or, const Standard_Real Param, const Standard_Real UPar, const Standard_Real VPar)
    : myPnt (P), myFace (F), myOri (Or), myPar (Param), myUPar (UPar), myVPar (VPar)
  {
  }
  
  const gp_Pnt& Pnt () const { return myPnt; }
  
  const TopoDS_Face& Face () const { return myFace; }
  
  TopAbs_Orientation Orientation () const { return myOri; }
  
  TopAbs_Orientation& ChangeOrientation () { return myOri; }
  
  Standard_Real Parameter () const { return myPar; }
  
  Standard_Real UParameter () const { return myUPar; }

  Standard_Real VParameter () const { return myVPar; }

private:
  gp_Pnt myPnt;
  TopoDS_Face myFace;
  TopAbs_Orientation myOri;
  Standard_Real myPar;
  Standard_Real myUPar;
  Standard_Real myVPar;
};

#endif // _LocOpe_PntFace_HeaderFile
