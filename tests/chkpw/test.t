#!/bin/sh
#!/usr/bin/perl -w
eval 'exec perl -x ${0} ${1+"${@}"} ;'
  if (0);
######################################################################
#
# $Id: test.t,v 1.17 2017/04/19 19:43:26 ssegreti Exp $
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
# Purpose: Tests for chkpw.
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

  my @aTuples = ();
  my @aOutput = ();
  my $sActualStatus = 0;
  my $sCommand = "../../src/chkpw/pathwell-chkpw" . GetProperties()->{'Extension'};
  my $sCommandLine = "";
  my $sCreateDbCommand = "../../src/create-db/pathwell-create-db" . GetProperties()->{'Extension'};
  my $sCreateDbCommandLine = "";
  my $sDatabaseFilename = "./pathwell_testing.db";
  my $sErrFile = "test.t.stderr";
  my $sMatch = 0;
  my $sArgs = "";
  my $sOutFile = "test.t.stdout";
  my $sSeedArg = " -S 5";
  my $sSrcDir = $ENV{'srcdir'} || ".";

  $sCommand =~ s,/,\\,g if (GetProperties()->{'OsClass'} eq "WINX");
  $sCreateDbCommand =~ s,/,\\,g if (GetProperties()->{'OsClass'} eq "WINX");

  $sArgs = " -c -f $sSrcDir/../../data/topology_blacklist.default -d $sDatabaseFilename";
  $sArgs =~ s/'/"/g if (GetProperties()->{'OsClass'} eq "WINX");
  $sCreateDbCommandLine = $sCreateDbCommand . $sArgs;

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

=head1 TEST_GROUP=2

=head1 TEST_COUNT=2

=head1 TEST_SUMMARY

Test the ability to create and cleanup a database.

=cut

  unlink("$sDatabaseFilename");
  
  @aOutput = qx($sCreateDbCommandLine);

  ok(join("", @aOutput) eq "", "Test command: \`$sCreateDbCommandLine\`.");

  ok(unlink("$sDatabaseFilename") eq 1, "Cleaning up database for TEST_GROUP 2.");

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

=pod

=head1 TEST_GROUP=3

=head1 TEST_COUNT=4

=head1 TEST_SUMMARY

Test generic usage with no pathwell checks.

=cut

  # Create a fresh database.
  @aOutput = qx($sCreateDbCommandLine);
  ok(join("", @aOutput) eq "", "Creating fresh database for TEST_GROUP 3.");

  @aTuples =
  (
    [" -d $sDatabaseFilename '' 'Password123!'", "OldPassword='';\nOldTopology='?z';\nOldId='0';\nNewPassword='Password123!';\nNewTopology='?u?l?l?l?l?l?l?l?d?d?d?s';\nNewId='170898284';\nPasswordOk='true';\nPasswordHint='';\n"],
    [" -d $sDatabaseFilename '' 'PASSWORD'", "OldPassword='';\nOldTopology='?z';\nOldId='0';\nNewPassword='PASSWORD';\nNewTopology='?u?u?u?u?u?u?u?u';\nNewId='292968';\nPasswordOk='true';\nPasswordHint='';\n"]
  );

  $sArgs = "";

  foreach my $paArguments (@aTuples)
  {
    $sArgs = @{$paArguments}[0];
    $sArgs =~ s/^ */ /g;
    $sArgs =~ s/ +$//g;
    $sArgs =~ s/'/"/g if (GetProperties()->{'OsClass'} eq "WINX");

    $sCommandLine = $sCommand . $sArgs;

    @aOutput = qx($sCommandLine);

    ok(join("", @aOutput) eq @{$paArguments}[1] , "Test command: \`$sCommandLine\`.");
  }

  #Clean up the used database.
  ok(unlink("$sDatabaseFilename") eq 1, "Cleaning up database for TEST_GROUP 3.");


#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

=pod

=head1 TEST_GROUP=4

=head1 TEST_COUNT=4

=head1 TEST_SUMMARY

Blacklist checks.

