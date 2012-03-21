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

#include <IGESBasic_SingularSubfigure.ixx>
#include <gp_GTrsf.hxx>


IGESBasic_SingularSubfigure::IGESBasic_SingularSubfigure ()    {  }


    void  IGESBasic_SingularSubfigure::Init
  (const Handle(IGESBasic_SubfigureDef)& aSubfigureDef,
   const gp_XYZ& aTranslation, const Standard_Boolean hasScale,
   const Standard_Real aScale)
{
  theSubfigureDef = aSubfigureDef;
  theTranslation  = aTranslation;
  hasScaleFactor  = hasScale;
  theScaleFactor  = aScale;
  InitTypeAndForm(408,0);
}

    Handle(IGESBasic_SubfigureDef) IGESBasic_SingularSubfigure::Subfigure () const
{
  return theSubfigureDef;
}

    gp_XYZ  IGESBasic_SingularSubfigure::Translation () const
{
  return theTranslation;
}

    Standard_Boolean  IGESBasic_SingularSubfigure::HasScaleFactor () const
{
  return hasScaleFactor;
}

    Standard_Real  IGESBasic_SingularSubfigure::ScaleFactor () const
{
  return theScaleFactor;
}

    gp_XYZ  IGESBasic_SingularSubfigure::TransformedTranslation () const
{
  gp_XYZ tmp = theTranslation;
  if (HasTransf()) Location().Transforms(tmp);
  return tmp;
}
