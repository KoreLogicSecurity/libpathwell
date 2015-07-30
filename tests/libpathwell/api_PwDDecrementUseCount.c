/*-
 ***********************************************************************
 *
 * $Id: api_PwDDecrementUseCount.c,v 1.1 2013/08/23 21:13:03 klm Exp $
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
#include <inttypes.h>
#include <pathwell.h>
#include <stdio.h>
#include <stdint.h>
#include <tap.h>

typedef struct _TEST_TUPLES
{
  char acType[16];
  int iTokenSet;
  int64_t i64Id;
  unsigned int uiFinal;
  unsigned int uiCount;
  unsigned int uiFirst;
  char acError[256];
} TEST_TUPLES;

int
main(int iArgumentCount, char *ppcArgumentVector[])
{
  char acDescription[1024] = "";
  int iError = 0;
  int iIndex = 0;
  PW_D_CONTEXT *psPwDContext = NULL;
  PW_T_CONTEXT *psPwTContext = NULL;
  TEST_TUPLES asTuples[] =
  {
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, 1, 1, 1, 2, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, 1, 1, 2, 3, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, 1, 1, 3, 4, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, 1, 2, 1, 3, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, 1, 2, 2, 4, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, 1, 2, 3, 5, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, 1, 3, 1, 4, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, 1, 3, 2, 5, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_1, 1, 3, 3, 6, ""},

    { "baseN+1", PATHWELL_TOKEN_SET_ID_2, 1, 1, 1, 2, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_2, 1, 1, 2, 3, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_2, 1, 1, 3, 4, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_2, 1, 2, 1, 3, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_2, 1, 2, 2, 4, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_2, 1, 2, 3, 5, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_2, 1, 3, 1, 4, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_2, 1, 3, 2, 5, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_2, 1, 3, 3, 6, ""},

    { "baseN+1", PATHWELL_TOKEN_SET_ID_3, 1, 1, 1, 2, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_3, 1, 1, 2, 3, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_3, 1, 1, 3, 4, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_3, 1, 2, 1, 3, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_3, 1, 2, 2, 4, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_3, 1, 2, 3, 5, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_3, 1, 3, 1, 4, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_3, 1, 3, 2, 5, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_3, 1, 3, 3, 6, ""},

    { "baseN+1", PATHWELL_TOKEN_SET_ID_4, 1, 1, 1, 2, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_4, 1, 1, 2, 3, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_4, 1, 1, 3, 4, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_4, 1, 2, 1, 3, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_4, 1, 2, 2, 4, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_4, 1, 2, 3, 5, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_4, 1, 3, 1, 4, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_4, 1, 3, 2, 5, ""},
    { "baseN+1", PATHWELL_TOKEN_SET_ID_4, 1, 3, 3, 6, ""},

    { "bitmask", PATHWELL_TOKEN_SET_ID_1, 132, 1, 1, 2, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, 132, 1, 2, 3, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, 132, 1, 3, 4, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, 132, 2, 1, 3, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, 132, 2, 2, 4, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, 132, 2, 3, 5, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, 132, 3, 1, 4, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, 132, 3, 2, 5, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_1, 132, 3, 3, 6, ""},

    { "bitmask", PATHWELL_TOKEN_SET_ID_2, 132, 1, 1, 2, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_2, 132, 1, 2, 3, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_2, 132, 1, 3, 4, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_2, 132, 2, 1, 3, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_2, 132, 2, 2, 4, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_2, 132, 2, 3, 5, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_2, 132, 3, 1, 4, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_2, 132, 3, 2, 5, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_2, 132, 3, 3, 6, ""},

    { "bitmask", PATHWELL_TOKEN_SET_ID_3, 132, 1, 1, 2, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_3, 132, 1, 2, 3, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_3, 132, 1, 3, 4, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_3, 132, 2, 1, 3, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_3, 132, 2, 2, 4, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_3, 132, 2, 3, 5, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_3, 132, 3, 1, 4, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_3, 132, 3, 2, 5, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_3, 132, 3, 3, 6, ""},

    { "bitmask", PATHWELL_TOKEN_SET_ID_4, 132, 1, 1, 2, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_4, 132, 1, 2, 3, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_4, 132, 1, 3, 4, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_4, 132, 2, 1, 3, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_4, 132, 2, 2, 4, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_4, 132, 2, 3, 5, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_4, 132, 3, 1, 4, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_4, 132, 3, 2, 5, ""},
    { "bitmask", PATHWELL_TOKEN_SET_ID_4, 132, 3, 3, 6, ""},
  };
  unsigned int uiCount = 0;
  unsigned int *puiFinal = NULL;

  plan_tests(72);

  psPwTContext = PwTNewContext();
//ok(psPwTContext != NULL, "new context");
//if (!PwTContextIsValid(psPwTContext))
//{
//  diag(PwTGetError(psPwTContext));
//}

  psPwDContext = PwDNewContextFromParameters(":memory:", SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL, 0);
//ok(PwDContextIsValid(psPwDContext), "new context");
//if (!PwDContextIsValid(psPwDContext))
//{
//  diag(PwDGetError(psPwDContext));
//}

  iError = PwDAcquireConnection(psPwDContext);
//ok(iError == 0, "acquire connection");
//if (iError != ER_OK)
//{
//  diag(PwDGetError(psPwDContext));
//}

  iError = PwDCreateSchema(psPwDContext);
//ok(iError == 0, "create schema");
//if (iError != ER_OK)
//{
//  diag(PwDGetError(psPwDContext));
//}

  for (iIndex = 0; iIndex < (sizeof(asTuples) / sizeof(asTuples[0])); iIndex++)
  {
    iError = PwTSetId(psPwTContext, asTuples[iIndex].i64Id);
//  ok(iError == 0, "set ID");
    iError = PwTSetTokenSet(psPwTContext, asTuples[iIndex].iTokenSet);
//  ok(iError == 0, "set token set");
    iError = PwTSetEncoding(psPwTContext, ((strcmp(asTuples[iIndex].acType, "bitmask") == 0) ? PATHWELL_ENCODING_BITMASK : PATHWELL_ENCODING_BASENP1));
//  ok(iError == 0, "set encoding");
    iError = PwDSetUseCount(psPwDContext, psPwTContext, asTuples[iIndex].uiFirst);
//  ok(iError == 0, "set use count");
    for (uiCount = 0; uiCount < asTuples[iIndex].uiCount; uiCount++)
    {
      iError = PwDDecrementUseCount(psPwDContext, psPwTContext);
//    ok(iError == 0, "decrement use count");
//    if (iError != ER_OK)
//    {
//      diag(PwDGetError(psPwDContext));
//    }
    }
    puiFinal = PwDGetUseCount(psPwDContext, psPwTContext);
    if (puiFinal != NULL)
    {
      snprintf
      (
        acDescription,
        1024,
        "\"%s\", %d, %"PRId64", %u, %u, %u, %u",
        asTuples[iIndex].acType,
        asTuples[iIndex].iTokenSet,
        asTuples[iIndex].i64Id,
        asTuples[iIndex].uiFirst,
        asTuples[iIndex].uiCount,
        asTuples[iIndex].uiFinal,
        *puiFinal
      );
      ok(*puiFinal == asTuples[iIndex].uiFinal, acDescription);
    }
    else
    {
      snprintf
      (
        acDescription,
        1024,
        "\"%s\", %d, %"PRId64", %u, %u, %u, \"%s\"",
        asTuples[iIndex].acType,
        asTuples[iIndex].iTokenSet,
        asTuples[iIndex].i64Id,
        asTuples[iIndex].uiFirst,
        asTuples[iIndex].uiCount,
        asTuples[iIndex].uiFinal,
        PwDGetError(psPwDContext)
      );
      ok(strcmp(PwDGetError(psPwDContext), asTuples[iIndex].acError) == 0, acDescription);
    }
  }

  iError = PwDReleaseConnection(psPwDContext);
//ok(iError == 0, "release connection");
//if (iError != ER_OK)
//{
//  diag(PwDGetError(psPwDContext));
//}

  PwDFreeContext(psPwDContext);
  PwTFreeContext(psPwTContext);

  return exit_status();
}
