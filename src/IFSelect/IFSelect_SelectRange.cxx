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

#include <IFSelect_SelectRange.ixx>
#include <stdio.h>


IFSelect_SelectRange::IFSelect_SelectRange ()    {  }

    void  IFSelect_SelectRange::SetRange
  (const Handle(IFSelect_IntParam)& rankfrom,
   const Handle(IFSelect_IntParam)& rankto)
      {  thelower = rankfrom;  theupper = rankto;  }

    void  IFSelect_SelectRange::SetOne (const Handle(IFSelect_IntParam)& rank)
      {  thelower = theupper = rank;  }

    void  IFSelect_SelectRange::SetFrom
  (const Handle(IFSelect_IntParam)& rankfrom)
      {  thelower = rankfrom;  theupper.Nullify();  }

    void  IFSelect_SelectRange::SetUntil
  (const Handle(IFSelect_IntParam)& rankto)
      {  thelower.Nullify();  theupper = rankto;  }


    Standard_Boolean  IFSelect_SelectRange::HasLower () const 
      {  return (!thelower.IsNull());  }

    Handle(IFSelect_IntParam)  IFSelect_SelectRange::Lower () const 
      {  return thelower;  }

    Standard_Integer  IFSelect_SelectRange::LowerValue () const
{
  if (thelower.IsNull()) return 0;
  return thelower->Value();
}

    Standard_Boolean  IFSelect_SelectRange::HasUpper () const 
      {  return (!theupper.IsNull());  }

    Handle(IFSelect_IntParam)  IFSelect_SelectRange::Upper () const 
      {  return theupper;  }

    Standard_Integer  IFSelect_SelectRange::UpperValue () const
{
  if (theupper.IsNull()) return 0;
  return theupper->Value();
}

    Standard_Boolean  IFSelect_SelectRange::Sort
  (const Standard_Integer rank, const Handle(Standard_Transient)& ,
   const Handle(Interface_InterfaceModel)& ) const 
{
  Standard_Integer rankfrom = 0;
  if (!thelower.IsNull()) rankfrom = thelower->Value();
  Standard_Integer rankto   = 0;
  if (!theupper.IsNull()) rankto   = theupper->Value();
  return (rank >= rankfrom && (rankto == 0 || rankto >= rank));
}

    TCollection_AsciiString  IFSelect_SelectRange::ExtractLabel () const 
{
  char lab[30];
  Standard_Integer rankfrom = 0;
  if (!thelower.IsNull()) rankfrom = thelower->Value();
  Standard_Integer rankto   = 0;
  if (!theupper.IsNull()) rankto   = theupper->Value();
  if (rankfrom == rankto) sprintf(lab,"Rank no %d",rankfrom);
  else if (rankfrom == 0) sprintf(lab,"Until no %d",rankto);
  else if (rankto   == 0) sprintf(lab,"From no %d",rankto);
  else                    sprintf(lab,"From %d Until %d",rankfrom,rankto);

  return TCollection_AsciiString(lab);
}
