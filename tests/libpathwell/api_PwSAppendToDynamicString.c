/*-
 ***********************************************************************
 *
 * $Id: api_PwSAppendToDynamicString.c,v 1.8 2017/04/20 13:23:39 klm Exp $
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
#include <stdlib.h>
#include <string.h>
#include <tap.h>

typedef struct _TEST_TUPLES
{
  char acInput[128];
  char acStringToAppend[128];
  char acOutput[256];
} TEST_TUPLES;

int
main(int iArgumentCount, char *ppcArgumentVector[])
{
  char acDescription[1024] = "";
  char *pcAppendedString = NULL;
  int iAppendIteration = 0;
  int iIndex = 0;
  TEST_TUPLES asTuples[] =
  {
    { "aa", "bb", "aabb" },
    { "aa", "cc", "aacc" },
    { "a", "b", "ab" },
    { "a", "c", "ac" },
    { "94zwuZyN8DqD16h", "TIivplucV8Lyyb5MU3QTOKPPm", "94zwuZyN8DqD16hTIivplucV8Lyyb5MU3QTOKPPm" },
    { "zn1q", "O", "zn1qO" },
    { "vEdYvqf10", "etO9NJ6uMPa45ZLjKUb", "vEdYvqf10etO9NJ6uMPa45ZLjKUb" },
    { "QZ2AYFO", "Sfhqsa0YvwN", "QZ2AYFOSfhqsa0YvwN" },
    { "zjUyjWa", "YAc", "zjUyjWaYAc" },
    { "fQ", "d943dskXfOHX3U2", "fQd943dskXfOHX3U2" },
    { "H7B", "JZj", "H7BJZj" },
    { "xuoVH9", "YUqqjSC1GzzBZBf3GRzvTy", "xuoVH9YUqqjSC1GzzBZBf3GRzvTy" },
    { "Z5k5IVr2CJvPM", "y3UgOSlMsO87DpLLI", "Z5k5IVr2CJvPMy3UgOSlMsO87DpLLI" },
    { "pCmIQvQF03odoRvMvPBx", "ZnP23u3m3iN7inR", "pCmIQvQF03odoRvMvPBxZnP23u3m3iN7inR" },
    { "DNZylFrF82s4v2PYRYbK", "eDbwEno", "DNZylFrF82s4v2PYRYbKeDbwEno" },
    { "y", "rf3sMFsQorxYBdMkwXRq", "yrf3sMFsQorxYBdMkwXRq" },
    { "ahNGA8N3", "ZLgRrXL0x4", "ahNGA8N3ZLgRrXL0x4" },
    { "h5CTEYChwGtPvQu", "2cXa3mG8ztkAkEHMZdJDNe5l", "h5CTEYChwGtPvQu2cXa3mG8ztkAkEHMZdJDNe5l" },
    { "51JU", "Z80rKMYdIR", "51JUZ80rKMYdIR" },
    { "W21X", "YKBEnnQxUYyAQmPoHiP1lk", "W21XYKBEnnQxUYyAQmPoHiP1lk" },
    { "sQE4Azaw9kZE", "me2OOgj9mp6Z0u9nzrQq", "sQE4Azaw9kZEme2OOgj9mp6Z0u9nzrQq" },
    { "f5R5yquwpq2", "ovlgUkEw0T0UyDRUh5PFZerd", "f5R5yquwpq2ovlgUkEw0T0UyDRUh5PFZerd" },
    { "rjP", "d", "rjPd" },
    { "p8kJ", "0", "p8kJ0" },
    { "KylAMEdrUqf2DLtlncY", "RtzXwfY9AZUCSOKFamZnQ", "KylAMEdrUqf2DLtlncYRtzXwfY9AZUCSOKFamZnQ" },
    { "RRe", "ceTfuRkPsi3eyzh2yqN", "RReceTfuRkPsi3eyzh2yqN" },
    { "DrESyvXFABDMCD9Ej8", "holgI2jFTwWo1mtBlK5AYdq", "DrESyvXFABDMCD9Ej8holgI2jFTwWo1mtBlK5AYdq" },
    { "08l", "M80NRkyb", "08lM80NRkyb" },
    { "lnx352eqjtjfnU2F", "p", "lnx352eqjtjfnU2Fp" },
    { "n1fEeplYvrcHTcQVB", "Qp1UKUg2m1bxHB", "n1fEeplYvrcHTcQVBQp1UKUg2m1bxHB" },
    { "T", "ERDIayf9WTLbLFPXrQ", "TERDIayf9WTLbLFPXrQ" },
    { "DYy7g0ptRoL", "Ieh", "DYy7g0ptRoLIeh" },
    { "TrxvkZ6A2Sngdea", "YlDYPGG1kVZv9mpRsP4yKBoS", "TrxvkZ6A2SngdeaYlDYPGG1kVZv9mpRsP4yKBoS" },
    { "lQKdsQTNDgOYrzQz8M0eI", "fMN", "lQKdsQTNDgOYrzQz8M0eIfMN" },
    { "dZ61F", "RmpD0qEj4W15WRbIdWS", "dZ61FRmpD0qEj4W15WRbIdWS" },
    { "KeOHyBzaCXnGqQxrWPFisLW", "B5", "KeOHyBzaCXnGqQxrWPFisLWB5" },
    { "9kCXwNvemLBAtM7J", "cv0Qi7ZCo2eNDjwE2RRo3nX9", "9kCXwNvemLBAtM7Jcv0Qi7ZCo2eNDjwE2RRo3nX9" },
    { "Z28JwQEpx", "yYB8A2SvFFCCQgEt0rj", "Z28JwQEpxyYB8A2SvFFCCQgEt0rj" },
    { "Se8SpDRCo", "WhCAnzwM", "Se8SpDRCoWhCAnzwM" },
    { "uXBhnLwwiQ0xpT0L2gIyea", "7I1eIDtfIGTLDg", "uXBhnLwwiQ0xpT0L2gIyea7I1eIDtfIGTLDg" },
    { "LCY86IrL", "VxwgjUIeLknENqs2xyVk6W", "LCY86IrLVxwgjUIeLknENqs2xyVk6W" },
    { "FUzNOC9MCBQKcHs", "TlD8Eb2g3EyaSMpIUmZ9v0TXh", "FUzNOC9MCBQKcHsTlD8Eb2g3EyaSMpIUmZ9v0TXh" },
    { "InN", "1nj3R8d72Tx", "InN1nj3R8d72Tx" },
    { "QDQAUi1UI", "MjR8O", "QDQAUi1UIMjR8O" },
    { "NXfQEs", "BvpM8adB99pfsdxnGuN3sR", "NXfQEsBvpM8adB99pfsdxnGuN3sR" },
    { "IThkXAT4KRXBXn3NMQln6L", "t", "IThkXAT4KRXBXn3NMQln6Lt" },
    { "W3xzz1Esl0l1YNHkT0", "M9a", "W3xzz1Esl0l1YNHkT0M9a" },
    { "ze94giF", "vTypmRNYIuqjByq", "ze94giFvTypmRNYIuqjByq" },
    { "DwD7lj3dqi1yzJFfla", "AKV9anNoofGlMH8", "DwD7lj3dqi1yzJFflaAKV9anNoofGlMH8" },
    { "AFbBa0j2r", "Lfbyddqmhwsx", "AFbBa0j2rLfbyddqmhwsx" },
    { "ayjmlD3wuNQsA", "90sZZ", "ayjmlD3wuNQsA90sZZ" },
    { "AEspzSgvE", "zOh8mPyz0lnFrRZbN3", "AEspzSgvEzOh8mPyz0lnFrRZbN3" },
    { "jC4w3RHu9n9KaEu", "CVr", "jC4w3RHu9n9KaEuCVr" },
    { "f0q6", "mjtkkAs452mUFBnJ", "f0q6mjtkkAs452mUFBnJ" }
  };

  plan_tests(108);

  for (iIndex = 0; iIndex < (sizeof(asTuples) / sizeof(asTuples[0])); iIndex++)
  {
    /* The basic idea is to build up the final string by performing two appends in succession. */
    for (pcAppendedString = NULL, iAppendIteration = 0; iAppendIteration < 2; iAppendIteration++)
    {
      char *pcStringToAppend = (iAppendIteration == 0) ? asTuples[iIndex].acInput : asTuples[iIndex].acStringToAppend;
      char *pcStringToExpect = (iAppendIteration == 0) ? asTuples[iIndex].acInput : asTuples[iIndex].acOutput;
      pcAppendedString = PwSAppendToDynamicString
      (
        pcAppendedString,
        (strcmp(pcStringToAppend, "undef") != 0) ? pcStringToAppend : NULL
      );
      if (pcAppendedString != NULL)
      {
        snprintf
        (
          acDescription,
          1024,
          "Input: \"%s\"\tExpected Output: \"%s\"\tActual Output: \"%s\"",
          pcStringToAppend,
          pcStringToExpect,
          pcAppendedString
        );
        ok(strcmp(pcStringToExpect, pcAppendedString) == 0, "%s", acDescription);
      }
      else
      {
        snprintf
        (
          acDescription,
          1024,
          "Input: \"%s\"\tExpected Output: \"%s\"\tActual Output: \"%s\"",
          pcStringToAppend,
          pcStringToExpect,
          (pcAppendedString == NULL) ? "undef" : pcAppendedString
        );
        ok(pcAppendedString == NULL && strcmp(pcStringToAppend, "undef") == 0, "%s", acDescription);
      }
    }
    if (pcAppendedString != NULL)
    {
      free(pcAppendedString);
    }
  }

  return exit_status();
}
