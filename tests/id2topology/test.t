#!/usr/bin/perl -w
######################################################################
#
# $Id: test.t,v 1.13 2013/09/10 18:34:56 klm Exp $
#
######################################################################
#
# Copyright 2013-2013 KoreLogic Inc., All Rights Reserved.
#
# This software, having been partly or wholly developed and/or
# sponsored by KoreLogic, Inc., is hereby released under the terms
# and conditions set forth in the project's "README.LICENSE" file.
# For a list of all contributors and sponsors, please refer to the
# project's "README.CREDITS" file.
#
######################################################################
#
# Purpose: Tests for KoreLogic::PathWell::PwTRoutines.
#
######################################################################

use 5.008;
use strict;
use Test::More 0.045;

BEGIN
{
  plan('tests' => GetTestCount());

  sub GetTestCount
  {
    my $sCount = 0;
    open(FH, __FILE__) or die("Error='Failed to open test file ($!).'\n");
    while (my $sLine = <FH>)
    {
      $sCount += $1 if ($sLine =~ /^=head1 TEST_COUNT=(\d+)/)
    }
    close(FH);
    return $sCount;
  }
}

######################################################################
#
# VerifyExitStatus
#
######################################################################

sub VerifyExitStatus
{
  my ($sActualStatus, $sTargetExitCode, $sTargetSignal) = @_;

  my $sActualSignal = $sActualStatus & 127;
# my $sActualCoreDump = $sActualStatus & 128; # Reserved for future use.
  my $sActualExitCode = $sActualStatus >> 8;

  return ($sActualExitCode == $sTargetExitCode && $sActualSignal == $sTargetSignal) ? 1 : 0;
}


######################################################################
#
# Tests
#
######################################################################

  ####################################################################
  #
  # Declare common variables and data structures.
  #
  ####################################################################

  my @aOutput = ();
  my $sActualStatus = 0;
  my $sCommand = "../../src/id2topology/pathwell-id2topology";
  my $sCommandLine = "";
  my $sErrFile = "test.t.stderr";
  my $sMatch = 0;
  my $sOutFile = "test.t.stdout";
  my $sSrcDir = $ENV{'srcdir'} || ".";

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

=pod

=head1 TEST_GROUP=1

=head1 TEST_COUNT=1

=head1 TEST_SUMMARY

Test usage when no arguments are given.

=cut

  $sCommandLine = qq($sCommand > $sOutFile 2> $sErrFile);
  @aOutput = qx($sCommandLine);
  $sActualStatus = $?;
  $sMatch = 0;
  if (open(FH, "< $sErrFile"))
  {
    while (my $sLine = <FH>)
    {
      if ($sLine =~ /^Usage:/)
      {
        $sMatch = 1;
        last;
      }
    }
    close(FH);
  }
  ok(VerifyExitStatus($sActualStatus, 1, 0) && $sMatch, "Test usage when no arguments are given.");


#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

=pod

=head1 TEST_GROUP=3

=head1 TEST_COUNT=50

=head1 TEST_SUMMARY

Test other conversions.

