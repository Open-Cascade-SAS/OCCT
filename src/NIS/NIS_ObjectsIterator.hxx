// File:      NIS_ObjectsIterator.hxx
// Created:   02.09.07 23:47
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2007


#ifndef NIS_ObjectsIterator_HeaderFile
#define NIS_ObjectsIterator_HeaderFile

#include <NCollection_Vector.hxx>
#include <Handle_NIS_InteractiveObject.hxx>

class Handle_NIS_InteractiveContext;

/**
 * Iterator of objects contained in a NIS_InteractiveContext instance. The
 * iteration is always in the ascending sense of object ID. Examples:
 * @code
 *    // Erase all objects in the Context
 * NIS_ObjectsIterator anIter (myContext);
 * for (; anIter.More(); anIter.Next())
 *   myContext->Erase (anIter.Value(), Standard_False);
 *
 *    // Set object attributes to their IDs (hardly useful outside the example)
 * anIter.Initialize (myContext);
 * Handle(NIS_InteractiveObject) anObj;
 * while (!(anObj = anIter.Value().IsNull())) {
 *   anObj->SetAttribute (static_cast<void *> (anObj->ID()));
 *   anIter.Next();
 * }
 * @endcode
 */

class NIS_ObjectsIterator 
{
 public:
  // ---------- PUBLIC METHODS ----------


  /**
   * Empty Constructor.
   */
  inline NIS_ObjectsIterator    () {}

  /**
   * Constructor.
   * @param theCtx
   *   Interactive context that is to be iterated for all objects.
   */
  inline NIS_ObjectsIterator    (const Handle_NIS_InteractiveContext& theCtx)
  { Initialize (theCtx); }

  /**
   * Reset the Iterator to start the iterations of objects.
   * @param theCtx
   *   Interactive context that is to be iterated for all objects.
   */
  Standard_EXPORT void          Initialize
                                (const Handle_NIS_InteractiveContext& theCtx);

  /**
   * Query if the Iterator has an object (not yet finished the iteration
   * process).
   */
  inline Standard_Boolean       More    () const
  { return myIter.More(); }

  /**
   * Returns the current object at the iteration pointer. If the iteration is
   * over (More() == False) this method returns NULL Handle.
   */
  inline Handle(NIS_InteractiveObject)
                                Value   () const
  { if (More()) return myIter.Value(); return NULL; }

  /**
   * Step forward to the next valid InteractiveObject instance.
   */
  Standard_EXPORT void          Next    ();

 protected:
  // ---------- PROTECTED FIELDS ----------

  NCollection_Vector <Handle_NIS_InteractiveObject>::Iterator myIter;

};

#endif
