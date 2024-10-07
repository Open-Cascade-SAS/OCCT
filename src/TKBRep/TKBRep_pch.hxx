#ifndef TKBRep_HXX
#define TKBRep_HXX

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

#include <TopoDS_Shape.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_CompSolid.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Builder.hxx>
#include <TopoDS_Iterator.hxx>

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>

#include <BRepTools.hxx>

#endif // TKBRep_HXX