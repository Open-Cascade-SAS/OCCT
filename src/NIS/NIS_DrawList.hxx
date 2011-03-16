// File:      NIS_DrawList.hxx
// Created:   09.07.07 21:37
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2007


#ifndef NIS_DrawList_HeaderFile
#define NIS_DrawList_HeaderFile

#include <Handle_NIS_View.hxx>
#include <Handle_NIS_InteractiveObject.hxx>
#include <NCollection_List.hxx>

class NIS_InteractiveContext;

/**
 * Block of comments describing class NIS_DrawList
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
  Standard_EXPORT NIS_DrawList (const Handle_NIS_View& theView);

  /**
   * Destructor.
   */
  Standard_EXPORT virtual ~NIS_DrawList ();

  /**
   * Query the list corresponding to the given type.
   * @param theType
   *   Integer value coinciding with the enumerated NIS_Drawer:DrawType.
   */
  inline Standard_Integer       GetListID      (const Standard_Integer theType)
  { return myListID + (theType&0x3); }

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
  inline Standard_Boolean       IsUpdated      (const Standard_Integer theType)
  { return myIsUpdated [theType&0x3]; }

  /**
   * Set the flag indicating that the List should be updated (rebuilt).
   */
  inline void                   SetUpdated     (const Standard_Integer theType)
  { myIsUpdated [theType&0x3] = Standard_True; }

  /**
   * Query if the given list should be processed by Dynamic Hilighting.
   */
  inline const NCollection_List<Handle_NIS_InteractiveObject>&
                                DynHilightedList() const
  { return myDynHilighted; }
  
  /**
   * Query the View.
   */
  inline const Handle_NIS_View& GetView        () const
  { return myView; }

  /**
   * Update the list of Dynamically Hilighted entities.
   */
  Standard_EXPORT Standard_Boolean SetDynHilighted
                                (const Standard_Boolean              isHilight,
                                 const Handle_NIS_InteractiveObject& theObj);

 protected:
  // ---------- PROTECTED METHODS ----------
  Standard_EXPORT void          SetUpdated      (const Standard_Integer,
                                                 const Standard_Boolean);

  inline void                   SetListID       (const Standard_Integer theID)
  { myListID = theID; }



 private:
  // ---------- PRIVATE METHODS (PROHIBITED) ----------
 NIS_DrawList             (const NIS_DrawList& theOther);
 // NIS_DrawList& operator = (const NIS_DrawList& theOther);

 private:
  // ---------- PRIVATE FIELDS ----------

  Handle_NIS_View                                myView;
  Standard_Integer                               myListID;
  Standard_Boolean                               myIsUpdated[4];
  NCollection_List<Handle_NIS_InteractiveObject> myDynHilighted;
};


#endif
