#!/bin/sh
#!/usr/bin/perl -w
eval 'exec perl -x ${0} ${1+"${@}"} ;'
  if (0);
######################################################################
#
# $Id: test.t,v 1.13.2.5 2015/09/30 16:05:54 klm Exp $
#
######################################################################
#
# Copyright 2013-2015 The PathWell Project, All Rights Reserved.
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
  ####################################################################
  #
  # The Properties hash is essentially private. Those parts of the
  # program that wish to access or modify the data in this hash need
  # to call GetProperties() to obtain a reference.
  #
  ####################################################################

  my (%hProperties);

  ####################################################################
  #
  # Initialize platform-specific variables.
  #
  ####################################################################

  if ($^O =~ /MSWin(32|64)/i)
  {
    $hProperties{'OsClass'} = "WINX";
    $hProperties{'Extension'} = ".exe";
  }
  else
  {
    $hProperties{'OsClass'} = "UNIX";
    $hProperties{'Extension'} = "";
  }

  ####################################################################
  #
  # Define helper routines.
  #
  ####################################################################

  sub GetProperties
  {
    return \%hProperties;
  }

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

  ####################################################################
  #
  # Create a test plan.
  #
  ####################################################################

  plan('tests' => GetTestCount());
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
  my $sCommand = "../../src/id2topology/pathwell-id2topology" . GetProperties()->{'Extension'};
  my $sCommandLine = "";
  my $sErrFile = "test.t.stderr";
  my $sMatch = 0;
  my $sOutFile = "test.t.stdout";
  my $sSrcDir = $ENV{'srcdir'} || ".";

  $sCommand =~ s,/,\\,g if (GetProperties()->{'OsClass'} eq "WINX");

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
    ["$^X -e 'print qq(\\n);'", "-f -", "ID=''; Topology='undef'; Error='ID could not be converted to a 63-bit integer.';"],
    ["$^X -e 'print qq(foo\\n);'", "-f -", "ID='foo'; Topology='undef'; Error='ID could not be converted to a 63-bit integer.';"],
    ["$^X -e 'print qq(1foo\\n);'", "-f -", "ID='1foo'; Topology='undef'; Error='ID could not be converted to a 63-bit integer.';"],
    ["$^X -e 'print qq(foo1\\n);'", "-f -", "ID='foo1'; Topology='undef'; Error='ID could not be converted to a 63-bit integer.';"],
    ["$^X -e 'print qq(1foo1\\n);'", "-f -", "ID='1foo1'; Topology='undef'; Error='ID could not be converted to a 63-bit integer.';"],
    ["$^X -e 'print qq(1foo1foo\\n);'", "-f -", "ID='1foo1foo'; Topology='undef'; Error='ID could not be converted to a 63-bit integer.';"],
    ["$^X -e 'print qq(foo1foo1\\n);'", "-f -", "ID='foo1foo1'; Topology='undef'; Error='ID could not be converted to a 63-bit integer.';"],
    ["$^X -e 'print qq(1foo1foo1\\n);'", "-f -", "ID='1foo1foo1'; Topology='undef'; Error='ID could not be converted to a 63-bit integer.';"],
    ["$^X -e 'print qq(0\\n);'", "-f -", "ID='0'; Topology='?z';"],
    ["$^X -e 'print qq(5\\n);'", "-f -", "ID='5'; Topology='undef'; Error='Invalid baseN+1 topology ID.';"],
    ["$^X -e 'print qq(123\\n);'", "-f -", "ID='123'; Topology='?s?s?u';"],
    ["$^X -e 'print qq(425\\n);'", "-f - -s 2 ", "ID='425'; Topology='?d?f?s?f';"],
    ["$^X -e 'print qq(32124\\n);'", "-f - -s 4", "ID='32124'; Topology='?H?x?f?H?s';"],
    ["$^X -e 'print qq(124\\n);'", "-f -", "ID='124'; Topology='?s?s?s';"],
    ["$^X -e 'print qq(\\n);'", "-f - -t bitmask", "ID=''; Topology='undef'; Error='ID could not be converted to a 63-bit integer.';"],
    ["$^X -e 'print qq(foo\\n);'", "-f - -t bitmask", "ID='foo'; Topology='undef'; Error='ID could not be converted to a 63-bit integer.';"],
    ["$^X -e 'print qq(1foo\\n);'", "-f - -t bitmask", "ID='1foo'; Topology='undef'; Error='ID could not be converted to a 63-bit integer.';"],
    ["$^X -e 'print qq(foo1\\n);'", "-f - -t bitmask", "ID='foo1'; Topology='undef'; Error='ID could not be converted to a 63-bit integer.';"],
    ["$^X -e 'print qq(1foo1\\n);'", "-f - -t bitmask", "ID='1foo1'; Topology='undef'; Error='ID could not be converted to a 63-bit integer.';"],
    ["$^X -e 'print qq(1foo1foo\\n);'", "-f - -t bitmask", "ID='1foo1foo'; Topology='undef'; Error='ID could not be converted to a 63-bit integer.';"],
    ["$^X -e 'print qq(foo1foo1\\n);'", "-f - -t bitmask", "ID='foo1foo1'; Topology='undef'; Error='ID could not be converted to a 63-bit integer.';"],
    ["$^X -e 'print qq(1foo1foo1\\n);'", "-f - -t bitmask", "ID='1foo1foo1'; Topology='undef'; Error='ID could not be converted to a 63-bit integer.';"],
    ["$^X -e 'print qq(0\\n);'", "-f - -t bitmask", "ID='0'; Topology='?z';"],
    ["$^X -e 'print qq(1\\n);'", "-f - -t bitmask", "ID='1'; Topology='undef'; Error='Topology ID falls outside the allowed range.';"],
    ["$^X -e 'print qq(37942\\n);'", "-f - -t bitmask", "ID='37942'; Topology='?d?d?s?l?u';"],

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
      $sArgs1 =~ s/'/"/g if (GetProperties()->{'OsClass'} eq "WINX");
    }

    $sArgs2 = @{$paArguments}[1];
    $sArgs2 =~ s/^ */ /g;
    $sArgs2 =~ s/ +$//g;
    $sArgs2 =~ s/'/"/g if (GetProperties()->{'OsClass'} eq "WINX");

    $sCommandLine = $sArgs1 . $sCommand . $sArgs2;

    chomp(@aOutput = qx($sCommandLine));

    ok($aOutput[0] eq @{$paArguments}[2] , "Test command: \`$sCommandLine\`.");
  }

