#!/usr/bin/env python3
import sys,math,numpy as np,zlib,base64
from math import sqrt,atan2,sin,cos,pi,log
# CSB Python Bot - iter 60800
# ===== WEIGHT BLOB =====
W="""c-mCGS(+qClB6f1Dywd`xJN{;)m7cqGu;Eshh^UJjx*r?_W=FG>Y3S5SrNXNsi}&v@JdX)jHQ&cmbB!wwk0hkr;=(<Yg%(^`d=z#q-Xkk<y9|B%aVScC6%$P`X;a2Yb^R_)mxXWYc2X;S@n~mKV9$Vb?chzGPiOSu9tdEt)`y-<I2lRSE;$KYg%T?xvjOdy4Kcry=IhJQtSFbnWeT;>SNUM?V)$*^&>q}d5oV*O(~7J>u&WBi=JjpeQmWYIdi{jUlKQ#^)|ggH={dBxwKXHGX8sB^maY`{_Cgi{98Q}7fG|`vMjnUJxOjQughBW?aHktR!LXU_j<g&thMqyTFF)xKXKP;=!-V=MHzZUPw#Z?xvzOSt1iwh=lTn4lhQ9+s!P?f)ufy3dZQMPPtkRoo~6_!4L<sqsTUShugJPotS;-m=AtKJ`*MAxLrdcgT6ncC_1{<BfxhkSp^wm$t&0|P)f(s-N6~ZX)AgUe&>OOTn3i{{P0OuiFCXnp>)-#OE3|8lT6=5yUGCbTuItpPEy{XYeVN*5yh69GogBLDRePuPuPu+I<@u;vnTuZ9v`0%FZId`@ll4eRkK4R=skNoo(R0?-bVvTsbgciTb?AET)=GLj^=L0Wt!{D|zi9cn**qkTlC{oV>Ibd&wrc^AlG#^XQ(NSxOp6|LXd96V1Z6>@{g0$W``P}pvB?X6aVpZv{^(tjBds%kCG9eYXYqa`FTSuZ!B?APXlW~whLG_ezgyWnNlo&ye?HqYoeO6$CCT42r}^}!>MlNXQ7w&xcJ(&-EFhFx_ohwHl9a-oE$MsJbL%l1yCVtEuB2UyuT`Br)lp}eIt=>lSaZ))i=)jdbBsQ<N`2J+9G|D8v-Y7&w*9~9Or~0fwpL3h0a<%#vr1xK>3p2WaXj8XUt9Wq{zp&e_u9_q^LakE<NbOZ=W+7qcwDW%AAC2@>v}!+{c&AqyB_CbdvC|%cziq_`}2H!o{#I<+UNPc&QI;`$2HTHuICTk`!@G&N*0i2Jxrg=vFi4BNyW!G*Rd{}<Y3Nq-Pq!FPrIIDjy`%ldD2<*s1iV3s&1Q}c(gg@oDYd;-ICre@oL&+?F#!>KXi1C<zJ4Je&UpJT(tAr{WvB1yRx$=g^a#KKgJ0{A4taZmv$*@9hadmRxZ8xz-|&F&M@EUZ4%wgabNZ6g~KgjY9AIABtrYizolm6Ch?U%y=I+uWKqxNANsDF-ldyr1&y5YwWg<qPuI!~X_dI>qjT@eIP_KV;W+dxY47j;+#kENvGm6>2Er<xT2R>a+w!+Iwc-Ue#8T>GB^On4gH-4&uggRmXyuo0&6eYv9;ZGZul$zC==^B4(VBCe9B&>+*PiK7kiDe;ZF(<uBef~3ppP#)fLmK1lHJkPu7f>Ce%5Zw)Y3O?5NA*qk7JyE{L9LfeA;Z%k`HQ%-xdr)_Oe>*Yu^%!dLKQ#KpVN#V`XK-D(a6{(icRhYSnA`kdU19*OcD>oM_Ue>Th(COjnJ)=Je`c*)#pXqEIo49NdMMX#rPB=T@Y>w4gel5~Z@Q6|=j@m}QQ8E_Ijvm0amfeUZdTb8wN{=4v^(a6->Z%4Qy;abopx7z%xio{4|?yn(`(-jT|dC`w8$jQy0sYm1eWbyND4^&Lx%CFDMkNtO(`)Z5T^ST`0Hji`015;R0QBhX7a^O0sOv7W-46CYrG>Ye4Luo)<0t4%$(>^7?XUyk4U?|<i}d6AT;-Zbk+vex@%HLC?!Fn1j*BMB^64ul#PL^cx9tlYLvmdubOF+KNhC0+Sb3!<NB(RD^yC@v~P+q<Q=_}4t9_N?)rv>{4&!I~t=M#pZ-ZecrDYk66#r_M>2$jh!}3*(EEc5V0t+BeC(9)$~Nt+hUv9H^O&mW~q>Bh!elq_d=@MH9$F(hZbtG(Af1_!q6SJ3cTwt0GTBx7E=Gy1wM((B{ev>zGX4@|Azq7U&+cTn3$${wlNAXI$Ym@5k0UtYhM4ESPftku7H7^)TDgQ-0_(wD--@a7xtDzBb#seaF&p{2Qhzo<Xis<56^?mk%wy*7I*t9sl?5J(o@UHaSdkf0`|%{26r9TX(g8?`dGYP^9a6R215f3dg3O(l9Jak~-P4d;#>8U8i==*`!cV+sC-1mqzGPsa@3hxKE^(yV$Lh_;K2_GP<=ROlNop5{b?h<DKcDdwY$ue%Gp|^>4Ncxu+cuOIz4KKXm3K_Ij%nvGg5!%MKep>yYM?#m0#C<GHWn)I)1K<W!{Z67qfduH#&`y7#UBe8{`gZR2?L_nEd{vKFQm>C1I1ws~4w#9tR&H(8{0Kea>0x<9WmN7`Npf_7hDpW80wnfl&loBKcJ`yuaLzVv+VdaR?%1Jedixo@K``!=>QWGUx<jJnGc&~m@VD_xfyg-t(iZ+(vTao2|Id*8Js>o_DPhc1I?qoZ_Zi=3iv9jfD_71T}XW~IE-$#YA<e&{efzFCuYy%HB=L((g2uI1Ie&%*u5&S0KpB>HBLsMVLY6hoxNm4Zv@x5tUg|4O)y98P|Sp5*{wYZ_X&Bs)o50C|wI`?lfLmqYSdp8X2Z)ji7dSrB^+Xid*+ZtT>?<LHMvgz{)4Q0=((EuF{rG~WF^&oWV&BN46qki&J3wr}#i*y=X6E=xN1F0;t>_H{qD%F?I#k@}gW{QJ2VmXRMdt6%vea*uu~dp~rU^z4$EqrbLovTV;ss@Jw?ZFLl-gZd#mbnIOo92!HeS--Z~FF9g;+qGYZT$|J`8JDsBZKVD9++|T`;n9{;?$zF&I;xvgQF?8c4%H*ExjTEe&HR*>l9*n5$i`gH+V`_g9sY5&W9xdrw#~C0ecRS^*L~%-NdwqX)<x>FgeA7Im0np0zwAvHo_7z!iWOP(O?cmOnU89B{I<LfO4gkqGd%Fsm(P4r9&CKEW2FO>!h<h(7a06%!!Prst^E(K*7{P?ecd0)H#ka45l(T0iJMmKlJrm}E?rjJD7~iUozSOo`}}nt>DN-eAHVPkJK7t$(L-Zkr0a0RWL6ZIC8^Q{g}oqgIvo<mo*x}8G_Uoovs=18v@1e%;3oO_HqUi=EkE!|2oH*_2T_Pr`T0$|m83J%(dR>dBd!Xv|NZ~@Tl?2JE_ui){jVP~g}vYeOCI-L?8_~ygsJIpX9E2y;g^UjJj276F2l3H6T*yfzj~UUhl0@c^Ch($D>tOhe$i(hhf@Sv@M{n$)zuMpZ`QvAznI^S6N34lvKCq+{jLxlBL_Ksx6Nvw_@%4e94VBAeZg~;ueIXf@_&4im7UcqIjnZwblXR*3RHCKGQYASdKf~dil}8T=eJ+5H;(A>vm_@ub`+H*mndGUlkDJ(aElk=y5OVB=_0kDT`2fp%cfsd97mlI{&Ki1XGmKwAGc)%eY(FZt&)!itDvxB=+})WKMVd=$qSa&uDR^gn~g15mbek!={D+u<D!enCz2;6<(TpRaH90UT2O4ZUiZ{9eU@rPDBDX+A;sFaiMOvOAyKmZL$_bCej5&-4#1rlVL9w8OM$L&B4lr~<6RqOj-pQJUP%qD8@_T;Ea)@l?ZKlV=2+w8X4wY)(ryvHuNLS&{~WykO^(X?Kw4y<^2^uPQ#XIXO3KP)?u~6(?NmnaE#E>LhNNLQ^r$_3a{`KANoj5A^H|WIdZ6zP))!V*`&#)?=YDkGIE+>$iR-;8?s(<iB$Qp7SRBslSI&st`lm)%fYsunM|*D<IR>6f3!%3~A^^c8Zde>zWFq_)DiB|JpVm5f8T<}qD8FOXvXhh=>C(Y$AN82mcSn5E&+oM9T<)kN{NNqSq#Y)b>Et-dN;uU?i}lJWwRX@$Js#};CMC50Bqi`4ieHP@uJyUirJhyqByTgMxuqYrv5#gyRn~pOl=LRAVzxaEd57073q8+%DZJk5(Z`fDZh6Cs*T?hu{o9oIUcWz||Mv0mIzFBs<EQ%5`}5=YHSPb{p6Ru}zQ2FhE2S=;&ujmD{GPtOKgPH8`F_>Mb-f=S?^pZi-_LK)<8}Q!-`B_Yk8h73uV?w`Gtc$E&F3Xw`i-B!;Vq{m{_p8=jmNLw?RnutY>v<tr?TYt<r+UPM9#SLm2{o<MCY9VO-f2?Lv~YPgLWV^y6vdNY~N)BWgxe-t?h5WZ!DRtuk7l2we~T#ZR>rXGT__wy*{*(Ivaakwq?^gX%(9ss-j~kTdifyI7GWPcpH6}$}aM{bdbl`5Pc`A6yNUJ#E<R6A-@iEnG(-NyfKi}Mk1*?j6BduD^VF?;Lt}C@n=$=Jo%zG@Q%pw5y$xCBo<EL#A12i%nsbGlukL^s~kGqk~IH4{Zr51>xs&Jyw~rt72jDNDcAi|=BpDC>1reZokAjUM}(qdqaZ!Ef$6nFc1oE(v>!E}%Xvu9bvk@>Yuy(6-qr_Y`%W6csVg>QU^wwwwIiQ6<oSWk&_m2pws(>E$@j<u15oqauYL?-$F%D+bq^AXRSPdMSmrvVL-ppN)ot68h*CE^{;q2!U0cGdk2+q8W>&mM1xu28g$bk_)`q>;Ux~EgAV}QFjC0G&F*)Yi0XdJ39Lb34YO5@9TF7Y-4WLs~`qU*p3Ere8em%<_Cn(A%uSxEgpoM?XQ}_#n{&Mj$k)v%6TEC@Vb(_Tw5&B9YRZ5|?uuacP7S~B5i2|0|=?^?|f_6Osxm<QxYo=!&&K<5{^CTP1-pay{3dyKL_cgJXB;@r0T3tPTZ%<un=(rV5%)5V2<a5ZuVx-n^z!p|LRG!XUwR|M!O}7@=7<+K(&dfOXi<`FCq0T969f_HWjptnZv22zi-MX&fIO0RH;U(S4TD>{j6WmEh=t7D$<YDxE=lOKz3%XPq86A4c!5vda`<0Gte_$h!*FL_6Vuq^UDNJ0g6f;TcP2rrJbNpN#bKHGJB9~nLO{?>;8-*pR7fBUqUvBFk*lx_(#{FkIfm&Q0O!6?xO6a-7GFSXXte5^N3ZiwOe-QMLVVqo~*#aU17fUpk$Y({uIqDl3<Ld-Y*<=ikftyt#!KGVT*c5pvQ-;$yYip5$jV$;9B(v<`V91LThpQY=-izr~aPjyXjv#KTTqgy3>A)!<Lr!kRDN4P+N_A4Wk=IA&Gc9-8uD-j1G3mgjKy1+i;QO%mc*mFS?sXg}m4yiLmwz2k6!I?p**Bfi@zUF7R*0!>mCjoCv5~_jnKsr-v5<n1P8mgNYs{?un)#t!k?QVqmm7?e)}IpLM?Zd<lE){8R)^!*rLSH4Qc%VT$x<?s@P=!pgOjlt%Z4VDEnQFz+QgdP$9IW?^x{6o^4Y)7?yy4vl*A0LPe<i}YaG79@EI-EUL_J^A18bFMz)>n<%yRlF)Xe|L^$Q#B?-4U{@FP=$3rXrL?kgeQuV+^)dIRYq0KoRwJbgjq>GZRM`WS-2W#n6D8lP<LlieYIL&jl8<rJ~gt~DI7ZO0pN1Katgzv^_U03^~sHD|3Ks|B!UBdrOu9c)>k?#05w&hdm*A>Sra^A&JU`17FVX@M>Kwn9+XU>ZrS=%ubw$AxJeNX?gzgxNR6{Vx42Tp=^ygwb3+|H*iG8EYBq)C>DapWeF%ftWGR(Ar6shD@|GT!pGXYWoqcBd&7DJ!_w4mvfk>C%s`g;apO$y0yGE=)&_G8nok#rx=qp?K?*F(m<8>U)%{tl9^OXY;b`%Onx>IiyY-L5h!5#JF)p_1hmL5FUe6?O@HDee;z#MJ|CX99d{7=T;m^t&VH;h{Y*&$=hTtQ5YmjU7XtO&?4P3^16sll2)L7`>x~HKC~6R$vxC|T{1meqlhQlB)?}LlVr^xqfupMCH7<duk*L}zvz(G_0)OzTYKsu6p+eoki26B`IS~^tJ*;=`Ku|it;r!)S+7}i!1lbKq$gP%c`4)bpWCjZmYWkYEJ;U8lA_~B_HXl>ZPp=|m8O=ERw5l<nfAqraH$dkfc*Uz+24oUM7Ee8g;_x`bwfItb&VoP=1_EJM768>NQE1E_~v*({u_msyv}iltHi@jRgmW=!$fXbj>RV5Mh{1bOL|`#ekZIZ{-2Mm?Ht}_4!pb};_k=zAVsJ%E~jRBQ9~IH35!4fuwz8_)%lFj@|*R1^=Xv%ksXGSp^)b8|4PuOjqZn(R@&anJEC>!O;k4S+GCDNXI@+j&ozo6)bG*Kn3hGhg`if)C4#ANt0NLBcU>x<zsZF_cjV?`R$U`0vqWLEJ6|6a0F(_0_lTEm^qsWDN6O?>*7TphJ|w9p?$VHwR;Wc?64ylsSh6!EcAdIWAty`<QqJvmv?no8=lHFgG*v<ql(|silH5uW>P7~ETeB0)pK(0~wzInQD;+PlMh?TdY~hFH|7(-CNG2b#*J=>|Zg_T-kaeJrj;n|@rSPCFJ{)H7*4gg`h?Q7k+q@vG`)Yy9_D@`yLil*iWknL+&`fvP%KMVTw)<@^yu^0#1B~sGH{IwK>54*$&Y^wS`xYV_ibc!#lq$;qsbJ`T;B26B{%ylkzpLLmBh|&PY!%!<9TbFa9gA}|c5bwz6ho`66n<bv-V;YuI^ebZ^zY<*$w>VB(zTLZm&KR(Wg`S5Z$UlPn%xc=aas>ms4ME@YTMT|sP@VzJdwNfi(KgK?{lI0s0gT!iVtnDh*Sr9&}ABK5Lbz>$a?AE>)#!fMS11$D|GZIg3FH}4Vf-GxT_r&7YyTAp?7sL+z9D~mv-gj-?+i4qxR3=bjq%E`^HO110ob4Ocri!#oxi{ZIbWJNi>T6iVn}6BZTNV)X*VtUJ9rRC9KXts%?JX>-UFDlHn>>S`IAw?%K`x^l}mJg4`JEWLtHF$Rm^fVP__J)ng@Opi;&YJQIl}9Cvvz`mm$h#b2F&eQ@PtCp<~NT<j_s6vcjM3(8AU-c%hIMKTo?*Ic)A*>DU!Dw=)jNHxG2_ysTW@p?I$lL%*0+e_#CbdI7BdZ(*9JK3{tNsrg5U&`aw4lGF5${iLRG(Dh%X%4bX74^9F^BUX#`~TrNeNeS%=W#yLw(rmFnV$c<+}*#)-Ix2N`+H2>$H&|it3DnpseK*l<NO%&A;J1d3id76{ab3!PZ>Hb^|L-7Y5X7F>W_1i-IfxVk1FSGq$vZqJ)dJ9^8O`5z4eR3-*^1H$F?a1lWi*Vy{|ikI06kx-&{6@{>R?*ziaRPloNWMLpn|iv#qkS@+YOU8rFuBT=Al`Jvs_hI8k0xZt2Hscfs4fxGi7amv)gcSXNS68C|OLgej5yNfV+zr7Yz(P`#^CqD3Yh4{15%c0HUN{~yU}XC($*$xV~OHI6|4Nvp0v7+e)at81Il=#smWFm(V4!0rq(1BCXRQhCp*gi~{FdHYP)^SM<m?|7^a`3qxz9P-2$sqi_coQu}wfS*6wwgdOj&rXHl`b}x`NBSj?|1qd5kww56L8c|=7f_F5r5=0C^ReSHlC9ij6BN4XiH~u~*O&Oqxlx4h*nU^6=j>f#C(_?8#ei4;NC1iE-v+8eYGoUX!UqDsUH*WM*`^1T-pO(_>=f$!uj}mwC+ZGSrQjoF)fLIHLyG#>m$QT5>6V{$eEwN?Z=8&itn6x=;201qOSy6-Uk+N$=1E6f29HZRvx$z+`)|*>-jx#q0hgMJU3bo#giHs=7zVq{jxwslhe)YPV-Tr1RfaC-=DMAOI<RroSxbiu5oH6ATdn9TDO=Y5(Ccv73&m2BFVYgbGYS9JycmlC0a8c{+4e%zi);{4t~L-z_(q0djN`(GlL;@<3MENVx3><=JePJz)A~W}@JT&C@-0@k!;BfguXR2+FMk9<fB9n^L#lt6<rFZG+b%}P^v8>(Ue_B=a^tlKYt}!nW^=ySxM#cOG8P{?BvN3mC(JiHinE_3@7H-A`_UBKrK8T{Ja>F*J>2m;_Fca_Ps(tc{=MW&OFy-7IHmr_AF3e{3uc2U9r{GPeH~9ZOqXA_ZS|j0dJ<|9Z25Y;o;tZNXZcSl{)x5G0o<N_{{E7y5H+)hK9{&kPciNS+}DFHx8LIMWBtn5uy%$)5{;nCfdhei!q5~l8r3;QZOZoXdS3G_8}{?>&-wH5+|K9orRS4D*T-JZ{7z@TzJq}3EVAqr@DzjEoR0_C(b|Cv=^y03-e0-{=IWbGfBD^)9ON6+vIYp~4qsz!T?t4IQ*HC)zP$aYQ6D_d$GIcL<*qcv6>>mdaF-pE7{hm$3RXEaIPCUqy>6UN`Dh0zS^bG$$sd=c^zY>C=m6pu8N<nEHzK(kK!_^1pJ;sDu#bIpx)3jbdun+*H!AH(D$@YK-_9v%;)Oa}*RiFaWP~pc|DQ<$?zw--bKuUtex=woA2Zg>_@7(Ru^80yT9o2e8Qn{FcKNOnb>HJSFGXdCG_VY^5dhqytbDmqR=w0)b%kni;1y41WYAyF?N^P@TUYP<Bj?M#F70$p5kbV!Ard=eZ#Owzz}Mt(d{ycGJhy4R@nGo&Yeu@X8WPOiSxcZtqBiDgZpk5Q%eSA8pVIm3@f(iCUBmeHywdS&`uzFV-|<o}DXO_X`ctk=JNUr;AxG@mN%1Kx`}=i09`f`L@N3fC<Ef`uwlRMC#u<FcXLLDwInM2AIjZ)aBmH?l#r_vrUzb~+b3|W%s<Ip8dY|9ru9*kGHRnH%P-I~ZgAJIQuWc0MvF$kl)4`9HSr`fUq<;^5@<;(pL>B6DA1@-!Y;QH{Vub`q35c1?uK!g<ulKjbrZXTU3M9U1+s@Jt*C(Y~?0U+x8pm<$+jZfV4!H{2=zkC|08Vc9y@umTX;Lyaa!h<n>1&nJE#?tA-!R~NU8D=@q4#g!v^kx3%l|_o%hTWGC4$?Qr`b>Gw^sfX(i=O6%oMz?mhTgLP!(}v4)juXjO8`LO$v~A2z(CMSSq)^k}!U4%^Hx2#4>+{-hZ6m*thoAeZ%fFJgA{S`upErmjWPpQ6w;~^!v|AMn_(f9I4d*s!Z?8{`s@z52GgqyZ47I+MwJ&<ZXTSreIsj<(0Co9f-^INxIl@K71%IHCDlXfE-&2DQV%nHlOq_SY_0iO8`PbAB17I4qU!ggl*jaixywjeCHZc;7Y!HqImcHB(5E&FID%a@GXCQ9sjJh1M#^n-Lct!U7TAX{UNeB{nh5A<?D6Ci8avq-j}-+?{Gh}7I)2N4GV`t44i&)44l3sl)7Y`)FlqmEoagx;{w(EEcUK9;9Nr{?5O9bG_W*(e(Zn{|BK^dV`0syw~c_J#sf=S`muadV31zBq<;>@DY6)Sf1h&1r<9_;{naqz^0kb(A<x^9a!kM8P+5!hUu<zPeqxOU0=#*v95S$pWI|iWo-6(XJSP9Q0eU0Y%gzzuoyyBth&@vUVYui{shSa^5cZBJ*O76>iT;N%c?yWBJd%lIjiV=_7<)_ZfC@3?bAP?M@q}vwqQrw~j<em6QV7;T(7I}8G^Q&8g&@E4Q7^#rdKE}{EC!I|+P6sS7O<LyykhVB_}a-%jdt$4)Tvf`J3)#O2I`Cbm;^xkz8(6@|0z~%m%^isqEp|u{_msdhntSJ)FHUEZSU8%%cYb_8T%{V_uG*^%cY|W*uB55c^-04fp0zZIXbxCx`MTjhqT;r{Yd8z3Ld-s*!FL8^6DWYq^rNSho0t=bycw4v{CgK^R>6Oo#ziC?Ygb$U&}R*{h}t*^x}`_el2~yu3sp8ZpSazS;qW%JrrM5Y3?bT2x8hj5M@|7u6HHM$X^zi#V?T;ql__2wMM9Uro2YVhg#>f&q4-?T!Arps0Yx*<RLiyG7;J|7te^&@Qv2(6`+b@K=>;~#}f&Ywwy+Yx{QXZFSC`yz@I|$1bxl_WZ9=txUL>k0Bcki_~cYvIdhu08NftHFx!kas$OUA&S!Z62pEU~eTq<{3PLKFjp8B=DAX=_xw{6Iy!Gg}LSb;HcM-2flhQ&%P6n7w#kqrYPT-A*yeK9Y5C?0XB`UPJ9Bm~vRLJ1)*u)?syL}7fO>==3f0Jxr;9kBE^L<cRX^%k{1NUY>ke)6yTsMSjE779B5-r85+8|;rrL@*I+OrLXLl>#Jxj*PR8;PRq+-1r}-V_CHm{2>klIX*EmP7ve<cM8^vY*1xDM{CsPMIPybu!-FSd2DiJLc@pg;3D%eRLf-`idrBlXV8eK(>%yJ^KCH$#-P(PG5=d^=B?ar$jn)la`Txw9EDQP?#rKI#)$nQpl$q2swe*d|rB?j;qeqHlD3sWF}^IGJ!BYdNLNtO?H|ck9TzyE?K~{bL(qtcf@~k!~i(Dp`^GOCF+BS5_x-iB@96}7TPA5IG-hV5CKAOX_*`mq6-qiEl!}oo!HabAesQnodj?v8IV&K=!v-Ol@}7JOZg*0xot~#!<|72(uF8_s@avA+5~)cd)rQ8HrF!I$%|n;_B?8|AL?BItPRT4!J^5;<(uhO8`{UEyVV3<JPO3U1v#2G0%O*f!*Cg@>#m>D#9$|Nk+@|e6u=cVDpmS~%V0B~q)fe`#0?Rnxj~`Vh(n#T24=Gu@rOM!l*5~-2lFQp7&ok3iQd5I;R1$i)2#D->h>vxcC51O+xl6Y;pkTxm(HLbPH)?s7n*>(>rN>2Y5@Nw?GJ6oH^7k!B*#;mdTg!id7GG=)@<JzC5*N@XHI9PEuDXEloXu~E@c>k!fLw3PYMS^$eSp#RKLTz!@Yj%=#OSW<R+KfoQ_S0%}2^;Jrbb$UbjwB3%_HohH(ulX;XT2Yn1j!Wm`FOL$3Q!Sg8PVoJ%icPJuuFq)5tDRGqi6oxPv*OweSc?~k6^H|<>mgRS3eWpDM=NoybjW}W9)H+_<<)7S?+5+Fhp!hn<SbANT+N#)jMiia$k&L{{2KaFl>8{n<EmP{p<8i(sh`CXn0HO9B=@ekgoGw`uq&)mTVN&8Jb<9j<3ZKAu(D95C3KfPtk6z<LNaPyZ3YMgYG(WSnY+wqI;i#r-^cf{l{s;svZ-R{3bku}amH=0&Q3sI$VwBEo&e}c0)i`{3t=h*=-uUh`NR=!`7?zAB~Ki-4my+$mPjRjq!*(ZEzd#nZ?WMBDCQ##Y);@F*5vPP3LY;f>>R6&)I{LIxos@awifUsWa%u6+h+-Pl1(v)s6(tx<$=qbG<JHrSDCg7W!v!_+6m^379K8x`dvXMw*I#)D}HQGYN?Uue&r(@?`7J{4sN~lGz=uR%C&uG?@BI~QM35F#Dh-Gj5!S(~}uB?%{<B71+c>C_&TivV0sfy+>H+DJ$iN2=hhSDdUt^<Y_NxFzg8HJ`}G^kq>Fc~q8LqvtP)gAm_y+J{h?tzIL9>CgW)Sy)N$g5oLpOPVHDUvYUp4RzbD*#OBvoihy&xzVEvR{KX=uy7F$fT!HfL(vnNkIqYvKSf#rh{USHnOuw!-}4@XpZLnu#JmRF^}FkF9Cgqbnv>h+L>sM_EOxBEZy4i$D2`dZp}n6>@1dOz+A&s?;L~y8V_v~y04SNkRBOwVt1bD*<8K7<U`WLx00=r!rJsHeZHjTC{QH|(GhCz2d>)V^yh$YVmGA>>0BI`F@B5*a;1*&VX&l<Jvl4Lf&;H&Xq{Cvs25wAYW<ty;i9LA+8Q~1_wA8Yrcrz?V6rV91S5`SQ2&H5__<VR4C0d`O!G{#q@<N*L#^|yPg=n8PTRURFyMLQO|YgIifw$ZG@fx62Q@gtm`*(^5`8;aA7a<VMaGU<rX>#Jkt~%4TW5J9fM;lk;t6|=wW-nOSuC|Xwu`V42S2%F@BWRtCubI<$AN4Boy=!XhU<c3En)tj(yDc^@_8$R0P9=>C*SGv$LDmE2i(rBxl2@EZ(GJ5tj$0ZzDMxBj-e6a%?)HrG{V=23T;JK{3IIuY5mCbz}Mm8@uU~A5*ivXzt9=mxP&1PIAY!Y0g&WOUy@<^>z_G01N_)p*-*nV$;EKGJ9GfcoiDX#7SZ={@?_ZH$f`&!-U)M`25y1uuAM{n1TF(>lcl*y6;BNpW{uNsBjgTAeuyV;kj&~>hW-#cch%jM&;|^=(5=}C1wDXLb67@?r0e>I5iF)hu!Dvz;lrxA0$P#pP4X_h4o;>{fW&c9TF@diaO=6#IIJk8ZQWpY*=={V4r7vVXy|omb@sI{9kfV)_4GQ?>`n+G1qyko@NI`eqduKx!2g}ku|IyUk8z&wyebN(<Gnf0U6<)}R;>y`bY@#%Znk-l(W1!FL1J~Rg8~CNB+G;u0p<eT`aBBY!h~P5YF{YFW=iO@>~2L_jOdY?bpL{Jd-d?CcRLRI<4q+B-FX$}&v66EhOg9p`9hGfwnU87%LyIY-34OzI2bLk(tr<&8(Q3?e#=I33w!q0jxPyBEn{|WxgO%-d<LDwYqU^P`M8@VdcD(3C=*m*211mLC|#x#UF)4&qX_8&&P|>WJ=&{V_1xrRAd54iITAb0UE#&J@Ug^gBe6a<2=y9QBL$PAkv@7Y93_D@y8AsqOd9KGP7otteD&;5iyUr{ge-In;85@~2e)>Vcv`@CI5Gx`ahF@fMAbOgoV(kM^ybY-%++m7k-r~K5Z;HEXI+e*$^q%b$qO{l;%s>i!U4eMtDiXCw*AHyff%$r+!f2!d=z%k?5ssmpcty<vXxz*2w|4UC!`1@7leED%C@h!k{z(b&|FZW?<YHsNz2^;T_PXY?~kWq(P{nRMXOIeK2i&dgnp^9hb+X7!?pL3>EBq_bEB(ZC2D^V85RJ!QUxIY^Gw&=bqn(SH#@`9Jht{)s`lW30L<#|c9dgr6FWM(;B@r5leHp2AYCA<L^q=~PX%Ue%>6W$VJoBoCS^zz55`~OK!$Y!DWhmk=2hoQ%3%y+$p@i92CX*RIa9qb&=ZW{n2?}M3OvxHT+^#GXyT<O&ropFhAg*Fo~?}oCC+Z+%9`H=gpuP@i%Y#>d97r-Q^~oPmA5Z}!FL|MgumQu2tzlpkHs&pc`s3L3o-%X&$Yt}V6grg&30IJ;>={J2U)rx>Jhvyc5Dgrvs*85ySSkxWRIj5g4o<_=TnX@a3GS%QSbrTf|x-dI8h#t(=4$O%G?k|=Nq^U8ctfoC@c@Q$N;wf6$ts}oc*w*F5!0L_By&+jYwD`Vlc=m-<@!NHO7hFJcsL{Jl)2zMzJ+6>EOzR`23ahj^0$x#oZIwjLg0bG0NR%#reDHWP`IGZX7R$e7N@IS|1XP!DTb66KNCw+m|5am)-R5E+9Pba0yH{<Gf1-wuPCN4smgL3EGmIOoph$Zb8NHI0t+3RxlQ*0Q)K#6`yUZ*WlQ=vBcfpgA*IJhqP|?C1_Cs&`x|rB$UZsa^8DHmEhL!qj@MQO)#+#gX{mt+<qgdR%8vbN-=-0PIgaF$@_NIU0>)w8qXD^|4+r*$JV7N9+T9X>~42M2Jg_}gKdN+HagtNu<Jwq6}T0-24AVqbJVL2UZr<F$Q0`TeJG6I`}_xQ(FMyqH=tjr4)PMZ>S0t8MYa8Ron(ay1J$4YLvOyWmbPP4g!p`X03u3retz^hpQPqx6Mt*_Hr^d3r*HCVS_-H=5Hh#w1RN;;S+)r!9E2iK!@2cxiWXYC*>`ozfKvO;DMGw2D1_m|KILCge)Ma)lj-hJ^pPeeT>`Oc%qj6o)QxK#oaMeDTi+WVViSHtBkpD*7b@#bnDob6?61}opKkCF@LnQ<Z^xX-7>POj-{z*1W1Y1So%QMS$ug7XU+8W&Fn$;*LuR$P<y&f->YCALYrv|$Twtg`BBEL%=kD+A<6Cg;>n!2f<Gsd@&3Y6bv*UALtD2)Cy5PtR*o7vda;K8a%NDB4Nxp`f(LpaRy*b6Pm>{i^nFozk+#r+j5Ek2b0{-HbvVz$#V^!IGsNAAMOoFqKX)*v{-KstN>?R`g4R10anLMIa2eF~R;I{jNz6bgM!E8;pL~lz#z@)1;1z-xu?UKrvY&F75QUL3A&_pPKUx^c`g_ujoCHicHupU1<q_9FTY?KFab~W;yJYY4|i|p=`h@hM1Ov>Wv#yS~Q?esfs=W*~W`qMXHQEkJ@(;x`F2wEmhx09vk1->LtEyUn~9>71S6%tWk(COG2?&U+=NR-Am-9fi3Jpn_6(xaJ7#_-)5SyH0rPAgzk58U%eDBBIql((W_V^?4@X;vX?&><`tJ+4h2L!SD&DPW<DzOtGQ=Avzoh=JPS;lvNf$)y-Ju-a)N)nYK8B!4<fkLL)U9SchGp!?hq;-^TSR9A~=RPaW)1~ZmxG&35kx0}vk0l!5O2YkY&P~TZaT(M`ffLFK*i0oS1k$}MuwGXRX2RBS0NM0?+FiQ^U^VB|)+tcy1w>uUvA`a>%3GguTr$;KI*qYKZ>4yxohFjA~2s@}O0TrS_w3;`qj#6&)hMj8aQ|J`u{|&q>c63b?<i3rtdZ@4xxy<GOhF&|9H?85U+Rcji%Gr8@TJQkq%Ol__E?_x{g@2LVtan=7x~hx5aGHUDqpzVw7x(JJkYlHut}&-OpanGS0L+Suc$7)Jf1EYcNu)M)v8RbpcH+ae&h@S*;O<fDvtg+2aj){G!R{8)QEQKUi-eT=xz!vVhQs&f3J+N~ClOt7MC3cw@C{xmK*RLBN}nOB&FM#H8R0}+3C4p{jQ;6Np%r$6QE6<?(;WhhjwO^Y^lB3lmbPq8x2>SR-6OXASBM2oSpX~Tv(y}ER6@1Id&!Eg;mY~nT9zA%Vc6vo<=z_&6JA_Ivr7a$;rAK8$;LZIWht|>#}N@Nxr8Ei9JM&YTy7|iy`Lq0F_E~?##MUwt_!pm0aW+Le_;i&2P2q7TF?d3Bnvmw2aHoc4|MJzEAf@jm&VV;E0w}CPC*$)=qO$qb?#R*s*jW2NJRcYN>@^H9GQMU^sw|ui2@K3aDQb^4(rtl=aqAiq+gp;F)gY%0hTCt>dY0RczY0<bxSz_lzq5MsmkN(rhy!rWCjkw(w&oBH_EQL&f%!KSt&2{1H>^xTJV0{o6*BWLs)ZnkroQwOW%WRvycc~O?E_n&+tla-LT7Ii<AvLrkmJ&40#DKdWlakp~=FsM81O%q&mB2*bfH8ynfjl0~(HG?!VQs1Yaz}v?%xm%Z>~@ZtNn0NWH)nmE>Gs0xu4))cXFyixqrg*ISdVi_?j=MNl?0@<^ncWCZnNI_w%XBo>q=N0;NE`k1KiR=-IvJ52R@S2Odm&fR&}dbZQKC&D?}VTKvgW(zEoaSJFkaM=BV!-eNUDD#7yEbCS;7)neRk?2L}dcgl_j8vt=zILZV{zmKn2EKcie}1<;QXo8N?S<m5){E}>9Uh`BIkT+?N)K$0Gq5|=Pv8X&u`xpa3Dbe%YyEA&yp}~mmq!bq7X5S#2Z;EKkgX;?x^v)_elV0yNGrnH=|x_wzkLL|bGx0GJ?fiH9>j8FH}h+qp<d%f)pbxk0WJ?533@_kB&}4nPNZaYc($(lCBq@TJ%IAhfo?bT!eT@OUIK1dp@!R*VC{S^)CFPaa*c1x=dxd8U(L_-*bJp~p>)e-f1G$0$5yW)L#?ZTEuHB+=t+Knv64t`C)uaEuj8{!+R@+{WZO1G0w;zODEYEGqsT|DOSdCKf<<DtxZ%#2ZW191p<HAgEI}Jvvm&4lDqFB`etdG7iZsX4VYMLxYZ0t&ZIYBs$gcc`1N}7mNvQzpNRAk-$Ar7hhSpU-x)&<{r~QF6v{Bj3ZBoTW$oxR5O9f?Tw^P(Q-T8%GB}xs#ROeB!9NJPW9`QT@ES|{fn8x6SVr`~<qeWQ8ZFRr94TK&<{&S<Iw???oO@awFp_3jom`(x5bHRGfV6%#PEhK{FIB4@2k(EGqx-Z1Zn;zLEEItoQxT<e9l3p^oX!Ff5vr)0YWMx1EX~DLq-A|k;)aj&L*AUOe)v<a}%EC7=_Y>tK_%xcq43f!C%~CD_R2`6ba-}RKve^{SM6|;k!a-_s0dONw9fKo?|3!N|Eqo7}dN-<TTJI|ppTXlMQ!kAjWU3>*AAN7H<+(N-C3(V<rCry~YzaxFVJeapTAdH*)>3`-kw2%-#qz`GQ*c?!2Nfa3*FS-kp_e2*H!}|Fs)K&{X}dX|+KZeXGWM_x-}bN8h-7Tdhyrft-)1A%rMBd+Q%uH+5$)StnxvRxfE#b!QtpeTxLAS#ZS1rcNp&rcvh0ciXkyjYW=MeCw8q_*;W}+kpXTwwnd+EI^my68vBNGS!wrAiERWWm`<nW6?fu+lK6OSFg2}MYa+yXO%8pZh{cE=J0Ivy0t8OQ~<-^moBnbn>fLsnBPSmE1?&!mWz&r<Pfttu^lR=yJ3N5VbF!Z=fRd5sTYzG2CBGV28c&0S#>yrPsys*vEo<AKZ8>#z;?s32+JIj2ge>-G7(xGe26UNDLv1xK>!MyeLGZFrGr~FCqp~0!BkT%DSv@5AU=gv~1*GX7z!@bb`QTB__mArZs8%uiB5}|bz&5imjc6OZ)YcBFt_q3S^Dm<Hv_Nb1+!Yj^p$@$8N!r)zw3L)+R-L{o(x(1(!Q-55~z0anpq<H|f(eWEFRJpn&;}#l|w7Tx?2!>DQ%v-)%IBi?FVT1t9?XIu@H6nr4*wdhr!Hr4|92#Y$%DP(c*fn!U=B=8Fnt*;+h;-rzEjR9Bj+3jM3M?WUUGhGm7N+jo=d}I>^7G~9m=QsGf}XhDk%&I~1f?PEWf!N)FL6Doj2*(#Wo$Co5<2Tpf-d<23!HhY8RQolXSuOFA#vLwHUKGk5Kpb?E#0(IPd&6l{V*6&M`_b)N_2sXU?!a;+J|#T8x7(ZGav(6XLFTvEwU^W3z30VZbvvnY(`?iKOtaS4K=6{!-H!exo~eidcm=?YoImiKW=Q%Bk53MG06sGGfFubkU8xz=VJj^=t!vy;6_PTO2{@6K@=<&Ya%U-PQ3Iy$N~A*N2jF`6euYdS(_bl7;5swg3*9LRBTWxAG42RcSwzt;bA#TgwsNEj}uv)74T8^x5UA~D%f`rz_r|n=Wg(4&%v)*0+&lwe5nVaAK3vAa#wcAT<$S2G8*WUOTGKfdLdgVIozhh#kgdu-T<akD=0Ps=5aT2d%aWgCe&Flt}c(22pxh4n|Ob7p@^_uI>gK!!~S_Sv4%MfqE>)^Ul7vB&Ka3wZdIoMoHw)hn~866>`s2Bej(fMrZ%zxX1k4c{&GXxO^{O}`;IRH+ud~e#<9b~=bE3VMj?i+$YxBE7t&q-UG98r)Ka5p8(Bjy0jb7Fr$)uZJ%(#^g%a7{T`VnFE!MjaC%ciIn?w~wHkZ0Xc0zbr?;>9mPIKVLZce7`TudN|OSD`pcTZ?uznbsv4z#*_;lBJEG8cr?(GMOtq~-3FahCcfAyiO6*!oB@LV>!D)Q#NNyaDT2sNS3U489|*7>s`=OWqhvq1Wu&e5L&`CZJ=UwxByp#7NGvh0ofYk3-z!zX1%`<V4Mr0`ORaXBdYl=ugHS52L$n44psJ(Y{fY(m50>HU)26ehecuuLhqyUCb*C6k~Zj5)kCMo8)b@M=|v}=FC_SQjIddEfh`*j}Y{PqxTjMhX1je_tBJp4RCX}!Y_6SsdNHYY>^K(?WSY4^jK)Fm2}PGbZFbt53<$uxjr2nri-o~I#lU%P7cOj{Z*baSslF0ge%x4q<dixc*~Y=mwDgxNx3?fXE1<CB`5}!oqP(ZD+PKh{g1#%-uV4cZ~+j~Q(nI``HR;zf3z2jZjO)J>(O7w59$2lm-E`+<5Y}&$mM-}%FMs!+|Kv$?%%)(Z4=6$asBdn>D&E0mJ<$}Z&yz6_w74fh{pk&-+r95J{|mQ+w^n)_&oS#pZf09&nIIGc*n&n{PyYh9>>_@&%yuVI>+((c)#Yk&22v)pZywR+aJ&C{oJ>U>(u?Y#_Pv*Jmz^``lNgv&*OY;TtF9G?~0-ipdlZ3;WV%SK1$ICUT-L6AT*btE^%v9cDBPfOj~pJE-zn=VgcD)ZbII#ljk!YJxJ+BWd6a%R+qoFmgO(5_cao;CY*|*4|lahV^yf0=zU7jFibJ)E=Z-op8mDn$_1{4l7S91KrqSj!z^q1--$OhvKy9djZ5di9gvLpAh?=uuiPF|QnXU11*#YXC!5m<5nu+NunEcinuF{EF%(w0$(vwOgpTJ}r&|O_SW=BdO_t-N<VSSmp$9D=!?CSAY?BEvhC~NJ;q`dy<_3VlcPFN~N(Yp^otvC;X#fTJr(4?zEz0;s4j4mDIz?6hG3F*R+uO7&NE!nYDqA`4XAN2u)F3qzvb1Y-YMZdcHYbxYaOi2Ccxnc~Z=8%=oS5fu;>ns(&+sxAm^qc}?IAA)@Ku+-j8k0=0yCLhWPjaX;xC|CA+AQfFLnye5acZQonTyDoF?-+saG3iPt?-=M&c`d+w=OlE{w-9=m0X`li3#3@UWQ!Hub}`qkDV`tnQn9DVAhio>H`%`3XQ-8BezDGyv@soap~Te(X4#TEU@ql=0xzFA>p^g~p3Uk1h6pBD`~j<c1Y5@mzQ&W`6RLJv&w1X&6~ISHix@FKef65swn@U=YF4vPV3i*PR3c&G1m<slPY3yD7eTYX>iswgmdvXeE;%GXRL7m^x8SA0Fg#&J%Mf=~?<3?es_gMcyLu*l1+lu9I%Gz^ro=ut0a?OHf)O{38ax48pR=6o9JpYcY46nc<Yffd2x&)QO|%MjMF3p;%JjxWF%6%AxHgK$SHt+twAS))fj*g~;a7=z#3OXcim0;ZQk}_S1QoM(F}6c8jFz-Fcm6OKR6P=~>0+li3|aUYm@!2U@Vtp<He@VqC&bsMlOdb&@gq0Ns^ApjRSUM77l}UM_ddcMU?KNFPN()i69~4ve-oOm{Wd*`U&53a(C0h}M$8<#tnu7q=27kEkH-00+9>98KDP3V6VY%0^&Ey^)L>A@?vzj#I&OpE%H5?IjVL3|E)=&fRm<v?l;r&5}h>Vb?{L`nj_L&LQ-T7~yo1dEaPuV3q)yDLrz%9?yKVeLGiq?7Q(H#Qq6<>s%GX!F&i>(q|CG>HLUZ&5jlAAmn)_UR(f2^#S`lNh-;J8NBBRuE&`FRmu)Gt8v<8ea){j7KqWv|F6s0?!u1UN4L{UHB{s9r_0U(NbWVL1n!=5Qt4o0ocr@co(SQMiMpF|?mw}~j;dT?pdx|Vh{|aoTm$VA92fC_haqlTL;!=3JL-SN)SCXOxUF|}#<#_%dQ{8eisKTkTsNbHArXJ+A^z&lFCP39WEN3|`qPNs{l)pumgg@7;7uNuzc>N%SHy5LuSS)13FKvX?O~|%O?PtNLi{v;z45DiB(D8+!63}aHwnjI@BSuU`AdUz{koqo#sLtr(UjNj+6I~Fg6;u5E8T+!?ddFLU?ss1rj|GG(?;*tuym6Lw7=gsH!C&hlzQ9xqq;oyB<+spe$c5)6tfvc0@O;|B`?C`HRN3K)uTL|C@052h>X8I5>x38=+NHh0_U6LOzM}}T2g>>E+$vnT%es~PgeMcr|%qPD#hi}nUMkLzj|DAKj+qWx_>wMB>Qu}_M=^NVr@ryHkzIG4ZzX!Igg~@%au8{{xki(IHpqE0F<Tq^*G1o`pSN7>wM5(D>w82sI+Z(rW=3yp<}I|^(N<ZJI8@1JoS`Q&hNM;3HyD%_PLGyn9+?q+PST6Q{H{E7s`De*GDJ))7#M$mHoDHqvyHH;yjP-`O(wa9{u?7n&TKn`nx=#!df5OHO`cNrtM1~JoxNKqf4i(54tC{GnaP$a4*)9j*BLM<EQm;n8pCG02Pi8BgX-Oq^JCZb!UzM?V4JHZ98`Tay$A)|JKAyc<oTXN<rf05m~)J*fWJ?wzH1M(NDz!*M!&`j!9kV+OM^rgC^^D-E>3rj6P{5+qI$RLHG_S!nfn+AJ6yIp9Kn=k>oEz=DqG8kMvz?>ruCl_WTuIwj-V86P^+|KO2bd@3fQZ8poEtG12Y`vVXh!`AP~Q$PAylByk9Y<goE;RcLJkB$MXH_L9Gmw)V=}%I@Urs*q1EMCl_Sb?zsZVdm=|Ka$)`;+DbR6NAsQ`2o%B0R(rNFPF~Pt{2h_x6Q!C?PP*=#Y-~dUi{vlCt-V=2SeTTaatu9cx7-1t;)18=^^*WdE#E|t2X14&~-AhVSk!@RE{~;er?Cm_%OghB$AfDul>@gq870911<ucmpz?OtiP8h0RPc;JQW?Z{YgQdw%usg)xGK%g5xKJ-90dc-*ouchg~A3#c@pSC2iwvdf~BEpRLLw#D!=ZCM8Py?s%|6>)<wW{nn<$u)i{*Jasm8l*%~s`>katRjbfo{vMyae-PD6eZUp3n_AD6PQ#XUplRIf+p6VUcHFFW(9>8Or(#3h;MnD6sn#}u%7NbhVIE|<mkF25Ws584>id8c=iGQayuUikUqAW<fYM{~=!a_U$y7Y6;)jz7s9v5Ptg@GFa=MzG#R}D{9NnKCdF+cG`(%#HJfseoZsWN>y?r}=>G}FDEmjm8{SH1w!JRJk!Pt&Uye{2xP|0E1(R|iRit!+oTB8|Dp8jWR=j&6qux!%bI)^as)0!cjA!qr-18L9@NeSyzF*{NlV6yKH4IwwYMft+G8)CMDCRN3Gt6NS7q_YSMi6RshCW&rO(~d$<H?)2;Txmo_UrBe#t2M>2Jdii#mR=u-7VLGpwJe3WKc5FuDLpK`qTSpEw1J?=aTJIB1$O2WHxHV=R3`as`l^6bm>>$+x(<n3pn*DkC`@yPo4ld@Lq_W163YI8v)BS%(ZcVgq^C!!IUHfff2SPw=IM2m<L%cBO4fLk%dg$zG|77}=6)-$I~iVF9nR))ln--yunLLPzLUOP>Ur!*rviGuX%3Gg(Qo+)nx#p}>LA03{k{zI%+baLdXK-H|1Jk<fv<KJS~MuccrqgxkgV_zZfiG{1pdPmqd9$Xo>KcD`iXv8(!a2&M~jok%$Q5rzF_t+yzKtB=uOP-Pc#`{KuAnL9j$q8M_=5GLZW#wL^?7ffe<p&OErh3eIk>oy69jSwYHn6+*JTw<a%E3{WxFO{&=462h=U+laZJ*TaVXu>VV3~@`~elN~t~`9on?lw*S5z=k>Vsn)fL!@Q{0RJzi4C#{rex^QAAY^W%Ib9j8y(to<STsW+VOcKm++GWr89I@o~l(Iunfx6k90?*9EXfBNWuLqqKS)LmTq-Rr&lj4kSq$9HKxDYPdPAMfpX{E%mHr1#_cdz+t!p5sy9ug`PS-Jc)Z_MYFr@A>%t?fZFdpRXVDA>+@VYx({OQ%Z{7J4ax#*p9f2p|c2g7TO#E_|2wF^hl)`d+Ub1y93rSx?7Pd62E3?Q3?$nnFipcc@9j#(4$F^*_`*h8?*i}pK>P?ZKiOj+BvHw*xA@R_|BNAv0NSiN{xRM8a=X!$O2qK7Mxsgc4$~)9Gr>#TmkY=t2;oWn&*B&2bUyH466mNv_#;_OH3slEuqxyt2_RvV!P%Nb0)wWw}AJTAj56umu12cq2xvYBLB2gEY2R@vxDQqDj6e5QijfEc}E0<G*Dvx44-ygGnsPE6%$D7K-4hogZO7DXO6j7pq@aSP`Q$RAxQwKPjA#js6a+Ag$nuNSTkHJ-#sW~yaMytz{;7}p?;yvlj&{U3qmx~-A-6RS@$m*qLII7l8nl9mNXm3y_o+ln!l*P)R<&s_Fn(W$JZJnX7olwN-!%2UN8WG(J^oEQItx`P1Nu7!`)kDeya49_$)#BZ1v=hziOd2<k0s7p1vBLT4LVSU0!iGWmIbn7IKfCpDDK-Q>yb#iRsov4;2V{EI2cQ!87DMfd5V|6B&ot$JnFb3}DTNkJsYn6|a<|Q}YGOBUyYnj!@3c4L!zhl+ykrT+}l%gQJsD$#?Uh&ziwdBv50>Hpvr!Mbr*Tf3Hm6QG^B*+ps5qZ<PUhd6T!ibjl~4F5)%4$d$~IO$$=@e0SO7a7=9nnRoo2%6wL;nRxq{>uTID*otPZn2p(&OafXwf{g*e;1}k}1MBFgX`B)+8eu9RqMhJO57*{a044%&=7}C84I$PqaC7w;gAdAb%9GOrZ60X8D^}c~!r(6QG{57jBXn3JIXem^3Cg6I3RTWrG-+*TUS2hi+}Df75_&-yFtmC|`W<F7tI)MCi12J2nfmP%m*+&4>Q;8683OnQAec&|n?YgjjOnx^dbF|ejP$87WYsUHgvnLPI(JH#Jp~s8w4>FHn(qG{_DW#xBV)ydT&`W+4`yzAz4C;)wg=Uk(a;FhQ69wwj*hlr*b^Q3Jahf<E8QwWC~kJdU#5(lu3^0Yv=A^bWGWo-fX5PLd_!*TmJetAa39DWnl;SjagZDQZC0_c_sOKkR_Xx}2^>i^{}-)v>#<1R6UjD4evy<h=WcR=ag{0r{MT;YiQv5f^A__eft+=ZUg`DIv`nx}=!#U@XquE?H-grodQdAwr9j0GJK*qnGZh9lLUIm9jTnJ$j6&I3oC#T%zJAbz5QDHhC5e2l<XK8S43I*evAc?>@aD021*(OXe%Nsti%hyw-gM4pQG--#mQV8LPdJ-`{HMOnZsrll8`L!nG()0KMHoz7rDC*N#lY)f-k+FnK__nB^2+F@Gn$D2$aI9k!(_{qoCsZ+!s7kKlF&Ryu0Fcd-1m|$a&)#IC35zW<uyYR1K*ZN0DC)2O#3U*l5S9VjYMxTBEX3G&5MV)P#Z96v6$S|&nUMQJD;M%&f^eh{q<46eMGh3r3~|7keh5K%hqXtEvmg|sPGP)C|iT;S?8fg+q%7IonO1uJ?r6-yDyf+L3)7%WHxNGGpCjzfNS*H1<ED<3;sKgORiDm=*A%5Lo3{o5@1B542xl~L46m}>MPt;6X%D`*kV{pG2aJ%1!dqeOeJPcA=vr@UhYN-l6&@1Oc7Si1e0$18Rs~W9P^Px!IMm9Gs&oHqcz1WK-@;3HnU3E>CVwa9#o(1i*s|d>y^PRcawki=nyFeZ!FPO8Fspq8^!QNeQVa6de0*_ed9e;)C>&4I?2Q}vuo#=J_xHu4z@G*pVVxs;BwQdMwfu6L>ZA5T`JAAbQuN@X43B?+j}d<ssbY>y}p{aE*N=%IxSnkH|)#~4{`Dco+UlM04=U(P+2r&6D&_&qE&ufJ;15Dac%LzT7vD(&}A#QHTsIpY_h_|$lgl_#UKX49uK7Pth)7X4_~AAfO-y{G(dMk&w|hA^fanp-POg_bNQSbPe$JrlgrR-?ov>ITx=!H(Nphn)5aLI8Vwk9LaZ)Kx}UlP{YMVCrF(g2_0*+m20S_i9Nkfi<#X}~ekHqa$-Z1Q7ax!tS2Cf`L(8MpiUUQCce@?47VMX%Zn)hj@Gl#)P6=v;^@W?S+-~Ce@QRnKvGdG{joH`Ps=;H~GQ&dY4|EF;*0uTOb~!r|g?TZYLJzwy<K`Q6cu@`Oa>oM!%y36GW7`R|O!~GA!)YjvK_@tT--Ol^aqA}iTWH*+VG&@`jeiOkv)u7Wp+<{sa0q4<oUQ~S<#dxaU?>>+R`MImX=X&%oEcOQ6T8z{jow|pc#Tbgn7;^92{q(H-6MPrUor!X&wTJbxRhUMJu4(zJ;&E(#9T`^lSTRJdiHEOm-0fO?Y^%!_e2N{LJ=hjGhYEtU)+VBZ^9DuP6fqP4p7*QWU4u4?080@%QVdhDf2VlJ+f=WXZE^pp6g(RGF`*X#%1wXV;y2{a(26@hpWZ3BBT29&9fB^xy#)<9Q<5(g!e`6_IdkTz$|V~JO%);p)&udGj9hfs{KjJ8*AWG=mx7Ay)&Hm5r8g~=d%3Ksq9(wCWB7*NGg+etiE#jLX7oLK!hU0(`I~hm|nYB^)5jX79-AX&T7XFewl|45YaBf8G&Z(es*JQavP;vv7D0PYVPh2sLbMiIkWIWP@JOLXPV{c=y#tPpJUvFJ3P#{?*{9|l|>KgGciLA$-%5`=oI3$>2(4iNWKiZ7dLL_-sp3tpbAktu)P)x!f5?yNu&lvYRdVK>h4775cGKiSKYlsM(nRKN!g>CoQ|?QtSO>KB-ZVH=29eLZ5{wtaP=sUfg>?1Q-%sPhR=8))HWS&%D*x<2u450z<E@YmIOkq89^SEWIN1wDd2qp5<?5H#;BVfjoFObZC||L+W+18J34hn%+kdzHTOckJJlHrj8co>a4-4ml$hqm?OKaF%3~g$AuGZB5Ob6aq;$J$GO@uuo+6kU?!=#&;aqMtBV@ft#-=Zllp0BZ;JnMrJt!J$1TAGWoYd$vUA{n?<T>P%G5NH(`}A)9FiU2tU&)9Vsa$S;XeQ`D0ZfcLWQV#(_&|Bqd%rXFeQ!H}30jKC(s$P{ntZ@=@iN*+==ti#0QSX6dP=6<xDE3q_jH`v>gzH3o-r^C3-6F9J`@jJjcK$ilYr%A9NSoT1*+QV?`;IvGCpU2otw5ppY&vCabh}4GbfAO^vB9w9esSCJEQJ5y0Pj*0XXR5y{@lkuUFpRZ7lo3Eg#ej2MnsiWP3V7r!Ef1m?f{4IhhvZ@zFHI4GJ7^PCho~)$YT*C8ZwFvAA|d-k?K>);xw|ZsVMF*O3AI;sibnuX$_k6~isfEc+Gh7o$Z)yBy0-jzE#r$wt&!>vnhyUE9h|RiWFvWmpX$HBg*8t#AsBF1Z+buzlV81}!aHPiNI+9_;k_J$;vYIC^^zpC+Ajux!NtD5$2uXL&7$vC~`b=Kv~}2)#_JyH6An3D<PfllSE%ALpX)1Ng^btVe1)BhHxT5~#T9$;WpuK|rG13K$XyV_%72*RDMYKljNqw=~?-C?wpG1V|MJ2J=M?o%A8m3<JAr9$@!3cnThC@?bBP&HZnkCZw_v?>9HHm#;}G00Q$Lj4r5?a3J0PYjVn5mOHhU?zC-?Sp|EAJ1#vS&E2{AcHV;j(TXGH=0WJn<gH>)-*&=ew(vzQ{brW|%sc}<jZlQm4M1EDO3idb9(y*KRJPR_%RK{N0eo`fDD6NxD)0c%@F-#;BmRDgPV5_2$LbRV<AUL+!Knz6g^ELiJa3lhN~-0i(Mtqn!~BRb(Yi&~caKOvR1xQY=}q2fbK%Df8_-KKV%!aaJTjwNX}zF*BNL~R0qE<~#B*qp#b`_hP1X%h#{--*nK9kqL69H4Y^#$VWe$%319$`%9JN<MTNJUU-a2(DZxG%ris*`wxyV5axyg57`zJj~?^@6Yfe#0<QDuc^$`4P=e2s@kXr~8W+@Vdn91+l+_f^C(z|XX0tK)@A${3U*R+5h591ilj1RY<>Uu-!k_%H2225d0gNp}~*hd`ba%RaZ@?oX!Qw3AqEgQ&$7Pl_fKAY)Kn3nDh$UOe*wsqIMn7lQ3P>T`d~pBSGf0OXlKWTbOD#%J1?%zo|1_yG1;UUuD%=YU9S+YT@lX#$4cc3O9+=Wj*;jO`p-)c^U05%2rh84W=#nD5Lj&sbyGkQ2_FU9Z@9bu%u47y4Q-^KKr}bv({Tm!|LgdF|~%i`(_!wK6Z`dF0XF<v5Sy>SKEi-Sv;wA0JO<p)*LtAyN21<{C5;EX(B7b0C%LO;(d4EW}@M;yqXlonQ{iCS#%V?Gf4Nn`qV0r!_}6-FnZ*PBV%c0WFJpx8-IUh&G721}E-Uk$}T*W^7(HBLmQ*w(zIBOb8atXiKn9*t;;_OugB5mYXNXyi;I4z&_GQ<C+aq^mL@jt34v@?VjdDV%4%WaWIA(rE~(%)$vAm?V8~lkHwQ~`=F=TRFR%)4iiPWJDDcsVPF?Ft>ni&EDXK~$e(yaTHM{)jaghx<cAk(b~yYwPfNu<GTiZ6=Yt8FX51k`wfjExRNI7W#yMR%4S8oU704YVe`gU|K6Mp_JB@F~wI=zYkT6})7mj-@2W>MRIU}Qb7y-ODC$lXTg9J6@?he0~bGI(#T{AhuSHDue#{LE!&FUA{o9om0zg&YF=x+84R?PUp8c@F(`S3uK2ClRMCL7!<qoPeeAWz{cptJ+s9j!UE!eN%BcHfLd@-krmP$Rf3H=B@?>m>?g*`z?hqhl6W$|lAmx;A=5L!eXB7$g@bo!N~+BjKGdK;dFo%AOhAvN{nFq$q?20Cw9=SHLf}YA(v!uTk991*s)jjqXOQGr4h#7I@Dka@6MFi4l!X4ZsCU^E9YexIf?{aI@_6<N+A&N~8@^4XP7NhUM1|hlgr6`*QT(kij+PkDiz21{U^=<YkS1Ra5owoYm9_!KgbxyEA$&<(Vge@Zm|V*i;V`gE<2yo<c1hFK~#%R{&iw*^K!FF3;LA_Z#MWNYCdEQ&InKj6iUzmBc2a{61-#h8#(+{_(5Zv9J!rT1OrEk#~$~2Ou?Q!zAA?MepXFVT2XA_`p}wn*>(_&DZqs(kf5SvcX3{41r0X(bpT(9bsKBv{O%ynU-_D1?^DYen`nSDk-NnX#?+_&YI!taG93i?VWJh&9r;5sZb=Y8?zYduHD?<TLB4Gw$iRSnUlCbu6>;Olqo8^f#VYD{sb2e&JLXMK7jR6Ef`>CwxyrFn*vFDxELtYw#Yp9c5~bQ0kK~^?~;e=rfi^3%TBvPSqIZp*2qSBrgsbZH*9+`r<lhElaJgyQ8uwi!{Z8;7a4B_wNg;y&fl4mL^lFDkPrg+TPSd<n?s%n1U(V&4O}|uNF9syz(JUkBUFziVic0W(gpOv+quX69C>%MV5i#R5t>e#!?^%uiVn$hVV=#nk-4No4`+1OdrY5mR~(nQ;Gu0XR0JQhy2#zf4A{dRNz0ovn0LMPpL;iIpqJh6Tw>&0G_xBIA7rXE$a~kD9Ib{T%%)~pLPz>~OTaVCPFtNa8XnahNx|W~Hu?T;YNp~r3KR=yZeVk0`g4bYTi@zk|JzA29Q0Cja**=vP$zOFn$GV{pt2Y3j{;mBNx6KZf8QVJgNo<QWZL8~#KF;mjEyhSE)JS$R_>YvR5wJ_HF_Z&1v<v@KcQ=e$NE6$MN!Pn6g^rhh6%)cc%{?ad8qL4AO{G~$?)KiqayEcUjY1Trq8%=cz1t}!S#|-YMV|fqsCZM$OlY@)7>>!4P>hBvm?Y~Zy`yrgKtiYW-le^dbs(Lr&|y_g(=tVO|NuqJB8kIR?}e1iE*L?h?c~PYM|paY132sU|*R$YhH3F6aZK^^G134M85MJAPzgn6;*lR6STZjoC9gSdXyLZ7nn4L!iF?9CsI=2u9)d=Zc|ftEhSvCHPWF*vPbI1%)vHtkta42s)v)W0b;MFsKuRY&Q{wMj8e<$=|W##p**O`<>2&>uGH%8)@Y#3Fk!mKE~ahC8sxxy1i|tW1Nm;s)gU?RIDF%V?xRrF<sLKF?zX<{ntpWEb;Ic0e@p&kmA}x(^ZlaClbyO^Foc2{ncQ(rbi@`w_w5@*k7H@uCrlAR6a~2os9+RW1>>GhCWIUh3QW}~W^=tT72WMWMzaSM3`j=<rib+-C#1G7W|-#IpvI3FOy8%aKP(ivDZmitv&&9UT<PbrwT%FAe|AL@FyCHXF=5Z()9^HBWeAhr1i`0AKf7T>=Y1_#WRSPefW%Mf(W3^p#IkqVkwH-y76JWD30MAE_McP#jIs-GQ7E*a36c*Qogd-nh2IwrRp`Z~L^pd(U;>YjgX5e^mwmtHkuK}{MFt-i`J#d}mz?L(z*7VE0w0GHlQ7AysdbN#M}5-;Vtsv>7aesS)x!|f^={5^iXlo1OAs%wK_|0*b#_Ekg&Q(scJNn&#&Q=U*Ze1)T6Ck|Q{$ayi9QcsYq?{cbhm*7&v^bClo!1sIr>hbDif)Y`ZY!O&TInM$}y%gN?-0=1?v@g*{F-<{FiZwU+p2zrI%=9&*qVERF+-G$!4c=<z_XZ>>5)vsz;7^hO(3IF&@w1R}Z?}a7&YkA;?`gyd<|olo@&0h|!v_7DP)a$koaBR;O=bG+}Z7DrtFAJkt~y;1bQ79P3hoUezV$Y<iCz9q`dUZ+5wR@};HBa5O!v;%S{51MRod0rpAX@h1P{*vbx`pAdY9;I=Bz_Q~3|s~sP3*kAN9AA0Sh9tysXIp}GAM2N4*{`dqK+t({yFWO_?=a!Cd*OETU$CT|I`S?H6^`8qZ75n4;nf|4YpY_!*KmXX%6R+YSwa%2eGkr1-GwH)V`S$`~#C_8|yKM)7?4zDC!q10nhZ|`RIf-+>p2zVMndp5U<JibDm_CZ9x>{7MNQ=Z&uxuUfMkJ4EqeITjrT~7OlP@ji;zlo5aJaAS9x-!sl2=z!Vh)rm2IWQ%<RE`G-Cew{$7GzqWLjni_GkpVd`xUI0MG}iMmKfT(2QxD;BLTHA{bGxPd9}+Os!Fy3)mgS9<%(nJBzsnJ93cB*7Y88dq0SeE<J>X7{}nrdpL2JzIr!bxrpN;Wpo4^3TG3U0x<nm`f$n02*Jpv8ogv{jA%A5IqL39(8gz1KsoX*!>@P!(*)LEd6;0Qh^nd+u2v-e80(h!#2i%Fv25Pg^)~WhafpB>PL?f;!XAuS+w5*#&D;qxlaicqbsDSs-qtsU$Vn{sXMjCvhWFdUptA>Z_PwWvJd#1{wcN#nexF`<S|ZQ7kVFJIRbXdt>GjbWXD16Y_Z0}!Vp5-(x4Ox@KHGl4+;lB_j6l(0deYRW0E?KXebnt7J-@pARZ7wwo{)9y$B8Gm?X-Qb8zy&yXKSm+wtS&l^PiJT|Cm&5?fAziUe~|=7z}y?SA%E#kLku=jeh1KPWPnGHRuIg;QVUH&mKGF-1R+JI7bL|m$%c`2N<v!h#23fhb!bYh@fwTYN}s+>j9zpqyI0av;Kv!y8O{v7_<W!j?bII&tl^9TOv=|JTk_)8h3_5$G7+>xGN-Zc^-zd1cKXSOb9T=9E-Ar(-lbbvTSS2`d*E0;kHes$b$n59VE_8tL#DEMg**$2NV<lsTt<OscB0X5$AX9eKP(gyXc&&$tc6R-mu76DfnAX19GByAF61YgpPfFqaJ4;LU+0Df0Ky*tG+zbt{~ebxirqR*X#Q{p5yUt{RhF)@h`eS+S|2$d+c(2WIdL>D<YY%@$<@1MCOV*Yb&dAnd*YP?$&m+vDqy)n!xsXGP@IfO-A@Q_g_ttah~JoGDF*Z=$+ePMhw{<{ajugQyE{|PZR6#{VG2{kMFfTe&C6<@9jO$@i@<Yf4|c9l8yS@k8f#z&41QoZ^xMDKJynJOt|3bo`vQTa-cloOt|K4nGN+Ao^<-6D*u{JA3rT$L)>GY$6uV8{55H9`RaIG@1F+(`zE&dgFEAHp3OIH#yu9?^&p>vd=HxU@URvixLR3<0z}|*_(^w@ZRCaRuPlLbyVAD2_qk{D^U0I^9Os-DM-!h36xg}7@3azrkabeT@O+H=T+T-#tCi+5=Yt7EQb1hzF`H}=<$!%zyIZR8w70Ffhpn4YZpxcqrHc9yeQkKv=Q1cAuacTtjV~p}-R<nxXoe~$uS8udn3N)Aq8`y4$+dLQ-VeLJHn&#CC%_K&{kW1G7fD^c6udx&Pq}+~Y!68Vuu5^1%{bvfO~s7XYuU&G>4w)es#F0{%jF%p!*VBZ*J#jRJ?$5a3Jzd}2SUfTZOt>u@b$8S^xYkr&?_+OwYYUmA@KU3Iyw9L7#oCR=J%XMuDzMfHz!pKou{BW9FQtNhoLKQbtjdxx*SB-`t%2lijXNDr!m!g4aBh%D>Uze?e3dL3Hn$vqu1jPAcdg+bUVWP(?OebH7>JLb<qpk_G_JxV4RF3&?e7Hx}E<8%fe}jjOo*Qkl7p$>9!~DIs5yv03067c}(agbo~4F7|Y&vZ_e`DrC(2GP@c>Ze#%<4PDAG#o{i(7>&nD_r$*$2pG3sN5Uz5qJJbSO-WdEKC-yD7x~<Q*9%{A+4c717Glb8}Xh7Y2l`%!rR>V!qxD^A5$1bOH<33hKFTtEd@t+wEy48SLY&vVlc26uPJ4+pH?`JWihD)NB6VsTMw*CQwC7)StL@PZ4(4!8%>}GuB)#gAj4<Dd8+S1G2Kh)X0705<CNY4~W#b&PGVbIzcM4GN%^%!#}8P*FXt;}udK|btIFD8U8F<!_-B?gEvvy{%*4DJGZ;Sp;F@h#?Sl0V1^$@hTsX2j`YN(+x#$HxsO^2HrU1XP5bHHZzn(?EBBOEEQ}^lpw>LhBjBK1i6Z<!h`-^uM`f$-qIgt;_fKG#}$idBa)`1wn%!P3A2wUo7n~fhelDS$kn%^Tbg%XT*QT7MNzw{gGr1B3VIRp~uV}?`V0}c#E@$hJ8>b2ts8hr^UWq5bY8Hl~qBBTy+yvy$L0UNAbvogBZNc#g-RP!Zj4gq*n)i2tq}MiUdj8fiISEAYW6CAA|x&k@Y5@rIWtCR}&Mw@(Op$^1)nkIEGJ;H6Bm5K}er%o5{SIGo~)2-AxLHm#?1wX3psv43&|-1=`=7QFD3dBk<hoJyg%$KFZX3)Z8+Zx0B~#p3KOEF3n6NJoy5Ih)nbW%|iDS4qySh=k79-;PnJ0u$OZ70~{*WDVG<Lx#{@p4vuJbGL+skfPEWUg9U01Y5`8Y>GuOX9}THy8o~u@<VgZhs-q3DiGy=)y@xjkf^94@s3dS!2F(WH9j;x>F#x`McTjaBXQvCd9zJ3=@Z_O(uKY(RlwHzW>Kge!4;l+fkK!T7s~OZvzz<Cz6SI42z{=M^2<9aUgUi?8*6?;ctk@I7>?e41T2RK{d2&y6izJ4lFE_+B){tUR<+Lqp(873r=~t5R4vL))r&i}FY|Z02>8J5YA3<g6k<Ct&@hrQ!W)md7Fp=2fHMJrGMMq5MW4>pO;l|pZovCTlEc^7iSPMA6#Q<djUR`f9*x3=I{q^MPs<+0>DAOGKG4`PalGc8av(!rr9CRylH33abc!%J~Cy?o*Ww=raBnf)_wc$Cy_TWLBaAlUjI42z^Cz)^}>AI>hS-P1^ta$unF!Pi=mx@Lpsnkq9wq>kY@saLS(I*smn?usG$of7=;!*p8W?(gXS^iiW=4OX`n*qVp66A1^!8bwA;v|})0H}D_)IHJvjV;>1%5?Cz!1<x#GI$~Vk<rV4@TS}`-5*HY`v*C4g~6p5kb--_KuqlUVr;pIUT#X5Kf}6kY?u2=quhhM9paJp_x!qQuzdmbNWOjoTY<!M3Gj>|TI=C9<`jc1ch5Y><m%`LvkQ4UPfkGmF@AdRaiYtzu}h3J`E(lhNbn!Q<GJk#7U|zkI>yn^%h{Ieu^f+MulqQLyw?t(5vbe`3o-+mpv<mWn>9};C>Xg1V?}DObG`t#=xe>^k~V+T>qT~&OAM<DH=BMXG*h`7?6ZMVF|~ME{tV9pZ5m???|9^%pL_=z@rr0(`ePfGyYFMYry<`jWk%p~VL2ysml6}yZ;QdP4(>hH3#cBstSS$%ki5v#)6hry8usKUpDZjd@2khPr68H|q148@$<8<$1}Df}r3&wP8v#+zUje>j|No!0?|`$S$olQB&V9pVhNvJ&6jTHhn6ADAB^Xc<QG!Sk6%+}A5s9K?BuWqzK~OLtsL0%|3o3$H%n@`E1=mCn4Cw1~PW7D`!2R}n@9pomft8u+RHsh(2WWum)K=3OHI%6vM#gh!L$8w1IemPxt;;0$R;Pvo7Cq?BjFtl$enJdQhBavkB|&OP96{T5h>*CYF$lrz2j*l*pG{&T9Q#>}NI;kt$<V4Gcuzr?KfWRc=0uFvM#w0-B;}A+E5e0vj%J}&Jn~m$Q!U(_3^_bHjLMZH9-{`C=p_MuhU83F#pyrTz#wf8U?^eE5(fqgDPO{aMk^%)lxYzm9}5p#6>zE08`4qhPa(-xohU`7w8<hNcOAQZ;O>iVU%DpeN&pQ9k^%HF0M4zW4HJ2JsknPpSrX%eKsjCTp@6u@$g~7NPOxv3T(d}vTKZ_O0N9!YQ2xSA23pHucmxF(80?0z%&^150E`G!RKQMDCKf9U+_i9@fkL+p$zzynIk6-}T*xn=Pzp;9M-Bf@cqb0_(5Rz}4Fj|R(qm{hi<DR*lWHh&h{!DPTMW-E0A+x;fKnWuQ>Z-(l`>Y4_}QaFNKA_}>CK{0*m4Z;55kVEg6RtCG=Xir3Oz+KNk$S7eEk4;!7md>?BF_SQad`%<R}IJFL4)6!MLRYoIpW|YB14*AsqMs120#F)X`Mw-Jw?k3|XmDfo-h>ivJm5k{NFCL>L8F%)-M2jye!A14Z`mbW)OWkPZ)vpBQWel1Mku+z}jkTyTK_JUg_a;iOU%VbF~`2@|_oVZ9XtUVNAkfE^_a%HuFNWQ4#qO|p(>)a(FajbX#0#GFH{J)FF#xn`0>9lGWK2?=}3&@P32g)>>|*Mzk-a33ITQ8b)!n<(rERA-2B5S<dRXW_*-fJQNaCCV_WfO-P5OieOO*OMqE>40+;PN9i=0>_a}ol5j8203c@ii<GC0lGwZoFnN4HVndr(FPAPKO3h;kpAE?M+`Ld4>E`)pfWt*kgoy98(^8?|I{3^qcqX$9mOZYmz99105RO6K0NXStQd3vVs`kc#;~%|ZB`8~#zjFmHLM}R%?+$rg>(yaXeR(Y5FNI|L(#@%6cwiNVNf!_JSqHW!f(A2e5MXq4KQd?;A%8CfN?Ek<*>n^6%D2?DDUn<of3YqunC}b2}LYSV<4?Znh6+ghc7k;B~_Tj!M74a?Hlkzunin2gW4L@g`jDQE@hSqpD5U)Bfix}er5RgRcOJJq$WTlY(421Ik29o$3fU0RIjN&EE5yzwUKLPxrR8C28i;Ir9wH%WzhLU3fUr(ld)`~L}wHA?V^?}PF$n$goEiOz8A!8LcR(Jf7nx+LRl;5NNFU@O#n%}7}&><2)a}kX!vqn<SUYz1{lmFm?S|Zfe>WSz(vmkcPw%ggWQwk$#Ep2V{&DnkONT0VUR;M=V7V>uo)eaM`Q!R8RB4U2irFU70G=}G&cncAW(sHH56w>Ef5RKWU>$i=?14j%?Gv6Fx<d|&rXK#(RO1jt~;QMs{$#ISx!7G=9q1Q))v@b+NK$IoxE^onK4j(%qawcxG-LX`7?9Ekl+9x##X@G6Y$yvwo?iGIKzY2j)760T@!Ti0p@B$VuEu4H7u~<j04CBvSx5zf%Z)Btil+L&|BTeK@A|ddr)DA`rb8==NYv)6r{<*c?`}s;ofm|EvcZZGsdy71g(Tk4zi*&X^M?CDpn(8j|@2!er%4CjDgUaNn<=qW_lX43z!N`JshqfTuV14X_;t2Sr(xr=rL&^P4q^10dYff!lcx-fO+o#e?(znNe+fhpmG3UDNw2ayd?yUuxyBh;Ve8^W9V*dfjBPMjl*au+}a5TOl7b+MBH>6)Q!Ru*#<C0c<exLEjKL46KHJ%`yQ%CJR>35$IOC!)AUGu2~huJD*#?52zrNn5LG~J)MvLMxkmu=qpLU|iBa608eA}Nm0FbKQ%^MqbVI@#tP+^mAj{Rpi4T~`nAF3EqY>&a8w_9|dRuyBBrw@%j~fp^yIe56K?6x(@uGbOOeH~`7|l%-q(LRnGfagO6Ou}3)8I5GFPRL>3ReMzx_Dg4iN#`xWWot+hP*^&GwCF&gegl=qQJD0F|A%a$BrxcZX!QCJ1fOwc4hEru;MWgxYLb%sBYXG$4mf=Dm;&qO66oC8BZo+xk@}%r6BGklgUJmQw$=2W_XT=2P~W|2C~lJW<lr{x(XKgI6|3eBV8B;s^S2FgePdhv=QXz!si7z7a;tgGBFaNa6(KSAY|)ctkTi6Fa`igu)_kWN&M7liftg<7E#ZrD1ed`AnZZr>yT;MNS}@2pu@t{1n{E}38A>7n52_nDdF@SoQ61<2_d6gL46yDO1fi^1cX{(IeLz+fNpwNugBmt(~CgN8|3{>NEOVu8E)?!Z~#Z$G0TPPwJj*A6GaI#7r@0~ghCP8d<QxJZjKQiO}1mja-q)$)W&@1cEwy9_*`1}maVWjg-kbM`bluUE*V2)r$KcEq#71dvmJu$fXELzL89+F=*JNCAdaviIl@PqW;8HC#B=xqE+}0<hZ>kGC=JW7A|Ycd#)u%TNmzo4^#{%%QB|8tPRJk?Dzd|k6+_cSU8Zmr`5>Z_MHs8m-4cyis1yU?H-^s;%RBjvK;{GR@2ruvu9LN$EW;pz*}#>6pa9bXU_M4N3kK%u2oqPNWduN#umZ$M4o*59Q(_sye-_uGws8zwH&H=N-hN<S4(%y8l?{R%>1a7_N{ct7pab%rfwY85n)j-Sv>k$IfQg7n>}eFDBANy&w1J2edD0?E(FVITU5=|M&!EIXjoc)&O7%?g)8*V;m7{Kk%ygC70W>DplydB8DXC8bO%`b*F}f`|=nr3p3;hm;5;Xuy2!Ev;9>^x@K~Q06qN<SsrU}qSOhD4_pgOuis!$-53yKOTumLj*@YX}K2pBaWL=?smCcXz~n#1N-Z3oU4$TNTk#iTwRUL1ZSkn8|KH%wXxmWM<cLm@p2RVO_K{K5@ILYx$oK+#bfOC_Ql)vE}C0+}uZg~3;CGJ2Ns-)Myr!5<<`W7+d!(wSP?lu)+{>NH)I00XHs9Qk4^<z5*6m#Id-n$b8JSHOmMQptfHqm=~3j2w^}vXEa$eF-#^2J=4%Gw8?)*lRsJu~`guBNEBOv>H7aOiTs<u*oW6S_#|)0852Y#{~^ZoDRcV0m)c5Ow9?Wj9Mso{W7sN(J&0!6*0&p0rJf>JEqYm1zN`tCZL?fc5wLW9h^ogq!l9Bi3W+HU{@o&!8mqX!>_<q5%oeSkqC+;VLpr`!Jx5hQ9}sD`lT@keYoU7Pdgsp1G1_W8wK!a6eJOK=whN0r60aTMAo4wfI%3<m;@zh*}fQY?o=X*1EE?&$drmuE)sW%wh&$hik*;1W7LIWq|e6C1qQVh2;GDdflyjP*u|azR{I17M5AG}XEjjwf!Om5k7v+cf!|1@XZ+@&?@lKk0kFaafMo)VbP#3IVkTM|Y3Nyu*kLG?O`J#|gqcCZ2BiL7Jq~530!Ck29NOC9cLrySPNF7wkw6wP(K8GvQ<e^f-f#e;)DxB%rh`sJ5CjYWvgp{0wlEyK8VKhaL_tUAnYaQ{rndA#SD`-zatb)Du&}F&_!brFO8~K=7y$5HSrXZ-sFaB81xF#fQ#3$|VfzJVC?=!Q3kRI|Y!sCo*-->WgW3;zMY=S$2i_{C;TGz^>(EF+&H;!*gIObVbddxB9XmK#fjDYn7nkgZ7*#;98Ny5!xYQtL3Vj~PTuo%B85o4H*OSF3sSz?&kHSqBPL;(e$u_)f$dkK@foM0zlrmbM6j)~w$SJFXPQvXhI%=YRvPvoec)2#X)wyuCp)vqCrvtjdL_sZ7^-)nC5FjENhWjuDg6B_FB)G7J=`;?1tT-ywVQ&@Mq+sBTW^~jq1ws1oKcMK4;9!zRBK3Do&-yeWTooUWNk;>!C$OjmtKh~bmnwq(QfF4hQy}t&Vp&%jO=_!RcURiW7^MA$(0h$Ew^WDH8{D)gs*mE2aQrg>6(}@Q))?5N65JZ%n28@4$%LSg7WAh9NCYGXa-fGW0eW-c4#SxusuCJd!@-y~Ov@N@v(RS_;a$i;fJP674Hd%#FvG*YsH;)FB#$eLW|-*P<Y+l6HJ8Y;n$e7rdNY{Op)MX#Rp2Cb3~Eg=G)S<p7m2Rvl=47-s!*kVH1;S6Eft0U;9y%Y<HJKW47LuWU`XS}F(p)yIJ-B=APD=IgjWNEJ%D*~H8t)4J={p3<`ZZIxH_3>Vk<&%fdpu}YFM%Wdrei6pa5#>aaBPbdqiEp2>`?tCFvq*7t|}lI{=*u&CEsZA1J`X&$1GWW1m8IT`;Rd1R?a<9U@#(vZO0givt~dpe_f-6rL_f5{BBI!QF+s2m8nxAvr1KBY+~xxrI+Dp)zQ<0lkV?#af<B@zG&gf*A=&Y004v3aKCsAK2_9)<PQ|#|$_PMGZ2TE+A~#j6w(MlfcM=Bw6sXk*|W>0z7A-RKtM>_~pSsmXT}~n9bq`Xd~_rP`vOEfak`PN{R6C<%B;Gqd81?D+w?x26B4%)g*K$Vdys61nD?KLMl2<x58Z(qe&4`5SZ;UbQpruKqIL%Cet{C<jXM*NsGyYSK8WQ7a5x#Y83k9^oZt&gp<?ByNIBtGBc(;O~-SQ;fX{4AWK)EkbuZj66PWIJT($aQYmAh5Q18P41uDNla*A1(B)i8T4?ClmWJX~qrNm9HP{ul5on;naRTNHl%T>7Xy5_`$POsCL8li)9pum*@Sng4q!Js9UNzU%$wSDZk`W|fKr|g|OH9TA=wt#O4_#R)Uj-nWWs}|=NarC~fFUUbga|-%6C)^6E5cw8ryRL~XgFd9&;US|RU9~hLp6LSAZVsjng;4wG-n`!vkG$EFerk(rou)Bkyhr|sPGL3g01A)kXFFOzyttsjaD3*kqlgBkkpBW=)ecgHE`k*=EN>Q)eR?%i!meNAo~W9J*49VxQ6h7hE+aPVK}+~1xt9XE_OT(&^J?!H~?!w$pq?kl@2#xSwq*~qK-KlaWW8gm42TZ@WL@J!gEGp6Kw0+#N$DA9vG*<xrUnY2AJb8{C7FoXM+*46Jaw<iJZ(Z^T682AhSlCPr;NCews-SNE~fqBtg|c%&JY2#=|lZzIn=5F(Bdh3|89~p3#_Wsgi*JZ4n2~2o;-^5V7NA|DxfCMOzThCzQc4@x;TfC4NOz1RCM#80G^qg=7wd9Q-)K1f#l!T{_03jU!G_O^luz>0_x#qQlgjiawa+V`~7CO#{y_DaJw>5pp*$Wa2k$B7hf9cR0hasK9-Qatq|=3)PPnCZZOKuVU!M_-}#aG-3i3R2&456RPf@+PVT>SvF2dZ9o;m;xQBKrQnr2<jRRUW(>NZ7C{(LmsvyUe%p>A`T=$NAj#Iz-VF0cbSnkMqM?w~r-^cY7^uVV*9Fcmu*#sHYmlC^LqtFvd!e4U1zbaP{(@2vQV*O_fYH6FN=^nKkx=mucPK)8Pz^^Rg+M4fP{&dFp)-xNCjm+Yu@gPaDFed{O%}ZH{lgDf)Y@Ulgfv_o1=8^QMsae|Xf%+fMX!!!<+8JlYb6q_k}VNEIt?_bIVb4}DrJa~7M+@9cWGIhA1nm%<W%GohH%<Ak+y-|kJAVX12_DvV;TeiQ&9Z}Mh?{13_s2|^`#-{fmAdBWMFt;&=f;SANc-FihZDE1i=n=Tnx@S#4m-{T%=>_V4`Y)iozBs>j<Aa0*&!phr-nbinvC))kuQ|-ZunO+)tGiL|7sCIDG63hrA%<2^$JV)jI0xF_dVB`vL4!xG^E1113oU08m(AIS8nZJD^DIVu!|d3<S)?AzxC=SXCGzTb3GgK?~LaP@*2ghJyp3R0VCC!*Cg{?Z(0#m+Ryv699P8Y$q%XRU6DLaIB)kpd!3mfciUBd{G^u5<LWR02F#htO}xRK%f@$A5gKy7-)c7f=;5DEW&OmlBbE`4L)!Uk@(o$gAE0ZM-)-TDI5q8qIf^eK`eqF5Izrk0U$>PEiCNvD5#%93@3(?5KM|FYAD|%nY=g_Vx;U3U#CuG7wX7CZ440`F!7r7vP2rbL!J*hig7r|axz`yfZ<_`%2+D)0Z?pIK|vL2uv4oT0Vg;U(NU3<0gMMU$dOD67O@(t%pv?7y&iScX0lO<n32(LHvg0oGAcG00MDoryaUznQLl;&7jgGV`(Mm9wJc^#K@S2+;?W{!F*L~2BKa!Re}W&h6M(D3&EsIFfm&832<xecT(O}dM8dFvohlP!JJ7=woYa7Y44P*s)PaQIL3n_N-#IJ^2srfgh50%}P^igu==Q^dh=a17WxEOz5&!_0RDl+9id7J|z|7EgV_H0}Br74#1>7?A0!XDa3x<)gm=XsE89T1$frLhan$!T1cOfr}0ct1_e(nhi)W>WMeG6gGXsZbTUqYV^Y?`rAfI~1W)@&3SM7dHD?Sq5~K-*Yu=~$YF4<)>}I2`mk$&g`eKqYwG6wvJfgqZ^u5pkH2_=C!kjLJulm_q{<Y=Ph(01u1@J*o_%Qa|LRKu$w<Z}@&JtaqS*#73)zD`=GbX+%h{!cxXXX$o*~!LJBJW5EMeivdzXbt&;AFdCVKGYf^fDdoZbpN{-Ui`0}g+1m#NTU1TNl2k=Hph`mp>=tRLg9)D+Luw~f8d(9*@G|32)uVovjj<E6AQYU%0|b<Xn6yeDoRH>VMPuX4M8~=qzBz>IFhlA@!n<cuVG%2?F!90JB|@hWg9e`%%$Z{@_7IWarbR}1c)V#WqWsX>jGiyWNzA9Bo;B=9iZH59EgfRyqllOx4^aYOGRa?xdeGF*)?`=OA}v2?K$;S<Kw^NXvw(3{#q<{<0~YnzA;@JU83b+#peAl&c+rUkZUELsqcDjwdL~o=KvIS<k0lidOPC5uBVf@Cjt}V2q02mga6W`CP-C(|2Ecxm<an`o46oTnBTXcmN-bI)!x`G6V8sWVXp}{O%TqzQ4M~3nlN7C}Q%{UX@-{>l4ON#EQ%Y5ln~hh8`-hafNl;6Ztyt_uktq*$@5{Jb)Xq{R5gsoDQfO)&>8PNA4c+k9B;dTjluM$RL1hoZDynvezXTMWkvgO()UKiaJB*kjC1YeXqeLgZ7u4Z0ky?lX)Tj-N6h;dST2#_Ahl~NLcTiY_APWZ$2~t@o!WaX_2}%z@I)N(odafxAMl_Ttg$6sO*idU}VJ&2$DJ1})K%Npd14MraM4C|y^_2^x^)NAjECmFhAdne0l{#n@0Mi<5(t-e?i^_q>{Xm+6o`d%e8x8LbPyl8O+4?%*6d{E|(1jwqD5QSH3Rxt#M&lht4vr-lh$&f^EXz)LD37F<uAv2rDjT&-${J*iFm*WviaukRc1pmJ8V%)0eO}F~CE!>q3*==iYE(vU*cvOFS20b-iHHYC@Fa4|R8+Ifo3lblp&P<|oC8EAlwLx&*PJjw#o{(p7CN}$X?l1HYIzQ1b`BH_wiU<8fNtfOj+PhxlVXT&mg8!GVMhrxGY&@cI$->A0XpV{XHbq|<SHh4;h6=Xbpvx$0N0b`J<%GnSBu07I1`}nsex-N@V-ql48=r|b~y|aH4+$u-Ws&|NsZH_NTg6&WJD(irsz_~hbn5o;Ujnu1_lVavdNhSLBC;<k32h^g6lCFE;Gcc8dN#k0F`4js$+;PFy&YgGV$=4#WdObwV_`RvTD@Xq`C%A=ma!BOmw<Un52ws;8WI2?4ux|Lz9Zj071m7#7Y;MlRA1cV9Yg0QQU==qe%}ID-nk2mhAsRtq5JIobUuT=$n#b2C0QgWNfVcpnC%y&8%5_)VCtp5VD3PKC0B(R<j!bomrF!QHL#j5-XJWbNrBPWN$=bZZM(IaY74KH-ZIpkkKTu0{HA8Vo|7rb}@Mo8<+qAUqh2HY#j~RRDAhxz^FC@N+Z5E)T)PZDPlZHrAOCD6&<%G8ZBiR@RF_|vG;Q3NLr3e>hfT!LYb0$7Vr}mc3J53Ac79L-Qw6+#Y0_>d~#)Xk}28qBAP#EZKnvP$cQu?u_>hVkdmD%XXn>ycI01f{x641kx@jrupD)`VZ=psD!^b_;oTKGXjBG;1Ta$!CjlzMAnojcA-HLV#T$#o!UF(WWS}<&tu-g6yJ7ANIv~1h#6T<oK6c;&B@H0nCSZK+n!p4Ow?Rx##OMe|(yflY6KQHgPZb?ku=!!glaaOrLNNe&r%@kBlX{z~RBqJ}gGFBh4nC-YL}?u|ejtrQ1qX|97boILKsrM>gc+WO#xMqozJSSVSY?1Ugb_ExL(B|wBK%S<=!l@rkrIQ$OApIR9o@|fq1J;Ts1bK@+Jat-F5+^bN$wcfPA3AJ8X*g?Ev8V~uA|umsv;mf3jP3C!APPbOAj|B7mb=@Y9w#NDAU27vl{6bAxhMgd@jg7nI*M&=zPOHPc@z7zMD+S^k5-JI#<z?mWuV*Cn89M9`V+h#2-=)POWxEdjm5w{FkcY%w8uI1biW=5e;@4=toCF`b00&fxxGrmAzp~yDey+lXH%4VQsJCnQr3fRV8&JQ)P0JLDV8FfdG1j<qv{CVJ4?hp=w}j&Y+ecTJ!1Bj0N*Olb#AxtbiR?0g((WN+8eUzyOYY6JLwX2%&}lVZf~-s}DW{Ot}>zp`v;b&4LM!r701@pO6#SNQihg7@-Y4N?0)n>skGJva-*jnK9#(ib+&gMqM%*QmTlpVJfIA<IodFWZ{#s<Kf9~gQO!vbMbHqXnR7t#MR*Yr)$^IFAZ3iRKA1gvW4F;aHwd$1*H^p%2gey3eYJd9FPWH$<zU(nF5P4l}Jm8%9StxF;sm)tr1nKd618ZqEQixpPD8)Y8nm0b&1ANnS(bTNjd1`w}VY)K<A;#G+HN^sGNq=;5f~B(7X>l0RVjmk;cI$gk%~hEUe<8Q-qWQx?h{@Vpt9$=OD?kQ3F6hCkqS185|#g#kQ&OkD^<V;zML{UJm~yYeQC6&A|kPMp>-fgkR0V*$8nioiWt1^GE(B+y{uQLqBj55`n3bLIpYW^As@uu?Wr@hlvp$j#>_BkcEFKjF})0q9rX1*`{<o0ku|Ou^U)jC9zX)z-I&LJy`terd~)X6gJ;fO24U=!u~f76m&#X;PhIh$fXiWn?4SOI;RQ|3F*An^|B0IB_0PLGgK*sf!1i~CW7-7zy@nH>_BT7sj00<nQoHa*5$;HQEvkM5)o#M@fXD|VAELCu0+HX^|r8*Kn-|s&{6QL&`}2*&6yxXn5wh@rCA`Bos8;M6eh%eJ_%AQngn9X;#MIaOh!IK<d`dGAtcad8qwRv^8gfJOeC5}ttw5GdIdQDbgA5?QL5CZVdc1l_zNHqV8Fonwl392@EsVa6e6>xh#9I%%@iXN8;sQ2H2VJV9ce7g8bG@$%s*mKC;}N>abOe=&WGSM4IKOMS=k8)$cClI6P6Pmx-NM87)BC$Fi55cfIwJdfH<aQC2g~ki(RC!ip~pvyA4QmC^zw!LDrqnK>Zs<P|5cTW_TJ^;Tlmb42-q-jy3W|!8Vym-#<iW4HH*`^rtWoLJDWl^MMX1HvjSAqDh{P4F(!sn{arcPC}u@5;62!PM{jRYa-neZp|@hnj2;)VPzct$~?0Gcy{qHs<?*XBsC4%_Sg>wXng|Y6yc{W7o5D{`2kqlc3?~q9(r*L(1T%40`WR->n5esq$vq4X6)dBroL_=?t=kBFa~qd@O|Xz*n}pU8jOL!U4nr&sgB03ng&@O?g>cQk)RE;2*eemA)ASmAiyk=0WJtaqP>bsr?M{XN?<?~!>A3TYM}ctC<QDP@pCGr{MdGb3L|DA`4q+q2sDR;05T;ManfM6uA)?hDIvd_#LQsEsG^*oLM?oZ3>pRuc-sbgb0Hv4!`UNg5n{+e*fXQyfL7*~B;C7+3Jus73`lTdn1o2wsDZ9#nbA@|L&a^XF_YS13TYTJ!~)`64t&Qj7?>nINS!HYi-w;*Rs1wplbIXBIaFB%fGH46z~SXW9T`((v!9`uq!QQ3q|LB3@|v_kLE9m`zma=7mJBZNiqIbx+h`EXOtifQiC%PyKocmH#NHuL(5uj~8wGeU<g;QJ1BGc2(US1!GQe9TFR5pd9eA=w5DT@egNgcZsutCsOm<$grzQ<qY4As?TC)CBWikhSf&%yfiiV&;h=Ym*OXkPIfRh_O;!0W`@(}Xz#7riz^n*lRFAD!Q^x#0~UMWV$Ax0y4MR#RG%nYLr5Xs?nxo9pHD+;p-*nV-8ZBcUJD)K02C;3suPK2+^aa1EWT(K~`3b^<@yy<uX5|uC#IEt1}q%>QLh5w^S-nR7mfuamuSRnyRQWSR(z^@mCxhFWZCR`*f=fRH^P(9!PWkgY9KrsdPzwpM?VmptrCWH+n;%E$?13ZaB9CIrm?+y1aJLNR$v6Z-%6Fwc_?JIf6qX6C@NF9VfX2f#B1!71I$(2Y*%t3TyJee+NrG(X3+-7PLc1@CnR}sFkB%n61`y5sQIaYWYxY#UFtfXYbDp3oT?8Ge0$Kkc2xr!2Acz8RG3)CjCD}nzf{9i7rgofWSSmvs(i-)?RCBk|%2ae!e20zAh@W60D6bJkOa>B5bL}`~Wyd`PiAje->@dS``3C}v0Ot2M}2MQ*NfjAcAYKH5FAub1T9st9Gzv(!Hdeal(Udw@N3Cn8O4R(0+0g4)w)EzAkLU?!#y6~oxlJ}?*dRqd{)$n@Z!MG~$Qo_U|Ph&agkic}}_hLH&ebf+i1n5uLqXfx9>MX`H68Tf9pMl*MsxM^Lg76UxSkh)qW(Io1K9Gi$OlG)bb4|!r)HWqt9H!eiS437jqv?hwn|?&2Kp#A!CZNp*_0^edjKK6*)gmAkwWB2FOO3t_CDK}x>#Gz)bXuRHc=`2lW}#%|EikQ?=Lqp))CiY=_n%!~FTcK~s#$FUqRC4Hs-;Gi8>TBNR=%1v_pt%{UxrR2*bbr3$rXX5ZIP<SRI=-clQ*XVF52=skpK-@CkrcRhx(uzMJiTteFNDB<f%#PyAew!yT0r+zy>l<h77LHMPF0}6DuO`0c1hWqlg{=>&Fa$1mXG?ft)I?k8B6BOOoq5Mp;#py?+!#MY)=2eJ8tqJX#-~FkL(=cmIY{VSR&)2k8;J)H!30>L#p&<^mSNR+2dUPGX8+#^^%;iZ!&x0NsrpF4)0<645cmV~$0$23?6-Z;UEG*xg~6gAd8ZZY}=B1U*)pwryBds+Q@f7!1PP5R*n^@+zk5U?{O^sI6eohM1L5NR^U|<bd@~St~YB=%E>;QDfOC=LJh(#~f}aSBbDhprnPa1mRzcxe!6{*63FO0^WrJE#|<{Pn{+gi;x`ZoniwBEJMQ!h~W%da;aVH$pT0hG~Zl!jJ8E*F7=i%wn*fU<Ay1ILa}XddV%;KQ%$^zI11{aL6m`V)-jW4ewKq9)r>?yP_n^PB57uq1}g^q4;wk=AeR7MR}je%`vd<&*2yrkf}b{V0tG)GQ!VtAv7rYAZ9T|I0n!!Iaqggk1HD29^s4-nsSk}omPj^m+`1@U0J0{~iGt8&L*pFs0T)jy8=)r-UWnsh!wK393<GXh;ee%$5(6F`t`_FV)YgU`2iUV=STv{+hZCSU{z51Yz@0fxt?Mu?0V}r%2!%Xns09raO2NJ+#y?8aBZ39~seJq=D2EztNck42Q5)5IL~MuxQCS;9l47X$LBK?E+QG70#$UB8{%aA5K_xFz$^-Fp&sw-KLZ!?2OX+wtRa2F0{FnMeku2u_9RHQ_@<uZL)2)=ne<Fsla`9hNkB|RqcBIJA2Z}m)5rmOCtJr}w36qJXnO-&-i1bB?HL9U4wEWUcLO%fD0Q*)N`bb9uO$>y(O7|7%JJaAmued%=;Zbr%WrSHMn~9P{I=wRDUN9rd_{!FI;jn}Wu%SkGB<-?keU$tm$$@`dA7w67?En6E>#OC~m;6x!QCShM64z(t*GH>wdG$^Ln~1qo2bhIwBG1h?3P_D72iMM}?p&NIlR`iSnZO=Uk`TR(I5dEj0!XW%x{o>3HIYIBShcBPtpS9P{O}kAuQew+T2&<{j*&l(3O!-10f3-{mXEOq6E+C2_@#sIqGmV+riS2UIBy|rvp61&Nf_V5IL)yjp|mg{TJ+%<s#XArkC~^y$rh9G7r)5-@D>tGjXMe)b5IkRZbB=>0|Nawm1z5gKN*ITL_t{2Sk(O@ue%stc><`jKo)WwOG^-MkWTG5^z#ZqlF`fuw>VheQL``)3x5?3X2YAz%M0^_JeG?B{P0r4>H+fa@Fg&alZu^dxVDu8dLAI%WuR4O_{HXsaT+ocF(eRrHq4GOGtGs|u;QRU2!)PKqk!<60-rD!+X5Ira`O;no1;^E7Q6xR{MX~*Q5l9l$I1Z-lb8zqCl{^RuptXi6y`A)coi}}#^X6gKD_X$=;2ob;3=qpBH=7OeE(_=7He*_epnsnS&_RdPDvEdYSH5$g{0>yF3_4;!eDV=rfcBJ16D7vpP*9@^Y?H^*x{Pt)x=cDQo>pyENUU+vExQARDCoUqEwk-Q;XuiN^BrHKrv2{9*Q1~5KRh4I1fKb{Ff}qa`D&5wzFmStIGHvS;u21Lq{kV?HgKOl4_;GmlmC>5pOz*|GGlt;V6s1|A*Q+<$pknK6s5_1Zvs+tAtF9;y?CYjO_W7&Ht&JlLc34TBI8zDLfV9Kg&w7mF55PKwy#3d1PIUwE%HjOqYQxfGr7gN*PZ*(tooMV}!g68ZH=QFKVEerHX<;1_C+D@_+EnwTTa=5nor$;!JF;=8$9^CJ3-Tlk2NS+4|YXr`j|QEL-0!TOYwYw0^V=XgLN55{B?_4O22T>J(R4A2=$Y1IrxD;{JJk?WF6+QQb_J`n1QduW7ix9<6U<tK5PJ0SRore9CzR3D~xvr^E6=Vb&?gcfv2@l<<f1;4j1>W+wn>tC$HJXWWc}7+kPHfbsBZ@)T1)!%^xhv3!~hRsk(_i1Thvc<FJ5^;o`=q}wcTr7)Kp$FcxoccB6%xZ(QYZ?5j<g5?yr^(NzzwPCq-EE2_&&^wUdR<UBp(QT`VrZfyoe-uzK3&M!$0%ABG9u{C*3YT1na$e!%&CiLRavc8+8BGBU&kdz0e6Y$XZhjbq!@?k`<iun`5>p^1DRxAAM_)T7Ps@i$;)Vx86I-d5wBgYvRjbGaR7dz+!#f1LuS5A-95<PnLv(H>$FSp|L*_u!#B}pbw;&AlEUz#;ff5L}&yCqJ*EVusP8YWwV)P^OOd&Z8UC9l%oeMk-Em19eO0WwHlwuHz2EQT*gW*4hjS=2L47EY6TnjTfhI|%a2||%$I)#>9kg&ryU7!@$Nd;U(3Sv%7gDNpxz>VYN7BnUB1aYbAGpXZlCiQqV#&V!9SV_-CD>xE~hKFVY!X+0|`f!0{OcvU>BVrmrC>553c7KkexUmA3bdu06RFY7gfe3o|ixO}Yhc$vr(Slk$WI=1?K&cR~8iO{X>K0iEl25gftpNgOC<%aa3dkv^@qhXJTly}P^<Sd8TBU|PwVo-p#`wpw{TIDl|Al4yKa50awFn5Llb8S_Fuf-GKfqp5rM(bA;T1|6%lbbWLiHuu7Q?t|MoAy3MMO*yA`Qz<qau6C*!@>YL>9$ifJ~uL|B1+t)PEwCH<^VahKr69;#5kFO-;7nE37Y>S=m9Y9tClx(VR_1$gG0h5==xTw3a4#rqqDIhvMi|KVmYWATI;GlIheTvnI%$b9LsZ=ytD4WIrHviLt5W*p7rMD;7O3O0mkXZ_^eox4ur?TE#%FY%FSRXV<4Q0@qiWTwitN`iO7HjnQy0$8sa7Oq1kNG*dmv{a09@4rqtgSN_}e%kDpX`#CX86~gti_mAtV31m+=S`u_CP}mMHC%*v2-62ae@*y`g3Y;)4R|_msAGG2aBXN|gf#(Xay)8fhhJV%0Wp)BG1T);SO1@F3tCbWxLAevyv|IQ<6Je21U_&Qg$y1XS24c;{y8{*t?i;Ar3s5(!z=d%@o)ZTUx?#tN7lj|WV<F8B^;4iH70U%SgX%&f$I|lbLRBf$^_WTkbR`Mm_RvfT-)ND6<o?2N{Upj0<i;hy0B;c7Io(2pE3kx}uPOO%66S$%`p1fFKtU+dY#c@b9W!3UG$*O&fh&z3{_bK!Db9h`Lm1^;C^Re`ot%>4F(N5Dvio7cc>p^(30h3y?}TQeZs@tXl@H}sZX9AB$jVvy40M+P0tlZvQZIqv24gFd@YQhvFog}TC`q+Nc+f%)6n;uD;((w^X_>8BXqiG#YbC2kY&fX10LI=xdxjfLggFK%xx*MiwMc!!vMJxRDI0Ne@Y4XME>vQA`1azmPR%817uE={(GoaI3sV6{1w|Iapu&;SE&z%-Ko#vc;ON4ec4G`od~#Lv*n>u(1DQ<R!2X@(kWH~$NkLs@#SYIrI8m6J<DzGfm2Y9m%WNo2bK&f!gh3*@3?c(E^shpJB<%XBOp=}>e4-Q*R}loGk`bXq)FS6ef*`X#YX~Ei7C%ymSW_coI8t)Jeh^k*GM&?;<)UPCkpU5zbwx>Gcs!Y^R5a4$`h-G|Cz3AP!`R`Hae@M}qDcx&w*P|~iZuTyH-FGcCX4D1Ml;9g{6T=0D!q#&H_TAW;(uBH2T#~Y21*JFF%lb*XM`#5P&Md7v9UC>5wVBEO>mPZA&RV2FLpUKda9D<Xn-5GodZXwgIACJQdLLgA7rX3@+c&7gl>_Plq%QPPF$a6M9@DlX?=r5XI!+t#2F!y4AxKL`Z)fT{o3sM7F~aKeN%oT#&PRgl8{TJKpf?=Xni%?<u&9HtChR|gk~k+pV+uQL6CuvjqAInN<WX~9p;B<+**k#VXy$Twjzw{1(g!Wc@FbUmJjxQFe0ssV{`_YTX6ye9O5Q6Li5lp$PS-TB`plnhND|{C0GSvWc4%@`?YhCq7nY4It(;P9VJm5Q>rm^KrPGz5*Z8kv|EU$pqWTG(O|d`yJzA5Ukh3<IpG0pTb1Ma1U`btmIT5aFigX0L(dJ*BZC=vFw}%*kXqY9NU4#)%qt8{r<o88Y$aA`s5S7gNfhQQdZ81RZc0u*D(9&n+p9uD33Wg4+h?^Qp9NKPCtfL@P^768)Rm|)6@G?E1-KZo@Xz^)FqhXMo2`wrznt(NurO%lz-<^T7qBg1DO8Efyx=1N$9bH`hQ~2WnqkNc-z!eN&9FMt!o#;1C+#s%rpvRcsx0Z+a5+j?O~f&^G?KtFRRBwpi^C-N_KU-h7pfX$*g<|@6cz*uoU;j=;n-X=W8sD7pg^ga8}2rM@>-Z0!-C<-mIo(?S_n@s7hIRE{P4-CIa)kCj>8yen*@K4p`c=TutJe&;Dc0RMd4?bgQZq@FZto$uZ-f^;e{76T%Q^7T>KN@5ZxqgJ}Rdm%0oK_J4I@N5yMuMmJ?o3We8dp$a3105xGdeg%%X_w&K{`3s*}NVP7zY1)ZH2$1f>Nj>0>x0v)laL8p;KA4ObNzW<{|(KOrHmu*w(|6r3%i!{H8_AOQEu*r(eZpD<%ek4h0vg^kpE{dE#<M<cXr}%4?i~m~IUVyQv{!mpdl2KPA?#`f=vJ~3Yqd);F2(r_}vj3$>cP&cc$zO)i`A6#;YQ_1pg!gCbkL>(GqJN0Te6skjMDu4E|EY?S9*QbrHRMzo71>nKM?(09>;U4wl9Bic5U35p!jni4&K5xE#SE)W<^V(i;3k+(=i&~E(eNf_)36aknuU5`c$y=t3{~oFycR>QZ=SF|!QmX_+`tG@mxdyMP*g#tImuiXG>Sm=%ApX3>%&p0LVA<Ld&Nl@l{xbM>0!wGw@-Hea(rXJ{UgWz`1`k_`)3H9L*imCQcS7-Ir099X0DOM0H7t+sQzVAfD>t6r07V7dg$;h+iEP^Ler^{DC=KV4(o_p#?T^KhTI04nuS<aTpzz9lDjgfXsWnArc2rSn?>JW#rj*WuSe^v<Vhp-a?0=rga!W(>$Cr`zVg3VpSTDot}o#r<gkYZ$c$y{e?41&OJ=1`n?~XaRLbHa%nk9Oj_*H6R)e9d75fh?d;cnm)zJPo$O>7N@1GG%0$UL33HT4S!u*doH*$S7i$9V5U!6t1ekVMC%s(kAYPvKjrS(l>Ba`=4+4C>IK2s|^|Krxzv*Sixx@w%TKB*Qca->$S|IMiXflbQ@N>0SZ$oiGmlZm~I<$CmB%kVc!G#-c^lI;IP%Ik!7L8&T~1EjV{x%sQqWX!IwQ67kogYxsp425(FD&T*V^6Tr__pj<@_R%QpNK#pbPWiu_e^U{`BK{YCeb^0(_P?e_wxbgNQ*nJgx_>$U)yl1(y??rR*y^ZMO)k1J{>bwu`u<QHD?5Lds3#@+{<U)bH<qRF;7O%Nlhf$^vkLoPjrKpjah8pLC&V9Fu9@Y?<!Z(CmFWFfj6d>kDkqOW+511~`u}kMa;zeQ=yCnW6Yl>X;!gx_Q&GZ#n2wCTe@1K&La=0uO2*2?|H!_UL}*A?H@bgHN=WI0;=hz?lqM-<y-3VpR1*47iK|fl{2`t|_WZ&1)oA^!W2x*%$_bkq#ntlXkKF%qcSd$WD#d^GpU=Mv@rRW^e^`a}S=4W%_0@{&E9KXh_&d^NMs**coB*Z>$99o69YXBYh=N6w55!JMzcYlo5ReQ063D`2MU;`AKsF0H@U$YKAX1v4fGk@ZI?A-6(kvx~$PQx^jgV4H)u~2$uX>b!D&#(-WK&zII~n0m3>g<0@$$<2uT<&SCh;#i$#)^c1iwE^#(!4f`J-Oe3FnV|{xoJZqWI6G%2#=!kbM5CQWk%+=op5ed{e65;y7DH!vS31h(I+u;cukQgsSV2r8Z&Xby}YociHm?KW+kCOBjq)@ua;%@1JC3$;$=S*HF=j)M+%WY^aKi7%8Vr9RM}bOvUG~Mo^tdpDM!2(<C9=|0$#N)BDwkJfKGLzwG(5a`Ru6rbDJ$cK&H<L=0w;B2P5`)+?@G4u7O-72-cWJE<#)@n4tmPX;lTm6uk_;x830vMWbiM^-jrkef0af6KhF<osEcC0Y6P_2bu%>L2>P{y(jc(-M;7`k(Iq#P!*Kv%dO2+`m@t{*Q~l74T=$Ih&CVEABLWt!DHc%ZWZKfBwprR7A>^LRJyWul)PZ=6`hmgn!G<znH0H<G(8JpDC<j{vVycdKCX<)ydT3*H^M|NLChq%W)|r{({awT_#g>f=bQD<JTu1d4>F6O6|+MK05yqPh6hHsAZkmiP#64Ux4M+DO_W#PeD&e*;Sx}jXW{4xMmP)i28pz>T*L_C|eIQ{%G_EORmjS`OkD3Eix%`0G^Jx|0&{;QIn;jrUCX5i7Ti`nP({#jXJDSO$X}_xjx}WRe3a#n;k1hOvxf>RC41+MJ;58<MR)XE3$s4_n*!GvDv}O&L0*rnWWUejQ_GFT$X<*Qj|8r;H#2eBefooNJZz5j(o~&{8OU*PnX?k?1V@2Uq<nlvFQ6_$V#LfUnUA=_?t5PrK(9JpUldoSSTFwGXIRef0a%{`6<gI1m2kyDXSP!{+(T)R8mm90#kQI>S{&u{PO%STc0*BI{j(%{b_2H|EXG3W@PIh!aqgv-(XT^kd#<-z08I%DlwvFkc>>(>^SQ@#bgHy`(L#G$;wnde}Xzn?*`fd^6M$f3Ki0cW_pBjVrmrsn9RSk`yZzjlp(5R@kgQk&nm31$@R1GN0s|uY8XZFx7_+=@kcpn{L!NLpIx7oU%&kQ8~@?{WmhmV?J9Tw%0J$J8UG_2|3R)%;%ZS;0uo6FG%lH8(Z^*Nk}f8h76hY**DIl^(rSg3+u)YeZ6}4NNS4{vq=}`3hS499$V((!l`sl6(c3M;(gTiFkynN|HIVBjVTW=StxREMJPJzBSfsEL-JhdHBJ)yS5&vkAof1q+B*nv_gUKXo4O5R&T0_#zkwA@rk@C<qO)L8o0!LAE5XYzT;W#Knp;Jp=i)L7=Zb<5TgwX~&Y?P}YOU^9wgGs`OM4=N?NLR|v^DH}1i^>x`dZHr=^@>%3^J!T+GI&+0krt0UBgi%chkMa(j@DOAs-@|HYmstGgiwqEd{%44kn<*b^JP9EESrhwD95KV36^D&=0dvD60MNUUc%R+YS}gh5rIfpCh{K1u1{k#lf2F7%VklaphsInEkFNc+5QvxVH*+uDcWgTHvf|8tjs^866*>5w+j5LET5c}^*_mL4-2#G`mmN!9)A=aB@*3Pwmv(7f5lF+zEakISIZjntUUiqCoxZ!?SB~+`CsMd4~%8|Us?VaE4MyV%Z|Kq_s`1Re>wga=4lfDE(f!+tXD}JFv8rH!9OEuTc(!T_^N7FMOTk#uv+9F5y4E2>`cBqG?(N~6&kur_#Lf}k~)zgZ4_743hS%?)%tWs!u?5#v|7G2)1vz~|C{?iVSV)<)>kXsf3*Gy_g^mlTQbVJ=o@ER<dJ2XOjZ-7q<0(iB^a7)@5us9E7R<-W&FpBa-#SjD~rF<L`aqEQx`jm{|@FSs49W>5|IF16#otK3ox{@fD?sS`A17;B*(>nlIUS4#D9wCc-2}q{<8|}myiGYe~SNVS^Sr}Rc1E+>rwpI$Rjz5f7BLX*}*(A<d2L0k*);#tNeHIU-sIv@!z2MADLm7i~m_IHW^rTI`U5%|B(V;A^xi;kN@h4@n1c8{3pqmZ2Xr7cIXgNKK@hAYpU!d@n1c8{Ad3V|BdK*O8(V<ivODIN&YwSpZ!DpS56-Pi9CGb`ZE3-s+@mB_){(7pJn4e`>*0Zs}TRo=0DN-R~G--N#lRi|1F#UsK>{D{rK}w{ZH{fd;XP=f7$r2{^$75%Ev!N`N#ODoFx8LjK4~`_$#%=Na-5OLTtYNWBivg4N?5n$rXa&MOk=QnW&M1?wztwiCSZ%{(fBiSN^B-kCi|F{`dE<Rycoj{lxqqV4bQV;~((YqWI6U@&EYzpQ((U82|AjJ3jv}7yq;Qe}(vOV}3&UKgM6>g#2GA%m0;s%KtSPf9(jDPcwa8Hj=aP-;BBlj*R~<74TI5fboS|G5<d<{zs*_Ug7*vkwa4<{<~;*i1%+%{*Mrm6XXB?bN;XYSNXqI7XQ`%>-ew#WBxA>owE3EWE+*o$A1kG=~iU*pqxDZQ~qzAJpX6e_|N`B{AVYP|NoT#pCtcP{_o=diTOYK$N10w7xAB!n}7e;@n1Vx{8#@|{;wUM|C<s1|HSyuP8$EUe~$mh=l{pWKjZ&P{{KJ6{}c0nTwgsd{vW^oe~7=4C({40{&DeNm-nyyzt%r`x%jVCT%Tp*|7v3)|LV^!yw`}w_`RdX@VuMSd{WJu`P;Jw@*~wh<-G^a<2M~?z^i83^O;M|;kTZe@Y}TA$Tz-v0U!T!O<w1#clZzYy~(XV0$!9}%71;UJKuk38{dB8Iegi`q5S778}fUHwB!BWnZSF0Qi->^q9M<jH-)d+wSvF;!SCKZ#|-|=zw-IDYro?~i^g*M=3#u-6JL6*F1w14+Blp)HSaxs`0FNo)b}&^xVHv#cEcEc@45s0-)3drV9a<vWce2F(;x5WFa7wNcgfM0`Aw4(eB8Ruyz|Pdd7Ztp`Mhst@*d4=@h0~uzWGaiez@yIeqX&g{Jfp5dGe3*_)|sIdF6RIeoc2DpMT3zKL5_YybV1X^7BVG;79kJ&b6QG@chlg`IZY$=Og)c{&CgQeE0bweDU@(`R<}Ve%}u-<vW(l<`3Qf0=Kqa!T)?>0k4v%!XLZ18ecZEt@qg}wfI-}-^Z`p|1=-)XJek&aMbgP@8x+#5AvT|F5^eP{=*y4WDg&5=QRJ+yD#ypy>Y+)<e2Jwf6+`{^_>y^*fx8;$>%@n<=@fMuRHx9|8>m^-ng#^@||@y`cICy%pdRz<6CE4>W_~-%cuXVx&K&`ar}q%3%v);$Ni@61YR06=l;YW`8UJ5_(P2gc%{O&eurPH`rB??<qfZW8NYenTi*C~`F@S=cl+~hd&p1UWAOa@KH&FUT<q7e>+&!5$9T)JZT!C$edfKtbt*soi<Z3h)$ROt>8<`>9Z%y++jsS!?t09-qCro;%F=WEmmhBFyVb_{#lH{tZz;ac&u=ozx0??4>wYrOKX&(ReAuOf{QdR5@M_1B{zc|k|C(>t@Ue4tdyBU`!dH)8&p$k`7Jv7tmHaDvE&uEL!QAZEk*5ns_@deUe0<lr{JuKN`EN_-@NKUy=kuEE<oDm#hyV7@i~MlAmAupL@VonSApfA&=e+ay-}rCM*6;@=9`Jtk3jNQIuI4NIKgn-9a1Fonx##(SUmxQ?_Z-RF{^a?eUpR#q^?8@qojH)lZ(G7wUe=xOo!yrIyX6^tc<y+<a>PKs%Addo6<x#Y-a3@$p5K>`ukjF{=%@T!i!SH;st)Dv<>hgw(<=Y%{jK<O^B&??K0KVSI@*c<(QgjF>E_#byVOB`$#aMOt?RGhsVY_Y#4*L(c(?^$_*^mHIZx${_IBs9b8GTj>b=Of-t%v-1z*a0jJ$#0{%X>HP+!D<`La8Ib?-`Ex4}@}_>!so(?v6Q=gd^zf5o}}g5`7h%gvtPuZ)?(=dXU9cUE`v;h7cujNMQ1ac958YhBuuzqNKEAOF)B-g3#~+}kpVi_9$kL&Nj9zPW+_;_OTK`HdgoP5g2E({P{tEUx6&?w!d;9$3U@ckIc}7=8!;>-V?$veWzWmj^$<*L^k7|K;_M`QkT@d5z}p<bSVj?tgip0WZ8^ga7x8eE-xl4F1Olll+c<HRFeF8o(FTIp}R?xA5y)tmLcyO7US|4dpr8*YQ=?Kg$1DdKq8+_b?u;Ip1IU=9B!~9%t~28uju^&RfPOEUf8wsJEVf(r6-o?Lp1|qDmwFNaN|eX}^B_nh7_A?{F)>CvPb~)@T|3qi6_k{!=YJ_||^@t=fycscHHj?wZOo55B~QG<=)as@0ay(*>`-au)ADX#`*T-9Y~L`FHYPmoMk%9C?L**LpE;pym7HQn&Mk6E5*T{c<#4{YqW``zBZWefJOMXZ|^v|Mv58-tgch{-LdR^CvG_%A0H`@QY`)=TjC9^B<XfJO6q95P$D2qj{q{XYy3HVZ84fE4Wv*kl%I7dwj!7llc1ceE+vK?fnZcoWl!Z3-|+rdhtsqmGD!R&f}krdYo(Zrt<FNQheUUtN2Ba%;$&h>hEv#=kw|<xAOW=HsXVB8Oxvg>Nfs-yD>b~qKf~+2V?l1mD=!Q3+M1v_kPBI{(|`p){o);wl47R={b&{F4pn7xs&~K$ByJz^&P<H6}`f*dhG(g<Iem1!9CaWp+~3ik8Zq;_xC6Adq&^x-*QcF-n#Woeybks{Z4tcc)Qom_F3Ih{=K$<9}cef)%S<-t?wP;6PNY%TfN(Y-#w_4|H@_8@V0N>;D1v1G~f5(IsWLER`J!l#`C=!+WB8@IGfkhCi8(Grg+!0ui&k#Ea%PtzQo`2_p^N4XLI<Bk7xS>$5?!WpUVf;Nbo6FwCA%w<2?D-#r*l3{^VWOTK=Z4j(_Rj5AxQ{XYneZEaXdPoyq52-j1)ivN3Nyq!&Nd=WbqO`e5!JRsC<qnEsTuu0Lds;Pabr<YQvb^XIk|@o)Q8;~m#k;i+#pf9=2peCJo^^Sb*J{(-r<{<)vz`qP3{yy`70_@j3`#QP_9d25`V-e)^L^RC`roxgcjlFwLuD*vsG=~q6W`AyC;{qN^o&z&JR@bza8<>N+w=LHw<_o~eL#(Vq8n*8FGXYy5lG~lbM#{8zeZ2zx=njegq#B<tC<fD2|=i}Gy@!Gt($E$PXu=mK*_4uiKD{;<G=fPaZZ~M&eyy+t`zuk<9yxA+0_-V5z^EFH3{;D72{_ub0`soFZU)a(0Q^l@7;;~$R?yag{HOc%slZyQ3tHu2HR^<9ynmhh2w(F}ey8g6bKl0c9jQM+NS^g(mf9KcKKf;F((EQ4Ffj@3v(*L1RuD`9d<5#-Z@qd0S;a|J{0N1De$$vib2OnQY_b>VMZ@%p+$N%8dr2ozg%Wt(Z=C@dQn7=geFFtpH;y-iR>Hd42&-PbUY3QGFLBg*-Cda>EcHCdc&h@>`P5j>KnSSHUF23>EANYAI6u<kjvwdSgGrwAgy8fp5asT1HNx$7f$4|Y~%>U$}YW{sSTKd1w{gF4@{5$`0#!vj_S5EUk3(oXEZhM-4_1d_f(<j&8V>^DM*P8n`zSYE^vATty{Gf{X<6wyxd}<3(?UeJyftuaK*$W;N)e6Umype~4hj%m*SMR%9%&K&1=7HU_MZ@@e;?Jw9i_c%^Bm8Gth|ymz2$nW~OVoUOt~mFpok8EvP7#N1STBwaDHgNpOc1TkJVWel*-l)0`v7t3D=mf6WS;2uZ7)$}$12flc^~n6$ARKN@j}t*+mF))-|P>H7hEHX-!2x{v}_{AJ>5)<Yj%og)wzL~+w+&;8Dn1HmAc~NORf-See`EA?$Q@UNu@2~msaCMuS7pF=&lDvd{2Q`^yBHmf1;<b?(QV?0hXAOZ;07PtBBbb-XJdO&`b1tMHPQ{JxiRkVV%hN=t@yM=U{N(Z>NiA=3Fdl-mVC};1==wi?&Eknj{AGtRZ%J9YvKNip1?>bHt#t_5?S^mWpP(@<qc&F9(bF+TyR~1H~OX-xVExzfvroHA~cLt%<uU8KTL}Uj}O`Efnn+j1u1;SuVa_`jjaBqK%k#Z$rWCs-pXWI^v5BnmFsFrh)Tae}CARcX*8pzTlh2?GBngaAw9H`4P{3U~h0ny$QiZ#wq@aht`Rw4&5i(<$V;KH>|DyTh{@8jZ=Ooopr+<ty?wfpMJc>Sz`aRsilMGeH>gkHtv71@V8)L4O7JDoE=<s-4ULimMaF={mmQs?xf)2{9YontE=d~=1?%<p2tg?exvwzubt@+sqtKJQ<olrVZNUh%MJ$H=WPv~MREV9Q)-Id5B4hEJ+-d)#a&PGzxF%+Cyo1y0x!eAJhD14rmhiNhrPs))SvIQ{_;n@^y^#vwKw$Qtx8M$+n(L#tv$4ecON=l?C3WlcymKz@tiv&-L3vuf6C$K#Nt8si!P1Fic7DbBIeyYO}zGG7tycg81ZZPy{-Q2CGq))sp7ifZN%?2#*3x7>&1m1FBR6`kBAS|c|u?Gq_FS3TCA>JR~+nHTfE)&5m9``L@~GO6{1#$YlVB>L{aUPQ$(+O7m5qVFBh$MFBbbAe?e65dW{(O#30e+$0tOr`y=AH4_1gXZ=Nq+Z1{}0`SWE$yS%1o6YC*5p7*eLf^QXFCQlLzf9fOd|9O(IM_nVP9=ck5oi|Smer38CIe)&W^4DvkOU^@L$4%qJ;Xfvc4{skUhCIAd^zU-FsP(`q(e$iM;@ih2iEFn!ES5EUR=nGOx#)Dk7Lj>xlDM_QjpE_p1=0MD2BPOn<Au97BQ9HThZz0AU@^A$U1G@vXNnz9PZOQCE)+A^D6yc~TCrv0DDlqlrQ-2t=86rmNn-3rW5nyfOcCF1I8WSmpo18Ab2Cx$#su-l<?Y2?HD`&bd!7|v{5eCsH?oST_25*o|Bk-moH=iZ$NN7niVt2T*0!H5-rUhiIA5$5ZTD>yi#}W=-u!sH`0V0GMYDd>MXf29h}T)EaF*U7Cbk_dF5ERteE8d9acaL_V)zH^#OS?K#hp7gi|+@OisIC2ar)uWqWJVj#Q6Ewi7vm46YYo25tnZpD}p;Fi5ot>OFXfmofx=lqG<K#P;vUfXT{Y=>WE9upDZSHT`am77l`}Mm?0KSn=F<E{Y1U?(?v<68Dibc+r?Wm`ir@HFA`5*xKcbcZmf8z`dG27@?vq(`-{ZlPD8|N7kc8Jr&fy%?>r~|)i{~iSL<<c+s%zd)l-HGb8$Cu<98dy{#s{cnobVi-{w~0)yHx&v6>@9?Fsb->wde~y{TB7x$DE=`!i>V=~JeQk8i6b(rl&}y`s5jQM*Xw*Z*4V+kdWzzqVYQk^h)Dyw(zH`|l5&6*r0S;}vyhHWAN!)mE%Z)DRu3ekIHS7l~Rkxj1<GHR8Mhw~4!hCE~#^9}*)UKVMw;+P&h~v5UpeOKXea%4ngAw&LEmFBP{uG*Vo)x1m_`%BiC1t0P2<pO%Vidkz&-ulp(3-8mteE@~nQwadiz-FJ(3c0D3~|8b<K*LjDyw9>6&^Q$Rw>Pzj#h_`BpRHu64<$fK+De)_Wd*`(x@%(!6Y0H|T^wb{WiF6|K<e<lbT3yc<cWoXl&K)yN{QH7h;*xH2#2Y1wn10oGaa|idQ|-n5V(vjxbSi#A40<3}?0<T>_~!mOLLb*k6m{q;v~3;5qN{AN<)_V}*{~(z-9sHj+&dhs9okKNy<n^ub4z1!_@`;Y+i|fN^X$DMzr)Mo%N^&4jeM}sm7B%p?S<(XpDZp}Wsgo@KK=B7&#w_I={-4Ua#bGhIKLae?D6)zXH|tS`S@M$&}naZ_wB4zvh$is-W6+lmy}%ka5^^V#?9RxQ-X}!mJi-Li0`b?o%f!f$CsS`skd(70Wb5oNUuCUmL56twX}#`9qgRv1~0pdgVXA^<!@fuihpU1<@ys|N$aXpN}rj~zI5ZhH@piTZ^B=mox@qBWN@%=>-1&azE9utc{hIMRk?inEuVWo3~tXyKGTL@)8$-Vzs959Ic+}ohHkpmoAURnbl!|FyjgdyO#j-UJ>PI<Z~nlH#{A=Z8u3jZp3DF2)rHU37p9-6vZ?gnJFBKY>DoB>rstpO_ST<id*|7_@izl_^?L{MdAU=)rYoK=S^ndl-l2)#dwYI5kH_w*!=G$lgRkz<j5}Lu@uO=;^XIzu<$d(}{QaB@xU%g$t_|_Im~s<O?DYBO`6c|J)ON4MS&jLQTbuK7FP*|iHkibpe*9KGZBRG<%2`+N4Z)>+*$V}{OQRzG@k=S*<I&|_>u%?I&u=f_IcHqUYjhmRV{feE1$lM(ciSv}Y;6NRxYnELRc%VrpL{zueU0{Z`hu%(EL}MI&9uEH#UJb6iElZ2E}v2Js8_Iev-j3HAA3E!ewlvLY?}6-T9WQ}?UwZG-`-g|TWO!p{Cy$cc2^(1#;weg4R?6WQhmKKZ++rrF4>%J*+#V5eRxxP<r}}JyZ)<bX^(|7)0YqH%wI3)%loyc!t3w+-20_x;0<uT@{WC@`8D5rpLgo<qW9?Mt^J={ZsU)=ur!@{qa#1%ukJy;*7y0w{Bc2>$7%$9FYfNOz4b`xb@LX8mtOzM+oo~eW>j^5(_>9}k2#|C#amxaAB=VIpZdFcFs1&beycun((1QO{YlN!{PXr-^HtOD@>53+^Zxw<{?&qje=))5AAM)|zi)cQOJCjGuWL8r)~%EM=Pw-@bpN9@FM4QO$xV}1l)f~xia&F27k_Kd2mFVZCH#-~91VI-wfy%|i}{Xuzm$CU;f?&w1snMN@5F;PcTM+|29y0~PdndtyVU2`9s7b`cH}1B_nw!<qs!mpo31|1zcx=T**10w?><W54c{7_p7qRVf5bbAzvR!Zym73O-;Y-fhBJr9e|dqwG@zsZ<6Y;80S%_`rVWeu^;`D%-PWw(ef+jOUT;{??6i))Q>lYL;Ew|TvL*9*&&I2H&axf+`++a=CeKXZ=Y4pd-#EXBPpRHFI68Yxux<NB-r?dy>D>b!=8u1WiN9}SzQ6Lt8~DB#mk6)-4E`}+8^-zt{?Q3vi=7J}4~C4L;Xn7pXutQecqX~(%*;~<9_8DwJwNETxld+8<F(@Qs*C+G3kHj!yFT|Xao6(>tvB#nX4LVQc}+5H&loE%DBR{Z|EQXHLH$0s<AT>qZx7D|>zrZy<(s<tD=xUiAKt8)e@6E%!MXNZ;>_V6iyuc_<QJd)ZOP>uFBivpkMP&-=<IKAe!2MMs*e0$A9nEXewzEYA8jSN4sDZ}bY+RZpm4spcEMEtiugiNyJHo<$H4Q&%dMu1eodYUe%mum)ZDmLZ2oqv|8Q}!Kdern?{p1<iLYD!P3vn3Wo5BH_@n#8w^OgmnA`14=WlC@8f&^{8sA(gbA9tl!J)r1!Eb#FMZ<M<{Z326!mLl_%<yjC2is=dFA6sg=K726(<8d><m}k#{)UBz#n6wG%(8*!h@EQ&^0vPn5S=@=^a`4<7Uves^h*Xc^*>rtUyQo!iQwwXdWhJay@C~2B!c%FP2rPzEfS*_J}I8M^HlLc>V9$7RTqjikLPD*?>j5g{FgSO{ck-*VWX<v-qO=EJI;Nxq-Lw)%oPj9iATR%>;D??%sYj{#Tyg$h(qfdq$W*joq6>7kz9Llpt!4EiRg3mn7I6tM|kkdJU;1xvHo|-iK1z}su{L<uDJf9jpE~>mu1?%-!pSlm3Eou?k~<f^58G6wp>3pGil<P;^@t1WM1D{+kf|wO8%hrZ8C2y+#`nkae*irV`tii&+5~g&ldeJUzy5(eMd08_U`nvPuCI~u5ZVmx@(2Mw&)vi`8xwd8>MNwWxtjg_q4CYCHnciu*V1DOy!y2UoUs?XTE=hIB(~$VEw$7LC?3I<xe&%$_zj6a`C}WN~X$-YlC?Q-r%P_J5gM@phITq*LV6e8nzVa{d2`>&rDCfP-i(mwtI{?(x9Diwl?y5-&rf8G~2<a9aKcG4Lvf8cXsxjC!ZIc|4e1xy{vvFxaDfGVtqex>)g(nWoLgYrhjuUf4$n<eD0smdC&D;7P!B6$&7Yy4Z0obC?2_@b!Po3XN$Ju-OO1l-U?c<L1OWNHR9O`eS>|!yW+NyKc)MBHdAbQxsw>#by3ju?#d$h;<n(ad*}1ItJ-GvblDY5f4p91`+K#-nKu=QfjPby_1>|d{lCr&o_u#k@M`J~aeudSyzg&Z6a4bo4???UocMa(^}$)4`-s1CV&V>SNaj9Y6CYIDBvx4iGySGi66)b2LG4-h1sgt3WR%Z8OnZ0l3BJ1N39+*CqTqe2XQtyNpQWo0IX&}0-Rk`5=I;cv+O!Kk8h5j>+Pxn<@If2Vt?0R6=C*p7t9mrc#ENGGyNeax_O{!DmBZrceLwf&udiAgEO{&ujDE3o@WA|A(gPMhlK%S2zL}ZpDho4rO3+6CS1@tqSuMNl?h>@Pp>ptCW}o<}#UyVGYbd7fY9Zc8w-Ym~O%ScC4NEudvtDfbrm;7_=e%He_mWKBg5JUR^SAOH>)s6B9XUAjY7-;V{^jkW_1uoZtDRpC-dW7VPb)6Z?7Vkw(B@%3^V^T(GGE6s>4`UgD_Z=~MqJ;zQ*iN*l`|8X?+@ahE-Jm#GqYWrP!c?Jc1GLShX)HgXHMzmiWhb_%4}R$Bj`9~Y;f0Iw}{R)N0#`l<E8O+BRzG?<Y4fOpMrCe?J_Gi=)qf$R}!Z`bWWz<G>3Puv7CQ$!K2=|E4~P>I_K7sEuT~pquw2tTJ-Pnsb^b^O=&}Vh*AIQBJTRRvzYe9)Zp^!vx5gNni{md{`uh6+KKdf<>z2js$Xh%>q)7bKc1L+cvfps?SmR(&&##NrDF~UbIyGtNZf9SLw{T-`V_AT?p-%nRJpiss@@fYQnj9+nEGzoy<)=iW5t5>7@@u~!Eg4%lYU9T9KY%2zC5qD;rDoOm^ZHXnAE4Ij!!NBX>4l8-P3ux`$Aspre%D~r@lY?rtAFMUux>td3|&1JL~lKzZ%^rsJUod>Y9DiQoqITOC5dU4^MgT2XE3OozJbA;jh=P>~9}q`}$cT`03jhT0b&lVz9liPwEA2XlljAiK$IVThxC3U%{r=z70wSUY>gR<G!gQO0U$#u^y?5SKpj^{kDOr&zIkma@EPH+%G4kX7#!zwQ>Emspl*AOto1&A+>P&jVb%80jUuOdZzZ*@1H96hNV_Me^2W3n)juuAG<I0!;d$nF0`*t&42vH)ZuG~q~7{;Sn8{#*QS15(=GLo+buPD%E;8(ucoKYUo<(jX5!q`%xe8pA3ilO)qdB2)a_k96kA8UCg%UJUnKfDqW@z{g5AC+s;J{q*YOFdK4;#RS{l1jtoc67%MWxBFD>{?^gQy4Sp4w&qHXQ@!TR0Pg6kI77h|k(saY$=r3Q=~ks9Cha&gABoyCtEFBRwB_O)nNyjh%c)<Lmv)RExZtviFm%g+`Um5fi#JTgA@)sm5^sSoxRKmO2G%v^hpc(Zwo-?_Ib-^PdVJrjN@$=^7$MD4cHbGuISs-E?M_vij8{GGEWdT-a<!@n7N$eVM?gIuk4bFkj&#re~Vym;dUp8Nh_@3nf9ysp1YE2;O>s*(%E!IB>qzFXos&wICBc!PIu*R#DT^&cu(R^_#l564>Gcdx!*a%Ez^clYEq=>=C#@eXOD_@M=7^Cg?+l)N#>^zMJ{8h+!pWxP|P=e%CS^ZBU<_j$F~eB>E#Eh-uI?gRYrrtay5cW+96ll-Kl_o#uq%dT;}_9s_*!>2FxI<&jbd!|z#Z^pYFd13S3-bF1tdha$H<t=>o`I1!~FAjz#8}i#;KZEz_w7n#lrF)egc-NcNVRmrn!PUXDD;VE1?{{y`9e;U$K0n=a@0jM*-~6N(jBDxj|7KZfhjZrhkH$Qd9<%=wZ*#qA>3wU)c!irT<+D%s(u=#8UaR)!de2qgUUJa7HQ2u6s}jDXLHgVynUYEQ6G~c~UxnZNX#;L;X~72$YhKdu>ou+4+0oydFw!d-wKb2wSY>vJn|PyS=Y3sDr#(7@*Zx)I{km@Q3MVGhd7CYM=8rbN{Pa8dUj9hB{ooBH6Y4h$rnH@%-aWNPdi|GU(|<26PQU$H<<cn~CwpgoJ0;z*%B|j}+_~v~UtZ!JdjBIn>h4MD&sywCJA<l~bm{a=`Y&fldS(B{>0M4ydglGh(q~P-F+K3AU(;slVCgGU2bM1B(k(szgTd*GXFZwT{OrT&Qy-X{{`R5`rI`nc)2a491Pvz@mVS`x)9TGn-1MH>UzQ$=4f4{1&o8N0e6aP<5q9Zy-&G4*H*22`IQIr_$n)O#z)0tQ)S>j#-Ye2|-TPZN{b5=9&RQ!re|5A<X^V57NPjW8UV88KV>XZec5?cx^oQv$2Q)}~<7%cqySRJ$igruWomvN_e=Iy$x;E*gTR%LjWz}<=r6(i?m7MzLb*1NgI<Vx^ua3Ofd}ZVG73(IHzEk6#^!y1|Zr=ZC8?WQkhg<D#_e1N2d4HGwQl(3J;`+5EBW^w;y=?UtrBjk6r4RnPHSI3p>H5|0PggJLp1x=FGwC09y_#OMxo>*@A8pd{Z$_nGI#?w=vBx7XUbf}R^oqlGrk`m3O=*Lgr=;zxUoCz7%#EeTt~<85W}OM?cIh#tPaSxz)NO2dty*^rinU*Y4Fjt4JtOLR)dx4`M_=gReJ>VyJ)X_upY_xD?+3o|*5}veH_rRqGrv#qbstXWql1U}rTgme#L=tx!`792?N8(Ru@C$5Ki+S|uYPBzxA(eJ`Mm4neB6_(`57I?1UJuH%6GDNyaTc6eEE5Q@~1>KZ`7dL-Va-Hy{50!^v-#55Vu;s?VWmEeg67~5AfOZmjrDKhwx^1ydSjP^)lbmb9hN^$5rV!H@@iY9#fD$<Ep*sLoa@r9@cwDS}guNeO0gPf_iKFmOi@mrXc^<1?i5jy<B>3Z7;p$sS)YoKW|7s`ijDrH5ui-d~87LE&hhhMH?PVk1ySrzNqtGLE}GG@x`$^{HF!2O4D~~=~p-0!*9K<Z~F2{E7Q&LN2lNT^q%yfCVSHtEOmmbDs3)#@99~=4NtF3FTZ71>7KW)4JKCo+3WPs3!b%PO6sfoRQ}keX<n1$r~amW@B8+1*LjoQ)p-4{S9yyY<a-wno$JlK;+XeD^%>qJKm6iV`Kh;e%7Amdp1~8|=G#YkwcmZrpOSy4zijEZ-e<jLc(s#l_<+q{ct_?}NmbUHcvsiC((Bq`vG>mC=e(Z(CyN1e{<eWVOz@CB*PF~eP=5D3xlA=a1a@{k;#-bA9e;>DP(2nt#BQKJml6m-Af3-WBWG$onz)ZW#bg&iHOsX<%^QJ0>Mx)@yhjQ>O=mYiPZ0Ax%;<+c<@(V)<4l7<PP3{#XnTJ?PvpxzGbw{U9wCE0U}RrCVM@C^*5}$iD5^I=n^2oQ-);~-L1RxqeYUGVHmjCE7$2=apHq=N6WW15m1Fijn@a*f&x<%cQx&8=^pjRVBe*6$4D!Q0i!@3<7`(4N3WeG|5%GyU_vVB>Q?{W!7T;k#GWCo-Tx)$jT#s`<DRMJDuAYfKke|aqj7ZKsyrj-OwE?d`u8TfCCajV@x}+99Md(C5nhB{r|M99l0{3k|)SjX}YN>!eU<T_wHp!?x*IKYW`|l+{8nt#kIab3xH8IIOs29*Ya8_?UAI}&*2c>mAM05f@kB>b-0`{OhdYX+rmoQvF@9X$Hf+a;hgQ!qIw<k+L1{OFzjt-MP1D{Sn7z+$OA4N<({D^Hm>{t}5"""
_r=zlib.decompress(base64.b85decode(W))
_d=np.frombuffer(_r[:80512],dtype=np.uint8)
_o=80512
_s=np.frombuffer(_r[_o:_o+2240],dtype=np.float32);_o+=2240
_z=np.frombuffer(_r[_o:_o+2240],dtype=np.float32);_o+=2240
_b=np.frombuffer(_r[_o:_o+2240],dtype=np.float32);_o+=2240
_n=np.frombuffer(_r[_o:_o+1696],dtype=np.float32)
# Dequantize kernels
_shapes=[(108,96),(40,64),(256,128),(128,128),(128,144)]
_ko=0;_so=0;_bo=0
ego_k=None;syn_k=None;mix1_k=None;mix2_k=None;pol_k=None
ego_b=None;syn_b=None;mix1_b=None;mix2_b=None;pol_b=None
_kn=[];_bn=[]
for _ni,_no in _shapes:
    _k=np.zeros((_ni,_no),dtype=np.float32)
    for _c in range(_no):
        _k[:,_c]=_d[_ko:_ko+_ni].astype(np.float32)*_s[_so+_c]+_z[_so+_c]
        _ko+=_ni
    _kn.append(_k);_bn.append(_b[_bo:_bo+_no].copy())
    _so+=_no;_bo+=_no
