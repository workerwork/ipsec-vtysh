/* Command interpreter routine for virtual terminal [aka TeletYpe]
   Copyright (C) 1997, 98, 99 Kunihiro Ishiguro

   This file is part of GNU Zebra.

   GNU Zebra is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published
   by the Free Software Foundation; either version 2, or (at your
   option) any later version.

   GNU Zebra is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GNU Zebra; see the file COPYING.  If not, write to the
   Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */


#include "command.h"
#include "memory.h"
#include "vtysh.h"
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/time.h>
#include "vtysh_config.h"
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>

// the common node 
struct cmd_node view_node =
{
    VIEW_NODE,
    "%s> ",
};

struct cmd_node enable_node =
{
    ENABLE_NODE,
    "%s# ",
};

struct cmd_node config_node =
{
    CONFIG_NODE,
    "%s\x1b[34m(config)\x1b[0m# ",
    1
};

static struct cmd_node auth_enable_node =
{
    AUTH_ENABLE_NODE,
    "Password: ",
};

struct cmd_node shell_node =
{
	SHELL_NODE,
	"%s\x1b[34m(shell)\x1b[0m# ",
};


struct cmd_node ipsec_node =
{
	IPSEC_NODE,
 	"%s\x1b[34m(ipsec)\x1b[0m# ",
	1	
};

struct cmd_node ipsec_s_node =
{
	IPSEC_S_NODE,
	"%s\x1b[34m(%s)\x1b[0m# ",
	1
};

struct cmd_node ipsec_c_node =
{
	IPSEC_C_NODE,
	"%s\x1b[34m(%s)\x1b[0m# ",
	1
};

struct cmd_node conn_s_node =
{
	CONN_S_NODE,
	"%s\x1b[34m(%s conn %s%s\x1b[34m)\x1b[0m# ",
	1
};

struct cmd_node conn_c_node =
{
	CONN_C_NODE,
	"%s\x1b[34m(%s conn %s%s\x1b[34m)\x1b[0m# ",
	1	
};

/* Command vector which includes some level of command lists. Normally
   each daemon maintains each own cmdvec. */
vector cmdvec;

/* Host information structure. */
struct host host;

/* Install top node of command vector. */
void cmd_install_node (struct cmd_node *node, int (*func) (struct vty *))
{
    vector_set_index (cmdvec, node->node, node);
    node->func = func;
    node->cmd_vector = vector_init (VECTOR_MIN_SIZE);
}

/* Compare two command's string.  Used in sort_node (). */
static int cmp_node (const void *p, const void *q)
{
    struct cmd_element *a = *(struct cmd_element **)p;
    struct cmd_element *b = *(struct cmd_element **)q;

    return strcmp (a->string, b->string);
}

static int cmp_desc (const void *p, const void *q)
{
    struct desc *a = *(struct desc **)p;
    struct desc *b = *(struct desc **)q;

    return strcmp (a->cmd, b->cmd);
}

/* Sort each node's command element according to command string. */
void cmd_sort_node ()
{
    int i, j;
    struct cmd_node *cnode;
    vector descvec;
    struct cmd_element *cmd_element;

    for (i = 0; i < vector_max (cmdvec); i++) 
        if ((cnode = vector_slot (cmdvec, i)) != NULL)
        {	
            vector cmd_vector = cnode->cmd_vector;
            qsort (cmd_vector->index, cmd_vector->max, sizeof (void *), cmp_node);

            for (j = 0; j < vector_max (cmd_vector); j++)
                if ((cmd_element = vector_slot (cmd_vector, j)) != NULL)
                {
                    descvec = vector_slot (cmd_element->strvec,
                            vector_max (cmd_element->strvec) - 1);
                    qsort (descvec->index, descvec->max, sizeof (void *), cmp_desc);
                }
        }
}

/* Breaking up string into each command piece. I assume given
   character is separated by a space character. Return value is a
   vector which includes char ** data element. */
vector cmd_make_strvec_old (char *string)
{
    char *cp, *start, *token;
    int strlen;
    vector strvec;

    if (string == NULL)
        return NULL;

    cp = string;

    /* Skip white spaces. */
    while (isspace ((int) *cp) && *cp != '\0')
        cp++;

    /* Return if there is only white spaces */
    if (*cp == '\0')
        return NULL;

    if (*cp == '!' || *cp == '#')
        return NULL;

    /* Prepare return vector. */
    strvec = vector_init (VECTOR_MIN_SIZE);

    /* Copy each command piece and set into vector. */
    while (1) 
    {
        start = cp;
        while (!(isspace ((int) *cp) || *cp == '\r' || *cp == '\n') &&
                *cp != '\0')
            cp++;
        strlen = cp - start;
        token = XMALLOC (MTYPE_STRVEC, strlen + 1);
        memcpy (token, start, strlen);
        *(token + strlen) = '\0';
        vector_set (strvec, token);

        while ((isspace ((int) *cp) || *cp == '\n' || *cp == '\r') &&
                *cp != '\0')
            cp++;

        if (*cp == '\0')
            return strvec;
    }
}
// luozh 2012/3/28
vector cmd_make_strvec(char *string)
{
    char *cp, *start, *token, *tstart, *tend;
    int strlen;
    vector strvec;
    char *string2;

    if (string == NULL)
        return NULL;
    // luozh 2012/3/28 to support " space
    string2 = strdup(string);
    if((tstart = strchr(string2, '"')) != NULL)
    {
        tend = strchr(tstart + 1, '"');
        if(tend != NULL)
        {
            *tstart = ' ';
            tstart ++;
            while(tstart < tend)
            {
                if(*tstart == ' ')
                    *tstart = '-';
                tstart ++;
            }
            *tend = ' ';
        }
    }
    cp = string2;

    /* Skip white spaces. */
    while (isspace ((int) *cp) && *cp != '\0')
        cp++;

    /* Return if there is only white spaces */
    if (*cp == '\0')
        return NULL;

    if (*cp == '!' || *cp == '#')
        return NULL;

    /* Prepare return vector. */
    strvec = vector_init (VECTOR_MIN_SIZE);

    /* Copy each command piece and set into vector. */
    while (1) 
    {
        start = cp;
        while (!(isspace ((int) *cp) || *cp == '\r' || *cp == '\n') &&
                *cp != '\0')
            cp++;
        strlen = cp - start;
        token = XMALLOC (MTYPE_STRVEC, strlen + 1);
        memcpy (token, start, strlen);
        *(token + strlen) = '\0';
        vector_set (strvec, token);

        while ((isspace ((int) *cp) || *cp == '\n' || *cp == '\r') &&
                *cp != '\0')
            cp++;

        if (*cp == '\0')
        {
            free(string2);
            return strvec;
        }
    }
}

