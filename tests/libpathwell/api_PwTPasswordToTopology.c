/*-
 ***********************************************************************
 *
 * $Id: api_PwTPasswordToTopology.c,v 1.7 2017/04/20 13:23:39 klm Exp $
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
#include <inttypes.h>
#include <pathwell.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <tap.h>

typedef struct _TEST_TUPLES
{
  int iTokenSet;
  char acPassword[PATHWELL_MAX_PASSWORD_SIZE];
  char acTopology[PATHWELL_MAX_TOPOLOGY_SIZE];
  int iError;
  char acError[256];
} TEST_TUPLES;

int
main(int iArgumentCount, char *ppcArgumentVector[])
{
  char acDescription[1024] = "";
  int iError = 0;
  int iIndex = 0;
  int iTokenSet = 0;
  PW_T_CONTEXT *psPwTContext = NULL;
  TEST_TUPLES asTuples[] =
  {
    { -1, "", "undef", ER, "Invalid or unsupported token set."},
    { -1, "undef", "undef", ER, "Invalid or unsupported token set."},
    { -1, "password", "undef", ER, "Invalid or unsupported token set."},

    { 0, "", "undef", ER, "Invalid or unsupported token set."},
    { 0, "undef", "undef", ER, "Invalid or unsupported token set."},
    { 0, "password", "undef", ER, "Invalid or unsupported token set."},

    { 5, "", "undef", ER, "Invalid or unsupported token set."},
    { 5, "undef", "undef", ER, "Invalid or unsupported token set."},
    { 5, "password", "undef", ER, "Invalid or unsupported token set."},

    { PATHWELL_TOKEN_SET_ID_1, "undef", "undef", ER, "Password undefined."},
    { PATHWELL_TOKEN_SET_ID_2, "undef", "undef", ER, "Password undefined."},
    { PATHWELL_TOKEN_SET_ID_3, "undef", "undef", ER, "Password undefined."},
    { PATHWELL_TOKEN_SET_ID_4, "undef", "undef", ER, "Password undefined."},

    { PATHWELL_TOKEN_SET_ID_1, "", "?z", ER_OK, ""},
    { PATHWELL_TOKEN_SET_ID_2, "", "?z", ER_OK, ""},
    { PATHWELL_TOKEN_SET_ID_3, "", "?z", ER_OK, ""},
    { PATHWELL_TOKEN_SET_ID_4, "", "?z", ER_OK, ""},

    { PATHWELL_TOKEN_SET_ID_1, "password", "?l?l?l?l?l?l?l?l", ER_OK, ""},
    { PATHWELL_TOKEN_SET_ID_2, "password", "?l?l?l?l?l?l?l?l", ER_OK, ""},
    { PATHWELL_TOKEN_SET_ID_3, "password", "?l?l?l?l?l?l?l?l", ER_OK, ""},
    { PATHWELL_TOKEN_SET_ID_4, "password", "?l?l?l?l?l?l?l?l", ER_OK, ""},
  };

  plan_tests(22);

  psPwTContext = PwTNewContext();
  ok(psPwTContext != NULL, "%s", "new context");

  for (iIndex = 0; iIndex < (sizeof(asTuples) / sizeof(asTuples[0])); iIndex++)
  {
    iError = PwTSetPassword(psPwTContext, strcmp(asTuples[iIndex].acPassword, "undef") ? asTuples[iIndex].acPassword : NULL);
//  ok(iError == 0, "%s", "set topology");
    iError = PwTSetTokenSet(psPwTContext, asTuples[iIndex].iTokenSet);
//  ok(iError == 0, "%s", "set token set");
    iError = PwTPasswordToTopology(psPwTContext);
    if (iError != ER_OK)
    {
      snprintf
      (
        acDescription,
        1024,
        "%d, \"%s\", \"%s\", \"%s\"",
        asTuples[iIndex].iTokenSet,
        asTuples[iIndex].acPassword,
        asTuples[iIndex].acError,
        PwTGetError(psPwTContext)
      );
      ok(strcmp(PwTGetError(psPwTContext), asTuples[iIndex].acError) == 0, "%s", acDescription);
    }
    else
    {
      snprintf
      (
        acDescription,
        1024,
        "%d, \"%s\", \"%s\", \"%s\"",
        asTuples[iIndex].iTokenSet,
        asTuples[iIndex].acPassword,
        asTuples[iIndex].acTopology,
        PwTGetTopology(psPwTContext)
      );
      ok(strcmp(PwTGetTopology(psPwTContext), asTuples[iIndex].acTopology) == 0, "%s", acDescription);
    }
  }

  PwTFreeContext(&psPwTContext);

  return exit_status();
}
