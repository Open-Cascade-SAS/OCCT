#ifndef TKMATH_PCH_H
#define TKMATH_PCH_H

// Standard library headers
#include <type_traits>

// Windows-specific headers (for MSVC)
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>        // For Windows API functions like WideCharToMultiByte
#include <tchar.h>          // For Unicode/MBCS mappings
#ifdef GetObject
#undef GetObject
#endif
#endif

#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Dir.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Trsf.hxx>
#include <gp_Quaternion.hxx>

#include <TopLoc_Location.hxx>

#endif // TKMATH_PCH_H