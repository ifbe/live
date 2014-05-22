#define NJ_OK 0
#define NJ_NO_JPEG 1
#define NJ_UNSUPPORTED 2
#define NJ_OUT_OF_MEM 3
#define NJ_INTERNAL_ERR 4
#define NJ_SYNTAX_ERROR 5
#define __NJ_FINISHED 6


typedef struct _nj_code{
    unsigned char bits, code;
} nj_vlc_code_t;

typedef struct _nj_cmp {
    int cid;
    int ssx, ssy;
    int width, height;
    int stride;
    int qtsel;
    int actabsel, dctabsel;
    int dcpred;
    unsigned char *pixels;
} nj_component_t;

typedef struct _nj_ctx {
    int error;
    const unsigned char *pos;
    int size;
    int length;
    int width, height;
    int mbwidth, mbheight;
    int mbsizex, mbsizey;
    nj_component_t comp[3];
    int qtused, qtavail;
    unsigned char qtab[4][64];
    nj_vlc_code_t vlctab[4][65536];
    int buf, bufbits;
    int block[64];
    int rstinterval;
    unsigned char *rgb;
} nj_context_t;

static nj_context_t nj;
static const char njZZ[64]={ 0, 1, 8,16, 9, 2, 3,10,
                            17,24,32,25,18,11, 4, 5,
                            12,19,26,33,40,48,41,34,
                            27,20,13, 6, 7,14,21,28,
                            35,42,49,56,57,50,43,36,
                            29,22,15,23,30,37,44,51,
                            58,59,52,45,38,31,39,46,
                            53,60,61,54,47,55,62,63};

long long offset;

void picture()
{
    unsigned char* edi=(unsigned char*)0x1400000;
    unsigned char* p=nj.rgb;
    int i;

    for(i=0;i<nj.width*nj.height;i++)
    {
        *edi=*(p+2);
        *(edi+1)=*(p+1);
        *(edi+2)=*p;
        edi+=4;
        p+=3;
    }
}


void* mymalloc(int i)
{
    void* p;
    p=(void*)offset;
    offset+=i;
    return p;
}


void point(int x,int y,int z)
{
    long long* video=(long long*)0x3028;
    long long base=*video;
    char* p=(char*)0x3019;
    char bpp=*p/8;

    int* address;

    address=(int*)(base+(y*1024+x)*bpp);
    *address=z;
}


void print(int x,int y,char ch)
{
    int i,j;
    long long rsi=0x6000;
    char temp;
    char* p;

    rsi+=ch<<4;
    x=8*x;
    y=16*y;

    for(i=0;i<16;i++)
    {
        p=(char*)rsi;
        for(j=0;j<8;j++)
        {
            temp=*p;
            temp=temp<<j;
            temp&=0x80;
            if(temp!=0){point(j+x,i+y,0);}
            else{point(j+x,i+y,0x00ffff00);}

        }
    rsi++;
    }
}

void print32(int x,int y,int z)
{
    char ch;
    int i;

    for(i=7;i>=0;i--)
    {
        ch=(char)(z&0x0000000f);
        z=z>>4;
        print(x+i,y,ch);
    }
}


static inline unsigned char njClip(const int x) {
    return (x<0)?0:((x>0xff)?0xff:(unsigned char) x);
}

#define W1 2841
#define W2 2676
#define W3 2408
#define W5 1609
#define W6 1108
#define W7 565

static inline void njRowIDCT(int* blk) {
    int x0, x1, x2, x3, x4, x5, x6, x7, x8;
    if (!((x1 = blk[4] << 11)
        | (x2 = blk[6])
        | (x3 = blk[2])
        | (x4 = blk[1])
        | (x5 = blk[7])
        | (x6 = blk[5])
        | (x7 = blk[3])))
    {
        blk[0] = blk[1] = blk[2] = blk[3] = blk[4] = blk[5] = blk[6] = blk[7] = blk[0] << 3;
        return;
    }
    x0 = (blk[0] << 11) + 128;
    x8 = W7 * (x4 + x5);
    x4 = x8 + (W1 - W7) * x4;
    x5 = x8 - (W1 + W7) * x5;
    x8 = W3 * (x6 + x7);
    x6 = x8 - (W3 - W5) * x6;
    x7 = x8 - (W3 + W5) * x7;
    x8 = x0 + x1;
    x0 -= x1;
    x1 = W6 * (x3 + x2);
    x2 = x1 - (W2 + W6) * x2;
    x3 = x1 + (W2 - W6) * x3;
    x1 = x4 + x6;
    x4 -= x6;
    x6 = x5 + x7;
    x5 -= x7;
    x7 = x8 + x3;
    x8 -= x3;
    x3 = x0 + x2;
    x0 -= x2;
    x2 = (181 * (x4 + x5) + 128) >> 8;
    x4 = (181 * (x4 - x5) + 128) >> 8;
    blk[0] = (x7 + x1) >> 8;
    blk[1] = (x3 + x2) >> 8;
    blk[2] = (x0 + x4) >> 8;
    blk[3] = (x8 + x6) >> 8;
    blk[4] = (x8 - x6) >> 8;
    blk[5] = (x0 - x4) >> 8;
    blk[6] = (x3 - x2) >> 8;
    blk[7] = (x7 - x1) >> 8;
}

