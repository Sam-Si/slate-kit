#!/usr/bin/env python3
import time
# Start tracking process time at the very first line of execution
process_start_time = time.time()

import sys, math, ctypes, subprocess, os

C_SRC = r"""
#include <math.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#define PI 3.14159265358979323846
#define DEG_TO_RAD (PI / 180.0)
#define RAD_TO_DEG (180.0 / PI)
#define MAX_H 8
#define MAX_POP 96

/* ===== RNG ===== */
static unsigned g_s = 42;
static unsigned xr(void){g_s=(214013*g_s+2531011);return(g_s>>16)&0x7FFF;}
static int ri(int a,int b){return b<=a?a:a+(int)(xr()%(unsigned)(b-a+1));}
static double rd(void){return(double)xr()/32767.0;}
void seed_rng(unsigned s){g_s=s;}

/* ===== Trig LUT ===== */
static double CL[360], SL[360];
void init_lut(void){
    for(int i=0;i<360;i++){
        double r=i*DEG_TO_RAD;
        CL[i]=cos(r); SL[i]=sin(r);
    }
}

/* ===== Pod state ===== */
typedef struct {
    double x, y;
    double vx, vy;
    double angle;
    int next;
    int lap;
    int shieldtimer;
    int boosted;
    int isFirstTurn;
} Pod;

/* ===== Physics Engine ===== */
static double newCollide(const Pod* p1, const Pod* p2) {
    double dx = p2->x - p1->x;
    double dy = p2->y - p1->y;
    double pLength2 = dx*dx + dy*dy;
    if (pLength2 <= 640000.0) return 0.0;
    
    double dvx = p2->vx - p1->vx;
    double dvy = p2->vy - p1->vy;
    double dot_val = dx*dvx + dy*dvy;
    
    if (dot_val > 0.0) return 10.0;
    
    double vLength2 = dvx*dvx + dvy*dvy;
    if (vLength2 == 0.0) return 10.0;
    
    double disc = dot_val*dot_val - vLength2*(pLength2 - 640000.0);
    if (disc <= 0.0) return 10.0;
    
    double t = (-dot_val - sqrt(disc)) / vLength2;
    return t;
}

static int cpCollide(double p1x, double p1y, double p2x, double p2y, double cpx, double cpy) {
    double dx = p2x - p1x;
    double dy = p2y - p1y;
    double pd2 = dx*dx + dy*dy;
    double ppx = p1x, ppy = p1y;
    
    if (pd2 != 0.0) {
        double u = ((cpx - p1x)*dx + (cpy - p1y)*dy) / pd2;
        if (u > 1.0) { ppx = p2x; ppy = p2y; }
        else if (u > 0.0) { ppx = p1x + u*dx; ppy = p1y + u*dy; }
    }
    
    double distSQ = (ppx - cpx)*(ppx - cpx) + (ppy - cpy)*(ppy - cpy);
    return distSQ < 360000.0;
}

static void bounce(Pod* a, Pod* b) {
    double nx = b->x - a->x;
    double ny = b->y - a->y;
    double dist = sqrt(nx*nx + ny*ny);
    nx /= dist; ny /= dist;
    
    double rvx = a->vx - b->vx;
    double rvy = a->vy - b->vy;
    
    double m1 = (a->shieldtimer == 4) ? 0.1 : 1.0;
    double m2 = (b->shieldtimer == 4) ? 0.1 : 1.0;
    
    double force = (nx*rvx + ny*rvy) / (m1 + m2);
    if (force < 120.0) force += 120.0;
    else force += force;
    
    a->vx -= nx * force * m1;
    a->vy -= ny * force * m1;
    b->vx += nx * force * m2;
    b->vy += ny * force * m2;
    
    if (dist <= 800.0) {
        double ddiff = dist - 800.0;
        a->x += nx * -(-ddiff/2.0 + 0.00001);
        a->y += ny * -(-ddiff/2.0 + 0.00001);
        b->x += nx * (-ddiff/2.0 + 0.00001);
        b->y += ny * (-ddiff/2.0 + 0.00001);
    }
}

static void nextTurn(Pod pods[4], const double* CX, const double* CY, int ncp, int laps) {
    double remaining = 1.0;
    double curps_x[4], curps_y[4];
    for(int i=0; i<4; i++) { curps_x[i] = pods[i].x; curps_y[i] = pods[i].y; }
    
    while(remaining > 0.0) {
        double first = remaining;
        int ci = -1, cj = -1;
        
        for(int i=3; i>0; --i) {
            for(int j=i-1; j>=0; --j) {
                double t = newCollide(&pods[i], &pods[j]);
                if(t <= first) {
                    first = t; ci = i; cj = j;
                }
            }
        }
        
        for(int i=0; i<4; i++) {
            pods[i].x += pods[i].vx * first;
            pods[i].y += pods[i].vy * first;
        }
        
        remaining -= first;
        
        if (ci != -1) {
            bounce(&pods[ci], &pods[cj]);
        }
        
        if (remaining > 0.0 && ci != -1) {
            int c_idx[2] = {ci, cj};
            for(int k=0; k<2; k++) {
                int idx = c_idx[k];
                if (pods[idx].lap >= laps) continue;
                if (cpCollide(curps_x[idx], curps_y[idx], pods[idx].x, pods[idx].y, CX[pods[idx].next], CY[pods[idx].next])) {
                    pods[idx].next++;
                    if(pods[idx].next >= ncp) { pods[idx].next=0; pods[idx].lap++; }
                }
            }
            curps_x[ci] = pods[ci].x; curps_y[ci] = pods[ci].y;
            curps_x[cj] = pods[cj].x; curps_y[cj] = pods[cj].y;
        }
    }
    
    for(int i=0; i<4; i++) {
        pods[i].vx = trunc(pods[i].vx * 0.85);
        pods[i].vy = trunc(pods[i].vy * 0.85);
        pods[i].x = floor(pods[i].x + 0.5);
        pods[i].y = floor(pods[i].y + 0.5);
        if (pods[i].shieldtimer > 0) pods[i].shieldtimer--;
    }
    
    for(int i=0; i<4; i++) {
        if (pods[i].lap >= laps) continue;
        if (cpCollide(curps_x[i], curps_y[i], pods[i].x, pods[i].y, CX[pods[i].next], CY[pods[i].next])) {
            pods[i].next++;
            if(pods[i].next >= ncp) { pods[i].next=0; pods[i].lap++; }
        }
    }
    
    for(int i=0; i<4; i++) {
        if (pods[i].lap >= laps) continue;
        double dx = pods[i].x - CX[pods[i].next];
        double dy = pods[i].y - CY[pods[i].next];
        if (dx*dx + dy*dy == 360000.0) {
            pods[i].next++;
            if(pods[i].next >= ncp) { pods[i].next=0; pods[i].lap++; }
        }
    }
}

static void apply_action(Pod* p, double target_x, double target_y, int thrust, int shield, int boost) {
    if (shield) {
        p->shieldtimer = 4;
        if (p->isFirstTurn) p->isFirstTurn = 0;
        return;
    }
    
    double dx = target_x - p->x;
    double dy = target_y - p->y;
    
    if (dx != 0.0 || dy != 0.0) {
        double a = atan2(dy, dx);
        if (p->isFirstTurn) {
            p->isFirstTurn = 0;
            p->angle = a;
        } else {
            double diff = fmod(a - p->angle, 2*PI);
            if (diff >= PI) diff -= 2*PI;
            else if (diff <= -PI) diff += 2*PI;
            if (diff <= -18.0 * DEG_TO_RAD) a = p->angle - 18.0 * DEG_TO_RAD;
            else if (diff >= 18.0 * DEG_TO_RAD) a = p->angle + 18.0 * DEG_TO_RAD;
            p->angle = a;
        }
    }
    
    int t_val = 0;
    if (boost && p->boosted == 0) {
        t_val = 650;
        p->boosted = 1;
    } else {
        t_val = thrust;
        if (t_val > 200) t_val = 200;
        if (t_val < 0) t_val = 0;
    }
    
    if (p->shieldtimer > 0) t_val = 0;
    if (dx == 0.0 && dy == 0.0) t_val = 0;
    
    double cc = cos(p->angle);
    double cs = sin(p->angle);
    if (fabs(cc - (-0.28)) < 5e-16 && fabs(fabs(cs) - 0.96) < 5e-16) {
        cc = -0.28;
        cs = (cs > 0) ? 0.96 : -0.96;
    }
    
    p->vx += cc * t_val;
    p->vy += cs * t_val;
}

/* ===== GA Individual ===== */
typedef struct {
    double ra[MAX_H], ba[MAX_H];
    int rt[MAX_H], bt[MAX_H];
    int rs, bs;
    double sc;
} Ind;

/* ===== Evaluate ===== */
static double evaluate(
    const Ind* ind, const Pod* base, int H,
    const double* CX, const double* CY, const double* EX, const double* EY,
    const double* DTE, const double* RRX, const double* RRY,
    int ncp, int laps, int mx,
    int rp, int bp, int orp, int obp, int rb,
    int fboost, int rtimeout,
    double dw, double aw, double sw, double lw, double apw,
    double byw, double opw, double sfw, double faw, double rw
) {
    Pod sim[4]; memcpy(sim, base, 4*sizeof(Pod));
    int ir_cp = sim[rp].next, ir_lap = sim[rp].lap;
    int ior_cp = sim[orp].next, ior_lap = sim[orp].lap;
    double r_act = (double)H + 0.3, o_act = (double)H + 0.3;

    for (int t = 0; t < H; t++) {
        int pre_next[4];
        for(int k=0; k<4; k++) pre_next[k] = sim[k].next;

        /* Our runner */
        int rth = ind->rt[t];
        int rb_flag = 0, rs_flag = 0;
        if (t == 0 && fboost && sim[rp].shieldtimer == 0) rb_flag = 1;
        else if (t == ind->rs && ind->rs < 3 && sim[rp].shieldtimer == 0) rs_flag = 1;
        
        double ang_rad = sim[rp].angle + ind->ra[t] * DEG_TO_RAD;
        double r_tx = sim[rp].x + cos(ang_rad) * 10000.0;
        double r_ty = sim[rp].y + sin(ang_rad) * 10000.0;
        apply_action(&sim[rp], r_tx, r_ty, rth, rs_flag, rb_flag);

        /* Our blocker */
        int bth = ind->bt[t];
        int bs_flag = 0;
        if (t == ind->bs && ind->bs < 3 && sim[bp].shieldtimer == 0) bs_flag = 1;
        
        double bang_rad = sim[bp].angle + ind->ba[t] * DEG_TO_RAD;
        double b_tx = sim[bp].x + cos(bang_rad) * 10000.0;
        double b_ty = sim[bp].y + sin(bang_rad) * 10000.0;
        apply_action(&sim[bp], b_tx, b_ty, bth, bs_flag, 0);

        /* Opp runner: aim entry points */
        apply_action(&sim[orp], EX[sim[orp].next], EY[sim[orp].next], 200, 0, 0);

        /* Opp blocker: chase our runner with vel lead + aim ahead */
        {
            double tx2 = sim[rp].x + sim[rp].vx * 1.5;
            double ty2 = sim[rp].y + sim[rp].vy * 1.5;
            double dx_cp = CX[sim[rp].next] - sim[rp].x;
            double dy_cp = CY[sim[rp].next] - sim[rp].y;
            double dcp = sqrt(dx_cp*dx_cp + dy_cp*dy_cp);
            if(dcp > 0){ tx2 -= 500*dx_cp/dcp; ty2 -= 500*dy_cp/dcp; }
            double dd = sqrt((sim[obp].x - sim[rp].x)*(sim[obp].x - sim[rp].x) +
                             (sim[obp].y - sim[rp].y)*(sim[obp].y - sim[rp].y));
            int sh = (dd < 850 && sim[obp].shieldtimer == 0) ? 1 : 0;
            apply_action(&sim[obp], tx2, ty2, 200, sh, 0);
        }

        nextTurn(sim, CX, CY, ncp, laps);

        /* Check CPs for activation time */
        for (int k = 0; k < 4; k++) {
            if (sim[k].next != pre_next[k]) {
                if (k == rp && r_act > (double)H) r_act = (double)t + 0.5;
                if (k == orp && o_act > (double)H) o_act = (double)t + 0.5;
            }
        }
    }

    double s = 0;
    /* Boundary penalties */
    for(int k=0; k<2; k++){
        int pi2 = (k == 0) ? rp : bp;
        if (sim[pi2].x < -1000 || sim[pi2].x > 17000 || sim[pi2].y < -1000 || sim[pi2].y > 10000)
            s -= 100000.0;
    }
    
    /* Win/loss */
    if(sim[rp].lap >= laps) s += 1e9;
    if(sim[orp].lap >= laps) s -= 1e9;

    /* CP crossing bonus */
    {
        int cur = sim[rp].lap * ncp + sim[rp].next;
        int ini = ir_lap * ncp + ir_cp;
        int crossed = cur - ini;
        if(crossed > 0) s += crossed * 15000.0;
    }

    /* Runner remaining distance */
    if (sim[rp].lap < laps) {
        int rl = sim[rp].lap * ncp + sim[rp].next;
        if (rl < mx) {
            double dx = sim[rp].x - EX[sim[rp].next];
            double dy = sim[rp].y - EY[sim[rp].next];
            s -= (DTE[rl] + sqrt(dx*dx + dy*dy)) * dw;
        }
    }

    /* Velocity alignment, lateral, angle, speed */
    {
        double dx = EX[sim[rp].next] - sim[rp].x;
        double dy = EY[sim[rp].next] - sim[rp].y;
        double d = sqrt(dx*dx + dy*dy);
        if(d > 0){
            double nx = dx/d, ny = dy/d;
            s += (sim[rp].vx * nx + sim[rp].vy * ny) * aw;
            double lat = sim[rp].vx * ny - sim[rp].vy * nx;
            s -= fabs(lat) * lw;
            double ta = atan2(dy, dx);
            double ae = ta - sim[rp].angle;
            while(ae > PI) ae -= 2*PI; while(ae < -PI) ae += 2*PI;
            s -= fabs(ae * RAD_TO_DEG) * apw;
        }
        s += sqrt(sim[rp].vx*sim[rp].vx + sim[rp].vy*sim[rp].vy) * sw;
    }

    /* Activation time */
    s -= 1000.0 * r_act;

    /* Bypass opponent blocker */
    {
        double ox = sim[obp].x - sim[rp].x, oy = sim[obp].y - sim[rp].y;
        double cx = CX[sim[rp].next] - sim[rp].x, cy = CY[sim[rp].next] - sim[rp].y;
        s += byw * atan2(fabs(ox*cy - oy*cx), ox*cx + oy*cy);
    }

    /* Opponent delay */
    s += 10000.0 * opw * o_act;
    if (sim[orp].next == ior_cp && sim[orp].lap == ior_lap) {
        double dx = sim[orp].x - CX[sim[orp].next], dy = sim[orp].y - CY[sim[orp].next];
        s += 10.0 * opw * sqrt(dx*dx + dy*dy);
    }

    /* Blocker evaluation */
    if (rtimeout) {
        if (sim[bp].lap < laps) {
            int bl = sim[bp].lap * ncp + sim[bp].next;
            if (bl < mx) {
                double dx = sim[bp].x - EX[sim[bp].next], dy = sim[bp].y - EY[sim[bp].next];
                s -= (DTE[bl] + sqrt(dx*dx + dy*dy)) * dw;
            }
        }
    } else {
        /* Stay in front of opp runner */
        {
            double bx = sim[bp].x - sim[orp].x, by = sim[bp].y - sim[orp].y;
            double cx = CX[rb] - sim[orp].x, cy = CY[rb] - sim[orp].y;
            s -= sfw * atan2(fabs(bx*cy - by*cx), bx*cx + by*cy);
        }
        /* Face opp runner */
        {
            double dx = sim[orp].x - sim[bp].x, dy = sim[orp].y - sim[bp].y;
            double ta = atan2(dy, dx);
            double ae = ta - sim[bp].angle;
            while(ae > PI) ae -= 2*PI; while(ae < -PI) ae += 2*PI;
            s -= faw * fabs(ae * RAD_TO_DEG);
        }
        /* Ram rest point */
        {
            double dx = sim[bp].x - RRX[rb], dy = sim[bp].y - RRY[rb];
            double d = sqrt(dx*dx + dy*dy);
            s -= rw * (d < 300 ? (d - 300)*0.1 : d - 300);
        }
        /* Opponent activation delay bonus */
        s += 15000.0 * o_act;
    }

    /* Shield costs */
    if (ind->rs < 3) s -= 330.0;
    if (ind->bs < 3) s -= 495.0;
    
    /* Thrust bonus */
    if (ind->rs >= H && ind->rt[0] > 0) s += ind->rt[0] * 0.16;
    if (ind->bs >= H && ind->bt[0] > 0) s += ind->bt[0] * 0.06;

    return s;
}

/* ===== Combined GA ===== */
void run_combined_ga(
    const double* pods,
    const double* CX, const double* CY,
    const double* EX, const double* EY,
    const double* DTE,
    const double* RRX, const double* RRY,
    int ncp, int laps, int H, int PS, int mx,
    int rp, int bp, int orp, int obp, int rb,
    int fboost, int rtimeout,
    const double* prev_ra, const int* prev_rt,
    const double* prev_ba, const int* prev_bt,
    int prev_rs, int prev_bs, int has_prev,
    int budget_us,
    double dw, double aw, double sw, double lw, double apw,
    double byw, double opw, double sfw, double faw, double rw,
    double* out_ra, int* out_rt, double* out_ba, int* out_bt,
    int* out_rs, int* out_bs, int* out_it, double* out_sc
){
    Pod base[4];
    for (int i = 0; i < 4; i++) {
        base[i].x = pods[i*10];
        base[i].y = pods[i*10+1];
        base[i].vx = pods[i*10+2];
        base[i].vy = pods[i*10+3];
        base[i].angle = pods[i*10+4] * DEG_TO_RAD;
        base[i].next = (int)pods[i*10+5];
        base[i].lap = (int)pods[i*10+6];
        base[i].shieldtimer = (int)pods[i*10+7];
        base[i].boosted = (int)pods[i*10+8];
        base[i].isFirstTurn = (int)pods[i*10+9];
    }

    Ind pop[MAX_POP];
    int idx = 0;

    /* Seed 0: previous best shifted */
    if (has_prev) {
        for (int t = 0; t < H - 1; t++) {
            pop[0].ra[t] = prev_ra[t+1]; pop[0].rt[t] = prev_rt[t+1];
            pop[0].ba[t] = prev_ba[t+1]; pop[0].bt[t] = prev_bt[t+1];
        }
        pop[0].ra[H-1] = rd()*36-18; pop[0].rt[H-1] = ri(0,200);
        pop[0].ba[H-1] = rd()*36-18; pop[0].bt[H-1] = ri(0,200);
        pop[0].rs = (prev_rs>0 && prev_rs<H) ? prev_rs-1 : H;
        pop[0].bs = (prev_bs>0 && prev_bs<H) ? prev_bs-1 : H;
        idx = 1;
    }

    /* Heuristic seeds */
    {
        Pod hsim[4]; memcpy(hsim, base, 4 * sizeof(Pod));
        for (int t = 0; t < H; t++) {
            /* Runner aim */
            double tx = EX[hsim[rp].next], ty = EY[hsim[rp].next];
            double ddx = tx - hsim[rp].x, ddy = ty - hsim[rp].y;
            double dd = sqrt(ddx*ddx + ddy*ddy);
            if (dd > 0) {
                double ux = ddx/dd, uy = ddy/dd;
                double perp = hsim[rp].vx * (-uy) + hsim[rp].vy * ux;
                double cf = fabs(perp) * 3.0; if (cf > 800) cf = 800;
                if (perp > 0) { tx += uy*cf; ty -= ux*cf; }
                else { tx -= uy*cf; ty += ux*cf; }
                if (dd < 2500) {
                    int nnc = (hsim[rp].next + 1) % ncp;
                    double bl = (2500 - dd) / 2500;
                    tx = tx*(1-bl) + EX[nnc]*bl; ty = ty*(1-bl) + EY[nnc]*bl;
                }
            }
            double ta = atan2(ty - hsim[rp].y, tx - hsim[rp].x);
            double diff = ta - hsim[rp].angle;
            while(diff > PI) diff -= 2*PI; while(diff < -PI) diff += 2*PI;
            double diff_deg = diff * RAD_TO_DEG;
            if (diff_deg > 18) diff_deg = 18; if (diff_deg < -18) diff_deg = -18;
            
            if (idx < PS) { pop[idx].ra[t] = diff_deg; pop[idx].rt[t] = 200; }
            if (idx+1 < PS) { pop[idx+1].ra[t] = diff_deg; pop[idx+1].rt[t] = 150; }

            double ang_rad = hsim[rp].angle + diff_deg * DEG_TO_RAD;
            double r_tx = hsim[rp].x + cos(ang_rad) * 10000.0;
            double r_ty = hsim[rp].y + sin(ang_rad) * 10000.0;
            apply_action(&hsim[rp], r_tx, r_ty, 200, 0, 0);

            /* Blocker aim */
            double btx = RRX[rb], bty = RRY[rb];
            double bd = sqrt((hsim[bp].x - hsim[orp].x)*(hsim[bp].x - hsim[orp].x) +
                             (hsim[bp].y - hsim[orp].y)*(hsim[bp].y - hsim[orp].y));
            if (bd < 3000) { btx = hsim[orp].x + hsim[orp].vx*2; bty = hsim[orp].y + hsim[orp].vy*2; }
            
            double bta = atan2(bty - hsim[bp].y, btx - hsim[bp].x);
            double bdiff = bta - hsim[bp].angle;
            while(bdiff > PI) bdiff -= 2*PI; while(bdiff < -PI) bdiff += 2*PI;
            double bdiff_deg = bdiff * RAD_TO_DEG;
            if (bdiff_deg > 18) bdiff_deg = 18; if (bdiff_deg < -18) bdiff_deg = -18;
            
            if (idx < PS) { pop[idx].ba[t] = bdiff_deg; pop[idx].bt[t] = 200; }
            if (idx+1 < PS) { pop[idx+1].ba[t] = bdiff_deg; pop[idx+1].bt[t] = 200; }

            double bang_rad = hsim[bp].angle + bdiff_deg * DEG_TO_RAD;
            double b_tx2 = hsim[bp].x + cos(bang_rad) * 10000.0;
            double b_ty2 = hsim[bp].y + sin(bang_rad) * 10000.0;
            apply_action(&hsim[bp], b_tx2, b_ty2, 200, 0, 0);
            
            apply_action(&hsim[orp], EX[hsim[orp].next], EY[hsim[orp].next], 200, 0, 0);
            apply_action(&hsim[obp], hsim[rp].x, hsim[rp].y, 200, 0, 0);

            nextTurn(hsim, CX, CY, ncp, laps);
        }
        if (idx < PS) { pop[idx].rs = H; pop[idx].bs = H; idx++; }
        if (idx < PS) { pop[idx].rs = H; pop[idx].bs = H; idx++; }
    }

    /* Shield variants from seed 0 or first heuristic */
    {
        int src = 0;
        for (int sv = 0; sv < 4 && idx < PS; sv++) {
            memcpy(&pop[idx], &pop[src], sizeof(Ind));
            switch (sv) {
                case 0: pop[idx].rs = 0; pop[idx].bs = H; break;
                case 1: pop[idx].rs = H; pop[idx].bs = 0; break;
                case 2: pop[idx].rs = 1; pop[idx].bs = H; break;
                case 3: pop[idx].rs = H; pop[idx].bs = 1; break;
            }
            idx++;
        }
    }

    /* Extreme seeds */
    if (idx + 4 <= PS) {
        for(int t=0; t<H; t++){
            pop[idx].ra[t]=0; pop[idx].rt[t]=200; pop[idx].ba[t]=0; pop[idx].bt[t]=200;
            pop[idx+1].ra[t]=0; pop[idx+1].rt[t]=0; pop[idx+1].ba[t]=0; pop[idx+1].bt[t]=0;
            pop[idx+2].ra[t]=-18; pop[idx+2].rt[t]=200; pop[idx+2].ba[t]=-18; pop[idx+2].bt[t]=200;
            pop[idx+3].ra[t]=18; pop[idx+3].rt[t]=200; pop[idx+3].ba[t]=18; pop[idx+3].bt[t]=200;
        }
        pop[idx].rs=H; pop[idx].bs=H;
        pop[idx+1].rs=H; pop[idx+1].bs=H;
        pop[idx+2].rs=H; pop[idx+2].bs=H;
        pop[idx+3].rs=H; pop[idx+3].bs=H;
        idx += 4;
    }

    /* Fill remaining with random */
    for (int i = idx; i < PS; i++) {
        for (int t = 0; t < H; t++) {
            double v = ri(-400, 400) / 10.0;
            pop[i].ra[t] = v < -18 ? -18 : (v > 18 ? 18 : v);
            int raw = ri(-100, 500);
            pop[i].rt[t] = raw < 0 ? 0 : (raw > 200 ? 200 : raw);
            
            v = ri(-400, 400) / 10.0;
            pop[i].ba[t] = v < -18 ? -18 : (v > 18 ? 18 : v);
            raw = ri(-100, 500);
            pop[i].bt[t] = raw < 0 ? 0 : (raw > 200 ? 200 : raw);
        }
        pop[i].rs = ri(0, H+4); pop[i].bs = ri(0, H+4);
    }

    /* Initial evaluation */
    int best = 0, worst = 0;
    for (int i = 0; i < PS; i++) {
        pop[i].sc = evaluate(&pop[i], base, H, CX, CY, EX, EY, DTE, RRX, RRY,
                             ncp, laps, mx, rp, bp, orp, obp, rb, fboost, rtimeout,
                             dw, aw, sw, lw, apw, byw, opw, sfw, faw, rw);
        if (pop[i].sc > pop[best].sc) best = i;
        if (pop[i].sc < pop[worst].sc) worst = i;
    }
    double wsc = pop[worst].sc;

    /* Evolution loop */
    struct timespec t0, tn; clock_gettime(CLOCK_MONOTONIC, &t0);
    int iters = 0; double amplitude = 1.0;
    Ind child;

    while(1) {
        iters++;
        if ((iters & 255) == 0) {
            clock_gettime(CLOCK_MONOTONIC, &tn);
            long el = (tn.tv_sec - t0.tv_sec) * 1000000L + (tn.tv_nsec - t0.tv_nsec) / 1000L;
            if (el >= budget_us) break;
            amplitude = 1.0 - (double)el / (double)budget_us;
            if (amplitude < 0) amplitude = 0;
        }

        /* Stagnation */
        if (pop[best].sc < wsc + 0.3) {
            for (int i = 0; i < PS; i++) if (i != best) pop[i].sc -= 2000;
            wsc -= 2000;
        }

        /* Tournament select */
        int p1 = ri(0, PS-1), p2 = ri(0, PS-1);
        int par = (pop[p1].sc >= pop[p2].sc) ? p1 : p2;
        double threshold = 0.25 + amplitude;

        if (ri(0, 4) == 0) {
            /* Crossover */
            p1 = ri(0, PS-1); p2 = ri(0, PS-1);
            int par2 = (pop[p1].sc >= pop[p2].sc) ? p1 : p2;
            for (int t = 0; t < H; t++) {
                if (rd() < 0.5) { child.ra[t] = pop[par].ra[t]; child.rt[t] = pop[par].rt[t]; }
                else { child.ra[t] = pop[par2].ra[t]; child.rt[t] = pop[par2].rt[t]; }
                if (rd() < 0.5) { child.ba[t] = pop[par].ba[t]; child.bt[t] = pop[par].bt[t]; }
                else { child.ba[t] = pop[par2].ba[t]; child.bt[t] = pop[par2].bt[t]; }
            }
            child.rs = (rd() < 0.5) ? pop[par].rs : pop[par2].rs;
            child.bs = (rd() < 0.5) ? pop[par].bs : pop[par2].bs;
        } else {
            /* Mutation */
            for (int t = 0; t < H; t++) {
                child.ra[t] = pop[par].ra[t]; child.rt[t] = pop[par].rt[t];
                child.ba[t] = pop[par].ba[t]; child.bt[t] = pop[par].bt[t];
                if (rd() < threshold) { double v = ri(-400, 400) / 10.0; child.ra[t] = v < -18 ? -18 : (v > 18 ? 18 : v); }
                if (rd() < threshold) { int raw = ri(-100, 500); child.rt[t] = raw < 0 ? 0 : (raw > 200 ? 200 : raw); }
                if (rd() < threshold) { double v = ri(-400, 400) / 10.0; child.ba[t] = v < -18 ? -18 : (v > 18 ? 18 : v); }
                if (rd() < threshold) { int raw = ri(-100, 500); child.bt[t] = raw < 0 ? 0 : (raw > 200 ? 200 : raw); }
            }
            child.rs = pop[par].rs; child.bs = pop[par].bs;
            if (rd() < threshold) child.rs = ri(0, H+4);
            if (rd() < threshold) child.bs = ri(0, H+4);
        }

        /* Small mutation */
        { int si = ri(0, H-1); double v = child.ra[si] + rd()*24 - 12; child.ra[si] = v < -18 ? -18 : (v > 18 ? 18 : v); }
        { int si = ri(0, H-1); int v = child.rt[si] + ri(-50, 50); child.rt[si] = v < 0 ? 0 : (v > 200 ? 200 : v); }
        { int si = ri(0, H-1); double v = child.ba[si] + rd()*24 - 12; child.ba[si] = v < -18 ? -18 : (v > 18 ? 18 : v); }
        { int si = ri(0, H-1); int v = child.bt[si] + ri(-50, 50); child.bt[si] = v < 0 ? 0 : (v > 200 ? 200 : v); }

        child.sc = evaluate(&child, base, H, CX, CY, EX, EY, DTE, RRX, RRY,
                            ncp, laps, mx, rp, bp, orp, obp, rb, fboost, rtimeout,
                            dw, aw, sw, lw, apw, byw, opw, sfw, faw, rw);

        if (child.sc > pop[best].sc) best = worst;
        pop[worst] = child;
        if (child.sc > wsc) {
            worst = 0; wsc = pop[0].sc;
            for (int i = 1; i < PS; i++) if (pop[i].sc < wsc) { worst = i; wsc = pop[i].sc; }
        }
    }

    /* Output */
    memcpy(out_ra, pop[best].ra, H * sizeof(double));
    memcpy(out_rt, pop[best].rt, H * sizeof(int));
    memcpy(out_ba, pop[best].ba, H * sizeof(double));
    memcpy(out_bt, pop[best].bt, H * sizeof(int));
    *out_rs = pop[best].rs; *out_bs = pop[best].bs;
    *out_it = iters + PS; *out_sc = pop[best].sc;
}
"""

