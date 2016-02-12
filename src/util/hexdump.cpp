/**
* LICENSE PLACEHOLDER
*
* @file hexdump.h
* @package OpenPST
* @brief hexdump helper functions
*
* @author Gassan Idriss <ghassani@gmail.com>
* @author https://github.com/posixninja/DLOADTool
*/

#include "hexdump.h"

void hexdump(unsigned char *data, unsigned int amount) {
    return hexdump(data, amount, stderr);
}

void hexdump(unsigned char *data, unsigned int amount, FILE* file) {
    unsigned int    dp, p;  /* data pointer */
    for (dp = 1; dp <= amount; dp++) {
        fprintf(file, "%02x ", data[dp - 1]);
        if ((dp % 8) == 0)
            fprintf(file, " ");
        if ((dp % 16) == 0) {
            fprintf(file, "| ");
            p = dp;
            for (dp -= 16; dp < p; dp++)
                fprintf(file, "%c", hex_trans_dump[data[dp]]);
            fflush(file);
            fprintf(file, "\n");
        }
        fflush(file);
    }
    // tail
    if ((amount % 16) != 0) {
        p = dp = 16 - (amount % 16);
        for (dp = p; dp > 0; dp--) {
            fprintf(file, "   ");
            if (((dp % 8) == 0) && (p != 8))
                fprintf(file, " ");
            fflush(file);
        }
        fprintf(file, " | ");
        for (dp = (amount - (16 - p)); dp < amount; dp++)
            fprintf(file, "%c", hex_trans_dump[data[dp]]);
        fflush(file);
    }
    fprintf(file, "\n");

    return;
}
#ifdef QT_CORE_LIB
void hexdump(unsigned char *data, unsigned int amount, QString& out, bool toHtml)
{
    unsigned int    dp, p;  /* data pointer */


    //hexdump(data, amount);

    QString tmp;

    for (dp = 1; dp <= amount; dp++) {
        tmp.sprintf("%02x ", data[dp - 1]);
        out.append(tmp);

        if ((dp % 8) == 0) {
            out.append(" ");
        }

        if ((dp % 16) == 0) {
            out.append("| ");
            p = dp;
            for (dp -= 16; dp < p; dp++) {
                tmp.sprintf("%c ", hex_trans_dump[data[dp]]);
                out.append(tmp);
            }
            if (toHtml) {
                out.append("<br>");
            } else {
                out.append("\n");
            }
        }
    }

    // tail
    if ((amount % 16) != 0) {
        p = dp = 16 - (amount % 16);
        for (dp = p; dp > 0; dp--) {
            out.append("   ");
            if (((dp % 8) == 0) && (p != 8))
                out.append(" ");
        }
        out.append(" | ");
        for (dp = (amount - (16 - p)); dp < amount; dp++)
            tmp.sprintf("%c ", hex_trans_dump[data[dp]]);
            out.append(tmp);
    }

    if (toHtml) {
        out = out.left(out.length() - 4);
    } else {
        out.append("\n");
    }

    return;
}
#endif