static inline void njColIDCT(const int* blk, unsigned char *out, int stride) {
    int x0, x1, x2, x3, x4, x5, x6, x7, x8;
    if (!((x1 = blk[8*4] << 8)
        | (x2 = blk[8*6])
        | (x3 = blk[8*2])
        | (x4 = blk[8*1])
        | (x5 = blk[8*7])
        | (x6 = blk[8*5])
        | (x7 = blk[8*3])))
    {
        x1 = njClip(((blk[0] + 32) >> 6) + 128);
        for (x0 = 8;  x0;  --x0) {
            *out = (unsigned char) x1;
            out += stride;
        }
        return;
    }
    x0 = (blk[0] << 8) + 8192;
    x8 = W7 * (x4 + x5) + 4;
    x4 = (x8 + (W1 - W7) * x4) >> 3;
    x5 = (x8 - (W1 + W7) * x5) >> 3;
    x8 = W3 * (x6 + x7) + 4;
    x6 = (x8 - (W3 - W5) * x6) >> 3;
    x7 = (x8 - (W3 + W5) * x7) >> 3;
    x8 = x0 + x1;
    x0 -= x1;
    x1 = W6 * (x3 + x2) + 4;
    x2 = (x1 - (W2 + W6) * x2) >> 3;
    x3 = (x1 + (W2 - W6) * x3) >> 3;
    x1 = x4 + x6;
    x4 -= x6;
    x6 = x5 + x7;
    x5 -= x7;
    x7 = x8 + x3;
    x8 -= x3;
    x3 = x0 + x2;
    x0 -= x2;
    x2 = (181 * (x4 + x5) + 128) >> 8;
    x4 = (181 * (x4 - x5) + 128) >> 8;
    *out = njClip(((x7 + x1) >> 14) + 128);  out += stride;
    *out = njClip(((x3 + x2) >> 14) + 128);  out += stride;
    *out = njClip(((x0 + x4) >> 14) + 128);  out += stride;
    *out = njClip(((x8 + x6) >> 14) + 128);  out += stride;
    *out = njClip(((x8 - x6) >> 14) + 128);  out += stride;
    *out = njClip(((x0 - x4) >> 14) + 128);  out += stride;
    *out = njClip(((x3 - x2) >> 14) + 128);  out += stride;
    *out = njClip(((x7 - x1) >> 14) + 128);
}

#define njThrow(e) do { nj.error = e; return; } while (0)

static int njShowBits(int bits) {
    unsigned char newbyte;
    if (!bits) return 0;
    while (nj.bufbits < bits) {
        if (nj.size <= 0) {
            nj.buf = (nj.buf << 8) | 0xFF;
            nj.bufbits += 8;
            continue;
        }
        newbyte = *nj.pos++;
        nj.size--;
        nj.bufbits += 8;
        nj.buf = (nj.buf << 8) | newbyte;
        if (newbyte == 0xFF) {
            if (nj.size) {
                unsigned char marker = *nj.pos++;
                nj.size--;
                switch (marker) {
                    case 0x00:
                    case 0xFF:
                        break;
                    case 0xD9: nj.size = 0; break;
                    default:
                        if ((marker & 0xF8) != 0xD0)
                            nj.error = NJ_SYNTAX_ERROR;
                        else {
                            nj.buf = (nj.buf << 8) | marker;
                            nj.bufbits += 8;
                        }
                }
            } else
                nj.error = NJ_SYNTAX_ERROR;
       }
    }
    return (nj.buf >> (nj.bufbits - bits)) & ((1 << bits) - 1);
}

