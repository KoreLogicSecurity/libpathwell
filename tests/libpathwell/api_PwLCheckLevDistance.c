/*-
 ***********************************************************************
 *
 * $Id: api_PwLCheckLevDistance.c,v 1.6.2.4 2015/09/30 17:54:50 klm Exp $
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
  int iAccepted;
  char acError[256];
} TEST_TUPLES;

int
main(int iArgumentCount, char *ppcArgumentVector[])
{
  char acDescription[1024] = "";
  int iAccepted = 0;
  int iError = 0;
  int iIndex = 0;
  PW_L_CONTEXT *psPwLContext = NULL;
  PW_T_CONTEXT *psPwTContextA = NULL;
  PW_T_CONTEXT *psPwTContextB = NULL;
  TEST_TUPLES asTuples[] =
  {
    { "baseN+1", "baseN+1", 1, 1, "undef", "undef", 0, -1, "Topology A is not defined. That should not happen." },
    { "baseN+1", "baseN+1", 1, 1, "undef", "", 0, -1, "Topology A is not defined. That should not happen." },
    { "baseN+1", "baseN+1", 1, 1, "", "undef", 0, -1, "Topology B is not defined. That should not happen." },
//FIXME Perhaps this should fail since an empty string is not really a valid topology.
    { "baseN+1", "baseN+1", 1, 1, "", "", 0, 1, "" },
    { "bitmask", "bitmask", 1, 1, "undef", "undef", 0, -1, "Topology A is not defined. That should not happen." },
    { "bitmask", "bitmask", 1, 1, "undef", "", 0, -1, "Topology A is not defined. That should not happen." },
    { "bitmask", "bitmask", 1, 1, "", "undef", 0, -1, "Topology B is not defined. That should not happen." },
//FIXME Perhaps this should fail since an empty string is not really a valid topology.
    { "bitmask", "bitmask", 1, 1, "", "", 0, 1, "" },

    { "baseN+1", "baseN+1", 1, 1, "?z", "?z", 3, 0, "" },
    { "baseN+1", "baseN+1", 2, 2, "?z", "?z", 3, 0, "" },
    { "baseN+1", "baseN+1", 3, 3, "?z", "?z", 3, 0, "" },
    { "baseN+1", "baseN+1", 4, 4, "?z", "?z", 3, 0, "" },
    { "bitmask", "bitmask", 1, 1, "?z", "?z", 3, 0, "" },
    { "bitmask", "bitmask", 2, 2, "?z", "?z", 3, 0, "" },
    { "bitmask", "bitmask", 3, 3, "?z", "?z", 3, 0, "" },
    { "bitmask", "baseN+1", 4, 4, "?z", "?z", 3, 0, "" },

    { "baseN+1", "baseN+1", 1, 1, "?z", "?d", 0, 1, "" },
    { "baseN+1", "baseN+1", 2, 2, "?z", "?d", 0, 1, "" },
    { "baseN+1", "baseN+1", 3, 3, "?z", "?d", 0, 1, "" },
    { "baseN+1", "baseN+1", 4, 4, "?z", "?d", 0, 1, "" },
    { "baseN+1", "baseN+1", 1, 1, "?z", "?d", 1, 1, "" },
    { "baseN+1", "baseN+1", 2, 2, "?z", "?d", 1, 1, "" },
    { "baseN+1", "baseN+1", 3, 3, "?z", "?d", 1, 1, "" },
    { "baseN+1", "baseN+1", 4, 4, "?z", "?d", 1, 1, "" },
    { "baseN+1", "baseN+1", 1, 1, "?z", "?d", 2, 0, "" },
    { "baseN+1", "baseN+1", 2, 2, "?z", "?d", 2, 0, "" },
    { "baseN+1", "baseN+1", 3, 3, "?z", "?d", 2, 0, "" },
    { "baseN+1", "baseN+1", 4, 4, "?z", "?d", 2, 0, "" },
    { "baseN+1", "baseN+1", 1, 1, "?d", "?z", 0, 1, "" },
    { "baseN+1", "baseN+1", 2, 2, "?d", "?z", 0, 1, "" },
    { "baseN+1", "baseN+1", 3, 3, "?d", "?z", 0, 1, "" },
    { "baseN+1", "baseN+1", 4, 4, "?d", "?z", 0, 1, "" },
    { "baseN+1", "baseN+1", 1, 1, "?d", "?z", 1, 1, "" },
    { "baseN+1", "baseN+1", 2, 2, "?d", "?z", 1, 1, "" },
    { "baseN+1", "baseN+1", 3, 3, "?d", "?z", 1, 1, "" },
    { "baseN+1", "baseN+1", 4, 4, "?d", "?z", 1, 1, "" },
    { "baseN+1", "baseN+1", 1, 1, "?d", "?z", 2, 0, "" },
    { "baseN+1", "baseN+1", 2, 2, "?d", "?z", 2, 0, "" },
    { "baseN+1", "baseN+1", 3, 3, "?d", "?z", 2, 0, "" },
    { "baseN+1", "baseN+1", 4, 4, "?d", "?z", 2, 0, "" },
    { "bitmask", "bitmask", 1, 1, "?z", "?d", 0, 1, "" },
    { "bitmask", "bitmask", 2, 2, "?z", "?d", 0, 1, "" },
    { "bitmask", "bitmask", 3, 3, "?z", "?d", 0, 1, "" },
    { "bitmask", "bitmask", 4, 4, "?z", "?d", 0, 1, "" },
    { "bitmask", "bitmask", 1, 1, "?z", "?d", 1, 1, "" },
    { "bitmask", "bitmask", 2, 2, "?z", "?d", 1, 1, "" },
    { "bitmask", "bitmask", 3, 3, "?z", "?d", 1, 1, "" },
    { "bitmask", "bitmask", 4, 4, "?z", "?d", 1, 1, "" },
    { "bitmask", "bitmask", 1, 1, "?z", "?d", 2, 0, "" },
    { "bitmask", "bitmask", 2, 2, "?z", "?d", 2, 0, "" },
    { "bitmask", "bitmask", 3, 3, "?z", "?d", 2, 0, "" },
    { "bitmask", "bitmask", 4, 4, "?z", "?d", 2, 0, "" },
    { "bitmask", "bitmask", 1, 1, "?d", "?z", 0, 1, "" },
    { "bitmask", "bitmask", 2, 2, "?d", "?z", 0, 1, "" },
    { "bitmask", "bitmask", 3, 3, "?d", "?z", 0, 1, "" },
    { "bitmask", "bitmask", 4, 4, "?d", "?z", 0, 1, "" },
    { "bitmask", "bitmask", 1, 1, "?d", "?z", 1, 1, "" },
    { "bitmask", "bitmask", 2, 2, "?d", "?z", 1, 1, "" },
    { "bitmask", "bitmask", 3, 3, "?d", "?z", 1, 1, "" },
    { "bitmask", "bitmask", 4, 4, "?d", "?z", 1, 1, "" },
    { "bitmask", "bitmask", 1, 1, "?d", "?z", 2, 0, "" },
    { "bitmask", "bitmask", 2, 2, "?d", "?z", 2, 0, "" },
    { "bitmask", "bitmask", 3, 3, "?d", "?z", 2, 0, "" },
    { "bitmask", "bitmask", 4, 4, "?d", "?z", 2, 0, "" },

    { "baseN+1", "baseN+1", 1, 1, "?z", "?d?d", 0, 1, "" },
    { "baseN+1", "baseN+1", 2, 2, "?z", "?d?d", 0, 1, "" },
    { "baseN+1", "baseN+1", 3, 3, "?z", "?d?d", 0, 1, "" },
    { "baseN+1", "baseN+1", 4, 4, "?z", "?d?d", 0, 1, "" },
    { "baseN+1", "baseN+1", 1, 1, "?z", "?d?d", 1, 1, "" },
    { "baseN+1", "baseN+1", 2, 2, "?z", "?d?d", 1, 1, "" },
    { "baseN+1", "baseN+1", 3, 3, "?z", "?d?d", 1, 1, "" },
    { "baseN+1", "baseN+1", 4, 4, "?z", "?d?d", 1, 1, "" },
    { "baseN+1", "baseN+1", 1, 1, "?z", "?d?d", 2, 1, "" },
    { "baseN+1", "baseN+1", 2, 2, "?z", "?d?d", 2, 1, "" },
    { "baseN+1", "baseN+1", 3, 3, "?z", "?d?d", 2, 1, "" },
    { "baseN+1", "baseN+1", 4, 4, "?z", "?d?d", 2, 1, "" },
    { "baseN+1", "baseN+1", 1, 1, "?z", "?d?d", 3, 0, "" },
    { "baseN+1", "baseN+1", 2, 2, "?z", "?d?d", 3, 0, "" },
    { "baseN+1", "baseN+1", 3, 3, "?z", "?d?d", 3, 0, "" },
    { "baseN+1", "baseN+1", 4, 4, "?z", "?d?d", 3, 0, "" },
    { "baseN+1", "baseN+1", 1, 1, "?d?d", "?z", 0, 1, "" },
    { "baseN+1", "baseN+1", 2, 2, "?d?d", "?z", 0, 1, "" },
    { "baseN+1", "baseN+1", 3, 3, "?d?d", "?z", 0, 1, "" },
    { "baseN+1", "baseN+1", 4, 4, "?d?d", "?z", 0, 1, "" },
    { "baseN+1", "baseN+1", 1, 1, "?d?d", "?z", 1, 1, "" },
    { "baseN+1", "baseN+1", 2, 2, "?d?d", "?z", 1, 1, "" },
    { "baseN+1", "baseN+1", 3, 3, "?d?d", "?z", 1, 1, "" },
    { "baseN+1", "baseN+1", 4, 4, "?d?d", "?z", 1, 1, "" },
    { "baseN+1", "baseN+1", 1, 1, "?d?d", "?z", 2, 1, "" },
    { "baseN+1", "baseN+1", 2, 2, "?d?d", "?z", 2, 1, "" },
    { "baseN+1", "baseN+1", 3, 3, "?d?d", "?z", 2, 1, "" },
    { "baseN+1", "baseN+1", 4, 4, "?d?d", "?z", 2, 1, "" },
    { "baseN+1", "baseN+1", 1, 1, "?d?d", "?z", 3, 0, "" },
    { "baseN+1", "baseN+1", 2, 2, "?d?d", "?z", 3, 0, "" },
    { "baseN+1", "baseN+1", 3, 3, "?d?d", "?z", 3, 0, "" },
    { "baseN+1", "baseN+1", 4, 4, "?d?d", "?z", 3, 0, "" },
    { "bitmask", "bitmask", 1, 1, "?z", "?d?d", 0, 1, "" },
    { "bitmask", "bitmask", 2, 2, "?z", "?d?d", 0, 1, "" },
    { "bitmask", "bitmask", 3, 3, "?z", "?d?d", 0, 1, "" },
    { "bitmask", "bitmask", 4, 4, "?z", "?d?d", 0, 1, "" },
    { "bitmask", "bitmask", 1, 1, "?z", "?d?d", 1, 1, "" },
    { "bitmask", "bitmask", 2, 2, "?z", "?d?d", 1, 1, "" },
    { "bitmask", "bitmask", 3, 3, "?z", "?d?d", 1, 1, "" },
    { "bitmask", "bitmask", 4, 4, "?z", "?d?d", 1, 1, "" },
    { "bitmask", "bitmask", 1, 1, "?z", "?d?d", 2, 1, "" },
    { "bitmask", "bitmask", 2, 2, "?z", "?d?d", 2, 1, "" },
    { "bitmask", "bitmask", 3, 3, "?z", "?d?d", 2, 1, "" },
    { "bitmask", "bitmask", 4, 4, "?z", "?d?d", 2, 1, "" },
    { "bitmask", "bitmask", 1, 1, "?z", "?d?d", 3, 0, "" },
    { "bitmask", "bitmask", 2, 2, "?z", "?d?d", 3, 0, "" },
    { "bitmask", "bitmask", 3, 3, "?z", "?d?d", 3, 0, "" },
    { "bitmask", "bitmask", 4, 4, "?z", "?d?d", 3, 0, "" },
    { "bitmask", "bitmask", 1, 1, "?d?d", "?z", 0, 1, "" },
    { "bitmask", "bitmask", 2, 2, "?d?d", "?z", 0, 1, "" },
    { "bitmask", "bitmask", 3, 3, "?d?d", "?z", 0, 1, "" },
    { "bitmask", "bitmask", 4, 4, "?d?d", "?z", 0, 1, "" },
    { "bitmask", "bitmask", 1, 1, "?d?d", "?z", 1, 1, "" },
    { "bitmask", "bitmask", 2, 2, "?d?d", "?z", 1, 1, "" },
    { "bitmask", "bitmask", 3, 3, "?d?d", "?z", 1, 1, "" },
    { "bitmask", "bitmask", 4, 4, "?d?d", "?z", 1, 1, "" },
    { "bitmask", "bitmask", 1, 1, "?d?d", "?z", 2, 1, "" },
    { "bitmask", "bitmask", 2, 2, "?d?d", "?z", 2, 1, "" },
    { "bitmask", "bitmask", 3, 3, "?d?d", "?z", 2, 1, "" },
    { "bitmask", "bitmask", 4, 4, "?d?d", "?z", 2, 1, "" },
    { "bitmask", "bitmask", 1, 1, "?d?d", "?z", 3, 0, "" },
    { "bitmask", "bitmask", 2, 2, "?d?d", "?z", 3, 0, "" },
    { "bitmask", "bitmask", 3, 3, "?d?d", "?z", 3, 0, "" },
    { "bitmask", "bitmask", 4, 4, "?d?d", "?z", 3, 0, "" },

    { "baseN+1", "baseN+1", 1, 1, "?u", "?u", 2, 0, "" },
    { "baseN+1", "baseN+1", 1, 1, "?u", "?s", 2, 0, "" },
    { "baseN+1", "baseN+1", 1, 1, "?l", "?s", 1, 1, "" },
    { "baseN+1", "baseN+1", 1, 1, "?l?l?d", "?l?l?d", 3, 0, "" },
    { "baseN+1", "baseN+1", 1, 1, "?l?l?d", "?l?u?l", 2, 1, "" },
    { "baseN+1", "baseN+1", 1, 1, "?s?u?u", "?u?s?u", 1, 1, "" },
    { "baseN+1", "baseN+1", 1, 1, "?s?l?d?d", "?s?u?l?d", 2, 1, "" },
    { "baseN+1", "baseN+1", 1, 1, "?d?d?l?u?s?l", "?s?s?s?s?s?s?s", 2, 1, "" },
    { "baseN+1", "baseN+1", 1, 1, "?d?s?s?u?l?s?d", "?s?s?s?s?s?s?s", 3, 1, "" },
    { "baseN+1", "baseN+1", 1, 1, "?d?d?d?d?d?d?d?d?d?d?d?d?d?d", "?s?s?s?s?s?s?s?s?s?s?s?s?s?s", 12, 1, "" },
    { "baseN+1", "baseN+1", 1, 1, "?d?d?d?d?d?d?d?d?d?d?d?d?d?d", "?l?l?l?l?l?l?l?l?l?l?l?l?l?l", 17, 0, "" },
    { "baseN+1", "baseN+1", 1, 1, "?s?s?s?d?l?u?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s", "?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s", 4, 0, "" },
    { "baseN+1", "baseN+1", 1, 1, "?z", "?s?s?s?s?s?s?s?s?s?s?s?s?s?s", 4, 1, "" },
    { "baseN+1", "baseN+1", 1, 1, "?z", "?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s", 27, 1, "" },
    { "bitmask", "bitmask", 1, 1, "?u", "?u", 2, 0, "" },
    { "bitmask", "bitmask", 1, 1, "?u", "?s", 2, 0, "" },
    { "bitmask", "bitmask", 1, 1, "?l", "?s", 1, 1, "" },
    { "bitmask", "bitmask", 1, 1, "?l?l?d", "?l?l?d", 3, 0, "" },
    { "bitmask", "bitmask", 1, 1, "?l?l?d", "?l?u?l", 2, 1, "" },
    { "bitmask", "bitmask", 1, 1, "?s?u?u", "?u?s?u", 1, 1, "" },
    { "bitmask", "bitmask", 1, 1, "?s?l?d?d", "?s?u?l?d", 2, 1, "" },
    { "bitmask", "bitmask", 1, 1, "?d?d?l?u?s?l", "?s?s?s?s?s?s?s", 2, 1, "" },
    { "bitmask", "bitmask", 1, 1, "?d?s?s?u?l?s?d", "?s?s?s?s?s?s?s", 3, 1, "" },
    { "bitmask", "bitmask", 1, 1, "?d?d?d?d?d?d?d?d?d?d?d?d?d?d", "?s?s?s?s?s?s?s?s?s?s?s?s?s?s", 12, 1, "" },
    { "bitmask", "bitmask", 1, 1, "?d?d?d?d?d?d?d?d?d?d?d?d?d?d", "?l?l?l?l?l?l?l?l?l?l?l?l?l?l", 17, 0, "" },
    { "bitmask", "bitmask", 1, 1, "?s?s?s?d?l?u?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s", "?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s", 4, 0, "" },
    { "bitmask", "bitmask", 1, 1, "?z", "?s?s?s?s?s?s?s?s?s?s?s?s?s?s", 4, 1, "" },
    { "bitmask", "bitmask", 1, 1, "?z", "?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s?s", 27, 1, "" },

    { "baseN+1", "baseN+1", 3, 3, "?d?s?f?u?l?s", "?d?f?f?f?l?l", 3, 1, "" },
    { "bitmask", "bitmask", 3, 3, "?d?s?f?u?l?s", "?d?f?f?f?l?l", 3, 1, "" },
  };

  plan_tests(162);

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

    iAccepted = PwLCheckLevDistance(psPwLContext, asTuples[iIndex].iDistance);
    if (iAccepted == PATHWELL_TRUE || iAccepted == PATHWELL_FALSE)
    {
      snprintf
      (
        acDescription,
        1024,
        "\"%s\", \"%s\", %d, %d, \"%s\", \"%s\", %d, %d, %d",
        asTuples[iIndex].acTypeA,
        asTuples[iIndex].acTypeB,
        asTuples[iIndex].iTokenSetA,
        asTuples[iIndex].iTokenSetB,
        asTuples[iIndex].acTopologyA,
        asTuples[iIndex].acTopologyB,
        asTuples[iIndex].iDistance,
        asTuples[iIndex].iAccepted,
        iAccepted
      );
      ok(iAccepted == asTuples[iIndex].iAccepted, "%s", acDescription);
    }
    else
    {
      snprintf
      (
        acDescription,
        1024,
        "\"%s\", \"%s\", %d, %d, \"%s\", \"%s\", %d, %d, %d, \"%s\", \"%s\"",
        asTuples[iIndex].acTypeA,
        asTuples[iIndex].acTypeB,
        asTuples[iIndex].iTokenSetA,
        asTuples[iIndex].iTokenSetB,
        asTuples[iIndex].acTopologyA,
        asTuples[iIndex].acTopologyB,
        asTuples[iIndex].iDistance,
        asTuples[iIndex].iAccepted,
        iAccepted,
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
