/******************************************************************************/
/* microrl.c                                                                  */
/*                                                                            */
/*    Author: Samoylov Eugene aka Helius (ghelius@gmail.com)                  */
/*    BUGS and TODO:                                                          */
/*    -- add echo_off feature                                                 */
/*    -- rewrite history for use more than 256 byte buffer                    */
/*                                                                            */
/******************************************************************************/

#include <string.h>
#include "data_types.h"
#include "microrl.h"



static microrl_t rl;
microrl_t * pointerMicrorl = &rl;
int8_t * prompt_default = _PROMPT_DEFAULT;

#ifdef _USE_HISTORY

#ifdef _HISTORY_DEBUG
//*****************************************************************************
// print buffer content on screen
static void print_hist (ring_history_t * pThis)
{
    pThis->print ("\n");
    for (int32_t i = 0; i < _RING_HISTORY_LEN; i++) {
        if (i == pThis->begin)
            pThis->print ("b");
        else
            pThis->print (" ");
    }
    pThis->print ("\n");
    for (int32_t i = 0; i < _RING_HISTORY_LEN; i++) {
        if (isalpha(pThis->ring_buf[i]))
            pThis->print ("%c", pThis->ring_buf[i]);
        else
            pThis->print ("%d", pThis->ring_buf[i]);
    }
    pThis->print ("\n");
    for (int32_t i = 0; i < _RING_HISTORY_LEN; i++) {
        if (i == pThis->end)
            pThis->print ("e");
        else
            pThis->print (" ");
    }
    pThis->print ("\n");
}
#endif

//*****************************************************************************
// fake printf for prodaction 21-09-2012
static uint32_t fake_serprintf(const int8_t * format, ...)
{
    return 0;
}

//*****************************************************************************
// remove older message from ring buffer
static void hist_erase_older (ring_history_t * pThis)
{
    int32_t new_pos = pThis->begin + pThis->ring_buf [pThis->begin] + 1;
    if (new_pos >= _RING_HISTORY_LEN)
        new_pos = new_pos - _RING_HISTORY_LEN;

    pThis->begin = new_pos;
}

//*****************************************************************************
// check space for new line, remove older while not space
static int32_t hist_is_space_for_new (ring_history_t * pThis, int32_t len)
{
    if (pThis->ring_buf [pThis->begin] == 0)
        return true;
    if (pThis->end >= pThis->begin) {
        if (_RING_HISTORY_LEN - pThis->end + pThis->begin - 1 > len)
            return true;
    }    else {
        if (pThis->begin - pThis->end - 1> len)
            return true;
    }
    return false;
}

//*****************************************************************************
// put line to ring buffer
static void hist_save_line (ring_history_t * pThis, int8_t * line, int32_t len)
{
    if (len > _RING_HISTORY_LEN - 2)
        return;
    while (!hist_is_space_for_new (pThis, len)) {
        hist_erase_older (pThis);
    }
    // if it's first line
    if (pThis->ring_buf [pThis->begin] == 0)
        pThis->ring_buf [pThis->begin] = len;

    // store line
    if (len < _RING_HISTORY_LEN-pThis->end-1)
        memcpy (pThis->ring_buf + pThis->end + 1, line, len);
    else {
        int32_t part_len = _RING_HISTORY_LEN-pThis->end-1;
        memcpy (pThis->ring_buf + pThis->end + 1, line, part_len);
        memcpy (pThis->ring_buf, line + part_len, len - part_len);
    }
    pThis->ring_buf [pThis->end] = len;
    pThis->end = pThis->end + len + 1;
    if (pThis->end >= _RING_HISTORY_LEN)
        pThis->end -= _RING_HISTORY_LEN;
    pThis->ring_buf [pThis->end] = 0;
    pThis->cur = 0;
#ifdef _HISTORY_DEBUG
    print_hist (pThis);
#endif
}

