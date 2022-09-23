# CLib

CLib is a header-only library for C99 that implements the most important classes
from GLib: **GList**, **GHashTable** and **GString**.

GLib is a great library that provides some classes that are so useful that you
want to use them in every C project. For some of them you might even wish they
were part of the C standard libraray.

Unfortunately, GLib is a bit too heavy for many projects. An additional problem
that prevents its wide-spread use is its license: Many people don't want to use
LGPL licensed code or want to link everything statically which is in many cases
prevented by the LGPL.

Therefore CLib implements the APIs of the most important classes as header-only
libs under the MIT license.

## Usage

Pick the header files of the classes you want to use from *src/*, copy them to
a location in your project where header files are found by the compiler, and
include them in your C project files.

When you include a class for the first time in a compilation unit you need to
define *_CLIB_IMPL* to tell the preprocessor that you want the declarations
AND the implementations of the functions:

```c
#define _CLIB_IMPL 1
#include <gstring.h>
```

Whenever you use the same library again in the same compilation unit you only
need to include the header file:

```c
#include <gstring.h>
```

## Building the Tests

Since this library is header-only the only purpose of the build system is to
build the tests.

The tests can be built with the following commands on Linux and macOS:

```bash
mkdir build
cd build
conan install .. --build=missing
cmake .. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug
make
```

And on Windows with Visual Studio 2022:

```bash
md build
cd build
conan install .. --build=missing
cmake .. -G "Visual Studio 17"
cmake --build . --config Debug
```

You can create a release build an Linux and macOS with:

```bash
cmake .. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
make
```

And on Windows with:

```bash
cmake --build . --config Release
```

When creating a debug build you can also use the following options to enable a
sanitizer:

|Name | Description | Default |
|:---:|:---|:---:|
| ENABLE_ADDRESS_SANITIZER | Build with address sanitizer | OFF |
| ENABLE_MEMORY_SANITIZER | Build with memory sanitizer | OFF |
| ENABLE_LEAK_SANITIZER | Build with leak sanitizer | OFF |

Sanitizers cost performance and some are incompatible with each other so can only
use one sanitizer for any build.

For most development purposes it is best to create a debug build and enable
address sanitizer:

```bash
cmake .. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DENABLE_ADDRESS_SANITIZER=ON
```
