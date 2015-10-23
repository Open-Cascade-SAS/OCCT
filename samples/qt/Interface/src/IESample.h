#ifndef IESAMPLE_H
#define IESAMPLE_H

#ifndef NO_IESAMPLE_EXPORTS
#ifdef IESAMPLE_EXPORTS
#ifdef _WIN32
#define IESAMPLE_EXPORT __declspec( dllexport )
#else
#define IESAMPLE_EXPORT
#endif
#else
#ifdef _WIN32
#define IESAMPLE_EXPORT __declspec( dllimport )
#else
#define IESAMPLE_EXPORT
#endif
#endif
#else
#define IESAMPLE_EXPORT
#endif

#endif
