// Created on: 2011-02-03
// Created by: Mikhail SAZONOV
// Copyright (c) 2011-2014 OPEN CASCADE SAS
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

#ifndef _OSD_MAllocHook_HeaderFile
#define _OSD_MAllocHook_HeaderFile

#include <Standard_TypeDef.hxx>
#include <Standard_Mutex.hxx>
#include <stdio.h>
#include <fstream>

/**
 * This class provides the possibility to set callback for memory
 * allocation/deallocation.
 * On MS Windows, it works only in Debug builds. It relies on the
 * debug CRT function _CrtSetAllocHook (see MSDN for help).
 */
class OSD_MAllocHook
{
public:
  /**
   * Interface of a class that should handle allocation/deallocation events
   */
  class Callback
  {
  public:
    //! Allocation event handler
    /**
     * It is called when allocation is done
     * @param theSize
     *   the size of the memory block in bytes
     * @param theRequestNum
     *   the allocation order number of the memory block
     */
    virtual void AllocEvent
                   (size_t      theSize,
                    long        theRequestNum) = 0;

    //! Freeing event handler
    /**
     * It is called when the block is freed
     * @param theData
     *   the pointer to the user data section of the memory block
     * @param theSize
     *   the size of the memory block in bytes
     * @param theRequestNum
     *   the allocation order number of the memory block
     */
    virtual void FreeEvent
                   (void*       theData,
                    size_t      theSize,
                    long        theRequestNum) = 0;
  };

  /**
   * Implementation of the handler that collects all events
   * to the log file. It contains the method to generate the report
   * from the log file.
   */
  class LogFileHandler: public Callback
  {
  public:
    //! Constructor
    Standard_EXPORT LogFileHandler();

    //! Destructor
    Standard_EXPORT ~LogFileHandler();

    //! Create the file and start collecting events.
    //! Return false if the file with the given name cannot be created.
    Standard_EXPORT Standard_Boolean Open(const char* theFileName);

    //! Close the file and stop collecting events
    Standard_EXPORT void Close();

    //! Make synthesized report on the given log file.
    /**
     * Generate an easy to use report in the
     * new file with the given name, taking the given log file as input.
     * If theIncludeAlive is true then
     * include into the report the alive allocation numbers.
     */
    Standard_EXPORT static Standard_Boolean MakeReport
                   (const char* theLogFile,
                    const char* theOutFile,
                    const Standard_Boolean theIncludeAlive = Standard_False);

    Standard_EXPORT virtual void AllocEvent(size_t, long);
    Standard_EXPORT virtual void FreeEvent(void*, size_t, long);

  private:
    std::ofstream  myLogFile;
    Standard_Mutex myMutex;
    size_t         myBreakSize;
  };

  /**
   * Implementation of the handler that collects numbers of
   * allocations/deallocations for each block size directly in the memory.
   */
  class CollectBySize: public Callback
  {
  public:
    //! Constructor
    Standard_EXPORT CollectBySize();

    //! Destructor
    Standard_EXPORT ~CollectBySize();

    //! Reset the buffer and start collecting events.
    Standard_EXPORT void Reset();

    //! Write report in the given file.
    Standard_EXPORT Standard_Boolean MakeReport(const char* theOutFile);

    Standard_EXPORT virtual void AllocEvent(size_t, long);
    Standard_EXPORT virtual void FreeEvent(void*, size_t, long);

  public:
    struct Numbers
    {
      int nbAlloc;
      int nbFree;
      int nbLeftPeak;
      Numbers() : nbAlloc(0), nbFree(0), nbLeftPeak(0) {}
    };
    
    static const size_t myMaxAllocSize; //!< maximum tracked size

    Standard_Mutex myMutex;             //!< used for thread-safe access
    Numbers*       myArray;             //!< indexed from 0 to myMaxAllocSize-1
    ptrdiff_t      myTotalLeftSize;     //!< currently remained allocated size
    size_t         myTotalPeakSize;     //!< maxium cumulative allocated size
    size_t         myBreakSize;         //!< user defined allocation size to debug (see place_for_breakpoint())
    size_t         myBreakPeak;         //!< user defined peak size limit to debug
  };

  //! Set handler of allocation/deallocation events
  /**
   * You can pass here any implementation. For easy start, you can try
   * with the predefined handler LogFileHandler, which static instance 
   * is returned by GetLogFileHandler().
   * To clear the handler, pass NULL here.
   */
  Standard_EXPORT static void SetCallback
                   (Callback* theCB);

  //! Get current handler of allocation/deallocation events
  Standard_EXPORT static Callback* GetCallback();

  //! Get static instance of LogFileHandler handler
  Standard_EXPORT static LogFileHandler* GetLogFileHandler();

  //! Get static instance of CollectBySize handler
  Standard_EXPORT static CollectBySize* GetCollectBySize();
};

#endif /* _OSD_MAllocHook_HeaderFile */
