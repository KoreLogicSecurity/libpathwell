/*-
 ***********************************************************************
 *
 * $Id: api_PwHGenHint.c,v 1.16 2017/04/21 19:09:25 ssegreti Exp $
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
  char acGeneratedHint[PATHWELL_MAX_TOPOLOGY_SIZE];
  int iEnvironment;
  int iHintInfoLevel;
  int iHintPresLevel;
  int iMaxUse;
  int iMinLen;
  int iMinLev;
  unsigned int uiSeed;
} TEST_TUPLES;

int
main(int iArgumentCount, char *ppcArgumentVector[])
{
  char acDummyOldPassword[]= "";
  int iEnvironment = 0;
  int iError = 0;
  int iIndex = 0;
  int iHintInfoLevel = 0;
  int iHintPresLevel = 0;
  int iMaxUse = 0;
  int iMinLen = 0;
  int iMinLev = 0;
  unsigned int uiSeed = 0;
  PW_D_CONTEXT *psPwDContext = NULL;
  PW_C_CONTEXT *psPwCContext = NULL;
  PW_H_CONTEXT *psPwHContext = NULL;
  PW_T_CONTEXT *psPwTContext = NULL;
  PW_T_CONTEXT *psPwTDummyContext = NULL;
  TEST_TUPLES asTuples[] =
  {
    { "mypassword1", "The new password failed the blacklist check.", PATHWELL_ENV_CLI, PATHWELL_HINT_INFO_LEVEL_1, PATHWELL_HINT_PRES_LEVEL_1, 0, 0, 0, 1234567890},
    { "mypassword1", "The new password failed the blacklist check.", PATHWELL_ENV_CLI, PATHWELL_HINT_INFO_LEVEL_1, PATHWELL_HINT_PRES_LEVEL_2, 0, 0, 0, 1234567890},
    { "mypassword1", "The new password failed the blacklist check.", PATHWELL_ENV_CLI, PATHWELL_HINT_INFO_LEVEL_1, PATHWELL_HINT_PRES_LEVEL_3, 0, 0, 0, 1234567890},
    { "mypassword2", "The new password failed the blacklist check. Try making a password with this topology: llsllllslld.", PATHWELL_ENV_CLI, PATHWELL_HINT_INFO_LEVEL_2, PATHWELL_HINT_PRES_LEVEL_1, 0, 0, 0, 1942},
    { "florida", "PASSES_ALLCHECK", PATHWELL_ENV_CLI, PATHWELL_HINT_INFO_LEVEL_2, PATHWELL_HINT_PRES_LEVEL_1, 0, 0, 0, 8746},
    { "Password1", "The new password failed the blacklist check. Try making a password with this topology: slldlllld.", PATHWELL_ENV_CLI, PATHWELL_HINT_INFO_LEVEL_2, PATHWELL_HINT_PRES_LEVEL_1, 0, 0, 0, 193542},
    { "trustno1", "The new password failed the blacklist check. Try making a password with this topology: llullllld.", PATHWELL_ENV_CLI, PATHWELL_HINT_INFO_LEVEL_2, PATHWELL_HINT_PRES_LEVEL_1, 0, 0, 0, 99526},
    { "mypassword2", "\n                                            lllllllllld\n                                              |    |   \n  replace with a special character -----------+    |   \n                                                   |   \n    replace with a special character --------------+   \n", PATHWELL_ENV_CLI, PATHWELL_HINT_INFO_LEVEL_2, PATHWELL_HINT_PRES_LEVEL_2, 0, 0, 0, 1942},
    { "mypassword2", "\n                                            lllllllllld\n                                              |    |   \n  replace with a special character -----------+    |   \n                                                   |   \n    replace with a special character --------------+   \n\n This should produce the following topology: llsllllslld\n", PATHWELL_ENV_CLI, PATHWELL_HINT_INFO_LEVEL_2, PATHWELL_HINT_PRES_LEVEL_3, 0, 0, 0, 1942},
    { "mypassword3", "The new password failed the blacklist check. Try making a password with this topology: lllllllldlld.", PATHWELL_ENV_CLI, PATHWELL_HINT_INFO_LEVEL_3, PATHWELL_HINT_PRES_LEVEL_1, 0, 0, 0, 27},
    { "mypassword3", "\n                                          mypasswo rd3\n                                                  |   \n  insert a digit ---------------------------------+   \n", PATHWELL_ENV_CLI, PATHWELL_HINT_INFO_LEVEL_3, PATHWELL_HINT_PRES_LEVEL_2, 0, 0, 0, 27},
    { "mypassword3", "\n                                          mypasswo rd3\n                                                  |   \n  insert a digit ---------------------------------+   \n\n This should produce the following topology: lllllllldlld\n", PATHWELL_ENV_CLI, PATHWELL_HINT_INFO_LEVEL_3, PATHWELL_HINT_PRES_LEVEL_3, 0, 0, 0, 27},
    { "mypassword3", "The new password failed the blacklist check. Try making your new password something like this: mypasswo4rd3.", PATHWELL_ENV_CLI, PATHWELL_HINT_INFO_LEVEL_4, PATHWELL_HINT_PRES_LEVEL_1, 0, 0, 0, 27},
    { "mypassword3", "\n                      mypasswo rd3\n                              |   \n  insert a '4' ---------------+   \n", PATHWELL_ENV_CLI, PATHWELL_HINT_INFO_LEVEL_4, PATHWELL_HINT_PRES_LEVEL_2, 0, 0, 0, 27},
    { "mypassword3", "\n                      mypasswo rd3\n                              |   \n  insert a '4' ---------------+   \n\n This should produce the following password: mypasswo4rd3\n", PATHWELL_ENV_CLI, PATHWELL_HINT_INFO_LEVEL_4, PATHWELL_HINT_PRES_LEVEL_3, 0, 0, 0, 27},
    { "mypassword3", "{\"rejected topology\":\"lllllllllld\",\"suggested topology\":\"lllllllldlld\",\"modifications\":[{\"index\":8,\"type\":\"insertion\",\"new value\":\"d\"}]}", PATHWELL_ENV_JAVASCRIPT, PATHWELL_HINT_INFO_LEVEL_2, PATHWELL_HINT_PRES_LEVEL_MIN, 0, 0, 0, 27},
    { "florida", "PASSES_ALLCHECK", PATHWELL_ENV_JAVASCRIPT, PATHWELL_HINT_INFO_LEVEL_2, PATHWELL_HINT_PRES_LEVEL_MIN, 0, 0, 0, 27},
    { "trustno1", "{\"rejected topology\":\"llllllld\",\"suggested topology\":\"lslllllld\",\"modifications\":[{\"index\":1,\"type\":\"insertion\",\"new value\":\"s\"}]}", PATHWELL_ENV_JAVASCRIPT, PATHWELL_HINT_INFO_LEVEL_2, PATHWELL_HINT_PRES_LEVEL_MIN, 0, 0, 0, 12410},
    { "mypassword3", "{\"rejected password\":\"mypassword3\",\"suggested topology\":\"llllllllllld\",\"modifications\":[{\"index\":5,\"type\":\"insertion\",\"new value\":\"l\"}]}", PATHWELL_ENV_JAVASCRIPT, PATHWELL_HINT_INFO_LEVEL_3, PATHWELL_HINT_PRES_LEVEL_MIN, 0, 0, 0, 12451515},
    { "mypassword3", "{\"rejected password\":\"mypassword3\",\"suggested password\":\"mypasswordm3\",\"modifications\":[{\"index\":10,\"type\":\"insertion\",\"new value\":\"m\"}]}", PATHWELL_ENV_JAVASCRIPT, PATHWELL_HINT_INFO_LEVEL_4, PATHWELL_HINT_PRES_LEVEL_MIN, 0, 0, 0, 9876124}
  };

  plan_tests(20); //adjust this later

  // establish a database context, and fill it with topologies from the file
  psPwDContext = PwDNewContextFromParameters(":memory:", SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL, 0);
  //ok(PwDContextIsValid(psPwDContext), "%s", "new context");
  if (!PwDContextIsValid(psPwDContext))
  {
    diag("%s", PwDGetError(psPwDContext));
  }

  iError = PwDAcquireConnection(psPwDContext);
  //ok(iError == 0, "%s", "acquire connection");
  if (iError != ER_OK)
  {
    diag("%s", PwDGetError(psPwDContext));
  }

  iError = PwDCreateSchema(psPwDContext);
  //ok(iError == 0, "%s", "create schema");
  if (iError != ER_OK)
  {
    diag("%s", PwDGetError(psPwDContext));
  }

  iError = PwDLoadBlacklistedTopologies(psPwDContext, ppcArgumentVector[1]);
  //ok(iError == 0, "%s", "load blacklisted topologies");
  if (iError != ER_OK)
  {
    diag("%s", PwDGetError(psPwDContext));
  }

  for (iIndex = 0; iIndex < (sizeof(asTuples) / sizeof(asTuples[0])); iIndex++)
  {
    iEnvironment = asTuples[iIndex].iEnvironment;
    iHintInfoLevel = asTuples[iIndex].iHintInfoLevel;
    iHintPresLevel = asTuples[iIndex].iHintPresLevel;
    iMaxUse = asTuples[iIndex].iMaxUse;
    iMinLen = asTuples[iIndex].iMinLen;
    iMinLev = asTuples[iIndex].iMinLev;
    uiSeed = asTuples[iIndex].uiSeed;

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
    psPwCContext = PwCNewContextFromParameters(psPwDContext, psPwTDummyContext, psPwTContext, iMaxUse, iMinLen, iMinLev, PATHWELL_TRUE);
    //ok(PwCContextIsValid(psPwCContext) == PATHWELL_TRUE, "%s %s.", "creating PwC context for", asTuples[iIndex].acRejectedPassword);
    if (PwCContextIsValid(psPwCContext) != PATHWELL_TRUE)
    {
      diag("%s", PwCGetError(psPwCContext));
    }

    PwCAllCheck(psPwCContext);

    if (PwCGetCheckCode(psPwCContext) == PATHWELL_ALLCHECK_PASS)
    {
      ok(strcmp("PASSES_ALLCHECK", asTuples[iIndex].acGeneratedHint) == 0, "\"%s\" passed pathwell checks", asTuples[iIndex].acRejectedPassword);
    }
    else
    {
      //set up the hint context TODO: vary if deletions are allowed
      psPwHContext = PwHNewContextFromParameters(PATHWELL_HINT_MOD_DELETE_ALLOWED_DEFAULT, iHintInfoLevel, iHintPresLevel, iEnvironment, uiSeed);
      //ok(PwHContextIsValid(psPwHContext) == PATHWELL_TRUE, "%s %s.", "creating PwH context for", asTuples[iIndex].acRejectedPassword);
      if (PwHContextIsValid(psPwHContext) != PATHWELL_TRUE)
      {
        diag("%s", PwHGetError(psPwHContext));
      }

      if (PwHGenHint(psPwHContext, psPwCContext) != ER_OK)
      {
        diag("Problem generating hint for password %s", asTuples[iIndex].acRejectedPassword);
      }

      //check the hint for correctness
      ok(strcmp(PwHGetHint(psPwHContext), asTuples[iIndex].acGeneratedHint) == 0, "testing correctness of generated (%s) vs correct (%s)", PwHGetHint(psPwHContext), asTuples[iIndex].acGeneratedHint);
      //clean up
      PwHFreeContext(&psPwHContext);
    }

    //clean up the rest
    PwCFreeContext(&psPwCContext);
    PwTFreeContext(&psPwTDummyContext);
    PwTFreeContext(&psPwTContext);
  }

  iError = PwDReleaseConnection(psPwDContext);
  //ok(iError == 0, "%s", "release connection");
  if (iError != ER_OK)
  {
    diag("%s", PwDGetError(psPwDContext));
  }

  PwDFreeContext(&psPwDContext);

  return exit_status();
}