=cut

  # Create a fresh database.
  @aOutput = qx($sCreateDbCommandLine);
  ok(join("", @aOutput) eq "", "Creating fresh database for TEST_GROUP 4.");

  @aTuples =
  (
    [" -b -d $sDatabaseFilename '' 'PASSWORD'", "OldPassword='';\nOldTopology='?z';\nOldId='0';\nNewPassword='PASSWORD';\nNewTopology='?u?u?u?u?u?u?u?u';\nNewId='292968';\nPasswordOk='false';\nPasswordHint='The new password failed the blacklist check.';\n"],
    [" -b -d $sDatabaseFilename '' '8quxDlKkiUTAh0nMMpdA'", "OldPassword='';\nOldTopology='?z';\nOldId='0';\nNewPassword='8quxDlKkiUTAh0nMMpdA';\nNewTopology='?d?l?l?l?u?l?u?l?l?u?u?u?l?d?l?u?u?l?l?u';\nNewId='28641979867938';\nPasswordOk='true';\nPasswordHint='';\n"]
  );

  $sArgs = "";

  foreach my $paArguments (@aTuples)
  {
    $sArgs = @{$paArguments}[0];
    $sArgs =~ s/^ */ /g;
    $sArgs =~ s/ +$//g;
    $sArgs =~ s/'/"/g if (GetProperties()->{'OsClass'} eq "WINX");

    $sCommandLine = $sCommand . $sArgs;

    @aOutput = qx($sCommandLine);

    ok(join("", @aOutput) eq @{$paArguments}[1] , "Test command: \`$sCommandLine\`.");
  }

  #Clean up the used database.
  ok(unlink("$sDatabaseFilename") eq 1, "Cleaning up database for TEST_GROUP 4.");

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

=pod

=head1 TEST_GROUP=5

=head1 TEST_COUNT=8

=head1 TEST_SUMMARY

Max use checks (some with increments).

=cut

  # Create a fresh database.
  @aOutput = qx($sCreateDbCommandLine);
  ok(join("", @aOutput) eq "", "Creating fresh database for TEST_GROUP 5.");

  @aTuples =
  (
    [" -u 1 -d $sDatabaseFilename '' '11111111'", "OldPassword='';\nOldTopology='?z';\nOldId='0';\nNewPassword='11111111';\nNewTopology='?d?d?d?d?d?d?d?d';\nNewId='97656';\nPasswordOk='true';\nPasswordHint='';\n"],
    [" -i -u 1 -d $sDatabaseFilename '' '11111111'", "OldPassword='';\nOldTopology='?z';\nOldId='0';\nNewPassword='11111111';\nNewTopology='?d?d?d?d?d?d?d?d';\nNewId='97656';\nPasswordOk='true';\nPasswordHint='';\n"],
    [" -u 1 -d $sDatabaseFilename '' '11111111'", "OldPassword='';\nOldTopology='?z';\nOldId='0';\nNewPassword='11111111';\nNewTopology='?d?d?d?d?d?d?d?d';\nNewId='97656';\nPasswordOk='false';\nPasswordHint='The new password failed the maxuse check.';\n"],
    [" -u 1 -d $sDatabaseFilename '' 'PASSWORD'", "OldPassword='';\nOldTopology='?z';\nOldId='0';\nNewPassword='PASSWORD';\nNewTopology='?u?u?u?u?u?u?u?u';\nNewId='292968';\nPasswordOk='true';\nPasswordHint='';\n"],
    [" -i -u 1 -d $sDatabaseFilename '' 'PASSWORD'", "OldPassword='';\nOldTopology='?z';\nOldId='0';\nNewPassword='PASSWORD';\nNewTopology='?u?u?u?u?u?u?u?u';\nNewId='292968';\nPasswordOk='true';\nPasswordHint='';\n"],
    [" -u 1 -d $sDatabaseFilename '' 'PASSWORD'", "OldPassword='';\nOldTopology='?z';\nOldId='0';\nNewPassword='PASSWORD';\nNewTopology='?u?u?u?u?u?u?u?u';\nNewId='292968';\nPasswordOk='false';\nPasswordHint='The new password failed the maxuse check.';\n"]
  );

  $sArgs = "";

  foreach my $paArguments (@aTuples)
  {
    $sArgs = @{$paArguments}[0];
    $sArgs =~ s/^ */ /g;
    $sArgs =~ s/ +$//g;
    $sArgs =~ s/'/"/g if (GetProperties()->{'OsClass'} eq "WINX");

    $sCommandLine = $sCommand . $sArgs;

    @aOutput = qx($sCommandLine);

    ok(join("", @aOutput) eq @{$paArguments}[1] , "Test command: \`$sCommandLine\`.");
  }

  #Clean up the used database.
  ok(unlink("$sDatabaseFilename") eq 1, "Cleaning up database for TEST_GROUP 5.");

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