/* Free allocated string vector. */
void cmd_free_strvec (vector v)
{
    int i;
    char *cp;

    if (!v)
        return;

    for (i = 0; i < vector_max (v); i++)
        if ((cp = vector_slot (v, i)) != NULL)
            XFREE (MTYPE_STRVEC, cp);

    vector_free (v);
}

/* Fetch next description.  Used in cmd_make_descvec(). */
static char *cmd_desc_str (char **string)
{
    char *cp, *start, *token;
    int strlen;

    cp = *string;

    if (cp == NULL)
        return NULL;

    /* Skip white spaces. */
    while (isspace ((int) *cp) && *cp != '\0')
        cp++;

    /* Return if there is only white spaces */
    if (*cp == '\0')
        return NULL;

    start = cp;

    while (!(*cp == '\r' || *cp == '\n') && *cp != '\0')
        cp++;

    strlen = cp - start;
    token = XMALLOC (MTYPE_STRVEC, strlen + 1);
    memcpy (token, start, strlen);
    *(token + strlen) = '\0';

    *string = cp;

    return token;
}

/* New string vector. */
static vector cmd_make_descvec (char *string, char *descstr)
{
    int multiple = 0;
    char *sp;
    char *token;
    int len;
    char *cp;
    char *dp;
    vector allvec;
    vector strvec = NULL;
    struct desc *desc;

    cp = string;
    dp = descstr;

    if (cp == NULL)
        return NULL;

    allvec = vector_init (VECTOR_MIN_SIZE);

    while (1)
    {
        while (isspace ((int) *cp) && *cp != '\0')
            cp++;

        if (*cp == '(')
        {
            multiple = 1;
            cp++;
        }
        if (*cp == ')')
        {
            multiple = 0;
            cp++;
        }
        if (*cp == '|')
        {
            if (! multiple)
            {
                fprintf (stderr, "Command parse error!: %s\n", string);
                exit (1);
            }
            cp++;
        }

        while (isspace ((int) *cp) && *cp != '\0')
            cp++;

        if (*cp == '(')
        {
            multiple = 1;
            cp++;
        }

        if (*cp == '\0') 
            return allvec;

        sp = cp;

        while (! (isspace ((int) *cp) || *cp == '\r' || *cp == '\n' || *cp == ')' || *cp == '|') && *cp != '\0')
            cp++;

        len = cp - sp;

        token = XMALLOC (MTYPE_STRVEC, len + 1);
        memcpy (token, sp, len);
        *(token + len) = '\0';

        desc = XCALLOC (MTYPE_DESC, sizeof (struct desc));
        desc->cmd = token;
        desc->str = cmd_desc_str (&dp);

        if (multiple)
        {
            if (multiple == 1)
            {
                strvec = vector_init (VECTOR_MIN_SIZE);
                vector_set (allvec, strvec);
            }
            multiple++;
        }
        else
        {
            strvec = vector_init (VECTOR_MIN_SIZE);
            vector_set (allvec, strvec);
        }
        vector_set (strvec, desc);
    }
}

/* Count mandantory string vector size.  This is to determine inputed
   command has enough command length. */
static int cmd_cmdsize (vector strvec)
{
    int i;
    char *str;
    int size = 0;
    vector descvec;

    for (i = 0; i < vector_max (strvec); i++)
    {
        descvec = vector_slot (strvec, i);

        if (vector_max (descvec) == 1)
        {
            struct desc *desc = vector_slot (descvec, 0);

            str = desc->cmd;

            if (str == NULL || CMD_OPTION (str))
                return size;
            else
                size++;
        }
        else
            size++;
    }
    return size;
}

/* Return prompt character of specified node. */
char *cmd_prompt (enum node_type node)
{
    struct cmd_node *cnode;

    cnode = vector_slot (cmdvec, node);
    return cnode->prompt;
}

/* Install a command into a node. */
void cmd_install_element (enum node_type ntype, struct cmd_element *cmd)
{
    struct cmd_node *cnode;

    cnode = vector_slot (cmdvec, ntype);

    if (cnode == NULL) 
    {
        fprintf (stderr, "Command node %d doesn't exist, please check it\n",
                ntype);
        exit (1);
    }

    vector_set (cnode->cmd_vector, cmd);

    cmd->strvec = cmd_make_descvec (cmd->string, cmd->doc);
    cmd->cmdsize = cmd_cmdsize (cmd->strvec);
}


/* Utility function for getting command vector. */
static vector cmd_node_vector (vector v, enum node_type ntype)
{
    struct cmd_node *cnode = vector_slot (v, ntype);
    return cnode->cmd_vector;
}



/* Completion match types. */
enum match_type 
{
    no_match,
    extend_match,
    ipv4_prefix_match,
    ipv4_match,
    ipv6_prefix_match,
    ipv6_match,
    range_match,
    vararg_match,
    partly_match,
    exact_match 
};

static enum match_type cmd_ipv4_match (char *str)
{
    char *sp;
    int dots = 0, nums = 0;
    char buf[4];

    if (str == NULL)
        return partly_match;

