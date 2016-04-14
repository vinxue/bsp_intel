# (c) Copyright 2011-2016 Intel Corporation.
# This file is licensed under the terms provided in the file
# libcc54/LICENSE in this directory or a parent directory

#!/bin/bash

SHARED_DIR='../../../shared'
SHARED_INC=$SHARED_DIR'/dx/include'
SHARED_CODESAFE=$SHARED_DIR'/dx/codesafe/src'

DESTINATION='.'
#mkdir $DESTINATION

cp -a $SHARED_INC $DESTINATION
cp $SHARED_CODESAFE'/'crys/gen/inc/crys_host_rpc_config.h $DESTINATION'/include'
cp ../../kernel/cc_driver/dx_driver_abi.h $DESTINATION'/include'
