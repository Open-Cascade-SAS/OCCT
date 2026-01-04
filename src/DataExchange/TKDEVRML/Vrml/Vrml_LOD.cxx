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

#include <gp_Vec.hxx>
#include <Standard_Type.hxx>
#include <Vrml_LOD.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Vrml_LOD, Standard_Transient)

Vrml_LOD::Vrml_LOD()
{
  myRange = new NCollection_HArray1<double>(1, 1);
  gp_Vec tmpVec(0, 0, 0);
  myCenter    = tmpVec;
  myRangeFlag = false;
}

Vrml_LOD::Vrml_LOD(const occ::handle<NCollection_HArray1<double>>& aRange, const gp_Vec& aCenter)
{
  myRange     = aRange;
  myCenter    = aCenter;
  myRangeFlag = true;
}

void Vrml_LOD::SetRange(const occ::handle<NCollection_HArray1<double>>& aRange)
{
  myRange     = aRange;
  myRangeFlag = true;
}

occ::handle<NCollection_HArray1<double>> Vrml_LOD::Range() const
{
  return myRange;
}

void Vrml_LOD::SetCenter(const gp_Vec& aCenter)
{
  myCenter = aCenter;
}

gp_Vec Vrml_LOD::Center() const
{
  return myCenter;
}

Standard_OStream& Vrml_LOD::Print(Standard_OStream& anOStream) const
{
  int i;
  anOStream << "LOD {\n";

  if (myRangeFlag)
  {
    anOStream << "    range [\n\t";
    for (i = myRange->Lower(); i <= myRange->Upper(); i++)
    {
      anOStream << myRange->Value(i);
      if (i < myRange->Length())
        anOStream << ",";
    }
    anOStream << " ]\n";
  }

  if (std::abs(myCenter.X() - 0) > 0.0001 || std::abs(myCenter.Y() - 0) > 0.0001
      || std::abs(myCenter.Z() - 0) > 0.0001)
  {
    anOStream << "    center\t";
    anOStream << myCenter.X() << " " << myCenter.Y() << " " << myCenter.Z() << "\n";
  }

  anOStream << "}\n";
  return anOStream;
}
