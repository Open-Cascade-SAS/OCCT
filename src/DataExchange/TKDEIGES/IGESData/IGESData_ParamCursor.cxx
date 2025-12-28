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

#include <IGESData_ParamCursor.hxx>
#include <Interface_InterfaceError.hxx>

IGESData_ParamCursor::IGESData_ParamCursor(const int num)
{
  thestart = num;
  thelimit = thestart + 1;
  thecount = 1;
  theadv   = true;
  theisize = 1;
  theoffst = 0;
  thetsize = 1;
}

IGESData_ParamCursor::IGESData_ParamCursor(const int num, const int nb, const int size)
{
  thestart = num;
  thelimit = num + (nb * size);
  thecount = nb;
  theadv   = true;
  theisize = size;
  theoffst = 0;
  thetsize = size;
}

void IGESData_ParamCursor::SetTerm(const int size, const bool autoadv)
{
  theoffst += thetsize;
  thetsize = size;
  if (autoadv)
    theadv = (theoffst + thetsize == theisize);
  if (theoffst + thetsize > theisize)
    throw Interface_InterfaceError(
      "IGESDAta ParamCursor : required Term size overpass whole Item size");
}

void IGESData_ParamCursor::SetOne(const bool autoadv)
{
  SetTerm(1, autoadv);
}

void IGESData_ParamCursor::SetXY(const bool autoadv)
{
  SetTerm(2, autoadv);
}

void IGESData_ParamCursor::SetXYZ(const bool autoadv)
{
  SetTerm(3, autoadv);
}

void IGESData_ParamCursor::SetAdvance(const bool advance)
{
  theadv = advance;
}

//  THE REST : inline , see lxx
//     (Start,Limit,Count,ItemSize,TermSize,Offset,Advance)
