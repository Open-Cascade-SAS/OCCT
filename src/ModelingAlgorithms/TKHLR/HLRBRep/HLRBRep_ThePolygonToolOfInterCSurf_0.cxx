// Created on: 1992-10-14
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

#include <HLRBRep_ThePolygonToolOfInterCSurf.hxx>

#include <Standard_OutOfRange.hxx>
#include <gp_Pnt.hxx>
#include <HLRBRep_ThePolygonOfInterCSurf.hxx>
#include <Bnd_Box.hxx>

#include <iostream>

void HLRBRep_ThePolygonToolOfInterCSurf::Dump(const HLRBRep_ThePolygonOfInterCSurf& L)
{
  static Standard_Integer num = 0;
  num++;
  Standard_Integer nbs = HLRBRep_ThePolygonToolOfInterCSurf::NbSegments(L);
  std::cout << "\npol2d " << num << " " << nbs << std::endl;
  std::cout << "Deflection " << HLRBRep_ThePolygonToolOfInterCSurf::DeflectionOverEstimation(L)
            << std::endl;

  for (Standard_Integer i = 1; i <= nbs; i++)
  {
    gp_Pnt P(HLRBRep_ThePolygonToolOfInterCSurf::BeginOfSeg(L, i));
    std::cout << "pnt " << num << " " << i << " " << P.X() << " " << P.Y() << " " << P.Z()
              << std::endl;
  }
  gp_Pnt PF(HLRBRep_ThePolygonToolOfInterCSurf::EndOfSeg(L, nbs));
  std::cout << "pnt " << num << " " << nbs << " " << PF.X() << " " << PF.Y() << " " << PF.Z()
            << std::endl;
}
