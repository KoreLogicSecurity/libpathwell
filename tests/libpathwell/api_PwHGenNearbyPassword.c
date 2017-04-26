/*-
 ***********************************************************************
 *
 * $Id: api_PwHGenNearbyPassword.c,v 1.10 2017/04/21 19:09:25 ssegreti Exp $
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
#include <stdlib.h>
#include <string.h>
#include <tap.h>

typedef struct _TEST_TUPLES
{
  char acRejectedPassword[PATHWELL_MAX_PASSWORD_SIZE];
  char acGeneratedPassword[PATHWELL_MAX_TOPOLOGY_SIZE];
  int iSeed;
} TEST_TUPLES;

int
main(int iArgumentCount, char *ppcArgumentVector[])
{
  char acDummyOldPassword[] = "";
  int iError = 0;
  int iIndex = 0;
  PW_D_CONTEXT *psPwDContext = NULL;
  PW_C_CONTEXT *psPwCContext = NULL;
  PW_H_CONTEXT *psPwHContext = NULL;
  PW_T_CONTEXT *psPwTContext = NULL;
  PW_T_CONTEXT *psPwTDummyContext = NULL;
  TEST_TUPLES asTuples[] = // all of these passwords should be blacklisted in ../../data/topology_blacklist.default
  {
    { "mypassword1", "C~mypaRssword1", 1234567890},
    { "mypassword2", "my&assw.rd2", 1942},
    { "mypassword3", "mypasswo4rd3", 27}
  };

  plan_tests(20); //adjust this later

  // establish a database context, and fill it with topologies from the file
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
    //convert the new password to a topology and an ID
    psPwTContext = PwTNewContextFromParameters(PATHWELL_DEFAULT_ENCODING, PATHWELL_DEFAULT_TOKEN_SET_ID, asTuples[iIndex].acRejectedPassword, NULL, NULL);
    if (psPwTContext == NULL)
    {
      diag("%s", PwTGetError(psPwTContext));
    }
    if (PwTPasswordToTopology(psPwTContext) != ER_OK)
    {
      diag("%s", PwTGetError(psPwTContext));
    }
    if (PwTTopologyToId(psPwTContext) != ER_OK)
    {
      diag("%s", PwTGetError(psPwTContext));
    }

    psPwTDummyContext = PwTNewContextFromParameters(PATHWELL_DEFAULT_ENCODING, PATHWELL_DEFAULT_TOKEN_SET_ID, acDummyOldPassword, NULL, NULL);
    if (PwTContextIsValid(psPwTDummyContext) != PATHWELL_TRUE)
    {
      diag("%s", PwTGetError(psPwTDummyContext));
    }

    //set up the pathwell checking context
    psPwCContext = PwCNewContextFromParameters(psPwDContext, psPwTDummyContext, psPwTContext, 0, 0, 0, PATHWELL_TRUE);
    ok(PwCContextIsValid(psPwCContext) == PATHWELL_TRUE, "%s %s.", "creating PwC context for", asTuples[iIndex].acRejectedPassword);
    if (PwCContextIsValid(psPwCContext) != PATHWELL_TRUE)
    {
      diag("%s", PwCGetError(psPwCContext));
    }

    //set up the hint context
    psPwHContext = PwHNewContextFromParameters(PATHWELL_HINT_MOD_DELETE_ALLOWED_DEFAULT, PATHWELL_HINT_PRES_LEVEL_MIN, PATHWELL_HINT_PRES_LEVEL_MAX, PATHWELL_ENV_DEFAULT, asTuples[iIndex].iSeed);
    ok(PwHContextIsValid(psPwHContext) == PATHWELL_TRUE, "%s %s.", "creating PwH context for", asTuples[iIndex].acRejectedPassword);
    if (PwHContextIsValid(psPwHContext) != PATHWELL_TRUE)
    {
      diag("%s", PwHGetError(psPwHContext));
    }

    //call the hint context's code to generate a new topology
    ok(PwHGenNearbyTopology(psPwHContext, psPwCContext) == ER_OK, "%s %s.", "Generating nearby topology for", asTuples[iIndex].acRejectedPassword);

    //call the hint context's code to generate a new password
    ok(PwHGenNearbyPassword(psPwHContext, psPwCContext) == ER_OK, "%s %s.", "Generating nearby password for", asTuples[iIndex].acRejectedPassword);

    //check the new password for correctness
    ok(strcmp(PwHGetNearbyPassword(psPwHContext), asTuples[iIndex].acGeneratedPassword) == 0, "testing correctness of generated (%s) vs correct (%s)", PwHGetNearbyPassword(psPwHContext), asTuples[iIndex].acGeneratedPassword);

    //clean up
    PwTFreeContext(&psPwTContext);
    PwTFreeContext(&psPwTDummyContext);
    PwCFreeContext(&psPwCContext);
    PwHFreeContext(&psPwHContext);
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