//*****************************************************************************
// copy saved line to 'line' and return size of line
static int32_t hist_restore_line (ring_history_t * pThis, int8_t * line, int32_t dir)
{
    int32_t cnt = 0;
    // count history record
    int32_t header = pThis->begin;
    while (pThis->ring_buf [header] != 0) {
        header += pThis->ring_buf [header] + 1;
        if (header >= _RING_HISTORY_LEN)
            header -= _RING_HISTORY_LEN;
        cnt++;
    }

    if (dir == _HIST_UP) {
        if (cnt >= pThis->cur) {
            int32_t header = pThis->begin;
            int32_t j = 0;
            // found record for 'pThis->cur' index
            while ((pThis->ring_buf [header] != 0) && (cnt - j -1 != pThis->cur)) {
                header += pThis->ring_buf [header] + 1;
                if (header >= _RING_HISTORY_LEN)
                    header -= _RING_HISTORY_LEN;
                j++;
            }
            if (pThis->ring_buf[header]) {
                    pThis->cur++;
                // obtain saved line
                if (pThis->ring_buf [header] + header < _RING_HISTORY_LEN) {
                    memcpy (line, pThis->ring_buf + header + 1, pThis->ring_buf[header]);
                } else {
                    int32_t part0 = _RING_HISTORY_LEN - header - 1;
                    memcpy (line, pThis->ring_buf + header + 1, part0);
                    memcpy (line + part0, pThis->ring_buf, pThis->ring_buf[header] - part0);
                }
                return pThis->ring_buf[header];
            }
        }
    } else {
        if (pThis->cur > 0) {
                pThis->cur--;
            int32_t header = pThis->begin;
            int32_t j = 0;

            while ((pThis->ring_buf [header] != 0) && (cnt - j != pThis->cur)) {
                header += pThis->ring_buf [header] + 1;
                if (header >= _RING_HISTORY_LEN)
                    header -= _RING_HISTORY_LEN;
                j++;
            }
            if (pThis->ring_buf [header] + header < _RING_HISTORY_LEN) {
                memcpy (line, pThis->ring_buf + header + 1, pThis->ring_buf[header]);
            } else {
                int32_t part0 = _RING_HISTORY_LEN - header - 1;
                memcpy (line, pThis->ring_buf + header + 1, part0);
                memcpy (line + part0, pThis->ring_buf, pThis->ring_buf[header] - part0);
            }
            return pThis->ring_buf[header];
        }
    }
    return 0;
}
#endif








//*****************************************************************************
// split cmdline to tkn array and return nmb of token
static int32_t split (microrl_t * pThis, int32_t limit)
{
    int32_t i = 0;
    int32_t ind = 0;
    while (1) {
        // go to the first whitespace (zerro for us)
        while ((pThis->cmdline [ind] == '\0') && (ind < limit)) {
            ind++;
        }
        if (!(ind < limit)) return i;
        pThis->tkn_arr[i++] = pThis->cmdline + ind;
        if (i >= _COMMAND_TOKEN_NMB) {
            return -1;
        }
        // go to the first NOT whitespace (not zerro for us)
        while ((pThis->cmdline [ind] != '\0') && (ind < limit)) {
            ind++;
        }
        if (!(ind < limit)) return i;
    }
}


//*****************************************************************************
inline static void print_prompt (microrl_t * pThis)
{
    pThis->print (pThis->prompt_str);
}

//*****************************************************************************
inline static void terminal_backspace (microrl_t * pThis)
{
    pThis->print ("\033[D \033[D");
}

//*****************************************************************************
inline static void terminal_newline (microrl_t * pThis)
{
    pThis->print (ENDL);
}

#ifndef _USE_LIBC_STDIO
//*****************************************************************************
// convert 16 bit value to string
// 0 value not supported!!! just make empty string
static void u16bit_to_str (uint32_t nmb, int8_t * buf)
{
    int8_t tmp_str [6] = {0,};
    int32_t i = 0;
    if (nmb <= 0xFFFF) {
        while (nmb > 0) {
            tmp_str[i++] = (nmb % 10) + '0';
            nmb /=10;
        }
        for (int32_t j = 0; j < i; ++j)
            *(buf++) = tmp_str [i-j-1];
    }
    *buf = '\0';
}
#endif


//*****************************************************************************
// set cursor at position from begin cmdline (after prompt) + offset
static void terminal_move_cursor (microrl_t * pThis, int32_t offset)
{
    int8_t str[16] = {0,};
#ifdef _USE_LIBC_STDIO
    if (offset > 0) {
        snprintf (str, 16, "\033[%dC", offset);
    } else if (offset < 0) {
        snprintf (str, 16, "\033[%dD", -(offset));
    }
#else
    strcpy (str, "\033[");
    if (offset > 0) {
        u16bit_to_str (offset, str+2);
        strcat (str, "C");
    } else if (offset < 0) {
        u16bit_to_str (-(offset), str+2);
        strcat (str, "D");
    } else
        return;
#endif
    pThis->print (str);
}

