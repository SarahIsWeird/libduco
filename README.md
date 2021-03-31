# libduco

libduco is an easy-to-use C interface to the [Duinocoin](https://github.com/revoxhere/duino-coin) API. It's written in pure C89 for maximum compatibility! libduco makes use of native Windows/Linux API calls. Less dependencies, more speed!

Wrapping DUCO isn't supported, and it will never be, unless there is high demand for it.

## License

libduco, Copyright (c) 2021 Sarah Klocke

This program is free software. It comes without any warranty, to the extent permitted by applicable law. You can redistribute it and/or modify it under the terms of the Do What The Fuck You Want To Public License, Version 2, as published by Sam Hocevar. See the [LICENSE.md](LICENSE.md) file for more details.

## Prebuilt binaries

Please check the [Releases page](https://github.com/SarahIsWeird/libduco/releases) for prebuilt binaries. There are Windows and Linux binaries available.

## Building from source

**Prerequisites:**

- a C compiler (gcc/clang preferred, MSVC should work too)
- make

Building should be simple.

```bash
git clone github.com/SarahIsWeird/libduco.git
make
```

## Installing (Linux only)

There are installation/uninstallation targets available.

```bash
sudo make install
```

This will install libduco to ``/usr/lib/libduco.so`` and ``/usr/include/duco.h``. If you want to uninstall libduco, just run this:

```bash
sudo make uninstall
```

# Features

This is a list of the current implementation status of various features. For more information about any specific function, please check the docstrings either through your IDE or in the header file.

- [x] Account-related features
  - [x] Logging in (``duco_login()``)
  - [x] Registering a new user (``duco_register()``)
  - [x] Changing the password (``duco_change_password()``)
- [x] Balance-related features
  - [x] Getting the current balance (``duco_get_balance()``)
  - [x] Sending Duinocoin to another user (``duco_send_balance()``)
- [x] Other features
  - [x] Getting a list of past transactions (``duco_get_transactions()`` or ``duco_get_transactions_from()``)
  - [x] Native Windows support

## Usage example

This example program can be compiled with ``gcc -o example example.c -lduco``.

```c
#include <stdio.h>
#include <duco.h>

int main(int argc, char** argv) {
    duco_connection_t* conn = duco_connect(DUCO_POOL_ADDR, DUCO_POOL_PORT);

    duco_login(conn, "user", "123");

    double balance = duco_get_balance(conn);

    if (balance == -1) {
        duco_perror("Couldn't get the balance");
        return 1;
    }

    printf("%s's current balance: %f", conn->username, balance);

    duco_disconnect(conn);

    return 0;
}
```
