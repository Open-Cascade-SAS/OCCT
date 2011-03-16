// File:	Prs3d_NListOfSequenceOfPnt.hxx
// Created:	Thu Apr 20 14:53:27 2006
// Author:      Sergey Kochetkov	


#ifndef Prs3d_NListOfSequenceOfPnt_HeaderFile
#define Prs3d_NListOfSequenceOfPnt_HeaderFile

#include <TColgp_SequenceOfPnt.hxx>
#include <NCollection_DefineList.hxx>
#include <NCollection_DefineBaseCollection.hxx>

DEFINE_BASECOLLECTION(Prs3d_BaseCollListOfSequenceOfPnt,
                      TColgp_SequenceOfPnt)
DEFINE_LIST          (Prs3d_NListOfSequenceOfPnt,
                      Prs3d_BaseCollListOfSequenceOfPnt,
                      TColgp_SequenceOfPnt)

#endif




