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

    info = get_my_account_info(main_account, &result);
    if (!result) {
        print_user_info(info);
    }
    return 0;
}
```
