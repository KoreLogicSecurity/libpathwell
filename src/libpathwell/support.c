/*-
 ***********************************************************************
 *
 * $Id: support.c,v 1.25 2013/11/01 14:43:31 klm Exp $
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
#include "all-includes.h"

/*-
 ***********************************************************************
 *
 * PwSBaseNTo63Bit
 *
 ***********************************************************************
 */
int
PwSBaseNTo63Bit(char *pcValue, int64_t *pi64Value, int iBase)
{
  int iError = 0;
  int iErrorFlag = 0;
  int iResult = 0;
  int64_t i64ValueLower = 0;
  int64_t i64ValueUpper = 0;
  mpz_t mpzLimit = { 0 };
  mpz_t mpzLimitLower = { 0 };
  mpz_t mpzLimitUpper = { 0 };
  mpz_t mpzValue = { 0 };
  mpz_t mpzValueLower = { 0 };
  mpz_t mpzValueUpper = { 0 };

  /*-
   *********************************************************************
   *
   * Make sure the inputs are valid/supported. Note that GMP is known
   * to support the following base values: [0,2-62], so reject values
   * not in that set.
   *
   *********************************************************************
   */
  if (pcValue == NULL || pi64Value == NULL)
  {
    return -1;
  }

  if (iBase != 0 && (iBase < 2 || iBase > 62))
  {
    return -1;
  }

  /*-
   *********************************************************************
   *
   * Convert the specified value in the specified base to a number that
   * is <= 2^63 - 1 (i.e., 0x7FFFFFFFFFFFFFFF). Note that the following
   * code was designed to work on both 32- and 64-bit platforms.
   *
   *********************************************************************
   */
  mpz_init_set_ui(mpzLimitLower, 0xFFFFFFFF);
  mpz_init_set_ui(mpzLimitUpper, 0x7FFFFFFF);
  mpz_init_set_ui(mpzLimit, 0);
  mpz_init_set_ui(mpzValueLower, 0);
  mpz_init_set_ui(mpzValueUpper, 0);
  iError = mpz_init_set_str(mpzValue, pcValue, iBase);
  if (iError != 0)
  {
    iResult = -1; // The input does not represent a valid number in base N.
  }
  else
  {
    mpz_mul_2exp(mpzLimit, mpzLimitUpper, (mp_bitcnt_t)32);
    mpz_ior(mpzLimit, mpzLimit, mpzLimitLower);
    if (mpz_cmpabs(mpzValue, mpzLimit) > 0)
    {
      iResult = 1; // The magnitude of the converted value is larger than the self-imposed 63-bit limit.
    }
    else
    {
      mpz_tdiv_q_2exp(mpzValueUpper, mpzValue, (mp_bitcnt_t)32);
      mpz_tdiv_r_2exp(mpzValueLower, mpzValue, (mp_bitcnt_t)32);
      i64ValueUpper = (int64_t)(mpz_get_ui(mpzValueUpper)); // Note: The number of bits returned depends on the platform.
      i64ValueLower = (int64_t)(mpz_get_ui(mpzValueLower)); // Note: The number of bits returned depends on the platform.
      *pi64Value = ((mpz_sgn(mpzValue) < 0) ? -1 : 1) * ((i64ValueUpper << 32) + i64ValueLower);
      iResult = 0; // The converted value is OK (i.e., it'll fit inside int64_t).
    }
  }
  mpz_clears
  (
    mpzLimit,
    mpzLimitLower,
    mpzLimitUpper,
    mpzValue,
    mpzValueLower,
    mpzValueUpper,
    NULL
  );

  return iResult;
}


/*-
 ***********************************************************************
 *
 * PwSChomp
 *
 ***********************************************************************
 */