=pod

=head1 TEST_GROUP=6

=head1 TEST_COUNT=7

=head1 TEST_SUMMARY

Minimum Levenshtein distance checks.

=cut

  # Create a fresh database.
  @aOutput = qx($sCreateDbCommandLine);
  ok(join("", @aOutput) eq "", "Creating fresh database for TEST_GROUP 6.");

  @aTuples =
  (
    [" -L 1 -d $sDatabaseFilename '11111111' '11111112'", "OldPassword='11111111';\nOldTopology='?d?d?d?d?d?d?d?d';\nOldId='97656';\nNewPassword='11111112';\nNewTopology='?d?d?d?d?d?d?d?d';\nNewId='97656';\nPasswordOk='false';\nPasswordHint='The new password failed the minlev check.';\n"],
    [" -L 1 -d $sDatabaseFilename '11111111' '1111111a'", "OldPassword='11111111';\nOldTopology='?d?d?d?d?d?d?d?d';\nOldId='97656';\nNewPassword='1111111a';\nNewTopology='?d?d?d?d?d?d?d?l';\nNewId='97657';\nPasswordOk='true';\nPasswordHint='';\n"],
    [" -L 2 -d $sDatabaseFilename 'password' 'password'", "OldPassword='password';\nOldTopology='?l?l?l?l?l?l?l?l';\nOldId='195312';\nNewPassword='password';\nNewTopology='?l?l?l?l?l?l?l?l';\nNewId='195312';\nPasswordOk='false';\nPasswordHint='The new password failed the minlev check.';\n"],
    [" -L 2 -d $sDatabaseFilename 'password' 'passw0rD'", "OldPassword='password';\nOldTopology='?l?l?l?l?l?l?l?l';\nOldId='195312';\nNewPassword='passw0rD';\nNewTopology='?l?l?l?l?l?d?l?u';\nNewId='195288';\nPasswordOk='true';\nPasswordHint='';\n"],
    [" -L 2 -d $sDatabaseFilename 'PASSWORD' 'PASSWORD'", "OldPassword='PASSWORD';\nOldTopology='?u?u?u?u?u?u?u?u';\nOldId='292968';\nNewPassword='PASSWORD';\nNewTopology='?u?u?u?u?u?u?u?u';\nNewId='292968';\nPasswordOk='false';\nPasswordHint='The new password failed the minlev check.';\n"]
  );

  $sArgs = "";

  foreach my $paArguments (@aTuples)
  {
    $sArgs = @{$paArguments}[0];
    $sArgs =~ s/^ */ /g;
    $sArgs =~ s/ +$//g;
    $sArgs =~ s/'/"/g if (GetProperties()->{'OsClass'} eq "WINX");

    $sCommandLine = $sCommand . $sArgs;

    @aOutput = qx($sCommandLine);

    ok(join("", @aOutput) eq @{$paArguments}[1] , "Test command: \`$sCommandLine\`.");
  }

  #Clean up the used database.
  ok(unlink("$sDatabaseFilename") eq 1, "Cleaning up database for TEST_GROUP 6.");

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

=pod

=head1 TEST_GROUP=7

=head1 TEST_COUNT=14

=head1 TEST_SUMMARY

