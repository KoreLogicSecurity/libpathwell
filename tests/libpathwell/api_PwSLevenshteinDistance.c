/*-
 ***********************************************************************
 *
 * $Id: api_PwSLevenshteinDistance.c,v 1.6 2017/04/18 22:12:51 klm Exp $
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
#include <stdio.h>
#include <string.h>
#include <tap.h>

typedef struct _TEST_TUPLES
{
  char acStringA[256];
  char acStringB[256];
  int iDistance;
} TEST_TUPLES;

int
main(int iArgumentCount, char *ppcArgumentVector[])
{
  char acDescription[1024] = "";
  int iDistance = 0;
  int iIndex = 0;
  TEST_TUPLES asTuples[] =
  {
    {      "undef",      "undef",  0 },
    {      "undef",           "",  0 },
    {           "",      "undef",  0 },
    {           "",           "",  0 },
    {           "",          "a",  1 },
    {           "",         "aa",  2 },
    {           "",        "aaa",  3 },
    {           "",       "aaaa",  4 },
    {           "",      "aaaaa",  5 },
    {           "",     "aaaaaa",  6 },
    {           "",    "aaaaaaa",  7 },
    {           "",   "aaaaaaaa",  8 },
    {           "",  "aaaaaaaaa",  9 },
    {           "", "aaaaaaaaaa", 10 },
    {          "a",           "",  1 },
    {         "aa",           "",  2 },
    {        "aaa",           "",  3 },
    {       "aaaa",           "",  4 },
    {      "aaaaa",           "",  5 },
    {     "aaaaaa",           "",  6 },
    {    "aaaaaaa",           "",  7 },
    {   "aaaaaaaa",           "",  8 },
    {  "aaaaaaaaa",           "",  9 },
    { "aaaaaaaaaa",           "", 10 },
    {          "a",          "a",  0 },
    {         "aa",         "aa",  0 },
    {        "aaa",        "aaa",  0 },
    {       "aaaa",       "aaaa",  0 },
    {      "aaaaa",      "aaaaa",  0 },
    {     "aaaaaa",     "aaaaaa",  0 },
    {    "aaaaaaa",    "aaaaaaa",  0 },
    {   "aaaaaaaa",   "aaaaaaaa",  0 },
    {  "aaaaaaaaa",  "aaaaaaaaa",  0 },
    { "aaaaaaaaaa", "aaaaaaaaaa",  0 },
    {        "aaa",        "baa",  1 },
    {        "aaa",        "aba",  1 },
    {        "aaa",        "aab",  1 },
    {        "aaa",        "bba",  2 },
    {        "aaa",        "bab",  2 },
    {        "aaa",        "abb",  2 },
    {        "baa",        "aaa",  1 },
    {        "aba",        "aaa",  1 },
    {        "aab",        "aaa",  1 },
    {        "bba",        "aaa",  2 },
    {        "bab",        "aaa",  2 },
    {        "abb",        "aaa",  2 },
    {        "aaa",       "1aaa",  1 },
    {        "aaa",       "a1aa",  1 },
    {        "aaa",       "aa1a",  1 },
    {        "aaa",       "aaa1",  1 },
    {        "aaa",      "11aaa",  2 },
    {        "aaa",      "a11aa",  2 },
    {        "aaa",      "aa11a",  2 },
    {        "aaa",      "aaa11",  2 },
    {        "aaa",     "111aaa",  3 },
    {        "aaa",     "a111aa",  3 },
    {        "aaa",     "aa111a",  3 },
    {        "aaa",     "aaa111",  3 },
    {        "aaa",     "1a11aa",  3 },
    {        "aaa",     "1aa11a",  3 },
    {        "aaa",     "1aaa11",  3 },
    {        "aaa",     "1a1a1a",  3 },
    {        "aaa",     "a1a1a1",  3 },
    {     "Sunday",   "Saturday",  3 },
  };

  plan_tests(64);

  for (iIndex = 0; iIndex < (sizeof(asTuples) / sizeof(asTuples[0])); iIndex++)
  {
    iDistance = PwSLevenshteinDistance
    (
      (strcmp(asTuples[iIndex].acStringA, "undef") ? asTuples[iIndex].acStringA : NULL),
      (strcmp(asTuples[iIndex].acStringB, "undef") ? asTuples[iIndex].acStringB : NULL)
    );
    snprintf
    (
      acDescription,
      1024,
      "\"%s\", \"%s\", %d, %d",
      asTuples[iIndex].acStringA,
      asTuples[iIndex].acStringB,
      asTuples[iIndex].iDistance,
      iDistance
    );
    ok(iDistance == asTuples[iIndex].iDistance, "%s", acDescription);
  }

  return exit_status();
}