    for (;;)
    {
        memset (buf, 0, sizeof (buf));
        sp = str;
        while (*str != '\0')
        {
            if (*str == '.')
            {
                if (dots >= 3)
                    return no_match;

                if (*(str + 1) == '.')
                    return no_match;

                if (*(str + 1) == '\0')
                    return partly_match;

                dots++;
                break;
            }
            if (!isdigit ((int) *str))
                return no_match;

            str++;
        }

        if (str - sp > 3)
            return no_match;

        strncpy (buf, sp, str - sp);
        if (atoi (buf) > 255)
            return no_match;

        nums++;

        if (*str == '\0')
            break;

        str++;
    }

    if (nums < 4)
        return partly_match;

    return exact_match;
}

static enum match_type cmd_ipv4_prefix_match (char *str)
{
    char *sp;
    int dots = 0;
    char buf[4];

    if (str == NULL)
        return partly_match;

    for (;;)
    {
        memset (buf, 0, sizeof (buf));
        sp = str;
        while (*str != '\0' && *str != '/')
        {
            if (*str == '.')
            {
                if (dots == 3)
                    return no_match;

                if (*(str + 1) == '.' || *(str + 1) == '/')
                    return no_match;

                if (*(str + 1) == '\0')
                    return partly_match;

                dots++;
                break;
            }

            if (!isdigit ((int) *str))
                return no_match;

            str++;
        }

        if (str - sp > 3)
            return no_match;

        strncpy (buf, sp, str - sp);
        if (atoi (buf) > 255)
            return no_match;

        if (dots == 3)
        {
            if (*str == '/')
            {
                if (*(str + 1) == '\0')
                    return partly_match;

                str++;
                break;
            }
            else if (*str == '\0')
                return partly_match;
        }

        if (*str == '\0')
            return partly_match;

        str++;
    }

    sp = str;
    while (*str != '\0')
    {
        if (!isdigit ((int) *str))
            return no_match;

        str++;
    }

    if (atoi (sp) > 32)
        return no_match;

    return exact_match;
}

#define IPV6_ADDR_STR		"0123456789abcdefABCDEF:.%"
#define IPV6_PREFIX_STR		"0123456789abcdefABCDEF:.%/"
#define STATE_START		1
#define STATE_COLON		2
#define STATE_DOUBLE		3
#define STATE_ADDR		4
#define STATE_DOT               5
#define STATE_SLASH		6
#define STATE_MASK		7

static enum match_type cmd_ipv6_match (char *str)
{
    int state = STATE_START;
    int colons = 0, nums = 0, double_colon = 0;
    char *sp = NULL;

    if (str == NULL)
        return partly_match;

    if (strspn (str, IPV6_ADDR_STR) != strlen (str))
        return no_match;

    while (*str != '\0')
    {
        switch (state)
        {
            case STATE_START:
                if (*str == ':')
                {
                    if (*(str + 1) != ':' && *(str + 1) != '\0')
                        return no_match;
                    colons--;
                    state = STATE_COLON;
                }
                else
                {
                    sp = str;
                    state = STATE_ADDR;
                }

                continue;
            case STATE_COLON:
                colons++;
                if (*(str + 1) == ':')
                    state = STATE_DOUBLE;
                else
                {
                    sp = str + 1;
                    state = STATE_ADDR;
                }
                break;
            case STATE_DOUBLE:
                if (double_colon)
                    return no_match;

                if (*(str + 1) == ':')
                    return no_match;
                else
                {
                    if (*(str + 1) != '\0')
                        colons++;
                    sp = str + 1;
                    state = STATE_ADDR;
                }

                double_colon++;
                nums++;
                break;
            case STATE_ADDR:
                if (*(str + 1) == ':' || *(str + 1) == '\0')
                {
                    if (str - sp > 3)
                        return no_match;

                    nums++;
                    state = STATE_COLON;
                }
                if (*(str + 1) == '.')
                    state = STATE_DOT;
                break;
            case STATE_DOT:
                state = STATE_ADDR;
                break;
            default:
                break;
        }

        if (nums > 8)
            return no_match;

        if (colons > 7)
            return no_match;

        str++;
    }
    return exact_match;
}

static enum match_type cmd_ipv6_prefix_match (char *str)
{
    int state = STATE_START;
    int colons = 0, nums = 0, double_colon = 0;
    int mask;
    char *sp = NULL;
    char *endptr = NULL;

    if (str == NULL)
        return partly_match;

    if (strspn (str, IPV6_PREFIX_STR) != strlen (str))
        return no_match;

    while (*str != '\0' && state != STATE_MASK)
    {
        switch (state)
        {
            case STATE_START:
                if (*str == ':')
                {
                    if (*(str + 1) != ':' && *(str + 1) != '\0')
                        return no_match;
                    colons--;
                    state = STATE_COLON;
                }
                else
                {
                    sp = str;
                    state = STATE_ADDR;
                }

                continue;
            case STATE_COLON:
                colons++;
                if (*(str + 1) == '/')
                    return no_match;
                else if (*(str + 1) == ':')
                    state = STATE_DOUBLE;
                else
                {
                    sp = str + 1;
                    state = STATE_ADDR;
                }
                break;
            case STATE_DOUBLE:
                if (double_colon)
                    return no_match;

                if (*(str + 1) == ':')
                    return no_match;
                else
                {
                    if (*(str + 1) != '\0' && *(str + 1) != '/')
                        colons++;
                    sp = str + 1;

                    if (*(str + 1) == '/')
                        state = STATE_SLASH;
                    else
                        state = STATE_ADDR;
                }

                double_colon++;
                nums += 1;
                break;
            case STATE_ADDR:
                if (*(str + 1) == ':' || *(str + 1) == '.'
                        || *(str + 1) == '\0' || *(str + 1) == '/')
                {
                    if (str - sp > 3)
                        return no_match;

                    for (; sp <= str; sp++)
                        if (*sp == '/')
                            return no_match;

                    nums++;

                    if (*(str + 1) == ':')
                        state = STATE_COLON;
                    else if (*(str + 1) == '.')
                        state = STATE_DOT;
                    else if (*(str + 1) == '/')
                        state = STATE_SLASH;
                }
                break;
            case STATE_DOT:
                state = STATE_ADDR;
                break;
            case STATE_SLASH:
                if (*(str + 1) == '\0')
                    return partly_match;

                state = STATE_MASK;
                break;
            default:
                break;
        }

        if (nums > 11)
            return no_match;

        if (colons > 7)
            return no_match;

        str++;
    }

