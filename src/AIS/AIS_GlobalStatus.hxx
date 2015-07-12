// Created on: 1997-01-24
// Created by: Robert COUBLANC
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _AIS_GlobalStatus_HeaderFile
#define _AIS_GlobalStatus_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <AIS_DisplayStatus.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <Quantity_NameOfColor.hxx>
#include <MMgt_TShared.hxx>


class AIS_GlobalStatus;
DEFINE_STANDARD_HANDLE(AIS_GlobalStatus, MMgt_TShared)

//! Stores  information  about objects in graphic context:
//! - Status Of Display : in the main viewer
//! hidden in the main viewer
//! - Displayed Modes
//! - Active Selection Modes
//! - is the Interactive Object Current ?
//! - Layer Index
class AIS_GlobalStatus : public MMgt_TShared
{

public:

  
  Standard_EXPORT AIS_GlobalStatus();
  
  Standard_EXPORT AIS_GlobalStatus(const AIS_DisplayStatus aStat, const Standard_Integer aDispMode, const Standard_Integer aSelMode, const Standard_Boolean ishilighted = Standard_False, const Quantity_NameOfColor TheHiCol = Quantity_NOC_WHITE, const Standard_Integer aLayerIndex = 0);
  
    void SetGraphicStatus (const AIS_DisplayStatus aStat);
  
    void AddDisplayMode (const Standard_Integer aMode);
  
    void AddSelectionMode (const Standard_Integer aMode);
  
    void SetLayerIndex (const Standard_Integer AnIndex);
  
    void SetHilightStatus (const Standard_Boolean aStat);
  
    void SetHilightColor (const Quantity_NameOfColor aHiCol);
  
    Standard_Boolean IsSubIntensityOn() const;
  
    void SubIntensityOn();
  
    void SubIntensityOff();
  
  Standard_EXPORT void RemoveDisplayMode (const Standard_Integer aMode);
  
  Standard_EXPORT void RemoveSelectionMode (const Standard_Integer aMode);
  
  Standard_EXPORT void ClearSelectionModes();
  
    AIS_DisplayStatus GraphicStatus() const;
  
  //! keeps the information of displayed modes in the
  //! main viewer.
    const TColStd_ListOfInteger& DisplayedModes() const;
  
  //! keeps the active selection modes of the object
  //! in the main viewer.
    const TColStd_ListOfInteger& SelectionModes() const;
  
    Standard_Boolean IsHilighted() const;
  
    Quantity_NameOfColor HilightColor() const;
  
  Standard_EXPORT Standard_Boolean IsDModeIn (const Standard_Integer aMode) const;
  
  Standard_EXPORT Standard_Boolean IsSModeIn (const Standard_Integer aMode) const;




  DEFINE_STANDARD_RTTI(AIS_GlobalStatus,MMgt_TShared)

protected:




private:


  AIS_DisplayStatus myStatus;
  TColStd_ListOfInteger myDispModes;
  TColStd_ListOfInteger mySelModes;
  Standard_Integer myLayerIndex;
  Standard_Boolean myIsHilit;
  Quantity_NameOfColor myHiCol;
  Standard_Boolean mySubInt;


};


#include <AIS_GlobalStatus.lxx>





#endif // _AIS_GlobalStatus_HeaderFile