ego_k,syn_k,mix1_k,mix2_k,pol_k=_kn
ego_b,syn_b,mix1_b,mix2_b,pol_b=_bn
_no=0
eln_s=_n[_no:_no+108];_no+=108
eln_b=_n[_no:_no+108];_no+=108
sln_s=_n[_no:_no+40];_no+=40
sln_b=_n[_no:_no+40];_no+=40
rms_s=_n[_no:_no+128]
# ===== BIT BUDGET =====
BB=np.array([np.int32(10), np.int32(10), np.int32(10), np.int32(10), np.int32(10), np.int32(10), np.int32(10), np.int32(2), np.int32(3), np.int32(5), np.int32(10), np.int32(10), np.int32(1), np.int32(1), np.int32(12), np.int32(10), np.int32(10), np.int32(10), np.int32(12), np.int32(10), np.int32(10), np.int32(10), np.int32(10), np.int32(10), np.int32(12), np.int32(12), np.int32(10), np.int32(10), np.int32(10), np.int32(10), np.int32(5), np.int32(5), np.int32(3), np.int32(1), np.int32(8), np.int32(10), np.int32(12), np.int32(12), np.int32(12), np.int32(10), np.int32(10), np.int32(10), np.int32(10), np.int32(10), np.int32(1), np.int32(5), np.int32(5), np.int32(10), np.int32(12), np.int32(3), np.int32(1), np.int32(10), np.int32(10), np.int32(10), np.int32(8), np.int32(10), np.int32(10), np.int32(10), np.int32(10), np.int32(12), np.int32(12), np.int32(12), np.int32(12), np.int32(12), np.int32(10), np.int32(12), np.int32(12), np.int32(10), np.int32(12), np.int32(12), np.int32(10), np.int32(10), np.int32(10), np.int32(10), np.int32(10), np.int32(1), np.int32(5), np.int32(5), np.int32(10), np.int32(12), np.int32(3), np.int32(1), np.int32(10), np.int32(10), np.int32(10), np.int32(8), np.int32(10), np.int32(10), np.int32(10), np.int32(10), np.int32(12), np.int32(12), np.int32(12), np.int32(12), np.int32(12), np.int32(10), np.int32(12), np.int32(12), np.int32(10), np.int32(10), np.int32(10), np.int32(12), np.int32(10), np.int32(10), np.int32(12), np.int32(12), np.int32(10), np.int32(10), np.int32(10), np.int32(10), np.int32(10), np.int32(10), np.int32(10), np.int32(10), np.int32(10), np.int32(2), np.int32(3), np.int32(5), np.int32(10), np.int32(10), np.int32(1), np.int32(1), np.int32(12), np.int32(10), np.int32(10), np.int32(10), np.int32(12), np.int32(10), np.int32(10), np.int32(10), np.int32(10), np.int32(10), np.int32(12), np.int32(12), np.int32(10), np.int32(10), np.int32(10), np.int32(10), np.int32(5), np.int32(5), np.int32(3), np.int32(1), np.int32(8), np.int32(10), np.int32(12), np.int32(12), np.int32(12), np.int32(10), np.int32(10), np.int32(10), np.int32(10), np.int32(10), np.int32(1), np.int32(5), np.int32(5), np.int32(10), np.int32(12), np.int32(3), np.int32(1), np.int32(10), np.int32(10), np.int32(10), np.int32(8), np.int32(10), np.int32(10), np.int32(10), np.int32(10), np.int32(12), np.int32(12), np.int32(12), np.int32(12), np.int32(12), np.int32(10), np.int32(12), np.int32(12), np.int32(10), np.int32(12), np.int32(12), np.int32(10), np.int32(10), np.int32(10), np.int32(10), np.int32(10), np.int32(1), np.int32(5), np.int32(5), np.int32(10), np.int32(12), np.int32(3), np.int32(1), np.int32(10), np.int32(10), np.int32(10), np.int32(8), np.int32(10), np.int32(10), np.int32(10), np.int32(10), np.int32(12), np.int32(12), np.int32(12), np.int32(12), np.int32(12), np.int32(10), np.int32(12), np.int32(12), np.int32(10), np.int32(10), np.int32(10), np.int32(12), np.int32(10), np.int32(10), np.int32(12), np.int32(12), np.int32(10), np.int32(10), np.int32(12), np.int32(10), np.int32(12), np.int32(10), np.int32(10), np.int32(10), np.int32(10), np.int32(10), np.int32(5), np.int32(5), np.int32(5), np.int32(5), np.int32(12), np.int32(12), np.int32(12), np.int32(12), np.int32(3), np.int32(3), np.int32(3), np.int32(3), np.int32(12), np.int32(12), np.int32(7), np.int32(7), np.int32(5), np.int32(5), np.int32(5), np.int32(10), np.int32(10), np.int32(10), np.int32(10), np.int32(10), np.int32(10), np.int32(10), np.int32(10), np.int32(5), np.int32(10), np.int32(10), np.int32(10), np.int32(10)],dtype=np.float32)
_LV=2.0**BB-1.0;_HL=_LV/2.0
def quantize_obs(obs):
    o=obs.copy();m=_HL>0.75
    o[m]=np.round(o[m]*_HL[m])/_HL[m]
    return o
