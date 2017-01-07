# Welcome to libgpio

## General

*libgpio* is an open source helper library providing easy-to-use
functions for accessing GPIO pins on Linux(tm) systems.

## Why make yet another GPIO library?

Yes, there are plenty of libraries like this out there including
[libsoc](https://github.com/jackmitch/libsoc) which is very nice.
The reason for publishing this library is that it is smaller than
libsoc and, well, was already written so why not publish?

## You're using goto statements??  Eww...

See Section 7) of [Linux(tm) kernel's coding style](http://git.kernel.org/cgit/linux/kernel/git/torvalds/linux.git/tree/Documentation/process/coding-style.rst).

## Want to contribute?

Take a look at the [CONTRIBUTING file](https://github.com/animalcreek/libgpio/blob/master/CONTRIBUTING.md)

## Source code

*libgpio* source is available [here](https://github.com/animalcreek/libgpio).

## Source code documentation

*libgpio* source documentation is available
[here](https://animalcreek.github.io/libgpio)

## Building

This repository uses code from the
[animalcreek/libfile](https://github.com/animalcreek/libfile)
git repository.  To include that code, this repository adds libfile as a
[git submodule](https://git-scm.com/book/en/Git-Tools-Submodules).
One of the odd things about git submodules is that the contents of
the submodule are not downloaded when the parent repository is cloned.
To make the code appear in the parent repository, the person cloning
must first initialize the submodule and then update it to download the
contents of the submodule.  This can be done with the following git
commands:
```shell-script
	$ git submodule init
	$ git submodule update
```
The 'init' command need only be done once and the 'update' need only be
done when the contents of the submodule are to be updated.  For convenience
the Makefile contains rules to perform those tasks:
```shell-script
	$ make init_submodules
	$ make update_submodules
```
Once you have the source as you want it, you can build it with the following
command:
```shell-script
	$ make
```
The object files created will be 'gpio.o' and 'libfile/file.o'.
The object files can be removed with this command:
```shell-script
	$ make clean
```

## Reducing build noise
By default, the command line used to build the source will be printed
for each file (e.g., cc -c gpio.c -o gpio.o) along with a summary line
(e.g., CC    gpio.c).  Printing the command line can be stopped by setting
the make variable 'Q' to '@' (e.g., make Q="@").  

The idea behind this is similar to what the Linux(tm) kernel's main
[Makefile does](https://git.kernel.org/cgit/linux/kernel/git/torvalds/linux.git/tree/Makefile#n69)