    if (state < STATE_MASK)
        return partly_match;

    mask = strtol (str, &endptr, 10);
    if (*endptr != '\0')
        return no_match;

    if (mask < 0 || mask > 128)
        return no_match;
    return exact_match;
}

#define DECIMAL_STRLEN_MAX 10

static int cmd_range_match (char *range, char *str)
{
    char *p;
    char buf[DECIMAL_STRLEN_MAX + 1];
    char *endptr = NULL;
    unsigned long min, max, val;

    if (str == NULL)
        return 1;

    val = strtoul (str, &endptr, 10);
    if (*endptr != '\0')
        return 0;

    range++;
    p = strchr (range, '-');
    if (p == NULL)
        return 0;
    if (p - range > DECIMAL_STRLEN_MAX)
        return 0;
    strncpy (buf, range, p - range);
    buf[p - range] = '\0';
    min = strtoul (buf, &endptr, 10);
    if (*endptr != '\0')
        return 0;

    range = p + 1;
    p = strchr (range, '>');
    if (p == NULL)
        return 0;
    if (p - range > DECIMAL_STRLEN_MAX)
        return 0;
    strncpy (buf, range, p - range);
    buf[p - range] = '\0';
    max = strtoul (buf, &endptr, 10);
    if (*endptr != '\0')
        return 0;

    if (val < min || val > max)
        return 0;

    return 1;
}

/* Make completion match and return match type flag. */
static enum match_type cmd_filter_by_completion (char *command, vector v, int index)
{
    int i;
    char *str;
    struct cmd_element *cmd_element;
    enum match_type match_type;
    vector descvec;
    struct desc *desc;

    match_type = no_match;

    /* If command and cmd_element string does not match set NULL to vector */
    for (i = 0; i < vector_max (v); i++) 
        if ((cmd_element = vector_slot (v, i)) != NULL)
        {
            if (index >= vector_max (cmd_element->strvec))
                vector_slot (v, i) = NULL;
            else
            {
                int j;
                int matched = 0;

                descvec = vector_slot (cmd_element->strvec, index);

                for (j = 0; j < vector_max (descvec); j++)
                {
                    desc = vector_slot (descvec, j);
                    str = desc->cmd;

                    if (CMD_VARARG (str))
                    {
                        if (match_type < vararg_match)
                            match_type = vararg_match;
                        matched++;
                    }
                    else if (CMD_RANGE (str))
                    {
                        if (cmd_range_match (str, command))
                        {
                            if (match_type < range_match)
                                match_type = range_match;

                            matched++;
                        }
                    }
                    else if (CMD_IPV6 (str))
                    {
                        if (cmd_ipv6_match (command))
                        {
                            if (match_type < ipv6_match)
                                match_type = ipv6_match;

                            matched++;
                        }
                    }
                    else if (CMD_IPV6_PREFIX (str))
                    {
                        if (cmd_ipv6_prefix_match (command))
                        {
                            if (match_type < ipv6_prefix_match)
                                match_type = ipv6_prefix_match;

                            matched++;
                        }
                    }
                    else if (CMD_IPV4 (str))
                    {
                        if (cmd_ipv4_match (command))
                        {
                            if (match_type < ipv4_match)
                                match_type = ipv4_match;

                            matched++;
                        }
                    }
                    else if (CMD_IPV4_PREFIX (str))
                    {
                        if (cmd_ipv4_prefix_match (command))
                        {
                            if (match_type < ipv4_prefix_match)
                                match_type = ipv4_prefix_match;
                            matched++;
                        }
                    }
                    else
                        /* Check is this point's argument optional ? */
                        if (CMD_OPTION (str) || CMD_VARIABLE (str))
                        {
                            if (match_type < extend_match)
                                match_type = extend_match;
                            matched++;
                        }
                        else if (strncmp (command, str, strlen (command)) == 0)
                        {
                            if (strcmp (command, str) == 0) 
                                match_type = exact_match;
                            else
                            {
                                if (match_type < partly_match)
                                    match_type = partly_match;
                            }
                            matched++;
                        }
                }
                if (! matched)
                    vector_slot (v, i) = NULL;
            }
        }
    return match_type;
}

/* Filter vector by command character with index. */
static enum match_type cmd_filter_by_string (char *command, vector v, int index)
{
    int i;
    char *str;
    struct cmd_element *cmd_element;
    enum match_type match_type;
    vector descvec;
    struct desc *desc;

    match_type = no_match;

    /* If command and cmd_element string does not match set NULL to vector */
    for (i = 0; i < vector_max (v); i++) 
        if ((cmd_element = vector_slot (v, i)) != NULL)
        {
            /* If given index is bigger than max string vector of command,
               set NULL*/
            if (index >= vector_max (cmd_element->strvec))
                vector_slot (v, i) = NULL;
            else 
            {
                int j;
                int matched = 0;

                descvec = vector_slot (cmd_element->strvec, index);

                for (j = 0; j < vector_max (descvec); j++)
                {
                    desc = vector_slot (descvec, j);
                    str = desc->cmd;

                    if (CMD_VARARG (str))
                    {
                        if (match_type < vararg_match)
                            match_type = vararg_match;
                        matched++;
                    }
                    else if (CMD_RANGE (str))
                    {
                        if (cmd_range_match (str, command))
                        {
                            if (match_type < range_match)
                                match_type = range_match;
                            matched++;
                        }
                    }
                    else if (CMD_IPV6 (str))
                    {
                        if (cmd_ipv6_match (command) == exact_match)
                        {
                            if (match_type < ipv6_match)
                                match_type = ipv6_match;
                            matched++;
                        }
                    }
                    else if (CMD_IPV6_PREFIX (str))
                    {
                        if (cmd_ipv6_prefix_match (command) == exact_match)
                        {
                            if (match_type < ipv6_prefix_match)
                                match_type = ipv6_prefix_match;
                            matched++;
                        }
                    }
                    else if (CMD_IPV4 (str))
                    {
                        if (cmd_ipv4_match (command) == exact_match)
                        {
                            if (match_type < ipv4_match)
                                match_type = ipv4_match;
                            matched++;
                        }
                    }
                    else if (CMD_IPV4_PREFIX (str))
                    {
                        if (cmd_ipv4_prefix_match (command) == exact_match)
                        {
                            if (match_type < ipv4_prefix_match)
                                match_type = ipv4_prefix_match;
                            matched++;
                        }
                    }
                    else if (CMD_OPTION (str) || CMD_VARIABLE (str))
                    {
                        if (match_type < extend_match)
                            match_type = extend_match;
                        matched++;
                    }
                    else
                    {		  
                        if (strcmp (command, str) == 0)
                        {
                            match_type = exact_match;
                            matched++;
                        }
                    }
                }
                if (! matched)
                    vector_slot (v, i) = NULL;
            }
        }
    return match_type;
}

