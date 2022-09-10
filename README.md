# Clib

Clib is a header-only library for C that implements the most important classes
from Glib: GList, GHashTable and GString.

Glib is a great library that provides some classes that are so useful that you
want to use them in every C project. For some of them you might even wish they
were part of the C standard libraray. Unfortunately, GLib is a bit too heavy
for many projects. An additional problem that prevents its wide-spread use is
its license: Many people don't want to use LGPL licensed code or want to link
everything statically which is in many cases prevented by the LGPL.

Therefore CLib implements the APIs of the most important classes as header-only
libs under the MIT license.
