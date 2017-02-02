// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
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

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <gp_GTrsf.hxx>
#include <gp_XYZ.hxx>
#include <IGESDraw_ConnectPoint.hxx>
#include <IGESDraw_NetworkSubfigure.hxx>
#include <IGESDraw_NetworkSubfigureDef.hxx>
#include <IGESGraph_TextDisplayTemplate.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDraw_NetworkSubfigure,IGESData_IGESEntity)

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
      throw Standard_DimensionMismatch("IGESDraw_NetworkSubfigure : Init");
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