/* Check ambiguous match */
static int is_cmd_ambiguous (char *command, vector v, int index, enum match_type type)
{
    int i;
    int j;
    char *str = NULL;
    struct cmd_element *cmd_element;
    char *matched = NULL;
    vector descvec;
    struct desc *desc;

    for (i = 0; i < vector_max (v); i++) 
        if ((cmd_element = vector_slot (v, i)) != NULL)
        {
            int match = 0;

            descvec = vector_slot (cmd_element->strvec, index);

            for (j = 0; j < vector_max (descvec); j++)
            {
                enum match_type ret;

                desc = vector_slot (descvec, j);
                str = desc->cmd;

                switch (type)
                {
                    case exact_match:
                        if (! (CMD_OPTION (str) || CMD_VARIABLE (str))
                                && strcmp (command, str) == 0)
                            match++;
                        break;
                    case partly_match:
                        if (! (CMD_OPTION (str) || CMD_VARIABLE (str))
                                && strncmp (command, str, strlen (command)) == 0)
                        {
                            if (matched && strcmp (matched, str) != 0)
                                return 1;	/* There is ambiguous match. */
                            else
                                matched = str;
                            match++;
                        }
                        break;
                    case range_match:
                        if (cmd_range_match (str, command))
                        {
                            if (matched && strcmp (matched, str) != 0)
                                return 1;
                            else
                                matched = str;
                            match++;
                        }
                        break;
                    case ipv6_match:
                        if (CMD_IPV6 (str))
                            match++;
                        break;
                    case ipv6_prefix_match:
                        if ((ret = cmd_ipv6_prefix_match (command)) != no_match)
                        {
                            if (ret == partly_match)
                                return 2; /* There is incomplete match. */

                            match++;
                        }
                        break;
                    case ipv4_match:
                        if (CMD_IPV4 (str))
                            match++;
                        break;
                    case ipv4_prefix_match:
                        if ((ret = cmd_ipv4_prefix_match (command)) != no_match)
                        {
                            if (ret == partly_match)
                                return 2; /* There is incomplete match. */

                            match++;
                        }
                        break;
                    case extend_match:
                        if (CMD_OPTION (str) || CMD_VARIABLE (str))
                            match++;
                        break;
                    case no_match:
                    default:
                        break;
                }
            }
            if (! match)
                vector_slot (v, i) = NULL;
        }
    return 0;
}

/* If src matches dst return dst string, otherwise return NULL */
static char *cmd_entry_function (char *src, char *dst)
{
    /* Skip variable arguments. */
    if (CMD_OPTION (dst) || CMD_VARIABLE (dst) || CMD_VARARG (dst) ||
            CMD_IPV4 (dst) || CMD_IPV4_PREFIX (dst) || CMD_RANGE (dst))
        return NULL;

    /* In case of 'command \t', given src is NULL string. */
    if (src == NULL)
        return dst;

    /* Matched with input string. */
    if (strncmp (src, dst, strlen (src)) == 0)
        return dst;

    return NULL;
}

/* If src matches dst return dst string, otherwise return NULL */
/* This version will return the dst string always if it is
   CMD_VARIABLE for '?' key processing */
static char *cmd_entry_function_desc (char *src, char *dst)
{
    if (CMD_VARARG (dst))
        return dst;

    if (CMD_RANGE (dst))
    {
        if (cmd_range_match (dst, src))
            return dst;
        else
            return NULL;
    }

    if (CMD_IPV6 (dst))
    {
        if (cmd_ipv6_match (src))
            return dst;
        else
            return NULL;
    }

    if (CMD_IPV6_PREFIX (dst))
    {
        if (cmd_ipv6_prefix_match (src))
            return dst;
        else
            return NULL;
    }

    if (CMD_IPV4 (dst))
    {
        if (cmd_ipv4_match (src))
            return dst;
        else
            return NULL;
    }

    if (CMD_IPV4_PREFIX (dst))
    {
        if (cmd_ipv4_prefix_match (src))
            return dst;
        else
            return NULL;
    }

    /* Optional or variable commands always match on '?' */
    if (CMD_OPTION (dst) || CMD_VARIABLE (dst))
        return dst;

    /* In case of 'command \t', given src is NULL string. */
    if (src == NULL)
        return dst;

    if (strncmp (src, dst, strlen (src)) == 0)
        return dst;
    else
        return NULL;
}

/* Check same string element existence.  If it isn't there return
   1. */
static int cmd_unique_string (vector v, char *str)
{
    int i;
    char *match;

    for (i = 0; i < vector_max (v); i++)
        if ((match = vector_slot (v, i)) != NULL)
            if (strcmp (match, str) == 0)
                return 0;
    return 1;
}

/* Compare string to description vector.  If there is same string
   return 1 else return 0. */
