/*-
 ***********************************************************************
 *
 * $Id: api_PwLCalcLevDistance.c,v 1.13.2.4 2015/09/30 17:54:50 klm Exp $
 *
 ***********************************************************************
 *
 * Copyright 2013-2015 The PathWell Project, All Rights Reserved.
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
#include <stdio.h>
#include <string.h>
#include <tap.h>

typedef struct _TEST_TUPLES
{
  char acTypeA[16];
  char acTypeB[16];
  int iTokenSetA;
  int iTokenSetB;
  char acTopologyA[PATHWELL_MAX_TOPOLOGY_SIZE];
  char acTopologyB[PATHWELL_MAX_TOPOLOGY_SIZE];
  int iDistance;
  char acError[256];
} TEST_TUPLES;

int
main(int iArgumentCount, char *ppcArgumentVector[])
{
  char acDescription[1024] = "";
  int iDistance = 0;
  int iError = 0;
  int iIndex = 0;
  PW_L_CONTEXT *psPwLContext = NULL;
  PW_T_CONTEXT *psPwTContextA = NULL;
  PW_T_CONTEXT *psPwTContextB = NULL;
  TEST_TUPLES asTuples[] =
  {
    { "baseN+1", "baseN+1", 1, 1, "undef", "undef", -1, "Topology A is not defined. That should not happen." },
    { "baseN+1", "baseN+1", 1, 1, "undef", "", -1, "Topology A is not defined. That should not happen." },
    { "baseN+1", "baseN+1", 1, 1, "", "undef", -1, "Topology B is not defined. That should not happen." },
//FIXME Perhaps this should fail since an empty string is not really a valid topology.
    { "baseN+1", "baseN+1", 1, 1, "", "", 0, "" },
    { "bitmask", "bitmask", 1, 1, "undef", "undef", -1, "Topology A is not defined. That should not happen." },
    { "bitmask", "bitmask", 1, 1, "undef", "", -1, "Topology A is not defined. That should not happen." },
    { "bitmask", "bitmask", 1, 1, "", "undef", -1, "Topology B is not defined. That should not happen." },
//FIXME Perhaps this should fail since an empty string is not really a valid topology.
    { "bitmask", "bitmask", 1, 1, "", "", 0, "" },

    { "baseN+1", "baseN+1", 1, 1, "?z", "?z", 0, "" },
    { "baseN+1", "baseN+1", 2, 2, "?z", "?z", 0, "" },
    { "baseN+1", "baseN+1", 3, 3, "?z", "?z", 0, "" },
    { "baseN+1", "baseN+1", 4, 4, "?z", "?z", 0, "" },
    { "bitmask", "bitmask", 1, 1, "?z", "?z", 0, "" },
    { "bitmask", "bitmask", 2, 2, "?z", "?z", 0, "" },
    { "bitmask", "bitmask", 3, 3, "?z", "?z", 0, "" },
    { "bitmask", "baseN+1", 4, 4, "?z", "?z", 0, "" },

    { "baseN+1", "baseN+1", 1, 1, "?z", "?d", 1, "" },
    { "baseN+1", "baseN+1", 2, 2, "?z", "?d", 1, "" },
    { "baseN+1", "baseN+1", 3, 3, "?z", "?d", 1, "" },
    { "baseN+1", "baseN+1", 4, 4, "?z", "?d", 1, "" },
    { "baseN+1", "baseN+1", 1, 1, "?d", "?z", 1, "" },
    { "baseN+1", "baseN+1", 2, 2, "?d", "?z", 1, "" },
    { "baseN+1", "baseN+1", 3, 3, "?d", "?z", 1, "" },
    { "baseN+1", "baseN+1", 4, 4, "?d", "?z", 1, "" },
    { "bitmask", "bitmask", 1, 1, "?z", "?d", 1, "" },
    { "bitmask", "bitmask", 2, 2, "?z", "?d", 1, "" },
    { "bitmask", "bitmask", 3, 3, "?z", "?d", 1, "" },
    { "bitmask", "bitmask", 4, 4, "?z", "?d", 1, "" },
    { "bitmask", "bitmask", 1, 1, "?d", "?z", 1, "" },
    { "bitmask", "bitmask", 2, 2, "?d", "?z", 1, "" },
    { "bitmask", "bitmask", 3, 3, "?d", "?z", 1, "" },
    { "bitmask", "bitmask", 4, 4, "?d", "?z", 1, "" },

    { "baseN+1", "baseN+1", 1, 1, "?z", "?d?d", 2, "" },
    { "baseN+1", "baseN+1", 2, 2, "?z", "?d?d", 2, "" },
    { "baseN+1", "baseN+1", 3, 3, "?z", "?d?d", 2, "" },
    { "baseN+1", "baseN+1", 4, 4, "?z", "?d?d", 2, "" },
    { "baseN+1", "baseN+1", 1, 1, "?d?d", "?z", 2, "" },
    { "baseN+1", "baseN+1", 2, 2, "?d?d", "?z", 2, "" },
    { "baseN+1", "baseN+1", 3, 3, "?d?d", "?z", 2, "" },
    { "baseN+1", "baseN+1", 4, 4, "?d?d", "?z", 2, "" },
    { "bitmask", "bitmask", 1, 1, "?z", "?d?d", 2, "" },
    { "bitmask", "bitmask", 2, 2, "?z", "?d?d", 2, "" },
    { "bitmask", "bitmask", 3, 3, "?z", "?d?d", 2, "" },
    { "bitmask", "bitmask", 4, 4, "?z", "?d?d", 2, "" },
    { "bitmask", "bitmask", 1, 1, "?d?d", "?z", 2, "" },
    { "bitmask", "bitmask", 2, 2, "?d?d", "?z", 2, "" },
    { "bitmask", "bitmask", 3, 3, "?d?d", "?z", 2, "" },
    { "bitmask", "bitmask", 4, 4, "?d?d", "?z", 2, "" },

    { "baseN+1", "baseN+1", 1, 1, "?u", "?u", 0, "" },
    { "baseN+1", "baseN+1", 1, 1, "?u", "?s", 1, "" },
    { "baseN+1", "baseN+1", 1, 1, "?l", "?s", 1, "" },
    { "baseN+1", "baseN+1", 1, 1, "?l?l?d", "?l?l?d", 0, "" },
    { "baseN+1", "baseN+1", 1, 1, "?l?l?d", "?l?u?l", 2, "" },
    { "baseN+1", "baseN+1", 1, 1, "?s?u?u", "?u?s?u", 2, "" },
    { "baseN+1", "baseN+1", 1, 1, "?s?l?d?d", "?s?u?l?d", 2, "" },
    { "baseN+1", "baseN+1", 1, 1, "?d?d?l?u?s?l", "?s?s?s?s?s?s?s", 6, "" },
    { "baseN+1", "baseN+1", 1, 1, "?d?s?s?u?l?s?d", "?s?s?s?s?s?s?s", 4, "" },
    { "baseN+1", "baseN+1", 1, 1, "?d?d?d?d?d?d?d?d?d?d?d?d?d?d", "?s?s?s?s?s?s?s?s?s?s?s?s?s?s", 14, "" },
    { "baseN+1", "baseN+1", 1, 1, "?d?d?d?d?d?d?d?d?d?d?d?d?d?d", "?l?l?l?l?l?l?l?l?l?l?l?l?l?l", 14, "" },
    { "baseN+1", "baseN+1", 1, 1, "?s?s?s?d?l?u?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s", "?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s", 3, "" },
    { "baseN+1", "baseN+1", 1, 1, "?z", "?s?s?s?s?s?s?s?s?s?s?s?s?s?s", 14, "" },
    { "baseN+1", "baseN+1", 1, 1, "?z", "?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s", 27, "" },
    { "bitmask", "bitmask", 1, 1, "?u", "?u", 0, "" },
    { "bitmask", "bitmask", 1, 1, "?u", "?s", 1, "" },
    { "bitmask", "bitmask", 1, 1, "?l", "?s", 1, "" },
    { "bitmask", "bitmask", 1, 1, "?l?l?d", "?l?l?d", 0, "" },
    { "bitmask", "bitmask", 1, 1, "?l?l?d", "?l?u?l", 2, "" },
    { "bitmask", "bitmask", 1, 1, "?s?u?u", "?u?s?u", 2, "" },
    { "bitmask", "bitmask", 1, 1, "?s?l?d?d", "?s?u?l?d", 2, "" },
    { "bitmask", "bitmask", 1, 1, "?d?d?l?u?s?l", "?s?s?s?s?s?s?s", 6, "" },
    { "bitmask", "bitmask", 1, 1, "?d?s?s?u?l?s?d", "?s?s?s?s?s?s?s", 4, "" },
    { "bitmask", "bitmask", 1, 1, "?d?d?d?d?d?d?d?d?d?d?d?d?d?d", "?s?s?s?s?s?s?s?s?s?s?s?s?s?s", 14, "" },
    { "bitmask", "bitmask", 1, 1, "?d?d?d?d?d?d?d?d?d?d?d?d?d?d", "?l?l?l?l?l?l?l?l?l?l?l?l?l?l", 14, "" },
    { "bitmask", "bitmask", 1, 1, "?s?s?s?d?l?u?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s", "?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s", 3, "" },
    { "bitmask", "bitmask", 1, 1, "?z", "?s?s?s?s?s?s?s?s?s?s?s?s?s?s", 14, "" },
    { "bitmask", "bitmask", 1, 1, "?z", "?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s", 27, "" },

    { "baseN+1", "baseN+1", 3, 3, "?d?s?f?u?l?s", "?d?f?f?f?l?l", 3, "" },
    { "bitmask", "bitmask", 3, 3, "?d?s?f?u?l?s", "?d?f?f?f?l?l", 3, "" },
  };

  plan_tests(82);

  psPwTContextA = PwTNewContext();
//ok(psPwTContextA != NULL, "%s", "new PwT context A");
  psPwTContextB = PwTNewContext();
//ok(psPwTContextB != NULL, "%s", "new PwT context B");
  psPwLContext = PwLNewContextFromParameters(psPwTContextA, psPwTContextB);
//ok(psPwLContext != NULL, "%s", "new PwL context");

  iError = PwLCheckLevDistance(NULL, 0);
  ok(iError == ER, "%s", "Null PwL context handling");
  psPwLContext->psPwTContextA = NULL;
  psPwLContext->psPwTContextB = NULL;
  iError = PwLCheckLevDistance(psPwLContext, 0);
  ok(strcmp(PwLGetError(psPwLContext), "Invalid member context. That should not happen.") == 0, "%s", "Null PwT context handling (A+B)");
  psPwLContext->psPwTContextA = NULL;
  psPwLContext->psPwTContextB = psPwTContextB;
  iError = PwLCheckLevDistance(psPwLContext, 0);
  ok(strcmp(PwLGetError(psPwLContext), "Invalid member context. That should not happen.") == 0, "%s", "Null PwT context handling (A)");
  psPwLContext->psPwTContextA = psPwTContextA;
  psPwLContext->psPwTContextB = NULL;
  iError = PwLCheckLevDistance(psPwLContext, 0);
  ok(strcmp(PwLGetError(psPwLContext), "Invalid member context. That should not happen.") == 0, "%s", "Null PwT context handling (B)");
  psPwLContext->psPwTContextA = psPwTContextA;
  psPwLContext->psPwTContextB = psPwTContextB;

  for (iIndex = 0; iIndex < (sizeof(asTuples) / sizeof(asTuples[0])); iIndex++)
  {
    iError = PwTSetEncoding(psPwTContextA, (strcmp(asTuples[iIndex].acTypeA, "bitmask") == 0) ? PATHWELL_ENCODING_BITMASK : PATHWELL_ENCODING_BASENP1);
//  ok(iError == 0, "%s", "set encoding A");
    iError = PwTSetEncoding(psPwTContextB, (strcmp(asTuples[iIndex].acTypeB, "bitmask") == 0) ? PATHWELL_ENCODING_BITMASK : PATHWELL_ENCODING_BASENP1);
//  ok(iError == 0, "%s", "set encoding B");
    iError = PwTSetTokenSet(psPwTContextA, asTuples[iIndex].iTokenSetA);
//  ok(iError == 0, "%s", "set token set A");
    iError = PwTSetTokenSet(psPwTContextB, asTuples[iIndex].iTokenSetB);
//  ok(iError == 0, "%s", "set token set B");
    iError = PwTSetTopology(psPwTContextA, (strcmp(asTuples[iIndex].acTopologyA, "undef") == 0) ? NULL : asTuples[iIndex].acTopologyA);
//  ok(iError == 0, "%s", "set topology A");
    iError = PwTSetTopology(psPwTContextB, (strcmp(asTuples[iIndex].acTopologyB, "undef") == 0) ? NULL : asTuples[iIndex].acTopologyB);
//  ok(iError == 0, "%s", "set topology B");

    iDistance = PwLCalcLevDistance(psPwLContext);
    if (iDistance >= 0)
    {
      snprintf
      (
        acDescription,
        1024,
        "\"%s\", \"%s\", %d, %d, \"%s\", \"%s\", %d, %d",
        asTuples[iIndex].acTypeA,
        asTuples[iIndex].acTypeB,
        asTuples[iIndex].iTokenSetA,
        asTuples[iIndex].iTokenSetB,
        asTuples[iIndex].acTopologyA,
        asTuples[iIndex].acTopologyB,
        asTuples[iIndex].iDistance,
        iDistance
      );
      ok(iDistance == asTuples[iIndex].iDistance, "%s", acDescription);
    }
    else
    {
      snprintf
      (
        acDescription,
        1024,
        "\"%s\", \"%s\", %d, %d, \"%s\", \"%s\", %d, %d, \"%s\", \"%s\"",
        asTuples[iIndex].acTypeA,
        asTuples[iIndex].acTypeB,
        asTuples[iIndex].iTokenSetA,
        asTuples[iIndex].iTokenSetB,
        asTuples[iIndex].acTopologyA,
        asTuples[iIndex].acTopologyB,
        asTuples[iIndex].iDistance,
        iDistance,
        asTuples[iIndex].acError,
        PwLGetError(psPwLContext)
      );
      ok(strcmp(PwLGetError(psPwLContext), asTuples[iIndex].acError) == 0, "%s", acDescription);
    }
  }

  PwLFreeContext(psPwLContext);
  PwTFreeContext(psPwTContextA);
  PwTFreeContext(psPwTContextB);

  return exit_status();
}
