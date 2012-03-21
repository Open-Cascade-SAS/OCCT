// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
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

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <IGESGraph_LineFontDefTemplate.ixx>


IGESGraph_LineFontDefTemplate::IGESGraph_LineFontDefTemplate ()    {  }


// This class inherits from IGESData_LineFontEntity

    void IGESGraph_LineFontDefTemplate::Init
  (const Standard_Integer                anOrientation,
   const Handle(IGESBasic_SubfigureDef)& aTemplate,
   const Standard_Real                   aDistance,
   const Standard_Real                   aScale)
{
  theOrientation    = anOrientation;
  theTemplateEntity = aTemplate;
  theDistance       = aDistance;
  theScale          = aScale;
  InitTypeAndForm(304,1);
}

    Standard_Integer IGESGraph_LineFontDefTemplate::Orientation () const
{
  return theOrientation;
}

    Handle(IGESBasic_SubfigureDef) IGESGraph_LineFontDefTemplate::TemplateEntity
  () const
{
  return theTemplateEntity;
}

    Standard_Real IGESGraph_LineFontDefTemplate::Distance () const
{
  return theDistance;
}

    Standard_Real IGESGraph_LineFontDefTemplate::Scale () const
{
  return theScale;
}
