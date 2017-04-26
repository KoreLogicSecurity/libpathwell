/*-
 ***********************************************************************
 *
 * $Id: api_PwSBaseNTo63Bit.c,v 1.11 2017/04/20 13:23:39 klm Exp $
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
#include <gmp.h>
#include <inttypes.h>
#include <pathwell.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tap.h>

int64_t gi64Value = 0;

typedef struct _TEST_TUPLES
{
  char *pcValue;
  int64_t *pi64Value;
  int iBase;
  int iMpzStatus;
  int iPwSStatus;
} TEST_TUPLES;

int
main(int iArgumentCount, char *ppcArgumentVector[])
{
  char acDescription[1024] = "";
  char acPwSString[64] = "";
  char *pcMpzString = NULL;
  char *pcUndef = "undef";
  int iIndex = 0;
  int iMpzStatus = 0;
  int iPwSStatus = 0;
  mpz_t mpzValue = { 0 };
  TEST_TUPLES asTuples[] =
  {
    {                    NULL, &gi64Value, 10, -1, -1 },
    {                    NULL,       NULL, 10, -1, -1 },
    {                      "", &gi64Value, 10, -1, -1 },
    {                      "",       NULL, 10, -1, -1 },
    {                  "junk", &gi64Value, 10, -1, -1 },
    {                  "junk",       NULL, 10, -1, -1 },
    {                     "0", &gi64Value, 10,  0,  0 },
    {                     "0",       NULL, 10,  0, -1 },
    { "-18446744073709551617", &gi64Value, 10,  0,  1 }, // -(2^64 + 1)
    { "-18446744073709551616", &gi64Value, 10,  0,  1 }, // -(2^64 - 0)
    { "-18446744073709551615", &gi64Value, 10,  0,  1 }, // -(2^64 - 1)
    {  "-9223372036854775809", &gi64Value, 10,  0,  1 }, // -(2^63 + 1)
    {  "-9223372036854775808", &gi64Value, 10,  0,  1 }, // -(2^63 - 0)
    {  "-9223372036854775807", &gi64Value, 10,  0,  0 }, // -(2^63 - 1)
    {           "-4294967297", &gi64Value, 10,  0,  0 }, // -(2^32 + 1)
    {           "-4294967296", &gi64Value, 10,  0,  0 }, // -(2^32 - 0)
    {           "-4294967295", &gi64Value, 10,  0,  0 }, // -(2^32 - 1)
    {           "-2147483649", &gi64Value, 10,  0,  0 }, // -(2^31 + 1)
    {           "-2147483648", &gi64Value, 10,  0,  0 }, // -(2^31 - 0)
    {           "-2147483647", &gi64Value, 10,  0,  0 }, // -(2^31 - 1)
    {                    "-2", &gi64Value, 10,  0,  0 },
    {                    "-1", &gi64Value, 10,  0,  0 },
    {                     "0", &gi64Value, 10,  0,  0 },
    {                     "1", &gi64Value, 10,  0,  0 },
    {                     "2", &gi64Value, 10,  0,  0 },
    {            "2147483647", &gi64Value, 10,  0,  0 }, //  (2^31 - 1)
    {            "2147483648", &gi64Value, 10,  0,  0 }, //  (2^31 - 0)
    {            "2147483649", &gi64Value, 10,  0,  0 }, //  (2^31 + 1)
    {            "4294967295", &gi64Value, 10,  0,  0 }, //  (2^32 - 1)
    {            "4294967296", &gi64Value, 10,  0,  0 }, //  (2^32 - 0)
    {            "4294967297", &gi64Value, 10,  0,  0 }, //  (2^32 + 1)
    {   "9223372036854775807", &gi64Value, 10,  0,  0 }, //  (2^63 - 1)
    {   "9223372036854775808", &gi64Value, 10,  0,  1 }, //  (2^63 - 0)
    {   "9223372036854775809", &gi64Value, 10,  0,  1 }, //  (2^63 + 1)
    {  "18446744073709551615", &gi64Value, 10,  0,  1 }, //  (2^64 - 1)
    {  "18446744073709551616", &gi64Value, 10,  0,  1 }, //  (2^64 - 0)
    {  "18446744073709551617", &gi64Value, 10,  0,  1 }, //  (2^64 + 1)
  };

  plan_tests(37);

  for (iIndex = 0; iIndex < (sizeof(asTuples) / sizeof(asTuples[0])); iIndex++)
  {
    iMpzStatus = mpz_init_set_str(mpzValue, (asTuples[iIndex].pcValue == NULL) ? "" : asTuples[iIndex].pcValue, asTuples[iIndex].iBase);
    if (iMpzStatus == 0)
    {
      pcMpzString = mpz_get_str(NULL, asTuples[iIndex].iBase, mpzValue);
    }
    else
    {
      pcMpzString = pcUndef;
    }
    gi64Value = -55; // Note: This value was chosen to avoid test value conflicts.
    iPwSStatus = PwSBaseNTo63Bit(asTuples[iIndex].pcValue, asTuples[iIndex].pi64Value, asTuples[iIndex].iBase);
    if (gi64Value == -55)
    {
      snprintf(acPwSString, 64, "undef");
    }
    else
    {
      snprintf(acPwSString, 64, "%"PRId64, gi64Value);
    }
    snprintf
    (
      acDescription,
      1024,
      "%d, %d, %s, %s, %s",
      iMpzStatus,
      iPwSStatus,
      (asTuples[iIndex].pcValue == NULL) ? "undef" : asTuples[iIndex].pcValue,
      (pcMpzString == NULL) ? "undef" : pcMpzString,
      acPwSString
    );
    if (asTuples[iIndex].iMpzStatus == asTuples[iIndex].iPwSStatus)
    {
      ok
      (
           iMpzStatus == asTuples[iIndex].iMpzStatus
        && iPwSStatus == asTuples[iIndex].iPwSStatus
        &&
        (
          (pcMpzString != NULL && strcmp(pcMpzString, acPwSString) == 0) || (asTuples[iIndex].pcValue == NULL && pcMpzString == NULL)
        ),
        "%s",
        acDescription
      );
    }
    else
    {
      ok
      (
           iMpzStatus == asTuples[iIndex].iMpzStatus
        && iPwSStatus == asTuples[iIndex].iPwSStatus,
        "%s",
        acDescription
      );
    }
    mpz_clear(mpzValue);
    if (pcMpzString != NULL && pcMpzString != pcUndef)
    {
      free(pcMpzString);
    }
  }

  return exit_status();
}
