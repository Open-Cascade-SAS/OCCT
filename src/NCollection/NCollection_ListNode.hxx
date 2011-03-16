// File:        NCollection_ListNode.hxx
// Created:     Wed Apr 17 10:45:17 2002
// Author:      Alexander KARTOMIN (akm)
//              <akm@matrox.nnov.matra-dtv.fr>

#ifndef NCollection_ListNode_HeaderFile
#define NCollection_ListNode_HeaderFile

#include <NCollection_BaseAllocator.hxx>

#ifdef WNT
// Disable the warning "operator new unmatched by delete"
#pragma warning (disable:4291)
#endif

/**
 * Purpose:     This class is used to  represent a node  in the BaseList and
 *              BaseMap. 
 */              
class NCollection_ListNode
{
 public:
  //! The only constructor
  NCollection_ListNode (NCollection_ListNode* theNext)
  { myNext = theNext; }

  //! Next pointer access
  NCollection_ListNode*& Next (void)
  { return myNext; }

  //! Next pointer const access
  NCollection_ListNode* Next (void) const
  { return myNext; }

 private:
  //! operator= - forbidden
  NCollection_ListNode& operator=(const NCollection_ListNode& )
  {return *this;}
  //! copy constructor - forbidden
  NCollection_ListNode (const NCollection_ListNode& ) {}

 private:
  NCollection_ListNode * myNext; //!< Pointer to the next node
};

#endif