static int desc_unique_string (vector v, char *str)
{
    int i;
    struct desc *desc;

    for (i = 0; i < vector_max (v); i++)
        if ((desc = vector_slot (v, i)) != NULL)
            if (strcmp (desc->cmd, str) == 0)
                return 1;
    return 0;
}

/* '?' describe command support. */
vector cmd_describe_command (vector vline, struct vty *vty, int *status)
{
    int i;
    vector cmd_vector;
#define INIT_MATCHVEC_SIZE 10
    vector matchvec;
    struct cmd_element *cmd_element;
    int index;
    int ret;
    enum match_type match;
    char *command;
    static struct desc desc_cr = { "<cr>", "" };

    /* Set index. */
    index = vector_max (vline) - 1;

    /* Make copy vector of current node's command vector. */
    cmd_vector = vector_copy (cmd_node_vector (cmdvec, vty->node));

    /* Prepare match vector */
    matchvec = vector_init (INIT_MATCHVEC_SIZE);

    /* Filter commands. */
    /* Only words precedes current word will be checked in this loop. */
    for (i = 0; i < index; i++)
    {
        command = vector_slot (vline, i);
        match = cmd_filter_by_completion (command, cmd_vector, i);

        if (match == vararg_match)
        {
            struct cmd_element *cmd_element;
            vector descvec;
            int j, k;

            for (j = 0; j < vector_max (cmd_vector); j++)
                if ((cmd_element = vector_slot (cmd_vector, j)) != NULL)
                {
                    descvec = vector_slot (cmd_element->strvec,
                            vector_max (cmd_element->strvec) - 1);
                    for (k = 0; k < vector_max (descvec); k++)
                    {
                        struct desc *desc = vector_slot (descvec, k);
                        vector_set (matchvec, desc);
                    }
                }

            vector_set (matchvec, &desc_cr);
            vector_free (cmd_vector);

            return matchvec;
        }

        if ((ret = is_cmd_ambiguous (command, cmd_vector, i, match)) == 1)
        {
            vector_free (cmd_vector);
            *status = CMD_ERR_AMBIGUOUS;
            return NULL;
        }
        else if (ret == 2)
        {
            vector_free (cmd_vector);
            *status = CMD_ERR_NO_MATCH;
            return NULL;
        }
    }

    /* Prepare match vector */
    /*  matchvec = vector_init (INIT_MATCHVEC_SIZE); */

    /* Make sure that cmd_vector is filtered based on current word */
    command = vector_slot (vline, index);
    if (command)
        match = cmd_filter_by_completion (command, cmd_vector, index);

    /* Make description vector. */
    for (i = 0; i < vector_max (cmd_vector); i++)
        if ((cmd_element = vector_slot (cmd_vector, i)) != NULL)
        {
            char *string = NULL;
            vector strvec = cmd_element->strvec;

            /* if command is NULL, index may be equal to vector_max */
            if (command && index >= vector_max (strvec))
                vector_slot (cmd_vector, i) = NULL;
            else
            {
                /* Check if command is completed. */
                if (command == NULL && index == vector_max (strvec))
                {
                    string = "<cr>";
                    if (! desc_unique_string (matchvec, string))
                        vector_set (matchvec, &desc_cr);
                }
                else
                {
                    int j;
                    vector descvec = vector_slot (strvec, index);
                    struct desc *desc;

                    for (j = 0; j < vector_max (descvec); j++)
                    {
                        desc = vector_slot (descvec, j);
                        string = cmd_entry_function_desc (command, desc->cmd);
                        if (string)
                        {
                            /* Uniqueness check */
                            if (! desc_unique_string (matchvec, string))
                                vector_set (matchvec, desc);
                        }
                    }
                }
            }
        }
    vector_free (cmd_vector);

    if (vector_slot (matchvec, 0) == NULL)
    {
        vector_free (matchvec);
        *status= CMD_ERR_NO_MATCH;
    }
    else
        *status = CMD_SUCCESS;

    return matchvec;
}

/* Check LCD of matched command. */
static int cmd_lcd (char **matched)
{
    int i;
    int j;
    int lcd = -1;
    char *s1, *s2;
    char c1, c2;

    if (matched[0] == NULL || matched[1] == NULL)
        return 0;

    for (i = 1; matched[i] != NULL; i++)
    {
        s1 = matched[i - 1];
        s2 = matched[i];

        for (j = 0; (c1 = s1[j]) && (c2 = s2[j]); j++)
            if (c1 != c2)
                break;

        if (lcd < 0)
            lcd = j;
        else
        {
            if (lcd > j)
                lcd = j;
        }
    }
    return lcd;
}