static inline void njSkipBits(int bits) {
    if (nj.bufbits < bits)
        (void) njShowBits(bits);
    nj.bufbits -= bits;
}

static void njSkip(int count) {
    nj.pos += count;
    nj.size -= count;
    nj.length -= count;
    if (nj.size < 0) nj.error = NJ_SYNTAX_ERROR;
}

static void njDecodeLength(void) {
    if (nj.size < 2) njThrow(NJ_SYNTAX_ERROR);
    nj.length = *nj.pos<<8|*(nj.pos+1);
    if (nj.length > nj.size) njThrow(NJ_SYNTAX_ERROR);
    njSkip(2);
}

static inline void njDecodeSOF(void) {
    int i, ssxmax = 0, ssymax = 0;
    nj_component_t* c;
    njDecodeLength();
    if (nj.length < 9) njThrow(NJ_SYNTAX_ERROR);
    if (nj.pos[0] != 8) njThrow(NJ_UNSUPPORTED);
    nj.height = *(nj.pos+1)<<8|*(nj.pos+2);
    nj.width = *(nj.pos+3)<<8|*(nj.pos+4);

    if(nj.pos[5]!=3)njThrow(NJ_UNSUPPORTED);
    njSkip(6);

    if (nj.length <9) njThrow(NJ_SYNTAX_ERROR);
    for (i = 0, c = nj.comp;  i <3;  ++i, ++c) {
        c->cid = nj.pos[0];
        if (!(c->ssx = nj.pos[1] >> 4)) njThrow(NJ_SYNTAX_ERROR);
        if (c->ssx & (c->ssx - 1)) njThrow(NJ_UNSUPPORTED);  // non-power of two
        if (!(c->ssy = nj.pos[1] & 15)) njThrow(NJ_SYNTAX_ERROR);
        if (c->ssy & (c->ssy - 1)) njThrow(NJ_UNSUPPORTED);  // non-power of two
        if ((c->qtsel = nj.pos[2]) & 0xFC) njThrow(NJ_SYNTAX_ERROR);
        njSkip(3);
        nj.qtused |= 1 << c->qtsel;
        if (c->ssx > ssxmax) ssxmax = c->ssx;
        if (c->ssy > ssymax) ssymax = c->ssy;
    }
    nj.mbsizex = ssxmax << 3;
    nj.mbsizey = ssymax << 3;
    nj.mbwidth = (nj.width + nj.mbsizex - 1) / nj.mbsizex;
    nj.mbheight = (nj.height + nj.mbsizey - 1) / nj.mbsizey;
    c=nj.comp;
    for(i = 0;i<3;++i,++c) 
    {
        c->width = (nj.width * c->ssx + ssxmax - 1) / ssxmax;
        c->stride = (c->width + 7) & 0x7FFFFFF8;
        c->height = (nj.height * c->ssy + ssymax - 1) / ssymax;
        c->stride = nj.mbwidth * nj.mbsizex * c->ssx / ssxmax;
        if (((c->width<3)&&(c->ssx!=ssxmax)) || ((c->height<3)&&(c->ssy!=ssymax))) njThrow(NJ_UNSUPPORTED);
        c->pixels= mymalloc(c->stride*(nj.mbheight*nj.mbsizey*c->ssy/ssymax));
    }
        nj.rgb = mymalloc(nj.width*nj.height*3);
    njSkip(nj.length);
}

static inline void njDecodeDHT(void) {
    int codelen, currcnt, remain, spread, i, j;
    nj_vlc_code_t *vlc;
    static unsigned char counts[16];
    njDecodeLength();
    while (nj.length >= 17) {
        i = nj.pos[0];
        if (i & 0xEC) njThrow(NJ_SYNTAX_ERROR);
        if (i & 0x02) njThrow(NJ_UNSUPPORTED);
        i = (i | (i >> 3)) & 3;  // combined DC/AC + tableid value
        for (codelen = 1;  codelen <= 16;  ++codelen)
            counts[codelen - 1] = nj.pos[codelen];
        njSkip(17);
        vlc = &nj.vlctab[i][0];
        remain = spread = 65536;
        for (codelen = 1;  codelen <= 16;  ++codelen) {
            spread >>= 1;
            currcnt = counts[codelen - 1];
            if (!currcnt) continue;
            if (nj.length < currcnt) njThrow(NJ_SYNTAX_ERROR);
            remain -= currcnt << (16 - codelen);
            if (remain < 0) njThrow(NJ_SYNTAX_ERROR);
            for (i = 0;  i < currcnt;  ++i) {
                register unsigned char code = nj.pos[i];
                for (j = spread;  j;  --j) {
                    vlc->bits = (unsigned char) codelen;
                    vlc->code = code;
                    ++vlc;
                }
            }
            njSkip(currcnt);
        }
        while (remain--) {
            vlc->bits = 0;
            ++vlc;
        }
    }
    if (nj.length) njThrow(NJ_SYNTAX_ERROR);
}

