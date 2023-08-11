/*
                                   )
                                  (.)
                                  .|.
                                  | |
                              _.--| |--._
                           .-';  ;`-'& ; `&.
                          \   &  ;    &   &_/
                           |"""---...---"""|
                           \ | | | | | | | /
                            `---.|.|.|.---'

 * This file is generated by bake.lang.c for your convenience. Headers of
 * dependencies will automatically show up in this file. Include bake_config.h
 * in your main project file. Do not edit! */

#ifndef SOKOL_BAKE_CONFIG_H
#define SOKOL_BAKE_CONFIG_H

/* Headers of public dependencies */
/* No dependencies */

/* Convenience macro for exporting symbols */
#ifndef sokol_STATIC
#if defined(sokol_EXPORTS) && (defined(_MSC_VER) || defined(__MINGW32__))
  #define SOKOL_API __declspec(dllexport)
#elif defined(sokol_EXPORTS)
  #define SOKOL_API __attribute__((__visibility__("default")))
#elif defined(_MSC_VER)
  #define SOKOL_API __declspec(dllimport)
#else
  #define SOKOL_API
#endif
#else
  #define SOKOL_API
#endif

#endif
