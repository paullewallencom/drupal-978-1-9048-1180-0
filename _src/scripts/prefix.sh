#!/bin/sh
# $Id: prefix.sh,v 1.7 2006/03/27 10:28:34 killes Exp $

if [ $# != 2 ]; then
  cat >&2 << EOH
This is Drupal database prefixer.

Usage:
  $0 prefix original_db.sql >prefixed_db.sql

- all tables will prefixed with 'prefix'
EOH

exit 1;
fi

PREFIX=$1;
sed "s/^CREATE TABLE /CREATE TABLE $PREFIX/;
     s/^INSERT INTO /INSERT INTO $PREFIX/;
     s/^REPLACE /REPLACE $PREFIX/;
     s/^ALTER TABLE /ALTER TABLE $PREFIX/;
     s/^CREATE SEQUENCE /CREATE SEQUENCE $PREFIX/;
     s/^ALTER SEQUENCE /ALTER SEQUENCE $PREFIX/;
     s/^CREATE INDEX \(.*\) ON /CREATE INDEX $PREFIX\\1 ON $PREFIX/;
     s/^CREATE UNIQUE INDEX \(.*\) ON /CREATE UNIQUE INDEX $PREFIX\\1 ON $PREFIX/;
     s/^UPDATE \(.*\) SET /UPDATE $PREFIX\\1 SET /;
     s/^DROP TABLE IF EXISTS /DROP TABLE IF EXISTS $PREFIX/;
     " $2

