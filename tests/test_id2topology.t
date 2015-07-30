#!/bin/sh

FILE="id2topology/test.t.log"
if [ -f "${FILE}" -a -s "${FILE}" ] ; then
  sed '1d' "${FILE}" | egrep '^not ok ' && exit 1
  exit 0
else
  exit 1
fi