Minimum length checks.

=cut

  # Create a fresh database.
  @aOutput = qx($sCreateDbCommandLine);
  ok(join("", @aOutput) eq "", "Creating fresh database for TEST_GROUP 7.");

  @aTuples =
  (
    [" -l 6 -d $sDatabaseFilename '' 'a'", "OldPassword='';\nOldTopology='?z';\nOldId='0';\nNewPassword='a';\nNewTopology='?l';\nNewId='2';\nPasswordOk='false';\nPasswordHint='The new password did not meet the minimum length requirement.';\n"],
    [" -l 6 -d $sDatabaseFilename '' 'aa'", "OldPassword='';\nOldTopology='?z';\nOldId='0';\nNewPassword='aa';\nNewTopology='?l?l';\nNewId='12';\nPasswordOk='false';\nPasswordHint='The new password did not meet the minimum length requirement.';\n"],
    [" -l 6 -d $sDatabaseFilename '' 'aaa'", "OldPassword='';\nOldTopology='?z';\nOldId='0';\nNewPassword='aaa';\nNewTopology='?l?l?l';\nNewId='62';\nPasswordOk='false';\nPasswordHint='The new password did not meet the minimum length requirement.';\n"],
    [" -l 6 -d $sDatabaseFilename '' 'aaaa'", "OldPassword='';\nOldTopology='?z';\nOldId='0';\nNewPassword='aaaa';\nNewTopology='?l?l?l?l';\nNewId='312';\nPasswordOk='false';\nPasswordHint='The new password did not meet the minimum length requirement.';\n"],
    [" -l 6 -d $sDatabaseFilename '' 'aaaaa'", "OldPassword='';\nOldTopology='?z';\nOldId='0';\nNewPassword='aaaaa';\nNewTopology='?l?l?l?l?l';\nNewId='1562';\nPasswordOk='false';\nPasswordHint='The new password did not meet the minimum length requirement.';\n"],
    [" -l 6 -d $sDatabaseFilename '' 'aaaaaa'", "OldPassword='';\nOldTopology='?z';\nOldId='0';\nNewPassword='aaaaaa';\nNewTopology='?l?l?l?l?l?l';\nNewId='7812';\nPasswordOk='true';\nPasswordHint='';\n"],
    [" -l 6 -d $sDatabaseFilename '' 'aaaaaaa'", "OldPassword='';\nOldTopology='?z';\nOldId='0';\nNewPassword='aaaaaaa';\nNewTopology='?l?l?l?l?l?l?l';\nNewId='39062';\nPasswordOk='true';\nPasswordHint='';\n"],
    [" -l 6 -d $sDatabaseFilename '' 'aaaaaaaa'", "OldPassword='';\nOldTopology='?z';\nOldId='0';\nNewPassword='aaaaaaaa';\nNewTopology='?l?l?l?l?l?l?l?l';\nNewId='195312';\nPasswordOk='true';\nPasswordHint='';\n"],
    [" -l 6 -d $sDatabaseFilename '' 'aaaaaaaaa'", "OldPassword='';\nOldTopology='?z';\nOldId='0';\nNewPassword='aaaaaaaaa';\nNewTopology='?l?l?l?l?l?l?l?l?l';\nNewId='976562';\nPasswordOk='true';\nPasswordHint='';\n"],
    [" -l 6 -d $sDatabaseFilename '' 'aaaaaaaaaa'", "OldPassword='';\nOldTopology='?z';\nOldId='0';\nNewPassword='aaaaaaaaaa';\nNewTopology='?l?l?l?l?l?l?l?l?l?l';\nNewId='4882812';\nPasswordOk='true';\nPasswordHint='';\n"],
    [" -l 10 -d $sDatabaseFilename '' 'PASSWORD'", "OldPassword='';\nOldTopology='?z';\nOldId='0';\nNewPassword='PASSWORD';\nNewTopology='?u?u?u?u?u?u?u?u';\nNewId='292968';\nPasswordOk='false';\nPasswordHint='The new password did not meet the minimum length requirement.';\n"],
    [" -l 8 -d $sDatabaseFilename '' 'PASSWORD'", "OldPassword='';\nOldTopology='?z';\nOldId='0';\nNewPassword='PASSWORD';\nNewTopology='?u?u?u?u?u?u?u?u';\nNewId='292968';\nPasswordOk='true';\nPasswordHint='';\n"]
  );

  $sArgs = "";

  foreach my $paArguments (@aTuples)
  {
    $sArgs = @{$paArguments}[0];
    $sArgs =~ s/^ */ /g;
    $sArgs =~ s/ +$//g;
    $sArgs =~ s/'/"/g if (GetProperties()->{'OsClass'} eq "WINX");

    $sCommandLine = $sCommand . $sArgs;

    @aOutput = qx($sCommandLine);

    ok(join("", @aOutput) eq @{$paArguments}[1] , "Test command: \`$sCommandLine\`.");
  }

  #Clean up the used database.
  ok(unlink("$sDatabaseFilename") eq 1, "Cleaning up database for TEST_GROUP 7.");

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

