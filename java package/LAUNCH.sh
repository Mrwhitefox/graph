#!/bin/bash

NORMAL="\\033[0;39m"  # reset
RED="\\033[1;31m"

# compilation
COMPIL="javac -cp . com/github/vmarquet/graphJavaPackage/test/Test100.java"
echo -e "${RED}$COMPIL${NORMAL}"
eval "$COMPIL"

# execution
EXEC="java -cp . com.github.vmarquet.graphJavaPackage.test.Test100"
echo -e "${RED}$EXEC${NORMAL}"
eval "$EXEC"

exit 0
