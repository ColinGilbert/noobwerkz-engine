#!/bin/bash

COMPILER=$1
LANGUAGE=$2

# Exit script on any error
set -e 

OPTIONS=""
BUILDPATH=""

if [ "$COMPILER" == "gcc" ]; then
  echo "Building with GCC";
  BUILDPATH="gcc"
elif [ "$COMPILER" == "clang" ]; then

  OPTIONS="$OPTIONS -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang"
  echo "Building with CLANG";
  BUILDPATH="clang"  
fi  

if [ "$LANGUAGE" == "C++98" ]; then
  echo "Building with C++98";
  BUILDPATH="$BUILDPATH-cpp98"
elif [ "$LANGUAGE" == "C++11" ]; then
  echo "Building with C++11";
  OPTIONS="$OPTIONS -DCMAKE_CXX_FLAGS='-std=c++11' "
  BUILDPATH="$BUILDPATH-cpp11"  
fi  

#=====================================
# Color Settings:
#=====================================
NC='\033[0m'
OUTPUT='\033[0;32m'


echo -e "${OUTPUT}"
echo ""
echo "======================================================================"
echo "Building Release version with vectorchecks enabled"
echo "======================================================================"
echo -e "${NC}"


if [ ! -d build-release-$BUILDPATH-Vector-Checks ]; then
  mkdir build-release-$BUILDPATH-Vector-Checks
fi

cd build-release-$BUILDPATH-Vector-Checks

cmake -DCMAKE_BUILD_TYPE=Release -DOPENMESH_BUILD_UNIT_TESTS=TRUE -DSTL_VECTOR_CHECKS=ON -DOPENMESH_BUILD_PYTHON_BINDINGS=OFF $OPTIONS ../

#build it
make

#build the unit tests
make unittests

echo -e "${OUTPUT}"
echo ""
echo "======================================================================"
echo "Running unittests Release version with vectorchecks enabled"
echo "======================================================================"
echo -e "${NC}"

cd Unittests

#execute tests
./unittests --gtest_color=yes --gtest_output=xml

cd ..
cd ..

echo -e "${OUTPUT}"
echo ""
echo "======================================================================"
echo "Building Release version with vectorchecks disabled for python tests"
echo "======================================================================"
echo -e "${NC}"

if [ ! -d build-release-$BUILDPATH ]; then
  mkdir build-release-$BUILDPATH
fi

cd build-release-$BUILDPATH

cmake -DCMAKE_BUILD_TYPE=Release -DOPENMESH_BUILD_PYTHON_UNIT_TESTS=ON -DBUILD_APPS=OFF $OPTIONS ../

#build it
make

echo -e "${OUTPUT}"
echo ""
echo "======================================================================"
echo "Running Python unittests Release version "
echo "======================================================================"
echo -e "${NC}"


# Execute Python unittests
cd Python-Unittests

python -m unittest discover -v

cd ..
cd ..



echo -e "${OUTPUT}"
echo ""
echo "======================================================================"
echo "Building Debug version with vectorchecks enabled"
echo "======================================================================"
echo -e "${NC}"


if [ ! -d build-debug-$BUILDPATH-Vector-Checks ]; then
  mkdir build-debug-$BUILDPATH-Vector-Checks
fi

cd build-debug-$BUILDPATH-Vector-Checks

cmake -DCMAKE_BUILD_TYPE=Debug -DOPENMESH_BUILD_UNIT_TESTS=TRUE -DSTL_VECTOR_CHECKS=ON -DOPENMESH_BUILD_PYTHON_BINDINGS=OFF $OPTIONS ../

#build it
make

#build the unit tests
make unittests

echo -e "${OUTPUT}"
echo ""
echo "======================================================================"
echo "Running unittests Debug version with vectorchecks enabled"
echo "======================================================================"
echo -e "${NC}"


cd Unittests

#execute tests
./unittests --gtest_color=yes --gtest_output=xml

cd ..
cd ..

echo -e "${OUTPUT}"
echo ""
echo "======================================================================"
echo "Building Debug version with vectorchecks disabled for python tests"
echo "======================================================================"
echo -e "${NC}"

if [ ! -d build-debug-$BUILDPATH ]; then
  mkdir build-debug-$BUILDPATH
fi

cd build-debug-$BUILDPATH

cmake -DCMAKE_BUILD_TYPE=DEBUG -DOPENMESH_BUILD_PYTHON_UNIT_TESTS=ON -DBUILD_APPS=OFF $OPTIONS ../

#build it
make

echo -e "${OUTPUT}"
echo ""
echo "======================================================================"
echo "Running Python unittests Debug version "
echo "======================================================================"
echo -e "${NC}"


# Execute Python unittests
cd Python-Unittests

python -m unittest discover -v
