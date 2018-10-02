# otp [![Build Status](https://travis-ci.org/btroller/One-Time-Pad.svg?branch=master)](https://travis-ci.org/btroller/One-Time-Pad)

## Introduction
`otp` is a simple C-based tool for pseudorandomly generating [one-time pads](https://en.wikipedia.org/wiki/One-time_pad), as well as encrypting and decrypting data using them.

### Disclaimer
**Don't use this tool for anything serious.** I'm no expert in computer security or cryptography. The operation of adding and subtracting bytes to encrypt and decrypt from pads is simple enough, but still could be flawed. This tool **definitley** doesn't generate truly random pads.

## Use
To use `otp`, invoke it with one of the following formats:

**Note:** If not given, `infile` and `outfile` default to `stdin` and `stdout`, respectively.

``otp` -e [-i infile] [-o outfile] -p padfile` to encrypt from `infile` to `outfile` using `padfile`;

``otp` -d [-i infile] [-o outfile] -p padfile` to decrypt from `infile` to `outfile` using `padfile`;

``otp` -g -p padname [padsize]` to generate a padfile named `padname` of size `padsize`, where `padsize` is in bytes.

## Implementation
To generate pads, `otp` gets data from the Linux- and macOS-provided function/syscall `getentropy()` and writes it directly to a file of name `padname`. If `padsize` is not given, a default size of 1 KiB is used.

To encrypt data, `otp` XORs each input byte to its corresponding byte in the pad file and writes the result to the output file. If the given pad file is not as long as the input, a warning about perfect secrecy is printed to `stderr` and the pad is used again from its beginning.

To decrypt a file, `otp` reverses the encryption process by XORing the corresponding bytes in the pad and the encrypted file, then writing the result to the output file. If the end of the pad is reached before the output file is fully decrypted, the pad is reused from its beginning.

**Note:** Though `otp` can take input from `stdin` and write output to `stdout`, it is not intended for use as a tool to encrypt a line at a time by hand -- its `stdin`/`stdout` support is only intended to be used for redirection. The results of encryption would most likely be useless, as unprintable characters will display as '?' in a terminal, not revealing an encrypted byte's value.

## Requirements

* Support for `getentropy()` as defined in `sys/random.h`, which was introduced in glibc 2.25 during February of 2017.
* `gcc`/`clang` compatable C compiler
* `make`

## Installation
To compile `otp`, clone this repository, change to its directory, and use `make` to compile.
