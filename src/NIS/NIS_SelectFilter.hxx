// File:      NIS_SelectFilter.hxx
// Created:   20.08.07 13:49
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2007


#ifndef NIS_SelectFilter_HeaderFile
#define NIS_SelectFilter_HeaderFile

#include <Standard_DefineHandle.hxx>
#include <Standard_Transient.hxx>

class NIS_InteractiveObject;
class Handle_Standard_Type;

/**
 * Interface for selection filters. It can be used in NIS_InteractiveContext
 * by methods SetFilter and GetFilter.<br>
 * Unlike in AIS, only one filter can be installed in InteractiveContext. If
 * you need more than one filter instance to work together, create a composite
 * NIS_SelectFilter specialization that would hold a list of simpler Filter
 * instances.
 */

class NIS_SelectFilter : public Standard_Transient
{
 public:
  // ---------- PUBLIC METHODS ----------


  /**
   * Empty constructor.
   */
  inline NIS_SelectFilter () {}

  /**
   * Indicate that the given NIS_InteractiveObject passes the Filter.
   * @return
   *   True - theObject is kept in the Selection, False - excluded from it.
   */
  virtual Standard_Boolean
        IsOk    (const NIS_InteractiveObject * theObject)       const = 0;

  /**
   * Check if the type of InteractiveObject is allowed for selection.
   * Default implementation returns always True. 
   * @return
   *   True if objects of the given Type should be checked, False if such
   *   objects are excluded from Selection before any checking.
   */ 
  Standard_EXPORT virtual Standard_Boolean
        ActsOn  (const Handle_Standard_Type& theType)           const;


 protected:
  // ---------- PROTECTED METHODS ----------




 public:
// Declaration of CASCADE RTTI
DEFINE_STANDARD_RTTI (NIS_SelectFilter)
};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE (NIS_SelectFilter, Standard_Transient)

#endif
