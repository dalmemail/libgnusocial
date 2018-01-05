This library can be used to integrate GNU Social with other applications. It is based upon the functions from [GnuSocialShell](https://github.com/dalmemail/GnuSocialShell) by Dan Rulos.

# Installation

On Debian based systems:

``` bash
sudo apt-get install curl libcurl4-openssl-dev
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

To show your profile information:

``` C
#include <stdio.h>
#include <gnusocial.h>

int main(int argc, char **argv)
{
    struct gnusocial_gss_account acc;
    struct gnusocial_account_info info;
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

To publish a post:

``` C
#include <stdio.h>
#include <gnusocial.h>

int main(int argc, char **argv)
{
    struct gnusocial_gss_account acc;

    init_account(&acc, "https", "username", "domain_name", "password");
    if (verify_account(acc) == -1) return 1;
    send_status(acc, "g'day world!");

    return 0;
}
```

To read the last few posts in your home timeline. You can also use the constant *PUBLIC_TIMELINE* to show all posts from the server rather than just the ones associated with your account.

``` C
#include <stdio.h>
#include <gnusocial.h>

int main(int argc, char **argv)
{
    struct gnusocial_gss_account acc;
    struct gnusocial_status * posts;
    const int no_of_posts = 5;
    int i;

    init_account(&acc, "https", "username", "domain_name", "password");
    if (verify_account(acc) == -1) return 1;
    posts = read_timeline(acc, HOME_TIMELINE, no_of_posts);
    for (i = 0; i < no_of_posts; i++) {
        printf("[ID: %d] [%s] [%s] %s\n",
               posts[i].id, posts[i].date,
               posts[i].author_screen_name, (char*)posts[i].text);
    }

    return 0;
}
```

To list your friend subscriptions:

``` C
#include <stdio.h>
#include <gnusocial.h>

int main(int argc, char **argv)
{
    struct gnusocial_gss_account acc;

    if (argc < 2) {
        printf("No username given\n");
        return 1;
    }

    if (argc < 3) {
        printf("No gnusocial domain name given\n");
        return 2;
    }

    if (argc < 4) {
        printf("No gnusocial password given\n");
        return 3;
    }

    init_account(&acc, "https", argv[1], argv[2], argv[3]);
    if (verify_account(acc) == -1) return 4;
    print_users_array_info(acc, "statuses/friends.xml", 9999);

    return 0;
}
```

Compile with:

``` bash
gcc -Wall -std=gnu99 -pedantic -O3 -o test *.c -I/usr/local/include/libgnusocial -lcurl -lgnusocial
```

For a complete list of the functions available see the manpage.

``` bash
man libgnusocial
```
