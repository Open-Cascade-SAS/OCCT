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


#include <MoniTool_Element.hxx>
#include <MoniTool_ElemHasher.hxx>

Standard_Integer  MoniTool_ElemHasher::HashCode
  (const Handle(MoniTool_Element)& K, const Standard_Integer Upper)
{
  return ((K->GetHashCode() - 1) % Upper) + 1;
}

    Standard_Boolean  MoniTool_ElemHasher::IsEqual
  (const Handle(MoniTool_Element)& K1, const Handle(MoniTool_Element)& K2)
{
  if (K1.IsNull()) return Standard_False;
  return K1->Equates(K2);
}
