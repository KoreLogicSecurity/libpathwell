#!/bin/sh

FILE="topology2id/test.t.log"
if [ -f "${FILE}" -a -s "${FILE}" ] ; then
  sed '1d' "${FILE}" | egrep '^not ok ' && exit 1
  exit 0
else
  exit 1
fi