//*****************************************************************************
static void terminal_reset_cursor (microrl_t * pThis)
{
    int8_t str[16];
#ifdef _USE_LIBC_STDIO
    snprintf (str, 16, "\033[%dD\033[%dC", \
                        _COMMAND_LINE_LEN + _PROMPT_LEN + 2, _PROMPT_LEN);

#else
    strcpy (str, "\033[");
    u16bit_to_str ( _COMMAND_LINE_LEN + _PROMPT_LEN + 2,str+2);
    strcat (str, "D\033[");
    u16bit_to_str (_PROMPT_LEN, str+strlen(str));
    strcat (str, "C");
#endif
    pThis->print (str);
}

//*****************************************************************************
// print cmdline to screen, replace '\0' to wihitespace
static void terminal_print_line (microrl_t * pThis, int32_t pos, int32_t cursor)
{
    pThis->print ("\033[K");    // delete all from cursor to end

    int8_t nch [] = {0,0};
    for (int32_t i = pos; i < pThis->cmdlen; i++) {
        nch [0] = pThis->cmdline [i];
        if (nch[0] == '\0')
            nch[0] = ' ';
        pThis->print (nch);
    }

    terminal_reset_cursor (pThis);
    terminal_move_cursor (pThis, cursor);
}

//*****************************************************************************
void microrl_init (microrl_t * pThis, serprint_func f)
{
    memset(pThis->cmdline, 0, _COMMAND_LINE_LEN);
#ifdef _USE_HISTORY
    memset(pThis->ring_hist.ring_buf, 0, _RING_HISTORY_LEN);
    pThis->ring_hist.begin = 0;
    pThis->ring_hist.end = 0;
    pThis->ring_hist.cur = 0;
#endif
    pThis->cmdlen =0;
    pThis->cursor = 0;
    pThis->execute = NULL;
    pThis->get_completion = NULL;
#ifdef _USE_CTLR_C
    pThis->sigint = NULL;
#endif
    pThis->prompt_str = prompt_default;
    pThis->print = f == NULL ? &fake_serprintf : f;
#ifdef _ENABLE_INIT_PROMPT
    print_prompt (pThis);
#endif
}

//*****************************************************************************
void microrl_init_printf (microrl_t * pThis, serprint_func f)
{
    pThis->print = f == NULL ? &fake_serprintf : f;
}

//*****************************************************************************
serprint_func get_microrl_printf (microrl_t * pThis)
{
    return pThis->print;
}

//*****************************************************************************
void microrl_set_complete_callback (microrl_t * pThis, int8_t ** (*get_completion)(int32_t, const int8_t* const*))
{
    pThis->get_completion = get_completion;
}

//*****************************************************************************
void microrl_set_execute_callback (microrl_t * pThis, int32_t (*execute)(int32_t, const int8_t* const*))
{
    pThis->execute = execute;
}
#ifdef _USE_CTLR_C
//*****************************************************************************
void microrl_set_sigint_callback (microrl_t * pThis, void (*sigintf)(void))
{
    pThis->sigint = sigintf;
}
#endif

#ifdef _USE_ESC_SEQ
static void hist_search (microrl_t * pThis, int32_t dir)
{
    int32_t len = hist_restore_line (&pThis->ring_hist, pThis->cmdline, dir);
    if (len) {
        pThis->cursor = pThis->cmdlen = len;
        terminal_reset_cursor (pThis);
        terminal_print_line (pThis, 0, pThis->cursor);
    }
}

//*****************************************************************************
// handling escape sequences
static int32_t escape_process (microrl_t * pThis, int8_t ch)
{
    static int32_t seq = 0;

    if (ch == '[') {
        seq = _ESC_BRACKET;
    } else if (seq == _ESC_BRACKET) {
        if (ch == 'A') {
#ifdef _USE_HISTORY
            hist_search (pThis, _HIST_UP);
#endif
            return 1;
        } else if (ch == 'B') {
#ifdef _USE_HISTORY
            hist_search (pThis, _HIST_DOWN);
#endif
            return 1;
        } else if (ch == 'C') {
            if (pThis->cursor < pThis->cmdlen) {
                terminal_move_cursor (pThis, 1);
                pThis->cursor++;
            }
            return 1;
        } else if (ch == 'D') {
            if (pThis->cursor > 0) {
                terminal_move_cursor (pThis, -1);
                pThis->cursor--;
            }
            return 1;
        } else if (ch == '7') {
            seq = _ESC_HOME;
            return 0;
        } else if (ch == '8') {
            seq = _ESC_END;
            return 0;
        }
    } else if (ch == '~') {
            if (seq == _ESC_HOME) {
                terminal_reset_cursor (pThis);
                pThis->cursor = 0;
                return 1;
            } else if (seq == _ESC_END) {
                terminal_move_cursor (pThis, pThis->cmdlen-pThis->cursor);
                pThis->cursor = pThis->cmdlen;
                return 1;
            }

    }
    return 0;
}
#endif

