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

#include <IGESDraw_NetworkSubfigure.ixx>
#include <Standard_DimensionMismatch.hxx>
#include <gp_GTrsf.hxx>


IGESDraw_NetworkSubfigure::IGESDraw_NetworkSubfigure ()    {  }

    void IGESDraw_NetworkSubfigure::Init
  (const Handle(IGESDraw_NetworkSubfigureDef)&      aDefinition,
   const gp_XYZ&                                    aTranslation,
   const gp_XYZ&                                    aScaleFactor,
   const Standard_Integer                           aTypeFlag,
   const Handle(TCollection_HAsciiString)&          aDesignator,
   const Handle(IGESGraph_TextDisplayTemplate)&     aTemplate,
   const Handle(IGESDraw_HArray1OfConnectPoint)&    allConnectPoints)
{
  if (!allConnectPoints.IsNull())
    if (allConnectPoints->Lower() != 1)
      Standard_DimensionMismatch::Raise("IGESDraw_NetworkSubfigure : Init");
  theSubfigureDefinition = aDefinition;
  theTranslation         = aTranslation;
  theScaleFactor         = aScaleFactor;
  theTypeFlag            = aTypeFlag;
  theDesignator          = aDesignator;
  theDesignatorTemplate  = aTemplate;
  theConnectPoints       = allConnectPoints;
  InitTypeAndForm(420,0);
}

    Handle(IGESDraw_NetworkSubfigureDef)
    IGESDraw_NetworkSubfigure::SubfigureDefinition () const
{
  return theSubfigureDefinition;
}

    gp_XYZ IGESDraw_NetworkSubfigure::Translation () const
{
  return theTranslation;
}

    gp_XYZ IGESDraw_NetworkSubfigure::TransformedTranslation () const
{
  gp_XYZ TempXYZ = theTranslation;
  if (HasTransf()) Location().Transforms(TempXYZ);
  return ( TempXYZ );
}

    gp_XYZ IGESDraw_NetworkSubfigure::ScaleFactors () const
{
  return theScaleFactor;
}   

    Standard_Integer IGESDraw_NetworkSubfigure::TypeFlag () const
{
  return theTypeFlag;
}

    Handle(TCollection_HAsciiString) IGESDraw_NetworkSubfigure::ReferenceDesignator
  () const
{
  return theDesignator;
}

    Standard_Boolean IGESDraw_NetworkSubfigure::HasDesignatorTemplate () const
{
  return (! theDesignatorTemplate.IsNull() );
}

    Handle(IGESGraph_TextDisplayTemplate)
    IGESDraw_NetworkSubfigure::DesignatorTemplate () const
{
  return theDesignatorTemplate;
}

    Standard_Integer IGESDraw_NetworkSubfigure::NbConnectPoints () const
{
  return (theConnectPoints.IsNull() ? 0 : theConnectPoints->Length() );
}

    Handle(IGESDraw_ConnectPoint)  IGESDraw_NetworkSubfigure::ConnectPoint
  (const Standard_Integer Index) const
{
  return ( theConnectPoints->Value(Index) );
}
