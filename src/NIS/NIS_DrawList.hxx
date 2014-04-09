// Created on: 2007-07-09
// Created by: Alexander GRIGORIEV
// Copyright (c) 2007-2014 OPEN CASCADE SAS
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

#ifndef NIS_DrawList_HeaderFile
#define NIS_DrawList_HeaderFile

#include <Handle_NIS_View.hxx>
#include <Handle_NIS_InteractiveObject.hxx>
#include <NCollection_List.hxx>

class NIS_InteractiveContext;
/**
 * This macro defines that OpenGL draw lists will be allocated as array of 5
 * integers and any of them would not be deleted unless all interactive objects
 * in the given drawer are removed.
 * When the macro is undefined every draw list is created when needed and it is
 * destroyed when there is no objects to show in this draw list.
 */
//#define ARRAY_LISTS

/**
 * Implementation of a set of OpenGL draw lists for a given NIS_Drawer and
 * given NIS_View. Stored in NIS_Drawer instances.
 */

class NIS_DrawList 
{
 public:
  // ---------- PUBLIC METHODS ----------


  /**
   * Empty constructor.
   */
  Standard_EXPORT NIS_DrawList ();

  /**
   * Constructor
   */
  Standard_EXPORT NIS_DrawList (const Handle(NIS_View)& theView);

  /**
   * Destructor.
   */
  Standard_EXPORT virtual ~NIS_DrawList ();

  /**
   * Query the list corresponding to the given type.
   * @param theType
   *   Integer value coinciding with the enumerated NIS_Drawer:DrawType.
   */
  inline Standard_Integer       GetListID (const Standard_Integer theType) const
#ifdef ARRAY_LISTS
  { return myListID + theType; }
#else
  { return myListID[theType]; }
#endif

  /**
   * Set myListID to 0.
   * @return
   *   Previous value of myListID
   */
  Standard_EXPORT void ClearListID        (const Standard_Integer theType);

  /**
   * Set myListID to 0.
   * @return
   *   Previous value of myListID
   */
  Standard_EXPORT void ClearListID        (const Handle(NIS_View)& theView=NULL);

  /**
   * This method is called to start recording a new list. It must be eventually
   * followed by EndPrepare.
   * @param theType
   *   Integer value coinciding with the enumerated NIS_Drawer::DrawType.
   */
  Standard_EXPORT virtual void  BeginPrepare   (const Standard_Integer theType);

  /**
   * This method is called to end recording a new list. It must be preceded
   * by BeginPrepare.
   * @param theType
   *   Integer value coinciding with the enumerated NIS_Drawer::DrawType.
   */
  Standard_EXPORT virtual void  EndPrepare     (const Standard_Integer theType);

  /**
   * Call the previously prepared list when the screen is redrawn.
   * @param theType
   *   Integer value coinciding with the enumerated NIS_Drawer::DrawType.
   */
  Standard_EXPORT virtual void  Call           (const Standard_Integer theType);

  /**
   * Query if the given list should be prepared again.
   * @param theType
   *   Integer value coinciding with the enumerated NIS_Drawer::DrawType.
   */
  inline Standard_Boolean       IsUpdated (const Standard_Integer theType) const
  { return myIsUpdated [theType]; }

  /**
   * Set the flag indicating that the List should be updated (rebuilt).
   */
  Standard_EXPORT void          SetUpdated     (const Standard_Integer theType);

  /**
   * Query if the given list should be processed by Dynamic Hilighting.
   */
  inline const NCollection_List<Handle(NIS_InteractiveObject)>&
                                DynHilightedList() const
  { return myDynHilighted; }
  
  /**
   * Query the View.
   */
  inline const Handle(NIS_View)& GetView        () const
  { return myView; }

  /**
   * Update the list of Dynamically Hilighted entities.
   */
  Standard_EXPORT Standard_Boolean SetDynHilighted
                                (const Standard_Boolean              isHilight,
                                 const Handle(NIS_InteractiveObject)& theObj);

 protected:
  // ---------- PROTECTED METHODS ----------
  Standard_EXPORT void          SetUpdated      (const Standard_Integer,
                                                 const Standard_Boolean);

#ifdef ARRAY_LISTS
  inline void                   SetListID       (const Standard_Integer theID)
  { myListID = theID; }
#endif

 private:
  // ---------- PRIVATE METHODS (PROHIBITED) ----------
 NIS_DrawList             (const NIS_DrawList& theOther);
 // NIS_DrawList& operator = (const NIS_DrawList& theOther);

 private:
  // ---------- PRIVATE FIELDS ----------

  Handle(NIS_View)                                myView;
#ifdef ARRAY_LISTS
  Standard_Integer                               myListID;
#else
  Standard_Integer                               myListID[5];
#endif
  Standard_Boolean                               myIsUpdated[5];
  NCollection_List<Handle(NIS_InteractiveObject)> myDynHilighted;
};


#endif
