// ══════════════════════════════════════════════════════════════
// CSB Bot — Generated from PPO Checkpoint
// Iteration: 49600
// Source Checkpoint: /tmp/ppo_iter49600.pkl
// ══════════════════════════════════════════════════════════════

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

static const char B64C[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static int bv[128];
void bi(){for(int i=0;i<64;i++)bv[(int)B64C[i]]=i;}
int bd(const char*s,int len,uint8_t*o){
bi();int n=0;
for(int i=0;i<len;i+=4){
uint32_t a=bv[(int)s[i]],b=bv[(int)s[i+1]],c=bv[(int)s[i+2]],d=bv[(int)s[i+3]];
uint32_t v=(a<<18)|(b<<12)|(c<<6)|d;
o[n++]=(v>>16)&0xFF;
if(s[i+2]!='=')o[n++]=(v>>8)&0xFF;
if(s[i+3]!='=')o[n++]=v&0xFF;
}return n;}
constexpr int W=192,OBS=144,NACT=81;
float w0[OBS*W],b0[W],w1[W*W],b1[W],w2[W*NACT],b2[NACT];
void lw(const uint8_t*d){
float sc[]={0.12666549f,0.00335436f,0.02177963f,0.00333183f,0.01706923f,0.00542008f};int off=0;
auto dq=[&](float*dst,int cnt,int li){
for(int i=0;i<cnt;i++)dst[i]=((float)d[off+i]-63.0f)*sc[li];off+=cnt;
};
dq(w0,OBS*W,0);dq(b0,W,1);dq(w1,W*W,2);dq(b1,W,3);dq(w2,W*NACT,4);dq(b2,NACT,5);
}
void fwd(const float*obs,float*logits){
float h1[W],h2[W];
for(int j=0;j<W;j++){float s=b0[j];for(int i=0;i<OBS;i++)s+=obs[i]*w0[i*W+j];h1[j]=s>0?s:0;}
for(int j=0;j<W;j++){float s=b1[j];for(int i=0;i<W;i++)s+=h1[i]*w1[i*W+j];h2[j]=s>0?s:0;}
for(int j=0;j<NACT;j++){float s=b2[j];for(int i=0;i<W;i++)s+=h2[i]*w2[i*NACT+j];logits[j]=s;}
}
// ═══ Game State ═══
constexpr double PI=3.14159265358979323846,D2R=PI/180.0;
constexpr double ROTD[3]={-18.0,0.0,18.0};
struct Pod{double x,y,vx,vy,angle;int ncp,shld,boost;};
double cpx[30],cpy[30];
int ncp,laps,ngcp;
Pod pods[4];
int pto[2];     // player timeouts (100 initial, decremented each turn, reset on CP cross)
int plcp[4];    // last checkpoint ID seen per pod (for global CP tracking)
int gcp[4];     // global CP progress per pod (starts at 1)
int my_shld[2]; // shield timer for MY pods (0-3 countdown)
int my_boost[2];// boost used flag for MY pods (0=available, 1=used)
int turn;       // turn counter
void itrack(){for(int i=0;i<4;i++){plcp[i]=-1;gcp[i]=1;} turn=0;
my_shld[0]=my_shld[1]=0; my_boost[0]=my_boost[1]=0;}
void utrack(int i,int lcp){
if(plcp[i]>=0&&lcp!=plcp[i]){gcp[i]++;
// Reset timeout for the player whose pod crossed a CP
if(i<2)pto[0]=100; else pto[1]=100;
}
plcp[i]=lcp;
}
// ═══ Observation Builder (matches physics.py observation() EXACTLY) ═══
void bobs(float*obs){
// Always player=0, ego_pod=0. CG provides pods in order: my0, my1, opp0, opp1
const Pod&ego=pods[0];
double ec=cos(ego.angle),es=sin(ego.angle);
// Rotation into ego frame: rx,ry rotate a vector (dx,dy) into ego's local frame
auto rx=[&](double dx,double dy)->double{return dx*ec+dy*es;};
auto ry=[&](double dx,double dy)->double{return -dx*es+dy*ec;};
const double MW=16000,MH=9000,MV=800,MD=18358;
int idx=0;
// Section A (36 dims): per-pod ego-relative state
for(int p=0;p<4;p++){
const Pod&pod=pods[p];
double dx=pod.x-ego.x,dy=pod.y-ego.y;
obs[idx++]=rx(dx,dy)/MW; obs[idx++]=ry(dx,dy)/MH;
obs[idx++]=rx(pod.vx,pod.vy)/MV; obs[idx++]=ry(pod.vx,pod.vy)/MV;
double dirx=cos(pod.angle),diry=sin(pod.angle);
obs[idx++]=rx(dirx,diry); obs[idx++]=ry(dirx,diry);
obs[idx++]=(float)pod.ncp/(float)ngcp;          // progress: global_cp / n_global_cp
obs[idx++]=(float)pod.shld/4.0f;                // shield_timer / 4
obs[idx++]=pod.boost==0?1.0f:0.0f;              // boost available
}
// Section B (80 dims): 4 lookahead CPs × 4 pods, ego-rotated
for(int p=0;p<4;p++){
const Pod&pod=pods[p];
for(int la=0;la<4;la++){
int cg=pod.ncp+la;
int cl=cg%ncp;
if(cg>=ngcp) cl=0; // past finish → point to CP 0
double dx=cpx[cl]-pod.x, dy=cpy[cl]-pod.y;
// Ego-rotated relative position + distance
double rxv=rx(dx,dy), ryv=ry(dx,dy);
double dist=sqrt(rxv*rxv+ryv*ryv+1e-6);
obs[idx++]=rxv/MW; obs[idx++]=ryv/MH; obs[idx++]=dist/MD;
// Bearing relative to THIS pod's own direction (not ego)
double rd=sqrt(dx*dx+dy*dy+1e-6);
double cos_pod=cos(pod.angle), sin_pod=sin(pod.angle);
double cos_to=dx/rd, sin_to=dy/rd;
obs[idx++]=cos_to*cos_pod+sin_to*sin_pod;  // cos_bearing
obs[idx++]=sin_to*cos_pod-cos_to*sin_pod;  // sin_bearing
}
}
// Section C (8 dims): speed toward CP + total speed
for(int p=0;p<4;p++){
const Pod&pod=pods[p];
int cl=pod.ncp%ncp;
if(pod.ncp>=ngcp) cl=0;
double dx=cpx[cl]-pod.x, dy=cpy[cl]-pod.y;
double d=sqrt(dx*dx+dy*dy+1e-6);
obs[idx++]=(pod.vx*(dx/d)+pod.vy*(dy/d))/MV;
obs[idx++]=sqrt(pod.vx*pod.vx+pod.vy*pod.vy+1e-6)/MV;
}
// Section D (6 dims): pairwise pod distances
int pa[]={0,0,0,1,1,2},pb[]={1,2,3,2,3,3};
for(int k=0;k<6;k++){
double dx=pods[pa[k]].x-pods[pb[k]].x;
double dy=pods[pa[k]].y-pods[pb[k]].y;
obs[idx++]=sqrt(dx*dx+dy*dy+1e-6)/MD;
}
// Section E (4 dims): game-level features
int mb=max(pods[0].ncp,pods[1].ncp);
int ob=max(pods[2].ncp,pods[3].ncp);
obs[idx++]=pto[0]/100.0f;  // my timeout
obs[idx++]=pto[1]/100.0f;  // opp timeout
obs[idx++]=max(-1.0f,min(1.0f,(float)(mb-ob)/(float)ngcp)); // lead
obs[idx++]=(float)ncp/8.0f; // track complexity
// Section F (8 dims): exit direction features (bearing of CP+1 edge relative to each pod)
for(int p=0;p<4;p++){
const Pod&pod=pods[p];
int c0=pod.ncp%ncp, c1=(pod.ncp+1)%ncp;
if(pod.ncp>=ngcp) c0=c1=0;
if(pod.ncp+1>=ngcp) c1=0;
double edx=cpx[c1]-cpx[c0], edy=cpy[c1]-cpy[c0];
double ed=sqrt(edx*edx+edy*edy+1e-6);
double cos_pod=cos(pod.angle), sin_pod=sin(pod.angle);
obs[idx++]=(edx/ed)*cos_pod+(edy/ed)*sin_pod;
obs[idx++]=(edy/ed)*cos_pod-(edx/ed)*sin_pod;
}
// Pad to 144
while(idx<144) obs[idx++]=0;
}
// ═══ Weights ═══
static const char WTS[]=
"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"
"DZ8R7QPt/2useUuXvvkrFEeE49R/x2VcywogDiEyFSB2iq4jUaXTWBTTrKJcA0mUW+kNE/BD7GAjoQoVAVVAQ7o0dRvgiHZSro+BtWFs7OVIhy5eRl8f2RUv"
"GOfp1dDONL0+nyhqPRy9vpIjXls0D1DM+7k/ueHIm2HD8Pqj7qZ1mJbuEg9AZoruEFyHovTn6VUFIChIq4ufi4gssOBya9kyg67FwUGXVTDApywkznORpYNl"
"JEdbRtOwEVerswQWk05cQC3SOh4jIDrA1vOJeqBeSi4HDWkF0El4gcNwSK5HZh1wURGef8j0YB0+GeMhuUp/isI77RJz809/itefw9PPZbTPBCcAraxzAPI2"
"J84j5x5jKHpQpoDfL8s9rlsxDJzz6lLMUFrDLdubNano7A6CTUBxpC8Kz0AfEp80G0p7qiq2Ax4CLRe5ditHzXulgDpD5CBDew8MCqAjfJGOtkjg9VpAuSQp"
"RquuVjAo3kENSQF+TF72xUOidPi1sAphnZ0YBMDKlIxGgFPzBnmmXYwCvBHhD1nglEBXW+RI6QIsjTCjviW9tN7X+pbJHUVAzG8LT0BFISEAdQVu3aqHp0sn"
"I5bvu18uWjcKwnyuJaTBe2Dj/h9P999t63/q7//Zvv27ff/D2f7+z/F3jP2T1b971r83/h9D+Mef57/36Z9q/b9a9Uea/onM/3yef2eXf6TbP6D5n5P5u5r8"
"6cT+BeHf/38=";
static uint8_t cbuf[47364],wbuf[80545];
int main(){
// Decode and load weights
int clen=bd(WTS,sizeof(WTS)-1,cbuf);
tinf::inflate(cbuf,clen,wbuf,sizeof(wbuf));
lw(wbuf);
// Read initial game state
cin>>laps>>ncp; ngcp=laps*ncp+1;
for(int i=0;i<ncp;i++) cin>>cpx[i]>>cpy[i];
pto[0]=pto[1]=100;
itrack();
while(true){
// Read pod states from CG
// CG order: my_pod0, my_pod1, opp_pod0, opp_pod1
for(int i=0;i<4;i++){
int x,y,vx,vy,ang,ncpid;
cin>>x>>y>>vx>>vy>>ang>>ncpid;
pods[i].x=x; pods[i].y=y;
pods[i].vx=vx; pods[i].vy=vy;
pods[i].angle=ang*D2R;
// Track global checkpoint progress
utrack(i,ncpid);
pods[i].ncp=gcp[i];
// Set shield/boost state for MY pods (tracked from our actions)
if(i<2){
pods[i].shld=my_shld[i];
pods[i].boost=my_boost[i];
} else {
// Opponent: we don't know their shield/boost state
// Best guess: shield=0, boost=0 (always available)
pods[i].shld=0;
pods[i].boost=0;
}
}
// Decrement shield timers
for(int i=0;i<2;i++) if(my_shld[i]>0) my_shld[i]--;
// Decrement timeouts
if(turn>0){pto[0]=max(0,pto[0]-1); pto[1]=max(0,pto[1]-1);}
turn++;
// Build observation and run inference
float obs[OBS],logits[NACT];
bobs(obs);
fwd(obs,logits);
// Decode joint action → per-pod actions
int best=0;
for(int i=1;i<NACT;i++) if(logits[i]>logits[best]) best=i;
int a0=best/9, a1=best%9;
// Output actions for both pods
for(int pod=0;pod<2;pod++){
int act=(pod==0)?a0:a1;
int rot_idx=act/3, thr_idx=act%3;
double desired_angle=pods[pod].angle+ROTD[rot_idx]*D2R;
int tx=(int)(pods[pod].x+cos(desired_angle)*10000);
int ty=(int)(pods[pod].y+sin(desired_angle)*10000);
if(thr_idx==2){
cout<<tx<<" "<<ty<<" SHIELD"<<endl;
my_shld[pod]=4; // shield lasts 4 turns (3 reload + 1 active)
} else {
cout<<tx<<" "<<ty<<" "<<(thr_idx==1?200:0)<<endl;
}
}
}
}
