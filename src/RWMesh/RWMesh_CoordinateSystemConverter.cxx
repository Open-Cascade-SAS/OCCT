// Author: Kirill Gavrilov
// Copyright (c) 2015-2019 OPEN CASCADE SAS
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

#include <RWMesh_CoordinateSystemConverter.hxx>

#include <gp_Quaternion.hxx>

// ================================================================
// Function : RWMesh_CoordinateSystemConverter
// Purpose  :
// ================================================================
RWMesh_CoordinateSystemConverter::RWMesh_CoordinateSystemConverter()
: myInputLengthUnit (-1.0),
  myOutputLengthUnit(-1.0),
  myHasInputAx3 (Standard_False),
  myHasOutputAx3(Standard_False),
  //
  myUnitFactor (1),
  myHasScale (Standard_False),
  myIsEmpty  (Standard_True)
{
  //
}

// ================================================================
// Function : Init
// Purpose  :
// ================================================================
void RWMesh_CoordinateSystemConverter::Init (const gp_Ax3& theInputSystem,
                                             Standard_Real theInputLengthUnit,
                                             const gp_Ax3& theOutputSystem,
                                             Standard_Real theOutputLengthUnit)
{
  myInputLengthUnit  = theInputLengthUnit;
  myOutputLengthUnit = theOutputLengthUnit;
  myInputAx3         = theInputSystem;
  myOutputAx3        = theOutputSystem;
  if (theInputLengthUnit  > 0.0
   && theOutputLengthUnit > 0.0)
  {
    myUnitFactor = theInputLengthUnit / theOutputLengthUnit;
    myHasScale = Abs(myUnitFactor - 1.0) > gp::Resolution();
  }
  else
  {
    myUnitFactor = 1.0;
    myHasScale = Standard_False;
  }

  gp_Trsf aTrsf;
  if (myHasInputAx3
   && myHasOutputAx3)
  {
    aTrsf.SetTransformation (theOutputSystem, theInputSystem);
    if (aTrsf.TranslationPart().IsEqual (gp_XYZ (0.0, 0.0, 0.0), gp::Resolution())
     && aTrsf.GetRotation().IsEqual (gp_Quaternion()))
    {
      aTrsf = gp_Trsf();
    }
  }

  myTrsf    = aTrsf;
  myTrsfInv = aTrsf.Inverted();
  myTrsf.GetMat4 (myNormTrsf);
  myIsEmpty = !myHasScale && myTrsf.Form() == gp_Identity;
}