static inline void njDecodeDQT(void) {
    int i;
    unsigned char *t;
    njDecodeLength();
    while (nj.length >= 65) {
        i = nj.pos[0];
        if (i & 0xFC) njThrow(NJ_SYNTAX_ERROR);
        nj.qtavail |= 1 << i;
        t = &nj.qtab[i][0];
        for (i = 0;  i < 64;  ++i)
            t[i] = nj.pos[i + 1];
        njSkip(65);
    }
    if (nj.length) njThrow(NJ_SYNTAX_ERROR);
}

static inline void njDecodeDRI(void) {
    njDecodeLength();
    if (nj.length < 2) njThrow(NJ_SYNTAX_ERROR);
    nj.rstinterval = *nj.pos<<8|*(nj.pos+1);
    njSkip(nj.length);
}

static int njGetVLC(nj_vlc_code_t* vlc, unsigned char* code) {
    int value = njShowBits(16);
    int bits = vlc[value].bits;
    if (!bits) { nj.error = NJ_SYNTAX_ERROR; return 0; }

    if (nj.bufbits < bits)
        (void) njShowBits(bits);
    nj.bufbits -= bits;

    value = vlc[value].code;
    if (code) *code = (unsigned char) value;
    bits = value & 15;
    if (!bits) return 0;
    value = njShowBits(bits);

    if (nj.bufbits < bits)
        (void) njShowBits(bits);
    nj.bufbits -= bits;

    if (value < (1 << (bits - 1)))
        value += ((-1) << bits) + 1;
    return value;
}

static inline void njDecodeBlock(nj_component_t* c, unsigned char* out) {
    unsigned char code = 0;
    int value, coef = 0;

    for(value=0;value<64;value++){nj.block[value]=0;}
    c->dcpred += njGetVLC(&nj.vlctab[c->dctabsel][0],0);
    nj.block[0] = (c->dcpred) * nj.qtab[c->qtsel][0];

    do {
        value = njGetVLC(&nj.vlctab[c->actabsel][0], &code);
        if (!code) break;  // EOB
        if (!(code & 0x0F) && (code != 0xF0)) njThrow(NJ_SYNTAX_ERROR);
        coef += (code >> 4) + 1;
        if (coef > 63) njThrow(NJ_SYNTAX_ERROR);
        nj.block[(int) njZZ[coef]] = value * nj.qtab[c->qtsel][coef];
    } while (coef < 63);

    for (coef = 0;  coef < 64;  coef += 8)
        njRowIDCT(&nj.block[coef]);
    for (coef = 0;  coef < 8;  ++coef)
        njColIDCT(&nj.block[coef], &out[coef], c->stride);
}

static inline void njDecodeScan(void) {
    int i, mbx, mby, sbx, sby;
    int rstcount = nj.rstinterval, nextrst = 0;
    nj_component_t* c;
    njDecodeLength();
    if (nj.length < 10) njThrow(NJ_SYNTAX_ERROR);
    if (nj.pos[0] != 3) njThrow(NJ_UNSUPPORTED);
    njSkip(1);

    for (i = 0, c = nj.comp;  i < 3;  ++i, ++c) {
        if (nj.pos[0] != c->cid) njThrow(NJ_SYNTAX_ERROR);
        if (nj.pos[1] & 0xEE) njThrow(NJ_SYNTAX_ERROR);
        c->dctabsel = nj.pos[1] >> 4;
        c->actabsel = (nj.pos[1] & 1) | 2;
        njSkip(2);
    }

    if (nj.pos[0] || (nj.pos[1] != 63) || nj.pos[2]) njThrow(NJ_UNSUPPORTED);
    njSkip(nj.length);

    for (mbx = mby = 0;;) {

        for (i = 0, c = nj.comp;  i < 3;  ++i, ++c)
            for (sby = 0;  sby < c->ssy;  ++sby)
                for (sbx = 0;  sbx < c->ssx;  ++sbx)
                {
			njDecodeBlock(c,&c->pixels[((mby*c->ssy+sby)*c->stride+mbx*c->ssx+sbx)<<3]);
			if (nj.error){return;}
                }

        if (++mbx >= nj.mbwidth) {
            mbx = 0;
            if (++mby >= nj.mbheight) break;
        }

        if (nj.rstinterval && !(--rstcount)) {
            nj.bufbits &= 0xF8;

            i = njShowBits(16);
            if (nj.bufbits < 16)
                (void) njShowBits(16);
            nj.bufbits -= 16;

            if (((i & 0xFFF8) != 0xFFD0) || ((i & 7) != nextrst)) njThrow(NJ_SYNTAX_ERROR);
            nextrst = (nextrst + 1) & 7;
            rstcount = nj.rstinterval;
            for (i = 0;  i < 3;  ++i)	nj.comp[i].dcpred = 0;
        }

    }
    nj.error = __NJ_FINISHED;
}