def main():
    _h = math.hypot; _a2 = math.atan2; _cos = math.cos; _sin = math.sin
    PI = 3.14159265358979323846; D2R = PI / 180.0; R2D = 180.0 / PI
    D = ctypes.c_double; I = ctypes.c_int
    DP = ctypes.POINTER(D); IP = ctypes.POINTER(I)

    # Compile C extension with optimal performance flags
    c_path = "/tmp/_pod_cga.c"; so_path = "/tmp/_pod_cga.so"
    with open(c_path, 'w') as f:
        f.write(C_SRC)
    
    # Compile with Ofast + ffast-math + march=native for maximum execution speed
    subprocess.run(["gcc", "-O3", "-march=native", "-mavx2", "-mfma",
                    "-ffp-contract=off", "-shared", "-fPIC",
                    c_path, "-o", so_path, "-lm"],
                   capture_output=True, check=True)

    lib = ctypes.CDLL(so_path)
    lib.seed_rng.argtypes = [ctypes.c_uint]; lib.seed_rng.restype = None
    lib.init_lut.argtypes = []; lib.init_lut.restype = None
    lib.run_combined_ga.argtypes = [
        DP, DP, DP, DP, DP, DP, DP, DP,                     # pods,CX,CY,EX,EY,DTE,RRX,RRY
        I, I, I, I, I,                                      # ncp,laps,H,PS,mx
        I, I, I, I, I,                                      # rp,bp,orp,obp,rb
        I, I,                                               # fboost,rtimeout
        DP, IP, DP, IP, I, I, I,                            # prev_ra,prev_rt,prev_ba,prev_bt,prev_rs,prev_bs,has_prev
        I,                                                  # budget_us
        D, D, D, D, D, D, D, D, D, D,                       # 10 weights
        DP, IP, DP, IP, IP, IP, IP, DP                      # outputs
    ]; lib.run_combined_ga.restype = None
    lib.seed_rng(42)
    lib.init_lut()

    def da(lst): return (D * len(lst))(*lst)

    # Parse init
    laps = int(input()); ncp = int(input())
    CX = [0.0] * ncp; CY = [0.0] * ncp
    for i in range(ncp):
        a, b = map(int, input().split()); CX[i] = float(a); CY[i] = float(b)

    # Compute entry points (corner-cutting offset)
    EX = list(CX); EY = list(CY)
    has_sharp = False
    for i in range(ncp):
        p = (i - 1) % ncp; n = (i + 1) % ncp
        v1x = CX[i] - CX[p]; v1y = CY[i] - CY[p]
        v2x = CX[n] - CX[i]; v2y = CY[n] - CY[i]
        l1 = _h(v1x, v1y); l2 = _h(v2x, v2y)
        cos_theta = 1.0
        if l1 > 0 and l2 > 0:
            cos_theta = max(-1.0, min(1.0, (v1x * v2x + v1y * v2y) / (l1 * l2)))
        if cos_theta < 0.2:
            has_sharp = True
        corner_cut = 600.0 if (ncp >= 5 or cos_theta < 0.2) else 300.0
        shift_dist = corner_cut * (1.0 - cos_theta) / 2.0
        dx = CX[p] - CX[n]; dy = CY[p] - CY[n]; dd = _h(dx, dy)
        if dd > 0:
            EX[i] = CX[i] + shift_dist * dx / dd
            EY[i] = CY[i] + shift_dist * dy / dd

    # Distance to end (using CP-to-CP distances, matching GA bot)
    mx = laps * ncp + 1
    DTE = [0.0] * mx
    for i in range(mx - 2, -1, -1):
        c = i % ncp; nc2 = (c + 1) % ncp
        DTE[i] = DTE[i + 1] + _h(CX[c] - CX[nc2], CY[c] - CY[nc2])

    # Compute sharpness for each checkpoint
    SHARP = [1.0] * ncp
    for i in range(ncp):
        p = (i - 1) % ncp; n = (i + 1) % ncp
        v1x = CX[i] - CX[p]; v1y = CY[i] - CY[p]
        v2x = CX[n] - CX[i]; v2y = CY[n] - CY[i]
        l1 = _h(v1x, v1y); l2 = _h(v2x, v2y)
        if l1 > 0 and l2 > 0:
            SHARP[i] = max(-1.0, min(1.0, (v1x * v2x + v1y * v2y) / (l1 * l2)))

    # Ram rest points
    RRX = [0.0] * ncp; RRY = [0.0] * ncp
    for i in range(ncp):
        p = (i - 1) % ncp; n = (i + 1) % ncp
        dx = CX[p] + CX[n] - 2 * CX[i]; dy = CY[p] + CY[n] - 2 * CY[i]
        d = _h(dx, dy)
        if d > 0:
            RRX[i] = CX[i] + 1000 * dx / d; RRY[i] = CY[i] + 1000 * dy / d
        else:
            RRX[i] = CX[i]; RRY[i] = CY[i]

    # Config: adaptive based on map
    total_dist = sum(_h(CX[i] - CX[(i + 1) % ncp], CY[i] - CY[(i + 1) % ncp]) for i in range(ncp))
    avg_dist = total_dist / ncp
    use_handling = (ncp >= 5 or has_sharp) and avg_dist <= 6500.0

    if use_handling:
        dw, aw, sw, lw, apw = 2.9, 3.7, 0.2, 1.0, 60.0
        byw, opw, sfw, faw, rw = 25.0, 1.3, 35.0, 35.0, 0.045
    else:
        dw, aw, sw, lw, apw = 2.3, 1.7, 0.6, 1.2, 43.0
        byw, opw, sfw, faw, rw = 15.0, 1.2, 25.0, 25.0, 0.045

    # ctypes arrays
    c_CX = da(CX); c_CY = da(CY); c_EX = da(EX); c_EY = da(EY)
    c_DTE = da(DTE); c_RRX = da(RRX); c_RRY = da(RRY)

    H = 6; PS = 80
    
    # Pre-allocate ctypes variables once outside the main loop to avoid turn allocations
    c_pods = (ctypes.c_double * 40)()
    c_pra = (ctypes.c_double * H)()
    c_prt = (ctypes.c_int * H)()
    c_pba = (ctypes.c_double * H)()
    c_pbt = (ctypes.c_int * H)()
    
    out_ra = (ctypes.c_double * H)()
    out_rt = (ctypes.c_int * H)()
    out_ba = (ctypes.c_double * H)()
    out_bt = (ctypes.c_int * H)()
    out_rs = ctypes.c_int()
    out_bs = ctypes.c_int()
    out_it = ctypes.c_int()
    out_sc = ctypes.c_double()

    turn = 0
    ml = [0, 0]; ol = [0, 0]
    pmc = [-1, -1]; poc = [-1, -1]
    msc = [0, 0]; mbu = [False, False]
    prev_ra = None; prev_rt = None; prev_ba = None; prev_bt = None
    prev_rs = H; prev_bs = H

    while True:
        turn += 1
        px = [0.0] * 4; py = [0.0] * 4; pvx = [0.0] * 4; pvy = [0.0] * 4
        pang = [0.0] * 4; pnc = [0] * 4; plp = [0] * 4; psd = [0] * 4

        for i in range(2):
            x, y, vx, vy, ang, nc = map(int, input().split())
            px[i] = float(x); py[i] = float(y)
            pvx[i] = float(vx); pvy[i] = float(vy)
            pang[i] = float(ang); pnc[i] = nc
            if ang < 0:
                pang[i] = _a2(CY[nc] - y, CX[nc] - x) * R2D
                if pang[i] < 0: pang[i] += 360
            if pmc[i] == -1: pmc[i] = nc
            elif nc == 0 and pmc[i] == ncp - 1: ml[i] += 1
            pmc[i] = nc; plp[i] = ml[i]; psd[i] = msc[i]
            if msc[i] > 0: msc[i] -= 1

        for i in range(2):
            x, y, vx, vy, ang, nc = map(int, input().split())
            px[i + 2] = float(x); py[i + 2] = float(y)
            pvx[i + 2] = float(vx); pvy[i + 2] = float(vy)
            pang[i + 2] = float(ang); pnc[i + 2] = nc
            if ang < 0:
                pang[i + 2] = _a2(CY[nc] - y, CX[nc] - x) * R2D
                if pang[i + 2] < 0: pang[i + 2] += 360
            if poc[i] == -1: poc[i] = nc
            elif nc == 0 and poc[i] == ncp - 1: ol[i] += 1
            poc[i] = nc; plp[i + 2] = ol[i]; psd[i + 2] = 0

        t0 = time.time()
        
        # DYNAMIC TIME ALLOCATION FOR TURN 1
        if turn == 1:
            elapsed_us = int((t0 - process_start_time) * 1e6)
            budget_us = 900000 - elapsed_us
        else:
            budget_us = 75000

        # Role assignment using DTE
        def rprog(idx):
            prog = plp[idx] * ncp + pnc[idx]
            return DTE[min(prog, mx - 1)] + _h(px[idx] - EX[pnc[idx]], py[idx] - EY[pnc[idx]])

        rp = 1 if rprog(1) < rprog(0) - 200 else 0
        bp = 1 - rp
        orp_abs = 2 if rprog(2) < rprog(3) else 3
        obp_abs = 5 - orp_abs

        # Blocker target CP (ram_beacon)
        rb = pnc[orp_abs]
        od = _h(px[orp_abs] - CX[rb], py[orp_abs] - CY[rb])
        md = _h(px[bp] - CX[rb], py[bp] - CY[rb])
        if md > od + 2200:
            nb = (rb + 1) % ncp
            od2 = od + _h(CX[rb] - CX[nb], CY[rb] - CY[nb])
            md2 = _h(px[bp] - CX[nb], py[bp] - CY[nb])
            if md2 < od2 - 2200: rb = nb
            else: rb = (nb + 1) % ncp

        # Boost check for runner
        fboost = 0
        if not mbu[rp] and psd[rp] == 0:
            dd = _h(px[rp] - EX[pnc[rp]], py[rp] - EY[pnc[rp]])
            ta = _a2(EY[pnc[rp]] - py[rp], EX[pnc[rp]] - px[rp]) * R2D
            if ta < 0: ta += 360
            ae = ta - pang[rp]
            while ae > 180: ae -= 360
            while ae < -180: ae += 360
            if dd > 5000 and abs(ae) < 5: fboost = 1

        # Risk timeout check for blocker
        rtimeout = 0

        # Populate pre-allocated c_pods in-place (no Python object overhead)
        for i in range(4):
            idx = i * 10
            c_pods[idx] = px[i]
            c_pods[idx+1] = py[i]
            c_pods[idx+2] = pvx[i]
            c_pods[idx+3] = pvy[i]
            c_pods[idx+4] = pang[i]
            c_pods[idx+5] = float(pnc[i])
            c_pods[idx+6] = float(plp[i])
            c_pods[idx+7] = float(psd[i])
            c_pods[idx+8] = 1.0 if (i < 2 and mbu[i]) else 0.0
            c_pods[idx+9] = 1.0 if turn == 1 else 0.0

        # Populate previous best in-place (no Python object overhead)
        if prev_ra is not None:
            for t in range(H):
                c_pra[t] = prev_ra[t]
                c_prt[t] = prev_rt[t]
                c_pba[t] = prev_ba[t]
                c_pbt[t] = prev_bt[t]
            hp = 1
        else:
            hp = 0

        lib.run_combined_ga(
            c_pods, c_CX, c_CY, c_EX, c_EY, c_DTE, c_RRX, c_RRY,
            ncp, laps, H, PS, mx,
            rp, bp, orp_abs, obp_abs, rb,
            fboost, rtimeout,
            c_pra, c_prt, c_pba, c_pbt, prev_rs, prev_bs, hp,
            budget_us,
            dw, aw, sw, lw, apw,
            byw, opw, sfw, faw, rw,
            out_ra, out_rt, out_ba, out_bt,
            ctypes.byref(out_rs), ctypes.byref(out_bs),
            ctypes.byref(out_it), ctypes.byref(out_sc)
        )

        # Save for next turn
        prev_ra = [out_ra[t] for t in range(H)]
        prev_rt = [out_rt[t] for t in range(H)]
        prev_ba = [out_ba[t] for t in range(H)]
        prev_bt = [out_bt[t] for t in range(H)]
        prev_rs = out_rs.value; prev_bs = out_bs.value

        # Convert runner output to target point
        r_da = max(-18.0, min(18.0, out_ra[0]))
        r_ang = (pang[rp] + r_da) % 360
        if r_ang < 0: r_ang += 360
        r_rad = r_ang * D2R
        rtx = int(px[rp] + _cos(r_rad) * 10000)
        rty = int(py[rp] + _sin(r_rad) * 10000)

        r_shield = out_rs.value == 0 and psd[rp] == 0
        r_boost = fboost and not mbu[rp] and not r_shield
        if r_shield:
            r_str = f"{rtx} {rty} SHIELD"; msc[rp] = 3
        elif r_boost:
            r_str = f"{rtx} {rty} BOOST"; mbu[rp] = True
        else:
            tv = max(0, min(200, out_rt[0]))
            r_str = f"{rtx} {rty} {tv}"

        # Convert blocker output to target point
        b_da = max(-18.0, min(18.0, out_ba[0]))
        b_ang = (pang[bp] + b_da) % 360
        if b_ang < 0: b_ang += 360
        b_rad = b_ang * D2R
        btx = int(px[bp] + _cos(b_rad) * 10000)
        bty = int(py[bp] + _sin(b_rad) * 10000)

        b_shield = out_bs.value == 0 and psd[bp] == 0
        b_boost = False
        if not mbu[bp] and not b_shield and psd[bp] == 0:
            dd = _h(px[bp] - px[orp_abs], py[bp] - py[orp_abs])
            if dd > 5000:
                ba_t = _a2(py[orp_abs] - py[bp], px[orp_abs] - px[bp]) * R2D
                if ba_t < 0: ba_t += 360
                ae_b = ba_t - pang[bp]
                while ae_b > 180: ae_b -= 360
                while ae_b < -180: ae_b += 360
                if abs(ae_b) < 10: b_boost = True

        if b_shield:
            b_str = f"{btx} {bty} SHIELD"; msc[bp] = 3
        elif b_boost:
            b_str = f"{btx} {bty} BOOST"; mbu[bp] = True
        else:
            btv = max(0, min(200, out_bt[0]))
            b_str = f"{btx} {bty} {btv}"

        if rp == 0:
            print(r_str); print(b_str)
        else:
            print(b_str); print(r_str)
        sys.stdout.flush()

        ems = (time.time() - t0) * 1000
        print(f"T{turn} {ems:.0f}ms it={out_it.value} sc={out_sc.value:.0f} R{rp}",
              file=sys.stderr, flush=True)

if __name__ == '__main__':
    main()