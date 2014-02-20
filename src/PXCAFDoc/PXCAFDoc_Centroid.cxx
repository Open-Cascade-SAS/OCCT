// Created on: 2000-09-08
// Created by: data exchange team
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#include <PXCAFDoc_Centroid.ixx>

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

PXCAFDoc_Centroid::PXCAFDoc_Centroid()
{
}

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

PXCAFDoc_Centroid::PXCAFDoc_Centroid(const gp_Pnt& pnt):
       myPCentroid(pnt)
{
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

 void PXCAFDoc_Centroid::Set(const gp_Pnt& pnt) 
{
  myPCentroid = pnt;
}

//=======================================================================
//function : Get
//purpose  : 
//=======================================================================

 gp_Pnt PXCAFDoc_Centroid::Get() const
{
  return myPCentroid;
}
