/*-
 ***********************************************************************
 *
 * $Id: api_PwSDirname.c,v 1.1 2013/10/29 23:13:38 klm Exp $
 *
 ***********************************************************************
 *
 * Copyright 2013-2013 The PathWell Project, All Rights Reserved.
 *
 * This software, having been partly or wholly developed and/or
 * sponsored by KoreLogic, Inc., is hereby released under the terms
 * and conditions set forth in the project's "README.LICENSE" file.
 * For a list of all contributors and sponsors, please refer to the
 * project's "README.CREDITS" file.
 *
 ***********************************************************************
 */
#include <errno.h>
#include <inttypes.h>
#include <pathwell.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <tap.h>

typedef struct _TEST_TUPLES
{
  char acInput[256];
  char acOutput[256];
} TEST_TUPLES;

int
main(int iArgumentCount, char *ppcArgumentVector[])
{
  char acDescription[1024] = "";
  char *pcDirectory = NULL;
  int iIndex = 0;
  TEST_TUPLES asTuples[] =
  {
    { "undef", "undef" },
    { "",          "." },

    { "/",         "/" },
    { "//",        "/" },
    { "///",       "/" },

    { ".",         "." },
    { "./",        "." },
    { ".//",       "." },
    { ".///",      "." },

    { "..",        "." },
    { "../",       "." },
    { "..//",      "." },
    { "..///",     "." },

    { "...",       "." },
    { ".../",      "." },
    { "...//",     "." },
    { "...///",    "." },

    { "a",         "." },
    { "a/",        "." },
    { "a//",       "." },
    { "a///",      "." },

    { "ab",        "." },
    { "ab/",       "." },
    { "ab//",      "." },
    { "ab///",     "." },

    { "abc",       "." },
    { "abc/",      "." },
    { "abc//",     "." },
    { "abc///",    "." },

    { "/a",        "/" },
    { "/a/",       "/" },
    { "/a//",      "/" },
    { "/a///",     "/" },

    { "//a",       "/" },
    { "//a/",      "/" },
    { "//a//",     "/" },
    { "//a///",    "/" },

    { "/a/1",      "/a" },
    { "/a/1/",     "/a" },
    { "/a/1//",    "/a" },
    { "/a/1///",   "/a" },

    { "//a/1",     "//a" },
    { "//a/1/",    "//a" },
    { "//a/1//",   "//a" },
    { "//a/1///",  "//a" },

    { "//a//1",    "//a" },
    { "//a//1/",   "//a" },
    { "//a//1//",  "//a" },
    { "//a//1///", "//a" },

    { "/abc/123",      "/abc" },
    { "/abc/123/",     "/abc" },
    { "/abc/123//",    "/abc" },
    { "/abc/123///",   "/abc" },

    { "//abc/123",     "//abc" },
    { "//abc/123/",    "//abc" },
    { "//abc/123//",   "//abc" },
    { "//abc/123///",  "//abc" },

    { "//abc//123",    "//abc" },
    { "//abc//123/",   "//abc" },
    { "//abc//123//",  "//abc" },
    { "//abc//123///", "//abc" },
  };

  plan_tests(61);

  for (iIndex = 0; iIndex < (sizeof(asTuples) / sizeof(asTuples[0])); iIndex++)
  {
    pcDirectory = PwSDirname
    (
      (strcmp(asTuples[iIndex].acInput, "undef") ? asTuples[iIndex].acInput : NULL)
    );
    if (pcDirectory != NULL)
    {
      snprintf
      (
        acDescription,
        1024,
        "Input: \"%s\"\tExpected Output: \"%s\"\tActual Output: \"%s\"",
        asTuples[iIndex].acInput,
        asTuples[iIndex].acOutput,
        pcDirectory
      );
      ok(strcmp(asTuples[iIndex].acOutput, pcDirectory) == 0, acDescription);
      free(pcDirectory);
    }
    else
    {
      snprintf
      (
        acDescription,
        1024,
        "Input: \"%s\"\tExpected Output: \"%s\"\tActual Output: \"%s\"",
        asTuples[iIndex].acInput,
        asTuples[iIndex].acOutput,
        (pcDirectory == NULL) ? "undef" : pcDirectory
      );
      ok(pcDirectory == NULL && strcmp(asTuples[iIndex].acOutput, "undef") == 0, acDescription);
    }
  }

  return exit_status();
}
