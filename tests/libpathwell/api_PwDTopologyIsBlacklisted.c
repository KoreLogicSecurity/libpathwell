/*-
 ***********************************************************************
 *
 * $Id: api_PwDTopologyIsBlacklisted.c,v 1.6 2017/04/20 13:23:39 klm Exp $
 *
 ***********************************************************************
 *
 * Copyright 2013-2017 The PathWell Project, All Rights Reserved.
 *
 * This software, having been partly or wholly developed and/or
 * sponsored by KoreLogic, Inc., is hereby released under the terms
 * and conditions set forth in the project's "README.LICENSE" file.
 * For a list of all contributors and sponsors, please refer to the
 * project's "README.CREDITS" file.
 *
 ***********************************************************************
 */
#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif
#include <pathwell.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <tap.h>

typedef struct _TEST_TUPLES
{
  char acTopology[PATHWELL_MAX_TOPOLOGY_SIZE];
  int iBlacklisted;
  char acError[256];
} TEST_TUPLES;

int
main(int iArgumentCount, char *ppcArgumentVector[])
{
  char acDescription[1024] = "";
  int iBlacklisted = 0;
  int iError = 0;
  int iIndex = 0;
  PW_D_CONTEXT *psPwDContext = NULL;
  TEST_TUPLES asTuples[] =
  {
    { "undef",                  -1, "Topology argument is not defined. That should not happen." },
    { "?z",                      0, "" },
    { "?d",                      0, "" },
    { "?l",                      0, "" },
    { "?u",                      0, "" },
    { "?s",                      0, "" },
    { "?f",                      0, "" },
    { "?x",                      0, "" },
    { "?H",                      0, "" },
    { "?d?l?u?s",                0, "" },
    { "?d?l?u?s?f",              0, "" },
    { "?d?l?u?s?f?x",            0, "" },
    { "?d?l?u?s?f?x?H",          0, "" },
    { "?d?d?d?d?d?d?d?d",        1, "" },
    { "?d?d?d?d?d?d?d?d?d",      1, "" },
    { "?d?d?d?d?d?d?d?d?s?s?u",  1, "" },
    { "?d?d?d?d?d?d?d?d?u?l",    1, "" },
    { "?d?d?d?d?d?d?d?l",        1, "" },
    { "?d?d?d?d?d?d?d?u?l",      1, "" },
    { "?d?d?d?d?d?d?l?l",        1, "" },
    { "?d?d?d?d?d?d?u?l",        1, "" },
    { "?d?d?d?d?d?d?u?l?l",      1, "" },
    { "?d?d?d?d?d?u?l?l?l",      1, "" },
  };

  plan_tests(28);

  psPwDContext = PwDNewContextFromParameters(":memory:", SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL, 0);
  ok(PwDContextIsValid(psPwDContext), "%s", "new context");
  if (!PwDContextIsValid(psPwDContext))
  {
    diag("%s", PwDGetError(psPwDContext));
  }

  iError = PwDAcquireConnection(psPwDContext);
  ok(iError == 0, "%s", "acquire connection");
  if (iError != ER_OK)
  {
    diag("%s", PwDGetError(psPwDContext));
  }

  iError = PwDCreateSchema(psPwDContext);
  ok(iError == 0, "%s", "create schema");
  if (iError != ER_OK)
  {
    diag("%s", PwDGetError(psPwDContext));
  }

  iError = PwDLoadBlacklistedTopologies(psPwDContext, ppcArgumentVector[1]);
  ok(iError == 0, "%s", "load blacklisted topologies");
  if (iError != ER_OK)
  {
    diag("%s", PwDGetError(psPwDContext));
  }

  for (iIndex = 0; iIndex < (sizeof(asTuples) / sizeof(asTuples[0])); iIndex++)
  {
    iBlacklisted = PwDTopologyIsBlacklisted
    (
      psPwDContext,
      (strcmp(asTuples[iIndex].acTopology, "undef") == 0) ? NULL : asTuples[iIndex].acTopology
    );
    if (iBlacklisted == PATHWELL_TRUE || iBlacklisted == PATHWELL_FALSE)
    {
      snprintf
      (
        acDescription,
        1024,
        "\"%s\", %d, %d",
        asTuples[iIndex].acTopology,
        asTuples[iIndex].iBlacklisted,
        iBlacklisted
      );
      ok(iBlacklisted == asTuples[iIndex].iBlacklisted, "%s", acDescription);
    }
    else
    {
      snprintf
      (
        acDescription,
        1024,
        "\"%s\", %d, %d, \"%s\", \"%s\"",
        asTuples[iIndex].acTopology,
        asTuples[iIndex].iBlacklisted,
        iBlacklisted,
        asTuples[iIndex].acError,
        PwDGetError(psPwDContext)
      );
      ok(strcmp(PwDGetError(psPwDContext), asTuples[iIndex].acError) == 0, "%s", acDescription);
    }
  }

  iError = PwDReleaseConnection(psPwDContext);
  ok(iError == 0, "%s", "release connection");
  if (iError != ER_OK)
  {
    diag("%s", PwDGetError(psPwDContext));
  }

  PwDFreeContext(&psPwDContext);

  return exit_status();
}
