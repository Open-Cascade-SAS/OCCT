#include <TCollection_AsciiString.hxx>

static TCollection_AsciiString XSDRAW_CommandPart
  (Standard_Integer argc, const char** argv, const Standard_Integer argf)
{
  TCollection_AsciiString res;
  for (Standard_Integer i = argf; i < argc; i ++) {
    if (i > argf) res.AssignCat(" ");
    res.AssignCat (argv[i]);
  }
  return res;
}
