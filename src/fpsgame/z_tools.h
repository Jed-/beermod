/*
 * function prototypes and structures used in mod
 */

#ifndef Z_TOOLS_H
#define Z_TOOLS_H

#include "z_tree.h"

// gban/pban struct optimised for use in tree
struct gbaninfo
{
    // in host byte order, because easier to compare
    uint first, last;

    void parse(const char *name)
    {
        union { uchar b[sizeof(enet_uint32)]; enet_uint32 i; } ipconv, maskconv;
        ipconv.i = 0;
        maskconv.i = 0;
        loopi(4)
        {
            char *end = NULL;
            int n = strtol(name, &end, 10);
            if(!end) break;
            if(end > name) { ipconv.b[i] = n; maskconv.b[i] = 0xFF; }
            name = end;
            while(int c = *name)
            {
                ++name;
                if(c == '.') break;
                if(c == '/')
                {
                    int range = clamp(int(strtol(name, NULL, 10)), 0, 32);
                    int mask = range ? 0xFFffFFff << (32 - range) : ENET_NET_TO_HOST_32(maskconv.i);
                    first = ENET_NET_TO_HOST_32(ipconv.i) & mask;
                    last = first | ~mask;
                    return;
                }
            }
        }
        first = ENET_NET_TO_HOST_32(ipconv.i & maskconv.i);
        last = ENET_NET_TO_HOST_32(ipconv.i | ~maskconv.i);
    }

    int print(char *buf) const
    {
        char *start = buf;
        union { uchar b[sizeof(enet_uint32)]; enet_uint32 i; } ipconv, maskconv;
        ipconv.i = ENET_HOST_TO_NET_32(first);
        maskconv.i = ENET_HOST_TO_NET_32(~(first ^ last));
        int lastdigit = -1;
        loopi(4) if(maskconv.b[i])
        {
            if(lastdigit >= 0) *buf++ = '.';
            loopj(i - lastdigit - 1) { *buf++ = '*'; *buf++ = '.'; }
            buf += sprintf(buf, "%d", ipconv.b[i]);
            lastdigit = i;
        }
        enet_uint32 bits = first ^ last;
        int range = 32;
        for(; (bits&0xFF) == 0xFF; bits >>= 8) range -= 8;
        for(; bits&1; bits >>= 1) --range;
        if(!bits && range%8) buf += sprintf(buf, "/%d", range);
        return int(buf-start);
    }

    inline int compare(const gbaninfo &b) const
    {
        if(b.last < first) return -1;
        if(b.first > last) return +1;
        return 0;   /* ranges overlap */
    }

    // host byte order
    inline int compare(uint ip) const
    {
        if(ip < first) return -1;
        if(ip > last)  return +1;
        return 0;
    }

    // host byte order
    bool check(uint ip) const { return (ip | (first ^ last)) == last; }
};

// basic pban struct with comments
struct pbaninfo: ipmask
{
    char *comment;
    time_t dateadded, lasthit;
    pbaninfo(): comment(NULL) {}
    ~pbaninfo() { delete[] comment; }
};

extern vector< z_avltree<gbaninfo> > gbans;
extern z_avltree<gbaninfo> ipbans;
extern vector<pbaninfo> sbans;


typedef void (* z_sleepfunc)(void *);
typedef void (* z_freevalfunc)(void *);

enum
{
    ZS_SLEEPS = 0,
    ZS_ANNOUNCES,
    ZS_NUM
};

struct z_sleepstruct
{
    int delay, millis;
    void *val;
    bool reload;
    z_sleepfunc func;
    z_freevalfunc freeval;

    z_sleepstruct() {}
    ~z_sleepstruct() { if(freeval) freeval(val); }
};

extern void z_checksleep();


extern void z_setteaminfos(hashset<teaminfo> *&dst, hashset<teaminfo> *src);


extern void forcespectator(clientinfo *ci);

extern int showbanreason;

extern clientinfo *getinfo(int n);

extern const char *privname(int type);
extern void sendservmsg(const char *s);
extern void sendservmsgf(const char *fmt, ...);
extern const char *colorname(clientinfo *ci, const char *name = NULL);
extern int findmaprotation(int mode, const char *map);

// z_log
extern void z_log_kick(clientinfo *actor, const char *aname, const char *adesc, int priv, clientinfo *victim, const char *reason);
extern void z_log_kickdone();
extern void z_showban(clientinfo *actor, const char *banstr, const char *victim, int bantime, const char *reason);


extern bool z_parseclient(const char *str, int &cn);
extern bool z_parseclient_verify(const char *str, int &cn, bool allowall, bool allowbot = false, bool allowspy = false);


extern bool z_autoeditmute;
extern int z_nodamage;

extern bool isracemode();
extern void race_maploaded(clientinfo *ci);


#endif // Z_TOOLS_H
