// Copyright (c) 2016 OPEN CASCADE SAS
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

#ifndef _SelectMgr_ToleranceMap_HeaderFile
#define _SelectMgr_ToleranceMap_HeaderFile

#include <NCollection_DataMap.hxx>

//! An internal class for calculation of current largest tolerance value which will be applied for
//! creation of selecting frustum by default. Each time the selection set is deactivated, maximum
//! tolerance value will be recalculated. If a user enables custom precision using
//! StdSelect_ViewerSelector3d::SetPixelTolerance, it will be applied to all sensitive entities
//! without any checks.
class SelectMgr_ToleranceMap
{
public:
  //! Sets tolerance values to -1.0
  Standard_EXPORT SelectMgr_ToleranceMap();

  Standard_EXPORT ~SelectMgr_ToleranceMap();

  //! Adds the value given to map, checks if the current tolerance value
  //! should be replaced by theTolerance
  Standard_EXPORT void Add(const Standard_Integer& theTolerance);

  //! Decrements a counter of the tolerance given, checks if the current tolerance value
  //! should be recalculated
  Standard_EXPORT void Decrement(const Standard_Integer& theTolerance);

  //! Returns a current tolerance that must be applied
  Standard_Integer Tolerance() const
  {
    if (myLargestKey < 0)
    {
      return 2; // default tolerance value
    }
    return myCustomTolerance < 0 ? myLargestKey : myLargestKey + myCustomTolerance;
  }

  //! Sets tolerance to the given one and disables adaptive checks
  void SetCustomTolerance(const Standard_Integer theTolerance) { myCustomTolerance = theTolerance; }

  //! Unsets a custom tolerance and enables adaptive checks
  void ResetDefaults() { myCustomTolerance = -1; }

  //! Returns the value of custom tolerance regardless of it validity
  Standard_Integer CustomTolerance() const { return myCustomTolerance; }

  //! Returns true if custom tolerance value is greater than zero
  Standard_Boolean IsCustomTolSet() const { return myCustomTolerance > 0; }

private:
  NCollection_DataMap<Standard_Integer, Standard_Integer> myTolerances;
  Standard_Integer                                        myLargestKey;
  Standard_Integer                                        myCustomTolerance;
};

#endif // _SelectMgr_ToleranceMap_HeaderFile
