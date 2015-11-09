IF (NOT CXX11_FLAGS OR NOT CXX11_FOUND)
include(CheckCXXSourceCompiles)
include(FindPackageHandleStandardArgs)

set(CXX11_FLAG_CANDIDATES
    #Gnu and Intel Linux
    "-std=c++11"
    #Microsoft Visual Studio, and everything that automatically accepts C++11
    " "
    #Intel windows
    "/Qstd=c++0x"
    )

set(CXX11_TEST_SOURCE
"
#include <algorithm>
#include <utility>
#include <memory>

class Matrix {
public:
    // Initializer lists
    Matrix(int a, int b, int c, int d)
        : data {a, b, c, d}
    {
        // Lambda functions
        std::transform(data, data+4, data,
                [](int x) { return x+1; });
    }

    // rvalue references
    Matrix(Matrix &&rhs) {
    }

private:
    int data[4];
};

int main()
{
    int n[] {4,7,6,1,2};
    // auto
    for (auto i : n) {
        Matrix mat (3,5,1,2);

        // std::move, unique_ptr
        std::unique_ptr<Matrix> m2(
        	new Matrix(std::move(mat)));
    }
    return 0;
}
")

foreach(FLAG ${CXX11_FLAG_CANDIDATES})
    set(SAFE_CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS}")
    set(CMAKE_REQUIRED_FLAGS "${FLAG}")
    unset(CXX11_FLAG_DETECTED CACHE)

    check_cxx_source_compiles("${CXX11_TEST_SOURCE}" CXX11_FLAG_DETECTED)
    set(CMAKE_REQUIRED_FLAGS "${SAFE_CMAKE_REQUIRED_FLAGS}")
    if(CXX11_FLAG_DETECTED)
        set(CXX11_FLAGS_INTERNAL "${FLAG}")
        break()
    endif(CXX11_FLAG_DETECTED)
endforeach(FLAG ${CXX11_FLAG_CANDIDATES})

set(CXX11_FLAGS "${CXX11_FLAGS_INTERNAL}")

find_package_handle_standard_args(CXX11 DEFAULT_MSG CXX11_FLAGS)
mark_as_advanced(CXX11_FLAGS)
ENDIF()