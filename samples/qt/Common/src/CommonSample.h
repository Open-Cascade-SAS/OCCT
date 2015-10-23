#ifndef COMMONSAMPLE_H
#define COMMONSAMPLE_H

#ifndef NO_COMMONSAMPLE_EXPORTS
#ifdef COMMONSAMPLE_EXPORTS
#ifdef _WIN32
#define COMMONSAMPLE_EXPORT __declspec( dllexport )
#else
#define COMMONSAMPLE_EXPORT
#endif
#else
#ifdef _WIN32
#define COMMONSAMPLE_EXPORT __declspec( dllimport )
#else
#define COMMONSAMPLE_EXPORT
#endif
#endif
#else
#define COMMONSAMPLE_EXPORT
#endif

#endif
