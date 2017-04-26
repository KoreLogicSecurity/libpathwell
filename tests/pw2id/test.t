#!/bin/sh
#!/usr/bin/perl -w
eval 'exec perl -x ${0} ${1+"${@}"} ;'
  if (0);
######################################################################
#
# $Id: test.t,v 1.14 2017/04/18 22:12:52 klm Exp $
#
######################################################################
#
# Copyright 2013-2017 The PathWell Project, All Rights Reserved.
#
# This software, having been partly or wholly developed and/or
# sponsored by KoreLogic, Inc., is hereby released under the terms
# and conditions set forth in the project's "README.LICENSE" file.
# For a list of all contributors and sponsors, please refer to the
# project's "README.CREDITS" file.
#
######################################################################
#
# Purpose: Tests for pw2id.
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
  my $sCommand = "../../src/pw2id/pathwell-pw2id" . GetProperties()->{'Extension'};
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

=head1 TEST_COUNT=12

=head1 TEST_SUMMARY

Test other conversions.

=cut

  my @aTuples =
  (
    ["$^X -e 'print qq(\\n);'", "-f -", "Password=''; ID='0';"],
    ["$^X -e 'print qq(Summer2013!\\n);'", "-f - -t bitmask", "Password='Summer2013!'; ID='182801411';"],
    ["$^X -e 'print qq(Change123.\\n);'", "-f -", "Password='Change123.'; ID='6835784';"],
    ["$^X -e 'print qq(4ppl3\\n);'", "-f - -s 3", "Password='4ppl3'; ID='3200';"],
    ["$^X -e 'print qq(abc123***\\n);'", "-f - -t baseN+1 -s 1", "Password='abc123***'; ID='972749';"],
    ["$^X -e 'print qq(abc123***\\n);'", "-f - -s 4 -t bitmask", "Password='abc123***'; ID='5522063579';"],

    ["", "''", "Password=''; ID='0';"],
    ["", "-t bitmask 'Summer2013!'", "Password='Summer2013!'; ID='182801411';"],
    ["", "'Change123.'", "Password='Change123.'; ID='6835784';"],
    ["", "-s 3 '4ppl3'", "Password='4ppl3'; ID='3200';"],
    ["", "-t baseN+1 -s 1 'abc123***'", "Password='abc123***'; ID='972749';"],
    ["", "-s 4 -t bitmask 'abc123***'", "Password='abc123***'; ID='5522063579';"],
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

