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

## Building the Tests

Since this library is header-only the only purpose of the build system is to
build the tests.

The tests can be built with the following commands:

```bash
cmake .
make
```

By default the build system creates debug binaries.

You can create a release build with:

```bash
cmake -DCMAKE_BUILD_TYPE=Release .
make
```

When creating a debug build you can also use the following options to enable a
sanitizer:

|Name | Description | Default |
|:---:|:---|:---:|
| ENABLE_ADDRESS_SANITIZER | Build with address sanitizer | OFF |
| ENABLE_MEMORY_SANITIZER | Build with memory sanitizer | OFF |

Address sanitizer and memory sanitzier are not compatible with each other so you
can only use one of both for any build.

For most development purposes it is best to create a debug build and enable
address sanitizer:

```bash
cmake -DENABLE_ADDRESS_SANITIZER=ON .
make
```