# ===== NN FORWARD =====
def silu(x):return x/(1.0+np.exp(-np.clip(x,-20,20)))
def ln(x,s,b):m=x.mean();v=x.var();return(x-m)/np.sqrt(v+1e-6)*s+b
def rmsn(x,s):ms=np.mean(x*x);return x/np.sqrt(max(ms,1e-3)+1e-6)*s
def forward(obs):
    e0=ln(obs[:108],eln_s,eln_b);e1=ln(obs[108:216],eln_s,eln_b);sy=ln(obs[216:],sln_s,sln_b)
    h0=silu(e0@ego_k+ego_b);h1=silu(e1@ego_k+ego_b);hs=silu(sy@syn_k+syn_b)
    h=np.concatenate([h0,h1,hs])
    mx=silu(h@mix1_k+mix1_b)
    mx=rmsn(mx,rms_s)
    mx=mx+silu(mx@mix2_k+mix2_b)
    return mx@pol_k+pol_b
# ===== JAX COMPAT =====
def _cond(pred,true_fn,false_fn,*args):
    return true_fn(*args) if pred else false_fn(*args)
def _switch(index,branches,*args):
    idx=int(index)
    idx=max(0,min(idx,len(branches)-1))
    return branches[idx](*args)
# ===== OBS CODE (from obs_v7.py) =====

