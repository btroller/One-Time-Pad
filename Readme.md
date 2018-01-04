# OTP

## Introduction
OTP is a simple C-based tool for pseudorandomly generating [one-time pads](https://en.wikipedia.org/wiki/One-time_pad), as well as encrypting and decrypting data using them.

## Use
To use OTP, invoke it with one of the following formats:

**Note:** If not given, `infile` and `outfile` default to `stdin` and `stdout`, respectively.

`otp -e [-i infile] [-o outfile] -p padfile` to encrypt from `infile` to `outfile` using `padfile`;

`otp -d [-i infile] [-o outfile] -p padfile` to decrypt from `infile` to `outfile` using `padfile`;

`otp -g -p padname [padsize]` to generate a padfile named `padname` of size `padsize`, where `padsize` is in bytes.

## Implementation
To generate pads, OTP reads `padsize` bytes from `/dev/random` and writes them directly to a file of name `padname`. If `padsize` is not given, a default size of 1 KiB is used.

To encrypt data, OTP adds each input byte to its corresponding byte in the pad file and writes the result to the output file. If the given pad file is not as long as the input, a warning about perfect secrecy is printed to `stderr` and the pad is used again from its beginning.

To decrypt a file, OTP reverses the encryption process by subtracting the corresponding byte in the pad from the encrypted file and writing to the output file. If the end of the pad is reached before the output file is fully decrypted, the pad is reused from its beginning.

**Note:** Though OTP can take input from `stdin` and write output to `stdout`, it is not intended for use as a tool to encrypt a line at a time by hand -- its `stdin`/`stdout` support is only intended to be used for redirection. The results of encryption would most likely be useless, as unprintable characters will display as '?' in a terminal, not revealing an encrypted byte's value.

## Installation
To install OTP, clone this repository, change to its directory, and use `make` to compile. 
