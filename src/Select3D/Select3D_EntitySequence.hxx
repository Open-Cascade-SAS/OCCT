#ifndef _Select3D_EntitySequence_Header
#define _Select3D_EntitySequence_Header

#include <NCollection_Sequence.hxx>

class Handle(Select3D_SensitiveEntity);

typedef NCollection_Sequence<Handle(Select3D_SensitiveEntity)> Select3D_EntitySequence;
typedef NCollection_Sequence<Handle(Select3D_SensitiveEntity)>::Iterator Select3D_EntitySequenceIter;

#endif // _Select3D_EntitySequence_Header
