// Created on: 2005-10-14
// Created by: Mikhail KLOKOV
// Copyright (c) 2005-2014 OPEN CASCADE SAS
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

#ifndef _IntTools_SurfaceRangeLocalizeData_HeaderFile
#define _IntTools_SurfaceRangeLocalizeData_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <IntTools_MapOfSurfaceSample.hxx>
#include <IntTools_DataMapOfSurfaceSampleBox.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColgp_HArray2OfPnt.hxx>
#include <Standard_Boolean.hxx>
#include <IntTools_ListOfSurfaceRangeSample.hxx>
class IntTools_SurfaceRangeSample;
class Bnd_Box;
class gp_Pnt;



class IntTools_SurfaceRangeLocalizeData 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT IntTools_SurfaceRangeLocalizeData();
  
  Standard_EXPORT IntTools_SurfaceRangeLocalizeData(const Standard_Integer theNbSampleU, const Standard_Integer theNbSampleV, const Standard_Real theMinRangeU, const Standard_Real theMinRangeV);
  
  Standard_EXPORT IntTools_SurfaceRangeLocalizeData(const IntTools_SurfaceRangeLocalizeData& Other);
  
  Standard_EXPORT IntTools_SurfaceRangeLocalizeData& Assign (const IntTools_SurfaceRangeLocalizeData& Other);
IntTools_SurfaceRangeLocalizeData& operator = (const IntTools_SurfaceRangeLocalizeData& Other)
{
  return Assign(Other);
}
  
    Standard_Integer GetNbSampleU() const;
  
    Standard_Integer GetNbSampleV() const;
  
    Standard_Real GetMinRangeU() const;
  
    Standard_Real GetMinRangeV() const;
  
  Standard_EXPORT void AddOutRange (const IntTools_SurfaceRangeSample& theRange);
  
  Standard_EXPORT void AddBox (const IntTools_SurfaceRangeSample& theRange, const Bnd_Box& theBox);
  
  Standard_EXPORT Standard_Boolean FindBox (const IntTools_SurfaceRangeSample& theRange, Bnd_Box& theBox) const;
  
  Standard_EXPORT Standard_Boolean IsRangeOut (const IntTools_SurfaceRangeSample& theRange) const;
  
  Standard_EXPORT void ListRangeOut (IntTools_ListOfSurfaceRangeSample& theList) const;
  
  Standard_EXPORT void RemoveRangeOutAll();
  
  //! Set the grid deflection.
    void SetGridDeflection (const Standard_Real theDeflection);
  
  //! Query the grid deflection.
    Standard_Real GetGridDeflection() const;
  
  //! Set the range U of the grid of points.
  Standard_EXPORT void SetRangeUGrid (const Standard_Integer theNbUGrid);
  
  //! Query the range U of the grid of points.
    Standard_Integer GetRangeUGrid() const;
  
  //! Set the U parameter of the grid points at that index.
    void SetUParam (const Standard_Integer theIndex, const Standard_Real theUParam);
  
  //! Query the U parameter of the grid points at that index.
    Standard_Real GetUParam (const Standard_Integer theIndex) const;
  
  //! Set the range V of the grid of points.
  Standard_EXPORT void SetRangeVGrid (const Standard_Integer theNbVGrid);
  
  //! Query the range V of the grid of points.
    Standard_Integer GetRangeVGrid() const;
  
  //! Set the V parameter of the grid points at that index.
    void SetVParam (const Standard_Integer theIndex, const Standard_Real theVParam);
  
  //! Query the V parameter of the grid points at that index.
    Standard_Real GetVParam (const Standard_Integer theIndex) const;
  
  //! Set the grid point.
    void SetGridPoint (const Standard_Integer theUIndex, const Standard_Integer theVIndex, const gp_Pnt& thePoint);
  
  //! Set the grid point.
    const gp_Pnt& GetGridPoint (const Standard_Integer theUIndex, const Standard_Integer theVIndex) const;
  
  //! Sets the frame area. Used to work with grid points.
  Standard_EXPORT void SetFrame (const Standard_Real theUMin, const Standard_Real theUMax, const Standard_Real theVMin, const Standard_Real theVMax);
  
  //! Returns the number of grid points on U direction in frame.
    Standard_Integer GetNBUPointsInFrame() const;
  
  //! Returns the number of grid points on V direction in frame.
    Standard_Integer GetNBVPointsInFrame() const;
  
  //! Returns the grid point in frame.
  Standard_EXPORT const gp_Pnt& GetPointInFrame (const Standard_Integer theUIndex, const Standard_Integer theVIndex) const;
  
  //! Query the U parameter of the grid points
  //! at that index in frame.
  Standard_EXPORT Standard_Real GetUParamInFrame (const Standard_Integer theIndex) const;
  
  //! Query the V parameter of the grid points
  //! at that index in frame.
  Standard_EXPORT Standard_Real GetVParamInFrame (const Standard_Integer theIndex) const;
  
  //! Clears the grid of points.
  Standard_EXPORT void ClearGrid();




protected:





private:



  Standard_Integer myNbSampleU;
  Standard_Integer myNbSampleV;
  Standard_Real myMinRangeU;
  Standard_Real myMinRangeV;
  IntTools_MapOfSurfaceSample myMapRangeOut;
  IntTools_DataMapOfSurfaceSampleBox myMapBox;
  Handle(TColStd_HArray1OfReal) myUParams;
  Handle(TColStd_HArray1OfReal) myVParams;
  Handle(TColgp_HArray2OfPnt) myGridPoints;
  Standard_Integer myUIndMin;
  Standard_Integer myUIndMax;
  Standard_Integer myVIndMin;
  Standard_Integer myVIndMax;
  Standard_Real myDeflection;


};


#include <IntTools_SurfaceRangeLocalizeData.lxx>





#endif // _IntTools_SurfaceRangeLocalizeData_HeaderFile
