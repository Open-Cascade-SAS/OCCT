// File:        NCollection_BaseMap.hxx
// Created:     Thu Apr 18 15:02:53 2002
// Author:      Alexander KARTOMIN (akm)
//              <akm@matrox.nnov.matra-dtv.fr>


#ifndef NCollection_BaseMap_HeaderFile
#define NCollection_BaseMap_HeaderFile

#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <Standard_OStream.hxx>

#include <Standard.hxx>
#include <Standard_Macro.hxx>
#include <NCollection_BaseAllocator.hxx>
#include <NCollection_ListNode.hxx>

//#ifdef WNT
//// Disable the warning "operator new unmatched by delete"
//#pragma warning (disable:4291)
//#endif

typedef void (* NCollection_DelMapNode) 
     (NCollection_ListNode*, Handle(NCollection_BaseAllocator)& theAl);

/**
 * Purpose:     This is a base class for all Maps:
 *                Map
 *                DataMap
 *                DoubleMap
 *                IndexedMap
 *                IndexedDataMap
 *              Provides utilitites for managing the buckets.
 */              

class NCollection_BaseMap 
{
 public:
  // **************************************** Class Iterator ****************
  class Iterator
  {
  protected:
    //! Empty constructor
    Iterator (void) :
      myNbBuckets (0),
      myBuckets   (NULL),
      myBucket    (0),
      myNode      (NULL) {}
    
    //! Constructor
    Iterator (const NCollection_BaseMap& theMap) :
      myNbBuckets (theMap.myNbBuckets),
      myBuckets   (theMap.myData1),
      myBucket    (-1),
      myNode      (NULL)
    {
      if (!myBuckets) 
        myNbBuckets = -1;
      else
        do {
          myBucket++;
          if (myBucket > myNbBuckets) 
            return;
          myNode = myBuckets[myBucket];
        } while (!myNode);
    }

  public:
    //! Initialize
    void Initialize (const NCollection_BaseMap& theMap)
    {
      myNbBuckets = theMap.myNbBuckets;
      myBuckets = theMap.myData1;
      myBucket = -1;
      myNode = NULL;
      if (!myBuckets) 
        myNbBuckets = -1;
      PNext();
    }

    //! Reset
    void Reset (void)
    {
      myBucket = -1;
      myNode = NULL;
      PNext();
    }

  protected:
    //! PMore
    Standard_Boolean PMore (void) const
    { return (myNode != NULL); }
    
    //! PNext
    void PNext (void)
    {
      if (!myBuckets) 
        return; 
      if (myNode) 
      {
        myNode = myNode->Next();
        if (myNode) 
          return;
      }
      while (!myNode) 
      {
        myBucket++;
        if (myBucket > myNbBuckets) 
          return;
        myNode = myBuckets[myBucket];
      }
    }

  protected:
    // ---------- PRIVATE FIELDS ------------
    Standard_Integer       myNbBuckets; //!< Total buckets in the map
    NCollection_ListNode **myBuckets;   //!< Location in memory 
    Standard_Integer       myBucket;    //!< Current bucket
    NCollection_ListNode * myNode;      //!< Current node
  };

 public:
  // ---------- PUBLIC METHODS ------------

  //! NbBuckets
  Standard_Integer NbBuckets() const
  { return myNbBuckets; }

  //! Extent
  Standard_Integer Extent() const
  { return mySize; }

  //! IsEmpty
  Standard_Boolean IsEmpty() const
  { return mySize == 0; }

  //! Statistics
  Standard_EXPORT void Statistics(Standard_OStream& S) const;

 protected:
  // -------- PROTECTED METHODS -----------

  //! Constructor
  NCollection_BaseMap (const Standard_Integer NbBuckets,
                       const Standard_Boolean single) :
                         myData1(NULL),
                         myData2(NULL),
                         isDouble(!single),
                         mySaturated(Standard_False),
                         myNbBuckets(NbBuckets),
                         mySize(0) {}

  //! BeginResize
  Standard_EXPORT Standard_Boolean BeginResize 
    (const Standard_Integer  NbBuckets,
     Standard_Integer&       NewBuckets,
     NCollection_ListNode**& data1,
     NCollection_ListNode**& data2,
     Handle(NCollection_BaseAllocator)& theAllocator) const;

  //! EndResize
  Standard_EXPORT void EndResize 
    (const Standard_Integer NbBuckets,
     const Standard_Integer NewBuckets,
     NCollection_ListNode** data1,
     NCollection_ListNode** data2,
     Handle(NCollection_BaseAllocator)& theAllocator);

  //! Resizable
  Standard_Boolean Resizable() const
  { return IsEmpty() || (!mySaturated && (mySize > myNbBuckets)); }

  //! Increment
  void Increment()
  { mySize++; }

  //! Decrement
  void Decrement() 
  { mySize--; }

  //! Destroy
  Standard_EXPORT void Destroy(NCollection_DelMapNode fDel,
                               Handle(NCollection_BaseAllocator)& theAllocator,
                               const Standard_Boolean doReleaseMemory
                                 = Standard_True);

  //! NextPrimeForMap
  Standard_EXPORT Standard_Integer NextPrimeForMap
    (const Standard_Integer N) const;

 protected:
  // --------- PROTECTED FIELDS -----------
  NCollection_ListNode ** myData1;
  NCollection_ListNode ** myData2;

 private: 
  // ---------- PRIVATE FIELDS ------------
  Standard_Boolean isDouble;
  Standard_Boolean mySaturated;
  Standard_Integer myNbBuckets;
  Standard_Integer mySize;

  // ---------- FRIEND CLASSES ------------
  friend class Iterator;

};

#endif