//*****************************************************************************
// insert len int8_t of text at cursor position
static int32_t microrl_insert_text (microrl_t * pThis, int8_t * text, int32_t len)
{
    if (pThis->cmdlen + len < _COMMAND_LINE_LEN) {
        memmove (pThis->cmdline + pThis->cursor + len,
                         pThis->cmdline + pThis->cursor,
                         pThis->cmdlen - pThis->cursor);
        for (int32_t i = 0; i < len; i++) {
            pThis->cmdline [pThis->cursor + i] = text [i];
            if (pThis->cmdline [pThis->cursor + i] == ' ') {
                pThis->cmdline [pThis->cursor + i] = 0;
            }
        }
        pThis->cursor += len;
        pThis->cmdlen += len;
        pThis->cmdline [pThis->cmdlen] = '\0';
        return true;
    }
    return false;
}

//*****************************************************************************
// remove one int8_t at cursor
static void microrl_backspace (microrl_t * pThis)
{
    if (pThis->cursor > 0) {
        terminal_backspace (pThis);
        memmove (pThis->cmdline + pThis->cursor-1,
                         pThis->cmdline + pThis->cursor,
                         pThis->cmdlen-pThis->cursor+1);
        pThis->cursor--;
        pThis->cmdline [pThis->cmdlen] = '\0';
        pThis->cmdlen--;
    }
}


#ifdef _USE_COMPLETE

//*****************************************************************************
static int32_t common_len (int8_t ** arr)
{
    int32_t len = 0;
    int32_t i = 1;
    while (1) {
        while (arr[i]!=NULL) {
            if ((arr[i][len] != arr[i-1][len]) ||
                    (arr[i][len] == '\0') ||
                    (arr[i-1][len]=='\0'))
                return len;
            len++;
        }
        i++;
    }
}

//*****************************************************************************
static void microrl_get_complite (microrl_t * pThis)
{
    int8_t ** compl_token;

    if (pThis->get_completion == NULL) // callback was not set
        return;

    int32_t status = split (pThis, pThis->cursor);
    if (pThis->cmdline[pThis->cursor-1] == '\0')
        pThis->tkn_arr[status++] = "";
    compl_token = pThis->get_completion (status, pThis->tkn_arr);
    if (compl_token[0] != NULL) {
        int32_t i = 0;
        int32_t len;

        if (compl_token[1] == NULL) {
            len = strlen (compl_token[0]);
        } else {
            len = common_len (compl_token);
            terminal_newline (pThis);
            while (compl_token [i] != NULL) {
                pThis->print (compl_token[i]);
                pThis->print (" ");
                i++;
            }
            terminal_newline (pThis);
            print_prompt (pThis);
        }

        if (len) {
            microrl_insert_text (pThis, compl_token[0] + strlen(pThis->tkn_arr[status-1]),
                                                                    len - strlen(pThis->tkn_arr[status-1]));
            if (compl_token[1] == NULL)
                microrl_insert_text (pThis, " ", 1);
        }
        terminal_reset_cursor (pThis);
        terminal_print_line (pThis, 0, pThis->cursor);
    }
}
#endif

//*****************************************************************************
void new_line_handler(microrl_t * pThis){
    int32_t status;

    terminal_newline (pThis);
#ifdef _USE_HISTORY
    if (pThis->cmdlen > 0)
        hist_save_line (&pThis->ring_hist, pThis->cmdline, pThis->cmdlen);
#endif
    status = split (pThis, pThis->cmdlen);
    if (status == -1){
        //          pThis->print ("ERROR: Max token amount exseed\n");
        pThis->print ("ERROR:too many tokens");
        pThis->print (ENDL);
    }
    if ((status > 0) && (pThis->execute != NULL))
        pThis->execute (status, pThis->tkn_arr);
    print_prompt (pThis);
    pThis->cmdlen = 0;
    pThis->cursor = 0;
    memset(pThis->cmdline, 0, _COMMAND_LINE_LEN);
#ifdef _USE_HISTORY
    pThis->ring_hist.cur = 0;
#endif
}

