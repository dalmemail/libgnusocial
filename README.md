This library can be used to integrate GNU Social with other applications.

# Installation

On Debian based systems:

``` bash
sudo apt-get install curl libcurl-dev
```

On Arch/Parabola

``` bash
sudo pacman -S curl
```

Then clone the repo, build and install it.

``` bash
git clone https://github.com/bashrc/libgnusocial
cd libgnusocial
make
sudo make install
```

# Examples

To show your profile information.

``` C
#include <stdio.h>
#include <gnusocial.h>

int main(int argc, char **argv)
{
    struct gss_account acc;
    struct account_info info;
    int result = 1;

    init_account(&acc, "https", "username", "domain_name", "password");
    if (verify_account(acc) == -1) return 1;

    info = get_my_account_info(acc, &result);
    if (!result) {
        printf("Name: %s\nNick: %s\nLocation: %s\nDescription: %s\n" \
               "Web: %s\nFollowers: %d\nFriends: %d\n",
               (char*)info.name, (char*)info.screen_name,
               (char*)info.location, (char*)info.description,
               (char*)info.url, info.followers, info.friends);
    }
    return 0;
}
```

Compile with:

``` bash
gcc -Wall -std=gnu99 -pedantic -O3 -o test *.c -I/usr/local/include/libgnusocial -lcurl -lgnusocial
```
