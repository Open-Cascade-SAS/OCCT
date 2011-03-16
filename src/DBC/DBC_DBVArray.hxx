#ifndef _DBC_DBVArray_HeaderFile
#define _DBC_DBVArray_HeaderFile

#ifdef OBJY
#ifndef _PStandard_ArrayNode_HeaderFile
#include <PStandard_ArrayNode.hxx>
#endif
declare(ooVArray,PHandle_PStandard_ArrayNode)
typedef ooVArray(PHandle_PStandard_ArrayNode) DBC_DBVArray;
#endif

#ifdef CSFDB
#include <PStandard_ArrayNode.hxx>
//typedef Handle_PStandard_ArrayNode* DBC_DBVArray;
typedef void* DBC_DBVArray;
#endif

#ifdef OBJS
#include <PStandard_ArrayNode.hxx>
typedef PStandard_ArrayNode* DBC_DBVArray;
#endif

#endif