_MAX_VEL = 800.0
_MAX_DIAG = 18358.0
_SHIELD_MAX = 4.0
MAX_CP = 24
MAX_CHECKPOINTS = 6
_MAX_TURN_RAD = 18.0 * np.pi / 180.0
_REACH_RADIUS_T4 = 1721.825
_COAST_FACTOR_T4 = 3.186625
_MAX_TURNS = 501.0
_FRICTION = 0.85
_MAX_THRUST = 200.0
_BOOST_THRUST = 650.0
_BOOST_DETECT_THRESH = 400.0
_TIMEOUT_MAX = 100.0
_CP_ETA_MAX_TURNS = 20.0
_FAR_POINT = 10000.0
def _vieta_tti(dx, dy, dvx, dvy, collision_radius=800.0):
    a = dvx**2 + dvy**2
    b = 2.0 * (dx * dvx + dy * dvy)
    c = dx**2 + dy**2 - collision_radius**2
    is_colliding = c <= 0.0
    discriminant = b**2 - 4.0 * a * c  # FIX: was b²-ac, must be b²-4ac
    has_root = (discriminant > 0.0) & (a > 1e-10)
    sqrt_disc = np.sqrt(np.maximum(discriminant, 0.0))
    denom = -b + sqrt_disc
    t_raw = np.where(denom > 1e-10, 2.0 * c / denom, 99.0)  # FIX: 2c/denom
    t_col = np.where(t_raw > 0.0, t_raw, 99.0)
    t_col = np.where(has_root, t_col, 99.0)
    t_col = np.where(is_colliding, 0.0, t_col)
    return 1.0 / (1.0 + np.clip(t_col, 0.0, 10.0))
