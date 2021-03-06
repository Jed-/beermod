#ifndef Z_MAPLOADED_H
#define Z_MAPLOADED_H

#include "z_patchmap.h"

VAR(mapload_debug, 0, 0, 1);

static void z_maploaded(clientinfo *ci, bool val = true)
{
    if(val && !ci->xi.maploaded)
    {
        ci->xi.maploaded = totalmillis ? totalmillis : 1;
        if(mapload_debug) sendservmsgf("%s has loaded map", colorname(ci));
        z_sendallpatches(ci);
    }
    else if(!val) ci->xi.maploaded = 0;

    if(val) race_maploaded(ci);
}

#endif // Z_MAPLOADED_H