=pod

=head1 TEST_GROUP=8

=head1 TEST_COUNT=7

=head1 TEST_SUMMARY

Hint engine CLI checks on hint info level 2.

=cut

  # Create a fresh database.
  @aOutput = qx($sCreateDbCommandLine);
  ok(join("", @aOutput) eq "", "Creating fresh database for TEST_GROUP 8.");

  @aTuples =
  (
    [" -b -h 2 -d $sDatabaseFilename '' 'mypasswd'", "OldPassword='';\nOldTopology='?z';\nOldId='0';\nNewPassword='mypasswd';\nNewTopology='?l?l?l?l?l?l?l?l';\nNewId='195312';\nPasswordOk='false';\nPasswordHint='The new password failed the blacklist check. Try making a password with this topology: ldldlllll.';\n"],
    [" -b -h 2 -p 1 -d $sDatabaseFilename '' 'mypasswd'", "OldPassword='';\nOldTopology='?z';\nOldId='0';\nNewPassword='mypasswd';\nNewTopology='?l?l?l?l?l?l?l?l';\nNewId='195312';\nPasswordOk='false';\nPasswordHint='The new password failed the blacklist check. Try making a password with this topology: ldldlllll.';\n"],
    [" -b -h 2 -p 2 -d $sDatabaseFilename '' 'mypasswd'", "OldPassword='';\nOldTopology='?z';\nOldId='0';\nNewPassword='mypasswd';\nNewTopology='?l?l?l?l?l?l?l?l';\nNewId='195312';\nPasswordOk='false';\nPasswordHint='\n                                            l lllllll\n                                             | |     \n  insert a digit ----------------------------+ |     \n                                               |     \n    replace with a digit ----------------------+     \n';\n"],
    [" -b -h 2 -p 3 -d $sDatabaseFilename '' 'mypasswd'", "OldPassword='';\nOldTopology='?z';\nOldId='0';\nNewPassword='mypasswd';\nNewTopology='?l?l?l?l?l?l?l?l';\nNewId='195312';\nPasswordOk='false';\nPasswordHint='\n                                            l lllllll\n                                             | |     \n  insert a digit ----------------------------+ |     \n                                               |     \n    replace with a digit ----------------------+     \n\n This should produce the following topology: ldldlllll\n';\n"],
    [" -b -h 2 -e 1 -d $sDatabaseFilename '' 'mypasswd'", "OldPassword='';\nOldTopology='?z';\nOldId='0';\nNewPassword='mypasswd';\nNewTopology='?l?l?l?l?l?l?l?l';\nNewId='195312';\nPasswordOk='false';\nPasswordHint='{\"rejected topology\":\"llllllll\",\"suggested topology\":\"ldldlllll\",\"modifications\":[{\"index\":1,\"type\":\"insertion\",\"new value\":\"d\"},{\"index\":3,\"type\":\"substitution\",\"old value\":\"l\",\"new value\":\"d\"}]}';\n"]
  );

  $sArgs = "";

  foreach my $paArguments (@aTuples)
  {
    $sArgs = @{$paArguments}[0];
    $sArgs =~ s/^ */ /g;
    $sArgs =~ s/ +$//g;
    $sArgs =~ s/'/"/g if (GetProperties()->{'OsClass'} eq "WINX");

    $sCommandLine = $sCommand . $sSeedArg . $sArgs;

    @aOutput = qx($sCommandLine);

    ok(join("", @aOutput) eq @{$paArguments}[1] , "Test command: \`$sCommandLine\`.");
  }

  #Clean up the used database.
  ok(unlink("$sDatabaseFilename") eq 1, "Cleaning up database for TEST_GROUP 8.");

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