def _kinematic_reach(dx, dy, dvx, dvy, spd_ego, spd_other):
    coast_dx = dx + dvx * _COAST_FACTOR_T4
    coast_dy = dy + dvy * _COAST_FACTOR_T4
    d_coast = np.sqrt(coast_dx**2 + coast_dy**2 + 1e-8)
    r_total = 2.0 * _REACH_RADIUS_T4
    min_reach = np.maximum(0.0, d_coast - r_total) / _MAX_DIAG
    max_reach = (d_coast + r_total) / _MAX_DIAG
    return np.clip(min_reach, 0.0, 1.0), np.clip(max_reach, 0.0, 2.0)
def _multi_hyp_intent(
    ego_px, ego_py,
    opp_px, opp_py, opp_vx, opp_vy, opp_ang,
    target_cp_x, target_cp_y,
    runner_px, runner_py,
    horizon=4, friction=_FRICTION, max_thrust=_MAX_THRUST,
):
    max_turn_rad = _MAX_TURN_RAD
    def _sim_intent(tx, ty, thrust):
        px_s, py_s = opp_px, opp_py
        vx_s, vy_s = opp_vx, opp_vy
        ang_s = opp_ang
        for _ in range(horizon):
            dx_s = tx - px_s
            dy_s = ty - py_s
            target_ang = np.arctan2(dy_s, dx_s)
            delta = np.arctan2(
                np.sin(target_ang - ang_s),
                np.cos(target_ang - ang_s))
            delta = np.clip(delta, -max_turn_rad, max_turn_rad)
            ang_s = ang_s + delta
            vx_s = vx_s + np.cos(ang_s) * thrust
            vy_s = vy_s + np.sin(ang_s) * thrust
            px_s = px_s + vx_s
            py_s = py_s + vy_s
            vx_s = vx_s * friction
            vy_s = vy_s * friction
        return px_s, py_s
    rx, ry = _sim_intent(target_cp_x, target_cp_y, max_thrust)
    bx, by = _sim_intent(runner_px, runner_py, max_thrust)
    far_x = opp_px + np.cos(opp_ang) * _FAR_POINT
    far_y = opp_py + np.sin(opp_ang) * _FAR_POINT
    cx, cy = _sim_intent(far_x, far_y, 0.0)
    d_runner = np.sqrt((rx - ego_px)**2 + (ry - ego_py)**2 + 1e-8) / _MAX_DIAG
    d_blocker = np.sqrt((bx - ego_px)**2 + (by - ego_py)**2 + 1e-8) / _MAX_DIAG
    d_coast = np.sqrt((cx - ego_px)**2 + (cy - ego_py)**2 + 1e-8) / _MAX_DIAG
    return (np.clip(d_runner, 0.0, 2.0),
            np.clip(d_blocker, 0.0, 2.0),
            np.clip(d_coast, 0.0, 2.0))
