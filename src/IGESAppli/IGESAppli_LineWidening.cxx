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

#include <IGESAppli_LineWidening.ixx>
#include <IGESData_LevelListEntity.hxx>


IGESAppli_LineWidening::IGESAppli_LineWidening ()    {  }


    void  IGESAppli_LineWidening::Init
  (const Standard_Integer nbPropVal,
   const Standard_Real    aWidth,    const Standard_Integer aCornering,
   const Standard_Integer aExtnFlag, const Standard_Integer aJustifFlag,
   const Standard_Real    aExtnVal)
{
  theNbPropertyValues  = nbPropVal;
  theWidth             = aWidth;
  theCorneringCode     = aCornering;
  theExtensionFlag     = aExtnFlag;
  theJustificationFlag = aJustifFlag;
  theExtensionValue    = aExtnVal;
  InitTypeAndForm(406,5);
}


    Standard_Integer  IGESAppli_LineWidening::NbPropertyValues () const
{
  return theNbPropertyValues;
}

    Standard_Real  IGESAppli_LineWidening::WidthOfMetalization () const
{
  return theWidth;
}

    Standard_Integer  IGESAppli_LineWidening::CorneringCode () const
{
  return theCorneringCode;
}

    Standard_Integer  IGESAppli_LineWidening::ExtensionFlag () const
{
  return theExtensionFlag;
}

    Standard_Integer  IGESAppli_LineWidening::JustificationFlag () const
{
  return theJustificationFlag;
}

    Standard_Real  IGESAppli_LineWidening::ExtensionValue () const
{
  return theExtensionValue;
}
