# Gossip

## Introduction

`gossip` is a very minimal tool to parse and collect data from
`/proc/$PID/smaps_roolup` into a CSV file. The goal of the tool is to
have minimal dependencies -- currently only libstdc++ is needed --
while allowing the user to collect data in a format that's easy to
post-process with a spreadsheet or Jupyter Notebooks.

## Building

Minimal introductions for building the tool in both supported modes
follow.

### Compiling for the current Host environment

```
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