char *
PwSChomp(char *pcString, char *pcChompSet, int iMode)
{
  char *pcChompedString = NULL;
  int iFinalIndex = 0;
  int iFirstIndex = 0;
  int iIndex1 = 0;
  int iIndex2 = 0;
  int iLength = 0;
  int iLChomp = PATHWELL_FALSE;
  int iNChompCharacters = 0;
  int iRChomp = PATHWELL_FALSE;
  int iSize = 0;

  /*-
   *********************************************************************
   *
   * Make sure the inputs are valid/supported. It's considered an error
   * if the input string is NULL, but the same is not true if the chomp
   * set is NULL. Note that an empty chomp set effectively reduces this
   * function to a string copy.
   *
   *********************************************************************
   */
  if (pcString == NULL)
  {
    return NULL;
  }
  iLength = strlen(pcString);
  iFinalIndex = iLength - 1;

  iNChompCharacters = (pcChompSet == NULL) ? 0 : strlen(pcChompSet);

  switch (iMode)
  {
  case PW_S_CHOMP_BOTH:
    iLChomp = PATHWELL_TRUE;
    iRChomp = PATHWELL_TRUE;
    break;
  case PW_S_CHOMP_LEFT:
    iLChomp = PATHWELL_TRUE;
    break;
  case PW_S_CHOMP_RIGHT:
    iRChomp = PATHWELL_TRUE;
    break;
  default:
    return NULL;
    break;
  }

  /*-
   *********************************************************************
   *
   * Conditionally locate the first character of the chomped string.
   *
   *********************************************************************
   */
  if (iLength && iLChomp == PATHWELL_TRUE)
  {
    for (iIndex1 = 0; iIndex1 <= iFinalIndex; iIndex1++)
    {
      for (iIndex2 = 0; iIndex2 < iNChompCharacters; iIndex2++)
      {
        if (pcString[iIndex1] == pcChompSet[iIndex2])
        {
          break;
        }
      }
      if (iIndex2 == iNChompCharacters)
      {
        iFirstIndex = iIndex1;
        break;
      }
    }
    iLength = (iIndex1 > iFinalIndex) ? 0 : iFinalIndex - iFirstIndex + 1;
  }

  /*-
   *********************************************************************
   *
   * Conditionally locate the final character of the chomped string.
   *
   *********************************************************************
   */
  if (iLength && iRChomp == PATHWELL_TRUE)
  {
    for (iIndex1 = iFinalIndex; iIndex1 >= iFirstIndex; iIndex1--)
    {
      for (iIndex2 = 0; iIndex2 < iNChompCharacters; iIndex2++)
      {
        if (pcString[iIndex1] == pcChompSet[iIndex2])
        {
          break;
        }
      }
      if (iIndex2 == iNChompCharacters)
      {
        iFinalIndex = iIndex1;
        break;
      }
    }
    iLength = (iIndex1 < iFirstIndex) ? 0 : iFinalIndex - iFirstIndex + 1;
  }

  /*-
   *********************************************************************
   *
   * Copy the chomped string into a new buffer. If there's nothing left,
   * simply return an empty string.
   *
   *********************************************************************
   */
  iSize = iLength + 1;
  pcChompedString = malloc(sizeof(char) * iSize);
  if (pcChompedString == NULL)
  {
    return NULL;
  }
  strncpy(pcChompedString, (iLength == 0) ? "" : &pcString[iFirstIndex], iSize);
  pcChompedString[iLength] = 0;

  return pcChompedString;
}



/*-
 ***********************************************************************
 *
 * PwSCreateDirectory
 *
 ***********************************************************************
 */
int
PwSCreateDirectory(char *pcPath, mode_t tMode)
{
  char *pcDirectory = NULL;
  char *pcParentDirectory = NULL;
  char **ppcDirectories = NULL;
  int iCount = 0;
  int iError = 0;
  int iIndex = 0;

  /*-
   *********************************************************************
   *
   * Make sure the input is valid.
   *
   *********************************************************************
   */
  errno = 0;
  if (pcPath == NULL)
  {
    errno = ENODATA;
    goto PW_S_CLEANUP;
  }
  pcDirectory = pcPath;

  /*-
   *********************************************************************
   *
   * If the directory already exists, we're done.
   *
   *********************************************************************
   */
  if (PwDDbDirectoryExists(pcDirectory))
  {
    goto PW_S_CLEANUP;
  }

  /*-
   *********************************************************************
   *
   * Convert the specified path into an array of directories.
   *
   *********************************************************************
   */
  iCount = 0;
  do
  {
    ppcDirectories = (char **)realloc(ppcDirectories, sizeof(char *) * (iCount + 1));
    if (ppcDirectories == NULL)
    {
      goto PW_S_CLEANUP; // Note: Error number already set.
    }
    ppcDirectories[iCount++] = pcDirectory;
    if (strcmp(pcDirectory, ".") == 0 || strcmp(pcDirectory, "/") == 0)
    {
      break; // We've found the root.
    }
    pcParentDirectory = PwSDirname(pcDirectory);
    if (pcParentDirectory == NULL)
    {
      goto PW_S_CLEANUP; // Note: Error number already set.
    }
    pcDirectory = pcParentDirectory;
  } while (1);

  /*-
   *********************************************************************
   *
   * Walk the set of directories in reverse order creating any that do
   * not exist along the way.
   *
   *********************************************************************
   */
  for (iIndex = iCount - 1; iIndex >= 0; iIndex--)
  {
    if (!PwDDbDirectoryExists(ppcDirectories[iIndex])) // Note: If this fails, errno will be set.
    {
      iError = mkdir(ppcDirectories[iIndex], tMode);
      if (iError == -1)
      {
        goto PW_S_CLEANUP; // Note: Error number already set.
      }
    }
  }
  errno = 0; // Note: This must be explicitly cleared to indicate success.

PW_S_CLEANUP:
  /*-
   *********************************************************************
   *
   * Release any resources that were allocated. However, don't free the
   * first array element since the caller owns it.
   *
   *********************************************************************
   */
  if (ppcDirectories != NULL)
  {
    for (iIndex = 1; iIndex < iCount; iIndex++)
    {
      free(ppcDirectories[iIndex]);
    }
    free(ppcDirectories);
  }

  return (errno != 0) ? ER : ER_OK;
}