def _build_ego_radar(
    state,
    px: np.ndarray,
    py: np.ndarray,
    vx: np.ndarray,
    vy: np.ndarray,
    ang: np.ndarray,
    ncp: np.ndarray,
    shield: np.ndarray,
    collided: np.ndarray,
    prev_vx: np.ndarray,
    prev_vy: np.ndarray,
    prev_delta_ang: np.ndarray,
    prev_thrust: np.ndarray,
    ego_local,
    opp_inferred_shield_pair: np.ndarray,
    opp_mass_ratio_pair: np.ndarray,
    boosted: np.ndarray,
) :
    ei = 0
    ti = 1
    o0 = 2
    o1 = 3
    ego_a = ang[ei]
    cos_a = np.cos(ego_a)
    sin_a = np.sin(ego_a)
    cp_pad_limit = np.minimum(
        state.n_global_cp + 2, np.int32(MAX_CP - 1))
    max_cp_f = np.maximum(state.n_global_cp, np.int32(1)).astype(
        np.float32)
    def _rotate(dx, dy):
        local_x = dx * cos_a + dy * sin_a
        local_y = -dx * sin_a + dy * cos_a
        return local_x, local_y
    v_fwd = (vx[ei] * cos_a + vy[ei] * sin_a) / _MAX_VEL
    v_lat = (-vx[ei] * sin_a + vy[ei] * cos_a) / _MAX_VEL
    spd_ego = np.sqrt(vx[ei]**2 + vy[ei]**2 + 1e-8)
    speed = spd_ego / _MAX_VEL
    delta_ang = ang[ei] - state.prev_pod_angle[ego_local]
    delta_wrap = np.arctan2(np.sin(delta_ang), np.cos(delta_ang))
    delta_ang_norm = np.clip(delta_wrap / _MAX_TURN_RAD, -1.0, 1.0)
    prev_da = np.clip(prev_delta_ang[ei] / _MAX_TURN_RAD, -1.0, 1.0)
    prev_vfwd = np.clip(
        (prev_vx[ei] * cos_a + prev_vy[ei] * sin_a) / _MAX_VEL,
        -1.0, 1.0)
    prev_vlat = np.clip(
        (-prev_vx[ei] * sin_a + prev_vy[ei] * cos_a) / _MAX_VEL,
        -1.0, 1.0)
    prev_t = prev_thrust[ei]
    thrust_last = np.where(
        prev_t < -0.5, -1.0,        # shield (encoded as -1 in prev_thrust)
        np.where(
            prev_t < 0.5, 0.0,       # coast (thrust=0)
            np.where(
                prev_t > _BOOST_DETECT_THRESH, 1.0,  # boost
                prev_t / _BOOST_THRUST)))     # thrust (200/650 ≈ 0.308)
    shield_cooldown = shield[ei] / _SHIELD_MAX
    ego_cp_progress = ncp[ei].astype(np.float32) / max_cp_f
    cp_remaining = (max_cp_f - ncp[ei].astype(np.float32))
    turns_remaining = np.maximum(
        _MAX_TURNS - state.turn.astype(np.float32), 1.0)
    urgency = np.clip(cp_remaining / turns_remaining, 0.0, 1.0)
    cg0_brk = np.minimum(ncp[ei], state.n_global_cp - 1)
    dx_brk = state.global_cp_x[cg0_brk] - px[ei]
    dy_brk = state.global_cp_y[cg0_brk] - py[ei]
    d_brk = np.sqrt(dx_brk**2 + dy_brk**2 + 1e-8)
    braking_dist = np.clip(
        (spd_ego * spd_ego) / np.maximum(d_brk * _MAX_THRUST, 1.0),
        0.0, 1.0)
    bounce = collided[ei].astype(np.float32)
    ego_boosted = boosted[ei].astype(np.float32)
    feat_a = np.array([
        v_fwd, v_lat, speed, delta_ang_norm, prev_da,
        prev_vfwd, prev_vlat, thrust_last,
        shield_cooldown, ego_cp_progress, urgency,
        braking_dist, bounce, ego_boosted,
    ])  # 14 dims
    def _cp_local(pod_i, cp_off):
        cp_g = np.minimum(ncp[pod_i] + cp_off, cp_pad_limit)
        dx = state.global_cp_x[cp_g] - px[pod_i]
        dy = state.global_cp_y[cp_g] - py[pod_i]
        d = np.sqrt(dx**2 + dy**2 + 1e-8)
        dist = d / _MAX_DIAG
        d_safe = np.maximum(d, 1e-4)
        cp_cos_a = dx / d_safe
        cp_sin_a = dy / d_safe
        cos_brg = cp_cos_a * cos_a + cp_sin_a * sin_a
        sin_brg = cp_sin_a * cos_a - cp_cos_a * sin_a
        return dist, cos_brg, sin_brg
    def _speed_toward(cp_off):
        cp_g = np.minimum(ncp[ei] + cp_off, cp_pad_limit)
        cg = np.minimum(cp_g, state.n_global_cp - 1)
        dx = state.global_cp_x[cg] - px[ei]
        dy = state.global_cp_y[cg] - py[ei]
        d = np.sqrt(dx**2 + dy**2 + 1e-8)
        return np.clip((vx[ei] * dx + vy[ei] * dy) / (d * _MAX_VEL),
                        -1.0, 1.0)
    cp0_g = np.minimum(ncp[ei], cp_pad_limit)
    cp0_dist, cp0_cos, cp0_sin = _cp_local(ei, 0)
    cg0 = np.minimum(ncp[ei], state.n_global_cp - 1)
    dx_cp0 = state.global_cp_x[cg0] - px[ei]
    dy_cp0 = state.global_cp_y[cg0] - py[ei]
    d_cp0_b = np.sqrt(dx_cp0**2 + dy_cp0**2 + 1e-8)
    speed_toward_cp = (vx[ei] * dx_cp0 + vy[ei] * dy_cp0) / d_cp0_b
    vel_cp_align = np.clip(speed_toward_cp / spd_ego, -1.0, 1.0)
    speed_toward_cp0 = np.clip(speed_toward_cp / _MAX_VEL, -1.0, 1.0)
    cp1_dist, cp1_cos, cp1_sin = _cp_local(ei, 1)
    speed_toward_cp1 = _speed_toward(1)
    cp2_dist, cp2_cos, cp2_sin = _cp_local(ei, 2)
    speed_toward_cp2 = _speed_toward(2)
    cp1_g = np.minimum(ncp[ei] + 1, cp_pad_limit)
    exit_dx = state.global_cp_x[cp1_g] - state.global_cp_x[cp0_g]
    exit_dy = state.global_cp_y[cp1_g] - state.global_cp_y[cp0_g]
    exit_dx_e = exit_dx + 1e-8
    exit_dy_e = exit_dy + 1e-8
    exit_len = np.sqrt(exit_dx_e**2 + exit_dy_e**2)
    exit_len_safe = np.maximum(exit_len, 1e-4)
    ex_cos_a = exit_dx_e / exit_len_safe
    ex_sin_a = exit_dy_e / exit_len_safe
    cos_exit_rel = ex_cos_a * cos_a + ex_sin_a * sin_a
    sin_exit_rel = ex_sin_a * cos_a - ex_cos_a * sin_a
    seg_len_01 = np.sqrt(exit_dx**2 + exit_dy**2 + 1e-8) / _MAX_DIAG
    cp2_g = np.minimum(ncp[ei] + 2, cp_pad_limit)
    seg_dx_12 = state.global_cp_x[cp2_g] - state.global_cp_x[cp1_g]
    seg_dy_12 = state.global_cp_y[cp2_g] - state.global_cp_y[cp1_g]
    seg_len_12 = np.sqrt(seg_dx_12**2 + seg_dy_12**2 + 1e-8) / _MAX_DIAG
    bearing_to_cp = np.arctan2(dy_cp0, dx_cp0) - ego_a
    bearing_wrap = np.arctan2(np.sin(bearing_to_cp), np.cos(bearing_to_cp))
    curvature_need = np.abs(bearing_wrap) / np.pi
    feat_b = np.array([
        cp0_dist, cp0_cos, cp0_sin, vel_cp_align,
        cp1_dist, cp1_cos, cp1_sin,
        cos_exit_rel, sin_exit_rel,
        curvature_need,
    ])  # 10 dims
    tdx = px[ti] - px[ei]
    tdy = py[ti] - py[ei]
    trl_x, trl_y = _rotate(tdx, tdy)
    tvdx = vx[ti] - vx[ei]
    tvdy = vy[ti] - vy[ei]
    tvl_x, tvl_y = _rotate(tvdx, tvdy)
    td = np.sqrt(tdx**2 + tdy**2 + 1e-8)
    closing_t = -(tvdx * tdx + tvdy * tdy) / (td * _MAX_VEL)
    dir_t2e_x = -tdx / td
    dir_t2e_y = -tdy / td
    t_face = (np.cos(ang[ti]) * dir_t2e_x + np.sin(ang[ti]) * dir_t2e_y)
    t_cp_progress = ncp[ti].astype(np.float32) / max_cp_f
    t_ncp_diff = (ncp[ti].astype(np.float32) -
                  ncp[ei].astype(np.float32)) / max_cp_f
    t_shield_cd = shield[ti] / _SHIELD_MAX
    t_boosted = boosted[ti].astype(np.float32)
    t_dvx = vx[ti] - vx[ei]
    t_dvy = vy[ti] - vy[ei]
    tti_t = _vieta_tti(tdx, tdy, t_dvx, t_dvy)
    t_cg = np.minimum(ncp[ti], state.n_global_cp - 1)
    t_dx_cp = state.global_cp_x[t_cg] - px[ti]
    t_dy_cp = state.global_cp_y[t_cg] - py[ti]
    t_d_cp = np.sqrt(t_dx_cp**2 + t_dy_cp**2 + 1e-8)
    t_vel_toward_cp = np.clip(
        (vx[ti] * t_dx_cp + vy[ti] * t_dy_cp) / (t_d_cp * _MAX_VEL),
        -1.0, 1.0)
    t_dist_cp = t_d_cp / _MAX_DIAG
    feat_c = np.array([
        trl_x / _MAX_DIAG, trl_y / _MAX_DIAG,
        tvl_x / _MAX_VEL, tvl_y / _MAX_VEL,
        closing_t, t_face,
        t_cp_progress, t_ncp_diff,
        t_shield_cd, t_boosted, tti_t,
        t_vel_toward_cp, t_dist_cp,
    ])  # 13 dims
    def _opp_block(oi, opp_pair_idx):
        odx = px[oi] - px[ei]
        ody = py[oi] - py[ei]
        orl_x, orl_y = _rotate(odx, ody)
        ovdx = vx[oi] - vx[ei]
        ovdy = vy[oi] - vy[ei]
        ovl_x, ovl_y = _rotate(ovdx, ovdy)
        od = np.sqrt(odx**2 + ody**2 + 1e-8)
        closing_o = -(ovdx * odx + ovdy * ody) / (od * _MAX_VEL)
        dir_o2e_x = -odx / od
        dir_o2e_y = -ody / od
        o_face_ego = (np.cos(ang[oi]) * dir_o2e_x +
                      np.sin(ang[oi]) * dir_o2e_y)
        o2t_dx = px[ti] - px[oi]
        o2t_dy = py[ti] - py[oi]
        o2t_d = np.sqrt(o2t_dx**2 + o2t_dy**2 + 1e-8)
        o_face_team = (np.cos(ang[oi]) * o2t_dx / o2t_d +
                       np.sin(ang[oi]) * o2t_dy / o2t_d)
        o_bounce = collided[oi].astype(np.float32)
        o_cg = np.minimum(ncp[oi], state.n_global_cp - 1)
        o_dx_cp = state.global_cp_x[o_cg] - px[oi]
        o_dy_cp = state.global_cp_y[o_cg] - py[oi]
        o_d_cp = np.sqrt(o_dx_cp**2 + o_dy_cp**2 + 1e-8)
        o_cp0_dist = o_d_cp / _MAX_DIAG
        o_cp_progress = ncp[oi].astype(np.float32) / max_cp_f
        o_ncp_lead = (ncp[oi].astype(np.float32) -
                      ncp[ei].astype(np.float32)) / max_cp_f
        ego_cg = np.minimum(ncp[ei], state.n_global_cp - 1)
        our_cp_dx = state.global_cp_x[ego_cg] - px[oi]
        our_cp_dy = state.global_cp_y[ego_cg] - py[oi]
        our_cp_d = np.sqrt(our_cp_dx**2 + our_cp_dy**2 + 1e-8)
        o_vel_toward_our_cp = np.clip(
            (vx[oi] * our_cp_dx + vy[oi] * our_cp_dy) /
            (our_cp_d * _MAX_VEL), -1.0, 1.0)
        o_inferred_shd = opp_inferred_shield_pair[opp_pair_idx] / _SHIELD_MAX
        o_heading_cos = (np.cos(ang[oi]) * cos_a +
                         np.sin(ang[oi]) * sin_a)
        o_heading_sin = (np.sin(ang[oi]) * cos_a -
                         np.cos(ang[oi]) * sin_a)
        o_accel_x = vx[oi] / 0.85 - prev_vx[oi]
        o_accel_y = vy[oi] / 0.85 - prev_vy[oi]
        o_accel_mag = np.sqrt(o_accel_x**2 + o_accel_y**2 + 1e-8)
        o_inferred_boosted = np.where(o_accel_mag > _BOOST_DETECT_THRESH, 1.0, 0.0)
        opp_to_ego_x = px[ei] - px[oi]
        opp_to_ego_y = py[ei] - py[oi]
        opp_to_cp_x = state.global_cp_x[o_cg] - px[oi]
        opp_to_cp_y = state.global_cp_y[o_cg] - py[oi]
        d_o2e = np.sqrt(opp_to_ego_x**2 + opp_to_ego_y**2 + 1e-8)
        d_o2c = np.sqrt(opp_to_cp_x**2 + opp_to_cp_y**2 + 1e-8)
        blocking_align = np.clip(
            (opp_to_ego_x * opp_to_cp_x +
             opp_to_ego_y * opp_to_cp_y) / (d_o2e * d_o2c),
            -1.0, 1.0)
        return np.array([
            orl_x / _MAX_DIAG, orl_y / _MAX_DIAG,
            ovl_x / _MAX_VEL, ovl_y / _MAX_VEL,
            closing_o, o_face_ego, o_face_team,
            o_bounce, o_cp_progress, o_ncp_lead,
            o_vel_toward_our_cp, o_cp0_dist,
            o_inferred_shd, o_inferred_boosted,
            o_heading_cos, o_heading_sin,
            blocking_align,
        ])  # 17 base dims
    def _opp_tti_reach_hyp(oi):
        o_dx = px[oi] - px[ei]
        o_dy = py[oi] - py[ei]
        o_dvx = vx[oi] - vx[ei]
        o_dvy = vy[oi] - vy[ei]
        tti_o_val = _vieta_tti(o_dx, o_dy, o_dvx, o_dvy)
        opp_cp_g = np.minimum(ncp[oi], state.n_global_cp - 1)
        opp_cp_x = state.global_cp_x[opp_cp_g]
        opp_cp_y = state.global_cp_y[opp_cp_g]
        runner_x = px[ei]
        runner_y = py[ei]
        d_hyp_r, d_hyp_b, d_hyp_c = _multi_hyp_intent(
            px[ei], py[ei],
            px[oi], py[oi], vx[oi], vy[oi], ang[oi],
            opp_cp_x, opp_cp_y,
            runner_x, runner_y,
        )
        opp_dist = np.sqrt(o_dx**2 + o_dy**2 + 1e-8)
        collision_approach_cos = np.clip(
            (vx[ei] * o_dx + vy[ei] * o_dy) /
            (np.maximum(spd_ego, 1.0) * opp_dist),
            -1.0, 1.0)
        nx = o_dx / opp_dist
        ny = o_dy / opp_dist
        o_dx_cp = opp_cp_x - px[oi]
        o_dy_cp = opp_cp_y - py[oi]
        o_d_cp = np.sqrt(o_dx_cp**2 + o_dy_cp**2 + 1e-8)
        ram_quality = np.clip(
            -(nx * o_dx_cp + ny * o_dy_cp) / o_d_cp,
            -1.0, 1.0)
        opp_turn_rate = np.clip(
            prev_delta_ang[oi] / _MAX_TURN_RAD, -1.0, 1.0)
        vel_toward_ego = -(vx[oi] * o_dx + vy[oi] * o_dy) / opp_dist
        o_spd = np.sqrt(vx[oi]**2 + vy[oi]**2 + 1e-8)
        o_spd_to_cp = (vx[oi] * o_dx_cp + vy[oi] * o_dy_cp) / o_d_cp
        o_role = np.clip(
            (vel_toward_ego - o_spd_to_cp) / np.maximum(o_spd, 1.0),
            -1.0, 1.0)
        opp_vx_t, opp_vy_t = vx[oi], vy[oi]
        opp_px_t, opp_py_t = px[oi], py[oi]
        for _ in range(4):
            opp_px_t = opp_px_t + opp_vx_t
            opp_py_t = opp_py_t + opp_vy_t
            opp_vx_t = opp_vx_t * _FRICTION
            opp_vy_t = opp_vy_t * _FRICTION
        o_future_x_t4 = (opp_px_t - px[ei]) / _MAX_DIAG
        o_future_y_t4 = (opp_py_t - py[ei]) / _MAX_DIAG
        prev_spd = np.sqrt(prev_vx[oi]**2 + prev_vy[oi]**2 + 1e-8)
        curr_spd = np.sqrt(vx[oi]**2 + vy[oi]**2 + 1e-8)
        o_accel_trend = np.clip((curr_spd - prev_spd) / _MAX_VEL, -1.0, 1.0)
        opp_own_cp_dist = np.clip(o_d_cp / _MAX_DIAG, 0.0, 1.0)
        interpose_x = px[oi] + o_dx_cp * 0.4
        interpose_y = py[oi] + o_dy_cp * 0.4
        ip_dx = interpose_x - px[ei]
        ip_dy = interpose_y - py[ei]
        interpose_dist = np.clip(
            np.sqrt(ip_dx**2 + ip_dy**2 + 1e-8) / _MAX_DIAG, 0.0, 1.0)
        ip_local_x = ip_dx * cos_a + ip_dy * sin_a
        ip_local_y = -ip_dx * sin_a + ip_dy * cos_a
        interpose_bearing = np.clip(
            np.arctan2(ip_local_y, ip_local_x + 1e-8) / np.pi,
            -1.0, 1.0)
        return np.array([
            tti_o_val, o_role, collision_approach_cos,
            ram_quality, opp_turn_rate,
            d_hyp_r, d_hyp_b, d_hyp_c,
            o_future_x_t4, o_future_y_t4, o_accel_trend,
            opp_own_cp_dist, interpose_dist, interpose_bearing,
        ])  # 14 extended dims (was 11 + 3 new V14 interposition)
    feat_d = np.concatenate([
        _opp_block(o0, 0), _opp_tti_reach_hyp(o0),
        _opp_block(o1, 1), _opp_tti_reach_hyp(o1),
    ])  # 62 dims (17 + 14 per opp × 2)
    d_o0 = np.sqrt(
        (px[o0] - px[ei])**2 + (py[o0] - py[ei])**2 + 1e-8)
    d_o1 = np.sqrt(
        (px[o1] - px[ei])**2 + (py[o1] - py[ei])**2 + 1e-8)
    min_opp_dist = np.minimum(d_o0, d_o1) / _MAX_DIAG
    def _turn_sev(off):
        c0 = np.minimum(ncp[ei] + off, cp_pad_limit)
        c1 = np.minimum(ncp[ei] + off + 1, cp_pad_limit)
        c2 = np.minimum(ncp[ei] + off + 2, cp_pad_limit)
        d1x = state.global_cp_x[c1] - state.global_cp_x[c0]
        d1y = state.global_cp_y[c1] - state.global_cp_y[c0]
        d2x = state.global_cp_x[c2] - state.global_cp_x[c1]
        d2y = state.global_cp_y[c2] - state.global_cp_y[c1]
        ta = np.arctan2(d2y + 1e-8, d2x + 1e-8) - np.arctan2(
            d1y + 1e-8, d1x + 1e-8)
        return np.array([np.cos(ta), np.sin(ta)])
    feat_e = np.concatenate([
        _turn_sev(0),
        np.array([min_opp_dist]),
    ])  # 3 dims
    def _frac_prog(pod_i):
        cg = np.minimum(ncp[pod_i], state.n_global_cp - 1)
        ddx = state.global_cp_x[cg] - px[pod_i]
        ddy = state.global_cp_y[cg] - py[pod_i]
        dd = np.sqrt(ddx**2 + ddy**2 + 1e-8)
        return (ncp[pod_i].astype(np.float32) +
                1.0 - np.clip(dd / _MAX_DIAG, 0.0, 1.0)) / max_cp_f
    ego_frac = _frac_prog(ei)
    o0_frac = _frac_prog(o0)
    o1_frac = _frac_prog(o1)
    best_opp_frac = np.maximum(o0_frac, o1_frac)
    best_opp_lead = np.clip(best_opp_frac - ego_frac, -1.0, 1.0)
    t_frac = _frac_prog(ti)
    team_lead = np.clip(t_frac - ego_frac, -1.0, 1.0)
    feat_f = np.array([best_opp_lead, team_lead])  # 2 dims
    friction = _FRICTION
    ego_vx, ego_vy = vx[ei], vy[ei]
    ego_px_t, ego_py_t = px[ei], py[ei]
    for step in range(3):
        ego_px_t = ego_px_t + ego_vx
        ego_py_t = ego_py_t + ego_vy
        ego_vx = ego_vx * friction
        ego_vy = ego_vy * friction
    future_x_t3 = (ego_px_t - px[ei]) / _MAX_DIAG
    future_y_t3 = (ego_py_t - py[ei]) / _MAX_DIAG
    cg_eta = np.minimum(ncp[ei], state.n_global_cp - 1)
    d_eta = np.sqrt(
        (state.global_cp_x[cg_eta] - px[ei])**2 +
        (state.global_cp_y[cg_eta] - py[ei])**2 + 1e-8)
    effective_speed = np.maximum(spd_ego * _FRICTION, 1.0)
    cp_eta = np.clip(d_eta / effective_speed / _CP_ETA_MAX_TURNS, 0.0, 1.0)
    def _min_approach(oi):
        opx, opy = px[oi], py[oi]
        ovx, ovy = vx[oi], vy[oi]
        epx, epy = px[ei], py[ei]
        evx, evy = vx[ei], vy[ei]
        min_d = np.sqrt((opx - epx)**2 + (opy - epy)**2 + 1e-8)
        for _ in range(6):
            opx = opx + ovx
            opy = opy + ovy
            ovx = ovx * friction
            ovy = ovy * friction
            epx = epx + evx
            epy = epy + evy
            evx = evx * friction
            evy = evy * friction
            d = np.sqrt((opx - epx)**2 + (opy - epy)**2 + 1e-8)
            min_d = np.minimum(min_d, d)
        return min_d / _MAX_DIAG
    intercept_risk_o0 = _min_approach(o0)
    intercept_risk_o1 = _min_approach(o1)
    intercept_risk = np.clip(
        1.0 - np.minimum(intercept_risk_o0, intercept_risk_o1), 0.0, 1.0)
    feat_fe = np.array([
        future_x_t3, future_y_t3,
        cp_eta, intercept_risk,
    ])  # 4 dims (was 10, freed 6 for V14 interposition)
    return np.concatenate([
        feat_a, feat_b, feat_c, feat_d, feat_e, feat_f, feat_fe,
    ])  # 14+10+13+62+3+2+4 = 108 dims
