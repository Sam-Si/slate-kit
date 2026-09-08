#include <iostream>
#include <cmath>
#include <cstring>
#include <algorithm>
#include <cstdint>
using namespace std;

namespace tinf {
struct Tree { unsigned short table[16]; unsigned short trans[288]; };
static const unsigned char clcidx[] = {16,17,18,0,8,7,9,6,10,5,11,4,12,3,13,2,14,1,15};
static unsigned char lbits[30], dbits[30];
static unsigned short lbase[30], dbase[30];
static Tree slt, sdt;
static bool inited = false;
struct Src { const unsigned char *s; int p; int len; };
static unsigned int getbit(Src &d) {
    unsigned int v = (d.s[d.p >> 3] >> (d.p & 7)) & 1; d.p++; return v;
}
static unsigned int getbits(Src &d, int n) {
    unsigned int v = 0; for (int i = 0; i < n; i++) v |= getbit(d) << i; return v;
}
static void build(Tree &t, const unsigned char *lens, int n) {
    unsigned short offs[16]; int i;
    for (i = 0; i < 16; i++) t.table[i] = 0;
    for (i = 0; i < n; i++) t.table[lens[i]]++;
    t.table[0] = 0; offs[0] = 0;
    unsigned int sum = 0;
    for (i = 1; i < 16; i++) { offs[i] = sum; sum += t.table[i]; }
    for (i = 0; i < n; i++) if (lens[i]) t.trans[offs[lens[i]]++] = i;
}
static int decode(Src &d, Tree &t) {
    int sum = 0, cur = 0, len = 0;
    do {
        cur = 2 * cur + getbit(d); len++;
        sum += t.table[len]; cur -= t.table[len];
    } while (cur >= 0);
    return t.trans[sum + cur];
}
static void init() {
    if (inited) return; inited = true;
    int i; unsigned char lens[288];
    for (i = 0; i < 144; i++) lens[i] = 8;
    for (; i < 256; i++) lens[i] = 9;
    for (; i < 280; i++) lens[i] = 7;
    for (; i < 288; i++) lens[i] = 8;
    build(slt, lens, 288);
    for (i = 0; i < 30; i++) lens[i] = 5;
    build(sdt, lens, 30);
    for (i = 0; i < 30; i++) lbits[i] = dbits[i] = 0;
    for (i = 0; i < 30; i++) { if (i >= 8 && i < 28) lbits[i] = (i - 4) / 4; }
    lbase[0]=3; for (i=1;i<30;i++) lbase[i]=lbase[i-1]+(1<<lbits[i-1]);
    for (i = 2; i < 30; i++) dbits[i] = (i - 2) / 2;
    dbase[0]=1; for (i=1;i<30;i++) dbase[i]=dbase[i-1]+(1<<dbits[i-1]);
}
static void block(Src &d, Tree &lt, Tree &dt, unsigned char *out, int &op) {
    for (;;) {
        int sym = decode(d, lt);
        if (sym == 256) return;
        if (sym < 256) { out[op++] = sym; }
        else {
            sym -= 257;
            int length = lbase[sym] + getbits(d, lbits[sym]);
            int dist_sym = decode(d, dt);
            int dist = dbase[dist_sym] + getbits(d, dbits[dist_sym]);
            for (int i = 0; i < length; i++) { out[op] = out[op - dist]; op++; }
        }
    }
}
int inflate(const unsigned char *src, int srclen, unsigned char *dst, int dstcap) {
    init();
    Src d; d.s = src; d.p = 0; d.len = srclen;
    int op = 0, bfinal;
    do {
        bfinal = getbit(d);
        int btype = getbits(d, 2);
        if (btype == 0) {
            d.p = (d.p + 7) & ~7;
            int len = d.s[d.p/8] | (d.s[d.p/8+1]<<8); d.p += 32;
            for (int i = 0; i < len; i++) { dst[op++] = d.s[d.p/8]; d.p += 8; }
        } else if (btype == 1) {
            block(d, slt, sdt, dst, op);
        } else {
            int hlit = getbits(d, 5) + 257;
            int hdist = getbits(d, 5) + 1;
            int hclen = getbits(d, 4) + 4;
            unsigned char clens[19] = {};
            for (int i = 0; i < hclen; i++) clens[clcidx[i]] = getbits(d, 3);
            Tree clt; build(clt, clens, 19);
            unsigned char lens[316] = {};
            int n = 0;
            while (n < hlit + hdist) {
                int s = decode(d, clt);
                if (s < 16) { lens[n++] = s; }
                else if (s == 16) { int c = getbits(d, 2) + 3; for (int i = 0; i < c; i++) lens[n] = lens[n-1], n++; }
                else if (s == 17) { int c = getbits(d, 3) + 3; for (int i = 0; i < c; i++) lens[n++] = 0; }
                else { int c = getbits(d, 7) + 11; for (int i = 0; i < c; i++) lens[n++] = 0; }
            }
            Tree dlt, ddt;
            build(dlt, lens, hlit);
            build(ddt, lens + hlit, hdist);
            block(d, dlt, ddt, dst, op);
        }
    } while (!bfinal);
    return op;
}
} // namespace tinf

