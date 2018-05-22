This library can be used to integrate GNU Social with other applications. It is based upon the functions from [GnuSocialShell](https://github.com/dalmemail/GnuSocialShell) by Daniel Martin.

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
    gnusocial_account_t acc;
    gnusocial_account_info_t info;
    int result = 1;

    gs_init_account(&acc, "https", "username", "domain_name", "password", NULL);
    if (gs_verify_account(acc) == -1) return 1;

    info = gs_get_my_account_info(acc, &result);
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
    gnusocial_account_t acc;

    gs_init_account(&acc, "https", "username", "domain_name", "password", NULL);
    if (gs_verify_account(acc) == -1) return 1;
    gs_send_status(acc, "g'day world!");

    return 0;
}
```

To read the last few posts in your home timeline. You can also use the constant *PUBLIC_TIMELINE* to show all posts from the server rather than just the ones associated with your account.

``` C
#include <stdio.h>
#include <gnusocial.h>

int main(int argc, char **argv)
{
    gnusocial_account_t acc;
    gnusocial_status_t * posts;
    const int no_of_posts = 5;
    int i;

    gs_init_account(&acc, "https", "username", "domain_name", "password", NULL);
    if (gs_verify_account(acc) == -1) return 1;
    posts = gs_read_timeline(acc, HOME_TIMELINE, no_of_posts);
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
    gnusocial_account_t acc;

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

    gs_init_account(&acc, "https", argv[1], argv[2], argv[3], NULL);
    if (gs_verify_account(acc) == -1) return 4;

    char count[32];
    int n_users = 9999;
    snprintf(count, 32, "count=%d", n_users);
    char *xml_data = gs_send_to_api(acc,count,"statuses/friends.xml");
    int xml_data_size = strlen(xml_data);
    char error[512];
    if (gs_parseXml(xml_data, xml_data_size, "<error>", 7, error, 512) > 0) {
        printf("Error: %s\n", error);
    }
    else if (xml_data_size > 0) {
        char name[64];
        char screen_name[64];
        char url[128];
        int start_status_point = 0;
        int real_status_point = 0;
        char *array_data;
        array_data = &xml_data[0];
        int i;
        for (i = 0; i < n_users && (real_status_point+13) < xml_data_size; i++) {
            gs_parseXml(array_data, (xml_data_size-real_status_point), "<name>", 6, name, 64);
            gs_parseXml(array_data, (xml_data_size-real_status_point), "<screen_name>", 13, screen_name, 64);
            gs_parseXml(array_data, (xml_data_size-real_status_point), "<ostatus_uri>", 13, url, 128);
            start_status_point = gs_parseXml(array_data, (xml_data_size-real_status_point), "</user>", 7, "", 0);
            printf("%s,%s,%s\n", name, screen_name, url);
            real_status_point += start_status_point;
            array_data = &xml_data[real_status_point];
        }
    }
    else {
        printf("Error: Reading %d users from '%s://%s/api/%s'\n", n_users, acc.protocol, acc.server, source);
    }
    free(xml_data);

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
