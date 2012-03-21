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

#include <IGESData_ParamCursor.ixx>
#include <Interface_InterfaceError.hxx>



IGESData_ParamCursor::IGESData_ParamCursor (const Standard_Integer num)
{
  thestart = num;  thelimit = thestart+1;
  thecount = 1;    theadv   = Standard_True;
  theisize = 1;    theoffst = 0;   thetsize = 1;
}

    IGESData_ParamCursor::IGESData_ParamCursor
  (const Standard_Integer num,
   const Standard_Integer nb, const Standard_Integer size)
{
  thestart = num;  thelimit = num + (nb*size);
  thecount = nb;   theadv   = Standard_True;
  theisize = size; theoffst = 0;   thetsize = size;
}

    void  IGESData_ParamCursor::SetTerm
  (const Standard_Integer size, const Standard_Boolean autoadv)
{
  theoffst += thetsize;
  thetsize  = size;
  if (autoadv) theadv = (theoffst + thetsize == theisize);
  if (theoffst + thetsize > theisize) Interface_InterfaceError::Raise
    ("IGESDAta ParamCursor : required Term size overpass whole Item size");
}

    void  IGESData_ParamCursor::SetOne (const Standard_Boolean autoadv)
      {  SetTerm (1, autoadv);  }

    void  IGESData_ParamCursor::SetXY  (const Standard_Boolean autoadv)
      {  SetTerm (2, autoadv);  }

    void  IGESData_ParamCursor::SetXYZ (const Standard_Boolean autoadv)
      {  SetTerm (3, autoadv);  }

    void  IGESData_ParamCursor::SetAdvance(const Standard_Boolean advance)
      {  theadv = advance;  }

//  LA SUITE : inline , cf lxx
//     (Start,Limit,Count,ItemSize,TermSize,Offset,Advance)
