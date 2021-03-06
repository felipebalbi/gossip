![Build](https://github.com/felipebalbi/gossip/workflows/CMake/badge.svg) ![Android](https://github.com/felipebalbi/gossip/workflows/Android/badge.svg)

# Gossip

## Introduction

`gossip` is a very minimal tool to parse and collect data from
`/proc/$PID/smaps_roolup` into a CSV file. The goal of the tool is to
have minimal dependencies -- currently only libstdc++ is needed --
while allowing the user to collect data in a format that's easy to
post-process with a spreadsheet or Jupyter Notebooks.

## Building

Minimal instructions for building the tool in all supported modes
follow.

### Compiling for the current Host environment

```
$ mkdir build
$ cd build
$ cmake ..
$ cmake --build .
```

#### Compiling under guix

For those using the great `guix` package manager -- or perhaps the
full `GuixSD` distribution -- we provide a manifest file names
`guix.scm`. The compilation process starts with creating a pure
environment containing only the required packages:

```
$ guix shell --pure -m guix.scm
$ mkdir build
$ cd build
$ cmake ..
$ cmake --build .
```

### Cross-compiling for AArch64

We also support a cross-compilation mode from Host to AArch64. The
resulting binary will be, currently, statically linked to make sure
the resulting binary can run in varied systems, including Android.

```
$ mkdir build
$ cd build
$ cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/aarch64-toolchain.cmake ..
$ cmake --build .
```

### Cross-compiling for Android

We can use Android NDK to compile `gossip` for Android. Assuming NDK
is installed to `/opt/android-ndk`, make sure the following variables
are set:

```
$ export NDK=/opt/android-ndk ABI=arm64-v8a MINSDKVERSION=23
```

Of course, replace the values to match your target device. After that
we can start the build:

```
$ mkdir build
$ cd build
$ cmake \
    -DCMAKE_TOOLCHAIN_FILE=$NDK/build/cmake/android.toolchain.cmake \
    -DANDROID_PLATFORM=android-$MINSDKVERSION                       \
    -DANDROID_ABI=$ABI                                              \
    -DCMAKE_BUILD_TYPE=Release                                      \
    -DCMAKE_SYSTEM_NAME=Android                                     \
    -DCMAKE_SYSTEM_VERSION=$MINSDKVERSION ..
$ cmake --build .
```

The resulting binary should be pushed to the device using `adb`. Note
that this requires a rooted device with read-write partition that can
take runnable executables.

```
$ adb push src/gossip /path/to/location
```

## Running

After compiling the binary we can run it as follows:

```
$ gossip --interval 30 --num-samples 120 --output output.csv
```

The command above will collect 120 samples with  with a period of 30
seconds. That is to say, at every 30 seconds every `smaps_rollup` will
be parsed and added to the output CSV file for a total of 120
measurements.

One can get help with the `-h` parameter:

```
$ gossip -h
Usage: gossip [options] 

Optional arguments:
-h --help        	shows help message and exits
-v --version     	prints version information and exits
-i --interval    	Sampling interval in seconds [default: 1]
-n --num-samples 	Stop after this many samples [default: 10]
-o --output      	Output file name [default: "output.csv"]
```

## Output Contents

`gossip` will traverse the `/proc` filesystem looking for process
directories. For each of the directories it will read two files:

1. `comm`: contains the process name
2. `smaps_rollup`: contains details regarding process' memory usage

From `comm`, we have a single string to extract with a `read()`;
`smaps_rollup` needs to be parsed. The file format is somewhat like
shown below:

```
556022e84000-7ffd4edcd000 ---p 00000000 00:00 0                          [rollup]
Rss:              400352 kB
Pss:              361496 kB
Pss_Anon:         290092 kB
Pss_File:          71216 kB
Pss_Shmem:           188 kB
Shared_Clean:      44348 kB
Shared_Dirty:        384 kB
Private_Clean:     65528 kB
Private_Dirty:    290092 kB
Referenced:       385692 kB
Anonymous:        290092 kB
LazyFree:              0 kB
AnonHugePages:         0 kB
ShmemPmdMapped:        0 kB
FilePmdMapped:         0 kB
Shared_Hugetlb:        0 kB
Private_Hugetlb:       0 kB
Swap:                  0 kB
SwapPss:               0 kB
Locked:                0 kB
```

The first line contains details of the address space and is
unimportant for `gossip` and merely skipped. For the other lines, each
numeric value is extracted and used to build a line in CSV
format.

Each line in `gossip`'s output contains the total CPU Usage Time, the
number of CPUs, the process `PID`, process name, and each of the
values from `smaps_rollup` in the order they are extracted, followed
by the total scheduled time of the process. Any further processing is
expected to happen after-the-fact. This was deliberate decision to
make sure `gossip` would run quickly and consume very little memory
(currently below 1MiB, most of which comes from `libstdc++` itself).