static inline void njConvert()
{
    int i;
    nj_component_t* c;
    for (i = 0, c = nj.comp;  i < 3;  ++i, ++c)
    {
         if ((c->width < nj.width) || (c->height < nj.height))
         {
             int x,y,xshift=0,yshift=0;
             unsigned char *out, *lin, *lout;
             while (c->width < nj.width) { c->width <<= 1; ++xshift; }
             while (c->height < nj.height) { c->height <<= 1; ++yshift; }

             out = mymalloc(c->width * c->height);

             lin = c->pixels;
             lout = out;
             for (y = 0;  y < c->height;  ++y)
             {
                 lin=&c->pixels[(y>>yshift)*c->stride];
                 for(x=0;x<c->width;++x)lout[x]=lin[x>>xshift];
                 lout += c->width;
             }
             c->stride=c->width;
             //free(c->pixels);
             c->pixels = out;
         }

        if ((c->width<nj.width)||(c->height<nj.height))njThrow(NJ_INTERNAL_ERR);
    }

        int x, yy;
        unsigned char *prgb = nj.rgb;
        const unsigned char *py  = nj.comp[0].pixels;
        const unsigned char *pcb = nj.comp[1].pixels;
        const unsigned char *pcr = nj.comp[2].pixels;
        for(yy=nj.height;yy;--yy)
        {
            for (x = 0;  x < nj.width;  ++x)
            {
                register int y = py[x] << 8;
                register int cb = pcb[x] - 128;
                register int cr = pcr[x] - 128;
                *prgb++ = njClip((y       +359*cr+128)>>8);
                *prgb++ = njClip((y- 88*cb-183*cr+128)>>8);
                *prgb++ = njClip((y+454*cb       +128)>>8);
            }
            py += nj.comp[0].stride;
            pcb += nj.comp[1].stride;
            pcr += nj.comp[2].stride;
        }
}

int njDecode(const void* jpeg, const int size) {
    nj.pos = (const unsigned char*) jpeg;
    nj.size = size & 0x7FFFFFFF;
    if (nj.size < 2) return NJ_NO_JPEG;
    if ((nj.pos[0] ^ 0xFF) | (nj.pos[1] ^ 0xD8)) return NJ_NO_JPEG;
    njSkip(2);
    while (!nj.error)
    {
        if ((nj.size < 2)||(nj.pos[0]!= 0xFF)) return NJ_SYNTAX_ERROR;

        njSkip(2);			//ffd8
        switch (nj.pos[-1]) {
            case 0xC0: njDecodeSOF();  break;
            case 0xC4: njDecodeDHT();  break;
            case 0xDB: njDecodeDQT();  break;
            case 0xDD: njDecodeDRI();  break;
            case 0xDA: njDecodeScan(); break;
            case 0xFE:{
                      njDecodeLength();
                      njSkip(nj.length);
                      break;
            }
            default:{
                if ((nj.pos[-1] & 0xF0) == 0xE0)
                {
                    njDecodeLength();
                    njSkip(nj.length);
                }
                else
                    return NJ_UNSUPPORTED;
            }
        }
    }
    if (nj.error != __NJ_FINISHED) return nj.error;
    nj.error = NJ_OK;
    njConvert();
    return nj.error;
}


void main()
{
    offset=0x800000;
    int i=0x12345678;
    i=njDecode((char*)(long long)where(),size());
    print32(0,0,i);
    picture();
}
