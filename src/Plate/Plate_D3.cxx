// Created on: 1998-02-17
// Created by: Andre LIEUTIER
// Copyright (c) 1998-1999 Matra Datavision
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



#include <Plate_D3.ixx>

Plate_D3::Plate_D3(const gp_XYZ& duuu, const gp_XYZ& duuv, const gp_XYZ& duvv, const gp_XYZ& dvvv)
:Duuu(duuu),Duuv(duuv),Duvv(duvv),Dvvv(dvvv)
{
}

Plate_D3::Plate_D3(const Plate_D3& ref)
:Duuu(ref.Duuu),Duuv(ref.Duuv),Duvv(ref.Duvv),Dvvv(ref.Dvvv)
{
}