=cut

  my @aTuples =
  (
    ["echo ''", "-f -", "ID=''; Topology='undef'; Error='ID could not be converted to a 63-bit integer.';"],
    ["echo 'foo'", "-f -", "ID='foo'; Topology='undef'; Error='ID could not be converted to a 63-bit integer.';"],
    ["echo '1foo'", "-f -", "ID='1foo'; Topology='undef'; Error='ID could not be converted to a 63-bit integer.';"],
    ["echo 'foo1'", "-f -", "ID='foo1'; Topology='undef'; Error='ID could not be converted to a 63-bit integer.';"],
    ["echo '1foo1'", "-f -", "ID='1foo1'; Topology='undef'; Error='ID could not be converted to a 63-bit integer.';"],
    ["echo '1foo1foo'", "-f -", "ID='1foo1foo'; Topology='undef'; Error='ID could not be converted to a 63-bit integer.';"],
    ["echo 'foo1foo1'", "-f -", "ID='foo1foo1'; Topology='undef'; Error='ID could not be converted to a 63-bit integer.';"],
    ["echo '1foo1foo1'", "-f -", "ID='1foo1foo1'; Topology='undef'; Error='ID could not be converted to a 63-bit integer.';"],
    ["echo '0'", "-f -", "ID='0'; Topology='?z';"],
    ["echo '5'", "-f -", "ID='5'; Topology='undef'; Error='Invalid baseN+1 topology ID.';"],
    ["echo '123'", "-f -", "ID='123'; Topology='?s?s?u';"],
    ["echo '425'", "-f - -s 2 ", "ID='425'; Topology='?d?f?s?f';"],
    ["echo '32124'", "-f - -s 4", "ID='32124'; Topology='?H?x?f?H?s';"],
    ["echo '124'", "-f -", "ID='124'; Topology='?s?s?s';"],
    ["echo ''", "-f - -t bitmask", "ID=''; Topology='undef'; Error='ID could not be converted to a 63-bit integer.';"],
    ["echo 'foo'", "-f - -t bitmask", "ID='foo'; Topology='undef'; Error='ID could not be converted to a 63-bit integer.';"],
    ["echo '1foo'", "-f - -t bitmask", "ID='1foo'; Topology='undef'; Error='ID could not be converted to a 63-bit integer.';"],
    ["echo 'foo1'", "-f - -t bitmask", "ID='foo1'; Topology='undef'; Error='ID could not be converted to a 63-bit integer.';"],
    ["echo '1foo1'", "-f - -t bitmask", "ID='1foo1'; Topology='undef'; Error='ID could not be converted to a 63-bit integer.';"],
    ["echo '1foo1foo'", "-f - -t bitmask", "ID='1foo1foo'; Topology='undef'; Error='ID could not be converted to a 63-bit integer.';"],
    ["echo 'foo1foo1'", "-f - -t bitmask", "ID='foo1foo1'; Topology='undef'; Error='ID could not be converted to a 63-bit integer.';"],
    ["echo '1foo1foo1'", "-f - -t bitmask", "ID='1foo1foo1'; Topology='undef'; Error='ID could not be converted to a 63-bit integer.';"],
    ["echo '0'", "-f - -t bitmask", "ID='0'; Topology='?z';"],
    ["echo '1'", "-f - -t bitmask", "ID='1'; Topology='undef'; Error='Topology ID falls outside the allowed range.';"],
    ["echo '37942'", "-f - -t bitmask", "ID='37942'; Topology='?d?d?s?l?u';"],

    ["", "''", "ID=''; Topology='undef'; Error='ID could not be converted to a 63-bit integer.';"],
    ["", "'foo'", "ID='foo'; Topology='undef'; Error='ID could not be converted to a 63-bit integer.';"],
    ["", "'1foo'", "ID='1foo'; Topology='undef'; Error='ID could not be converted to a 63-bit integer.';"],
    ["", "'foo1'", "ID='foo1'; Topology='undef'; Error='ID could not be converted to a 63-bit integer.';"],
    ["", "'1foo1'", "ID='1foo1'; Topology='undef'; Error='ID could not be converted to a 63-bit integer.';"],
    ["", "'1foo1foo'", "ID='1foo1foo'; Topology='undef'; Error='ID could not be converted to a 63-bit integer.';"],
    ["", "'foo1foo1'", "ID='foo1foo1'; Topology='undef'; Error='ID could not be converted to a 63-bit integer.';"],
    ["", "'1foo1foo1'", "ID='1foo1foo1'; Topology='undef'; Error='ID could not be converted to a 63-bit integer.';"],
    ["", "0", "ID='0'; Topology='?z';"],
    ["", "5", "ID='5'; Topology='undef'; Error='Invalid baseN+1 topology ID.';"],
    ["", "123", "ID='123'; Topology='?s?s?u';"],
    ["", "-s 2 425", "ID='425'; Topology='?d?f?s?f';"],
    ["", "-s 4 32124", "ID='32124'; Topology='?H?x?f?H?s';"],
    ["", "124", "ID='124'; Topology='?s?s?s';"],
    ["", "-t bitmask ''", "ID=''; Topology='undef'; Error='ID could not be converted to a 63-bit integer.';"],
    ["", "-t bitmask 'foo'", "ID='foo'; Topology='undef'; Error='ID could not be converted to a 63-bit integer.';"],
    ["", "-t bitmask '1foo'", "ID='1foo'; Topology='undef'; Error='ID could not be converted to a 63-bit integer.';"],
    ["", "-t bitmask 'foo1'", "ID='foo1'; Topology='undef'; Error='ID could not be converted to a 63-bit integer.';"],
    ["", "-t bitmask '1foo1'", "ID='1foo1'; Topology='undef'; Error='ID could not be converted to a 63-bit integer.';"],
    ["", "-t bitmask '1foo1foo'", "ID='1foo1foo'; Topology='undef'; Error='ID could not be converted to a 63-bit integer.';"],
    ["", "-t bitmask 'foo1foo1'", "ID='foo1foo1'; Topology='undef'; Error='ID could not be converted to a 63-bit integer.';"],
    ["", "-t bitmask '1foo1foo1'", "ID='1foo1foo1'; Topology='undef'; Error='ID could not be converted to a 63-bit integer.';"],
    ["", "-t bitmask 0", "ID='0'; Topology='?z';"],
    ["", "-t bitmask 1", "ID='1'; Topology='undef'; Error='Topology ID falls outside the allowed range.';"],
    ["", "-t bitmask 37942", "ID='37942'; Topology='?d?d?s?l?u';"]
  );

  my $sArgs1 = "";
  my $sArgs2 = "";

  foreach my $paArguments (@aTuples)
  {
    $sArgs1 = @{$paArguments}[0];
    if ($sArgs1 ne "")
    {
      $sArgs1 =~ s/ *$/ | /g;
    }

    $sArgs2 = @{$paArguments}[1];
    $sArgs2 =~ s/^ */ /g;
    $sArgs2 =~ s/ +$//g;

    $sCommandLine = $sArgs1 . $sCommand . $sArgs2;

    chomp(@aOutput = qx($sCommandLine));

    ok($aOutput[0] eq @{$paArguments}[2] , "Test command: \`$sCommandLine\`.");
  }

