// Copyright (c) 2015 OPEN CASCADE SAS
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

#include <StdPersistent_PPrsStd.hxx>


//=======================================================================
//function : Import
//purpose  : Import transient attribuite from the persistent data
//=======================================================================
void StdPersistent_PPrsStd::AISPresentation::Import
  (const Handle(TDataXtd_Presentation)& theAttribute) const
{
  theAttribute->SetDisplayed (myIsDisplayed);

  Handle(TCollection_HExtendedString) aDriverGUID = myDriverGUID->ExtString();
  if (aDriverGUID)
    theAttribute->SetDriverGUID (
      Standard_GUID (aDriverGUID->String().ToExtString()));

  if (myColor != -1)
    theAttribute->SetColor
      (static_cast<Quantity_NameOfColor> ((Standard_Integer)myColor));
  else
    theAttribute->UnsetColor();

  if (myMaterial != -1)
    theAttribute->SetMaterialIndex (myMaterial);
  else
    theAttribute->UnsetMaterial();
 
  if (myTransparency != -1.)
    theAttribute->SetTransparency (myTransparency);
  else
    theAttribute->UnsetTransparency();
 
  if (myWidth != -1.)
    theAttribute->SetWidth (myWidth);
  else
    theAttribute->UnsetWidth();
}

//=======================================================================
//function : Import
//purpose  : Import transient attribuite from the persistent data
//=======================================================================
void StdPersistent_PPrsStd::AISPresentation_1::Import
  (const Handle(TDataXtd_Presentation)& theAttribute) const
{
  AISPresentation::Import (theAttribute);
  theAttribute->SetMode (myMode);
}