/* Command line completion support. */
char **cmd_complete_command (vector vline, struct vty *vty, int *status)
{
    int i;
    vector cmd_vector = vector_copy (cmd_node_vector (cmdvec, vty->node));
#define INIT_MATCHVEC_SIZE 10
    vector matchvec;
    struct cmd_element *cmd_element;
    int index = vector_max (vline) - 1;
    char **match_str;
    struct desc *desc;
    vector descvec;
    char *command;
    int lcd;

    /* First, filter by preceeding command string */
    for (i = 0; i < index; i++)
    {
        enum match_type match;
        int ret;

        command = vector_slot (vline, i);

        /* First try completion match, if there is exactly match return 1 */
        match = cmd_filter_by_completion (command, cmd_vector, i);

        /* If there is exact match then filter ambiguous match else check
           ambiguousness. */
        if ((ret = is_cmd_ambiguous (command, cmd_vector, i, match)) == 1)
        {
            vector_free (cmd_vector);
            *status = CMD_ERR_AMBIGUOUS;
            return NULL;
        }
        /*
           else if (ret == 2)
           {
           vector_free (cmd_vector);
         *status = CMD_ERR_NO_MATCH;
         return NULL;
         }
         */
    }

    /* Prepare match vector. */
    matchvec = vector_init (INIT_MATCHVEC_SIZE);

    /* Now we got into completion */
    for (i = 0; i < vector_max (cmd_vector); i++)
        if ((cmd_element = vector_slot (cmd_vector, i)) != NULL)
        {
            char *string;
            vector strvec = cmd_element->strvec;

            /* Check field length */
            if (index >= vector_max (strvec))
                vector_slot (cmd_vector, i) = NULL;
            else 
            {
                int j;

                descvec = vector_slot (strvec, index);
                for (j = 0; j < vector_max (descvec); j++)
                {
                    desc = vector_slot (descvec, j);

                    if ((string = cmd_entry_function (vector_slot (vline, index),
                                    desc->cmd)))
                        if (cmd_unique_string (matchvec, string))
                            vector_set (matchvec, XSTRDUP (MTYPE_TMP, string));
                }
            }
        }

    /* We don't need cmd_vector any more. */
    vector_free (cmd_vector);

    /* No matched command */
    if (vector_slot (matchvec, 0) == NULL)
    {
        vector_free (matchvec);

        /* In case of 'command \t' pattern.  Do you need '?' command at
           the end of the line. */
        if (vector_slot (vline, index) == '\0')
            *status = CMD_ERR_NOTHING_TODO;
        else
            *status = CMD_ERR_NO_MATCH;
        return NULL;
    }

    /* Only one matched */
    if (vector_slot (matchvec, 1) == NULL)
    {
        match_str = (char **) matchvec->index;
        vector_only_wrapper_free (matchvec);
        *status = CMD_COMPLETE_FULL_MATCH;
        return match_str;
    }
    /* Make it sure last element is NULL. */
    vector_set (matchvec, NULL);

    /* Check LCD of matched strings. */
    if (vector_slot (vline, index) != NULL)
    {
        lcd = cmd_lcd ((char **) matchvec->index);

        if (lcd)
        {
            int len = strlen (vector_slot (vline, index));

            if (len < lcd)
            {
                char *lcdstr;

                lcdstr = XMALLOC (MTYPE_TMP, lcd + 1);
                memcpy (lcdstr, matchvec->index[0], lcd);
                lcdstr[lcd] = '\0';

                /* match_str = (char **) &lcdstr; */

                /* Free matchvec. */
                for (i = 0; i < vector_max (matchvec); i++)
                {
                    if (vector_slot (matchvec, i))
                        XFREE (MTYPE_TMP, vector_slot (matchvec, i));
                }
                vector_free (matchvec);

                /* Make new matchvec. */
                matchvec = vector_init (INIT_MATCHVEC_SIZE);
                vector_set (matchvec, lcdstr);
                match_str = (char **) matchvec->index;
                vector_only_wrapper_free (matchvec);

                *status = CMD_COMPLETE_MATCH;
                return match_str;
            }
        }
    }

    match_str = (char **) matchvec->index;
    vector_only_wrapper_free (matchvec);
    *status = CMD_COMPLETE_LIST_MATCH;
    return match_str;
}

/* Execute command by argument vline vector. */
int cmd_execute_command (vector vline, struct vty *vty, struct cmd_element **cmd)
{
    int i;
    int index;
    vector cmd_vector;
    struct cmd_element *cmd_element;
    struct cmd_element *matched_element;
    unsigned int matched_count, incomplete_count;
    int argc;
    char *argv[CMD_ARGC_MAX];
    enum match_type match = 0;
    int varflag;
    char *command;

    /* Make copy of command elements. */
    cmd_vector = vector_copy (cmd_node_vector (cmdvec, vty->node));

    for (index = 0; index < vector_max (vline); index++) 
    {
        int ret;

        command = vector_slot (vline, index);

        match = cmd_filter_by_completion (command, cmd_vector, index);

        if (match == vararg_match)
            break;

        ret = is_cmd_ambiguous (command, cmd_vector, index, match);

        if (ret == 1)
        {
            vector_free (cmd_vector);
            return CMD_ERR_AMBIGUOUS;
        }
        else if (ret == 2)
        {
            vector_free (cmd_vector);
            return CMD_ERR_NO_MATCH;
        }
    }

    /* Check matched count. */
    matched_element = NULL;
    matched_count = 0;
    incomplete_count = 0;

    for (i = 0; i < vector_max (cmd_vector); i++) 
        if (vector_slot (cmd_vector,i) != NULL)
        {
            cmd_element = vector_slot (cmd_vector,i);

            if (match == vararg_match || index >= cmd_element->cmdsize)
            {
                matched_element = cmd_element;
#if 0
                printf ("DEBUG: %s\n", cmd_element->string);
#endif
                matched_count++;
            }
            else
            {
                incomplete_count++;
            }
        }

    /* Finish of using cmd_vector. */
    vector_free (cmd_vector);

    /* To execute command, matched_count must be 1.*/
    if (matched_count == 0) 
    {
        if (incomplete_count)
            return CMD_ERR_INCOMPLETE;
        else
            return CMD_ERR_NO_MATCH;
    }

    if (matched_count > 1) 
        return CMD_ERR_AMBIGUOUS;

    /* Argument treatment */
    varflag = 0;
    argc = 0;

    for (i = 0; i < vector_max (vline); i++)
    {
        if (varflag)
            argv[argc++] = vector_slot (vline, i);
        else
        {	  
            vector descvec = vector_slot (matched_element->strvec, i);

            if (vector_max (descvec) == 1)
            {
                struct desc *desc = vector_slot (descvec, 0);
                char *str = desc->cmd;

                if (CMD_VARARG (str))
                    varflag = 1;

                if (varflag || CMD_VARIABLE (str) || CMD_OPTION (str))
                    argv[argc++] = vector_slot (vline, i);
            }
            else
                argv[argc++] = vector_slot (vline, i);
        }

        if (argc >= CMD_ARGC_MAX)
            return CMD_ERR_EXEED_ARGC_MAX;
    }

    /* For vtysh execution. */
    if (cmd)
        *cmd = matched_element;

    if (matched_element->daemon)
        return CMD_SUCCESS_DAEMON;

    /* Execute matched command. */
    return (*matched_element->func) (matched_element, vty, argc, argv);
}