/*-
 ***********************************************************************
 *
 * PwSDirname
 *
 ***********************************************************************
 */
char *
PwSDirname(char *pcPath)
{
  char *pcDirectory = NULL;
  int iLength = 0;
  int iIndex = 0;

  /*-
   *********************************************************************
   *
   * Make sure the input is valid.
   *
   *********************************************************************
   */
  errno = 0;
  if (pcPath == NULL)
  {
    errno = ENODATA;
    return NULL;
  }
  iLength = strlen(pcPath);
  if (iLength > FILENAME_MAX - 1)
  {
    errno = ENAMETOOLONG;
    return NULL;
  }
  iIndex = iLength - 1;

  /*-
   *********************************************************************
   *
   * Allocate memory to hold the result.
   *
   *********************************************************************
   */
  pcDirectory = malloc(sizeof(char) * FILENAME_MAX);
  if (pcDirectory == NULL)
  {
    return NULL; // Note: Error number already set.
  }

  /*-
   *********************************************************************
   *
   * Back up over trailing slashes or until nothing is left.
   *
   *********************************************************************
   */
  while (iIndex > 0 && pcPath[iIndex] == '/')
  {
    iIndex--;
  }

  /*-
   *********************************************************************
   *
   * Back up until the next slash is found or nothing is left.
   *
   *********************************************************************
   */
  while (iIndex > 0 && pcPath[iIndex] != '/')
  {
    iIndex--;
  }

  /*-
   *********************************************************************
   *
   * Back up over trailing slashes or until nothing is left.
   *
   *********************************************************************
   */
  while (iIndex > 0 && pcPath[iIndex] == '/')
  {
    iIndex--;
  }

  /*-
   *********************************************************************
   *
   * Conditionally return "." or anything that remains.
   *
   *********************************************************************
   */
  if (iIndex < 0 || (iIndex == 0 && pcPath[iIndex] != '/'))
  {
    iLength = 1;
    strncpy(pcDirectory, ".", iLength);
  }
  else
  {
    iLength = iIndex + 1;
    strncpy(pcDirectory, pcPath, iLength);
  }
  pcDirectory[iLength] = 0;

  return pcDirectory;
}


/*-
 ***********************************************************************
 *
 * PwSLevenshteinDistance
 *
 * Note: This function is based on the iterative full matrix pseudocode
 *       obtained from the following wiki page:
 *
 *         http://en.wikipedia.org/wiki/Levenshtein_distance
 *
 * Note: This implementation could be more space efficient by using the
 *       iterative two-row matrix approach noted in the above wiki page,
 *       but there is no real urgency to do so until the strings being
 *       evaluated get large enough to become a burden on the stack.
 *
 ***********************************************************************
 */
int
PwSLevenshteinDistance(char *pcStringA, char *pcStringB)
{
  int iCost = 0;
  int iIndexA = 0;
  int iIndexB = 0;
  int iLengthA = (pcStringA != NULL) ? strlen(pcStringA) : 0;
  int iLengthB = (pcStringB != NULL) ? strlen(pcStringB) : 0;
  int aaiMatrix[iLengthA + 1][iLengthB + 1];

  aaiMatrix[0][0] = 0; // Note: This initialization should be fine since all remaining cells will be filled in as we go.
  for (iIndexA = 1; iIndexA <= iLengthA; iIndexA++)
  {
    aaiMatrix[iIndexA][0] = iIndexA;
  }
  for (iIndexB = 1; iIndexB <= iLengthB; iIndexB++)
  {
    aaiMatrix[0][iIndexB] = iIndexB;
  }
  for (iIndexB = 1; iIndexB <= iLengthB; iIndexB++)
  {
    for (iIndexA = 1; iIndexA <= iLengthA; iIndexA++)
    {
      iCost = (pcStringA[iIndexB - 1] == pcStringB[iIndexA - 1]) ? 0 : 1; // There's no edit cost when they're the same.
      aaiMatrix[iIndexA][iIndexB] = PwSMin3
      (
        aaiMatrix[iIndexA - 1][iIndexB    ] + 1,    // Remove
        aaiMatrix[iIndexA    ][iIndexB - 1] + 1,    // Insert
        aaiMatrix[iIndexA - 1][iIndexB - 1] + iCost // Change
      );
    }
  }

  return aaiMatrix[iLengthA][iLengthB];
}
