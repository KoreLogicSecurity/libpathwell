/*-
 ***********************************************************************
 *
 * $Id: api_PwSChomp.c,v 1.3 2013/08/30 21:15:01 klm Exp $
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
  char acInput[64];
  char acChompSet[16];
  int iMode;
  char acOutput[64];
} TEST_TUPLES;

int
main(int iArgumentCount, char *ppcArgumentVector[])
{
  char acDescription[1024] = "";
  char *pcChompedString = NULL;
  int iIndex = 0;
  TEST_TUPLES asTuples[] =
  {
    { "undef", "undef", -1, "undef" },
    { "", "", -1, "undef" },

    { "undef", "", PW_S_CHOMP_LEFT, "undef" },
    { "undef", "", PW_S_CHOMP_RIGHT, "undef" },
    { "undef", "", PW_S_CHOMP_BOTH, "undef" },
    { "undef", "", -1, "undef" },

    { "", "undef", PW_S_CHOMP_LEFT, "" },
    { "", "undef", PW_S_CHOMP_RIGHT, "" },
    { "", "undef", PW_S_CHOMP_BOTH, "" },
    { "", "undef", -1, "undef" },

    { "", "", PW_S_CHOMP_LEFT, "" },
    { "", "", PW_S_CHOMP_RIGHT, "" },
    { "", "", PW_S_CHOMP_BOTH, "" },

    { "", "a", PW_S_CHOMP_LEFT, "" },
    { "", "a", PW_S_CHOMP_RIGHT, "" },
    { "", "a", PW_S_CHOMP_BOTH, "" },

    { "", "ab", PW_S_CHOMP_LEFT, "" },
    { "", "ab", PW_S_CHOMP_RIGHT, "" },
    { "", "ab", PW_S_CHOMP_BOTH, "" },

    { "", "abc", PW_S_CHOMP_LEFT, "" },
    { "", "abc", PW_S_CHOMP_RIGHT, "" },
    { "", "abc", PW_S_CHOMP_BOTH, "" },

    { "a", "a", PW_S_CHOMP_LEFT, "" },
    { "a", "a", PW_S_CHOMP_RIGHT, "" },
    { "a", "a", PW_S_CHOMP_BOTH, "" },

    { "aaa", "a", PW_S_CHOMP_LEFT, "" },
    { "aaa", "a", PW_S_CHOMP_RIGHT, "" },
    { "aaa", "a", PW_S_CHOMP_BOTH, "" },

    { "aa", "a", PW_S_CHOMP_LEFT, "" },
    { "aa", "a", PW_S_CHOMP_RIGHT, "" },
    { "aa", "a", PW_S_CHOMP_BOTH, "" },

    { "ab", "ab", PW_S_CHOMP_LEFT, "" },
    { "ab", "ab", PW_S_CHOMP_RIGHT, "" },
    { "ab", "ab", PW_S_CHOMP_BOTH, "" },

    { "abab", "ab", PW_S_CHOMP_LEFT, "" },
    { "abab", "ab", PW_S_CHOMP_RIGHT, "" },
    { "abab", "ab", PW_S_CHOMP_BOTH, "" },

    { "ababab", "ab", PW_S_CHOMP_LEFT, "" },
    { "ababab", "ab", PW_S_CHOMP_RIGHT, "" },
    { "ababab", "ab", PW_S_CHOMP_BOTH, "" },

    { "abc", "abc", PW_S_CHOMP_LEFT, "" },
    { "abc", "abc", PW_S_CHOMP_RIGHT, "" },
    { "abc", "abc", PW_S_CHOMP_BOTH, "" },

    { "abcabc", "abc", PW_S_CHOMP_LEFT, "" },
    { "abcabc", "abc", PW_S_CHOMP_RIGHT, "" },
    { "abcabc", "abc", PW_S_CHOMP_BOTH, "" },

    { "abcabcabc", "abc", PW_S_CHOMP_LEFT, "" },
    { "abcabcabc", "abc", PW_S_CHOMP_RIGHT, "" },
    { "abcabcabc", "abc", PW_S_CHOMP_BOTH, "" },

    { "a", "", PW_S_CHOMP_LEFT, "a" },
    { "a", "", PW_S_CHOMP_RIGHT, "a" },
    { "a", "", PW_S_CHOMP_BOTH, "a" },

    { "a", "x", PW_S_CHOMP_LEFT, "a" },
    { "a", "x", PW_S_CHOMP_RIGHT, "a" },
    { "a", "x", PW_S_CHOMP_BOTH, "a" },

    { "a", "xy", PW_S_CHOMP_LEFT, "a" },
    { "a", "xy", PW_S_CHOMP_RIGHT, "a" },
    { "a", "xy", PW_S_CHOMP_BOTH, "a" },

    { "a", "xyz", PW_S_CHOMP_LEFT, "a" },
    { "a", "xyz", PW_S_CHOMP_RIGHT, "a" },
    { "a", "xyz", PW_S_CHOMP_BOTH, "a" },

    { "aa", "", PW_S_CHOMP_LEFT, "aa" },
    { "aa", "", PW_S_CHOMP_RIGHT, "aa" },
    { "aa", "", PW_S_CHOMP_BOTH, "aa" },

    { "aa", "x", PW_S_CHOMP_LEFT, "aa" },
    { "aa", "x", PW_S_CHOMP_RIGHT, "aa" },
    { "aa", "x", PW_S_CHOMP_BOTH, "aa" },

    { "aa", "xy", PW_S_CHOMP_LEFT, "aa" },
    { "aa", "xy", PW_S_CHOMP_RIGHT, "aa" },
    { "aa", "xy", PW_S_CHOMP_BOTH, "aa" },

    { "aa", "xyz", PW_S_CHOMP_LEFT, "aa" },
    { "aa", "xyz", PW_S_CHOMP_RIGHT, "aa" },
    { "aa", "xyz", PW_S_CHOMP_BOTH, "aa" },

    { "aaa", "", PW_S_CHOMP_LEFT, "aaa" },
    { "aaa", "", PW_S_CHOMP_RIGHT, "aaa" },
    { "aaa", "", PW_S_CHOMP_BOTH, "aaa" },

    { "aaa", "x", PW_S_CHOMP_LEFT, "aaa" },
    { "aaa", "x", PW_S_CHOMP_RIGHT, "aaa" },
    { "aaa", "x", PW_S_CHOMP_BOTH, "aaa" },

    { "aaa", "xy", PW_S_CHOMP_LEFT, "aaa" },
    { "aaa", "xy", PW_S_CHOMP_RIGHT, "aaa" },
    { "aaa", "xy", PW_S_CHOMP_BOTH, "aaa" },

    { "aaa", "xyz", PW_S_CHOMP_LEFT, "aaa" },
    { "aaa", "xyz", PW_S_CHOMP_RIGHT, "aaa" },
    { "aaa", "xyz", PW_S_CHOMP_BOTH, "aaa" },

    { "ab", "a", PW_S_CHOMP_LEFT, "b" },
    { "ab", "a", PW_S_CHOMP_RIGHT, "ab" },
    { "ab", "a", PW_S_CHOMP_BOTH, "b" },

    { "aab", "a", PW_S_CHOMP_LEFT, "b" },
    { "aab", "a", PW_S_CHOMP_RIGHT, "aab" },
    { "aab", "a", PW_S_CHOMP_BOTH, "b" },

    { "aaab", "a", PW_S_CHOMP_LEFT, "b" },
    { "aaab", "a", PW_S_CHOMP_RIGHT, "aaab" },
    { "aaab", "a", PW_S_CHOMP_BOTH, "b" },

    { "ba", "a", PW_S_CHOMP_LEFT, "ba" },
    { "ba", "a", PW_S_CHOMP_RIGHT, "b" },
    { "ba", "a", PW_S_CHOMP_BOTH, "b" },

    { "baa", "a", PW_S_CHOMP_LEFT, "baa" },
    { "baa", "a", PW_S_CHOMP_RIGHT, "b" },
    { "baa", "a", PW_S_CHOMP_BOTH, "b" },

    { "baaa", "a", PW_S_CHOMP_LEFT, "baaa" },
    { "baaa", "a", PW_S_CHOMP_RIGHT, "b" },
    { "baaa", "a", PW_S_CHOMP_BOTH, "b" },

    { "abb", "a", PW_S_CHOMP_LEFT, "bb" },
    { "abb", "a", PW_S_CHOMP_RIGHT, "abb" },
    { "abb", "a", PW_S_CHOMP_BOTH, "bb" },

    { "aabb", "a", PW_S_CHOMP_LEFT, "bb" },
    { "aabb", "a", PW_S_CHOMP_RIGHT, "aabb" },
    { "aabb", "a", PW_S_CHOMP_BOTH, "bb" },

    { "aaabb", "a", PW_S_CHOMP_LEFT, "bb" },
    { "aaabb", "a", PW_S_CHOMP_RIGHT, "aaabb" },
    { "aaabb", "a", PW_S_CHOMP_BOTH, "bb" },

    { "bba", "a", PW_S_CHOMP_LEFT, "bba" },
    { "bba", "a", PW_S_CHOMP_RIGHT, "bb" },
    { "bba", "a", PW_S_CHOMP_BOTH, "bb" },

    { "bbaa", "a", PW_S_CHOMP_LEFT, "bbaa" },
    { "bbaa", "a", PW_S_CHOMP_RIGHT, "bb" },
    { "bbaa", "a", PW_S_CHOMP_BOTH, "bb" },

    { "bbaaa", "a", PW_S_CHOMP_LEFT, "bbaaa" },
    { "bbaaa", "a", PW_S_CHOMP_RIGHT, "bb" },
    { "bbaaa", "a", PW_S_CHOMP_BOTH, "bb" },

    { "aba", "a", PW_S_CHOMP_LEFT, "ba" },
    { "aba", "a", PW_S_CHOMP_RIGHT, "ab" },
    { "aba", "a", PW_S_CHOMP_BOTH, "b" },

    { "aabaa", "a", PW_S_CHOMP_LEFT, "baa" },
    { "aabaa", "a", PW_S_CHOMP_RIGHT, "aab" },
    { "aabaa", "a", PW_S_CHOMP_BOTH, "b" },

    { "aaabaaa", "a", PW_S_CHOMP_LEFT, "baaa" },
    { "aaabaaa", "a", PW_S_CHOMP_RIGHT, "aaab" },
    { "aaabaaa", "a", PW_S_CHOMP_BOTH, "b" },

    { "abba", "a", PW_S_CHOMP_LEFT, "bba" },
    { "abba", "a", PW_S_CHOMP_RIGHT, "abb" },
    { "abba", "a", PW_S_CHOMP_BOTH, "bb" },

    { "aabbaa", "a", PW_S_CHOMP_LEFT, "bbaa" },
    { "aabbaa", "a", PW_S_CHOMP_RIGHT, "aabb" },
    { "aabbaa", "a", PW_S_CHOMP_BOTH, "bb" },

    { "aaabbaaa", "a", PW_S_CHOMP_LEFT, "bbaaa" },
    { "aaabbaaa", "a", PW_S_CHOMP_RIGHT, "aaabb" },
    { "aaabbaaa", "a", PW_S_CHOMP_BOTH, "bb" },

    { "abbba", "a", PW_S_CHOMP_LEFT, "bbba" },
    { "abbba", "a", PW_S_CHOMP_RIGHT, "abbb" },
    { "abbba", "a", PW_S_CHOMP_BOTH, "bbb" },

    { "aabbbaa", "a", PW_S_CHOMP_LEFT, "bbbaa" },
    { "aabbbaa", "a", PW_S_CHOMP_RIGHT, "aabbb" },
    { "aabbbaa", "a", PW_S_CHOMP_BOTH, "bbb" },

    { "aaabbbaaa", "a", PW_S_CHOMP_LEFT, "bbbaaa" },
    { "aaabbbaaa", "a", PW_S_CHOMP_RIGHT, "aaabbb" },
    { "aaabbbaaa", "a", PW_S_CHOMP_BOTH, "bbb" },

    { "aaaaahelloaaaaa", "a", PW_S_CHOMP_LEFT, "helloaaaaa" },
    { "aaaaahelloaaaaa", "a", PW_S_CHOMP_RIGHT, "aaaaahello" },
    { "aaaaahelloaaaaa", "a", PW_S_CHOMP_BOTH, "hello" },

    { " \t\t test string \t with tokens\t  \t", " \t", PW_S_CHOMP_LEFT, "test string \t with tokens\t  \t" },
    { " \t\t test string \t with tokens\t  \t", " \t", PW_S_CHOMP_RIGHT, " \t\t test string \t with tokens" },
    { " \t\t test string \t with tokens\t  \t", " \t", PW_S_CHOMP_BOTH, "test string \t with tokens" }
  };

  plan_tests(154);

  for (iIndex = 0; iIndex < (sizeof(asTuples) / sizeof(asTuples[0])); iIndex++)
  {
    pcChompedString = PwSChomp
    (
      strcmp(asTuples[iIndex].acInput, "undef") ? asTuples[iIndex].acInput : NULL,
      strcmp(asTuples[iIndex].acChompSet, "undef") ? asTuples[iIndex].acChompSet : NULL,
      asTuples[iIndex].iMode
    );
    if (pcChompedString != NULL)
    {
      snprintf
      (
        acDescription,
        1024,
        "Input: \"%s\"\tExpected Output: \"%s\"\tActual Output: \"%s\"",
        asTuples[iIndex].acInput,
        asTuples[iIndex].acOutput,
        pcChompedString
      );
      ok(strcmp(asTuples[iIndex].acOutput, pcChompedString) == 0, acDescription);
    }
    else
    {
      snprintf
      (
        acDescription,
        1024,
        "Input: \"%s\"\tExpected Output: \"%s\"\tActual Output: \"%s\"",
        asTuples[iIndex].acInput,
        asTuples[iIndex].acOutput,
        (pcChompedString == NULL) ? "undef" : pcChompedString
      );
      ok(pcChompedString == NULL && strcmp(asTuples[iIndex].acOutput, "undef") == 0, acDescription);
    }
  }

  return exit_status();
}
