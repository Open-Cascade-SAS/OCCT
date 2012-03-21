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

#include <IGESData_DefSwitch.ixx>

//  DefSwitch : represente une definition, soit vide (-> valeur = 0),
//  soit comme rang dans une table (-> valeur > 0 ce rang),
//  soit comme reference (-> valeur < 0), la reference elle-meme est ailleurs


IGESData_DefSwitch::IGESData_DefSwitch ()             {  theval = 0;  }

    void IGESData_DefSwitch::SetVoid ()                   {  theval = 0;  }

    void IGESData_DefSwitch::SetReference ()              {  theval = -1;  }

    void IGESData_DefSwitch::SetRank (const Standard_Integer val)
                                                          {  theval = val;  }

    IGESData_DefType IGESData_DefSwitch::DefType () const
{
  if (theval <  0) return IGESData_DefReference;
  if (theval >  0) return IGESData_DefValue;
  return IGESData_DefVoid;
}

    Standard_Integer IGESData_DefSwitch::Value () const   {  return theval;  }
