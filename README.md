# shorklocomotive

My cute, shark-based take on [Toyoda Masashi's sl](https://github.com/mtoyoda/sl). The original sl is a command that kindly pokes fun at making typos when trying to type `ls`, cures "your bad habit of mistyping" as Masashi puts it. shorklocomotive (sl) pays homage to the original, but thematically made for SHORK Operating Systems like [SHORK 486](https://github.com/SharktasticA/SHORK-486), even using SHORK's ASCII art mascot. It also works on modern Linux distributions just fine.


## Planned features

* Custom colours
* Different shark species
* Some sort of shark-themed/based train to pay closer homage to the original


## Building

### Requirements

You just need a C compiler (tested with GCC with either glibc or musl).

### Compilation

Simply run `make` to compile shorklocomotive.

### Installation

Run `make install` to install to `/usr/bin` (you may need `sudo` if not installing as root). If you want to install it elsewhere, you can override the install location prefix like `make PREFIX=/usr/local install`.

**Note:** If you have a package maintainer's version of the original sl installed, or have compiled and installed the original sl yourself, this may conflict and/or overwrite it.



## Running

Simply run `sl`.
