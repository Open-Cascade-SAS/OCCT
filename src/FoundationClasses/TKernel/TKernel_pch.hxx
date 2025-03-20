#ifndef TKERNEL_PCH_H
#define TKERNEL_PCH_H

// Standard library headers
#include <atomic>
#include <iostream>
#include <limits>
#include <ostream>
#include <random>
#include <sstream>
#include <string>
#include <type_traits>

// Windows-specific headers (for MSVC)
#ifdef _WIN32
  #ifndef NOMINMAX
    #define NOMINMAX
  #endif
  #include <tchar.h>   // For Unicode/MBCS mappings
  #include <windows.h> // For Windows API functions like WideCharToMultiByte
  #ifdef GetObject
    #undef GetObject
  #endif
#endif

// TKernel headers
#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>
#include <Standard_GUID.hxx>
#include <Standard_Handle.hxx>
#include <Standard_Macro.hxx>
#include <Standard_Stream.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>

#include <Precision.hxx>

#include <Quantity_Color.hxx>
#include <Quantity_ColorRGBA.hxx>
#include <Quantity_TypeOfColor.hxx>

#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TCollection_HExtendedString.hxx>

#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_BaseAllocator.hxx>
#include <NCollection_Buffer.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_DefineHArray1.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <NCollection_IndexedMap.hxx>
#include <NCollection_List.hxx>
#include <NCollection_Map.hxx>
#include <NCollection_Sequence.hxx>

#include <Message_ProgressIndicator.hxx>
#include <Message_ProgressRange.hxx>
#include <Message_ProgressScope.hxx>

#include <OSD_Parallel.hxx>
#include <OSD_Path.hxx>

#endif // TKERNEL_PCH_H