=pod

=head1 TEST_GROUP=9

=head1 TEST_COUNT=7

=head1 TEST_SUMMARY

Hint engine CLI checks on hint info level 3.

=cut

  # Create a fresh database.
  @aOutput = qx($sCreateDbCommandLine);
  ok(join("", @aOutput) eq "", "Creating fresh database for TEST_GROUP 9.");

  @aTuples =
  (
    [" -b -h 3 -d $sDatabaseFilename '' 'mypasswd'", "OldPassword='';\nOldTopology='?z';\nOldId='0';\nNewPassword='mypasswd';\nNewTopology='?l?l?l?l?l?l?l?l';\nNewId='195312';\nPasswordOk='false';\nPasswordHint='The new password failed the blacklist check. Try making a password with this topology: ldldlllll.';\n"],
    [" -b -h 3 -p 1 -d $sDatabaseFilename '' 'mypasswd'", "OldPassword='';\nOldTopology='?z';\nOldId='0';\nNewPassword='mypasswd';\nNewTopology='?l?l?l?l?l?l?l?l';\nNewId='195312';\nPasswordOk='false';\nPasswordHint='The new password failed the blacklist check. Try making a password with this topology: ldldlllll.';\n"],
    [" -b -h 3 -p 2 -d $sDatabaseFilename '' 'mypasswd'", "OldPassword='';\nOldTopology='?z';\nOldId='0';\nNewPassword='mypasswd';\nNewTopology='?l?l?l?l?l?l?l?l';\nNewId='195312';\nPasswordOk='false';\nPasswordHint='\n                                            m ypasswd\n                                             | |     \n  insert a digit ----------------------------+ |     \n                                               |     \n    replace with a digit ----------------------+     \n';\n"],
    [" -b -h 3 -p 3 -d $sDatabaseFilename '' 'mypasswd'", "OldPassword='';\nOldTopology='?z';\nOldId='0';\nNewPassword='mypasswd';\nNewTopology='?l?l?l?l?l?l?l?l';\nNewId='195312';\nPasswordOk='false';\nPasswordHint='\n                                            m ypasswd\n                                             | |     \n  insert a digit ----------------------------+ |     \n                                               |     \n    replace with a digit ----------------------+     \n\n This should produce the following topology: ldldlllll\n';\n"],
    [" -b -h 3 -e 1 -d $sDatabaseFilename '' 'mypasswd'", "OldPassword='';\nOldTopology='?z';\nOldId='0';\nNewPassword='mypasswd';\nNewTopology='?l?l?l?l?l?l?l?l';\nNewId='195312';\nPasswordOk='false';\nPasswordHint='{\"rejected password\":\"mypasswd\",\"suggested topology\":\"ldldlllll\",\"modifications\":[{\"index\":1,\"type\":\"insertion\",\"new value\":\"d\"},{\"index\":3,\"type\":\"substitution\",\"old value\":\"p\",\"new value\":\"d\"}]}';\n"]
  );

  $sArgs = "";

  foreach my $paArguments (@aTuples)
  {
    $sArgs = @{$paArguments}[0];
    $sArgs =~ s/^ */ /g;
    $sArgs =~ s/ +$//g;
    $sArgs =~ s/'/"/g if (GetProperties()->{'OsClass'} eq "WINX");

    $sCommandLine = $sCommand . $sSeedArg . $sArgs;

    @aOutput = qx($sCommandLine);

    ok(join("", @aOutput) eq @{$paArguments}[1] , "Test command: \`$sCommandLine\`.");
  }

  #Clean up the used database.
  ok(unlink("$sDatabaseFilename") eq 1, "Cleaning up database for TEST_GROUP 9.");

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