//*****************************************************************************
#if (defined(_ENDL_CRLF) || defined(_ENDL_LFCR))
    static int32_t tmpch = 0;
#endif

void microrl_insert_char (microrl_t * pThis, int32_t ch)
{
    // 07-08-2012 for blank console
    if (ch == KEY_ETX) {
        if (pThis->sigint != NULL) {
            pThis->sigint();
        }
        return;
    }

#ifdef _USE_ESC_SEQ
    static int32_t escape = false;

    if (escape) {
        if (escape_process(pThis, ch))
            escape = 0;
    } else {
#endif
        switch (ch) {
            //-----------------------------------------------------
#ifdef _ENDL_CR
            case KEY_CR:
                new_line_handler(pThis);
            break;
            case KEY_LF:
            break;
#elif defined(_ENDL_CRLF)
            case KEY_CR:
                tmpch = KEY_CR;
            break;
            case KEY_LF:
            if (tmpch == KEY_CR)
                new_line_handler(pThis);
            break;
#elif defined(_ENDL_LFCR)
            case KEY_LF:
                tmpch = KEY_LF;
            break;
            case KEY_CR:
            if (tmpch == KEY_LF)
                new_line_handler(pThis);
            break;
#else
            case KEY_CR:
            break;
            case KEY_LF:
                new_line_handler(pThis);
            break;
#endif
            //-----------------------------------------------------
#ifdef _USE_COMPLETE
            case KEY_HT:
                microrl_get_complite (pThis);
            break;
#endif
            //-----------------------------------------------------
            case KEY_ESC:
#ifdef _USE_ESC_SEQ
                escape = 1;
#endif
            break;
            //-----------------------------------------------------
            case KEY_NAK: // ^U
                    while (pThis->cursor > 0) {
                    microrl_backspace (pThis);
                }
                terminal_print_line (pThis, 0, pThis->cursor);
            break;
            //-----------------------------------------------------
            case KEY_VT:  // ^K
                pThis->print ("\033[K");
                pThis->cmdlen = pThis->cursor;
            break;
            //-----------------------------------------------------
            case KEY_ENQ: // ^E
                terminal_move_cursor (pThis, pThis->cmdlen-pThis->cursor);
                pThis->cursor = pThis->cmdlen;
            break;
            //-----------------------------------------------------
            case KEY_SOH: // ^A
                terminal_reset_cursor (pThis);
                pThis->cursor = 0;
            break;
            //-----------------------------------------------------
            case KEY_ACK: // ^F
            if (pThis->cursor < pThis->cmdlen) {
                terminal_move_cursor (pThis, 1);
                pThis->cursor++;
            }
            break;
            //-----------------------------------------------------
            case KEY_STX: // ^B
            if (pThis->cursor) {
                terminal_move_cursor (pThis, -1);
                pThis->cursor--;
            }
            break;
            //-----------------------------------------------------
            case KEY_DLE: //^P
#ifdef _USE_HISTORY
            hist_search (pThis, _HIST_UP);
#endif
            break;
            //-----------------------------------------------------
            case KEY_SO: //^N
#ifdef _USE_HISTORY
            hist_search (pThis, _HIST_DOWN);
#endif
            break;
            //-----------------------------------------------------
            case KEY_DEL: // Backspace
            case KEY_BS:  // ^U
                microrl_backspace (pThis);
                terminal_print_line (pThis, pThis->cursor, pThis->cursor);
            break;
#ifdef _USE_CTLR_C
            case KEY_ETX:
            if (pThis->sigint != NULL)
                pThis->sigint();
            break;
#endif
            //-----------------------------------------------------
            default:
            if ((ch == ' ') && (pThis->cmdlen == 0))
                break;
            if (microrl_insert_text (pThis, (int8_t*)&ch, 1))
                terminal_print_line (pThis, pThis->cursor-1, pThis->cursor);

            break;
        }
#ifdef _USE_ESC_SEQ
    }
#endif
}