def _build_global_synergy(
    state,
    px: np.ndarray,
    py: np.ndarray,
    vx: np.ndarray,
    vy: np.ndarray,
    ncp: np.ndarray,
    shield: np.ndarray,
    prev_delta_ang_all: np.ndarray,
    prev_thrust_all: np.ndarray,
    player,
) :
    max_cp_f = np.maximum(state.n_global_cp, np.int32(1)).astype(
        np.float32)
    def _pincer(oi):
        d_p0 = np.sqrt(
            (px[0] - px[oi])**2 + (py[0] - py[oi])**2 + 1e-8)
        d_p1 = np.sqrt(
            (px[1] - px[oi])**2 + (py[1] - py[oi])**2 + 1e-8)
        dist_diff = (d_p0 - d_p1) / _MAX_DIAG
        v0x = (px[0] - px[oi]) / d_p0
        v0y = (py[0] - py[oi]) / d_p0
        v1x = (px[1] - px[oi]) / d_p1
        v1y = (py[1] - py[oi]) / d_p1
        pincer_cos = v0x * v1x + v0y * v1y
        return np.array([dist_diff, pincer_cos])
    feat_g = np.concatenate([_pincer(2), _pincer(3)])  # 4 dims
    pod_speeds = np.array([
        np.sqrt(vx[i]**2 + vy[i]**2 + 1e-8) / _MAX_VEL
        for i in range(4)
    ])  # 4 dims
    pod_cp_prog = np.array([
        ncp[i].astype(np.float32) / max_cp_f for i in range(4)
    ])  # 4 dims
    def _pod_dist_to_cp(i):
        cg = np.minimum(ncp[i], state.n_global_cp - 1)
        dx = state.global_cp_x[cg] - px[i]
        dy = state.global_cp_y[cg] - py[i]
        return np.sqrt(dx**2 + dy**2 + 1e-8) / _MAX_DIAG
    pod_dist_cp = np.array([_pod_dist_to_cp(i) for i in range(4)])  # 4
    pod_shields = np.array([
        shield[i] / _SHIELD_MAX for i in range(4)
    ])  # 4 dims
    team_spread = np.sqrt(
        (px[0] - px[1])**2 + (py[0] - py[1])**2 + 1e-8) / _MAX_DIAG
    opp_spread = np.sqrt(
        (px[2] - px[3])**2 + (py[2] - py[3])**2 + 1e-8) / _MAX_DIAG
    feat_h = np.concatenate([
        pod_speeds, pod_cp_prog, pod_dist_cp, pod_shields,
        np.array([team_spread, opp_spread]),
    ])  # 18 dims
    cp_prog = ncp.astype(np.float32) / max_cp_f
    timeout_self = state.player_timeout[player].astype(np.float32) / _TIMEOUT_MAX
    timeout_opp = state.player_timeout[1 - player].astype(
        np.float32) / _TIMEOUT_MAX
    lead = (np.maximum(ncp[0], ncp[1]).astype(np.float32) -
            np.maximum(ncp[2], ncp[3]).astype(np.float32)) / max_cp_f
    cp_prog_diff_team = cp_prog[0] - cp_prog[1]
    cp_prog_diff_opp = cp_prog[2] - cp_prog[3]
    def _team_frac_prog(pod_i):
        cg = np.minimum(ncp[pod_i], state.n_global_cp - 1)
        ddx = state.global_cp_x[cg] - px[pod_i]
        ddy = state.global_cp_y[cg] - py[pod_i]
        dd = np.sqrt(ddx**2 + ddy**2 + 1e-8)
        return (ncp[pod_i].astype(np.float32) +
                1.0 - np.clip(dd / _MAX_DIAG, 0.0, 1.0)) / max_cp_f
    our_best_frac = np.maximum(_team_frac_prog(0), _team_frac_prog(1))
    opp_best_frac = np.maximum(_team_frac_prog(2), _team_frac_prog(3))
    true_team_lead = np.clip(our_best_frac - opp_best_frac, -1.0, 1.0)
    feat_i = np.array([
        timeout_self, timeout_opp, lead,
        cp_prog_diff_team, cp_prog_diff_opp,
        true_team_lead,
    ])  # 6 dims
    pod_prev_da = np.array([
        np.clip(prev_delta_ang_all[i] / _MAX_TURN_RAD, -1.0, 1.0)
        for i in range(4)
    ])  # 4 dims
    feat_j = pod_prev_da  # 4 dims
    cp_diff_team = np.abs(ncp[0].astype(np.float32) -
                            ncp[1].astype(np.float32)) / max_cp_f
    team_cp_coverage = np.clip(cp_diff_team, 0.0, 1.0)
    d_opp0 = np.sqrt((px[2] - px[0])**2 + (py[2] - py[0])**2 + 1e-8)
    d_opp1 = np.sqrt((px[3] - px[0])**2 + (py[3] - py[0])**2 + 1e-8)
    d_opp2 = np.sqrt((px[2] - px[1])**2 + (py[2] - py[1])**2 + 1e-8)
    d_opp3 = np.sqrt((px[3] - px[1])**2 + (py[3] - py[1])**2 + 1e-8)
    min_opp_dist_team = np.minimum(
        np.minimum(d_opp0, d_opp1),
        np.minimum(d_opp2, d_opp3))
    proximity = np.clip(1.0 - min_opp_dist_team / (10.0 * 800.0), 0.0, 1.0)
    best_ram_opportunity = proximity  # [0, 1] — high when collision is imminent
    d0_cp = np.sqrt(
        (state.global_cp_x[np.minimum(ncp[0], state.n_global_cp - 1)] - px[0])**2 +
        (state.global_cp_y[np.minimum(ncp[0], state.n_global_cp - 1)] - py[0])**2 + 1e-8)
    d1_cp = np.sqrt(
        (state.global_cp_x[np.minimum(ncp[1], state.n_global_cp - 1)] - px[1])**2 +
        (state.global_cp_y[np.minimum(ncp[1], state.n_global_cp - 1)] - py[1])**2 + 1e-8)
    role_clarity = np.clip((d1_cp - d0_cp) / _MAX_DIAG, -1.0, 1.0)
    best_cp = np.maximum(ncp[0], ncp[1]).astype(np.float32)
    race_phase = np.clip(best_cp / max_cp_f, 0.0, 1.0)
    timeout_self_k = state.player_timeout[player].astype(np.float32)
    elimination_risk = np.clip(1.0 - timeout_self_k / _TIMEOUT_MAX, 0.0, 1.0)
    timeout_opp_k = state.player_timeout[1 - player].astype(np.float32)
    opp_elimination_risk = np.clip(1.0 - timeout_opp_k / _TIMEOUT_MAX, 0.0, 1.0)
    turn_f = np.maximum(state.turn.astype(np.float32), 1.0)
    scoring_velocity = np.clip(best_cp / turn_f, 0.0, 1.0)
    opp_best_cp = np.maximum(ncp[2], ncp[3]).astype(np.float32)
    opp_scoring_velocity = np.clip(opp_best_cp / turn_f, 0.0, 1.0)
    feat_k = np.array([
        team_cp_coverage, best_ram_opportunity, role_clarity, race_phase,
        elimination_risk, opp_elimination_risk,
        scoring_velocity, opp_scoring_velocity,
    ])  # 8 dims
    return np.concatenate([feat_g, feat_h, feat_i, feat_j, feat_k])  # 40 dims
