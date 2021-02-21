# libduco

libduco is an easy-to-use C interface to the [Duinocoin](https://github.com/revoxhere/duino-coin) API. It's written in pure C89 for maximum compatibility! Currently, it's only tested on Linux, but it will be updated soon to also run flawlessly on Windows. If you compile with MinGW, it *should* work already.

## License

libduco, Copyright (c) 2021 Sarah Klocke

This program is free software. It comes without any warranty, to the extent permitted by applicable law. You can redistribute it and/or modify it under the terms of the Do What The Fuck You Want To Public License, Version 2, as published by Sam Hocevar. See the LICENSE.md file for more details.

## Installing

Installing is super easy!

```bash
git clone https://github.com/SarahIsWeird/libduco
make
sudo make install
```

This will install libduco to ``/usr/lib/libduco.so`` and ``/usr/include/duco.h``. If you want to uninstall libduco, just run this:

```bash
sudo make uninstall
```

# Features

This is a list of the current implementation status of various features.

- [x] Account-related features
  - [x] Logging in (``duco_login()``)
  - [x] Registering a new user (``duco_register()``)
  - [x] Changing the password (``duco_change_password()``)
- [ ] Balance-related features
  - [x] Getting the current balance (``duco_get_balance()``)
  - [x] Sending Duinocoin to another user (``duco_send_balance()``)
  - [ ] Wrapping Duinocoin on TRON
  - [ ] Unwrapping Duinocoin from TRON
- [x] Other features
  - [x] Getting a list of past transactions

## Usage example

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