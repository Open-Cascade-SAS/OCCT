// Created on: 1998-02-17
// Created by: Andre LIEUTIER
// Copyright (c) 1998-1999 Matra Datavision
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

#include <Plate_D3.hxx>

Plate_D3::Plate_D3(const gp_XYZ& duuu, const gp_XYZ& duuv, const gp_XYZ& duvv, const gp_XYZ& dvvv)
    : Duuu(duuu),
      Duuv(duuv),
      Duvv(duvv),
      Dvvv(dvvv)
{
}

Plate_D3::Plate_D3(const Plate_D3& ref)
    : Duuu(ref.Duuu),
      Duuv(ref.Duuv),
      Duvv(ref.Duvv),
      Dvvv(ref.Dvvv)
{
}