def observation_v7(
    state,
    player,
    ego_pod = 0,
) :
    abs_idx_0 = player * 2
    team_idx_0 = player * 2 + 1
    opp0_idx = (1 - player) * 2
    opp1_idx = (1 - player) * 2 + 1
    order_0 = np.array([abs_idx_0, team_idx_0, opp0_idx, opp1_idx])
    abs_idx_1 = player * 2 + 1
    team_idx_1 = player * 2
    order_1 = np.array([abs_idx_1, team_idx_1, opp0_idx, opp1_idx])
    rs_base = player * 2
    def _extract(order, ego_abs):
        return (
            state.pod_x[order],
            state.pod_y[order],
            state.pod_vx[order],
            state.pod_vy[order],
            state.pod_angle[order],
            state.pod_next_cp[order],
            state.pod_shield_timer[order],
            state.pod_collided[order],
            state.prev_pod_vx[order],
            state.prev_pod_vy[order],
            state.prev_delta_ang[order],
            state.prev_thrust[order],
            state.pod_boosted[order],
            ego_abs,
        )
    (px0, py0, vx0, vy0, ang0, ncp0, sh0, col0,
     pvx0, pvy0, pda0, pt0, bst0, ego0) = _extract(order_0, abs_idx_0)
    (px1, py1, vx1, vy1, ang1, ncp1, sh1, col1,
     pvx1, pvy1, pda1, pt1, bst1, ego1) = _extract(order_1, abs_idx_1)
    opp_shield_pair = state.opp_inferred_shield[rs_base:rs_base + 2]
    opp_mr_pair = state.opp_collision_mass_ratio[rs_base:rs_base + 2]
    radar_0 = _build_ego_radar(
        state, px0, py0, vx0, vy0, ang0, ncp0, sh0, col0,
        pvx0, pvy0, pda0, pt0, ego0, opp_shield_pair, opp_mr_pair, bst0)
    radar_1 = _build_ego_radar(
        state, px1, py1, vx1, vy1, ang1, ncp1, sh1, col1,
        pvx1, pvy1, pda1, pt1, ego1, opp_shield_pair, opp_mr_pair, bst1)
    global_sh = state.pod_shield_timer[order_0]
    global_pda = state.prev_delta_ang[order_0]
    global_pt = state.prev_thrust[order_0]
    global_syn = _build_global_synergy(
        state, px0, py0, vx0, vy0, ncp0, global_sh,
        global_pda, global_pt, player)
    return np.concatenate([radar_0, radar_1, global_syn]).astype(
        np.float32)
# ===== CG I/O =====
MR=18.0*pi/180.0
MT=200.0;BT=650.0;SM=4;TM=100
PD=800.0;MD=18358.0
ROTS=[-MR,0.0,MR]
THRUSTS=[(0.0,False,False),(MT,False,False),(BT,True,False),(0.0,False,True)]
def decode_action(a,ang,px,py,sh_cd,used_boost):
    ri=a%3;ti=a//3
    rot=ROTS[ri];new_ang=ang+rot
    thr,is_boost,is_shield=THRUSTS[ti]
    if is_shield and sh_cd>0:thr=0;is_shield=False;is_boost=False
    if is_boost and used_boost:thr=MT;is_boost=False
    tx=int(round(px+cos(new_ang)*10000))
    ty=int(round(py+sin(new_ang)*10000))
    if is_shield:return f"{tx} {ty} SHIELD"
    if is_boost:return f"{tx} {ty} BOOST"
    return f"{tx} {ty} {int(thr)}"
def get_mask(sh_cd,used_boost):
    m=np.ones(12,dtype=bool)
    if sh_cd>0:m[3:]=False
    else:
        if used_boost:m[6:9]=False
    return m
# ===== GAME STATE → V7GameState =====
from collections import namedtuple
_fields = ['pod_x','pod_y','pod_vx','pod_vy','pod_angle','pod_next_cp',
           'pod_shield_timer','pod_boosted','player_timeout',
           'global_cp_x','global_cp_y','n_global_cp','turn',
           'pod_won','num_unique_cp','unique_cps','player_oob',
           'collision_count','min_cross_dist','prev_pod_angle',
           'prev_pod_vx','prev_pod_vy','prev_delta_ang','pod_collided',
           'prev_thrust','opp_inferred_shield','opp_collision_mass_ratio']
GS = namedtuple('GS', _fields)

laps=int(input())
ncp=int(input())
cpx=[];cpy=[]
for _ in range(ncp):
    cx,cy=map(int,input().split())
    cpx.append(cx);cpy.append(cy)
total_cp=ncp*laps+1
gcpx=[];gcpy=[]
for l in range(laps):
    gcpx.extend(cpx);gcpy.extend(cpy)
gcpx.append(cpx[0]);gcpy.append(cpy[0])  # finish line
_gx=np.zeros(30);_gy=np.zeros(30)
for i in range(min(len(gcpx),30)):
    _gx[i]=gcpx[i];_gy[i]=gcpy[i]
_ucp=np.zeros((8,2))
for i in range(min(ncp,8)):
    _ucp[i]=[cpx[i],cpy[i]]

prev_ang=np.zeros(4)
prev_da=np.zeros(4)
prev_vfwd=np.zeros(4)
prev_vlat=np.zeros(4)
prev_thrust=np.zeros(4)
pod_shield_cd=np.zeros(4,dtype=np.int32)
pod_boosted=np.zeros(4,dtype=np.bool_)
opp_shield=np.zeros(4)
opp_mr=np.zeros(4)
prev_vx=np.zeros(4);prev_vy=np.zeros(4)
first_turn=True
last_actions=[1,1]
turn=0
while True:
    pods=[]
    for i in range(4):
        parts=input().split()
        pods.append([int(parts[j]) for j in range(6)])
    turn+=1
    px=np.array([p[0] for p in pods],dtype=np.float64)
    py=np.array([p[1] for p in pods],dtype=np.float64)
    vx=np.array([p[2] for p in pods],dtype=np.float64)
    vy=np.array([p[3] for p in pods],dtype=np.float64)
    ang_deg=np.array([p[4] for p in pods],dtype=np.float64)
    ncp_idx=np.array([p[5] for p in pods],dtype=np.int32)
    ang_rad=ang_deg*pi/180.0
    if first_turn:
        prev_ang=ang_rad.copy()
        prev_vx=vx.copy();prev_vy=vy.copy()
    # Delta angle
    da=np.zeros(4)
    if not first_turn:
        for i in range(4):
            d=ang_rad[i]-prev_ang[i]
            da[i]=atan2(sin(d),cos(d))
    # Body-frame velocities for prev
    if not first_turn:
        for i in range(4):
            c=cos(prev_ang[i]);s_=sin(prev_ang[i])
            prev_vfwd[i]=(vx[i]*c+vy[i]*s_)/800.0
            prev_vlat[i]=(-vx[i]*s_+vy[i]*c)/800.0
    # Collision detection
    col=np.zeros(4,dtype=np.int32)
    for i in range(4):
        for j in range(i+1,4):
            d=sqrt((px[i]-px[j])**2+(py[i]-py[j])**2)
            if d<PD+1:col[i]+=1;col[j]+=1
    # Shield tracking for our pods
    if not first_turn:
        for i in range(2):
            a=last_actions[i]
            ti=a//3
            if ti==3:pod_shield_cd[i]=SM
            else:pod_shield_cd[i]=max(pod_shield_cd[i]-1,0)
            if ti==2 and not pod_boosted[i]:pod_boosted[i]=True
    # Thrust type for our pods
    if not first_turn:
        for i in range(2):
            a=last_actions[i];ti=a//3
            if ti==3:prev_thrust[i]=-1.0
            elif ti==2:prev_thrust[i]=1.0
            elif ti==1:prev_thrust[i]=200.0/650.0
            else:prev_thrust[i]=0.0
    # Opponent shield inference
    if not first_turn:
        opp_shield=np.maximum(opp_shield-1,0)
        for opp in range(2,4):
            dvx=vx[opp]-prev_vx[opp];dvy=vy[opp]-prev_vy[opp]
            dvm=sqrt(dvx**2+dvy**2+1e-8)
            mr=min(dvm/400.0,3.0)/3.0
            opp_mr[opp]=mr
            if mr>0.5:opp_shield[opp]=SM
    # Build V7GameState
    state=GS(
        pod_x=px,pod_y=py,pod_vx=vx,pod_vy=vy,
        pod_angle=ang_rad,pod_next_cp=ncp_idx,
        pod_shield_timer=pod_shield_cd.astype(np.float64),
        pod_boosted=pod_boosted,
        player_timeout=np.array([TM,TM],dtype=np.float64),
        global_cp_x=_gx,global_cp_y=_gy,
        n_global_cp=np.int32(total_cp),
        turn=np.int32(turn),
        pod_won=np.zeros(4,dtype=np.bool_),
        num_unique_cp=np.int32(ncp),
        unique_cps=_ucp,
        player_oob=np.zeros(4,dtype=np.bool_),
        collision_count=col,
        min_cross_dist=np.full(4,10000.0),
        prev_pod_angle=prev_ang.copy(),
        prev_pod_vx=prev_vx.copy(),
        prev_pod_vy=prev_vy.copy(),
        prev_delta_ang=prev_da.copy(),
        pod_collided=(col>0).astype(np.float64),
        prev_thrust=prev_thrust.copy(),
        opp_inferred_shield=opp_shield.copy(),
        opp_collision_mass_ratio=opp_mr.copy(),
    )
    # Build obs using exact training code
    obs=observation_v7(state,player=0)
    obs=np.array(obs,dtype=np.float32)
    obs=quantize_obs(obs)
    # Forward pass
    logits=forward(obs)
    # Action masking
    m0=get_mask(pod_shield_cd[0],pod_boosted[0])
    m1=get_mask(pod_shield_cd[1],pod_boosted[1])
    jm=np.outer(m0,m1).flatten()
    logits[~jm]=-1e9
    ja=int(np.argmax(logits))
    a0=ja//12;a1=ja%12
    last_actions=[a0,a1]
    cmd0=decode_action(a0,ang_rad[0],px[0],py[0],pod_shield_cd[0],pod_boosted[0])
    cmd1=decode_action(a1,ang_rad[1],px[1],py[1],pod_shield_cd[1],pod_boosted[1])
    print(cmd0)
    print(cmd1)
    prev_ang=ang_rad.copy()
    prev_da=da.copy()
    prev_vx=vx.copy();prev_vy=vy.copy()
    first_turn=False
