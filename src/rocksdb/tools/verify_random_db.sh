#!/bin/bash
#
# A shell script to verify DB generated by generate_random_db.sh cannot opened and read correct data.
# ./ldb needs to be avaible to be executed.
#
# Usage: <SCRIPT> <DB Path>

#
# The following only applies to changes made to this file as part of YugaByte development.
#
# Portions Copyright (c) YugaByte, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except
# in compliance with the License.  You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software distributed under the License
# is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
# or implied.  See the License for the specific language governing permissions and limitations
# under the License.
#
scriptpath=`dirname $BASH_SOURCE`
if [ "$#" -lt 2 ]; then
  echo "usage: $BASH_SOURCE <db_directory> <compare_base_db_directory> [dump_file_name]"
  exit 1
fi

db_dir=$1
base_db_dir=$2
dump_file_name=${3:-"dump_file.txt"}
db_dump=$db_dir"/"$dump_file_name
base_db_dump=$base_db_dir"/"$dump_file_name

set -e
echo == Dumping data from $db_dir to $db_dump
./ldb dump --db=$db_dir > $db_dump

echo == Dumping data from $base_db_dir to $base_db_dump
./ldb dump --db=$base_db_dir > $base_db_dump

diff $db_dump $base_db_dir
