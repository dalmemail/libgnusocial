/*
 * Copyright (C) 2016 Dan Rulos.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "gnusocial.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int loglevel=LOG_NONE;

int parseXml(char *xml_data, int xml_data_size, char *tofind, int tofind_size, char *output, int output_size)
{
    int pos = 0;
    int act_pos = 0;
    int start_pos = 0;
    int ret = -1;
    int i;
    memset(output, '\0', output_size);
    for (i = pos; act_pos != tofind_size && i < xml_data_size; i++) {
        if (tofind[act_pos] == xml_data[i]) {
            act_pos++;
            if (act_pos == tofind_size) {
                pos = i;
                ret = 0;
            }
        }
        else {
            act_pos = 0;
        }
    }
    start_pos = pos+1;
    if (ret == 0) {
        for (i = 0; (xml_data[start_pos+i] != '<' ||xml_data[start_pos+i+1] != '/') && i < output_size; i++) {
            output[i] = xml_data[start_pos+i];
        }
        ret = (start_pos+i);
    }
    return ret;
}

struct gnusocial_status makeStatusFromRawSource(char *raw_data, int data_size)
{
    struct gnusocial_status out_status;
    char buffer[16];
    parseXml(raw_data, data_size, "<text>", 6, out_status.text, 1024);
    parseXml(raw_data, data_size, "<id>", 4, buffer, 16);
    out_status.id = atoi(buffer);
    parseXml(raw_data, data_size, "<screen_name>", 13, out_status.author_screen_name, 64);
    parseXml(raw_data, data_size, "<in_reply_to_status_id>", 23, buffer, 16);
    out_status.in_reply_to_id = atoi(buffer);
    parseXml(raw_data, data_size, "<in_reply_to_screen_name>", 25, out_status.in_reply_to_user, 64);
    parseXml(raw_data, data_size, "<created_at>", 12, out_status.date, 64);
    return out_status;
}

void print_status(struct gnusocial_status status_)
{
    printf("\033[36m@%s (ID %d)", status_.author_screen_name, status_.id);
    if (status_.in_reply_to_user[0] != '\0') {
        printf(" → @%s (ID %d)", status_.in_reply_to_user, status_.in_reply_to_id);
    }
    printf("\033[32m\n%s\n", status_.text);
    printf("\033[33m%s\n\033[m", status_.date);
}

int FindXmlError(char *xml_data, int xml_data_size)
{
    int ret = 0;
    char error[512];
    if ((ret = parseXml(xml_data, xml_data_size, "<error>", 7, error, 512)) > 0) {
        printf("Error: %s\n", error);
    }
    return ret;
}

int get_number_of_groups(struct gnusocial_gss_account account)
{
    char source[128];
    snprintf(source, 128, "&screen_name=%s", account.user);
    char *xml_data = send_to_api(account, source, "users/show.xml");
    char error[512];
    char n_groups[32] = "0";
    int xml_data_size = strlen(xml_data);
    if (parseXml(xml_data, xml_data_size, "<error>", 7, error, 512) > 0) {
        printf("Error: %s\n", error);
    }
    else {
        parseXml(xml_data, xml_data_size, "<groups_count>", 14, n_groups, 32);
    }
    return atoi(n_groups);
}

void init_account(struct gnusocial_gss_account * acc, char * protocol,
                  char * user, char * server, char * password)
{
    sprintf(acc->protocol, "%s", protocol);
    sprintf(acc->user, "%s", user);
    sprintf(acc->server, "%s", server);
    sprintf(acc->password, "%s", password);
}

void print_user_info(struct gnusocial_account_info info)
{
    printf("\033[31mName: %s\n\033[32mUser: %s\n\033[33mLocation: %s\n\033[34mDescription: %s\n\033[35mUrl: %s\n",
    info.name, info.screen_name, info.location, info.description, info.url);
    if (info.followers > 0) {
        printf("\033[36mFollowers: %d\n\033[m", info.followers);
    }
    else {
        printf("\033[36mFollowers: ?\n\033[m");
    }
    if (info.friends > 0) {
        printf("Friends: %d\n", info.friends);
    }
    else {
        printf("Friends: ?\n");
    }
    if (info.statuses > 0) {
        printf("Status: %d\n", info.statuses);
    }
    else {
        printf("Status: ?\n");
    }
}

void print_group_info(struct gnusocial_group_info group)
{
    printf("\033[31mNickname: %s\n\033[32mFullname: %s\n\033[33mDescription: %s\n\033[34mUrl: %s\n", group.nickname,
    group.fullname, group.description, group.url);
    if (group.member) {
        printf("\033[35mYou are member of this group\n");
    }
    else {
        printf("\033[35mYou aren't member of this group\n");
    }
    if (group.admins >= 0) {
        printf("\033[36mAdmins: %d\n", group.admins);
    }
    else {
        printf("\033[36Admins: ?\n");
    }
    if (group.members >= 0) {
        printf("\033[mMembers: %d\n", group.members);
    }
    else {
        printf("\033[mMembers: ?\n");
    }
}

void print_little_group_info(struct gnusocial_little_group_info group)
{
    printf("\033[36m!%s\033[31m (ID %d)\n\033[32m%s\n\033[m", group.nickname, group.id, group.description);
}

void help_command(char *args)
{
    if (args == NULL) {
        printf("/help\t\t\t\tPrints this help\n");
        printf("/quit\t\t\t\tClose GnuSocialShell\n");
        printf("/me\t\t\t\tPrints account information\n");
        printf("/send [MESSAGE]\t\t\tSend MESSAGE to GnuSocial\n");
        printf("/favorite [ID]\t\t\tFavorites the status ID\n");
        printf("/unfavorite [ID]\t\tDelete the status ID from the favorites list\n");
        printf("/search [ID]\t\t\tFind one status using its ID\n");
        printf("/delete [ID]\t\t\tDelete one status using its ID\n");
        printf("/reply [ID] [MESSAGE]\t\tSend MESSAGE as a reply of status ID\n");
        printf("/favorites [NUMBER]\t\tDownloads 'NUMBER' favorites from favorites list\n");
        printf("/mentions [NUMBER]\t\tDownloads 'NUMBER' mentions\n");
        printf("/home_timeline [NUMBER]\t\tDownloads 'NUMBER' status from Home Timeline\n");
        printf("/ht [NUMBER]\t\t\tThe same that /home_timeline\n");
        printf("/public_timeline [NUMBER]\tDownloads 'NUMBER' status from Public Timeline\n");
        printf("/pt [NUMBER]\t\t\tThe same that /public_timeline\n");
        printf("/rt [ID]\t\t\tRepeat the notice 'ID'\n");
        printf("/user_info [ID]\t\t\tPrints user detailed information using its ID\n");
        printf("/ui [SCREEN_NAME]\t\tPrints user detailes information using its screen_name\n");
        printf("/followers_list\t\t\tPrints the people who is following you\n");
        printf("/friends_list\t\t\tPrints the people you are following\n");
        printf("/group\t\t\t\tThe same that '/help group'\n");
        printf("/groups\t\t\t\tPrints information about the groups in which you are\n");
        printf("/start_follow [SCREEN_NAME]\tFollow one user using its SCREEN_NAME\n");
        printf("/sf [SCREEN_NAME]\t\tThe same that /start_follow\n");
        printf("/stop_follow [SCREEN_NAME]\tUn-Follow one user using its SCREEN_NAME\n");
        //printf("/notifications\t\t\tPrints latest notifications\n");
    }
    else if (strcmp(args, "group") == 0) {
        printf("/group show [ID]\t\tPrints group detailed information using its ID\n");
        printf("/group join [ID]\t\tSubscrite to the given group\n");
        printf("/group leave [ID]\t\tLeave the given group\n");
        printf("/group list [NUMBER]\t\tPrints detailed information of 'NUMBER' groups\n");
        printf("/group_timeline [ID] [NUMBER]\tDownloads 'NUMBER' status from a group called by it 'ID'\n");
    }
    else {
        printf("No such help for '%s'\n", args);
    }
}