static const char*B85="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!#$%&()*+-;<=>?@^_`{|}~";
static uint8_t bv[128];
void bi(){for(int i=0;i<85;i++)bv[(uint8_t)B85[i]]=i;}
int bd(const char*s,int len,uint8_t*o){
bi();int n=0;
for(int i=0;i+4<len;i+=5){
uint32_t v=bv[(uint8_t)s[i]];
v=v*85+bv[(uint8_t)s[i+1]];v=v*85+bv[(uint8_t)s[i+2]];
v=v*85+bv[(uint8_t)s[i+3]];v=v*85+bv[(uint8_t)s[i+4]];
o[n++]=(v>>24)&0xFF;o[n++]=(v>>16)&0xFF;o[n++]=(v>>8)&0xFF;o[n++]=v&0xFF;}
return n;}
union F16{uint16_t u;};float f16(uint16_t h){uint32_t s=(h>>15)&1,e=(h>>10)&0x1F,m=h&0x3FF;if(e==0){if(m==0)return s?-0.0f:0.0f;float f=m/1024.0f;f*=1.0f/(1<<14);return s?-f:f;}if(e==31)return s?-1e30f:1e30f;float f=(1.0f+m/1024.0f);for(int i=0;i<(int)e-15;i++)f*=2.0f;for(int i=0;i<15-(int)e;i++)f*=0.5f;return s?-f:f;}
constexpr int W=192,OBS=128,NACT=81,APOD=9;
float w0[OBS*W],b0[W],w1[W*W],b1[W],w2[W*NACT],b2[NACT];
void lw(const uint8_t*d){
// Per-column scales at offset 77457
const uint8_t*sp=d+77457;
auto rs=[&](int i){return f16((uint16_t)(sp[i*2]|(sp[i*2+1]<<8)));};int off=0;
for(int c=0;c<192;c++){float s=rs(c);for(int r=0;r<OBS;r++)w0[r*W+c]=((float)d[off++]-127.0f)*s;}
for(int c=0;c<192;c++){float s=rs(192+c);for(int r=0;r<W;r++)w1[r*W+c]=((float)d[off++]-127.0f)*s;}
for(int c=0;c<81;c++){float s=rs(384+c);for(int r=0;r<W;r++)w2[r*NACT+c]=((float)d[off++]-127.0f)*s;}
{float s=rs(465);for(int i=0;i<W;i++)b0[i]=((float)d[off++]-127.0f)*s;}
{float s=rs(466);for(int i=0;i<W;i++)b1[i]=((float)d[off++]-127.0f)*s;}
{float s=rs(467);for(int i=0;i<NACT;i++)b2[i]=((float)d[off++]-127.0f)*s;}
}
void fwd(const float*obs,float*logits){
float h1[W],h2[W];
for(int j=0;j<W;j++){float s=b0[j];for(int i=0;i<OBS;i++)s+=obs[i]*w0[i*W+j];h1[j]=s>0?s:0;}
for(int j=0;j<W;j++){float s=b1[j];for(int i=0;i<W;i++)s+=h1[i]*w1[i*W+j];h2[j]=s>0?s:0;}
for(int j=0;j<NACT;j++){float s=b2[j];for(int i=0;i<W;i++)s+=h2[i]*w2[i*NACT+j];logits[j]=s;}
}
constexpr double PI=3.14159265358979323846,D2R=PI/180.0;
constexpr double MAX_ROT=18.0*D2R;
constexpr double MW=16000.0,MH=9000.0,MV=800.0,MD=18358.0,SM=4.0;
// 9-action: 0-2=coast[-18,0,+18], 3-5=thrust[-18,0,+18], 6-8=shield[-18,0,+18]
constexpr double RT[9]={-MAX_ROT,0.0,MAX_ROT,-MAX_ROT,0.0,MAX_ROT,-MAX_ROT,0.0,MAX_ROT};
constexpr int TT[9]={0,0,0,200,200,200,0,0,0};
constexpr bool SH[9]={false,false,false,false,false,false,true,true,true};
struct Pod{double x,y,vx,vy,a;int nc,sh,bu;};Pod P[4];
double cx[30],cy[30],gx[60],gy[60];
int nc,la,ng,npl;
int to[2],lc[4],gc[4],ms[2],mb[2],tn;
void it(){for(int i=0;i<4;i++){lc[i]=-1;gc[i]=1;}tn=0;ms[0]=ms[1]=mb[0]=mb[1]=0;to[0]=to[1]=100;}
void ut(int i,int l){if(lc[i]>=0&&l!=lc[i]){gc[i]++;if(i<2)to[0]=100;else to[1]=100;}lc[i]=l;}
double sc[4];
void cs(){for(int i=0;i<4;i++){int ci=min(gc[i],ng-1);
double dx=P[i].x-gx[ci],dy=P[i].y-gy[ci];
int pi=(i<2)?0:1;
if(to[pi]<=0)sc[i]=-1e9;else sc[i]=gc[i]*50000.0-sqrt(dx*dx+dy*dy);}}
// Observation: 128 dims matching training/physics.py observation()
void bo(float*o,int pod){  // pod=0 or 1 (our pods)
int od[4]; // ego, team, opp0, opp1
if(pod==0){od[0]=0;od[1]=1;od[2]=2;od[3]=3;}
else{od[0]=1;od[1]=0;od[2]=2;od[3]=3;}
double px[4],py[4],vx[4],vy[4],ag[4];
int np[4],sh[4],bu[4];
for(int i=0;i<4;i++){int j=od[i];
px[i]=P[j].x;py[i]=P[j].y;vx[i]=P[j].vx;vy[i]=P[j].vy;
ag[i]=P[j].a;np[i]=gc[j];sh[i]=P[j].sh;bu[i]=P[j].bu;}
double mc=(double)max(ng,1);int x=0;int pl=min(ng+2,ng+3-1);
for(int i=0;i<4;i++)o[x++]=px[i]/MW;
for(int i=0;i<4;i++)o[x++]=py[i]/MH;
for(int i=0;i<4;i++)o[x++]=vx[i]/MV;
for(int i=0;i<4;i++)o[x++]=vy[i]/MV;
for(int i=0;i<4;i++)o[x++]=cos(ag[i]);
for(int i=0;i<4;i++)o[x++]=sin(ag[i]);
for(int i=0;i<4;i++)o[x++]=(float)np[i]/(float)mc;
for(int i=0;i<4;i++)o[x++]=(float)sh[i]/SM;
for(int i=0;i<4;i++)o[x++]=bu[i]==0?1.0f:0.0f;
for(int p=0;p<4;p++)for(int c=0;c<2;c++){
int g=min(np[p]+c,pl);double dx=(gx[g]-px[p])/MD,dy=(gy[g]-py[p])/MD;
double d=sqrt(dx*dx+dy*dy),tc=atan2(gy[g]-py[p],gx[g]-px[p]),b=tc-ag[p];
o[x++]=dx;o[x++]=dy;o[x++]=d;o[x++]=cos(b);o[x++]=sin(b);}
int pa[]={0,0,0,1,1,2},pb[]={1,2,3,2,3,3};
for(int k=0;k<6;k++){double dx=px[pa[k]]-px[pb[k]],dy=py[pa[k]]-py[pb[k]];
o[x++]=sqrt(dx*dx+dy*dy)/MD;}
int m1=max(np[0],np[1]),m2=max(np[2],np[3]);
o[x++]=to[0]/100.0f;o[x++]=to[1]/100.0f;
o[x++]=(float)(m1-m2)/(float)mc;
for(int p=0;p<4;p++){int g0=min(np[p],pl),g1=min(np[p]+1,pl);
double dx=gx[g1]-gx[g0],dy=gy[g1]-gy[g0],ea=atan2(dy,dx),r=ea-ag[p];
o[x++]=cos(r);o[x++]=sin(r);}
for(int k=0;k<6;k++){double dx=px[pb[k]]-px[pa[k]],dy=py[pb[k]]-py[pa[k]];
double d=sqrt(dx*dx+dy*dy)+1e-6,ux=dx/d,uy=dy/d;
double dv=vx[pb[k]]-vx[pa[k]],dw=vy[pb[k]]-vy[pa[k]];
o[x++]=-(dv*ux+dw*uy)/MV;}
for(int c=0;c<4;c++){int c0=min(np[0]+c,pl),c1=min(np[0]+c+1,pl),c2=min(np[0]+c+2,pl);
double d1x=gx[c1]-gx[c0],d1y=gy[c1]-gy[c0],d2x=gx[c2]-gx[c1],d2y=gy[c2]-gy[c1];
double ta=atan2(d2y,d2x)-atan2(d1y,d1x);o[x++]=cos(ta);o[x++]=sin(ta);}
for(int i=2;i<=3;i++){double dx=px[0]-px[i],dy=py[0]-py[i];
double d=sqrt(dx*dx+dy*dy)+1e-6;
o[x++]=cos(ag[i])*(dx/d)+sin(ag[i])*(dy/d);}
cs();double os[4];for(int i=0;i<4;i++)os[i]=sc[od[i]];
float er=0;for(int i=1;i<4;i++)if(os[i]>os[0])er+=1.0f;er/=3.0f;
double ob=max(os[2],os[3]);float or2=0;
for(int i=0;i<4;i++)if(os[i]>ob)or2+=1.0f;or2/=3.0f;
o[x++]=er;o[x++]=or2;
for(int p=0;p<4;p++){int g=min(np[p],ng-1);
double dx=gx[g]-px[p],dy=gy[g]-py[p],d=sqrt(dx*dx+dy*dy),ds=d+1e-6;
double st=vx[p]*(dx/ds)+vy[p]*(dy/ds);
double eta=(st>1.0)?d/st:50.0;
o[x++]=max(0.0f,min(1.0f,(float)(eta/50.0)));}
for(int p=0;p<4;p++){double s=sqrt(vx[p]*vx[p]+vy[p]*vy[p])+1e-6;
o[x++]=(vx[p]*cos(ag[p])+vy[p]*sin(ag[p]))/s;}
int oi=(os[2]>=os[3])?2:3;int ce=min(np[0],ng-1);
double otx=gx[ce]-px[oi],oty=gy[ce]-py[oi];
double oa=atan2(oty,otx),ob2=oa-ag[oi];
o[x++]=cos(ob2);o[x++]=sin(ob2);
o[x++]=(sh[0]==4)?1.0f:0.0f;o[x++]=(float)bu[0];
for(int p=0;p<4;p++)o[x++]=sqrt(vx[p]*vx[p]+vy[p]*vy[p])/MV;
float tr=max(ng-1,1);o[x++]=max(0.0f,min(1.0f,(float)(tr-np[0])/tr));
}
// Weights
static const char WTS[]=
"M7(K~W!F_E_`0f_t}d#8c0pIUfub-p24jO=p0GShwk2yyYsi#xRAxp-#vJkHd+(X&d*2juM9!I#Qc`M`JWIy&h{3pQY$%}GPz@AA!;gkly;v-&iPhh}Ay%an"
"85u9$yZ794_Wt(w?Y+;@$+I$Xu9-BM@|uZ)SD`FbQj?wVe<-W0Va=K1`^-(3Rx@8P9c>EL#^$|0ls0EqO1xSWCa|c&ROu7@S8VfmkhXY~rSMeHzVbzZi?gB;"
"&hST4d$4G}6U$e{@HYE!FwSVDjMd61C5&*2{-vE2(%N84>z#8}uR3L%W62n!=>_AI9T=r->6~)f(kQJn9el($uI2cYu3s?tlrt(SgR3s-U?<)Or*kuuS|!_("
"Ra8QcRAw$|@AgzMHyNS&UOH|p_liktq(1(J!|@YrS7)V{%6sdLl6G1t&joIc*DU_Vz%4$cv=C;1IgwgAl}WFi#HGEYKLs9AbETv=URa?rE!XT>In+IMPTp<y"
"%&W{Pby+=W<{dsFI&LD3SFzJC_+U26wDVqBTo!lzR#;K;@}`y2;yu<|rJT6TOA}kHDUCEvVt}*wrSuA)lioG8mO*knAZ~AzF*LZsC$r_|-671-ANb54k|*W8"
"W`Oy{5~`vecG+08H{3C4z4it-lek&)X(wfVonu7aNty%P?)6?;QCQ3$CI~Yux1_|49Oek?G*pGl6?3?Xl~Ee5@%$VMj29$bC354miIT@?Y`Gq1!$W?Mf*1Yq"
"JO4drDRPd<6FPd~vgVDnjTc1ZRtMOd;X*uNVK+Bjcxy3=F7W8(V=hV`qUX{@-tgDXq^eo9-I_o97slObKfxoGO~ab-ybiXAQIh#t?Km$OH-#=FZXq`p$AU2$"
"7VJq|r(vN(ldNJ%k%iH^J1h5;w{~@Hr5~MQcuf_!xK6mDhJ{vYR19#FRXf6D(?r5bc5Kf!?G_(%&fX9f!tch<#Ztu3VuGEJr)Ab>>cPKkJn-^|)mrm$!=2(P"
"+7Q3Yp6y<|cyx)cE2b64<JqW|u?ptc_Q7-8HZm-|xF9e?KGMW-^LWSxGb)-99F6akVU@4&LbxdKDR}*sV5;OL<2;4ZOjnEU2gQ*2oSD<j#)}hMoL`T;?7LL5"
"lyuJ0a>!j-EaFQz%WTr&zcHrepPpR*?jxxfS6-x}urAkIEK^Y`v0+!X-(IBR?QEyWO4ENo+braDc}PF3@3(&w8!*wk>>rA!RNFotm=fZ{epd)ln4|A*&$aJ-"
"1Ml#t;b;P@WdHad;C^3yS>y49aX~t~-W}$pj&mdUIS*FA!`e~=PCq&m?D^2vTI6a~Mo~n=+kV7(*AC>w^mN02Ls$M|W;#^vt^UaXgVtw(AMgXV`b&?hhh-C{"
"4O5l@4itp&O&do2EC~3S49d_rUT{K*ldJOrUJ-=XvL^)`Gzc495X+$rzY{_V1^_5c?t)TLx=`WWL!s1q2(AZTWm1cU-Zbtpv>_$io7#H<7QCo~3G~NFA!6F?"
"hfrc5(W>Bsk;1BNc%uIS`}p)2@Pctg5t+<!|A27;CKh)GVS>9#GZ<KN6zI4$G?@*a^EI_3xaTJE1ow+Uo@}ms6c^1!r#~<L?{)55R~})S8@L-cDx6JsEFCvH"
"ZX8V?i@=$M%LtKtT=Y+_;Td0IaOKO9D<1J{*dgEZ?0^&*<C60WN$S)3PHtJO>~_k9irevP>NZ(0Be<H44wknbe%6i0<w&<q9augWcnfBZ3&jzfHJ?4yImp|5"
"5M?2R<H|%H_l%70>elV+(KtA(<_B0bDFRVAxBOr7&G`#M;kh4#dz+fA#k27O;1pqCsFQHAuQyD<M73ehrU>0VId6hDsk8HA>tlZffNTkA0DRWWE_7zLU6Js9"
"1$Ya=V7bT2hw!vl+bxSQ6THvyaf$;07UJ&_!;aIvz}00fM%ruzX9v+cl~OV=Nn7z%6&b)yG0(!H{1MoNrYkM}fo<q&v?W}M)*{#jo=VYKEY#9yTLjI3dnHW$"
"KPsKlk1TIiPjX=yGv!(`wm?w8KvVh6`1xN(w04(7>C)S|p4X3g8MOme0-}PAG-WKoyMxzdch;WPH$@purPUvQchB(qw(d#Xx+D@uq~1Cxj>WCHjL1VkXqK9@"
"X9U95?fw67;zP{sZZjEIwG=89oOuv~1>P&~(+@c#Pujt4fo*u<5w8LIlFKzCDG-<(W=pJKkgBR7I|@JQ<>9vm=P<+YnjAqNe1Y7pTS&mPL8#I6F0cfBH~nUG"
"?gBy{E`?yC5C~O5^u>Sh<fQ?|BTgD-uuc|KLbsWT6rpt&SZpa&BPc3BbFRkzuT3RWnaob{FkqKkx)&DH;L><bI2{ncQ>>$3Ag0D+2)g=Xgk;_i5D@$b7ek1~"
"7p{JnI&;<7c@T=0-T^91<1%ZQY8Z}HL#uu60olNZ3J|Aq*T>T!_*tC~7R)f&P8eBW4>(6qz^VsIExXGrm~uR$1cfrHkI>~C+ru<aZjmtY3yA%CBw&6r_*=yE"
"`BVD+a!WkoUPcUi0bHmD{M0!w;7QTqb|NAqbJ$#A1-oY6OtHYm&7bn$9%in6m20?Q#4MS#t}jQK9B&zp=kOGTV<TuKo^6{OABiz_qK%gWFeMfd*uZ-aMl<tH"
"-xNTE(5?(S$%pzJKqB<by7(7<&)?;K*Bn#Nz$5zt0iY|@2y^IJzJ9^U?u9~lpg6!E*e3h~fIilVLD^u)C6$)uh80F1VO_Yd4$NtBynb|eCzdrV$s$CF*bbv0"
"#@5r@6uChrEd^-ei&$ezm`|*aV={XhYYk45k|%~>>D?e-^r=`Ptb2S0c85<R&TF-Y*Y$9NC=QGWZ_DvBkyZRe;{$kTApw;pDl$=TEU+Jh(fFR|X*^dL(*nZ="
"2EnDLFs!mnZcy#LwAGV+Jd}h@TLC+X$yBBUPUFcD`!pG+=?VDL?cv6>aKL2nHPA?HMq};02%)PFJQ89*79eL`Z``f$KbKrrVT~LMrR?~jPnzijF2z94Zt_Qk"
"3popZ@1t*PuW_}j;rJ2*WPCm3?Hjh!Nh`Py*N!2~sx>Tzb2A@1^R&<#4{{bhYO#<!F(PB}u(m5__yhR(X?yhxJ$FW~apJj>*r`_%Qa}j!BsiwVGgkT$(=k;E"
"G!Xy!rD5*%hExegO$^-m3V4SxGYN;qR|LscD{w;DSpAVMd_s{g&x73|Fv{9KE|}qR7MXdk-*Cd)@^MVt0LJEeu-_8j8L0(&FXk)yhx9DJ=0MWS&~3vWT-tKP"
"s^M*3sImI!T;&m@26zef2bJMKqJeXR#;*fQR1fQ|p7#!)=U!GaxEXbLbV;ql1cF^Itesm9GKWzRpL7C21dkOk55O1@fHI2QwzLi~rZlY1%idCV2=*fh>@dW1"
"-aH0)BV+@(5C%!#K(x`%zCN2{N@g5k>Tx`}PM+SCsjD&@f`RyfRX&~9CIgzn%1gf-HbIhyM7%kS9rF(@2T8c(Q@~m<$&WNX?@ce3h*(}DOvJb}dHd>`o)js<"
"ctZ?P!xj%v2zjx0;reont7UZdqQtv9YapO5nwPaLzHT@CV^N2h4P9t{2@U{z!bABAYD4Ov>*vF>WMC=p)nf_5uY7i>x-22aE=ogasR(B6xix_6_MjmD1MLNf"
"+C7LGzkHv|&P%(FxT`Zmb5)Z1(sF24&I>njm2ArOOYlxiG>1;prUnmK7Q8rMjd3xTyOFp-Yr%OQvgf-Or7xr~bwDApQ;6SGMZ$U!^9R)9Gq?zmE_<bs;W;wA"
"U<QiB7v|eTYOUDKvukEkM!aVkH|-(115yx9fLira;0ymU^sQXKv=vOQ$pE$6hPmKVnnp`ff=s51;gZlL-v=fUa;@WE=62t}M?gpL61<$QYJr;|yn5@yL%Zle"
"jV%EdhX^@^n-QjI@;Q8jE<}(;@(`C8Y7#f1NRmPF(6uPe;4kwEWD?5J!1s}{VdzTZjK-trty}d2)d&eo1`!p8fyG*)RrEQM!J#a4RTfE0=1WEVuK*5Lmo9HS"
"U%WNNYCIgSzv~B2jE1*u`=ZdQu%+cGx4*SJDyzYjDGR77V)Q8$ua+fKKY8;HjbG-mBn7ttog2zq$^_mMZPEDTo$G4#H_AcUzR-&=##1UIbCOt+Qs=4yeS;Ai"
"0I&;io0ruYYe9|x?B|<dvQY&>C*sdwd)bMd9?%M*1-?siAC~iEFiISKetImO;6aNTuIM|sLI?-s>hg{1wCDrzCp*40q2oe^DOr~5a?ie*p9wFxtKoy{8t=f!"
"T6)1802}3OW@EiSSZ7dvo?;HnMTwhcv$e5n7Z{1htpg~T48J9rQU@+T5IAUJ$c|a?)d2QQszJ@!esxWCz{sHg@C-Mw1c;B|{(MRPo_HeuL%i&sOB><G-BMhL"
"rJV;W=yN9C_KwEX4~SNiEwD(b1W;&oW^Riyn<cXfT@FuTq@jbR2z}xWqm{%YivnDdfrloS5ZRJS5A7CgEu;L4WFc;_C+rFYt;;zD2t26;0C5HN%Wbxp&0QB-"
"<*L(5Ydket%@Cg!ZAFTfJF%*ESK6tyHW`5W8xG!&r}=#jWCE$^0gnpM?|23n<nVm>6XW;!YuDRsgRetKYzP@zdKuOy3t&DcMTcONFlj;rCqlm!J4@gWIkO0~"
"Km7?|pe`ecKZ{X8tnt4ZbcEQ#GgCrE!e(j!u%_`>|JxISQduBzT@Y1B3LnU%5TDmhi3|M&F_EX%IcVI|rXr?M`$z4F_?M|Eh~W(JR=3F!5e(aB0RRFXMDC<b"
"%})>*6=E*A>npFhnxJfm2Ht9G97+FpP5N6Qv|ClGlE@E!h9~Nf;Pmt{Co^CLWosz6VRQw<o^%m>Vp(#%O3PGgkZfD$->h4Y3m`>^!>azYqg$96j3I=^8buvO"
"K$U7hRRjha6dfk-Yy!nV*vA07k?1jH0C5F^3_>enV}!x1;2jVOLLI{n4Jo6Ce=C9<Jdl!Ngdy1iFC*|$$_4=0Aa1_!@C&7*i{NV;%!}lLqCDoZy7Z>POw7<g"
"YT;G5xF+F<n<1P+a|oJuxOl9A*uYVjX>xqqp08X9oyTmAhl&WJRr#89i~)=(YlSf@)1Y5&m9vPV2ohk7R~>FmnK(R};s~uu9k+cqv<^dDQ5y;YJ0RMC(5X7?"
"VEp(jriYR|T{|!kXF*SXvFfUTIHaAb%>$f(cN0MYh!drrXb?^{=+ph7S^|Dx4ANfpxTtSa#G;$<Q3a7v*~%pbiKnJv2;dr(lx9Z`%xahrIJhQeqGXx8$8y(W"
"TudGO(q?050)zzRMQ~Hdl+FYs0R{su4(d-Mkg7>R;*Hhk#00Es><D%sDnugq^H%x{z7XVRA)m)X2mZ0_i)p**X2d07`QO`jr32}k@#*y6*qb?P<?J1bxm@<y"
"g}vw5@4x&QcYO!7C~B#3=HL1<v4<E3)d9dd3?{gXj7jxYh>Z?|I1@2e2{Q~3QW3J%rI|+^Zx;gw<ANe>N&pYE3ESq=jQLOcRY<bsbRnJa8!_~nJaexDa7ecG"
"&fjG3Q}<33w7Y}&jVZ;;PYpKqE<L)~o3aJ8EdwNYb{$fR4WId)jY5S}n4@^bMgR}+kKr?i-ObzEz2g9@HFn}5QB5(;);cWOMVVOT2Wf?G`vJ?cK^APqTK+A?"
"p<F?m4HxeC{@-nw-I>462R@H^mh!ADvn)$(cdg12*4p*Uj2G{Ob7YOt9}m8l@obO_cSkj_1O0^oy?5gs_Lj?ktWV`<#Kr8BH;H(UxRm<yY{DPn7j+7En_GB-"
"g!wn|CizYs3T<!WM+8_G|GUr56jvVGWgVa>+_L~$@<R`{VsYdMk4Z!x2$X@IFZx`F8<Q?sU+i-kCg6k0aT`Gqc}twOO}&(BpgbiwPTRyG-UP(7Ar+-+E<`|x"
"e<JSUcO8gocpy(>cn5SJW%LO!5NhHQl>vCoph#j7b~H$sad#pIXw<lLnV=LNLJUoh)EH>q@=ag?>=F~x)irSkrr5q5s11{8Vu1j6EvrGb>K3Gm(huZ`g<<fE"
"Bs>(kAn<KV-3P6aF-<BCcdIk(e@n%S%r^WOJP{gP9+3D*oqYg)d3EjB4c}DjI$6q3?C2JxSE!)_@DdaP58WJsMyTA3!+o|3)>47-G#>gt#oAI@pa4Kx)<Yq#"
"gudXWgt7zl2K=od!a+Br`SG14ywHb=&xo;vCnbfoa8`C!Z}SYCmHAQhs%J(@g7w&Y{nKGq4Ar#zcv$w!;`8c*m{=})eo@oKxBk$MMEMQ6w5Hr6f&F6Ro-N&b"
"Eys@n&o3fP!jtfSk>*yU7p_16gkMwQliniml3a>Q*^=rhZq5%~tmKk`M{#fr!OH+|4dB&~LCHpUZR634J<KLAT3K)U)vMp(YF0f;S~Ktt-mklt)WrG@g>9In"
"=t3xj$^;(Xwz_<D>}_idm3=#1K7NYV`sBK5y!_K@#J=PseyZZ5dWL_fP6Gio7lF}9_w>MeGsF^D6?Fqt5BwoapuS!sVG#t<lqWKT(1T(q2|X$B!E7uhXF%wD"
"Y#8K7dR1ZBXTc_w8X!_Z+%5W{8`CrZlXhU{%vP4_4Rr;JQ3y5QFR*i4=ba5D_7LJNg3~$Ig`}VuYUv9GBjN@r4@yT-=>^7)fVPKdjQG=KxQc||=ssx^LpF<e"
"1ucg8Q>Lh$1aJ*^qYoN9tfdqM`4J_z$OQvFKDO{rSZ?$U9&ia^Um+)W;0V-@7;|v@RHH88)z9G9MinU){F|g)tyL&u!nS27@Ov5=1td}#epj9ZiCK`}_4kG6"
"LR%dY&xy~u7a+-UJ_p0;vwqR=S2`E($-A&TlE;>!|JS$9O7Zk0lZ>(<hEf2EFJa0;?2xPJfT<4yN`2cvxkDYU3nIUu_6DFNw8??leATpF%WkXZ-2LuH&7Sr$"
"%Dc@ax(y~QQ=|^(qR&(|u9Fm0gCndOcUn@ebx5-#7-QNf67F#DUm6S5-Y_hC8!U+`aF#SKz$^pla{~jdgUJLi!salA5mI;!Q3_ODxA8{EY_n!6cb}Lteu7B$"
"@Z(>+>ex4~YK9BA)JrL(hqtQOVHY_E4L33o0ru}Sp!$u?qknt~@eZ0OjfjCa!O)WtserluXi90-ByoA<*z4l$pvOjn4kw;$P^V>2Z(`v#ZSn;I$~zeXJ*Ip3"
")MK~q4H?KzM6d_?2QOrC$*~04!6YCMVHfnBpCPUJ?aS{DRg`H-X!bQclVZzEBVw{6tT3{liW|)G(L2rXu$;q(3B(Oq5><eS4{EYlsBr^m98j*!Aq2#siY%3g"
"KCm=JO0>Te;2l(val~(H7TX|qfsqs?SvCytlGF9Sl&-y}TS5}qbvv-D%sLP{=a<!b*2^sWcX2Pi&%GG$vQLFG?sDKch!=lI_8~croPYC!z2%nYP0@2)sv^WN"
"edS{@jyK{E^AFa+C2%=b4r=M~?;YLfaxiGE*n2s}e-14Q<#f01bdL+iM|Q?L4sYNK;2%1w%e>`&7x2F){waUdzTZ6KXM>yXnH75m7?#2<H~-MWI-BqCXt`04"
"NQghLj<=5E^lA>!=a?c(Nm=_43|qF@z;1A?6z<)U2+lF?7f##KFHjVXQBa!MU*U@g)3D}}ICY;xqDc7rFqAdUS-mGJ5r(mp$KO3=<wl=%6|+Ln`iE|N`^+ZZ"
"-R%OWfbXEIfgON(%1_Tv_q#$aTF6Wm;efzk1S#T<&vdW)B7#sVv7P+8`cras+HM=h+u}{}4@^UU+O5H1xcrm~tRpw=JrG*Cav!}*#d9;5)F|ON9O2KiZ@lfo"
"q_t(pK&d1JLhQqzL!O5FuiPjmpd3R)h5LEH<HO#72mAqXPZbqI?BFI#&Yu@&C129#&5k<a{%5(Tzo&jy+!RKLRRoB2I|1-r*`4D>x7t}m2&Ez$mHe$*c|i+t"
"YAayQAg#Rvi&6eIDte3TEYaRTvgQyWqpY*f!a0$RFn_WF9#f}7l3hW{0SpKeUjwuwEZ<KfL6N)%m+%pYQ#P6hgDr@s*m^{?KrQYlst{SC^d{~^>0W2n{S25N"
"F%qZS_$46`{lP>Q=U)&0e-0nxJ!`Hz$~yuK0XKF9N_Z2fI|iUJu=r%l=-tE)0G=Cjy^U^V3hwJnIkjS77P@6#fJs2w)aHUHM*e3sKNjgJ3TRLTnMFl)az3pJ"
"b)KDdxm$3BD+_71ZE|p6?Z2YlEB-%ymj6HMpGu~V@;&}(m}rk5k{2<X8N2y|3uiVq-w{dO!_7c*BtP&wn_Tz*0uUMq`3$#;yK7b;yj4|<4sM4?In61ffZ!Fm"
"gzhiI3eaW~N-V2W#_*9IJf;vy1jt;*N?sHND@6&}+pJC(2Xrb7f3*G2<8S2svIl;QiQx(ah08nSULKB>JSBsVazIPCH-Q}=<zFoj5~eK>_faNe5sCz(E7*tA"
">7DuT&)Gxb@6_<KWBqFRw~WHNH<)_|v(7*-sZtyq*wV;^M^x8UZczI4Wbd*<*u;ZViky;3ycfG~3HhKiHv2Y%u%sFeVeN?@%k4nOVG=TY-qZUby5JXx;Jvj="
"VG4H0yqMFm00@i<C^ra2GYC|&BA$k&48UPT@$cGsZG9P6djHezCX+HBLLr0cC=YQfc5r1ess$lpI@N^SQ7ZR6+rHy6h$UjY)TAdRJQSk~Va6Ae2I9nBF!(Ki"
"?=b*-)J4m6B&Izf49c=Bb(t{#@v7W*53!=?@r-xU1BV*j!Hqb>O3I9|;L4ridVkyW{kdJfk{8J#5GC^x6jho1`OdfJin3c^=<8*HOCK+bz{>LG^we!#e!ba)"
"+`I7e12$``VjhGg4oVkuEIgzhXe+^Wt_!&74}%kw!)SM^|Lf(2Z0CnRyzSN_lyixgxmzyT3#r)Mh6*JS1X>vRw=sVA=4TtPUnt{8uBVdiuo8Z|U3gor#N%#d"
"?X2$mvu-XS5X9MXIi8bnkBDWHR0#oxqVJSs?7_3onkJNfh*kH*)C1bJhr|~t1K_BxvponPT%I^loq+aMG$@5L&JJvBTU!l@xz?`VgOft_M`@#QQBOc&Vh5oZ"
"^bFcmxk3&_2k+`Y_6!~PnGSjzwB%{^0c~Rl59@z;Y_}!KG@)EIupIwC+B-756qp3VA_`1J>mV8;QFiy6fVQxj06C8<N|t4miwx!|b$qZOJ8o>-M$D@?t)|Uc"
"v!8vUCNO#k(rI<RN~Ovt+oszC$QnbczUKSdkhCHmRBI}(P<I%`__`){owC?>SH}}4SOuEq^yWGjM0Hrr1_!-`r1F$&(wlZ*Tq2HWqe?u;i*{n>br89C0peB9"
"m<NS4Os;hH7GCUcpDPD=N>X%&^wdzF>R?f*B8eBszvLa%S9OMoYfZc~0G&Ips1P|{XBG7|F&jD_5s9+wLCbkD?L<e9F?=B(_gCS*L-RVsj8po&l(5H^T%r~t"
"Ca*N*sdJ}`oMsvv3EpHLdTHe4TrTS34`xDu+<8zGsvUBbQA-vc85$7qBKNCog<}L1xQL*7h#liC{T^g-it}!b9}`FPhJKdcEZ*wB9G1WV?@-6Us<r&q>~?Ie"
"&ovW=DCg8tX5IWV_r=s&>qE5qXt)Q@(SA>G%ReB0-gm)mN!;&He1u1Vj&OMroTsN}HAZQ-@yjLkFpHK-=bGtN&D{XRFY}m3s%7;!-;jvKoEIxIdz~W@D?uci"
"s`UssGH+&>ha4ZK(yXovU}GYOkDvSM(tvu>5cj~@5z2gB-0^ERvuh;R)PPn|E&`?HAm=c^X1<Q-z8X*QjQPB0c=Q`~Sg4=3eq9fHYvaFo+j!<&x&Q*618smA"
"&3sq?WHo43eO2aJ2A2h*?d6E?@6GRcELAdFVntr%gEHk6$bN2%e<9!e<eWR{-Y33UT*+N|)9{lO<>f+zIzebtf(Hui;0j!?u9uq)a0obE<iodoewDALDh5OC"
"G*psye#)JXTvs`s6jlJa5^Wn0;_u6^ekyU_!WVxR>;cpa>w~B<W44;FbD2Br9&l>|2Fd${$Qk826eE4&ayXG76Vbu3v$roUX`w)Fi`>Nd!m3e+GUf0pK`n@q"
"fz|6G=2i>Rm-N~<$w42j_QrLt8PC!sZs0Ni4TRjHnokuU7nGJJ6f1#x!`#6&xDr(u>tqMy9^f4zRB{94<U;@#W%AzMXFgzH+daWepnMWYOQ*Lb+a?RH&aey0"
"eBkL3=H>WIYg`vSoHz}3CLmyO&7kJZ?gQZIMO&0&w`ENmcn_9us6;18m{V;J9M4b@dJK;9D*pnI5L&0)`%ExwADuPj1e6SM6P(r&_mdbys5?5pu7kI~BA#0k"
"6w>i{l(P548-1W8=*3&s-A<VE(PaSV4#a8;pau_MoFGh7{{hKs{Hw2WV2!2XE0*@!vNt*d+=1jET=TjCk=q*hsP=^0P|AI?kb*T5rIPEaoKJOcB6Td*s=X+|"
"k2nIdz;9ijI(s^{WFilykV}?RJ`^@vm@3-k*}4DKy;1dT5qCu6_kLld%kcvVVbbL$&&$pHe^2kp2a9<L83z6>a#0SBS8&HyjX(aiUy$MM0?2hZFI;gBP;T=)"
"KOx3<_r$HD33m0|9lE%)_-nSU2r}#8)fos12b9aRp#<;5j|f#E4PRTAK`Eu6UENs$(C^b62)~~#R3y~v{JRn3iV7iAt5Bt)<VsxxptOniKj?{%l7!UMR15|h"
";A^@S+HS!QTu--zfhK5SdBfnB<tml$M~NT0{d(E9&2!FBAseR1&z)OrW6>;xa|H8)TY8=vigWilb#4gz8_KBo*CieyMKVC{UKV_FY?_|O`$FrGmG?xjGphcy"
"btMOi`6ahT#y|*9-E?-OcM!OZQWyWzmeToXOeBlyIEX-QRP?>edv*V>r8Z!1-JYZZ`4dB2)DNjlem?;PWhNYTAKTrNJEo@*0@Nbjn5%oGUvVSIy3#SK&!y<8"
"h{Tu?{X(?vlXIq<wTq3aarjTgU`pJU5ROygu(jpoPxY30(!)%Glf#lt;_MfxLAk$E-anPn>N4i`U<BO8!YA;K@Z0L=F8`Fc$RE*}f9%oBT(rknhgd3tA!N0P"
"0kWSQ3SurWEqu$}4rud;AC375Pm3VOROdH{*CMX<`jjTX#0woI2`<jR3TbD5B@SmH2_Od%XFy6+th*24hun>Sz;N@sKn*~S{2QK!Jis6zd8p<qU7*!~zf&D^"
"tOkcEi&K_^G2d-X>%jm76~YPu7GJLY$jS}OC7@1V`c(iXxGCb_jZ11W#&Q$@fzL>41eJo4yYi5@@v~cNPnKti7>});a))sJ=<GQ>#1!86>uxOPJXc3VU4;^l"
"gXU17to^DlKeqQkCCi)=gb+3mF@;v4R7FyP?nymRo{O(w-0GIFcy>jIRqLsC6h%N}#eiXrwqI&J2y0N3y2sr6{3wJ?ISml$yFL3M_e*9~-R;i%TP!XM!)@wj"
"og4p!Gc25*IYVtE)U^b>QSPaSJ6@lUIdyJR9uH<HWe04}w|S=`d!rCsd5o=ApFn(R=99ua2pDDw4qYfjGejm|VAeZk)vpp*Q?ZOzJzFjQ(3Y57&L^k#s>9k("
">^89-1PnaScyVgAk2{S<xExq1pjd=2W8wTan{PflF@nMMp(X*+<B6>E4`cD@2Vdj@aeBvqhq)u-w1E2(Sa?e7XhZ@_je^u+6qv0Wl}bpup>9OxO$E^?ZOvCH"
"d;8W)cwfwka#B(l)YyFlp6D)gJL?FBW1tl@MBOq(oLVVhev|4BxnsT$UG$PZ4tb~Ac-~sLL%u%jA9lOG?=sT|AKi0{UIS6g!tzpc!L`6mJk2z`z^(U}w`Bqw"
"FY06kWkrqLp|!bZ^9{1=<)9D7+yo5_`|4lH0qFFW>H2|Zu9q448GBqqDI);ZRd5S0h9ePIN1)rDjMJ3<mWdEB<<2K>2ds)KuCB&P>Uhxk6J1#O*0QLJg7hgb"
"U(F~u{=}yckX46LuOkFh;c@^Z__5TD7p{jv^(gcuoWl>rMpz-DuBfSn$~rZ_uT$9uyae-Jb?_QE_%rY&TarMkXWzLxSLjsTexm9WIBADXiXM$#$;oW%#uu7a"
"XZNp+v9qX$RBq`||NWvr*&)AC6U~Ij@G3*!pv4i`FYZ3^SOr%HMvq!cf3O=uQ^|q0)K5W;pP&=-S=eQ!Lu6V~A<Nl4m}_|R<X6-X{+EQXHz2yrVB!a%8kN)H"
"`5)dG%nVdm3e6oI=EW&FQ(9^v0jyFRAcb)Q@B=}Ii@ODvs5y5edV6E=nN7+wX<ti3pu-r8H2}Gw9$HD=#?E~gF+ldj=AaIG0JB$X5JDa(Ep9-mdK(;fnp4Yq"
"1k^r@*TxpB?TzI_tXVYs>(6yz%k{}tgZNj$MaCZg!aczyV50)9ae~?zf*DP;=*p-W*+T~26kacx5J%G7gewyKhVm+OMc4>t>}mbWPal{4bc<QjmnTM@GpDis"
"SUx}#l_;>5jOrDR+M;5w%TB3n%^=IQx((0O2m7qysIG%J^<t0tF@(ROf;pw7fNiwW@sYiOs=FzaX=zti*XXipVqbgpC|~F2b*1`I>vJwCJe>e}fjeW804_e5"
"N>cf12?F?Dnacs#cE9_}&%3TaG$^FyL=bVs$R|#8LvJEN1RI3Ng4sR@edrH#?hArCYp7EYRvIQHTm&<5TpoO;Av&tW0$<8X{Y2PRl>fF=x~%P&vUM~8)b&6d"
"*G@nvogdjo>0YsBVu`A5*1EmIdw^y%8dAlWo8KcACBqZM#~J153aaN&`zgHzDm8nz$yCD4aDBIK81*X%)m1^Aw|!_|@J*HLObu;;F!6$#RUB8Gy#NiCOjpYN"
"y)QI|VsL(xyen1R4g!@y_owY!=AvvcyqM~}G`cQMse1oa-whT)ltj~YT!K7EkR4P;g8MLB8m3JyO_d`&?2wltlf=+jp}MKJ$@6nSKSOMX+SjC5Bi>9Kt2lK|"
";A6_~c?2E%s}~`k74=$pMs!a>Pz=nvZ`<7af?-)G8dJg8tC_l3L)(>V+FiHt?xRq(nMS&cU(+V9N5C_w!*D;5SvTA$gbsu<TUQq9<~^&QqmK^HHK?>zx8#WW"
"g2L9jp{(7w-AM!yT|zQx7<&X0@+m@UgL*KCWDQ^NBo|!jXZoam*t%*E+N?ZtRd7~!alr|J=uW6xJ_OO$8){FDuGD4Y^?)Mu$=*fyyYU$S;cL`OPw5&;1K#7m"
");I{$J}-LW%!s<xT$s!QhA;RN=9}>gVNV^mUqw)Qoqd{!L^u^eaPaXLU-iiF-cNM$uB5^bq&}?sjW0DLKYmeg+ACu4nuyejqNKQ=l^|b$uNcHd>ST?eXn+>p"
"!Bh^Yd&_VIQeU)uOPVfVFF-s9DpOE*KIO`(W}L!kNq$JEo<ay5w)bqCUF^9PMc88??Zd+QsxS-dZPi7D5D88wo(254c=Fv>mc6ZtK;7)5E>GKxcmRNXc1kar"
"E|x<)D#A{&STM?GvQPz38rkw&j#^d--NXuKx2j#;%?cSdFLhVLD2%E=n|a^?A@o@6d`#zrZnSkR)MbDM7^<IsWQ6F2F~P(hrFJpWrX)=Mi+t>;G~;4SJ%(d$"
"5KTm#>!+@ri5t}QE{Y~p|1v$vCm<1IrgG4D9CKIn7s$*U@h@3~vu!1Y)(q+W)ZQwEnSk@U-^#AHdrpczX(2=LLf1{jwv>?{k|Q^ROE4MPngY;ui@fNvUIA5Z"
"+Acu|s$y3b)q}_GdFWpkf6sf}cDFCSeYu}nqi0s35PO&qR#m<HkFuHh?q*ugJN+Whi@dD*lB&CVkA1J(sq$rTqtS1StK65V<T>l5pR9iHzpk%*w)=^q%yYrJ"
"zhKI^i%3D-MOc``{+>(Fc|1ibtBnRA(@JKC;=NyJxBWuJF;TPw32)=02PO#Bvu++Ar}h&te=!}C@*if8F8~%Fg2?q?(+;*rwneGC4_^Bx{%+fJtMRE-qdpC6"
"XxX~&f4}wC8cu%`$3PIWe!DB`4S&Yv88rf6r7(4*(BS~?T27l#_uF6Vy0|itXpC!62A7u5QWKn`Mzs3x(l)E;*F7-RfbyCX%gz;+WfojjP4TA2@Vym@KN6Fr"
"4Ktq4^9sart9)~}p-08&3}g(<lR*Sj+2S(u$&}EOiUgo@Q9M9Z(_j6-V2cWJOeQvBxMYk;gE8PP{W0}wJ_(+_@>o#s5HIo}{LNOb3-q^$`$B`YfV?}=>)B34"
"NwuP%m13{lmaH=jM4l1Rj0ikEPqoAmt2gcf`SHK}q(UTFKl(F>lL#sRwJkzgkzjp7h0g>zbn=2KA;h-3+P_~whyV)QoZ7IbbcKogocjd$|D35D?Nc?XAYQF5"
"f20uS)vC&A;%U2&CdENgd7f*z-^fjg4%wmLsQ7XqD57AoOs#PcrH1;zxeIw+sL+%_gmfe^+0rAkuKTJ1ruS#yg}*7+<;!+Rci}BW!y;GeujWt7!wBYitxas*"
"O#U!4kiA0&zcZA>q%Z>Fj(OmsKcM_8y+*^J2;*t*k_S_Q4ifb*FoY!g5@Ar&##2)_g4A7n&Qh+z#86hmjj4@;y|JVLC$9Eu-?dc@bWcN175_*VfD{et!e~tv"
"6AZYZ_`gNW*wuSfl{@bS>4+;J2S+w46V@>gv~5%rJmY;k358vcv=_@p?VcZ>clYp;{J*#C^BYastiYWp&ubV~p~oiEt-zKhxB^^^uBYQWL-g@a<DR}`^*Vqa"
"^p$%SMmoi~8T%nF5BW8|4gfzBeK_WybhiN9RhN(Sp8U7s7ww<K8~zT_a=S6icY0sbU|%2{;?CR8>Y)T0+UP)pG3nwR>5d{>q>V*&^;=W=Tm2~sx0kP#V7Z*K"
"{PAZZ>Bks7)#2~Zwj)i1xiS|^kI9*(c27qgr?BcAewmxPQ&Q8ODw~}*W6HPVrh6O7=hc{cTui-R*m?vofNLb}QrjaDlO|=Zo`|QYjaGW9e-gbTd!Ys(W2^pU"
"%>D}a2UZdcZD`Z9oaocylKd%gNj1!td!IZhe=YQs;KURHEYPxR|Ip>{3fHZuwR0Wm)qO`mUFYvZycBi%712lfgLUZ2rV}VCRrsm-Mc3H}=y~w}ZY~(F6B0g$"
"cGLyw#wzsOn#&Zzfg92eIAU!MgvU2@+rkfJIk}NbKjh>yurt~@Zu0JPQs%kOIduSAPxfoB5j*NGBhb*MbAZ4Kk-veY_B?4fY%6T|ow$XkmxWZba@%IJ@*KIN"
"Y51%1Epn_sVmkIV*K|r`T<SzwKl|6qi|+C3b2=lg=t=T9x0n{4Yr?n5&lNSndzBUf19q)+2Xz}=y*Y-)R{v3cNIkK}zp^8q9)(Hbt$;62+sddFigF3I?llC~"
"l++Im+Q!pn**1X+nUvJVTPP_r87aeTa{If->Jl97KT&=bxJ9&+ny&Zzx9j5#;JDP}M+XJ;;QNr0#uJ?-uV(VN{&oL?zu!*T9r>)9GMr^%VCG;3Qcn$=O014;"
"4s-!K4p>6JGJibJ+ByeSLd+n`z!3I=mc`<>jC}wO(bE}0^&gN;4@t5pr@}fEnuBZ%W97T*Is&IUlTEh-+s+3fi-S-Ufzj^6Q#UXCLL)zxFJ)$N<%$1z_}$^c"
"moB|8%Y2rZ=`@hj4}Iq4D{SF<d7)2@4KY(?=|2#w!QekFo%^Goj0)mCw~tpkWM}_qFvw=k4vw0e)zy|QK3Pme%6Q)H9L$DmYFVCETlzV`XTuKU8?8`_YDywn"
"X^v@vvKH_A^+MfuvI0iYX$zjnlp6MWV52$h38b97r#>xtQqYcBwlt(&8VCYm2u&x*HiA@$=vESiAOI$Or_SX9uAy@~qJ}KTp_cdCQ3G5+fR*;@E+W18%~n68"
"_Ey)z*aBspRmk(<l3>sXFu{PZf=Sj5YMh&$S)Rr!#jd3I2M2T&M(TzBsyQ%$5Vt*_*(rR_VcxXSgt(e`Lf@$l>WSP4GcmgJkjoqLt+3%Vuzq8?u`*kB=L!%M"
";4^hPTJ@+|>1594oU|3^B*-P?jtajv)0AzvJ{|P%Ab78`JxT+bc*wLj?+I%@_XBb+w;YJT0s$B-bYN-MYt*XNLlk0UF_8~Y`-tJ9!)K(aI&~U}j&_hPTfQ{N"
"sr7SUWPJz)$+iZjM?CKaB9|r`z{x&N&gT-z66sEW>QTglcJ*1ynOv^OMA|FqKE+RyOzr`RwzSmDs(hv&l&xO3>H9aQzRU(ha>*q<Rm(AuzE_2<H@``W{?`Ah"
";nI`^R$Y25w38pNTW@`x(sO_A8&6<RqE8!sdcktND`yj|B++jPSu~_PrjYg;6jc~}5d5*)tj4PO?B>w=W+^H6LIwzZnzv64hhFq$h;%N7C!}9TM3C>bN9i;R"
"H-KU68X5roh2b7HPd7&C!O>a&lsn2Tp8k^EO|gTJKwIlduoK7gA{NO3Ru)bzA9YNL9+Rf{-saTKBkjoxsz`-Tr3JYwZ@v7TUi19+Jw+5Ol!BGMyjYfjZ5{j5"
"h0p8Iep^iCm6GvPt~zRN$nSBDk1h5>Ye7uJ;kPJAu~HNpVwuW#de7hHJ`TJ(Z>)L`U`5c5WfJZXYsYVExf$l<eqUpK0Eu>8gre14Q7)OpBGECmK*jMSa!OMW"
"$f_oVWl9m#r9AP<db2cTDU|K2Ovx=Zr<eQ%w`X3hSJj%lAuIWmKS?IVm+O5CoIQ9W7v<8^WOzi)iA~$b-kuqG@i?74LR}EFWmm}#hojkE%TvvpI&#~%CDw+>"
"JALy-;b;$xCkLnFZj@XIRHq031Ka!o-SzkBApWbpjG$@yLz79+rQ(Ut-evAsJYC>z#eS#_`suGw(dVKQb-9{$uh1qeDkd^>JkSnmL|m#B!0_wsSMWjE9(EzQ"
"7&Sx-O|n<@2Qdp<ayjgVE*;3i9qq^@&CYk+7v*u#=Zr5*{r0b1wF_smjRHgmz7LL>m7h)`R;i@3bp#LrvO<5!fDP=u{(mTT9gBr7#H;bZMG2#UQ5O%$H-7b+"
"xM+R{FnbY{{ifoZG7fYGj^XT|xRKw`r9FXA{&8K=Bl$6*B9^tWQ;eV18={DG{EMy|l-qzUupd>&UqgR6>hQuuELX9D%~Rf$+LLJ`48_)ZgRkrD2WASZv}s(+"
"o5jaHMWI{6x6}Y!w88%k&wW#N5!`0P1>0^dtb9>^@R<FbNz-veaM&Qn|F7#@koM<hO4_q(!1fdme8X=x#fo2ifvftYh(KpqK-2Pi|CjiC`|sDA8Ozr_r>G%}"
"?DAH}th!EGSA{Q0oRvi>OIGGZ8GW_re>}u8T6z5ojSuXG`<svR5`4$xnwg^yUbz1|3-i_Ragc3M+M-~FZwghgh4jqIYEf)o<wBe;EY<MggC(nexxDMyvAJGd"
"5s#a*k88hBT=OO3Osyws*fI62?6-8J?ZE{#tsswp#lLst5}vsjP%kU-9a#OO3G0+B)?$Kc3<zXr5Ci3>x{VA>X%qnAk5K#Z{swF7r_|LqJ?jtYiO<|B!rV!D"
"4x)$yOJL*^o9SL$wJbv2VYLQv6kaPCw<^JYzcG4ymThKthfHf87*u7?#U99gdow16p&10%6!sv(-9xJ%EpGERZG@nYBci160JIKY>m4#;G{zMdSch+b;jP%h"
"H)$Q<C%_tnMjw1lWgHdnq94Wcv^y>4{KdNx?RlemJ0%+_*`XOL3!&C&ZymnCYK`0lwD^x6xFSFin{=U*l=Ml|Gw6eQ!Jmp_aP9_F$|Cij@h4QDs4HbH>C^|R"
"7)xL8-SWeTFs8XLsdXsqy$@F~6-cYEGq&Km`U#tPQKv*H{Xz^<4{7)8>*FpK#`eH}<%elq>h10Dmcp!DxP({F>j_a)gxe=*?>Wz#h0CjIsM~C0&*gLFtSKz("
">E?UnA+?z0+3<4*i|?#n)gSt@ka<~ErI+GB``fy4`2B0uJ0RPNJa3xZj_(V3!QS%G*ZlO?qj}q_jmt0Ox~xPVV+2<}Ts=5k7R`L5N`B!=72C4P#hF}-adCv-"
"%eX%kZq=V)T~<In3|1uD;>$4W9(!>Ruk7u#(lJ(mkA*BNSZt4uiq+GT#r`=KB5=<zUatRsDw6uj<zi`z6409QdwELIu9c-MSa~{a&t~EXTgJ;*ldB%)j}~3p"
"^FvJ8)%y<jHyG5LfL!ScI>w65!x0Tr;ez%4gSz{RHYo^B#Jb*EX!i5AmJ$S(4W{xyuh*H}y8c7ybxMPSVc7ZvCqw5VY}E%-DMmf3l<}rbk<^J(_Y2VZA-7Va"
"5pEY%gETfH`A^-p)PZ1C&(onGbuofu4-IUYet7cSuw>xGBn<lX{t=3Ov}Ig%(kg6c1@UVPzijWf{8D{3*n0m0oohhWgyB(3q_>-zwigt|u}#>^5O_-?B$SXo"
"Yq%~1(#*a{k(7UOU!rH-I(dD)f<UbRD%%vCC_qwH@sj%KEBE-WJ4yX-;4r&hsCxIO-`_&2%`3@l`oTfTpR?3M2;;Ghir3{fyT6~iJ_Tc&!Rbm<@4c^gZW;S9"
"WnoUo_Wo}8x-?fGeO0f48W=lTaV2eaan!`r1<eb67-J_)d})TFrzdJ4x+<!Prz2(>wyo*FvXyi1x%rM@mTPDTv@gu)({eIu_4=3t4kAKL>4>MdQ7C#Yp9f;u"
"NoGP=r~X%&Xak6s*PL5DqjL3(%iU&2nP(u6phE@EBv+sZ2)fDC4U>plMaH4M`ZC8lL^-81ZjWqD90^cSb6tI4(2?duBz&z8VE{I)dE!QUGCc#nQ__huun{*t"
"A~U}hCj)mzb;LrCsnSv!J;+&5u;kKOWMF!UMLMiZM=nzeTT3fYM)%v0ROtIX@j_J`O}Er1V@NEK*XdOAWIv5!nHZK6Zst>36@Zy9xBs&9OI~edbhJu@7qXNR"
"oFzGS<l*y^%?O$oW*?UEHEi4Ub2*Ur^B2?v6hiaQtAqYQX!;YPkJvs|X98;}dH8SdGWV{1cYBu8dm<eV<=)b`u0MV14JY6Nfm-h+bzl9Dm@@7kEaHlfC&!UQ"
"NR*LsOc7!FV3d$_9T+57$QPJ7g{aJ|<SBy^XC`N!7i>&Nt-u3VUd{iAEEJ3Vv{d}esTcU->cLJVo^2^f>IcuW7nfDFwVVo2x5y$(@&Lk){rmpL7L#llw4PmI"
"l`2_Y7|bYZ#L&8yjod6>RLj?WIwdUMaCJ796s4-olsfP!lw#1pghdz?tV<8?hl$i%ohdg!CCFr#7*(6=Y$Ubi6GAkI=$xO>5nHAp7@3go>Dmpfxnx$w20(4w"
"1k>#qGtdPl-Szddoaik?LFQ<?z7He33X*8{%z(a&6QBN@{N*c^a12q5D(M)C7)|P<r;MmOD5?CeXeSgE;BKD=H1>p{Du=C4T@az(UFgC&8RViG$XJ4j>y8Xm"
"F|1QsmiEr<SL-MX+2KlV*m*5#fZIL^fG&yTnf=}={2eNi)b5qz?`T@g9_=<;T769&Q3*V|)UQnbPAH>X)ij{T99LnO#1R;D+SYB+j-~&Mc`0?wRedZz=6)vJ"
"8VfG<>wHUf8P6MQ+wtA7ubkEG{H_9eZ7!{5i`QCh5JJ!eu96S2=r9-KYaU&wOC8kqIF`{<A0+o6N}uA>7mQ!SB^*C<fQjfc--eoZ^&dI}(6XAV?w=XyTpKZ{"
"91H+KjUwWS`dA>U>Upw)M1&y?B$aI`N>Re+37VMqjLue6)1@OYj_CcZ-}D2ZxGZ^036Z2eWMs;mw-ZcH_=tJ0JRrFEzI?5FzdG;UEq+yaE^TH`krofjjWpbz"
"zln+*lG=<V7XFKWcX$$Z|3fcQp~6%CL+Eg`r8@upIc+J=sqyh5*@XzSS|PL@mzkLJJtf?FfF}w{S^^&t?n42a!W08ZX(zAsoif?~*UlI^b7;NuqtjPQcT&)1"
"Zb3(y2^-~(D~V$}3UzfsRq6&z-4zFaerljaC>33c(r6P=_W(rE+<u2!FTd#U>uAM`VxmVG2w)3~^t1MeI|na)iLbK9?WS9cak3YBYA_y-|NObZ6OF^q_+`vF"
"$Y)3*sZIGsv%HOUZ6_wEeO4wNe+FwJ{@G)GvT9?BV5TMTB{3jhVjN$K7w+_%;dJzNIpND$;&&_1?A%u|g&rbB!Yl)goi6Eswu3+<mgif4a`<TKR4Y9R<JB-i"
"Z^L+bWyP*=)}#di98)2y;SdhKT|7S7NO7G?E#LRRzYebtc@w|u+2I#sVa_C;ZuyvJr^85RnM!7wt%pPLKFsGg(tnO`4IcygFve8Z-e+jH{jWW>cj-*|;<oAF"
"SwpfED|)@AO4D};YHtJdhh!q-w{d3lO=2@d<r$)0nnXeB3V=;Pz}<DZ?^fg@+g7Y^coDSEX5@KN53-?Olpi+-{Vqt%x4?oltfWt;U=^Bu>!V$B?*=S|y5kU4"
"VbO9HbXLv>{*MXPdNXGMWD>aq=?1f*!evj8{Yz?*2Mq=#tOOmF2q{P%6x1?-P^dblGM{%R{6BF6^>H;+pCmltQVdn0>1@ktFZ0jX9;L&npNuHDD>n<O0%Po^"
"Zau*L>f^ec?U-go$Kp_mm#n(Zy-<D@+5nQMPH)!-=fL(2<&(@l>9PAo`}%+lW8!3I5=>v$$6ZlluBKfw|Gn04byn^lm$P3ytVz>0H5!e4L)K@@;kClNa{30f"
"jRn1RD`F*0B2iNZf@Y^H(JXV?nNYTJBKA#Pn5ZhYY%1}(;+V?%yxp69i~e3Oe(#ZA&r8sJl_ytpR?7ZS?|$*f$G{*88;dgV@pDzDSUPLZ{Lvpmv?o&*b44Yp"
">|B+ntf=zh=8v9PxkrulR{oV-BPMr?Iodc$L4ufVW@63o=wJ~_XVBBc(nVLED>4n|qfSFpAJ(B=)=rT=Q9l|rWnucL@xi&7DWba+N%SMsuLEnW5ACD9nq$|)"
"N^m!>H*$g)<bYuzR>X-g312k-N^fAoKSCRc7+04?f}jSIz44jdkt+s)<gMA~UpMs0&Ms|6rxO4TB~@Sodxnn7`FNxbi3zk>I0cHU>nsKW*^-?cSwNWWAT%NV"
"swJJn)oE`MR5zEojC5%4z5uktKAnJDQiMZ{{EZU!H43zKTh$8PO!N{Q%+Yy)RDZ-ZU`VT!XI$HRmhy>|@<)Jy%p((2v;)eFBpoESMJKs{$F$<IFQ_@2T2Dw="
"P-6<QWwI$~RPD|(+7IJ(98>>DtT)F{vo!6~=9l3_wu-Op$fo%;i}V}cD81hAsMCGgJWCymjs&XBz`h60=Jlpnv$W`4!N9L?5}$R0&=og@@B6VDsGqQJk|NLg"
"56G*s-^;q@w+>AC@Mm<w5N(yO(^uVpJ;CHRXh%66{vl{b7<dJhdq-a)x5p9)9~|u|g*M0YiqFZTqqfM!7z<8k&nT0LzPBxV)?9Q4<c@Cm-}ZOQN2oo*GHLa^"
"rIVnn%wFuutbd6~+V-=5Qk;9~{wWCZ)2iNhMqNLooffHmIrezHYlV&#&R*f*MDI;zeOJni&`VQ=tnNt_2HFcKy5QEuIM9~xu3WWSbZT38$W9rP=XSeydWj9A"
"%>pcil6rLB<H~v3)2Zm39IPO<pDIt4e{=ukE}tmT9b>T1(4klRDMKy)%vGmb5R<Du`5DB~o;~AepASCYW+4<IFJZ?8E1Vy)zTd{LevY=Bn&R@aKk@IXI5+o5"
"k%~eWA#<RY7uJ8O*Uq*r!XH*Ka>TjHaxTA8{B+zx{5Z`-vB^tB!BjY2ma$RJ6kkGwQ0cOI@=aczRL_Fs(fJtgY0Qkw;3p$Vr{TcD3(0N<n?o4l1GV1Xb2b9)"
"sR_c>$6fnXL+>L?BZQ@W*eXg(M^l^+U2>lNTsf>z?w{D#k=a@TO}RuVD;?b`u&mfWQMoREQwKj;F<~{#HQI+^>IoXg$+=k9yiE-%#7K=V|5=WYGOB&od&bEz"
"ewA>j>K=`7A!hgY6sY>N9<{$mb;|fgD1bh+(M$o2Qb*R+PSuc^`wWKRp@-h!`$W-feTbiA=IgII>0g>1$^KZ9R3=Y1SI!RMm~A>ghl>2v|EFwTyQ!Z(H}rHi"
"q|$w@>OKHU@L{TXY;&^6x{gwBrV#{`+iFgqb2DyF-Kk!x<p-S~PJQKs3R-Nwotc?&6EEx^eoD_<M0|HwWPP+zeFRXw-3@;LboF*zdg5-K_t@VAp=v6w>Jzys"
"t)c4H4LVCHx?BPMOzG-Op`yJ6owF-bFUU33&kESlo_q`(&X=!LQjE71_yHyNC7p(j@D68ka+T2YrS>O8pHNjhKS#1y*9V|T|GYmqoQMIZv-VU;b@5f1C^;Y%"
"w{1LK7rz+S2j>d#+-Tk;XAD<`^rJ6c*c<fYutRLy(ynLFnjJ}PKLIj_oSR68sz?o;@&mC9w?PyO?Lb@I9qpnFf!XbTDdDGPD6U+fW9CZLFKB5n+4ss-vqi=y"
"FUW1o2lYuTL;nlhZhW|*2BFiUIyF_5>B!AVK{=HUouVOWOBxjleHXmKKH^I;*gwWe&Qo!WlCQKkdr=$TuQNedf}q{R45Tcf%z=&Oi{c8Qx>}N%ybm^+ht+Tr"
"(hU+=-rIWJe1qp18_-U7s{|BGi}RsxVdgmP$>h{<$L%WZL;Q{HpW8w-gi!L41s*t7C(>a6LS+*=92N4ZPVcOUIor*r>4c}*9=`W^ujjJ7)#;a~S}S6A%#+UM"
"%<0yeyFY$mR&-U~AFw&yODc8Fwp5limy}sn<L*g2mfcyYn*C;PHXF1wYnMe>Hi*iD&e_?a=$5@{ld*%~+C5gxHovoxvP?wTRihO=b9X#Bb0_9gG`Gw#o!F%8"
"s@a*@v-vMoPzTcww%M?Ne&wenQ^mXf^1%}Pw87KR$^Pk+YOa|2P=4z4)S>(G>VeLMygFC@M7w|Hh0o>EXS+|%FAc=O_?OdWyHK5_g=;OzvfcxoP1{-AIVRSo"
"yu5aH<#YVLxr?ilGGwzl9Gtt8>(TZ-%dP*VJMpUj<ZIezTldZ$AN|l>ydCaY>YV)M`m$&*<}+EhIbM0l_a1etZ#x(!CzkB?vHte`LA8qBp~b1ojbf>nn)jS|"
"+xhFQYn{23-PtdExba_ZyZWG8|4@~CR%Gi}o~Nd@jnjVh)bi<hK+K1-JGEfF*|X<vneZ3OIB$>SV#39T!#jBAkzP0W(esC!@WZULT8N2U9ZsD)IeSMtJ~rB!"
"IXa%-_KhR$V)cpc;`-L=M*Ft8`T2Xr*}i`G{FL7~S8;D|m7kfc>>k-P`R%6%*E;J{vz^`>&kgy!y!F&`zSCG~jD>uZXT$MRoh`X~s_d<|*54c#TF>d>(RTKc"
"fn0n4(<h$k&tEj1e|hTS+*@y1T<JaWz@+zMrw*9wgSb%iJ~lbLIOLrp)w$#6<?%aOvxBX)Hv7&OchATb{)v3hI^6icg~^rSy+{6kiry<ovhz&SE6z-eI780P"
"(1Q*DNdN>uSfXLN8|__nb#-}@S?T@BbMkoay+0?tEAQHOqq|{AV(2+;SDJ~@#u{BHHg+Rcp_TSRx{<CF6RN(-i;T#OJo*0L_q>06|NDRR9dHw9Zj6F;3aNl6"
"=4EW>P1Tp?XH6T%x1!r?@1Li&*v+54Q7w#@%G<`JHb9wRHUYo2-i-x$23%^mb<BF>HKFvi`NQt9KJNF}b0cr-S;mp5KL8E$+_H>zvuLYY`Sq8(0)79qGXK>*"
"`>xg4-<w~A4su-8dsSPhGMU=+t-qG0muy|_K7<{6Qh9YbZ%cirb^7!pzLPZ%ozrm_e6))`Xqw~P#zJbP<8NLW4&KYM$lM_vd~rUx!{*lo7P<1yK`F6pg5KtZ"
"n7=bGH2sO{26_6dxaoh$r#|MlFJMcRv`~#J{$>U6LhGBg5b2Z$_3-VbQRcJH4S#1K<b=chGvBGz1rR<U#nF}RbUNSD>d_xXCsz@M9DTz!T-mI6WxsZ{e4Hy*"
"Q7<X`Y+v<Qr$t}rAu{-e1~?%+K$p;H$I&1;$KhwUo|KBGOUzO6hpN7@HtVkhm&ydTwEB~M+~fu*XX;dI?oBcOB*qL&rzI?OikJLJTTQ!9<|sYOCHpje#!%Xe"
"bjN8m^GQ=)j?e~$67k7I<aad}%?{*Jl3Ta^hQ@p$)nSNUw;{bT5qNU6L@db`>o;P<VMIoWacivxHKcm|+!b7cDUf2WvMBb^A#SgflG+lX^JlofC+a1LVa$5E"
"M0pAdRav-BCGwP+uD0Any=hcHNhevL%wp{vONtIoXMF=C-GEALtWgZ`<lyA_3!EE6tU~`+y+fh^ezk)6vI;=hh@~Vs5P2Z<fHFxpeSy<56$zQpGPwql5VqR}"
"XOIqqMR@ty|0g3NfHZpq@pzFK-yRDoh91^`0hO)5I%Nd8>Yc58IFnp<jq(+_*RGh-<=nzw;P6&&(RgpE0uK!C?a~zAb3gQ8xz619-pa2-s{8ceH|~BdOq^5a"
"`PDl6Y3+Koc5t(;HQ!GgyT%zTFrhy_Z$3-VcLZXk!+cQ|`nG1F8-sk>Og_X-;|B5IMk3m*eRMvn>TS_Dg%p>>vh}-*M$kC8FyUu`XrK7WN;;6h1KrGQoZQ$_"
"%+lMT7U<0KoQ|DUabT&%UMQg%5<H&IR{c-23ea8ESKgrBco+B|ubp{c9xbWbIbq8|zCP1R2ZdrPd3RT1Hn#uO?`n4!iF3z4Nj>lU6py8sgU;ExDb+bQE!BmI"
">5nEY?wuE}?`5BRhbDTfXZdRf*B60U#m>HdiLz^}0>?q8v!Jv@oLAr~ULxfqK)Z*welz)nQ{C3neRzJiaI1YTK2!YK!ph<^7+_p3?4Eya>bbabz4`zBm5}lY"
"c$Fr9V<7N)M$eh4DA_`P<rY+1c>ImKboitfCj3jQ4-gTR!e(u3`g2K&Zk^AwgdLZ&8#qyK@>F{Em-Y5mWqA18x&GfV39`la?z66td0>6#K+wbi%G&~{W$?E;"
"nWgudmozOyTxP0gB1_z{-$?8TehXhec8bCXKlk5}hVUo1Dqpco@seU6y7$#XMQz$IrjpI}n+-*<g$e8xRh)h4dmZR%>}*IN0|O~N_EY@~GhI~y1BMra#=?ak"
"fy~hoE9ln*=I#D+8^Nqg*FIVFEARtJR7f_~c2VFlURk|-XGTTMy^BWgn&B>raK85d1iC#|Mc(nG2`mR1%-($hzmj2tV36oW9!+<*G4CCQDR@X<&-eNdJigQ0"
"ZzR`=l$Ky`nRmz1_Wqw~E4?2iYCR){rFq0#O!SXB9^Db^<=nZe98`@BBbbn;V;%Eiv?w{U4VBXdASstS&^rl*LB3t3S#9iXmMSA~JcNX?v`ogVu_YUVTR)eH"
"otTjhsfw1n7!yU7%<c++VSZQQLv2-t%*XzBw{knI$*O=i7>ASs9&}Ez5wq^BFQgw6hP2QRt|_Lz*NB3iQM&CuNc^x#5Zx;vQ`}#dn-PP=TIVV&x}tqVPCKvt"
"cy!bnIXf6jZ|Lug8s%=UBDKQccAW~gqf&R&q+<_%l5^tCLK8V@Io}{Dw!Vh<;c1QQVwPO)H7cwEAWRx|#<J2Oj_5YE;#rGSY5t8)zKdyw(0*EENH{5hz2IWx"
"&ajZq&-w0f2)=GSljl^(?07R}h%#`?GoZkfo8Iz034NHZx<_O&_7bOuGuRqX_u^bWfBRs}+j%O(C`m%ocUF%?tt6UK;f(Y~hZOq$S)V8>JuH_*0i9l*11Y~5"
"NVbym@8s(`nqBLxhZf9>IC)3)H(NSe+Pj}>jTNV)aD2e*^0GXS^#==zV-S74d3~eaq@?vUdqtCXO~P2ZhK^F;q9oMOcA}4RAUZt`98KzW;{;V7<JV|uu~}9u"
"{7Inagj3@(IqBVMHnvH}1cK&@#mm|E9G-=!4yp+oM}^*sD2@Ax?`Z33m`9H+2vQAs80nURN}~l5tw_k5Yh!2R$olOL+7zBL=G2w}v$Mb7{ixySqK(Qf?$6Tx"
"Ij#b#4?$tstaSuL*vZ-ERd}H7AVM>y)cgOad}kc(+pQ;?AI9%a6Z)}B<NpZ}NPiE}nhyS|ju_2nY<sf`C>xkh)C_laydnTyIeZOh7<0C5;}t0ND>ubNyC{NR"
"IeZHh<BUJS{@M)gO|k%NXdzw~?qk*NLOdHWWXWVNG>F2g?y+9Q$$g)mpR;APci!#c+OYeMzjlt=AgO?*>MQ$3KUBJ)Ot?9Z=gDAIl}Bjk14WB<bfyWQ+1mT-"
"7wR%HXvrW!$I_VFR;>9nWF5SEk1<->8~K<=B=vB#E(r`w_zF`E*zOXGQ_f&cl8IgZCdMT+ve2u?7<||@{dviB%*cc$IuyHgR}HVJ7>>wHjT472L6ICG0+toN"
"&xLoN-q*6Y#%S>gNsO@?@YRBQw!Zg{$eR23t=Czm+&`I(2A?j~|JN!mk$KO*E7xwZ1^>)lv-b7kXuhex2JTE`gOh`Q%C6k|1nrN$aW6I&(+M#i{s-S&GGj`7"
"s2|?;p~fbr2;VPfQn+*%JF}yHBwgtUk5?p)nvun2OZbQ1RR6>7niucR!zp=_>A+>4L0(}Ng_#7aT?7OcChFtg?mKHUOQOqRIcFB=)G+s6jqi|uMV0SO)6T<K"
"I59%Z{KeDraOt0kz0mB}*7l$W2F+u7JTc8}PEI#3EU$kkH1s-ge;8hsaAN+<ziY*nmA$_+o|ErBKr;CVe2a>5*N$&lVfyV4x?8*V5`!9;2Vi#ViwV}RIj@pJ"
"WNiPbKf9WdG32>KcP&10m5)0=ZAu>o`%Sa{%I;mXL*373YP%--CeLS=ep9z<$jOA&jJ9QJ^WLPrx_chY*+>V}oiY_pGdPx5GM#x~awF$WxnG(=R&VOt{VvdK"
"`qAchyix<XHy=jM^((>UkB&;12IR8W+CpY)cV;=$<s)gC(iUL(eucfP5>1L_Jn1Vek)`W5a-QZf>x7;3{aNigUX&lo3kLjrL7c$8P5*X_HLt7|<(=+WHoVFk"
"Wq~Pa7ezhc_G9R)^J`7)57N1AX6Kc%w>|@0nQNM+{?S+C*LBx!VRFDS-6JU!*DX0V8L|v5wXB!wv1T7`1Pi%6G@O~Ieg!VLDbF`-V_8pU26ec@D3=(t%hplH"
"V=@x$tVI42jL#M_jKuj2+JjYT1E{JFH8yJWlz%WkXhr&pU1>;Qa0G3g_avisJEvirotXBU<sTE~0)>MD^wA!^TV1CV8#-6=yY5a9?ir^i_9!FH04mu$-+g_+"
"23t(V+^!A&#@Uj_{aR2bxmUx&MNvYa<>a^9xfcA2)g&8cogR`?Eh<A7byew25Pw~4Z1G);cKv>v9%+jYMr3e@HG5i+P^nqzsl>6X&j)+O^wtNmfK*E56{w`|"
"x7L^1ue?6f;F6V~;v4PSrteRQTr2tDq!QZ2y+oBpR!!$%^!0Sm;=@qi4am;w=8&uV$jZWc86w5|Uoc?7xqjH*mTh=D&?<}~TaWC4riY;$WM#2gaHLw(QH^pE"
"KaP)w$kULB8i}>`Y`VvV6CN>Es+!Df%-~0k9gagXbmT~;TCtIK$Rh*vWE0rczAOl;Ti8MTpB==x!UjbtR6}kV-E>^vK=ja1*ApvHCqa5#b<Vp<IpkbDlywI$"
"1{vb6*8t?J6;<8tBMn}l)L5-?wxWbrXNcmb&$dyDKILL<a?m{}u7vGl|5v;J%<d2Ki1Yg-@~2ao6?PrWjWJL?7A2;0S+byOFRks?&dT}g54RtdmNuVNQ}<T6"
"M4llS;Dfzx_xh!_5Z+~OJ)RG5o<6>hUBpfE-O^O;4m<p#zES8;Z68kqi>#^VrLX3!AKp*Le*!!m29V+<Z<yy0r71t%n9Z1vI`uD0Z%&kU#k*>v;6AXM{x7DQ"
"R*Zc2D<(&G($ADr6)dBO!P}P-@ZbL+6dyj@TV9GoF*?<3|L1lCQyN^IyYg<m)hUz980&0Z+?dPZ7AXzPD+lJhaDS0#IX?$qcod!N-+J(<w)@{y)^+4y>%{4e"
"#lQNC4@gJevDN<Xih)l|s;84hP*^ShmuZnh<fYo1=QGVWANhlY%vQJpmZwMmDKbtx6;N&Pd!VCYeQnd`+w`oBWyIe<cP2-+R_lH<M7!jmT$y3`#!}}uyXF12"
"p7k$&%Ko(UufMJKraKdZi>VcJw8P}zI^juEpj+_BMplPzUY%!hWWmAaDUN<|#qXu6t?{cJeKgR9b?FL&U+qTxSVU&1)k}M-WaM&t?dyVh&1v^mIahMR@{bme"
"h(r8I#2fONl|Gzh$RdjSRU%z6%l(L{$UOaXyEf5ldu!yOy8@BwmTHC@J$-M!mqPO{Wo}kiHxr}sTD)GbOty+bw`@ba(cOIc#}1EDOu-r8z3VRzSaoHP%}|O3"
"T<7E(&9WEg3g{}khQwK1l=ft$Mlhh8hhzhiKa&@eFj%fu&LBdMnd1qorWPuBGy?VN`enL%P%5MdnJq$O12s`z!R=~Y!VJ>m0dz~Occ*{ENX=Qqm8gW)fPe4U"
"1YAZ*Ra&vzFfeRRm+MuBV^o!PQG&gTpao<))DnM2qN_^+0lJ%p>@qCI9JT6+`^5o-W|E+5m(vPACQWR!=u|t@FKTRs;%cWpNQ<=(iNT@p?ly<9A=t^0TZvVv"
"l5cfAQ!gU3TAwLRHEQE?0-QLzS%w<ExqK-jl;kZ$@fXY%aItlRq8mXfBs<Ph5xD#BEZ^4J<=2cp%L5nAZi@f=ITKeZ)5wogPv)8G2?n^<WJH#CuOdzGj0g=I"
"SFbEf+nIMkvVLp{r|(H=<V0A<C$QI<GymKx&efAM<yxwqe3h(wjvybE7my)1x+&H5yr^?1PpvF$^w&*V;p`(@K3ZuXu5{2=ZW|}f7j3&&e=IBf8i{NX4q4+#"
"4l!gB=j*lW?E)IF@bqMNy#>GMm2pyR7xhNLj4yB&SEf>x`KAbx9Ix6J&;ceC1HIDY6LU5L$n7M5nb0qA2ruOO!NbSF4b+<m!$f|vb~x^FOAzjgPW#9Sy|01j"
"v6d>;S4_PQ(uQkMWN_NTkQERo{U_VU1mmHBJM?Di{2n0d=8ME=(HZJ(8{d_1E%s=|I4Cz{wybb2Y>O{SDp?CCjqdo7MHDDJ&tiZx-`%0D8dG+Aiz}iJRZ2S5"
"SoT#hD#jl+O;@sbIN!hyT+V<Z9O)!*!slh{x*^k%ZmN+&kDEpWh&@>bmGQvYU@AI0aIsqZhy?541XeTajE@uy*W)z<qn&lBEo1F@5@juOhpbGz%lK%y>zd$}"
"sWY-r1zF3WO}4>!9>I4ELKTXJ+BG@XK&{j;RD5oU=6t^<y~C0!69)$%+Io^8NM-UwkX-i|TmP}9kD@?9|I2o*Vcb;|k6N^!1QZCeHE2A!J&cUxJV#SabeLmC"
"mR?`0Ayf=-zA<ZpEx{n{M(BNx@hag;0pJrRzth}B{^slHp+T6Ksm+PFDI8vJaXVJ|d9gCTy^{jnqcv%ye>i{7lHGv1;ia0B{iZ;T&EApmTL~*c#07QR5f{j<"
"sG;=rzGb(RJxh?isq76kAD7Em&?O&L?-%ES`O9y9kAc^27_ID9{(}b;^|W}4>?_}WN9^60IMKM3R_<vUyE7)Zl?!1TN}9jSfQuY3drY+TPsuAE={u<Dol&?<"
"@8mb5RQh_tBVUF=x+$}|iYoe%Ig+<~fUbi|m2-z=h(u!!t~JH8#>Z>nKp;%DIywkJlQC8mz3a!MW5Jx+3@OiM%FJL&=OI+QLowD_=BI5>3#twk0EkNmQs)NQ"
"4531sZnw6C_P@kEgLNE@P#t5$>xSJFd##M5#}3s}oFGPJJEPE2k+OOcq&XG@12jzwXsiX{^}`l9Q1~*&3EGhT7NEZioNJhV=L!ve&#=Tucl=A_P~at>V;0MI"
"agXS;0(cN?@3u~b?!*l^9$TG8efh-!yZv0XtIt(8rGtjUgt7gWx^gb?n}&sEO{bTz%(6a0Kux2I+Int72FlaN_dUkYH{7N~ySn~z^A2U9@3(i!ich{ub1xzq"
"T5eRN3ix(t49F?6PH9<INjasoZB#ckx;1CLs95DX^bY%Kk2_*J&8vJK1d%By;U)<g>!zb3%9G8ql^_Xzw2yr2NoM{_Tt5@+z}=-#+*hfHa(5`95fNgu2Ol2l"
"8nM{#6EzdvuL1dlIc+l;JxXrBhluQ>Um5wrE!5;N>Q3aC1u1D17Q-9SmI<r|leW^OPR5@zzV@ye?!^u|7o4JRn;70iN&@uT+sa0+zcKQ=py;E~gUKkF`FO%$"
"1+7AvZteU1-SU-kKUKuDtK%Hy1^6nH2ARvU$&p$o7vYIG?Aurp#A<V*TqF+ea0(j(vP)`)@0l>?Q7TNpEr-5Z56%?@rB+9dF!~-x3{1ODLCUsu)B+Z)UAfw5"
"t$~)wDlKA>ElM%SmBQPadF&yB3ypxw$Ppg-6nHWhKU~?4gpy%Gz1R{Q(sSr}8Sk28BV)4dtc{Kqnu5YzT9<cnI5Dw!Xsa~m2!H;q@BVQi-y<NdyLPHYZ8l=~"
"92F($$v{{U-;amxbi5*5sorG6_FC~5FO3tdm$Bf>e>p8`KR-3Oo_^CE%t)JiPRkmkMp%XGr(W`#z-ZEu=4)(WBrjAdx`dfOlHRPy*AnAgTCQ}j92Q?0@xwn4"
"53FM=RqR8W9!+%L3;(p0D4c<6a+R2AW8TYU#b&q8o9JAxI!U{c8@>4AlCb2ImVlT}y^xHryJh!}3*;K|mp}PteX|HFuT^f#IvK7>g=rwU{C)(@uJ*cxB(HoY"
"oEDDVaNDzUePb!SRhmdO8L*ht(WbF<VZ5CfFqx{>Cgny~P_)O()JCECVKcq6yM=W32YGqzy0GzG;a;=+UZZu>+iO_-vY2kU)-u($<)}2qNaSAceOoLl*DBvy"
"tQi<G&~82&JU)#0cW}Q}Z6AySFs!tf`1FmnGWFu&7TEaG?A@@{ri3k25oA#i6*-m_%C=NSr4%~mB9#MdF3#AdPLZDEshIX;NJ4T<QulZc(9}>M7#=fJ*wPdV"
"<8*=xG|^y93spQ@uhG6J$GS|jA~s<wEzKACC!-IdB>7=faYWI`a3tpnrmC<4S@NHpnHG5RNqQpUX&fgAY~2)fu_|ijlXJZMlT%BEpfuJs)nOYfp%5C8njoB$"
"9Lja64zDezFtio~n=TG?*Wha3rLZIu`}GQ#!N*N{Qx}?^!gZN;*KU9fffhIOWNfF0Zm>rwE3Md?F2ia5gp}87P#grR)Ha=ABR$2d4Nb{!$KcvATkjSkXhc~@"
"axJ4t*`doYd7rEoxO1y-w=dwGE+Qwh90l8>?s+ZfV4E&T=NIVoCSa;<s{u)n*(1lyo56F#4(12!ineJHZJ5bXWa|706>`^6v2!`I=o5yysb!}(*O8E0y>4ti"
"bfYtv*IpT3?cNprMCIb!T+}JEjR?4W2uo`o9y*rYeI2orrR+LRj~knF8xn!__(5)80hfv|QJ$)uU>-R#VANukmlovasz%8cIzJAtkmKc2>fO0l;lZEuEMXTB"
"-7qfa_e2_Q?{&zlXFF8dJk08B=2!1=)&w#8+TiNJHMqkUw`Z@-io#h`O3yO4p2-^xDV+-zCRoA0ad%ie5$uN9bGZLL`ahe}R>YTznOh0VV&~r-caH71_KJh3"
"{x&u`p**NIaQk6(XAx~Dn{<L&E`_(wu}9^f@Wjdex!R?;%0jiN<x`T@Dlp#X-S}#1gS$imgt0Ww)PA|vjxKsDYtu8>l*krZ7utkHSh^#$g6ti0n!W6a<}S68"
"$}+#0J@x2RdMB&Sq%K0)G^t459*Q#%3rZM6u|{aRvC9iEQ?Au`2G8-K5(@yXD(j$%@s522n*9z*la&N3Hp5ubLwnR@Y}?nvNUFG5y@B{O64isc3xP4MGbuS~"
"ct>!X@CG~&kDJtDs5=Bqv@&TgK<lJS@H``hMwT1Td#k!SG17%COLHQZV&bDUCQ%+qI2#63^BNkj1Ux7INJ+9|*(yR-WLfq&447~b%Vf3gvohbxeL6DCpANtK"
"&A!}gOeoEUD^+D;*=TTMP3}j>5?fKLAIZE43w2FmO}TTK9ziR9S}3I?2VeJuRiOBrkAW?>sS~L#V)1zPx*;B9+3DtW&y!G5$NpPXbKA=r-Ecfj;4S~XX1hgq"
"JVt7eyumbU1{;>Q+U0HxS>7*swr*RM3Pg(DoJ-4Y+pto^BgwQ{o_9oYO{SY$#7x{%R;+5PM%IwfnE?=5@4-DCK-v{LlG2TR=1QId5L=+fdTl()QdOEPBl)Ug"
"cSjLnw++#>uh(HpdDq`#ED1d@R=_qj$GK}Hx>06=-h|BIKLwBeU5OH$zx9?hydq(zl3kxCa~P#Cr#aIUjyzTNXibcpvTPigF7-7U!ZU?r{V;Cni2~0<ES-4f"
"S2Ny}&c4+aE*&W*L_g3;iL%7OhRnDvAShYV7>WYgOBp&Z9mCflHM8NArh2D`S2BF3S2v}GwB-*jkkf>lN}9bv^TxDTx%Oljq&8dfmU=xAX7e~?<gpvKJIvBO"
"xrt$t+v?oWtMd_cK8)bL2de#JonZ#bvKxGrB!v#!<$=lcTiFRmKh39?&A7X1-UoIaHfX(;HM{Y{hwPH_;X4Xys!6Ri24Qwks!Qva@BQMPWWt1I9<|d;O(8%p"
"?FYA!Q0R%?1?8)cGGAL|x%%1)3qRSFE0^;3z=S`+J*LaW^Lu@|#hLH(L1-QI0++6LZoa=^Qif_PpmtiAsgpmz53`S`XhzIT8*(8am-shu?u8<SgUg_~e0#nt"
"MH|kGr@PA!<&4{o%|DdG!R4V}Ea&Q#PWuMr8jZ@jLuvEfg{f_M$@}c;57xtAm+ShCTop|G(zGS(Dyqk^Ct&z2twBrWd=<GKZYkA>#pofZCySME(GQ&WIC-k%"
"CuBR_mI0z4ZIOFieX6=r_GQbw2Vd<?YsenmAZH-s2)`z)JifGVn0HCl4b!(BDNW(w8uW=fDY7ygwF~Z{gtP#YGLJzuMMZVp@W{>RkPd`-=LNlqFBkOe<Bq=F"
"ZjY*)eOg_xH;x*skFaS6DwLL&4T`Q$AndXbgk_<rX(Gz1SKACNPgWf{k0)|ueIaa%ZGZL3N@G!Nnicg9Uz0(!s-QAo8r69<xxKW?K<R{X)E8UXx+dhxbvm!C"
"A~whm>U;vdCqsw{mX!eoYq|2Ef7w!j`5TDbde8E?d#<wK_RJnv1H&#>HpWIJth7)U3Y<#3&Tgo%)<kz(ZlcH0D$C&d7DH}okQ>8r<*Qvtv~+qRG={{{16|hE"
"_o}m7M7#H9&}mP}jp5IExgnUBt}WjBQuW&`f^{=aVeq9f7hD-f&r+GWAqM9Cd_)7mjBV~Kn;mJmlm<j;&O_+SBX8smMVa*`xhpsITe-y(X99EWqOp(nB`=t$"
"UOhMWGvmI)uiZJH(HVLgygjzN@7?a*+GDe)cF!xH9z@x_6C8R***(SQkPRu^n@ZF*JESPM(*4rA(-ks#6P2yW^^aDoM?mw38&|~J6W0OqoAi?Y&DVFH52_q="
"JaJ`k`Ov(%TA1y%OXXhs8+80ovf9=W-w)3*5aDOQ-+k>|^vS#V<@v==Zhq~%K&4aKAEnWKa;rERTZO**QD>%byYWBdc$4U!K2NXL{}xl36HhtJA)4LzSc~A%"
"*!)Sg;D!ENOWK2j{hwsA55`#}aZVOK1U5dtMG03v;dtn&alLUR*Bq<=`o#GL%se=W-NVl}gEzkhCOdzo**q&p9<B&1^dwccplAl?YOKoWg6&v3!x{z!I%Zbu"
"9T5~K@IuWI3`w#i)mx%Om1Go><s3$01zK}BPPS}=vMff^X~C&DA^c<<Q`HeweiEgxP?Tz`rUD0!ra_jj79>RQHO3M&fiY1HJhHDU$hxv6c(!ZfrY#$Zx}&Rv"
"(b2?a31@X(78GGZ3>XQfxhH9fhc#!vqWI+O8LqHO3GV)KRX%#Na`4ux+dr#{&&~R)rrRR9V;^1<Z+rA=FfBeF9P2c?74{XOO8J?V^bg_*@kD06*O^_HMfW<$"
"E$Y-|<d?OVc0K_+-nGsQ?4LYY0QUZoN@o3AM_*A2wID^Y;x4*WHL#k@a|@reD&96`jGW;sJ?Z+&l%sxNiyiIXw;UcWoP~y4yCMcbVh(w>S}uI&2%7HXt;$6E"
"L6@&K1m6%yCkVw*BqOQ?<`qxaa72=-uqA*YsdC9~J2|;AqiZZG>)=b2D~eXwql-RmcIqt~bZG`=260ES>YBwdv?@`Z1sf4tvP239(!v2bz|rdz_qIz3u^P3!"
"ohF;ZeU4{ihg2xD$7IJuPm4j34Unk`#!)eVlY+HC3P>F;^B6B%JQv8P;eu|JRi%aGah$YTaL~!~%UdW9=@8scPBDR{;htJRtd4!rkrUjNVM5A*i&4G(>2|L^"
")?;rBE+I6Bbb^E%ZVE>w=*nCV81*e@1oh|)7l_i$tAj33lZR+3u_sk=4MGiy3y;$m@Y0;aZQLAc63pi)rq#O^W^G@A^wJ6&Oazr2RH+Ysn3df^P324Wtqfq>"
"qRC?m_FN~a5O77$7#+@K-a_c0Ue(JhE>Sj6RIJ4+oeRv>t`ur{SlzEN)G}UtY4$OoOpuNuwi$q+*DFB)X(F?N!#ZWhtGtomsUWZ8>ew9k_1U)QD+i1W4>fa2"
"TK%u~Dx3~K*gee|fL`-(1ZUh^l66h3#i(3!Ap`fFMW1GU6w_@iM)gq#sryFIOlSc<+zBxl3C^M;wW(MRAcjiKF%5SBy(brHx)Q?F=3B0;6Ia>sQoW{W6BrX6"
"M}k6Dhep2GM@YTXb8(j{%%|>i66umxn}<Db!|kV){sBoe!J+-e?z_F#?m2<6F16Z|vq$*KHf1AfI+ptQu9Gd(t>ke<x-2x9G3}GIxB0fFTS0-2x|o5sL4S{J"
"g5nu_K9+SVER*W`%P)c$+xM^>z2>QbU;zEX8P*)E;1rX)y7z1DNE9cKN1LO0@n;Ee(R0s$#lECQ2P5Mn{BmYqD(sh+2Hn@i9(#|_FBw;{6N|0NE^xQ5fx`66"
"_lEnPNMAg+*H|g~b46!WtsHdQ^hjZVUs7BzWAwK~ytW)KW@R;h9#NU-13c=A4PsV=C-X|oy!3(+b7JFet~N5laW$Gcm>RTo{yNk?fnrmp6g}HLJ@fcN-kb>H"
"c3Y)tg~(xHs$?)b$TYZy7juqR=q|W;tDausq>WzLOb8xvBq1(+jZ`?+n<Wu=2E#R>+oBlB%<kB`{i*b7uP&I?*NKC(rHwpnP<gM<-0uo(cd{|(jI)svzpp!p"
")OV$Q)D5gjN=Mhp@)DCRwa@i77nKL4niEf!L>-7+6k534M+td`7_h<;r%Uzf4FZ-3?M6&7cVDWRXTcy{cBE>{ska_>n7#IWG0N>bMnF0}BpmG~OV`lL4Rd+-"
"KvO5=p@mA4%z5l~3TQ-8hJj7V5ZeMzyoLKqgyY}{wk(P)vVm~yt`Tb8U{Pn=PUm?W&shf(FtA%{9lnY7{Z3eK=PK(~9br*>EUV5;sn7Lh92gzRP@^W{MOBse"
"1%gdiuQiJ%)x77hXEHa9W4c^!l~<USylY{pS`k;b^a1$J5uh=a(W|I#K%A<Z1dAilj;OPXM_pt(&o!Y;cSc(Z4Gq`1-=P2q<rO}ZHBioT+cz}05zW))pfcZb"
"t0(3OS1)$A%QT@mvD#$xsK4N(nm$uNOI2o6KgofVDT5!VYoxSoyugHza@70#Zgup$Ax%g*nLv)Os9ru8Cb{R~E48a^#JL$)bE<&FY%Noo0Z>ZU$0ew@Q}Q%R"
"l0>^LbkdA;aXd=4dezONZdsAe2(kp?IGrbLC`@_fFb~a%LSa4F_STkywRMZ^Zd+;02Zx@B=#~>GHOc<^a7cYIIi@8sIQo|^C>G1G>)jMR^m3chjpS$~tl|_O"
"x2YM!<bz(!&|gw1n+C5pFk>J+yMYA#_GY0)pf_r~CQZUGK<4Rn_p$V<lYp8ivB--?qtQR(hh)j^Wmd=Siq98HzOv-}8`jL8{aB-VL;&yRu%^N7EE}Y81f8CZ"
"ny(U!*SQI;)e=K`&!Gu>Ec^(B9d-51B#EV2>!w|tz@gNabtP?-{=U&z!Z?k$|Krv{M|3db=Ddf9@jIeTjk-4(8n~*o20XqS$H)z#>LG|+K)+rU*Nne$FSg~5"
"#8^16(`El8e@@Tvjn$5%d6G>x@L5T)x-0saF<t_cU>zP`LluU=PwH2bZk4=D;E_d|F+dCZJMsn66Ju|`=}WGEr$AGJX<K6mRvRqtw^DBt&KB2YOsJ3?l4s~7"
"<I*j%UVMS!KC5>p%IQ~EJ4_<*mG|dfwTg)t6#iBo>CV5kH#WNRO)`@0z!NuWkE@a7A!bvO^7PIdZ`Zmvy>p$n%pExkV&X~$EY#=z56Yy61vc;GjyIOn#hc`9"
"|J4kP=%q$E`Ljp%k)RGD9MtL2k*c!w-p%A63b=Z^qVZ1aqhf}jD^Y5)6_J-x?LeNTJC}`Dt2?N=&9v??=af_LB(lP)cPVa2H`#AY<Xg4tn+M&`^MA8NEQ)Mh"
"Vt+9{Y!dA&#2uKK#^jfy?i=z^XFj=^60|UleF{C5=3)sDGYeP<!8siK-{P6u&7{0Ai%Dsy^unfNFzE7Cn=b7)pPU~=x3rr`aghfeUOLuf;zf5QTd&93*A5D2"
"a}DLk9_bq;=DxLcPhDqDS9jL8p=^F>%e*yAi9&qxMyXn1Q&)LUYNln|HddYfaw84MO+-^ge?ZokkF2eiw7F^DfCZB`W#m?xvrAl)arl%67|e%eP0TZzMwrZ_"
"NH4D)5|X2=^OBkxJWymD=O6FjFB`*E6N!@uAtMUh`w%fofq4Njq)-)Djdsk=d9vCfpt=rto+WXhM3;gFH$%(idTZR&4m@+hw)h5-3tM~oyi}1frzEBGpiEX$"
"qDJK}^+UO*3`WgbCoxNRE4fK02d_341;CLPwtym=GrC6<?MS(@7?5I{Hw7n+RvC6wHA2`01-zb@?6hevsI&;6nIaHdx}xV9spI~zOeEO57(lI5yLVtz&NDOB"
"VwKSo_|>CY=1`q{c{M9d`H{*9c^@erf#Tw}Imw@pdAHITa7d=o!xHoo4;<O2#{=A3(pG(;*`m(r`b@E2nIO+D<7rTt?Ef%=YqFAfYhD-T_dANY<G)unTa*)x"
"`&Nb+cnx)wOx*crtksCl4p^y+c)_nZw|eUMFf1>L>{kxf=S+{cTKmYi;&gI}?4N@#)VA19oH$L5Ze;O`nKO&=qlpN@E?<cDTR-mHKe%y_O4MeX(~!j1^i`6-"
"-)*O@l6ImI555&u{Rebw<Nn5luwxc6x=>zzEpmF~C#Hkrlm46ak6-MZ38LJNKfpg3IuItBTmvC6Q3a^H4lb5;O2#CeJ3y&n-C(K^C2_ON=U6RW6g5U-P*iR>"
"#xe;BD74BD9L&JDY0Z%cT>;QWP$AVL+~xHGgn;QIXHG7sIHD=y31tmVEHk1)FG%J8mz0naumYB<JQWHMPB5vuj}(qRi>^Qr>uNXz%^p?!_i&!gRuNQeleL`9"
"aG*j9-}@JY%ra=Q$Z+CnKBuThKkE_?bCBm<u~UsCg6q{U*VXu=d;IIN$gZOh6_jiL*$x|4ffm61dPzSBUJrGIEvVU=jqu#PvVCXtPStDi9zC1JHL!5ip(1sz"
"QpGrh290f)Npt0b!F*u`5}TD;_RN0|IHm=%IH8^N5s0VPmyLYm$yH4a-4mVjHxWLQS&;5IUug6Q1Dss#_TpRnCa+M;18U;`E_oclKE;VhE<yjrwGA6Esx*~Z"
"UYtltvLpCQN#P>72Bp2b%i<|x<U#{@&Uy;JVkL!X&+_U@{GpgPYK7CN0PXeh^N)T$m07Jd61VV??3!uu?n3*i$@qc2?EL&M;f>mku=U&`XTGJ}qRR`;!EUAf"
"GHiG?NQ5?s8RQjJ!kg`ZT92<L1#JD!PymIr({(pE={(=u<StWp>-d?%;GfV24(M6va83K+@q1S`_gbP+^*`2Uh?O67t^>@7KG$&9*4|2u!M*)>9?ud7^Pib7"
"{giIi-Xv?YIE1A6t=6+<)HvRHxcmoTubgiT{K_srbN;;W!;=H6esVM{^+H)10N&~xla_A1p#zoXfmzB(TS;+adS-2)FQTlRyg2DbruW4`-R)&1DvRqyW2iKN"
"GXL<4z9&6h8=kq#N1eZv*|$c{rVH=3nNt^%k+9kDt_zgd@>_rP=}N0Jk-lry+VQACKsF}%(vhB>i*`$xXJIDbYO;7Euw=%pVVDXuSM&c#!xjaaQQS!iRpt67"
"<>wiiSSv6k25MKx!$Ac~Qcog<VT6pp15-ADTA1?$Pp>JvXxLvxVvZ?8W4oItasA&bLxqP4i6k2KsJ>uwX-;@@-%$6-UDix^IO@`LQqvZGOj@<;9$%40Vm&fA"
"g|f`l#P5|RPl^i#gfRg~UfQra3mstDh823r8JprzhTz6<nRw5vGYij#dKDZOrZIYd(?1vyQ@DKRmkEDEtOvT#zAJK~8~;aVqyHv&r7<waV5<blxvS*46}eNo"
"ez${)IJ7tz;Hf<6x3p|$LQnTx;}gsA@*_3Iu;c64J_5x4FIpmdA>T5)>;3NGU*I`I;Rx|^@&l4-Y{y~LJ!3NDhn2WVlBpWGwLvlC4?a^P%u)L7F~Gp1oUw*e"
"4#}5zdiByicx2b+BLD(<CVEdoG>-FgXs47aM?xe{&=42uiL9i_i^o`cK_{$|pd{8PR-67H$9RwyQ8oJ)PKa@Ido#j)!*D*3ATo3Zq<{|zXn96bwMDRMoLNMl"
"F4q`Du3%?UtYGjYEMGo)rfc<uO}@%a?u;4*K`Sg3ME+AelAa+@;JEg_w#>HCJhteq)u));HS<?QlN}Q7s!cLnyDr(Ow+;cTT+g)D>-_<IptjN-4T%Lt;OqiN"
"wkh4zwI-dxB2HP{pv_rdZ>|~i+BMV)+|LSHcg6??OX@E>K0wJ^JAybZ`^LaX1_$eRBwKP>z@+Le;%r70HuCfY0xOaRO4D+B&xlA_J6CC&C%D>{4)n^`5Air7"
"V(<~|ek#{0FwWq(i&L*hP04M{q!H#VO1l1+xBEZ(n>SB9S}UYp%>1b9teQuwY~|Z^;;OXu^9$|c|4STwMPH<aU;S+FSH?fhe0uA{DdO94<^+HH8{|(uzczpC"
"@?S)oS4hh=F5VcPCqBUC5qC{oIw^ks$)C4I=dfz767cyS9}nj`SC3wSiWQ3G9(QZ%wHw>LZ}2%Sm|xw~TGe~>WJv)YEj@;xImPLzos7_8uXR7%s8)99w?RUU"
"$D{8g!-)?*I?4YQn|{<JPZA!oZQ33EvPl!72@R->2L-rnNEfOdzIvhd6uzEam~S>QwFMA`2}Ipsa5zBqXrnBqbdmBj2kP^r8yi`DJnOi`i2#-R6^1dsC)Py9"
"jnaKLRq>EwWI(;z{+`aS4S)P1*tN~_@TM+-xA#X5v`1yx7C36V;;oB29|<>%(x?g!H`GetM7T#e9WSRCQuF<#P+yd`m*1eLS?2CdwK+Ghp*L`VmI=J$wRJhK"
"OlOq^E^P~Qm`@kz&T3`f?{^o^ono2hUR0xRenzTeGvBYIWQ3tr#_palC<{heg{D2|&0*2V*g+#Wg=3wb>wxnLf~0`U$2Vx5hrHUd+AcB3;`O!b0!iV-;Gfo`"
"on>6BpbK)ZubXQ^2ir)>KME6@>{`{f`uzp0#(BF6vnv;r0#Tqh{q3f5W~-k@>aYL0ywq$O6T)*klts)!jEMe%u_plUy%ia_As5(y<0kaE7vNGJdK+O*B7>o0"
"J>zo{%@m6mw|3K--$DMdj*DCMO?6S={$4|FBOkS&m)0T<qn4z*e>MNz<L>W_=^F)^li@@5%JF*Ms^$Myl7v)pry-^oX+K@$BBT8SuuLsAO2MT^>vySDiZo7&"
"H2orwdv8_y@bTamL-+F5_7dtgwi}#$0l{kY;3eq|+$0U+Go2<@wCx>K`6@XztdPw(m5uBn0bWbjQHq*UUTQQ}L*n!jT#>foy+)>gs?|?fO{j{)bzqErQtjzi"
"Au~+D)Te1uKw@Lf$$qvHvq#J8WMeAP@7&6^w(Rr$az`nZ>_ysd{u@S1@~Bi&z##Ikq<1`-o^a8toR9XN@0%Npd|SMG`Kw)Z5q{>ZNd5KW{x5sfE3qT39x1pC"
"8t=lLr)Ini0c*QaPr9nU)O(}2zQq?;c)>@wtt}g6g|N{22)?@`3U^HEdHbb4H?g)YCrd*A6yz5=s%3q=kuBG$M+H0I?fesOAOcx!x>;K`W54^>!S@FpJJf&u"
"Bm2&d`BtmS?bR%<=;B)~W8;p!VNSN+z0>Bk0rT5A3Wg*0ezJx924>~;(ndt@9X7pA46W?T)jLYctIzfZd46fbR%m=l5+wi#V;=dF_=2|kMu)o%0YZ`}h6-3s"
"((`Q$CS@M?@MuxSM3)jcN@)>Fj0HKu=tzXh9@nk7Ch1Wfj~oUFwZ6cZ6+&>K5nnkLTINdZG`n51u1PIPYuXZsg^XUqovCt@AXH&Zlkqby)FS5mn9V4jH4H-*"
"l_Z2eM;N}$*O8Wo0xS-$_4x^jWP?cWm|g}6eBsSptwLVmZ!~=v=*Mzf=XvmJVbROa%7Q*mxtj4HkPyq18g`Tn8@%xWaX|86%G+7oOz7ambZNEK-&e2VIX25^"
"N3JDY9h>tH%ml5zrZJkElPU6+Vv<AcgCBQZt{dt;%Z_RKGBd@(?%OeLlypoZ=gfdzUXzOFh;RqHsj$=BT&E+_)<9!j-E`t;OtUg$sI9qOQKACfvBzec7xTAp"
"@h}IAT*I6lC?eB;j<g9(L)_T=h}$s{>t+Yklx!`-3LLGoU5%3iwQ5=fqSPC?4yw-y9uq3e7Zw(m4aoIj(Q0sn6iOug{YK*ua(LQM3k#u59&v<GZ$I6n3X%?%"
"0xs(<$k@s-C)ps&XGF)Y&DawNmKgT&MGL#*-lgmAyoiCGk#Q6`Xz-M36yUm0lI9gav9zi-jZHp<``+Hk%XJ!kGuPvpCdVeyt{gJcH4(>8Hi<P!*i`>5M$o(<"
"%%R4f?WY~C40OMOS6t;0aGD6v?Uw>_U2r(PiT~uHWvM<Da5EiQte@8!Z84yh=dH!*29V911O%LdBF+F9WEoU{#{MgI&tLfr*QS1YzxIEQ%}8OdS1Ck_CI1p#"
"z4QX?V$@mwJYNzx6N@@OJr^idvdAmyb!$;C|BFui@cRpE4XB8`!)uSaqE5H6J(i`pZQ<2L+k60AI-fKM^!N|^tJ?D;U}dkK{XXq2_Z`rFA-a9x_$vOic_XZy"
"U;f)5UAO^0-T249`}==DGh2Us{P&Pi+XNG7=#LWmAC8w(m3Oy(3AJL0%v!-|0h`D8w`u03i{OV3Ch+m<YGu+|*Doe#KlzTlF#V%Njj5X5W9y%NEMD9HmGun2"
"g94S;!h1LU;V4h5b2}D}0gSL)hc`aDx8iDoM{5VyuJJ>r1q_7~rJMBUKRNfa&Pl^@-q<aYYShCfWn0oL!1r|r(p||_pdH)Pn_iM>ztPs|H*-ACABnSpUst`*"
"j`a+ND*>+7+G-3RIj1$&1z1^**6nSyljcIXX>KL8rX0^(TvH@D4F(F`1=6SRn67AvP7Lr)bM!APW!iHtKIT12eG)RL8jX!3*;^oO5Cu!41Lal9hG1DXd{wqZ"
"$Ad__Ety66`#gR_kRq6>8!RkoM2X$}My+<_38%yYGchCqnaQn6^UT891@Cn7U$M(0-|+mU#FwGYz<AH<Gw>TfH3h$tc>8uI{qg7L2EU%~A~TQ4L!d0{m$4Sy"
"<CdoAHMw^o2^g*5I93JHqF6<I#zXMh_xdjs5Q}&1At*I0bx5__xbs?Nc;I&SjxQP?epW^9Wu0-t>%MfXS>gQ7aPzvcnH-;ask7@z8iHlL@AEr1vJ$kINFBer"
"_|vVk`K>$C5Cp03a<dmMPhWemgRc8mi%k7tpC8<~u;h*#eIs*Rd_;A|X0p@DZqd%Rz6hCfFXfHH1Wi12Hsci)NJEiiOa#{qqS&DMLe0i|P)H@^R-(<@xk=N!"
"6iQ_lSi6TqHq`!{!-w^l!u3r_K{M7(1C#JnfJWs;_<E7oM>?+by$-<-={GweD~0p-y#js%^eb}r9I*8=B(3&du@P}lX-!adNrZ~k_~0$1GRX`Bo|*5*HH`tk"
"d3AY)EU-mmRWyrdZ$~A+ONCB}1q@^6E<IebXk>?wa&9BGj7$7^CUu8RME+!uZ;kuIygk0frJ|kc&|Jbzy5^&2*e#ReeWSpeDGt@i>0mTOwQh5Y<*1Nm;cKg!"
"bNR^YrO>yV#(rU}$5{YVqT7Psyv&93171SC>;r)0eVc1LGv&bBtnVUbnv?QXpk!_o0xtv<an&WOjyXnJp~?LJ6unoFWQldAS6T`nDFQSAX8{o4z>UQi^q8J$"
"@2b1Hs>@fFN$>5wdA;}E=S}a*S65Zr=^l@PHzXHCESC%gm(s4JScRkrnbnJ46pGM`5XyPWjJP+=$#ehne~~xN|5ef5;Fgp%u=?oLlQx|>Q`Szijv<YxC5k}e"
"BRZ%%n2?!zAo$_{>Yd+=74Bix*3;RNmOrW(R(pm|@K7jrsfH&f?2~#eKJDi@AOAmx%XrdlD!HCm8Li=XgG1qGu+bBae7B3QJwvG(+3APC?(0Fefcpy@9~Bzw"
"pa1JMJhv|rd_CPg_#w5cI16&_I&w!&DGj4&W7jJ9Gp|&D#{6q=B5~8`K!b<ix7K1>kuV;{iGx=KeI@E=%KKvB9%!lu)FNL<#DKl@+%9&btY+8k`UsK#RpY@+"
"C(T=UX7_OC6_5IUz1@4WYJPEpIQ&|)Mkh<(Qdj@M+1mbfzf<0Nj9+&S30tho_a1B0|I0Vf+`>6hjCEb;xP0B))}Js82v&Y@mk`Lsj)(k*u67x<#PVh^75vTD"
"@#hR%1Lj@j0QlxlE1RQQ^XoZBa($}M4Vvd(02YLsPEamheBa1ja-(ZH{TuDR#s&vh_Y)js0JBc_O_+atjv~)q+TZIuXek|Pdsi#~e^y13o8m>$`0qLwhP@`f"
"gUO?f+{}6MP=&jzH<<pXO_A9@?_*KtApCOqGn3kwVju3(kFXNzoG~^rvAdamRMR*4%B!C4ImzgK^71jW`AH9kI`I0c|1}Y|g-_&4&R{Dcz$KRI@uP>_-Ofv^"
"#!^&{lHlgG<n0B^zL`9(=D#Zm2iY6?(Q{1jr`mtePY?OlU9JQiY_o@hD@rL!C-33aG`J8Ji8aO&oA8m~XBX#Ye~`M8wbU;Bn~kPlV*FP(%6sg^gaMz?#<OBS"
"Qo1Ml#o%Nb-?#`lV6~oU6og;T`eN_b*|<Nj5fV9EKY3jKND*pm@1&Z`Khu)7x$+FP_P(F1+X8U^`*)_cN@p(_olWv)+sRd3Rbyyx8PIHJj!$tFM!WWXn}N?O"
"O@Z>;8lQP->$3l`MYx6>=VtU1EISZ_Lc5biCO$=!FG>+TCLZMAheDT9Hr&mcUN)j*+4_shX=P$uQ0LTj7AkOR3byte)%I2^1Mj!Sd*!O*!iLrpbm1re#ai>x"
"gK|{uMqNt+67Mwn<)D(`BJD&<Sg{O|Agy?Bv`N^|sBWf{QxP@)Juvi0^c^vo*sOdpN~E(?WkA^;MZx-<xGS19B1!4`LfK4Fa%;1NewOOyL)hsF2joyL==!Gy"
"VSkj_Z)MUobI@qCk~owB>y+xc{MfG9%x;}jL~lba$5dp&6)?UwuBItkbY&V^f?W?%P<_?Pwre}4w=5>VbVKJoX%FQXCfsbaqRr+p@kpYR#CMw;`=STSC4n3#"
"{eI(T9bK6+N>zOq8szY;T7Z|>TwTwkBpmmhU;qO4P+M<_6@JAJ>@p75GvxL-Yzpe8#+BQWtCCxEn)iqj>jTSGbG2_XlwKAVlt3~0AR_AmlQTJzbwA&Y^!eQ?"
"WaVxCIis2jbuE-+Y0sP~>2rR>0PP9_nb_pWN?)@oCL_IBk_?3mCWADZ$Og9MD#Y5<lE!L>SsGdgx}05mMqCK$FH{r>FeR5``k{*DHChM0j|<eAGeGHXHjzu1"
"<<p%z=gvUM%0Rrsz%S?LbW4mjZbXA_&WAJDt`baRE>Ai6c5#2FgR^i2kObND9<hbFd^y&;yjn~MPI+_Y0nO>wg><bX%-%7<nW7cXf2v`{7ltm;J`gNfhQ?#P"
"x*#E8^e$1|X_GG7Gw#B7?hnfS{IOAr$Bzbe%63BdcB!n?_@woW-JIemSM;DbkH5<r^;_CIZYhq|7yPlKb<0dHtaqfaSy~46=>R9*vQ~pa80H<H4^yFtmHgh9"
"F+vr>%iiwY)`Kaj)lT+!7w9YnV9v*@V7k+7q<cNT;+L~Ui<edVx}EK5Z{Iz&8HXi@eYTf%tehe`jWQ}4yd$kJXnbLsd|7V?VmPEHl@}$g;}+|>t|7(H)!ows"
"YF7<D*Sb2=(lD-WdXfZIZ^d8FS1Z%hd+Fn<x@sUndO>-#2$JhzilNtkSloMlVO^c7jcA3H-+#^Q1=7yLex}k`_{F&)Q2YDx>^sNSCZ?O|U9ok|>w1_<qKQZ>"
"@^?+?=j}}D0aadVO%o=z#szTl-Jglgta_zz`YHPDh8q@Mf54<NEQmL?{I?2`zx_*rU~bXTi0bVNhhnY0y@w{4u7YY=@t7#&fB7u|t$Dd$zVPb~GQ64iX<6LG"
"?d@DN@)LsO2gqfFtlcvnKfB{emAJcvEa2YW)WK+hYC!yeGA#~qMNU}Q{24d+7xvf~Rewr%B&(wS&DP!8Al@$Dw|l=sU9!p)(h=|0_4m$y@SDC>KZ}{QLPIg{"
"-dSX9>mRrdap|3$yI)(XHgaGcX{QS$0!gQAZ5+h1x%{nt={fYjX;Jwlm$^J@iH(=@#N(<lw~eJJcsZlbYtrsR9A5d|hqY|IEsuH$^2%vbZW`!q5IKCvH4hpa"
"N)o(Mx^)97@n#PH`obUVV{?4^amrn30#a@<*)2m)^r<TU8#+y0zgH`k9_D{~thIgp%8Ne&(vxO3*Wb-QJi8<PYKDu|6Ss3uz^ML>Y5IF${idGZe@U=<<JNuD"
"*$m%fIUT=46u{ozup(YkQdyn<#}7*SNK-O?;q0r&gv@C=TfkmJG*Bt$$ka*}Uf00e*gpLQ&iIh~X~&7el2QSxN{x1^NLEG@1**dGn!S0qfDZwUO=J|GoCkhF"
"O($r>#kh+w!PHqLjuvVqB2%P9fPx?=j3RZQlqIWN)d++-4+wo3F4ru}K&N8{(NwuRY_b-IOjQ8q+4BEunLLn>SIQ*BH6VZoxUHg#mXtb27kQG_{dz@Rr$}&y"
"rm$5At-Bo>;cb!-#SQT*zXv2qWT2YDA?YoyKxe#X%Q@*aM1Nx{QSKkK)h<E1@9kO!l`8^I+ySVIn0|U;e6Md4(&%u-IbxrtZi<U~<8M~+#gz;G?kkEp-syza"
"Gh_9Lb~78{oaUOw4I{S+KP6Uy`=uA9`xoxd4~=J@9qufU{8%;g70L#lYnBKk8ys$>e7*RGRjSg*_#d8$vSdloxjuTmt*u{JWHoH|-2=kB(W2Db&4W1oyXSki"
"!AGxGbi1L24{^)sLi}aqnr|!;6uhgaHfncrBU!v4XzN5V1<h6&I$x+^o)`^rqeCRspr%xTQQ}Us6{dwUbvqLi#ay+gDa+!@P@k3->ME#w^Ux?%>itNucta%m"
"Vm8gagUZYIY}oPf-d;)bUQ4!;$YtIt<ITh#CjR@L{mkwg3%hHpP<vnHk=uRjj}s^UC?D`3IAhhLP5x!+rJ*K>iPX7aVSLG&!f4QI20LLzgqs$~qh-ao(W9$R"
"cWY3=D@F@;*M0utfauPh9dB(uS&QDLq*E?tTwzS4@KM!V!U)TY2A=D-Ma<+~sTw(m@ayH>?^Ajn?2EvyNntAW-HRp4i1aVt+hx;{`(hMV;*X(j!GC!|fU)lw"
"A;`Y)meZuBMd5quOZFZBl@n93w)?a9#BOQl^`5tJ`OWOHVb^>08-sUSw`n7C8!_PE@X6WR^ohRT$m#-OleEFG(;{GldesCF5kjxrL9$Nt#fi*hJr}q2Zk#%?"
"m9i_L@6p&XZ_ujK6=l)QL62$np<f;OU5L<IuE;Rg`LtO&Vk4b{L@6k4VW+aeS_ZL7_e!VslXfY`oai;&b6b^Kt4krOBZz?qSZdP@rA)=1<%^*Ml;~n<M`nbC"
"=XkoJAR<@~Gx_ahgnG<$?}~d<KpUVGuiGv6ip{Ka$9K#k07**j{C36LG-)P)ps)ld$JKyXm#Xy+#L}Pdd7S4(qKm~e#OG(SvcJVO8fC%C<zb;nKdBL|AJS&5"
"ir%$5(Gqt=XYiW^SbHdCCCpJ*>RX<`R-;SiE?5!?BCb!BAf~RbZa~Tgii?g#EJ0f31|Dfi`C%%)!>+7Kxz!x5{Ddc%*KrPh$zDT>2b_yxg?dtWg&?mlJ8aBb"
"E7K+@AS>I1(L9~4`YD$8+-1&$;`1&?lpA=QQDUaEIjJ}8HantyBr5iuJkd{W<fQces1hdhaE*{FSeXl4o%Cx(`(svj?b?Ka+fHdK!R)Z({R%uJ)Lqtk^Mhio"
"P&H2*0L^bYTP4ABTuqqTHUa6DEm*2F*pljl?gtz{B-Z*i&1gxjq#A}v-xTZF<tkNd_$^k#)KKVVipnQ1lSHAGV@j8A$kCp?r-w06$*eXE6PjPgPMagK_|=B3"
"fg@w(*ah6;>8I*+*?uL}a678id93w%nWjX&vE!=^@eQ;7TE>uSq^!#Ic1t&&Vzq?%14*r&Z$lI`oBuetN$ISNRrq|4G_Z+BXPKjJ!V+%JQ3-RlM}A|#O@J6?"
"TAjI+-Z2{<tgAHq=m1c*(G<Vbj?#NmyUTNp7sWc#v6*D`$iNI#Z?l4Lp3Aj$leP3ah$O=>Z?q;ZzClTMDwqRwG-F#6Y*>yBx*=Vb-h7$r-b>ee-Sx1KyLzXr"
"w0+V4x|U%!zU!qz9~oV1owy3Vg<fox!MHx7TL_YuLoiB@ur|x+*J<k(zF~{mSOY2x#yoKGYnxO2-R0rhv~BD9$b2`w$BV8?$SfX=PN8Ht-NG~Oo&M=L7&-Is"
"?B3<HquM18{X#cD0yAE+!1F$6^(3iF5Z$ph$8KyOITg?^NMy4B3yf|YKu#;jo`YHlS3F4e7e4|nBlW4gx!P)pyv7X;i4l0yPAfVxy4`gCLcLG4@g`kUd~Jx0"
"Q*mK!M`GjvZWRpVynZyk<N%V5AwpFIFlYd4Yo6F%Y-Q~rt(()WpoB9a69Qm_KI?L##u&ziUTxYZ1(1ZJRf%U4k`v6&p?5M&A*2~Wsc5b;nALlkP)N}?;zdKa"
"xj!XlI+ynf_dhyz?N{N^#ld1}`_%|JTxOflaS4U5i?O@zlkN}*n~J%eVv$iJs+o7Tcf!&;Z|*RMM3>gr#u^xPZ<Ms9UJuJ~;D46yz6m*>Zl5>rkzkQ3+yffp"
"Ggd}k$Iq3&zQUu;_B5{bJQU28?O6(}H)l3t`r(0Fug73Um@ZUKvSme7o)2=~)J|uqK_P9*j+C_ZGVR#Br6jA82HA~{57%v9;5Y@5xUS}?6B0}`*vUh_NMa?8"
"^h+kjqrPR!45pb@Yr`lm^95JqQ{70bb+gwAsF2>njod6HQ3g3IuQJ<?%Zi4I6DX=vw?tmn$cSWnDSI#%r7kO^fp+2yClj76H<0$^E!vU+ut-^F5la@D#Nle-"
"YyN1ax6(&tOHp;ckxKVh4p`uqJs?HuHS)O|!Orx@nfmZ}ac{dEx%;-7O4`<c`Y@SVy;&UztH9Hz@0r|0l3fR{@=LZj{=~pP8^yh%`4$~D)?@#z>&aFOM^j7*"
"h=t43z;A0nIxRBf)^q&da);#UG4^|4{R3+4lU{z~@uvcGCTIUGKsfKUQ|n%E=1G)tBz{<${c(eTJ;F6^6cgB^?KYAh;Q}Z&iL3uNR}u5#UmmB5hPhxa>ZWD3"
"bX4W-ut5i+ha7eJA>S3=Z;Ki>Q{wpsjk(M3^)QaJnvm?WX%k~S)zmyS?JFZ!&NKyYw8w`kVex|2m-u}FR>J@xs|Y7(JZgd$QS&flCO_gWQne&YWv3AH^B@pD"
"G!3&WD7^G}E^&yNc))9#X$pqvKxV@P176@K2Tcyv0#<UQZ%J!x!}&Z7+;B~?q0oZ7$q0t3)B=_>7+8_ma+<>>oU-}T3B{EWjbKPoT*m&E5YjYf*qp|z4r4oY"
"8-Y<fsWuQ9=Lp3smsy|^M>$;(90Bs-W)U`swBhMSzGzomtB#RO9ZegO(8Son|HlkaoJv6{ST}~s<fP^@twFpZK$O5Rfn{?<aUMa)Wr9L;q|R{-rcT=emuDUO"
"?C&|!lpE8CPcaCgVkrT)wUx9*(VXd-KE;p9t|OX!I+Szt3kh9gCQ6zvBOuo%wM`C_o|q-TEKcZ?LuILLNihl6Ck$fay=BejbWRYoNeM2Jc+lW<uZ&Z)ly{5{"
"U4sdHr_r*zx>S>tvQ?EPE|VjDMVp*48H{Ww>L%l9k~G$B?}oxcK-V=y+F~t_hBS`%t5b^?EWf}xhbGp}o47;+1I^HCrwK|oC*9?46PbDva1G~Aqw)LnTb=y("
"a~q|wZ{90o$43%p@&_DV4FtAN{uuCNdr=Pr)BRD<X&6sBDV;dz^r`FiUM!xw?|jl)9siR}85f^2NJF5EU-_TFgY+KzKy_C6E6Qa*5KZl(wrV(f0eE{~KVtHN"
"-FmlaRO3PKd>Qy?Px-Ka;q8vSl?ar!o)Rvn33DpIi0E3NfkGGGB~!$%9t}kjH<9=1rRg5R8(egomDIHi1p*{eAi8DSEy#wYD2nKgTB@>oTb)C5l+C@KO%I^?"
"-aAifx$928b=Xkd<}&)@Il|?R52<q%j-Qklu_`!P1XX&A+h7@S8e9Xbp^$wraKwix#nmXj<T|B<v(0?pS2ip4+8MXDwH?*!!Dd;$kiXR7hRZ>xnr}_nx)dYc"
"o-gGA7%b=C<lHlrxYg#UF;cz^Es)U`{09pj{A~tdq)H&Mg|3J%G5qbG<hfcywqTRUTdv&_v3L5fYTHh?f?k&#<8WUh&P5O!(Vq$y-ErMf{f|hejhd29ja$K_"
"J+7)Fegf|j-s8Js3W?wO(-Dpu$IW@_(qO=%+9-@txbq3lJNguc83!cQb41x;KpJ(xG%M4)B-dF#$-cyj^?|$ZNc|1pbE=>uy--f(6%I7&JFUS~TTaQ_6`iSN"
"mEDYPxKWnQpOw`u;?*N3iG*`Yn<*(6kCnXNV3mJf-#l!sci=O2(nAF2AtVp|(mFnSk^Ijt!rkRJGf&;+E3Kq+yl~T*)_0SZj$jhHncOn7@<#trG|~KVfxgF*"
"hqO>Lj_QW6rFP=MOiJkbv12<me|q?UEaI^GO58k2LljWIx>CC=VN&cU$ya!!HN<O(x!IMg78qv$?Aa^u;?>~MIvR^zCYCH{WAqS1=<g77%!A(MbZ7NaM0;1>"
"v{??^yHQu!tL}lOp$}hrKdQQ^-{tPS`8Ruf8T|*SGw9OI+4oaHQK=yS_D_$_10P3k-6gnh7nn_qj_?15FTc8<9W47<xF<W;R#kWUif-Pn-Cv!L-_z-+RGGS9"
"<mW2sRRxl~PoFgYKGfIXiWqUQJ;YI^+`}Z6_X_Lg+kad4tnUfil|oA|mVp^uqgVS=94y_y?Ohv(pm=e{13KO2Mm#Nc2v;y`Xsy6&&8!G*^IJy>yn~DJqy%bW"
"o$vKL?3vx-Hjfz|5O}r@4L*d@49K=3K33l#B^2~ZIdKfww`4rq@hz;7uj@mZ*-~sAQv;Y-a3x1qD_3dp*hF&j2ygZ&pF!JhdCZEZyjv^kWr+})t;<nPIedXo"
"{S7nf{6Lt-?MUcta2~FSm@Q%rodUH=z6P5HfobI*wb%oYF4gh`;ZG-jX+>cC^Q;(9ui2h#)+j<lotOyYY`RNRy?T8@yIZm-<;@ct2}P!Y_lUS*8DS#YYWY4|"
"6ogb}O$;?m5zzlf@gSy3){}*aqzO9hXjPFHIg$5SgP?7xi+YJAUPRC8vRy!Al<3hVO3}$BE8xHKd(O&KsxuQH>hXFGxs|luF04nUVsdU3r1pEQRzs$m{S`zz"
"7Ae-)=5Fs4uN3!=P`BGpQ#%~;(AerJ)Q3^2Mb96|>o>yS%VIsFTWCl~j~w4U?R#9I753=c)}I#(9KYRnC<A<{1?zbB)KdX(<OD7|?_M*enOipJkYtQ%(O1&#"
"T64F%^_2qCZAmBcaeEf4bi1!2Q9P3R^e17P!B=B@+pUshUvn<)63Us)EwJfO>wks+z$7=s22rJ`QCKT+s4t+PMZQ$aRqJ1H_xkjS*^Y)EFb{qiaSkgqAPX?f"
"#ujiqLx`Mj^B;FA7{9?+6f7+rDyA7s8In9+OF{0>n?JZ)8S^_umPZ5qaI3qxy9-u#YpuxR>G=CZC|BPz7hH4rNkuYyVdkFj)O>^2*Y~FlelZO4m|q-Og<2Lg"
"H>CT4**ZM`L(+44ml=U2WA)jCL`57BYwcelxk#*d(@nv6`1-!gNLcbU0bZ~>b~p9rMq@K*hu<o0s}*6GnNX9DmTf6q6eRg-HR0oJ`zLLb3B@fS6#PKCN!Al-"
"sZgy{|I-9@4(9OaN~#7WbdtF)>hf&v1AUWkt)UH*{A9k$G=ZXYf^$%{V~byA(d!?+E%Q@H`mVx8D^Kd{HSPNL8t81^$*|kIBw-gOjj9mokH2=4&qJ5!%W|Hk"
"If(PTDUhkgI3iN>c$iTG72+L(<_#N7Rwrc2_Qb8p6L#AOtAZj!E^N|#saFPqyI|e`^tKOf*&wDH910H62~totiui^@vS9&-X{7DKMNzCaEIu)zRfP(fo@~%+"
"52G=X7Ca^GC{5a_ui-+Yr&i(Y{DeINzT<*c)0QGpizPL`8JNN3>QpQ#${S=M8;EDJWSz9#9MrBhUqnBTzV)RmMWHvQ;9+e8=?9DfBznjv^td3#mae6L0!X_s"
")T-ZK&-ewSg46Y3SH<W;f?9uIUvN}+SwD`pTclKL$ekg-@o!pp$`iCwE04sX4BhF#r5dfKuSwn`ddHoZNKqM#u6TXH6B2STzc`+{p<7C0VK674^mV19UQO5j"
"nvIg3^UYy-bIq))%SFrEQIFQV5sO*3s25z}dOsxyD#2)Sb%jbmrufa97=*oGs%2zD$`+{xT#Ymctfp#8?a{BeP-sp3&rTf}fh|mKt?Bmdm{W=_`Qeq=Hkt2R"
"MA1*UDmuaQ?Hk_V_J!u&h~E#wRg%ctIx7KOjjZ$wAN#}0BjSFgc4S<3*@44SvN+Vr#U%?N2$GpXIi`U$;gh?MM@=^jo;?gnU^uD}AL$IQ$#BgMzR^Co0aOXj"
"YKTJDFhRMZcT#~|HEDt~cLaD#(`C^9o=#Z!TZuPpN)%SEN3=dgP<9u?m=P~lt#5PooL;Rk0>mxRS}3kbINW@j$*`s<cY6JN<GZ|f^{twp6lu(kLOl3&?HogU"
"9cT^8%K}wq?BW;3&`!2X6B@jUC)zBbWwE#te4gD|)5Wf$d}(UQZ^%0ZeMP*(ENhSOj3}v?EA(}6v!o~7df25{{v%V?L{0-#$C7wj(+t5D6<S4F9;)i5gSU8n"
"88-#e>S!rb<_+D|G#(-ZU2O0mtr4;?85R<Q&^n;(%Cy^3ZLg}g1k)0<iIGPpy}UgY$dciHo;WSBGBYYz=&GreM1)66pJ$dRNrl5&ngU39JXAf}G<Q`_I{7@)"
"Sd&3a?MM?*t8okw<e8>U&=Mm$ylxYijz3EbYxy1fHMY1CRbIN@n<2s-&#(NKd(g@%VXY>T|5JOMk(WVe1pN4Z*BW}_hi4p~6~5<VGvVOxHknp^ttrhoM@Kqn"
"Ho0z<?`Vs+*3-Q7*;6iF+rnv2P(PHwDIs^9LaCQ$TZK%jspX7W0C|sJ+f!%guO}^IuRgA?J&J9S(cp^JAQnpnZP)F_WSlQB@9$paXWgLt*`QLs?f$rkxtB8-"
"*Qv`@p%4={^me4w)pV)9w5omYwyb-_wy+K^EpBQ4No#t`GpnNX_LLBe1e@3*wDZR>J(h34%JS8+9Nwe}@K!jsd99Mh_(r<%e9in8*->SV_IBSHe^$g#%3N*x"
"dJJS{%^}RC-w3U~TWWOeW;x##Z{iO%+s(F)b=o~m7piA35K>XqkO6>)L%UV)*|1dKb|tJpICIUuEX#}AvjPJMyYK2(cX+K0B74<6!{O1+V5BS{?d?1jk0X0n"
"eFk|mS)fAvPFlRqC#}6W+c8dG^NF4rX`NvHQu?>1XD~U(fMus0A0EWEcOx$*j;M<EO+lY3#NVnPNr};DOTSiloLRkztR{@`A7KkXBAyyTUOp0S#2mHqm20?{"
"kn(G{?hih|^NVg|V|P$oSvzx8lmG6M&aVh_aX@9WFnUC3m*cV7{cLx!o|pF_VWI0Dudj*sVxAyK1oT7eN+I@xH%d~@X9y@<|I$45itDv1`d9A0U)Q<oMB{h="
"Uit;elMVQE{qtqUGo#e)i_vy{4->^->|cp<jOHGhQpv5YMk_fqS*rdA`8D{xSWZMwRA35y0o~d&dRy0i;t8<-qWNiKlkntG^$tu`W><DZqPw#gJtXt~HTPfp"
"1?XT+tNwI2jWv&qdRx!Fz_gTm{@O|nL{^O%=szy#zT%#QmDj}KK)pBRp)Cmfw!T)z(z>kUQM>4i%Rs$XgLwD=zfalsGu^OIp|?BLLOb{QDh5-d&uMxIs>$e8"
"#M@(01bvOt;m!x^pIVt#bJ=FcCg-Oc<IcJ87Hfl>WBC4OeHZp2&gr|obMK|sQst*^9J(~(lRgLCjWR2p38gmPOMS7^o%6nB==>qJakT4n;MMx&QP*#(2j{}z"
"EGbnd;xn*y`B~xH-9ltlvx)^AT_brDgwr?Dy_|^++s&X}-MK2lDYEv1`eTSDPR{qPKP@W$iLv##zq?bZ6};GXdpD2uygi=f2kYhpvM+oVr-#3gGvT(2EiFI)"
"KMb_U_NUjond`Og%u|wDvu@P$cm8p4qhM?de%A>K_Xgr12DtS?kihze{lmwb@0-0L4S?&F)WII<BVSlKX&b*d{D5fBZxKI5nLiivw@)(Z`0rO`)4q5!+U%Zv"
"*uUqV-aMa}UHAUyYWF*!Ganh$=_enql9|Q*@D-xRzuZ|}g6eM_u5UbHO3i8E5|A4jUbHy+;-|Ve(vIPJcBcFDo&DXk^(04!&*v6rve`)7`u?hMrseC>y=a7@"
"Trs;K2@x$5xfp9~MPk%XmlKKfgL096Y|W?Y*LeHM)AvKv{pKw=GD3$1SDoi}J7*mxNxE;Ir;L8(TZ8B}y}dk4zPeGk%iSX$I#a=Ygyq!tVhQ+6tGfCbhFDod"
"CYT`e&t%rxYn5ID77mjyq+>pP32jEmU$h2$8+z;3I}?}s){uGs`-Q#M%}g%j<3^9WR{(b%WB%>hH>sQUe{Z&WYe$zxJO4*ZQ0ol;O;KpKig$*Rj8^`QlR((s"
"$<#>)`bOjQrd&R4Qm7m`H*yE}XKtqdT4ob?tNr+eA_D!Q`froqzCpfn`L%6%q_FYRqB(oItPcL@rHYAWN?zgHWqLxKb6(Apv2XH~1Z!>=m3hJ{d(~(>^A=qs"
"jH%pE)=%#0mFp+fV~g^ZC$p;oH|o?wF-T!iZQJ*BjY_FFC;5M-v@1{5RkZRa6L0Db=w<z30E_t(KZ1Z+M;<-$if4vFLOQM;LK;{*ZTkF-DrIbAASq3)ZI;{3"
"$AblrCA?<<P5${o>^<TMslqC;=DX%dXRCbbxm#yAJ!$J6ki1(6H&;Grcm0MyR8+FATHr8`Gej5;cnRL&H(pXNp|$6eUPtW2m#$NGzTFTB9oZy({K-I@f(0xG"
"<9N}RQNtKxY^{MkxCJ+ge51^?NgO+CqfWb}a1vaT7j6l9HM7KPNd<lWwo9LOv!3U_#gRk3YvUTQ9SHpq5A-Q_e_%+Ms~GyQ&d0OhhmC&YCZ7vt72ddC9qWHh"
"idu`bD8ZL>V*qq~?1upRGqs+@wW(tsrYl*~F9P-DJR;mOL#MVe-3Ve0j{mmCJ|vnrLt?ECcYqTLWbY>t81Q4PbFe?$)Jk%8EFxR9(pGZ5oLMbWduU|4e@?!z"
"1;hPx5%u%f0WETt(W1ke(^j!DG$54KtL(ml`UO}+g*0*T?*)!Y7(rm0Lu{`}bjPB<G14k*Ssfz+N<7mwKXAZQK)+gY`M1Hpu1ijP5R<xU3$ZbQV6zH^&sXrm"
"j>O~o;tG1qQr{qsSM)&PoxMS>ZelE`if%{uDR<A%laMM7M9)8!l`wa#w>)7Nqa~Ve0uEKnen89%_x(74n0x~$tde&7pFHI1n3{?o=*F&2ikaq6IZum*K;5Rq"
"gN|*Am@jw(y|KE%)C}*aMSt9GZ7QV60b$@BcLWmkr->K>v_xsgd8ysT3hCqFndmUSa$R^28AmtIuIZ^YcJGI8#sM#T13ZHDFJJ$t&q_z=7B;i-{;$N8Wmdia"
"olYMG*zp~9`pQq5TMNIf4gLEE*TN06JA6E^oa5Y+NawY+!Jt=Qm9rV4@s$B~_FMQ+>HcUr`KPz>wm4Y-JH44o?R&~W=9A2G1*rIs#G@O7XO|w#96h+&{b+_~"
"pxf}H!4KYFz1v0BYMX+$b@N_l*=*f9yLpZ()$a4gROTl47+Dh4F>V6@l_~2<uS9t@DhS1vF$X$Ob3Ow@R-7_4r?twhL3b&kSc1ZcveBobEVL9~;@jIrT!G<y"
"m~Z#mULbkPW}Y;ebc!|U@Onc`6Ga}zaxtDV@Y0peluQ?_W-i@Txc;DSVCVZQT}+oYASCql(Q6~PR5dI1_bP!QF?El1V>ed}70dO(YwqPHtq$6NwotJsnf<f;"
"GADgWFTu>jh@)7=$}AJrt-`FglH+peqM*~S*C_DQ4{d!F2&FYBl(*3TG1p)MyO!Oe2uM(yOTswMvr5*`=h&(nS$tfpqztEMAf?1yev`T-rv_FOaYdOB@%)J7"
"vm&@|k*|LZBMlU<WB-@6Ej27{qge7KgfG_*P=5%SlYS)eMnTz<*kYJk%r6ShnVIz_xk5w&Yu1y*B4Uj;r+)A8hbpqE2#!XO1MT+S+PxyXb`Z1|xo>cwalvh9"
"*GE#Su|DaRUDa^v&c<}{-^t4*z|ck}!=30$?IpLMgj<DWHWsG34S@2okgI`zKwmr*<PXjy@dx%7o40Vcpwaow7Y9SGvGP|WURqgcHWB!(t)kq>mLl`)`a+?("
"A-mMtc)VIW)U`+sh=0F*zU4lxY*@u@wp8Z!moo8v=rgfwFEoSPb5OVVox2@BS*uJ%h;^lg$HbZV7Fa>Y_vcDfr4!zwklnnlm-XH&&@Jr6n*<~#ETfuDpF-`?"
"d9|Gs@VsO8vDn(MT0JyywCwEn55{3a5T$ZfVUx8{FlA=8cDMB0#r?t6gUWa-gRV$CYMATH#ja)Lg7!f|dV778vl7Mk)JU0CA8xs_&a{s5f%CT3hw0h1oBYwe"
"bxAk3o_g3<qh8^xC<^Z3?Q@ch2fS%z*tB0gdD&6=>nfXvv;>T`0Fcg!o@?v9z1WuUO0A<1$(O(PsIS#q>U#YbpWYoaH~Q`8cJkd&tH;Xmm7e$8EmXrWcCY`j"
"+NbQ^aH@AROL3f~ZPxm|M{mLHG~Wj&o;n&^p_dOzW#sM3jvS$uw%xlpJ5k7&#Zy?1-{zY=R2A1VwUrr)hP&w+Q{R+rmzf*%1fV{U(}Q(kRR8FGhJQUXy6p7p"
"IjtzKgq6GlGPKgqAI1P#J7<v9X@tO=qSbjL%I#>=AroyC_Izq#Ab{Ch4}0yvW?|beE+U{z+&Y*dt6K58nvH$KU$?|%H|4jsWE5>OWvuOU+`D}qMnBQkRKmeH"
"j%r@+%n+SLjK4DooP(Qqb-)PTa~3Zj*RmaWzv9}0x}er$Po5>x&OzybVujtW;z8dg132G)!ItFDg8935JUcICSH9x3ij~e=1s1iNKdWKL&TM^=sJ;)9bFFH+"
"{4goURN&{fogwiZ%p)|K=nzc0lz*Rw)Ea=7C7;gE(d@9sa;bHTy(U)`dXtq%L~2P+L`-i<&7WtdGL1a)NNBC4M|-Cx@gPIa&sE)_&6dx2wbmw`bP~7Vw-g8x"
"eTce$YtCypX0#oHTI349`Rm%owQtnO)H`J>UIrT86QbHoSWGQVjG5UJ43&`VwI~y-Ufz6!WH#v_ey8tM*)647!nELS{*bMn6wOG`0haNPsvsx5>~!7%!ukNq"
"hA`MTYDwTuzHjaL#CtG)xhtgJ`Yb3nLsvSAb_E@x;0)#z2e<hD`epI`3L(<pDbR;Gc)lTC!_4R((vfCM31dW85|{Q$B-7p&+O&$N8}96eoBrd*WaNno^YKBg"
"H@3uBx~<50ym?otl6%pF-U&c`>QP#>UqO1rO5d}P4%?iHZZ}Pqq=}(qdY%~ei`{q9uc5^ks%h1bBk+9ec2ul9bb4p+`cnJ&yD|vQd7&&n89DGm%Q-k;%Bsdj"
"X@GGT8xQO=Z7@S_2H<_B;l6@~9#bsI4ji1ti8siL_^b;L9Y#4Qc&daUW`oh8>s&x4a?!Hl?R47tvxkz=HiDy9Q$*OS^m+4n@_b>(tCixzWha8O-s4DONz{Fa"
")(y!c+X3ebdeCg+tSg!=i4Tgr;Ugp#lr7|4z~lW*K_tvxEs1EKFP4|zv06=i2G$BxfGA3<RZ;tlQtp@|`)xF)ARdu61abMlA}BhzAC@ptmkd}^eQyINb1uh7"
"j^dRWr!lv#=|1AQhHhfEC)M0eLAyiteKJ?jaZ!1hArW+2QzjZOw|u+pl|{HSrCW?9$U;lggLav-nC(hCTUrgim)VkXRg5r5LJ`-FHm16Fm^1fJGLM);FH*I~"
"cei#P+ea{H^nicZ+7&9oi$6+(ujxAP9}3a>A<D=nN&mB5NaR48yl7M+)$9NK!L@OwF+g#ch=P%u3FLCxURas%#lW>MK|4lt{bOe{Qye7!5(S~(X!o}(Exv$W"
"-RPx_%S5tM5y!<}OMCW;x7*!t%B=hTX*Cu5;;p`cr4jL87M<O4{03tAD4Q_I+tss`mnq~{qRv-(oZO_4+5>=LY|<%oI{^`>FIv6Hm8`Y%R9m|#j_WQ&^jE6k"
"XsI1ZMAXrrBzL8ey+d%Fy@stfSfNIV<w0_KhcA0nh=i32?p&gHYx~{!HW(GONf9L_*f6GL&_g*Sq(rurjG4GTsDo1gYmdmT7d8s3W0K3y_7Sam>w&BHM4~K="
"?iDciMQyc-FD9uHJg^_*;T<*$j>3^r;k+oN=!IO11>LKpj4C()>H&qXBdK~HGwv!>d~e-}cSX`D5(MBLE9q{3Wp1fG+8Fi=prYRx-ARghbviVOacLXB$BSrE"
"Lzue5=o%oUW$p%*5wY&A=E&M%Zg<5zUT(Sq!)LbT@Ls;YI=u|iAs2?d5|Eqdx@Dxc$ylog`8fomX|Z4|2wjK_JJouJn>tv+gwpIHdvZVH5p1Wl1uB_%#%sYg"
"bFCb+qqJ!9^K(kTdnPo;?cfznEz?;o09%KuxJfl$B|BWvS8w=6wj#hSj_dRT?O(~^Jc1Q4KCf9z%F|v|)peFHW)y8*iHi(i;%$M+D!W~HP#!@YLV|vv!~KTD"
"xY=S*kk%P=sv&D5RElgjB%Bq^zSbOwLR`0?djqyXqaC8xc`>!RNPM4+ktXa^$-K3drve3L9qkzbo+0sa^->n|SM7s!{4eY6PA$J}&_J9U^n0oiQ1+FjDR);Y"
"<DUNp+FWZKb(larSZC8jpH5{Tt;pJ8nZ8W3v))3CF09sy2&^!G(qqCIB?h>a%{o%nx(eg{h?g2-;D8lKV~}cK-^uJ9edMfr%6>yr9K1ICs|gOO2NO}5Y2}b%"
"V>hA$!iRPN`{XDh%<13CdTVbFn=7dt`^H;%kvQkW$}1T^P|3Ew4C1>;&>YI~Eo8*v2hbnd*4>)J_gF#n*2j;rRhIfng3(*JAoix1@e8!t+3e#YBr2V3X*Z=C"
"q)FGIRyV2Ek!8Pejj+XcEop=mpq4SV;XMPz+GVC|Q(ar@#7<nbiDvFFaCe%M{>XMDN8Sz&dWHCr%2e_~hc~6~NKB)<YB*W{ZK_S)LATs@SLsc!WDWRf;zwH;"
"Yh4n6;*C+WK^23AdaX8YWXK(>@fY`t<ePlbd;7y*;>Vqlraf&$A0Vj@0sVIcj{1nK65j<t`7NSbkJ5s)X+1I!TltSecV;L5lpnSp3U2{xbQM|D+*+GSLKkr{"
"cla^(zzQyGg^k7(eZhE?X2}H1D`L}vA6grObJwmnvh^ON-uqD6GK>r&UwdO!zprpNtDESx&pyFDYbzSW%<|&Wl|++1t6~NB6-MAiKnY%qYksRvpdhYWn$hxM"
"Kpj|Q1#?J4qNJuHBDMN{#Uu?EDVP<@?WW;so{&NTtfHdO=)4<>WfKD<`eghz+v{x$<9Qy|q^&YPu7@QRjbEdKw!{t^=!lVq*s{gD2A;F^HpaK7=49HSIj_c)"
"I<z?%PGrRoXf*BE012RmEVGmZ$(@waQY$->e~u??jW7+@Do$Bg%cGw29HX6Wwz*xW^Z7bPK%vKQ0_ecKi0qburqL(`lwJo37WNQZL=0zl?EAz)Rm<)28U+(s"
"y;`r~2_Z;wC_wmmG;9Rl6)TR<*J)bsYuzI<ErxDuL^U}<)d9V$PAZ(?N8n3}q`MwZhI4+SCdP<zO>{b@yx$0$Jt8ouvV6$}x~zCy@diBZmp52Zl5bM9jB*|P"
"0d=6*I@$IR?N)i0-2LiCj=&&JFPnF(K=LHDdgF+J)|HQUk&3I8cg5Q`5fi2AEh{exnd>ovKWEE<dUoPOunp7B9%Q2ZcHh}avtp#EavI-CAdQU@g>PM-e=s*C"
"f?)tuMgor=+6R|L4-XFV8gk!I{csldMD?Xn1<hCZ2r`D??sJhQFRl%#MElPf+Z|a}zr&aVr{-?xZASDu?N$xd%Y9W6`EIC+s-QdgB_6kT*$IYKAcE153x$r>"
"AR87<_A9JN^E#H&5^{*ErxHU;t`s_<bL8|4{hqG?x~5k|!whJN2BDwm=<OY`kW)iO7I04Wx{nnh-KuE}M{Vy3jGPM`a}D4W*0^1f9I2~QWj;@J9Jf%lMN+*t"
"79aWk6er`;xTX`bx5}o+3ua7Ynhr*A1^Gc9Pd3|ZzS7CO1FlOcqq6gbRX(VLPl_Cd^%MGWt$7jbSa|YX@=L(K{kiwkr!Y}(p1Q&lT~9~T%^!L9=Xc9E^MmrV"
"ZV?N+;vm+De;6j=AyLemGrc?P_Wt3%u0E@xV!@hXa!KHg(!G~07QXur^fodZqJ<<t(pX?~YPKi8sO4;e7r%RsTD<d`P!B&U1_!o5enShMU8_98S!>XAPwtIK"
"xBb=A?VI-Nup0F#)z}Doz~-0!<mfw%^Fx>Z#zBk8n8-`>$KShg`G-FYSL{1+fnnI{&cEdMA{}ymu#<h#NzTpwD4*%JUp0EZymGpCg^zv8%>3(^K}x->@b7@a"
"I=Az`)$@lxn&N`#KH%gf<!o%{pBRd~D{S5v9{Zp67qm{_-1{<gn}I3XtnKaHk~i7FxF!5WBo?TJQ}%=Q_TRhF6t#W+CLB5L8-xG#YK%kObao+lz4qzzJ8`Lc"
"8F)(mbMd<}(m)<;2Uk<p%{ilgvJq({GWC{GwF|j=)*D`r6t}a`_{@nhTJ+oXqhe=f{a0l(>?ZeO3?(3kKt~wx%%F0wcR}@YX1V{Hl`E@G>E3M?*Y7TPGk7B?"
"_m3j@v$4&DKXXbhtjqXAyX}|_1iwMNUfB#@9o`r1uf_Q813ftvWV(GEqEdTCh0OcOd`E$j)I&xX*Wn~68fq2V`erxwljpCC?f`CX!%d`FP@B?rlhqrkT3%q`"
"At!LSmS6<Gl$zfqA(1hOj9QFWWKs{K);!-W(}$Lqpk%3wbGE(iZ}+-3X>rY3Uac0b8wiq5n?adq;;NP^7iuLIcKFsw9=@OgvjUMi35H20rE9fyR5r-g6eFoh"
"mtLl+vbsHhfXzHe8}ALn4mUO(alKS3W%4arFuC5>+!#eoCV-Nc(@j~{LZzuyr-(3FHPlF+SuHbaC-ET+Rq(v`LbJLa6ig3TV+m>+Xt@^anN7HxP&?FFVk46x"
"A2#DyW8>ADe_PN}V=3KIT$~CE`SqMf6tkb=&`iS{in2i=8YFL4Yn9F_=MlE0Itjn?MqAx|)fqxqv2Km`poW75!`B)&^(B^s?#6+3%<<Q^0-Gre8{ppS$cB1i"
"9v1S+oW@^&O+U;GbatOJBLsGDO(;pS-M4tvwscR{>B&r?AWj<P98Qxo(={z6(0FO00;Fj&PAI7uXUU$VTV+P0IY%^AMgKh7K+#-X!+B96bjmh7zM{4%kMEeq"
"ls2(sqAByLVU<}6vEiERxSa{t$W~3$O-Z8}&y*zF)RrbVV@iQmw{*r3BuhgC$=5_l_bE+<(vl}>zRs%Fx|)#MWtBqtpzB-I<TYh8iqmDm+(Zlt#tfd5Y}qav"
"BFssEC6FCfFX@v$qe(|IbC_jns;Zf0y>7C+r*RYj5TxXa3Jb^^<sznTV~K_-vl70^9SJHgh%6yUa7zdjG^V>a)MeU!jg`!1o~C$Jtqm(Y?_;oSs&Tv;P^78i"
"gdwr4*x-4I5RJ(v!3{aQjq<vo+NJJPNXZQoGPngg74YClhe^X=?P$k1cD;`4##hG$6Zi_$pL_$?r8*o?8)$r6j>3N|9(Kb;)$sW?BHA49m-Tjqa0S>~$3ndw"
"SYL591wGr6(*qMU4A1fkkT(`|%2(-;y=tKC>R*{AU~JqYB-nx+oun%Ce$~)zH*{A!tS#IZef&i(sP6=j#kO<2CaZ=ch<bKPGp%sTHXgy#4P11V7$dnVE1%zq"
"1S5q~9pWIcz$=!po?|M9`>del_^?j9de-@+0oEy?4i>NqXArr3$%1mYR$`=@B6ka!136QaB#2|dJjGXuEu~_im7<@kxiBXN)2f^nh)wm%6^}JaC{6y>G+74)"
"a88?40F?ud6ANh(Wi(r>N>o*Mt0WdRUv#Gm7|tOnbiTREGBTU5LA%jE{cTEwMe;U<`G`P|i~8Cpf1&953}{Jq93+~kUy7opk5)P+%9PqJSzi}GP@51qc@$%W"
"3F3hoPu36L^^yJ}W389$W(*ENM)LdwPoYw9&oIcu+_<;Y)#KzkOBe<$>V9R<kjd0C>h;CFv6)cYh|M6}qO=2~{hYH_8B8hvKLzg{<Vbp73D$0CZ&yo66v@%d"
"kX#ODI2_;8Gi|3$b$4}nZ%|cm1@FC0Br=gm0O<fEy!W<%f-7&k+D-fMh8jANx|1k#lJ<1cp8mavV_fmyjL3-hz3=_r_g=)8$#w{Uq+r@)NST_567{luDRlW-"
"B<s+M!k9k7n#e@T(Rq(ssR^oWXA;9s9JH4Se$<1xP8NshEFQ@AHiCLHD5OM2uOpt2hoN@`jS13XK?xP#<8h|YC21eDX8Xf+RwrO{xywk7P}8WA2w07g%V;bR"
"_NGQ5*6qNG%y=bB4``e*J*txiBT0uIC1iO>1Huu%9Cg|;IZXhF*LiY$+4Bx>(~+U$SX2<DFsm0QO>#0X^#DQ%=`a|BHX9vC`JILj!fGt4ATX`<O~h!iT=q~o"
"^J%LPH;$?-0ks4Q2&|YIGe@0tSO1*wZI1GyQs&hl!5gDZPsNx}AqwpJ#Z^R8v^>wJbBgOgFlsbR+pSSIpwqtsPuKitFue@<d`t0HTy8&|(uQc9=cmCav#%&N"
"DA^fUB=aWlbD<ZPeIrq4&Tf_3angCxQ<g(S265?;i~e9#r}t}&Y2I^J^Eujgl6h&Rq6^2}d1?P;z0al{@h;{qJWrk#a$3pI_dRltPVXZ4Dl(q8A$oP(5+G$B"
"g~E8>ZG8a{+dMJ6Aou4_VyfBYcB{xVKs#+ISB%z0QMy6^v{mS`OP$C?$jNUx7YSdVhfu}rVN>QHCMpsY!U;L7_*$uhIu@c;1)j83Rw2D$(@FY;fzWt_NG8aj"
"?_@i7>5YZA5UYtW^VPPEMzu=|fpDwBP{9o{;BZ^I=U(=U?NrH`azj(Zr7TCgW;e51X#ntV8=bX9Z9oa}33*Ru6Kdf%rt}*W=Nej8S`lBLmDZ_SGzG|spj%@`"
"4Ol0%7@nOA0Q===0xuAhI<0bHGW-qNcT-^3yev|BvS(L}X9v!z?2#CF26hX18p8V)bP;80f)RI&w|NRAlFS7`EAI)6kQq<(s_BRMpMoXArw*4>fFWB@NH^kf"
"Q?83eXOy{X%BJ*8{Xge{5A-GCIudxF+j&bS5~Y%66NZ5-64vt7I-5dDJ0Lirom6@hY7m7<VYA@01#~_PT_@<A$1ZSEa?%{iaojK;O@L0)Vzr`#V1{%<mbf;C"
"1uRZ!Wg51URNgY=%W6;R7lj9{GKLmFZKPdq@oF7-&>r#!%|3rf?>5mepC5L?SX{1kqh=cQih>qYr45>c;cG%4byB<Hx&^%luc8W@(?a>l={TlvV$?~Vg%C#!"
"7b~Z*@t|C6rF{zvI@;hhyOT&#uFlB$Alp4Y5P}hlLvS9CB)wQZfawEPaVAQXjbbIO*UOD{gH)S|O#{o62Mu`nw71~22MrF1EJZpvf?QFQusJ13jTw@EFLOY$"
"S#_@faz}E8q-5TGjRdq_)q*+YHS=rGm6J51L;%J+5eK@HBoJ>BQy8eI$sS^w7B^gGdjiP~$#QAJwA#1Hju~^y>1AaUcd0#Y$SPBNly<AxqO;g7R-+AP-~iH~"
"9W<nnV<KC?#j)Mi>ZDGG)d3hF;N&#X$lY$GNo90;K*H{n17Rx&p*RE~1{08lT`=NBT?VJiO1Rx3Bh53$jXFpNjtg8Qu6J4af1So{u)<+e*eSOQR5`<x&frE|"
"4mgJ@+>paZT7q`9R_1txaH9(OqS51bf*?w&T`+Ea^WWG$nakDZ@=%c3;J=-rKEDDLA%@{53@5xkbt0sLEd=}BpQ1v>QlE`k*DdqpiuOnL>~a&@tn$Iy?BNZW"
"q8$@KE!aD+tND*}dAlqym|dL^+N}v|dZ@tXCG9#*Ec0>xl8@HK82u{P7}y@Ool4%kpo0{ldnMEQm^A6tURb&9@a8YNJl+l;oUX3qZ)=U(m>5l(t1<1xKZTMb"
"mOLd8ZW61$fN8pSrrVGv@6yC&PmHSeJ7N(X8mX#$UK1L#I2rvOD+rh`n7v~4yk*)z2x80$FnLvEj0<3v-4YscsGYRRxk{Ft^4o5W!KOfPH-_&!!VPm)^FJD#"
"EJuuR6cNsxSgt7qgqzN{MO^a^>>blCI5e6sNMKyY4-@)6$Chm&ByCF!FKUFTLAeZ$f&?~qAO)lod7=K|e0-6zHygxy8s&3Xt0uo9NkKMbiB%XjIm8+ev=WBQ"
"aAZjw@ux2-mFd?xZ;u;(0<*}zw^U}gF5raUNsPdnW+N+9jrOJj!npjse;P9wMwR{dIO#=jHFr})&dk_!eilT;n64w@_T$t$_N%apc$XZWijQA1TS-4<iUdx@"
"-ADqfy1g#yKl7pAp6kcAcjl#|v9IUs-eK$=@T!jtzyd>+gv5)w_;(TpH@Z}1s&X3pQY20#k)f%hnaq+bVkTkTjY2ZOvG&9-eZqsUyb*L?e^V0@pw5kTjBuh<"
"z#U)*D8CHDotYR+JEe0Is4wX0<HU@_h&0A=CFG9&7#OAb?ipK_X#jlAG(p<@F&xL4M8K%V6;d2RmAJwg`|m<XQV_4Zkw=QE%}dHr+yNs&qO6ZOGdM!pFye<&"
"gchBN+M!vzg1F)g4&VYwr-BeU8$O#4jv75a+V2ZQjTjU=35F2uin5iky7^xMpfky_VQHX>XC-)mzRqy7bVEOw8n9!I0Qb5=vl>B=v<k(`+DTvv_+TVrp#y<g"
">o{5r<lu9S!i_BM*|GlAWc&^{N3Pv8vS4*6h<5yxE@2Dd&e@;@KM)aGNjSf+T1t97TdWniMKeREmy-BWcx0jE5EH3gQB=xwC*y;$pm_#Bh^PXI+!uaA@Z{6d"
"t46akXkU&{QM1%Q=v1)5$%^2R5$Ri{hvW!hDCnmXGFgsm^(}=V)TPU+6VfxJR6CuvIgFmb5^VNoUrfY?v?rNNsmZiQ=1zJf`|g?su(9=Rg;|ldm4Xva+7glw"
"I+stNY}7&`e=x|41?!*@<Clvx$idT3Eb=gJwMW^D(wr4^D|yJa{J-AJAqE}J4X>j&2z69M^ev04^bmgq3>XllXC`9>BFQuJJabrQpp5x@`g05k9N~}nBDGLa"
"&X$Kl!O+SJ8??(by=a$fhG-B^Qci;=lD5!jc-8<@Y(*7%5(t}2diA^`=cW6mI5z8wLIG$C68;}M1Q`?8A%ZINMls-z1wHN*E+IWmDr%+u4kjiH!!A+^T^k>("
";lsGhw%4(cCJz)K)Tly7mOODGEs#Q#jcwo%Ft&{OKsf-)AXH_vc_|C%bgI%8UocK(^Kjmesf{K@*o2oc4I}|w%OK)JrZG9@SNbudDNKTqu#>!C_gg}Bqgdrv"
"y7Qn_GZ9T&vnIdCA%)+*76+VeD=K$8`%qV?V${ccRyge#YB~|F4aZJgBZIR}ir<WH%0UK&ByJuj8X%$uojf$+93fmd=^?<e2~#_^guy)ZFc@aA(Xh!~7jTWC"
"KipoRoj%lIFbQ!2*Ohb(IuZ#G?^)&eZ9MMP#MGLxkX(a;UZ=+HVX(xY9+KEk24HX$*T{URy5uZ{rb0iee9IzangrdCx1iJN-FrOq{^a%@=;p$uQ`a^}g7<k_"
"!TYZNOK>$<o#5tG-x{piKW{(UTD<y~Q#+fhk4+yulUuIj_odqoaesIA%p2-|GTcthZ5;{X-CRN3+r^G&e|LO;adO4k{Vzi=o9+L+<@Ks~>aN9XyC<s@IV=4P"
"6U6_W@{OaaUh<LrSEqOXTDJPky5D<pp)-Mhyz=iWLl2Sucjf9mHxmDOt?$)+{^|(h!xz=?gn|G4d2Vqy_nJi~H(S^4x9u0Sy&ZvRhtb|eW>lw6k9B53d(uaN"
"9pZ1KGm!#YTr|BhQbZrF-Cbu5f6C}I(+|uOw{^VhnGJad7Ts(!u3ggp)^a<<-q=$`JGq!}bqC*KeZ95ApH@KiOS9*V;9s{QghrxS12xcrsF2f%#u!$&>>JaC"
"-4>)g)_i@T_l+fY*Sg-mcd5!iALxH0pFKv0&npdW3`{&N^u4>TEY33ST+qUDfX`q{@z+bQz}h7v(tOlK&uKRH_{ROY?jAb4JpcAYcbUE=yiaYxf5W>O$PEcI"
"vezby;KTZZ-qQ6iS-o!V0dW08h3=))Ca?i1Z*~{XU(o*6bQoCd9LSU1Oj6O_!Z%sp>>p{T<aGV*+0$0|A2tJCi&)b#fY80kLP;pTv1|t9ztX<HTeHgUYJYvc"
"`*U&awtlnl@Jf9ayQ}-1aPAIr{j^cL1tZB%vVE_0l;x@A+wZC&1|Yk_5n}J=erOTPO~Bftp8b9O`oW5IXL{q7e+0X7X|l8IKN3B}_T7Iknh2L?R`SYk57&Ta"
"?MEHf#jhE=?aE_t;E6{1dSO?y>5-21W-q^|{VT&>YPokPO05@i;`MEOoB50VgT-kf)_Q&9Gywm7C*q|=nsKw)d3&W)((wA3YDD%|x_6IjpyIybm!~&>F&}$m"
"-e^9%)SE+}XnrRcy?>$~g?<}z=f14;y}c`2omjYY)@YGig=aZ@`bzqJM5hMKwS!gXka4Xmv2~Xl4fniFaCNE7r8X3+WY_VfY$BMRmnCg)EaqLe3Oh~y_<c^R"
"p5AjT+}3;g3teL!R!>(JhKG!wSUX`}qbCYi5>ZX1gSWY#^m=B7##uSPbkz!e+47N)Mmq%po;`IgBhgIr4T$b@%e8J9)b8lM9BzKBq_%Avh3)YI8{OA^DOlb{"
"=KJ-gRT7Ne%l2Jb)#aB&I|CM&3o3?0(#l)qAKDcPgSK}vzprZ!ZwV}Wlbg3a1GyV74Qx&X@A1C$+;#qoXfRzJ<d+qF*NV0;);`)<xbn-n?Ox>>@Xm)C1GjvO"
"+EWQ`ZVaD3t^No7esyv44nMVCNekC^vHgYL9o-TQ2^`%Y-F(gH_}8rhWmr+%u$r;Q%cUxVf01DWSN>M@#$Ht~eI)zMdpm#2@qJ`Hs(<u;d(8E@?4K3~AK^EC"
"s!<<eVCt*tcYnMus}0UQenAbdSOv^QX8i5UtAJ*~Vrt%N!xy!^Z3(co)IPAyJBH3rv|04N_z|&A{8~7d$}EY}*4IW#@WbM9n=|~`@_M^)j9z`9_WiWDZRw%P"
"$p-7<1>G+#n;CxRK$zT4#MSjpS8wr)tu4v8m~8xH@@>%e+jbZeh_r2+13Zvs3Rc-Qp3TAi%5-J7qR}2}etxa@GkNf?bt8Fvy)o;)r~IvW;TRb^FEehtp~SO7"
"--WimIL<o$vCbg?gzvH05pHb2X0q7~x%HxB(o$KKTWkDs73B*U6Rd)SURCc971tvzi%!-JF<_b*clHw9l5FNcR4v9kj=8GUayHSjmk5AYoS2z1-?h{-^75L7"
"&W9tKT;5X@9@m<3p%%*x^Tt5e!%`SCYAhu?2z9y1<bPH(B{oEJ+cI2F+YMcFd%Aq*FDzZ#YGi#bFGAbq2XbD=$s9FXORy`vAMcwenv+~%I|Vo-kcKH!1uiGw"
"wQ3|*arLMH4H%mn3b4a$Y#?*0<r{OWEPqeFiLJYTCLg9VB5BHSb|LFLPVF~2!;d-bTIR?;a7&NApXupasAi_IJaEDAg=I6yYV1hE?N~xuX}fxyk2kg)^D4Od"
"=2E}K`SnSQcF7G53k+?raw(Q{jAcP3pXe`j%0|te_LCd!&*k1DOQ(7;SYf&M3_lk!c3nf)^!lE|5xbM?`(Ra@o#XDmZH6^~qJKqP>5Cf0$)#L9UfPJBQi@4Y"
"NUumg_5sODSniIg6pQ0Wh0vi=RbC&DYHBRzPVMA5By^R*J6jteDM90JzZS&-+-TwYvu2CEV5@en_izbNwIJ;xdicOecJjp7Sn=NJp^PK`RVPE83AslMKI(WA"
"$QVDq!F3lTPZ+~3M9N_}?%^{>9Jg=Ww~1TQGxkFuAGbcp4Aq5`w~)N|0sfyvgwTVl;;%(R%Rc_R#q%G7l*8!#4}Z|-Px^RP3uf@bHoA;Ko@Lxugo8exuoxs0"
"HghOZ^MPTNHW9_>yvyPVIkK6A&x*SVtB4}xIEorbH-b25XCNv_x<Gpn#!eDaKI<TpUcX8mDhgCScc7M}agy*D>|QEggaX2v*n|eGC7nFsHkq|(64T^6luu4X"
"ahO)wGDIK%7imevs?U08ynrp4JbL#@RQ^5XGvOA!-8Jww0h<YP$mgSD^oq@l>nYgg%X=V7=<(-}lN1gVV?+k9MUrfvJz+GbqeL)|ny4^bNGJUkk01X%m&C0&"
"jH<DdgoOm0p|B+30c~Li5)Zm_B?wQ_|E?5AODq}f^3@U%yz3>@Ha}jH!nCa>H>0$*B3H(ofJyDlVTv3O!K6gu5BQO^%jb8NX+;FkCY>R?;u$m$CO2mHf8R#{"
"xYi6g3x5=|iUYDjA)ar=uP8-?I0hNh_q?Xsm_L04<kPX(-LijEr7d(6LFFoUdA;C|--acDto>lKo6-<rVqO>w262;}8OS><U>d2XhkB&JSk}R{jC=Rkh;Jsa"
"nd$7^Hz%{s<fB&3do$vhQv}K59ZN>{cvR|$$v>Q%?vN5L3p#R5?^{7))K|t0nJM!&l8%A*6H|35YahxI_lQ2BmqobE#g~GV*~^`=qrJYcQ4!#z>hVM)dQGb)"
"c?pX)bBr0Iqu#_Gl#53qxANYe7R@vj0ZUuG(yaQU2dI?FLR;-dNrA;^xyXw}Pj+p=TeiFGIXD}?w&vE_@&-#W<=Q%c@U>`>F%dZ$9V<E$58HX4IzTYhv~OqA"
"nlj!QSD<Ox;R>USi4}Z!$FsC0Dg>5%EzFi+Lp!cy1iGD`E2{;2Xvlj^eoKIfajzlyZkSY4f@vMH+7|*;L2<SeN;hKtYB{C~1LoL~#~2<Zlk2u*G@RV8`kH!K"
"V$~SY<azvd-k;cZ%c(4|-Ka(NSTr;@6Aq9hXczlQI|!%Ye0*q)P+7AeT1#NPL#JykK`c%u_bxLsNaD_Fl2ZFIt}^1=tpf?=?J1Q#q2A{(s|ZiTg|}Sn?yx_?"
"3YI*U2m{#ggrm@Yl37wQ+s=^h2zx<Hh&(P!YAQ^cD4E#e!TS8LI%tlp(aCb?npWeIhi!(~CIuv>sZ`IJ2}ix#Il8WMq^dJvQ*l{d$&tyPOBGB5?P`(MkhFWQ"
"-;btA8zKtyK)W>wMgr4KkI_&tV7VyP?m$>Ql;jHHy<uj;8N1VndOoGe6(bdB{MsIq-C~H4m~@Y|lqEzOp{GT#HeFGgP5DtrXxZ6?6A{N=bS`IHX<|v=7WR^Y"
"l4DLF{IWLy`L#^Q8T&SAH+fC|x;K>ZbFF~KKtaYxg@nijGE#;j0UuC~c{89fnhE-CF_R{nbO(!wi3-|E`8;j);DmUT^5GsAqQQKS-4Q%_hmmueZ85}^pgola"
"?9N2|GHIyJ=h5y^I~^oeNL+3r@Ny0cn(Gn+8Z@uU#5r7RRG~FOR$PQJ5gsOxfWnq@(?O_8^Wru|4)$R+tk2+9xd|{NCol!6)x$)*&ucT#%vdfIE(WIb29G@8"
"FhqJJm|mvREo(9uw02{Glk$};^Zk}2q^{(!SjVOHCT)#+EvO+9n0&<R^<y?rMI`KY&~K0VCTn(`E(>__VWibFc`FeUcRAV{UrIQGyQPFjNn$Ht5O3ye3F8h!"
"=Zu(o!sW6<trR($7=KNb##X4L8;)?mHFq=wuZK7pqihup5gp?Dnv(|UG{@$Gq>_|LV7KFct7k@cbX$Hje|PU~S@K6q&6(}?6Om<p{@z|w7G)Ps{oEB;QqCQh"
"6pb_AsLIcdTGnl?@QA&;)l0EaVgi^SqBKCvV<uj0Tn>S3xoA_`Te`ycK5u*)m3-CRmTw%3ncw+x|K)3c#opfE@dVJa^x~BB_xpl=`rrS4eu?TW?fuC>Qt>YW"
"t>o_>1Cah%@(tb5;(>Z#F}bDsh&t{q9}3QG{(2kA7@mu;FDoA?qE|{9Q*G3HxN867T6S*T*!9>m$J?hA-j^5KGaH?U5(8klzg1O-m-FX-1^bs(jKh*`?aa@R"
";?obi)?JP0n18h0&NJQKMQ|DMX{oGROZ>Qgm9{Z8!Yz4sd5D9(3fxOcKHh3;w~lNJFMPRo{`xQ2>qpz(08x?;&bxoV&m9i@`#&(3@HTJj&#x!>|H9Fz|L&0;"
"B|b^Lrh33U&|Y22ZAzX7?sQnU6z_Dt*|sN*|24caqIjqXyqi-@)P~9jRiv+;8Q;`wdWr1u#(R?3Pq^KMZuf!AqU7%H)Fjd6(y3oTfhGRjaTZ>?^b0Wi@L|uk"
"r{UdW?r*Lo*&go%sN&&LAVuh~A62f<(4tP#lC>7Eb3H$R9>&$5?yjr1whXf`{p|RKE5BlP5BIQOzAPP^LI1y_rR((n{kxfYvd7%|_Hau4j~uP?*N?1D`Y)3|"
"QT)f^k#=}0yDoS_-&tKckiNe4&Hf4h--)uvr1uSpH!~W3ZIC=#L;4z_iJoT7jTVm9&#Tg}F<Xpw`&edCv5vNz(&$Rz%rERTTgE#qDAz9k%wBqU+|+OD1iPG_"
"^>&KwqM1hXm`~@5U}pDEYd2_^p_1&YYy1%*@rvm|M*gI|E!#S9EWY&l{*SKze0hDVjnPnE_TzE%KeoYP-#`6knL)OfH@|;9DgJl9R{fg?RvY$t=4IW#aSnB3"
"%;cK*kUs7%?Mtt1e%ZD~b#DY$hE&Je$UsK7RGETz8lG>}Q{y}OO#)Bd+jvWyd1<;cy}5p0Y|x1B?yX6q+`{R<bq5(@#_h78ed#yu;*$rPmIF2S9_w(glM*2Q"
"0h9b1E!U=92J(gSdo;vSG4H9`v;8aNi{_7`(r5b{rp+EU``j-MUpfDq#jTwkGKjB=&yAqJzs0{w|3CjSJ3;NT_P#Tak^UQ7E&qonX4HN+`-=RFncM3A+5C>+"
"N#Oqa*zJ{bn?K)xs`~H6SOfBp<k8m)LhkBK`Q1ADVm&$9LwX6UeAGE54Zg}*U+#7vOBKtyqpgNA%E|rNFCltaz_?owHb3}E7k_-*)$eP$N1VgWcASe*Gd4i#"
")q1k1j(n@!AA}fs{^#1}!m!x+iura*`gEtK+NeqAp8I0=-5bAIUO(8uLQqLEF^B#B2)rKn$G=%#z}DG&e|jS(_@4r${MQdGPWM6N75QgNJL-N$X_x(gI&RG$"
"%HM5%y#-~A-%YTFrB5WG%NZ4?IvqZ21^Ti{ZW$}*uw-X-L=(Fvst7Bk4J|aH?lf~6pE&WI4@sXwyVy>83)4r=^yX&8x@uPSl+8vdqI41Sh-S`hQitq-=X!C@"
"Yv-DEO;d%ttnplcwgQIzTA5vI+QpadH9uH<Ag(kk9?F!^&Ir+8Hzae^mml+4cu`tEyAUvbs?_VAZrR*J<@im@{~=p7E{YOO<0iRY;5T){l{-}=ZoL|mF6p+7"
"!L^uGm}SDPLhL7v*!U*ib;G$^TW?CEFV45-Py9z=P@8V=RV0Cx;!pl}caSL`xm!}No%^+=^z5hy?5kJyxmz1+**QEgY1b|WELhrab-&s;8vqvcjBQDGW`OVc"
"p6$b!`lDV)vVGgW{L(jv&-MRiY2$E<3ScG4O*ZiV?y_zK{^tLjoWVCadw+H<!T(=;rQ&zbtgh&9GOw$D#<``wxs*B--V5CAEbU7!w!hl9XN`T4r4i+0x&KsN"
"uc!}sZ`I)E3n@<3R&}HCjn<&rf0kEWsZ@6~2B)Ui$?HPG^x3DVSEv=Qr{_wePdwR!jhgu+4s~ViS~)33NfsoU#VqEK!%DuFnhb(`qp+x{@#dATQ@~D0f2&e9"
"uXZT@dmpynTX-gDwbwA8E3Ia%IR2)qnel$}1&>2C1Z(dw0`k977?dCFLoVA<^orq7*wamMV^!t0w^!x0bi>8_El1qg7mzII_H?0Zah)R1@OKJ;PX;GNVh+3J"
"GKng&;70hJ$|e2Hjhw!HVLFgt-QiUxw=0zA+>doSZu_(xeQUtnl_n#7-C;X=G5&o)xsF|2eQZYe>le00cbkz$Zap$*Ds^kcTd|w@`nY5?V!Wa)ABwSh`{q-5"
"QtpWw?3Mbyb%>)ssJz+lD0(d~bng`7b$33JonYl2kDfvHjw}13n*Afy#pq2FTb!G%w`FFb4=Bto#!AJ@%?MX{3+Jw~#<+S)0u)88rC1F!32F)7cKDsf1>NL&"
"-gWQHls~ulh`%|xTO&P#&+BM*?=>6z%4O@eBo*!3x&Xpw(%)TKZ$z)xKXIXr#_8Rwd$mZj&<hTmO6%3qgV;!^H7XknYp=>mw}e={b@dBLTH%Yiz-sfza%0JV"
"Tz~a4TBodk-o0B&*U@|=cXOfi(fky+e>Z+4Xuw~{&P0X{OM=2=qbJiY`K_g!3yE^+dMmOlyM-^W%@1>}4h5Lw-d$X(xb(bA!nYH7t$sx}uwDU=&(6@L*$;*7"
"@y!Z>p8tXNh~0fn;`r$$>$)Tz>)SjFg6GrEbMv*$A2y$XXtQ;0`_g_Z-p#dw)8^t<wQ@IdqtqB>-3aOiMCIE;a<hB&D|tdf#$mA9I5u5daqU;%x`dW+%Xc?#"
"7m}+;E}R{mt2`d<cO2YJ-Q})ApUB@14e6Hz#mPojrf1RC;@eA!YUy$-ViDc(FxQqxct%2K&kF=htn4xHO7XyU^sU+lnt_dicJthPAh+;<-y7X65uOV#=m5^f"
"%XY_42Y{X;8S2})1Vb01FLKsb-4|9r29f>7xxK4<jc6~w9-cClHyhQ5(d*^LsJcI@xu7lX^T<Z8|CuDMBw_$qLynC7%fMmv)r*djPyff8cXOGlJs-^vE|#Cn"
"UIlmWm-oeM;1`njf;Wv!VQ#w7RVz3?d#Qivg#WkJB0AYEFS9i`$o4u#<~(PKK~+6QUM&&YkG|PBuO8SaIcle;{Q0S8oQ;vA5`|rOUS(O@d`a*6;RlutX*Slk"
"cNRq6Pk)=c)(rKxz6M;ot)J}oA9a(x!cKThU*BA-K1rM_H*d^d3mGp;D#uK$w{hb$MMj3kjbPn%%htb49Iw877OB&wKiYUu%hWx^XnuOO@b&vc$ljykEkVce"
"3+d~@8`=e4VY;=UFfjdK<??*8Ryo~^T7-ApOC9wM79CSS8Oa}U<Q0!WSc&<!;-{;ZHDkTfxz5E&U!HNF*O=WYlGtlM)DLpnZ`z%&-LS4J(viNco3P`9*bg{s"
"^@UgKA76IgZ=Bt_yj72`<=WACeQ~{3xEmQOH72+te*K6(yDP$)oq<Q1xY8Ck+AEGj>mY}_SNX{mdzl9QqI+0QmW{b^Zfddg7&`0Nx>GzBt|6bQ&Ibn#9C2ZM"
"bxo`h`E13n#^UATwOZse>4Ayayfn)5n6z+2sh65k7@*9A!>fUj+^l)AoHVs3SyYU-sVK1<Dcm)1$!L^SuDe|Q^I%yU3B1ynbGXI==VjGQ<Ys1<a;;}Z>hq0a"
"pqMB}R)AD3o7oJ^rHYI4IlsBjm~M)&WOeRP7u69#3!MGBV`E6rR%T$D$asNMrHynn<4Oh+Oi^m*GSj)X;qNN*&O_~JXx1!I##b_BgH1ufsmYa4CN*0Mn6)jB"
"v?OK9X-oyiRdR_n>#`~`VX7V&&W)NEYbjN8l;e#GHf4F<Y6{0@E*f>x$`uWIX%ejH!u06|19pss&d5(n*v;&L#a+n_Hl~~TKsiyRSwO0qP4xm}C-{xCAw!=%"
"vnKacYm5U;RC|IS%6`+a&559P_S!U(B5d#0HdE2GCmu{J2~!6Pi%4@b)=^{~N9v*Qj71_(aPn2XN$iDEQ!C+QYO)jrw7syn#9Wg3T$&U7b|IYg0P;*Y&<I>F"
"Oq=JkX{bBKrJ}q|`6{!X!H^pljHIY~&4GL{4b`>Lz$?uO#5o#2B`xJb7jj1)w3!`l%&z5wl|(bj1Txipt`nF~7nb-Fep4Ti?#SG!%G{AAuE7F!IQ#QI;P-a3"
"m&cGS3A|I?NX4>_R3Ndy%kB>fkmgo?U7d%IRf8dhS*%E~3l)t{?zN`aE8%o{svK~sdm%{~SQM)USO-;-BB1z?0BRRi{yhba%=UNhR_peBVRt@A*>U^s(uR^}"
"^^sZlUP5nYl@<-_BxX<=PlF>7XjeFt)hD>Sj|7MGXWw+tyeIai6n%){sT*wA3(jelRp;qD-J81G?bJuOb{fp+vpbT#%*~k->t9k<8%2NL%JDvro@j`g*X2T&"
"_S%Fm+zG%P(<T#Ddy13zR(j9CJR1Qq_xJP8A}DW7Po^Gi!21G8RNKG2v+Cuzu;hpk?v!<*V^zpG|G~zc%?D65zc(H6Lb&CCv7=wHqTY;kE24oIC5Cn<fEoqX"
"H{qMXlQ4QdXDlpz_H+JG;+tReyd@%YMoc`y$kJ7olY~a~4AncqyX}6>*Y(U7q;bNQBXS3t?c()`VB0$?X>^OBKIXk!B8t=Eu3eL@;Of^VqJb^HbHlhk`G+4J"
"jQTcm`_R<8VH=KJETTnF)0&>iJ!(OF3*wmY3}?4VDX>6lgl}(EO!S^Ih%Q`hJ*7SYDw*BcIO)QGz4>()*U+Z2wrWyq;T9Q&EgUl`fp<*(A?qG-qo@k8zj(~t"
"YyRcWx@Zy2o)LMDF;C$dA12`;{etdH=zin6=Id7cQ`|HSrLp{>dMkT*D!N83s;bRWs84)wAfec;mBvk##06a#2?sZ5xMkQ_rmRG6g6w5?EsU!Xka7*=oE1>n"
"nVwBP=-KzDl>uOSey>hxaBFga2W^!!fdfszIdf&}N$r<#u5>USp-{JNd%i2@m=oTTwVhJiSSLfdMPp`>{tZii*m2+*DQaW$pMAu?<@oBi>v+L_g5UkviD$3Q"
"!UQy>SX7@XeAvBW{Aw-v1!f$x<otzw!$J0gsmv-hFIj7p$v)=YW3gwx$zB^&@IBhAQ@+qn0O}b!T*O4?$MEgcwtneU%<9Ij#L%)$+@0YipBBNrA$~+W$J}lb"
"d^{Ez;@H>grr@C>icVfw|7>^1QY`GxMe(Qy*jea;Cq0oUBITsUwpd_jHp8S=a-yFPqQ>2oxg00H{P-E~$oKWHw$Zvjd0K=$#sk?aJSPE8tEN<MC2qH_8lQF&"
"pS!K2Kprk0O1F#slctt;P~K{v;1?equVC{h`nfDyL0#7-X#Zgl>KS)fxXzOwA+|HS#<{DB6a8PxpjAM)KEh8u%7eG~f}m5x-Kmj6l1|JC;Z|8oALxSU?Dh6N"
"?6Ezc+nSG(u-CZF+<>{3ER{2T6W4;=%%XWCKpExcQ^-KbutN?PG$r=OPx#yQPrvMVidgu(=wyDeWdAZuLBpy!)v3(A#<1>jJ^q9=k2y2W%#Ls`bA6`1N-e3X"
"jRMstJldBM?1rc@D(6Ai=(In&;e*@e?lNSeGShfBvu<HthyY&frx|AfP_E7El6N0NTS|prqtIuf_-zcLWhQ4l0+Lz4>(Yo5rQc`ZP#~iwanyrkG)c3;;l~4T"
")(61)fX1FeJ)jw6B66=W1DbvIh^|~UWF56;!WF`OT&t(xb_FCxCyt6hHMoM7VghJ2O4nQtB@|ExvOq2<P=awxVhP8B_>exAv*If;V@w2CJn2CdL8U2Y+@8>R"
"0Czxvr|2xC5qJ^I-JxA4O(U+12FhV)%5L^g|9Ek$N?pf+<e(Vp){OL_I^dYP()~F46<E)0F``}))7@I`ia96#lLL=(aw{`8uiNrsDzWhm<6vB|?|Y}9b1y&n"
"n0-im^_M-Yf|gIoTu<CszJCt!*p@Z(+P9MrnwQOA*JGc$tkcFUo!wXLRj!ZbS|nRhTg%aX+<W^1iqYn-U6qR5qW8!A;bSk@*7Oz~I<he7ZDkJ4(-(sviawul"
"mcWxp+@E_^weOF}Ln|+Fw`<-(ESMUTLF;vZK9EJBnf~oZ@eOM+w>=m3!npawKb2g-;my0BWK<?rZc)AMA<ZJwE7rlN<<Rq1T8hto_BH<i|LWK4Q~^t!k~p5C"
"uH1DNLRzmV=ag^99(JxOe^E_+O`3--SuA&;y;Zt62DR}~MWxjY^s(;mDX@h$uXS0<p_X5{84R{5Tg$vYix`Q*7`2f*HqM+5*$MRhB3iSlnv++OPyP1oNkLS!"
"<Ue#bc}<(WKaxvFD~q*^v1T?PUfqM*5mjcfN`L;z`R3#o*YhT#V!oSeb#{$5-o$!$pr?S&@GJ*s{7y#l+Xm^*c2>U8{w#kfxm29eoyyINxZh6Bt9F!)S5ynG"
"V>mR|PVa;_SX@amy_S&#*1R%GG9PY-5}o{?_l4V25_8s=h4L$Gjp_OftHLf&EQ#R3%B^)ywYI)GD)v57!6N;!&@+*1eS5Zc%T@9_;ytI3I`pg>>h_aS%;lo_"
"<#JYEwWyt5*R$#!Z;7*1{LUAbS4Tdc%UZ%^;COAVxoxrvrhCnyRfYX5-?)Fy@46WOu1&GCo>H_rALTB`MA;er*&IX2{<9QAy`?C=u9>qQL6MoQ)Ow`1B$6f_"
"Ygut{Eg%FXOOdretX+D(FVdY7D$|B6m|kI#rh)mTHD=bw7Ro+c+3j%3m2PE7<oi&AiiAfZWIEmWfwXcPECxK$O&iB|%i9n)0OyrRJ{c`C%UNUHs&mkqFYCu-"
"Wo4!G?N2YY2fx0VH^gezyKC*nw%Nv??6q$6l;9bTVgFq}HXr{Jll0a`PTuQ%n!lW4XJ&P86&8fd|DKpr@2cuQQO=`xZIQX%!fte9nJY@UT6syh=NGvI3|~7O"
">lMD&=U<-^o6@E<kXxC@4A*9vHAbFfi6sxjcY3U9b-gknp`NH5Lcu+udm`Qb&SLFOpyVSG8%Za%N7jvXGddJXr^6LiK53{}^>D~=Tsx%7E9~NnA6+a@e0())"
"h*f~&o#tB42#RLc+aqgg;G7WHy+UE^#9w%1^=?Xi*!d*i9~Z1H>iV-x3GaJ}Ma`DF_KId2y9I?7E7^^3n=eo%kycLPYx_iD=~Aek40Q_6_xalMN;qpw*|S1c"
"z%jvK)mKjV7fA2Q4%_TXxm%f%P<Pc1k?K%{O=TKC1WUJ5#jr2dLKlf$VwJrLd9H^uxlodmOIcS<1~_DWR6P!q*qqXT`}or8*r$UzbGl-=6K!?2t)OtK+Zt$U"
"!P8vp&YM2oK;k*KY<nZE+vt9ryOv;8Cv<P8rzN~Ui!Z46B+VC;<Mu-^G}TCNM0#wlI)SxvqCndxh)Wn`E0XBtU+D8~OsT-MIRoSbQ+oNeDb5-*<74xsA4;~n"
"oJyrzALdi{^{|+AOM*`o*S;ex-^!H&q3DK1?b|17{JI;x7D=Te8EzqCu9}Qazv^l2h%B@C*}r(yUmt%uku_DymV5PPt7`>C<L$MPw$6H)XWG2rvrQzwtC4QB"
"lZwr?PqH`S?D(Aae3l_%y%1SeZ0Sp{XcqB9JH1#>ZH8NHo-9Q)vtnN}D0a)4!PRiQUHooepgS$Krp#%3iofWxjn1+fth|>ikUrw<cUB68ZfRO_f*+JBPxzn7"
"G{4U;-!T`1aJ(y#Q@iBqQjLS0kLL67^inZxu9++jTJ&M%4q0WdlwSDcbbI32M9vhcneP`n%}on%qW{%_Rh9Lu(7Jy%;J+OCu3O$&&&an<{O5XNxj1RKkein9"
"|2)M|Y)NX*sb}#!_AsNI+YN58`I1Dul@|nCG|w+x3^x<;PWjt?;oc<Qk+WvNJeL&!t}bvotUSpTN*)M~*V&~~uQDO@Jy6+2l4F5uBHMdzp>zi=1;VinfQuhe"
")yX=~bLFI&MJlX9+*+~foZfSfs)uB8g<pLB)AOCNPy2HgRmpUBzuDY0fuhM)bEGB%&r0pv*MicssqailwmO*;FaIcaJ;5x_7|s>uB`Yt)7>bTO_buHdd1MbS"
"tfn`^ZQhD3?rvt}k&aIQ@R*^NKiWOfPp~s5)#r?9OLm2!6%8^u-IYAa6U**NcG@e&N~bm<CZFg$qF)|LkeOWbx%J9jThR|EHjE7NfNYejao5#IE*HtN%1Lw8"
"YC$5}`zQEIEOzO;ADwHCKE0YZt(7ddbFKQi85B(R8bfOe+Zlm<=d#~Bp7?Jf>AlUoy3zV1cO|t_ozs1gnU`?C9cQRF)U{Vt)9_s|JbRQr;h)75#XZfOAkqo0"
"fRcr5GZycbpX;N0vy#P}CF96Rm;r2XW_gcY_=Er4XCHMrC;V3jh14URLwxpFf=y;y&ne3fGNmvY*|?mpF(HE*NMY+;hkwQ@F-09@)6FF9(+c$mBOM+FXaNCf"
"J`R8uKp9k8e00pt2A3_qWB@-vpDLqMQ0W5UiN9ppF~e`0M{Prn^<8O=<CR+;KyHh{k7&Hn3p~ORRxGh6T&%414bJpD#3ph<@i3+GRs(?dw_Sy+?xjb`1;59q"
"B<W_?)n`Bi_r+=x=rsmOlo0qrwg8!M06wF}VgA5tw#{L<wvH)ZPL@Xrt4nL~Y_4I!c{5*MG|~-fjxe*ihWBr7aqLc&Zjn-bl9C!x3C^{I0x|1^m4PH9!N9Te"
"Q>|~(UOVrO7hY8E>uo=>jUX57&ApY4MLQ38hJk>b#?+zcdy6QU;fyU_v#@LME2`0P>yoz`gU4|(R`;Xy|LlmdjxRdu8O#xwP?jj|I{S>k(t}xpS%(-YuM-y2"
"VH@VFLQ)@~biyCRqz;}92G&vi<@V}`N1{|0v2M>vPoivnQA>A;Im|I4*S+-a{t^nCphdUgvzS^3YlCt{I-Yh-n5RH0LwX<jKi8gkWab=}N<XhTR+wJ|ui4K)"
"_m5WAmmMPD8H}mbxYH1fo@d$|8R5j_fQ{XPUjrtm^vggctex`cur-(6`!5}?yG>Jr_(ar8Xu)`U-_nO@XP-#y7Kckm<0#H1Yr25929ps;iuvC2ne5A$!%#=S"
"GtJs00s47fwB3SY(;&xKGkDrw24x*ko8KO4@L?xlS@fAdi|YZD?@_90cUV7fAGFh9PwXE4m4sOKc0YiS<PWU-GT;TvfMvjuJzzH$P`>4f!{AhhoYFw>8kcmY"
"_%|1ZJ&P6R>&Va;uz-{U)FjCz8<aEfuRW2gL4_u$fiUHkdur8PqR&cjpUKTSlUwblAe4ut0k5|JTKra#+kGLbwT*{SRl}@!e`j^pBZuTkXQS;3&6v5`tUlI7"
"mMNU6vc5N)6F?q|X2EB^9X7f_DW%cJ!U2GRj=87^9=HX4VJ1h&)r(X#@gwb#-u@%gr0trcw6nO*_DX?|Fb|@kFfAUs%A@TO*7*Dw!pwp1LPO(Djx(E?9rf_&"
"vJ(yb;~F37kls;hu8a{YSfIM??E_H8hkT3G;n4iyQ&E;TWhK2C7z*3uC~-DoGcQD8O)IE=DYkmquOwuoXJZo$PaqO)(Gu$ti$2q^Tzl$9js=+wrWq3WIBYP&"
"g0#>ci^epg;E*|#BE0wAzm(!PEVcI?c=9>PErsJRbT`5Gf#%M{U9Ou4f9Y|W<N>#U@?V|tU@7M0@(mla>iMB%d>UCs15wX7sUX&ATi{>2E3S=bY=F8H!6{iV"
"*gVAh5ODFS$Y!=W^?n+~xWTd^MkT;t0+D;{rvrSL3FGQJxb}3uJ%R}2N+-UzW{-~o0&NBLuX~4_wh6iBM^~#XrvWm~dyF3?<$8dZ;ur${jAqU<4Mh@I@S*b?"
"U3!9DJr@?||5STlqW`vX*mKU_*yU`^c|^!(9=l!~^2#Ea>vIGio4z^Hui-W$FDq{j8s?xR0nfN(L>)(>|7(};-U|9hgQF22Jp(2S_bh!dviwx2H9&waNPAse"
"XWdPCD>gZ8lh}z1^nzv5?$&lt+w1xAgi~|Et*hSlhvq@4DR1+h^ismiP@CQ!EUds5i-Cn1?`PCzhbSO62YoS+?U+DAaX)<veXI>HTQb*RJpFy`p51l|8Z}=>"
"x*M#nfG{}jA$CaTMU}qnO`g}2V$Lzg)WV|dlyZ4ovqWS=j#<)#HBo^4>z2T?8VE9cGcnSmRQXmnkUj`mc_7nUZC*o&a*;A$+8Uu!kRXJJVd@&K2AKq+?2^bU"
"wfYpO!ZjI3Yt0j!FbZ^eYoO|#b2?|FmQyn^K4P=#Ry^SCw2FZEp%p{M8*(gL#~pOq?>{EK6r&5S-K#hn`ya+#jpTK7+<F}-Z434mF`fG{g4zs0T<XnTnLz{L"
"xrNyq)|IN~Rm=FSg=>z-1EVO5%u_J=yA7^qJro)t=OTVwCiAwoeSH>?`=MB`G2=#>b`gBKZlh1|BT>7;b0V`72lpb@hTC{`zj+-Iz{)7Ow&n^Ant7^<+|x#8"
"VQ`3Ncw?f%M*yRB*{6RT*BPM&V#ykh#n2hs4WlpS_kZa8ToxGDG|vRw$>%kPD(FuQL$-^^-Vtk`5zrbRI~*1Tg-Jt^cP0^cg*hd>Y2j2YuYkkT5W`%|z%!T<"
"Zu&grKX37=EuVja8Y<8MiM_ObqJNir`59Mg1#nf^=f)P@1&s3LY-r4-c99<h?Y2n+BI$aKueR$0u*ohCVLNM7WEA1+vU;SA&7#P%4t(v}t_al|Ogx|FFmA@2"
"k`P-H2}L~ft_iO%Nktw}UnzqO^!SoLocb>RmJEE^I&MD;9q!0?`0fSbF$^Ixzh6cB&n{9}dYU<L$+FZSex#Zl(k!{_Vf~y}Mt3L(`{y2)+#u;0YBWrQ1!Q!8"
"%hYFaF(32PkPS5i+zy=WExY1W-tLOn6fXEuSZ(91LW&Pb$Gh43IAZWhNvz(%f>R*hlF?GDB-d@?Dl9)8y3KPyI>UTGcPpX+K$gd1jE7@J20RIc;xXzD`H4C<"
"OQg;c;n0hQU8U^>+nD<Tu(l`MS|oJNr#72KopLFu#91z6iwKt%hulkb><!Q4l$iq;<F-YVPZzu{-yihThy{nKv7i^1giBi+&OQse_<*Ic*eSW6bYW~VV)Oa("
"pev$QKuCWWa4bQne$%bE&?=2Om54-&t+r5l%(<ekYhz9CEM{bj46j`;a}WXsm|o*%SPr8SnypVoBJM?a8VUG=!8^V$l#v0TbOz3)e`MV^z%K%0;5AouZ>h)k"
"si23j6P9~$alrHTV#u8kPArYs=E~Yvp@9k0617U1rw9(Y=5+-BxwC|=1|vh{U>GATZeQcT(g)ZV9x}~<*{Kdw7{T_&)tEPHi$_7F%XKj<2L?<I`5I#UllAtH"
"Q|{m+Xs7FmOp+^_h9%tcOhVLz%=FSgnFlFB9fwjp%;+?*)Q=dW@gz8fjGBT4f8>$lOF?8zxB8x2U3^Y@ECIgd9@4)DR1aC(3t^f43E%=GF<R;Kp5ZthnR&+4"
"4b{q8^kwb*nE7OIgZK!=!y7&{@Xy;kV$+`=CT@l>x76XU>^uAHF#8G9pfMmOzsK(6gfg&?Dk1u?RRX)uh1mc@<x#C;maFOF049cX9%rphg{MqHT^gdBff<K("
"UaWg#wk|*|sA&PQe3(>woQe=%nD)i&V%wZK7Q};hk<Yb(C7^!YZA-i)Jv1OM+h=WO!OFgLos06xM|Q|;rEqnSzQ!UwF~-2cpp)6qzigbI0OqI|ZXYGo<SJpK"
"{;ti&*GX*LHyR{RVLIB{LHmq^_#scDgh{OzLp(epZJ>Px&=^MKD1Iic2N!J)T@{g?tE?_iR+G4Zt~cG`X${|y0sLKJp0+K@jPFmZF`YJ>nd`SajT()10V#qK"
"R9w%3=A5B0NgQHNh4cg(xq%x}uYlX2^-ag9^@^>wHP>X3TKJwDvgjzc29MleP|hfGnLViE7wzxBi<6L$O2?oDQi<1mkoR{zfv4>cj=FC~{FnfCH#U5Ic0zbx"
"qLW!EjSnL+9vTC}WC^rTm>AXf`{h<9f@&*-@rTLws9Pp7<5;)ji43{9`Vtsy&@+U%Uut@}KgP7;78?WAKa8j>2oKk4V|36ojZK<kX`k<*>k9=vZE0Ra@#^;!"
"w=DKQGv2gcw(Q?p++IY5_@@YLH-!VBH*uPQdNPYc%=fIzRpTqBfpPnUvmQhSP!V25;ov{6t34b3#ueAi4B_EJ{*7%@pUF4*bVY9fTqc^ruw{2t5%#75R{Z}j"
"xk`YeIIpyTY!G0lwi06wgXZt<IeJDCffx)(HUuGbB6N*3(g=Z$x$nDsdb+2(XQsR79?j@Rw}cKbAP$2Ccw@l$z>Z_@#(PvM@orpBoHdT|ddur<uG&d${Z;R;"
"|F2iC-m6#j9rj*-crV}L?O<usc)YtnH(e8PhAk0(ZYmg6y811{7(0n6o!PTm;)lo1>3o$IyesOumb0?~ZLBHJf&)aWuiY2Ras18jeRJX@JzGwAhY|A?t6uJ{"
"4;=Gfzta8k8QyMxjl$gKIBlVakEpoFKxK31aYy%rsZ_6QAe6xvzuHLGb7LVq{?US&J})FyOdT5uTgGkUOZ2lI=ImOR-GK>KAsP<$Mu(VmWR$!Tx9do{JnbXS"
"_$kXYV?K~rs3EP%-r?}#1u|8Q_1Pwz%wqIZq_0`;mRDR?QcjnpgR`wA`f&dlzRQ(Kq?}5!$-`xY#9jW~KH_*d^+FgQM(o%7JR+>xTSly2>pb5=8GQF>%%@95"
"y0}cSk|Pt!lgcBy?rA;jKT&0E;b&9Y+K`@^jz#!SFB^g@Lashioeo8MSbk=eD!_xMANKc}JRwVhW!R2*);bWL!tD`48;CrgwtCyrgypW_sC{RllJ*Al-r$8r"
"Jaxj<X__#{7rC}rU!4_0YZqGZeygRE_k1yIF=2g7lPk$(sZL)DnHr%&uQDHM(x(F7mKiMPp3z=6xOY+&j)U0D)%w+Ls@L(zOZqLC&?3Yu)Or0`Th;SpqH)eD"
"VNX=)RCF#8tYO>fMLr;WvXa-I9CLy@G!PS1UkVr3`~_}I^`_Qrckt#kMRV%#P+mVw(2pYSKC0(XB4w-P2wli|ws?N}1a+D*j5AA1kwlBN*D~wimV{H3=1h-6"
"QnPx7#GH<fxaWSNha}I^Dk2>lvbMNS;)(1){7(3xhN<^1zmUR*w;Qfo-P_12_i-$BqkHu%?ejkHVa{GYWC#uaNX3K)yIWf-yiHRiid8rHy4mrx^;FOlnPUUV"
"k1uHH6+T+dS0!jxlMt4!(gm(S-%m~(F&k@)<qbe*c$DCUtfxDR_XmjmiC$-8fHcpA%|$aa$0@6|kBcr}j^vJUdh?7Wv>a)nyp<Lkcww-E2zBXHG1J>oO~~IB"
">+VVCl58ze6%?|Ja36b5m#Ohh?K5E$&sko#d$+nz`1S@~TkE*m#Hz@*D8g-sh4k^1^b{Q!R~~Emp}BPqlXX{By4!;1(zfGNUtoa`#2#Nb%U<9HtJtHQAegx9"
";tgMcC(!)nS-ru=SrQD(s)8c~$Bz-dD6R<#=zxi6a8c%wsQcjDLbKCs(oBUGmxx3i*=>oNqAQ8|n7h&7Dc(P<p$I3Y5{&;aWH#VBPHBm91Kti_jVF^#akr>P"
"My`pOJrJf-BKHlib0<}a9rDlJQm=G!dHf_O%*2E&T((RZ43GC#D=Tg4nSk6<ebTCC#?pp*Mo&#dLfo$}_OmbZLN!y9V?&xmX8tBy;P$q?(PyyxXhVkK*fYtf"
"J;#kwLEdX*gmT_vYYh9X(}Wu=&z4~hpOItc&W8t1n7i!Lp2Ych9cgMa`=kx$y6~XY(au|cma$U7{)8oupIML6`(rFQI*_|VzHc0>b5F?yV)Xl(8#Z=}p&Bp8"
"S8l16nrOA}AxZf1>czkbM_WlY(^91@v$jv-yYRB(78N_3wN-E`YK8Sj9>1buE@iSuxQbkuJ)Mls-sB6sftH6IM!P#|PDGPbM|8p+<FZ~hXIJCwe!=c*PBDg="
"sCVz+%QdXA(cHvdyg;X#urB?$C%44aC5KLGC|TuTYtT<P+f#<uV>&CQ7M#{pH0P`LHwC#A%e~IMr%#@AjK6@p2e;~9)iOJX3j82`>3Yk{D!!k1i*gdC&_I7^"
"u(ZkJ9Bw<=d>B_xx`6%gafcEcO<5XvH8sQ0{BN!_1TV4l5w2!9#A-)}m#>5huu%Jh4uj4Xu;lGyM9mMfDPhdRk2;MQiwaI$-AI{dS?8YILT$hpFyhSO3Z1F;"
"b!&4zVUew+v4(y-QZvy@5mt{nX}Oa$XmDdv<4VSIE+t(Tip99_8uiGO?w|%<vJ;~s&8pD`m}Bfg|8&0pR@QEQ$Y35zgww@R2b!@+R$Jd%fuEhBV1L~SM@Miz"
"6>bWjiOz-n@!ziM=;g#neYi3nXU#qF<;#Hr57B+^wA17!bvc%#)k50K@)LghKtO}h2V<R>GE5uhDOc&jR0HYgH^qa?7pZhJ)?=Uc=JCIQ#`|>kgJnx+aIM3c"
"=iPfbt>14=*obsIOSW5EFg}^#Z$;nLN1J<w%7TuR*mBX1p|+~P;lR`t^`$OGL*1n@k3P-lW7(sPek!YO?W*;*k62K9%}J-)Hp-i8L#FVwK(W7E?hl=h2J6_y"
"WQ4aF!_yb+1#Vy09gV@_iC7b?pH$OBW{x|DYtyXmZ~mnLQ`#uqY{0O5kxbL026B=K&7O7Ej}6Q#W^+_SYB)0*=~!yLz+Cf37FDEuaz;BgA2m<*XZ$OX*@1o|"
"*FS0=(ub+n!l5a0By*h}40<z5fpJr2@i{y<wwR2)I5K$;j}~`DM{>DXw&k6q!&vafaLBJdG?5v_2`c6l@AqF0Wd^n6d?e_)8VE2UGh>QZUU9EcZAp?FaccX5"
"Os|$_XN0L-m*6LV;7@1|2nIUSmW=uRM|jTe!kBbIaASFHA|vTLX<{TjlEUc>?Nmj>HwP^vgZ^m9G(PVlB6xDt<Nm8FLJdT|9J9xSiHs)g4EpBumTO^8p#CTk"
"rrY~w6X&KnO(d6#34}M6oyfS-kpWCQ8KqJ^j^t=M&CqwTs}5I!^3!&HDlkeX&(RvJe~cFV<Kg>W?^4EqaT%&NAq;vZqMQ&PamOj&B$i}6>F_zbFfp8t3<X@1"
"Rwg&<pJ&#u%b)cH>k7W}cN@O5VZ*u&>kA7vZYtcgabw|ATedv?)YDIG-m-P;GuxisnvXlSZ+~w4j_upF=My#+Zr)V5adY9ueEO!pz5f3yJR<qm;@k2exli$q"
"bVo@<S}6(1Fw~8HB)u)YgO*7+38VN>eg*xVG=Ta`eW*eT%YP~_m;Y8eAb(r&%hKC>c1j-09>@kHKbP)C)q64}hf8WpniXR7p=7=EbHpk|O0UVUqjB^D>55!a"
"k}7?GUXy<%`KcsTQY(F4rbE}uLa0?CD;|+<mqgK@p??*xmn0OAWGkg#$exqmm%cB5R=iGjwB!}pHuUd{39+wao$N`zLYpj(cFB&&KbLJ&xa8kQ-!6G9t3fxS"
"yGx4DXG#u~tXF)r=MD5LX#?s+*U87lTA8S{T<SxAq4<&fe~Kegx$LCEh8h)(@{J|O(I?24XcBFeEy$meyX5;MF-3)<9W9W2tXPx&MEqa!Qt1)2Np2RmOD0Rd"
"RqRn5F8x;ajhvM1mAxvdEcT;F$$Qd$C7;Tk$krrvQb_tWdR}pAZ%+K5CBKrb%2lO@(Qo93(U&Bva!^_?$BQfEZ=j#a|0spTSHLdlzrinxZX*|<6u7hNT9F*i"
"z&qh-<jta^024KdzJk`EA3z5YKPVLy!~5V8NQb;AHbSl7I<gMg1x|^_;NO5r=|1tFVHyGw8w5c;U>x)#^~fvWUqBmr3Ctk(kgd=^L1OsNumafyqmUMc!M{V@"
"hyngeM8dZLCsIMHAO*I;D(Ji5pP~PN_CsGof7rb!{tOJkoX9KkieG|uzy_!jw1HQk1t5kTP=VwNXfMn|zZVt3--SPczJUG&JqzE3HX|DlGu(@8g$Kn??>-{R"
"z%lVbP$XW0N5BYl84<$|AVIVR2}ygQc8G<)7GHupkbTe-@H<cqLO>|0fIbiz5DHoo8O3fe4{G59@Cif-{TliUG=_W#K1GV)&B(vO<?sh0O!6u8Jtzb#fdb5e"
"o#IbmN_<E17m$G8MrgzY{Q;~2TznbvB5k{8pxkaT)DK$nlYRyr61@mEf%ipU1C^*6d;{GBe=PbId4S-eLQn^8fw$md=&@)cC<Ha&hG;uD5B;yG5*mampc<$$"
"zc(5>h8#e?hwKHPi{xNEcvn;jeBwWe{!#Qiq=EK9A*dBfLuHT$s)c?JNT3Gh{4C|56g&YpK`+pQ^FRR2`L2HsDnTg}2SonA987}-(3RiDufRu=UE+_${|5j7";
static uint8_t cb[65104],wb[78409];
int main(){
bd(WTS,sizeof(WTS)-1,cb);
tinf::inflate(cb,65086,wb,sizeof(wb));
lw(wb);
cin>>la>>nc;ng=la*nc+1;npl=min(ng+2,59);
for(int i=0;i<nc;i++)cin>>cx[i]>>cy[i];
for(int g=0;g<ng+3;g++){int ci=g%nc;gx[g]=cx[ci];gy[g]=cy[ci];}
it();
while(true){
for(int i=0;i<4;i++){int x,y,vx,vy,a,n;
cin>>x>>y>>vx>>vy>>a>>n;
P[i].x=x;P[i].y=y;P[i].vx=vx;P[i].vy=vy;P[i].a=a*D2R;
ut(i,n);P[i].nc=gc[i];
if(i<2){P[i].sh=ms[i];P[i].bu=mb[i];}else{P[i].sh=0;P[i].bu=0;}}
for(int i=0;i<2;i++)if(ms[i]>0)ms[i]--;
for(int i=0;i<2;i++)P[i].sh=ms[i];
if(tn>0){to[0]=max(0,to[0]-1);to[1]=max(0,to[1]-1);}
float obs0[OBS],lg0[NACT];
bo(obs0,0);fwd(obs0,lg0);
int best0=0;for(int i=1;i<NACT;i++)if(lg0[i]>lg0[best0])best0=i;
int a0=best0/APOD; // pod0 action (ego=pod0)
int a1=best0%APOD; // pod1 action (from pod0 perspective)
for(int p=0;p<2;p++){
int act=(p==0)?a0:a1;
double da=P[p].a+RT[act];
int tx=(int)(P[p].x+cos(da)*10000),ty=(int)(P[p].y+sin(da)*10000);
if(SH[act]&&ms[p]==0){cout<<tx<<" "<<ty<<" SHIELD"<<endl;ms[p]=4;}
else if(SH[act]){cout<<tx<<" "<<ty<<" 0"<<endl;}
else{cout<<tx<<" "<<ty<<" "<<TT[act]<<endl;}}
tn++;
}}
