// Created on: 1993-10-20
// Created by: Jean-Louis FRENKEL
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

#ifndef _Prs3d_PlaneSet_HeaderFile
#define _Prs3d_PlaneSet_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <gp_Pln.hxx>
#include <Quantity_Length.hxx>
#include <MMgt_TShared.hxx>
#include <Standard_Real.hxx>
class gp_Pln;


class Prs3d_PlaneSet;
DEFINE_STANDARD_HANDLE(Prs3d_PlaneSet, MMgt_TShared)

//! defines a set of planes used for a presentation
//! by sections.
class Prs3d_PlaneSet : public MMgt_TShared
{

public:

  
  Standard_EXPORT Prs3d_PlaneSet(const Standard_Real Xdir, const Standard_Real Ydir, const Standard_Real Zdir, const Quantity_Length Xloc, const Quantity_Length Yloc, const Quantity_Length Zloc, const Quantity_Length anOffset);
  
  Standard_EXPORT void SetDirection (const Standard_Real X, const Standard_Real Y, const Standard_Real Z);
  
  Standard_EXPORT void SetLocation (const Quantity_Length X, const Quantity_Length Y, const Quantity_Length Z);
  
  Standard_EXPORT void SetOffset (const Quantity_Length anOffset);
  
  Standard_EXPORT gp_Pln Plane() const;
  
  Standard_EXPORT Quantity_Length Offset() const;
  
  Standard_EXPORT void Location (Quantity_Length& X, Quantity_Length& Y, Quantity_Length& Z) const;
  
  Standard_EXPORT void Direction (Quantity_Length& X, Quantity_Length& Y, Quantity_Length& Z) const;




  DEFINE_STANDARD_RTTI(Prs3d_PlaneSet,MMgt_TShared)

protected:




private:


  gp_Pln myPlane;
  Quantity_Length myOffset;


};







#endif // _Prs3d_PlaneSet_HeaderFile