/* Execute command by argument readline. */
int cmd_execute_command_strict (vector vline, struct vty *vty, struct cmd_element **cmd)
{
    int i;
    int index;
    vector cmd_vector;
    struct cmd_element *cmd_element;
    struct cmd_element *matched_element;
    unsigned int matched_count, incomplete_count;
    int argc;
    char *argv[CMD_ARGC_MAX];
    int varflag;
    enum match_type match = 0;
    char *command;

    /* Make copy of command element */
    cmd_vector = vector_copy (cmd_node_vector (cmdvec, vty->node));

    for (index = 0; index < vector_max (vline); index++) 
    {
        int ret;

        command = vector_slot (vline, index);

        match = cmd_filter_by_string (vector_slot (vline, index), 
                cmd_vector, index);

        /* If command meets '.VARARG' then finish matching. */
        if (match == vararg_match)
            break;

        ret = is_cmd_ambiguous (command, cmd_vector, index, match);
        if (ret == 1)
        {
            vector_free (cmd_vector);
            return CMD_ERR_AMBIGUOUS;
        }
        if (ret == 2)
        {
            vector_free (cmd_vector);
            return CMD_ERR_NO_MATCH;
        }
    }

    /* Check matched count. */
    matched_element = NULL;
    matched_count = 0;
    incomplete_count = 0;
    for (i = 0; i < vector_max (cmd_vector); i++) 
        if (vector_slot (cmd_vector,i) != NULL)
        {
            cmd_element = vector_slot (cmd_vector,i);

            if (match == vararg_match || index >= cmd_element->cmdsize)
            {
                matched_element = cmd_element;
                matched_count++;
            }
            else
                incomplete_count++;
        }

    /* Finish of using cmd_vector. */
    vector_free (cmd_vector);

    /* To execute command, matched_count must be 1.*/
    if (matched_count == 0) 
    {
        if (incomplete_count)
            return CMD_ERR_INCOMPLETE;
        else
            return CMD_ERR_NO_MATCH;
    }

    if (matched_count > 1) 
        return CMD_ERR_AMBIGUOUS;

    /* Argument treatment */
    varflag = 0;
    argc = 0;

    for (i = 0; i < vector_max (vline); i++)
    {
        if (varflag)
            argv[argc++] = vector_slot (vline, i);
        else
        {	  
            vector descvec = vector_slot (matched_element->strvec, i);

            if (vector_max (descvec) == 1)
            {
                struct desc *desc = vector_slot (descvec, 0);
                char *str = desc->cmd;

                if (CMD_VARARG (str))
                    varflag = 1;

                if (varflag || CMD_VARIABLE (str) || CMD_OPTION (str))
                    argv[argc++] = vector_slot (vline, i);
            }
            else
                argv[argc++] = vector_slot (vline, i);
        }

        if (argc >= CMD_ARGC_MAX)
            return CMD_ERR_EXEED_ARGC_MAX;
    }

    /* For vtysh execution. */
    if (cmd)
        *cmd = matched_element;

    if (matched_element->daemon)
        return CMD_SUCCESS_DAEMON;

    /* Now execute matched command */
    return (*matched_element->func) (matched_element, vty, argc, argv);
}

/* Execute command in child process. */
int cmd_execute_system_command (char *command, int argc, char **argv)
{
    int ret;
    pid_t pid;
    int status;

    /* Call fork(). */
    pid = fork ();

    if (pid < 0)
    {
        /* Failure of fork(). */
        fprintf (stderr, "Can't fork: %s\n", strerror (errno));
        return -1;
    }
    else if (pid == 0)
    {
        /* This is child process. we should use the execvp ?*/
        switch (argc)
        {
            case 0:
                ret = execlp (command, command, NULL);
                break;
            case 1:
                ret = execlp (command, command, argv[0], NULL);
                break;
            case 2:
                ret = execlp (command, command, argv[0], argv[1], NULL);
                break;
            case 3:
                ret = execlp (command, command, argv[0], argv[1], argv[2], NULL);
                break;
            case 4:
                ret = execlp (command, command, argv[0], argv[1], argv[2], argv[3], NULL);
                break;
            case 5:
                ret = execlp (command, command, argv[0], argv[1], argv[2], argv[3], argv[4], NULL);
                break;
            case 6:
                ret = execlp (command, command, argv[0], argv[1], argv[2], argv[3], argv[4], argv[5], NULL);
                break;
            case 7:
                ret = execlp (command, command, argv[0], argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], NULL);
                break;
            case 8:
                ret = execlp (command, command, argv[0], argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], argv[7], NULL);
                break;
        }

        /* When execlp suceed, this part is not executed. */
        fprintf (stderr, "Can't execute %s: %s\n", command, strerror (errno));
        exit (1);
    }
    else
    {
        /* This is parent. */
        ret = wait4 (pid, &status, 0, NULL);
    }
    return 0;
}

/* Initialize command interface. Install basic nodes and commands. */
void cmd_init (int terminal)
{
    /* Allocate initial top vector of commands. */
    cmdvec = vector_init (VECTOR_MIN_SIZE);

    /* Default host value settings. */
    memset(&host, 0, sizeof(host));
    host.name = NULL;
    host.enable = NULL;
    host.enable_encrypt = NULL;
    host.config = NULL;
    host.trytimes = 0;
    host.chpasswd = 0;

    /* Install top nodes. */
    cmd_install_node (&view_node, NULL);
    cmd_install_node (&enable_node, NULL);
    cmd_install_node (&auth_enable_node, NULL);
    cmd_install_node (&config_node, NULL);
    cmd_install_node (&shell_node, NULL);
    cmd_install_node (&ipsec_node, NULL);
    cmd_install_node (&ipsec_s_node, NULL);
    cmd_install_node (&ipsec_c_node, NULL);
    cmd_install_node (&conn_s_node, NULL);
    cmd_install_node (&conn_c_node, NULL);

    srand(time(NULL));
}
