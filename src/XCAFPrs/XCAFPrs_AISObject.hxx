// Copyright (c) 2014 OPEN CASCADE SAS
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

#ifndef _XCAFPrs_AISObject_HeaderFile
#define _XCAFPrs_AISObject_HeaderFile

#include <AIS_ColoredShape.hxx>

#include <TDF_Label.hxx>

class XCAFPrs_Style;

//! Implements AIS_InteractiveObject functionality for shape in DECAF document.
class XCAFPrs_AISObject : public AIS_ColoredShape
{
public:

  //! Creates an object to visualise the shape label.
  Standard_EXPORT XCAFPrs_AISObject (const TDF_Label& theLabel);

protected:

  //! Redefined method to compute presentation.
  Standard_EXPORT virtual  void Compute (const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
                                         const Handle(Prs3d_Presentation)&           thePresentation,
                                         const Standard_Integer                      theMode);

  //! Set colors to drawer
  Standard_EXPORT void SetColors (const Handle(Prs3d_Drawer)& theDrawer,
                                  const Quantity_Color&       theColorCurv,
                                  const Quantity_Color&       theColorSurf);

  //! Fills out a default style object which is used when styles are
  //! not explicitly defined in the document.
  //! By default, the style uses white color for curves and surfaces.
  Standard_EXPORT virtual  void DefaultStyle (XCAFPrs_Style& theStyle) const;

private:

  TDF_Label myLabel;

public:

  DEFINE_STANDARD_RTTI(XCAFPrs_AISObject)

};

DEFINE_STANDARD_HANDLE(XCAFPrs_AISObject, AIS_ColoredShape)

#endif // _XCAFPrs_AISObject_HeaderFile