=pod

=head1 TEST_GROUP=10

=head1 TEST_COUNT=7

=head1 TEST_SUMMARY

Hint engine CLI checks on hint info level 4.

=cut

  # Create a fresh database.
  @aOutput = qx($sCreateDbCommandLine);
  ok(join("", @aOutput) eq "", "Creating fresh database for TEST_GROUP 10.");

  @aTuples =
  (
    [" -b -h 4 -d $sDatabaseFilename '' 'mypasswd'", "OldPassword='';\nOldTopology='?z';\nOldId='0';\nNewPassword='mypasswd';\nNewTopology='?l?l?l?l?l?l?l?l';\nNewId='195312';\nPasswordOk='false';\nPasswordHint='The new password failed the blacklist check. Try making your new password something like this: m3y9asswd.';\n"],
    [" -b -h 4 -p 1 -d $sDatabaseFilename '' 'mypasswd'", "OldPassword='';\nOldTopology='?z';\nOldId='0';\nNewPassword='mypasswd';\nNewTopology='?l?l?l?l?l?l?l?l';\nNewId='195312';\nPasswordOk='false';\nPasswordHint='The new password failed the blacklist check. Try making your new password something like this: m3y9asswd.';\n"],
    [" -b -h 4 -p 2 -d $sDatabaseFilename '' 'mypasswd'", "OldPassword='';\nOldTopology='?z';\nOldId='0';\nNewPassword='mypasswd';\nNewTopology='?l?l?l?l?l?l?l?l';\nNewId='195312';\nPasswordOk='false';\nPasswordHint='\n                        m ypasswd\n                         | |     \n  insert a '3' ----------+ |     \n                           |     \n    replace with '9' ------+     \n';\n"],
    [" -b -h 4 -p 3 -d $sDatabaseFilename '' 'mypasswd'", "OldPassword='';\nOldTopology='?z';\nOldId='0';\nNewPassword='mypasswd';\nNewTopology='?l?l?l?l?l?l?l?l';\nNewId='195312';\nPasswordOk='false';\nPasswordHint='\n                        m ypasswd\n                         | |     \n  insert a '3' ----------+ |     \n                           |     \n    replace with '9' ------+     \n\n This should produce the following password: m3y9asswd\n';\n"],
    [" -b -h 4 -e 1 -d $sDatabaseFilename '' 'mypasswd'", "OldPassword='';\nOldTopology='?z';\nOldId='0';\nNewPassword='mypasswd';\nNewTopology='?l?l?l?l?l?l?l?l';\nNewId='195312';\nPasswordOk='false';\nPasswordHint='{\"rejected password\":\"mypasswd\",\"suggested password\":\"m3y9asswd\",\"modifications\":[{\"index\":1,\"type\":\"insertion\",\"new value\":\"3\"},{\"index\":3,\"type\":\"substitution\",\"old value\":\"p\",\"new value\":\"9\"}]}';\n"]
  );

  $sArgs = "";

  foreach my $paArguments (@aTuples)
  {
    $sArgs = @{$paArguments}[0];
    $sArgs =~ s/^ */ /g;
    $sArgs =~ s/ +$//g;
    $sArgs =~ s/'/"/g if (GetProperties()->{'OsClass'} eq "WINX");

    $sCommandLine = $sCommand . $sSeedArg . $sArgs;

    @aOutput = qx($sCommandLine);

    ok(join("", @aOutput) eq @{$paArguments}[1] , "Test command: \`$sCommandLine\`.");
  }

  #Clean up the used database.
  ok(unlink("$sDatabaseFilename") eq 1, "Cleaning up database for TEST_GROUP 10.");

