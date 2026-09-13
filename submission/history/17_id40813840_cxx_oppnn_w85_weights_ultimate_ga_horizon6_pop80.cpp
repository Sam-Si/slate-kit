#pragma GCC optimize("Ofast,unroll-loops,inline,omit-frame-pointer")
#pragma GCC target("avx2,fma,bmi,bmi2")
#include <array>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace opp_nn {
static const char W85[]="RbTDE<QP^$et8%_*16ih)n2$mMOmsvjK68U>B4cs5d&62e!{T7B$^*TPr|-KJ9Df>Wnv|~RbTDE<QP^$et8%_*16ih)n2$mMOmsvjK68U>B4cs5d&62e!{T7B$^*TPr|-KJ9Df>Wnv|~x$1B`>-zFSoffz|X6y4pKo8o!H`Ls|YV-m{i8KR600000y2k}WJTIh0`MX+09J!-I$K8lSmFHGNIs`{We3{Qbzj&lY&%j<q>maE^+v<x$#4uSy;59`>i<8JdJTIh0`MX+09J!-I$K8lSmFHGNIs`{We3{Qbzj&lY&%j<q>maE^+v<x$#4uSy;59`>i<8Jd;}42H&{w%aY2k@Je9*W;Hm3VNqLunSrEs-GLgvRrOsUjBSaZ8VUn!`nPBih9c`a=!CN`rs<eSGHcc`$FS~9JwJ+8?h0jVN1xU^^-P&J0EFfE9*!YoWRUl@=k%O&`+y|V(ar8OO>sIaxIMzWnF<SLD<;jIL*6R{?+QLn$Uv#Y@*kg7nnjxHoA0h#<Ot}$#ei7*4P=_V*03@8(^DXglZZ!h*Kjj-G*@2apV^f0$CY^R8;i?xa`=O(c={1NW5Cb3;91U71^p{?Dr{w`fAN*SytX|nS!nkbqv+$gTC(<N&viY#NZm9(`id!neY+^Y31{Gbb{CNlUiL9O#BR3N7|P^`TzSTXakY%Y&4<1-|yIWPYo8m+H4hO%BIb0bNsc&y4Lai)!<tgL+_9<If(g)J~Bt}+ZV;VpZp_asBBYOOD;0X4I!>#R7jQ!flN^`iMO;54ZzU9=Q1HLZs=r>nK7+ObbHSeTVCFC{;&&#ks4RVN9dgRwHKVle@*bS%&|QKsghdMD*BhAHJOpere<d$IedlQ0Xb<S&Gw&MQGMtFEl3l`xGjoGaC^Jucv>9<4L0bEqgVXCe7Gfh|BaC?hkdS1b{v7O-U@si-@n6*zRQ_^hof7^Ge)#xXOn9J9bMho<%}At6Jq+ZE0&hpo0Na4!q6Q7&bu#4z_TwkpLaDKkW{5Ht#|b+k^TK`;?Cy{Rm!mnZkI4Kl?VpRL#~fH2Fo@~cED$T2Xh#3j0_A~>!hoiY9;YOxfs>N0e#9V}6?jISo5ktXP_V>DZ~=dco_Wi)=N8mDe2tF892%c)kk4J?r_jWK^RVW%;*S+OEDSt=ti(>0Q<dL_sx^{t{X@UizQUNY&pH!R~T0;1iijU=+L1T9!4ptLI}3p3C(Ij|(LvZ|@EN3Gnh#x8KSLoqF~t1$(#h$~dA2Qoi1nXyPNAF@=kCa}IQ&?qFLz9&tu)3VF8cQTH+Fszmv88vY<Myz`;iZpGo8!`u}!LIcm8?#2M*fXrC-Y*}hy)@va0yy)mo~&@N)GS@9Tb5KV&@v_}7_9>x$E0g9{H-)DP$*|EfU>$Z`zBu~IjwLfDl)}0q_A@-O|+#nDx~Nz7NcAza;@*O2dc(0%`jy#?jeXO;Hj`O5HV0A7%eU}tFcxobTad>g0iKv>ZndEDm3jeKAg9!Hnejo`Kl5yEwHhsNUP1N`LH6VsI11V)2}Nt@vcKIx-f63;xO1Tvn{r<3pKK|i>$mfQ!Gj-CN+jF?Xs@3`>yM^DXrZr1u|}+!7`1lb~)}eH6#Qv+_C5@SFyJ&6sbI{L^&C;;3rwIq^zZ<XE8>ph9|DJ7O;Y>rn3k!6sET@xv<T%#w-IaXfcW;_NjL;jxZgrf+%7vBPznGtTRzA6D-%PkS*UVbhM7BWUqUi=PmBAVKbknakB9exv^%k<E~&NahhbafgC6>J0a&V{IV&sn=v*Lekt^@GBDaFF`@G#ysh4@C$%4{RwIqH3o1k?xHL4Rq@j{3*`pe>CMtolf2>?8j<7=>GBwDe0j88SPA>v4A}kasHZ#>NQl(ln6epdeYcn{qbSJ{I39T9@S}$X=<S41IqBLeOGc@k7f2%e$@+*@o$g&=)M;@Ih@iNu7`7$K40WQF_#3sj}Ni8HQcq-I3VKUq-5ixtADz1gGc`N}d1t)kgR4!Yv%(V5br7t-!Nv(ag*rsPLkg;&9j;v{{a<f0O03?PoH!hkm0Ik`uIW<NsftpY!9k2#8qAYbNPpo;eh^Mcxi=-(wE4C~$tfVTqzcdM~G_iuH3oA=Coi%)^nlW22tS5IcfUbEoXRhn0cdL;#Po`BfyP@$dP%kDbd#DGs;gljO>aoV8FQ#UzyeTrUsVrPGTq|EGm#QBoB&~`v0jvU{f~<|JI4ssIK`LG>tSa{$0w}jIhOG&$>M`XRgd_sA5-}sRi7^)}a404+lr!HgleHTr=qfg@%pSTb!W+l50yGw^+b)8wE3Wb>mMjG|DWz_(eWaPLBq}$q)hbM|Au>-JX|FRazoKI;4=dIops7?KPp*fmlq5H?!Y=KqY$md=0<b5jPOGc3i?FgR53WNYXd#^_QYEgaexR?YBB=;0%PIo1;U$KyJg6J1)fwC?+$4*!044&oovHF7!!HD$#UuzUDz0%erm><iXC+@E1T;RW$t#00ke4tm0<b5sy(;ObHKdNM?yw~(wkqtY7p;6Gs2?Y)AFJIcsI!nQSS-?@e5?C4&$C^y1Ehs6f+({v*f0vLxh&N(k)@fmpfcB`U8`cH%_Mu7#3cS8og3D)BrpuCBrZHCPN<uzTAsNtM=;>AgsT0o*D(q#gfVliD>0O>2C9m!D=;W9tufNFU@595_AAt>1)=sWC^IiMOsR4$q$Zy&{xlvo7^+b%;4F|cK&MKut}Y*_0jxkYvMk}GpB^K!ZZo;Cs2|v|M>)B#Qn3QBldO}qKqB-SMY8WQPcBci)vj@_NUq%~1~3G)p)Z}Yhn=0Egr)B-)vbLk%Pmtdb21jH4XXF2V<>emV<=KJDJWbmoH3^~x3WJfO|yV7i7l!xwHkV||1o&92Czae)GTzX2C3PCMzGr;5~vt7l&UQ>8m_3ei6A?%v$JY2ur!^h+pHxdEvzLo`X&G%J+aa*bS$SeRxK$s%drz7TQx#2jy0RBQLc@!ytn0}qOhs5r7vkU4KbxKS1Z6JF_Ac{vN0X7LaGNW?W~q8QMK0_{j^W3eyba%<uJ^ud^69e6fdl=N++8y0w6Rnd?f<1<0J>F)v**YSTTmGovJ&wCajRKm9RIcOtM-vi=k>J%CD`gZkv;~w=C-_)t{lG=&V^XsIVY4T`0S&s;*ZkTQSI`ezDgtW-+3z9jmD$05ZZb=AbaNtto}4<1S^ilPpNF88MWz^D;NI9;x>)W3p|f>9JWVWvqrUSTX~xMlQ=SdM2hW#3|D+7BZ<WSE8G;7p=c70Vx|KEi(A6IWiA5A)OJkk|fKg`L78wVy*z7>#S<3C!<cSlQXcahb@IA8>|5+P%_o5Kdfyk$2bGCX&9(A$gvEua53;TJDOuD121l={-PeSaWX8dzp_WP=dgk-C8KyOZYqtk6R|V2uQYlsVk?y`Y%9F1D=`VQ)3BklZKww?nJ}=lag(?$A*>58q&T%Q0-XINjwpMji>vlCM<_J8zO<L54zFGx%_jM$468COT(tBn{i;*1ZnAGJn6P{@g)(R~F)bFic`md!4z5Tx;xjR~vMr4=tFgzVHKGtErzHn61GFSB_9gnTg0l!HRJI+okSV^Z*D~QWFD>;Y6f#XEF*D<^vn@d=;IIX+7A!C{WvN%Hv9De)39;QS7cXC~a5GM-;4L&SI5Vg#^cs33=deF5dak^yKQY)bfhHd_Ei4-?EgnU!->R!LQmn<P%qSP6B`r;~=pgnd;;9R-hAUDu;4>$&7cr$M-=tD0ovrn%Z7Bsf46ej65u;_Old)p5U9nNGJu|~Kji!00Ffw%#v!c%{-=!6-<}87)KdL{i`ZK7s6u3t&v#qo&`l}zTb2F$k#4WhB<}KZ&#;;B*i7rMkrzCx`OR!S4i>^twnXTxp$uitA6|VxfNHa|=0jQ8J2qergey$55e5SpvP&rd56{u3N3bHw=OD>@-B`v0}^{<trs;!Nz{Vi%SA+!gtsWZ(oGA@)K>9puHlAf`&qAr>&G%>m?3Z}X%!83fQs4t7CjkKyPl`J?YizkJ!x+oN{)Uk>$oGxECEU)*jS|P79U$%}h94z>yQ!tCJlcy||;GT=N(61h@d?y{Lr7xi>V<x^T{;8j|Yp~TTlrIP(92w0l*r;!`K&4Qy?yTY|7_=)eEHou4nl4Z>4XyLAVxinCgd~x*VJ7n|8!4|UkD59#MKExz$*tcs>oP4gD=Fu*(=;bA1TelWh%q`e5i3Hmd9>apVk*!p`YsVSMzoZz6sNhWgoFLB(xn=&OfG&l#4aVK+$$q0wI;^1b+UIW_%IEyx~HqIAT&QP123H~#4Aa+9V{%cv9PbLw=SY7>@wuF!>dgy#wWcgST8N44zNNhwWU0&c_vY_>!dlM;x7)MzA<a9B@vV~aVtGA^ehk>cPi7U3@l%)im&Iio-b@D37mvAqbY*0#;nFMF{0xs$0cvDHL%IEwi5O%TChm5s;c`b0j-Fqw5Z}QK&#xTe6XD{nl11#xvjY@4Jk=6!YHaQqqCJE)i8{)53MOJJ*grz^e(@u*DP(VxFO!IUNHA87OxR7$*?CdFErw@jV8z}O)}Ok+^WVa!6RfUFDYUyE3FTv_N$DmTqF-M5u<4@S1?5}2`(+L5HK2~#VVtyTPs%r!zY#^k|NbGb0jD$UMxYQP%iK&8mv~UKr6GbE-zv$Pcb<vpQ2MN(WzaqE3hR6pR8u3bFNo11uy_98Y0{zYqOjxik)dMeyAO@4lG+Nbgk+vrmpHFlPEf}Ag7e5#vL#&Xq+i4oUwWVyRBX>bs|kAbuy2xVyQtS)+Ib8?w^vayse_5BCOFg>N9X8CMM1;VXrrmu&e2_?<mkIr7PsI`YK5+H7NryODo?hLM%2jnXgQ(kFYSSe=9*T6sssN)tm}3DyAl{TOjDK{v+EX7#@PJ1trWY(I=fQj4r0DJSruyBPrOi$ge)Iv8{(OR<IT@IVW1Li7DAG#U*Ji+pzjDVXYRdKsN0w>Mo_LoheH-=Q7bT+p!z7QMP}#kTDCZXSpFV>9K_^GBW`#)vHA(AGC9_6r;+eb~JG;@wf{r5v>d*SFL8S$uMdw#iUU$jWF`HJ~W`Fa<0X&%ORIF?yX<9Ju|wrBO-~Z3o^Z<O)BBEh9N~VAhhW)8#Cyvb}#KITd}Y;9;`00Dy*oqgfP^%fi+AkkgQRtSgo?Ct+hF%gf)X9)3Mw$6SfL5`!wV*)3Xq*n6Aw)V6iMU?k2FS@GA$hjI=MM+oVOUK`F#HVH#d2y)h-J-mGD)bgKF&j<zhXlCPU8P%C;g9;qNDsIwZWg*4kO9x4v9{jj7kXEb>)<S?M9IJGb{bT7^`3a|~Z1+uWRm^Co1t}sBd+$vixC@iR-|1J?Q@UNRGU6jzM1~WD%`?nk^La>Rh&Z(*_9<%5!YcYK$ezdHz0xTRjF|y5~6Dnn^d!|V=%`<4V@Fs<)V6ZT;=%KhYM6Q@9C$OTjgs9N0^&>y6-!gYDh@r7HXRoTO=sC_d?k$F@xv-qA>@18S2edn^-J&+Hlep+9e6jYhEH4)?POZBu+_h`24m4&hV5#UNYc#sF@2wRtyeQYR6tz6ElrGPreJJT3Z#HDJbRY?+hOF{0@~h=5LNE)rajkVQIk71qUN`uwk(mv%6`~cZlO+PH<);Uqfw{IZ#<=&X#jzJCb*81JwzQ$7S-2}HF|mXpuQ8~xiZKB+=&gRRF(L1=7&V+UpRysen709(IkeofBCyUe6tIV_$St=sr;5xJ&?~;LKqVWmVxe`m8M#_6E~3w?POe@h#ICP2psmL`7_M|V?J|(A#xPSIX)U|04JB%}e6TdK@jEFn2Qd4qv8~9k7qGgrz_s@=cQJ9X7&>k;<+I?jp)(7ll&l4=bFPr8YOI$uJSsA#G`K;s3bJ4zG^E3<yRr~4@gXUt53;7E(Ktmg{H$^>Q8Mc<$S;nq&M^A4;jGfDCN<0_tuH(<d$9;L1g_sQwmCa0f~Oc4BcS=Ltg^x^!!TX7{;l|~&#XG2Jgs1}vNW5eKdZ;Aw7B)Lb}s?4ZlgA^613SZ3Oe^N=B7n3xVIInRxzQp=q!h@sW1t(FEbRZVy<wiA1Nd@dMBl{;xMeMnifhM(kgeaf3qkxr7ub*elr0w=P!ez;h=FcX*l4ztRzXZp|CKsH>(@4%%77rg}5oQkTK6PdNTR7eYe0gnkM`-!!wes_BBEhbg|T-XR{Z$gf_M+WUY9iG_}UA`6>3T1+CL9LAidov?}r_bFjala4l1-f-np=S~2^w|1q4cMm2vmuQ94LSESXgnKDeKAucB@G!ch5aV?Ulb11W@KdgQ<KQ)9ghc;|3#W5DDEwnwS&!BfP&$6zeUMqt!;4PadNHzE-O{nN8dbRf|Wh~OJcB<B~Eupe7D=kN|HJutJKewGIRjRb9{4WHt@+FBiZZfAR@i0?1<gH*MXR=u+wXcP=!L2K>E4F|%b~Yp^RxFe+kFa{K`mO#n@+9vx^CKp;_Af4>(X8mNcqt6E)2!~V?Xix9*)hH*3ACh~IJSYYWh@7^wyig;(WxXcQKR^+3o)xKfu;d3^R2e7`LN5eD5noCG_k;_Ni?#w)v@%ju&{}_;xYp;yP~SDGO#{5kEG`}4>q|b+bpfK+bL(U<+Bp1gQ%`7%_`)#q9~j;Z?Ua6s45aNN;1SOf2`3nlB#|!3#u!!r7V3gx2I2~oE&bnk*A0=d?`<^Nim@<*{ysmA2JvghcXB=oUqxloi)rf2rKU_F{tV+wY1JHj4(2_2d7#nsjyKg=BH(??<E<h52;ZvJE~Hw1*hsKEH=(4Tqw>sBcf)e%dgP13NYCzFtw4kZ7bX|btvhu%dadjU@)X61GOcs&#?kDce6pM<1#8MlP<C|(JXPWeX;Pc$~JW>qb94gH!rd=A*x8N*Q&#?aWIRn7^DHPHXtRlVlmh->a8g;WH(E#$|1loi@T~TyDC&Q>mh=qZmB~i1*bqH=$O&7TQoo}@Ucm}I<Q}#H!~!tRzAiyA~SR>j<ahtzo`;59VP~|>IE69Y_*533^vmr5;XEGsVx}53bL-L_O2AHXsnQ_QM6en+^r?8zoxgWQ83`A4I+*x=cKT$Bq}v52`|H>LnvD-U@=p$f1ffQjxlPnc{Dbuc(2C1)-(z&9;nSGH8;L6OqhMA->sLU&@0g{60c>kXS7VTEwjO?da+F~)vahUOgQ{6CaVQE^dR@B%`GY{=(f)z+bUTYZZHJ2Y%VOWjVSall`gR;LYdMnAFSmn^{yu|ajxDVbTlcYUM1f$12XlhxU;&n6)oN|MKg}BjwgsJo2mmdZ82xD!K^{BpfCEc^fZ+^xG(84xvZqAnk>OBSF-u3G%r4{?jdHkF|bWHSg2*P`!G2l=CC9yWiB?V;IZsAy0NXWn=SM<gegn4H8r`kXg26ChBAsROCk|3@3ogQn!0kbm9LC2=`7N;mM9`6s49`Nq^#UDH?ljW!m>Fv04QCr7qlHTU@=`c?I1WVH?Zz7q_QD1<}b9hQlWsbIV(o14>GQ;z%^yGgry6&-zuRuUM$MDJt@61;HfIGr?O@-YPJ5gq%eW9C@t)?2Qm9F8nKq3f3QrrWiR9&iZdrIxh;9L@h)t*zNjuQSS_NZx-Ok8&?rhS*(i>*%`*lyKcaE2=q@9)GPEV8A1ni|Bc`RYFgCHQhBB<D5VE_nH6&KD`LXmgKOP=2IITvfz$J1hCZ?PzN+lJwRk7DFA|62@m#k~FFuYbV3N=$TX1D#X{G~r9pCSyPH?)s2hpZeclrGM+Un~<e4YF~vi7e%>#JPB($gICFtE<JZyfwSI=q>IpyQGIWLAZG~O|NM%cduSF7_&MwR<9p08M2<M_%_w8CpOZkh@(|BY_7&O?JrNO1SpiRNv%ku+!vL#qb=31JE?K3>NLVGj5c<uny@q~U$JI8oG2nJVX<?ui?9*26gfz)x~HTq3p2a8Of2`TPcCCAM5S6VU#*QX#*<AfP&k^lWvKusEv!>8%`@nxDl4_Hk+S5g?j-ds#<a~iYA`jgtTaEUnli|!yRQ{Ag)DS65V4LaZXdibZL}>n>^TB2;-yz8O}CjaMWkD($0a7LbshOEwyiZcQL23^Z63m_2BjV-qOnG#{JD}NpQ=eTBd|!ar?w=y=c3y&xFqm3Ca&D9R;w>9tFQSjldcB0AfaBaEF^ED;;tu@u$y-<3!N`EysSVm2r(VJy(_{mpRg9GkgFZA4k`99PA>egM>GVo0JA7A*ftNeWVBYVud!A!gf;M;Rxtps%(j0vx~yz8iy@Gz!JYP@X`!03hAhslj<R5_5UsB?w<Lo*F04|e<*6RD4zq70Nvc<^=`vz9R3bGs6s(6evn^vTy)iAYvajc?mZ%Ocg0SQ;H7gdhV=mb#aIZACX*dKh2eJ&UuC>*$VkAZ%si|8AHaTN7;x2nAx3?3jcdIie#wJ&;pEC}#2slix0;Z<7yrM}lHn9P?8nSh?3m`47M7QW939l5arm4)LyEoW59kr4yqOg-P0;xf=mNW=7Trk?T|Dt?0n5`S77c3#JiMR(SzqFe+S}ZdzYnX<xGPhkg9k}!~XRS9bb+O+rqBQ-jiLl?XVz9Zkc`&Xo{INK(Z?0)K=&>9pX`lxS5wJQivZ)5L7BU|#cQ7R`HLdEclB>C}UbViZ1~AJlT%&-nji~508Z^DFb*!VU&#?J4G_|ZJ=q-t|C#M@V1Ta!Eva2Miax2p`Q!!7o4X6XQ1fe&Y2$xVCuQRwQ04|-WFgzPC^(fMn4YCq9{j6>;IjG{D`zI-x94>1q_AOj59V}uoR;vKC_^h)sUbOC_vatRxOshz*DzT5W?y6%eJ~bV(QZnbTO|+-2k0gmO>aS8Jd#Oda?5*#zwK38$STG(mVJ?}q#jk0xsxslPQnc--@-*`&=rSXxnY7%ftgWN9RxBbgPofO4Euu6w88s@f*fE4K6)Tpp?ywOm&@jp{V4VRL{;$g}Fs`pK)vWM1fHWDU@32`j)h$4>k09YQ47B33>8WD2n5_2}F0_lW&aeY4|EVChA1+?8)TB?cDl&^Sg{G`BiX0%dQLss{^&#Y`+b;w(xin0;?zG6Mg*AhzjI~U-=&lW}Z!iL}ZK~F)T{+~h$tZ+2!?u|))gVZ;@u{XaoUnv1)Gt9Y|0po7Nic>d$0{YIdbK_?60@qTyf9R+!>;o>Y$>s;oTrzqs48?Yy(v5}dN`&w*CeN?ZLngb@Gf4na;)MoTr@f|6(#|%G@=A0VJOD6xg5N;Te&YZPccKTsI7AzY_ef6U#xa4t*kpUAghh49<U2BSFZTEa3Tk=B(X8K3@qC!W-j9}zb*5qOrbI|maECPB_-}Nr7R7!6fo7Lcr6Jr@-jC&9yPAC#48andoXXgIxYMx-z^EWU92*+YN<~s*)CkDwK+Ak-Y)<t39m(<YcO-Yz@zW9#v!IJ1i3_}DJn3yMyPu;KQbRS*C^|;^QZ<ho-(JiP_&V?^q3AXi>CUmkF8m#k*pv#z9Xuzo3#J5>Zb=h@vr<ZFD?Ko?JIq?k|kQMO1-Q)3o(@^IgM7ZdMf6pB{#w_)u3P<Nwh$y@T!@j(YDpG#xXKCPc>Pw6s7O5bF*`txiCU1KrK@^c|7p8-8lm>rLi|Mfi8_6lCj9Go-J0Zn=om<2ehmyBD55=Eue8P@2~DFa3_GYD6Hfz_cehs>9Y5z046Y~y*Gid*0IhlA|>3cFDi?f!=?x|ldeB7&!I>lHnx4PDzCOBbfo<&-L)pQptVsg95rgIA+%R7%qg#>pSEKuHZd`?L@h)fkGB081uKs*_b!^RF|7bCu(3O`*CaJAShc5@RjRlg(JK2PVkRrAw4&`M8LV0~?k5K;emu!6oT}5RRiy7P6f!NR6gJ+mMJqj}GB2~Je=&<1%(cBP9JN6+6e^i6cCipHEUhoBWi!{jk|(Vuq(J>IFEUguZ8jo2MIkV}<GG|cC^j3U!Y{D8=r-N4?YM|4T{PseAUGJhx+p`kD7QYR1T_pgy0+Uf9K7Q$Za!VMPAqSzH#`@!%egwM3$h+HLZpN?G(3eh^{bvW9WBi*6TAsK!!?pGjxaPUDY5*j+Pg~<W4%qh<hdv?ekx})2e}%n1vEIazcfLtd%Gh!_%-^iCN$}^EH{iQ!l-FCM!KY~cf8NOZ@j{=fVg2f8y`2g!6e8mO|OV4=PZV#ue>s}DW8L_E3H$tZM%~;pDd_1Xs?|(;IZHsb2Iub3pd5IWjoxu=`7bQ{WR#K4Jk}F6RH`nurZdr1u%n<Wi5fJA}rswG^zS3vaEBr2`sQMD6l9m`6PKRNw7XDMY7#8x3ej%P8?~dVySE{g(^fX{Hg)As<UjPH89~TIkV_77ctGUT(B-Il_(#w?6h|+WTOY06E9*l-KKlIS}P(eXPVP4YAI!<%BTn`P%TF=PBp`^#;LKk_bsZSPP6thnJmbzbF`hP8YT-bIx7CI@+bqVRJ2&K*sN+Fay7&(pQ663NF*II+9gk}!>|!7(>-(&#IK+z|E54JfvdN&BQTV(wl}4%u&`#Zp&T=`YCT~tqqHNaPplEBN;TFnTehSyv@)J8_Ay#2#WQ&@gg*i_d#q+DzcQIQTQ>+S(5{|3G_J}moi`t^3ppV%As|||Y$H!7cR1iEqb%62*s(U0_P+2W+_tZ_KQtUT?W%S%7?I7W$+-J6_q@<8!#0;KB_vz1KD|;fOtGD~)Uwkt*so+RjX)o|MzKXSGP4*hmbtU6uQz2gX|pmU>abt9hq9J12Ba*i@-fV<K`me~M7TVnE~(KmX)AL#t*f1}2e>@1cbKI#oUUlJy*0ruove-}1Tr-#JT1gC(6tD(*B|Vuy|HvQY%i~^FRv~ru&NNKkg;C8%dQWxOSAd1mN=oh@;AJ=Iv_YC@-)`5<+ABL$0ZG@b1G4_1}tT;`mnGjd$De-&o5%E^C;yo1}LU1#h(N-?XYjCy)CCO#WJ9%9Uz>r389*>6se~#EFelNOfkbSH!2)97BIFoEHZd4cQ6Mv9jq=crmj1#3NU#v@To-?>mYtC`>qkSJFECB(5W1#F|#Bq;;B(9N||1%a;!b1->`ZqEG)k#iY}L>4XlPV#jAO+ekf=tPAD!Mtgdt~4Ifbx!K`tya57Q0hA_&o5VUP4(46uoku1zK7PRlDKCnzAQz-ATH?j$+C?+PTh$dU9M5?x|4Y2ziNV77T7ObSNx~W{QaVZP5VlYy%Z!KmuDKVt1F0k&QUb@sKge^rhz$FqZH#Phu1}`Bk-?rT~N3K+~h^5x6&oKL?Z!^UxGN?W@xh)hhDW*#*SFuYp$*bb9{-ppXtttbeT`kS9JFelXWv`B~_pLUtF)l8&FtF~C_og$q!>P(9z$!wgkuVdq>$8!sofNRGRk8Ln!8CTT5-?IG>?}+u^{f@K%q{gOe=)W&^R42t?llOs<*%78d#&OwmoQMU^DJgCBe?po+pl&s#wZ&frz_{BmaQnP)U--8@2@>HZ?It%>mYMA6DHcRaxhpVt)WyjOQ`oOa4~Byk}*;;uAS~J0k43vi?Qf2LpF~yJ~CUO!6(HpEvm1uW3XAKKeRD06)oVjxioF9?=LE*JtpWNG&9Yv5;U1Igt6x^-LycgkgWVMcdDr{V6Gf9A2S9g1;1W2*(K;Ja<CSrtE&RCudzX_Ah;E;*)~r!urco)m@zIeW;edB`K~Xi$fAcWL$P?P@UGyl;1l*R3N#cm4>8y(6|jK1akNdh3@+=W%P;`AQZpK=h8zVf!8E=#4z3Zj=%b}Ap(9;2(Z2_``YaKu*D=MXCN+sRcDLH5Dkdv1x-?2Qg|TR`II~1FAElfqP^3LG%`$u`=roKlySlir!Jh1^moz9a1}~vB=P3oQ7PN*i$1e*o@-oFOy)}%d@i1~Utu+ZUps}1X7dJXAGb$Q11uw+EJt&&F+$1HlbSp9>E-*PY2PQlzd@0;Bd%ecI9x)iOT(@Ve-JoGHTPS9<c&%!;GB1TNAF)HY&ZJK*_%c>BlB#5{J+5>w1uqvjzBM8;F&wEjsIt5$9I|sQ0wo4B0Wjntn=cM3X03;;m#>L3;kg4b)+w$r=&k25HZnsg$29CPm@z=I9jt+@9JCQIR4f>^C$m8<EVfKAKB+DyFEItYOE7z@FC4QiU@ZSGxh$YAcr0@_kS-}I!Y}E!dMyAoovO4k5HZ*?xUBW75wJNn!7vN4FR&vxyf2eA0X42M1)#m3m@^nLE~y1B2{LG^KOo;MG9cou!!S&xkTFiHkQ4>30H8=F)~+C)=q&>(n5j-Q;v&tm6f*=brLWMoH>-Xn0wq?qKryu`lB-UVVI}V`rn7l2A1nYiWS%gwbh1vb)2zlMY$<}PPa#&VVz3UhXDE8MBq#Bx7N&itWt+03m@AMb(=om#x-mnj_n-u>ohM8yH7KwnoGvx6Y?f5AUa>tYF0V4OZYx!)%CU8=j;nI6U@0>&!=cA2YO5nDLoxWYOSI;&KeEB6BPa<m1~m1n%PkEiH7YwZi>=D6ttfRY<};0_u_Qh$=qQ*d)g^DS94h{|R;j<L@-(rrb)~DZV6u8JG_sa3k}U%-p0m=3prw>6>a9K{P%_FSC@mtd#j{f~Fe4JM>#WhW(XDZ<E3ptUpe6jUDz+pn*`qZw6*4R|uCi7&P%U*f-6hwpS26i0R=CBomoQkcJTMctEiHet`z-vkH8^^&|1vDIW2*|X&am#Tk}Nzljx1>`C^bZ`1F&MTQ8E3p>@1(ON+=Vu-YU2)3^3ra12N65v5y6o+bmPI?4npPYalwLs<Cgd=Pf0v#w;QZTCC(V>@d-@<SxjvSEDwsMzgai60?&bw5fos=%Mr|b)~MaxG$bH_qCEH#H<=7e6`>*+%o*3-7sf1N*;kJ@t{Ytm9%WDelE_jw7EOANw7pXK(?Q{TPiBJ93iHwM>J=-c(pjF`LyG$KeXJjV6+Oiyt7)fny(`?Q?}+eR=*v#O}S9C53#AREi{6)+PEXOVYWuN2f5c7b21UKtTz=iuegSwGcZxNh_o-ZsIZ&3|GeR}d%2r9Ni^rMOs$T(%(Rm*fU**@z_=N?7_vFHn=eT<?5m8S*r|852fL^<A++VD_`A)xC8T((q$onRF*1>{UAfV@gtbk)!nhQ*Q?xWM@3EM!lDO8k2RM<h>nd`&9xnVibhjKUi8!yd+qQVOv@i@f%ej>=XSXD`!L@_4eJkWF^0LA=a=Ej(60`_8+qs~*zo68zmM`+Ju%<M$IyN>e^0p74*))ly)wBq(@v@>h!LY@!>9<@l!nK&GYqCZ&W~v>s0<qz-A+^gRU$1nbgD9)HQ>w@_r>&SJakwceLbGWxCNC|kH8S|E46JA`f32x0gsc-SUZAP2!YVN}$~n2B8aa+KJt+LH*0g)G<g<jVd8EXwwKNqp<uI2lps~6s&Zd2`^CN1ZShcXSjW!Q29yRtW{ji(3=dDC8+$9*TDy_G&4XvWD?Xj(`A2$NLsxZPd;jJ>M->ix#`7dNGT(7h#akFo>s<PLxzb&q`=Qq%<&a<a1jI>cIbStJY(X@WI@34n6;UP#Zs45dE7quR^k*r-asi?IpZZOoV(yZAmM;^)}HZ-X$C^M8SyQMWN(W^-@6|6wErL^ESimr7Xs49rF-?-GLr8W4qQKJ&AMlCzA<*I9|K`|XP=`*M`wlln}MK(sK@2=`2j<)k9Z>+5<^RA*I-><={@h^6^xv|EqgtXosY%qbVP%xG&VXf||l(R0iJec>cud|OcTO28`7$w6aZ7poLOf0mcvm%Zz)TA;l0kI1$H!l;gwy9F7%B&1Dr>_C8RkAv<Z?ZKkOfD2G=`IK^>9fwS<|)o4AiBe=#xP(pGr2XdmZe0k$*}=9D6CAhA)sZP>@F>{y|At~<uk&pH!1<K3cD#Vl(0^+XDo)Ryt7Xz?Wa&G@46SMj;M<$z%2{3-Kk5uX)2&Ljwyqw#;lgGO1XP6$1!WPe5WL`RyE`ab*>#TOROEU$u4}f+O!+AeYG>E1GNjSi?k)R$SEVJO|%WO4LmV3y0J5~O)ds7VJmO3^qYh-`6>>y7qghJ+$BaYizJ{gAh5fp^RO7PrZD9sJG7v(oHUj%skCl0130KIr6-<~v9kLvxGGk$(5&pK1hA~N4mCO^k}eCexiE~an>ko7$TgU#V7BY7n5q9XqBIe#Hntt9YPE|nnY9q7s4&+kTQGaDh&LlIU96KCa3>6(O0hAbq^eaKiLpMo<guK%eJVn+J*dm6a4fJdqcN>1nJg%?g0G#mvK^u-do4;Z9J8G<tTAsZzpR@u7BpA6+_YaWU@;e{QZFGeA*;Nk$})AWe>9LUP_i(v-!N{l?<le+4Y|0lK)4OCcd8()qM1Z3#j+NvXs~UtAG2C9ud*Ae!L-4%Td(XgA+ymmGqDD;_MxGvV5}vuK_2?8daky$M6p%2qqMuRd9Po&m#r_YQ>%<C$+G>jWU{TVny;rdBdvtGX0_8ToGBK#nKP9$makDXkhM^^Mz*%FYO7Ez>ML)tg10cUXsCI(2{2MHey@YLma}%NBqi9b|1fy9#4_r*4z*>p;4nQYuPLps7^yk3GO@WW-?474{<EN`ytF7Z+%O%og|*(SXtLP13o8e(D6}=EA~3TxLALcJ=cdiH1+oq_kgbFyQ!s?Lr>>W>4>8-h9kM>GNgrUi`=^5|TsDHVFQpJA#I&5RP%sd%FP%p&h%6l}*r;f(->aRold7w)O{`9|e5{AFIkU4OpDn1ga<s}cpD81>zOfc53b{ZqjVL;}?X>tYxUYDsBe9w@xhsVz1E+_xkts(mIIR;YC#P?ugeaJ?H?FiWezkHfowAxL5wXLeE3qCcU9iD0BQ)=|lBjjCLA4#OZL0nR>Zj|NrLI;hA-3qbS|IJRz_Fbv`>C$0)GDH`;;LLPH?#Grw5eSvZLhnsUo8T(?<euC->t7K@H7apGbGz6v$LWx7Bi}%$*vqLj4xg^QZHgNr>B^-Au-M{;4#H8V7JmFBQP1YxFBAzW;QP_$*&qUzAm?`ub6N*eK0&Da5QZu%DPyxhPi{VvM&m_b|}WRC!zSC3@Tfx;Vc0!;i<T)eKdG91+$g7->Q%^kgdeEH?Z+5q^J_K!LS~$vbajHm9rD5U^I=a;i<2yS+a;OI5Il89<Wp`rX(7*P_z)X{Hm_Bz@EA<Ahmk5Ah$KOq%C2o2`!kkv8#`t^|4eiY_J}x61o->CNdzjw6BCNx+&_av9-^v&!rWq>mLat+cfty!>ypK$+*I=;jlm=XDJ1<^PX_8tF!qcnz2H+=Q4IRoH9wMwX}VcmNB27K_G#zo~`LM`zRkc=``!F)wC?MWH6o_(zzqF#;!fB?KXz3w=L4S{-zVLSu<cPIjpO%w6x!-EwDW@IxIvhB`r&{S*~WRa<IXtey{SeAui^uqo~Wc60Xm;GbLHKz^;QWj3z^{oUI-zr>{}7N3dI`_$@IZejV2jPqT)sFEH*hHn7{PEHJArQ?s<RIIvK)p0z+R!MTpKTrjS+k|cq&ts{%Hn=SOPys#cD9VjuT!mrn;Ua#6MQ7_J?b*w70$}bfwHlVVw8nRa{thmFlohjq3-LxGp?5e>va;;;zu(6IWkg%4lzNyQ#>Zu*GsV&j60yH=<r?E`9CoH@)@hwR;@-Egc?Wyssk1$i7r7wlGtSQp4v#TSxCNOTPwz8oy1+1m0?W(sY@uxPlJ1GwyqO0Pt;WJXL0;+td`6)CsBekPAQml?K*e!OlgQT6bJsaOJ-!#E0>#pXbQLaO+sTvTlN~9>HsIcp{y0F-@iY~@57P1{J2($?(m#43=cd~M}+qYn(wJGl>9VtM&nzJn>NvM!Aow#%_%dbAD(Xin#kuUr-ovU!JPPL>l@F4cMP91TifFOk}!6H5^)vDsGaVcA+LAE8bs;(5cC$0>kg{{e`TQ06J;JdZ3f-?cUJgjoFrm@5*#4nSwZ?tHkuBB_B?luauBdoN&Ah@NtZ!RdRbgMVGLNFbp%ql)9P8|=j0vOXMcc`p_?X*j@_@KQi5w@V6|1uyju`<Fj?6kDEoi9K#6)D)Mq%WN^WukN~K`~FW<rOVAp(c~8J1WMu>oC!@I5kKlFf2rvvNfYAG#nGIK(RqESe-j5?ksn(nzb7;?H&-Y!=~u8EiKToTeq;SWHZgM7A%J>xw1;D)wMpbZ!j;oz?HZiBCkiPR4_=Q5U!Rld9f(3IxKIv>#b+0BPT7cGPL!q`>Bo=q_lvpwYT!BIW!?OC7>FxuPoU!4>9?!RxQi0ttjLzF0r02XD!DyU#@^1Su__l(76RN*_~9ckfSd(A1!w=x~x&J2Cy<K7_fb^t|`^8<uYTaBdZoEy0llZXt-l1dZ@mwvofGCh^L{lwzPA&?6HWcNv&tFNUM=72Q*r+WGekE-Y;Ua*0q2!rLx2+sIf#Z&oRrg*DFmij2arYE+H*6`kc+N@1&BhfHjviH7tZFoU}_PsWd|{5j55?n5<8=cr2l{N;P<|POL$<a5WsQw<^Ihmb2QgeytRr#;^LUN4VgjX{S#l05B7{Vz4u^)u=A56tID-aIrwKF15ckKB)-2kf{1Gh$#az5v|oXX0!*dmaX8g#juAk!LVbsp)XG=)hn49FtQ<`W->9X-!h!E^)9=xz%S{unkFbTAFkr8V7Y^;ShbinzpU)8qcos2skH_!<FFMZZ60B;uPz#`8n*Mbr!01|M6tiI@vX%)+c1Hw7`P#_OfY6A3$_h0G9-Ad5Ue&U$Eb?6O`c9EoVh$I*0o404zEG5fHWblwXp0Yuq4JU^|ocN{jIJnS2LR;*|?drRkdTZ7_2Oy_B1srg0QZu%_nN26sVdh^RW4-%QF8eRWN3*kTN&4zq5$6AF)_ATcvrE5}?Q_@~l59(yQF6I5n&@ptBS+D=lL(G_u+(w6J!v9<|}FeXwFJD6ZHsYP4=K1~3-1S*!ys0xZZf+OXj+p{ItmNH-)PW3be(<TARpW3UD+W1{u7&8}~wM=$xHrxoHZPPR6#Xt>ZU`Z0~KIV^-U>YmXp2qY_@k1)TeXe`Jo!my61VJZZuHLSWUr7hF2H8N_nPqlZojw>LuIU#;2b2jiUEvdpQN-)Z}<tetc>!s}~Y9`M#D=(z9<TSdkO0<QufihaBf2fhH2DZ7a9x{2Zs<Qv6*R##A{w}1khAh{!GoNK8g|FhWCAirz!?MFO%^xHwovTu*(J}3<RJ0N)AvdqEm$OtV+$B=A#JajNW-C#&q^?b*$gdVLIJG!2Xg2YwV6dUFVKT|D$Fevn7qShiDY1R4K(b0OprA4!Up3pX^{T5bx3fiynYyp9TQ79DnX*$Zy|BkE%&=aw5w_ked8#s^t)_#vBPk2CAu*w}g)e@#wXk+Df-B#zsIa{<AT!3X4lcg2;;{NIe5k9o=cY3(%C6tB?K4d)%(#`bhotc+nJj^^*(QdiQ7SdG5wirhrz-fNmNQ8(oh@A=gte8kU%T?AhBBY6*|heyE3xOD5vLh6?=VNL)3wAbq_xGe8!{_0Pc8~CrMQ5zhQOLE=0HlgEUeJDL%@$Z8?3ssPrs|IWU<^f*eN-n@FXv<le0atAiguMJ+E>l+^v?Y4LedR^Ql)i5j|(SdbOf1F1VGk60)|c0lcp*U6&`bRxuwj2r`DQkT~V2=rP(Ku015E$FNzam?<K$#W57EL9(?l6)9J!G_XE0Y_kHYKd57<6|JSLqq7{LbhE-L^DrcyW2`B#H7r;(Rw?SKo3k;jfU&Hw6D~2M9I+EBEirPSRInf~q_0ge@G4|6EGVtD4>bb0=d^FJjkS5KBC>8Wm^5K5j~4!_qBKM$5h(z%nXaOkd@&QL&8(j;sj18~*C-XZ|Fc51(ye;25-^9NzaOYCEH~II5i!Ii@iHzof-N*Mf3ZKb9<2*5k0~Rp9Vq&+NVJ+Q{<pv|&@Aq%^scR_vM{bR-8I58EUs{};vvenZL=G+@-hFo!nP}|@vLpH1FgL-7c}ZD<F9@+5GWb1s3`O-WF`M7l{Lq)c`4AZ(74^D^DlQWR4QpGzOa9^mMzRDdnz-qDy&*BqP7;Y`?C|eFR>9MHYuK_Ofvj4wyE!|hPk&c*(!~-;HaCcyfwBdS}-3nuPX7bR<<s(E-Or^c`vlMP^c%Xw5|oQ%%~lpeKnOQ<&)^KfG`iGSFhZv4lyCBL@SB8s3yX#hBWK2L?&Ob3A3WKpe`1$5ix)+2r*%@7%((6*D2mAb+Uyev9719+%-G09;a9|ue9E<9kgAxRv)b|<}WKVSS?4jxV2!jaId*D6}V}%Mzk!i+Ool|grwFgSTMk}X{Mbe=ra|te5WchF*B$v&apg=w=+tyXR5ufN3x71@vS5{3Z)mW9k92pm9ojPnX*YO;VC6F^r<GdleM0@ZLA_7wy`?3!?Vw@ShY*AbTOE>CAYaJt1rbVNTr&p?W;R8gR`NuAgK#2?=0!461Y~bQLtUOK(xTHhbhOY94&XC4XmH0`Whs(9JFe(I=X!<k0s44o-lEw{UGbEi8c2w;H2xRPcSGf2(-Dd(y>RN7P3^dwkaO6J~RWYYMVn8vM&U<)+HFSeK1BcYd9{re5~U&$*umaBCyS&@u!tGEwQ4oBp;nDsk<>Z?KPDpe6`E6pR}4P7PNIIA+$lZ5V!6uOtb2)Ca@wca<lLkl9;nAGB9>AGp?93{I7~88?o%JD71ek<fi{FQLP}TzOoFoz_bK2;V@gWZm!}X7%YLgC!r~=8mPQ1ODfDR;IUUWk}#9ANh?|{Co$==Ub0v+a<8H=IXCRN*0vh20Vqr+EjNNNxvi9|k+GmGA2P?Vp*3+bzqD<zu(cj6E3^Qyr>~B>aWx~ai?mKIE2zS<rz=J-mZ&eW1~n?I6R~5mC^nz3#WgmrNwfC28lp?IW-<}1ku&J8#;Vt{E~d&beJ!4_dnR!)!>xd^vn@q9G%xckMKQg%G$mWK8Y>Q_DlC1lbF%J|g0cv*i!H(@aJ5dhH!6v*cr<%1j<WWz+pcgm6RdGH?IgP|vnwmEu&21Qt}e%`yePk!`mxoq8m_l6i7iL8zplHkN~BOUhN>a2HZ`NZgS^Qh-Lf&YsxS&P54RmMpr%nQxH90dPATuV)v>#>o-}nMy0QhMuC&dt;;{g-1F%}K;jpoziKa-j+?~j;&NDDEpEEG5F|6OMLb42;A1$`DX)oole=te0kEC#+6S-ox$Dmgz!Laixy0e<Sx~?g>!mJ#!x-u=PF0X|#ktj;By06l&mo9CmYP4&iq^t-a)UbChnJjNCO|)>djl2;n*0f<WX(lkQ*D+VOTR4fWtQD!U_AxQBJSg%g8nrJkODUuz6ELMR2PFxv7p1r}awbx#;g-8B{;TRNQ?qIxiL?nVh^;&=6SbEz3K@MkY^@2l_PM7q6fnB495Om2u(^)4PpOSBO{|)fRj4nr87;oISTd@%?X*_5W3bgK@UA^HwkY;7JuE`9c&rRBA+EBj@VBxsS+BUUrK&}vtSiZ~2Ckbk%&SeUax=2EnI#pr+%|tM_On>8T(rq2A2DRPJhY0en6=X}m8mqbSSD^PsI+vN38VEb$+3g9tgY;^-ZJql!ZSEAz9^tH^R~JucC$OE9J3uUkToi?n<jFu?72X+*s)qEBAB7Dt-8J|j4`RK_OiIF+qYLTCMb>|*fh`{4=utqC^`PKGp%i=_ZOqDY_D;)-zo~Tf-zPwle2!Y%`&1gsH$hQoU5iU1FjmZfh@@<SSh|Gk+C(ZAG0*LcQ4+t6EqjKxh7LEC$%0W947-Zps*;KL$AgzK{shE^0H#C$e>TSkFr0q)T)iGbg*NlrK-xVOf4@WsVzk^ptUBn6s*^@FQzcDh^rDXwynFYFtZ^jlP)hcv9J5Hjkc^e<tox8%`Td;x~nv#{kFU-d$rQCTA%(Z%Pust(y*E>$EQ~(!7chL52!&UDz#B8S1F;bwz8ip3ARl#_qYYHqcf=`lOn66w6YGiin1QAPca80SS>m&i@9vB`y{rnyR*ltg`>_h#I0qclQ9)E!YU6ddbb3;L^OpezqEF-i!drRU!=INDY|kb?l90KaJCUM2P|K>|0s?v->cQEt}wV1n6>J%Z7j?$7b-(78nwBx$uW+xr?I=Ldau5%#w-jpurQ$?GBvum(XFg3RIwv3j<zqf93!ExXDsiuxU5Pv)T&jltfx#dQmvS)Gqe;i<R&G!T`IM)OSI@P!mwT_Rkgshbhr?-Mlf?QVz|L44yDbn+owpdM5#!u1)NkSdZiJ#y(dwx6}UmTh8=GuGpv8M1h)IGSFx+HX|`0bA+TpC*ra>8gt3XR6EQ5RS}~$8(zGHiuBcSFWw5xneloV4fg^J+zOzNEnlyZ{g0Q1ElQ4oWShSlcSS)C=oV1y<=d`=6j50YdT`<fvAhQOma4ioi;;n@$lrz(_m@Ngj=`Db-ur9!|7^ed);x0BcY_e~pL$BGe4l2C0s4mZ}B(#YsKBIcK=d7eG`>r*!Y%5bU_bPTYskwKkz_5X>L^u(wL@;0}f4KRv*R)QyoU-~LptHxWsITI*=deen4I&<?t+mi9A+e{ln=$9O^ed7rd9VPfnKgT|OD!BRi?+Wr_%3m`&$V2#F){kG*E4h_9<<7?zpzZaF*HRiIxe#`eJYQ!&9r_ma4e3nCa<KPrma}160MRhc_wMFh^lTh*0KLK>a+-~T`29h_@=+Htg^PSz%+)jq9~!V@hzsWLn~1$zaj`7x20*cjjkH7D6pt9ys11gKC<X8OE3Sax*DZ1(lZ3JyS4MR%PG;f8n&6R<Se$Z>#%{cX0LFq*|9jUQ@2Vn9Wd0ixiJf{o3Bl+oi^F5+$BP;B&|U*yd<|cRWgq$rLAqFo~>&%s<kq#pRTJQ6DV7-Y$~9!`Y<1;ov~pqX0ZjeDV*vp@hoaELN5iYwz8P7n6S*L0Wj|?Z?$c%`m?jFc(vZPE38kd5wA8MtFfZ7v@!`TkgrXZFEF___A$7znlxOo;HuE29kgt-JggqBQZ09@(ygB+XfralWhp}~k+!!kxU+V*ShZ9y{Vz1NhbD!vAuhL}9wv3VjIRf&iZH}4K_yV7lrFO|!?9GUth7rlbuObPqOkWXWH8M#Ah8B5?WVw}=rX4=CbLhpX)66R{j?h`+Aa+&-Z33CQz?_OnyxFYO0-icWG$tz2{2GFi!6k+#JyduO0`X@FEdg!O(bA746u_c&9FtW+pXUvo41T8S1e+uLanMXC@NJdqp(CO#-hG0k*w3I(Wn@&ZL`+2Dz+am0VdwH;4f^hFtai!05@bWlp^pfdMy+tH!6s;N;h>atuHUG@GuoIp{RtUy|VnY7O(~|SFL`iKC?ffE;1jkL9yB|`7k{+J-Ab>JhaTY?zX+RR;UWDWwjA6;jcg<6*-PD7%?R>=Cc{7h#bNvQnqch_8d(x86=UgBQJli*sy#wM!G#RVKAJok}kZi+OnIi{<Su(v?m~<y0MclG%#hZFg3ZeC@LHz#h0(OF(qfPv8d{(g|Ox=qcth7`L#N*bg`f*NHH<FC$=#gxT5a154Bn>wl3l{;wc0+hB36Sr7a*YCbr$N5HJ)lDYk92BdoD2MYUlx+qdboceK~CJGV(S<gekax2$lsORs3MiZk9cPqSAiIIM}OnzE6pG`1xiv#Dk=S+TUaC?PZ~qo`&q+p!d^W3&j6<}|r5gC-m`7q!~8m!O-vSg-FUT{Rr9khN?sLN$=F;;}(5^e-|Y9<&@Wc_kw*EVE26A*{4C&9tAhA|}fz4mLeE3o!q#ez9U67BfP(MyS5B*&~TAs4;dZOdtUy+OEN?pfi0ll{1Dasx*5lA-=;dUNp-sp|NVU61WUJ-nf&P6}^Nb?zlNKfv{sR)h_n69x@oTa=b>h5iAh36gQ183^zM5I<Is$>!t&_pR_i%W;m`j5vK7jrL>K*;I5*j3NRorY_>bJU^O2yr#zz_khW_YbF(?IpfKC9lP(^mNU|)fA0-kisHjc6gtT3*`l`1iE3ynM7rR)o1fSu!EVO*ECp7sqF|mL%f~x_rE}Rp#V6<l{lCQt8HniNZVlNvs4=XG%53tfPlrg2T)3~%I%`ta0;;^G28Zv^ksIVF>QLzWHiY>M<>@q2^{<H6<(y}T8k}CK$D7Z7TyENA<!89l|!?G~5=A`MUbuMx(=c`^WMlMP)y0o~pha?%P>?<oWurr>iK(r<?hO$~Dx--Br`>@chFuBJy3^1>$XQ=zDPpybHysx;pYpfWl2{F#25ICu*$fV&jytizu1~D47M6?+#$hT6hzoBKfiL$P#wX<6*rZHo(zbs=aF1Y!$X)4|r6e)tOWUP%TVj`6>jjo5ObS-AEW-&-F+A1u$z%2{8Jg{9Ux-AqOjV+)s#F&Ds6)bkF{VouuN3LKkdLuKYkF4>m^ehc20JYw-OfaLd^s!(u9xryLbF^@wD>6tjp|6=Q4Xb~zbtEFQvnXXQ$)&|5$tngg@3PY@*0!B7A~Qd)@}@mAda(?-392$AOfWYvN++f=th7w6f~vhNp0JQD3#b;d@~B>}Zz|WVZYo$2WHU0dk}nZ7K&i)}lr(^+AvJuKqOr@cZm}h;M=-P}Vyk#I*fYkl!!1RcDY7uGQZg<dSFx3<-n8_opfmNN;j~~j<f}q92CXJB`K2ta6Q~d^%(Tq3>9E?XNiO!c=B{42!Y~ar$uA5sH736^)uJIDL9xxM^tQLPt1#j+(J7fS(5-tgTPlVm(<m=5&NI-alCJtFKC9>-ExAsR!6A&WWj3&<>#I7dKeF7i%`Z)@X)Z1==duZ>WTry1J-OAS(3y>_>a@x!=&LXxL@*k(hp>0D9WH;dDJ!6^5GSoN1EzAZ(z4<$hOc9=kS=#5*sT>X5h39>7cE7p+b(Cbi6Gmsur4IB5wQw4BDi!hpd&%A2(^`>R4<t{#<nyt1hG}Hr7eW0`!Pc=oiNq6MlC6?ys-PGIkdp8EVxE6UaJ@-HZM}RbS%QLLnLgpp(9%?lr*$7@2?-FM>OcPTr*@eFe-j7eW^7!X)x5VJv0QcDmT0}Znj)DM>w6JFQ}ub&n()WJ0$fhEwY)V(lj(D4KfEXg{TN3Tb^YvF0MPR7q0uc*D|lMJv2zQ*De^aY_O;_U?q{O$gl-BIxd^I*|FoXpBrtdgDlE0B%%p4s<kySxGo~F)15aonzwE+o-~;(4yPcs2cJ@_gR2FqtFgzL6uH(c7cV!hF|jK$52a@~Q7lv~O)?Lzs<eEw;<D5g&#^Zx*)-@bz_s=yWwOCBGOu&9cD2r|Zm~|UnJ$|vEvmdBfVfe!q^uz=Y$&v`%B#Ym1QkQ3_ONEHhOl2KXsx`nCn<WhEwvOcnYFpIEUYB9S+lvb`mc{C@il%gJgn!m-7t4BOsIUb7O=9i&oN^uS1`CTHKidWH7~U-|F=voX{{+PBs4HAUaqpT$uM2BJhg+f3aY%Z)VNkGVz&I38?dY?G%*t^buX_kqOg4{vMe~J_N0=pPcA&GPq1UP6D-%UHoJ?sfv~=`)uzU=C9OELCoD6nQ!RV9U?c=L%PA_d@1k-fw=+s6m$I@U#HEj=I4~BoXe}kJ2Dh2IBQG7T>MeMxld<NiuC(-$Ewa0{g0;o6H?plMGO0?d<|m>ngDPRFmAjRwn<NIZwJ-d#P%Ln;2C$#5;47A}QY=ic`80vDJhBinzcCXfQ7*8p38`r=kt~?Dj=BD#Jt%`FYpjv7moMD3P^_di&9rVVg{zG*|F90I<gzj}8L#@eKr^)^U@Ts=uMr}!N~5neNUjXAvnmp`*Eg=Ob+XsEw>JE&F|4^TNi6j+6D}e!zOmk<8nm%3N~R^ANGMyh(z0kI4y->WLamaphojZ7O)7+{_qO+~ezc6Z12m?$L@75kDXp}*i8M{LY^(yb`mi;*ZL@MH3auHfakavyAh-LiCb`(Nu`MU3voPnkgeGDz{<BXhUAMOyX0MqqxHOZmgRV2TL8}+E{48srb*~<(T_>q5kG7V#DK4M8C#?6eaxE>fg{#ywm@#9qvZ}tZjWJFiOSPM`A*hVCu&RD1r7pQ451C;xv9+qDfUH6=YMHyQtF*i%c(h`(9k>Ow0wDvqB_>iaX|xKnyDVt0fHnyzi?tcDOtPxBjVL~#I51|btfsEBTrg^_NUxDF8>2`t7qgqQNh|`jm7?LXWijqB_$?bO{kcpkax4ikN-*QJakAsGxGr2X#~w!{iYfRghO-H@ZLG>L<uQ3L#kg3qShdbIc(`ya8mu*}K(rR8wlSl!<{Vh1ysgZjHY|H8TP=y3O0Tl7!<`_pcr)}afv-EOy(q=7eI+-zd9hWl+Zhis0GB~ChN~gG&LRe(JuY*$l(6=&^`kX0oir7&$*uG>EHL`6zpU{rj;Ok;B`QiXRjGfq>ZubiL!c+AgE6zNqOBJ&Ju{87n=Fd41}t?ZHn92_bEN(yGA-CCk14z**{l0Ak*pN72{xm)1}Lqp{;5{7v$MFZXQWauhpVnMQ?SOe1GZbIyswBWgd7Mhwk=z+^)6j2)-V{fBDIaHFDu<G_OH;bZlo8wxU&zfNH^iA9<gz=(k2132eptKjJ2yX^fLRfl$Hgw->yb2DKl8K(JK&@g0_RTgEEPz4YfzIh^?ryF0vW2%&0&r3ner)iL^4Q0w@A7*)@`^(6Dl}?kdc+*PU&)&@6JR$SnA$1F1ncqPf%{Dy{P^!YskAPp<B@ekx9`!L+O>Q>vP>sIY}H1GRQ6Z><!key)Kks;h`8s4u9hfu@?PgfMut(W%j_U$IWDL9J@75U|OywlPY!a5B)WMzJleO05kjc%)}EbF~&QjwtY=j-}43$|&2TBCA?7<Sk?{;<#h18ZB6wVlD!p>bZ)txGBUj8mt?m?W`QJ{wK<{6fbr#?JrX-m??=ZBdd$GMY_2(f-MfO_p&221Du7h;;}}r;IHs4D!5c7Q6wENptL=?R4E3uvb3MG3!rtO*({H-udDy5V>Ixph%|kjBdW)(B&(IMG#(x*Vyz#cv!DaBBQkd;qP0gf2eg|ghAgJ387|DLGOda*a->rtuQSN4m@rv0zO<&W)hI77;V{~=Mk@TO53yRch`A-RyfW{#sIio&5HnJ(WFZ@{YP3zXuCnzr$1Y(sY$yw{ldq$*q_XWNO}5>g*|puKXEQh{<G8i9=CQdiu`ZM&4y(N`i>#<K1v!8$p)z_gJuwt4Gp=$mv?^3C-!&?%$R;DY;VK9&*R8*-FEt+~$~Ds}%P#@43a&vYsi#7$Yc2sXU$<+qPc3sUNG*>ku((vQoGm!F+bnP~u`bW6sU+_$9I>IXBsH}#Hn;Amw<stvB(oheZLozQ$F1QmnKbh-#3a5Yd$guD12yBfUnLANB(0mG%%y9%PPeqDXEPHX^t8{j;jgBt1gqwyTDF!jm$*Htjxatn4=R`}N3G?o6tqaLAhF1(MX|srA}|p%%%KdSx49H8xFAm@<}pbogRe+5{J6m}H#k<gej>s&s<q!Rys&99g|bz&<)oD|(y2tLhAla*`73O&I<K~|D65JX#;<@e7PdCEd@aT_2r&RMMmH0zy)eL}88c?Ii!C-NXEF?|qqrlq;xbw;F0?5lhOvSu{-`)DrmiS0d9c8?u(HW1nJwkCYO5xt!LWe0S}}m45;U^3Gc&`ryCj*gj<nLW&8y=j{w%+)Z!eCp#Iazl#xWxu`!I~Q60({wV5h^dA*Y-%jy35ebhKHr_abbd3AhQc5G+|TS}}>S!>>TK3!s9mCLtRw9<)EO%BnxI0JmPKU9b-*1u=s#J}!y2#Vw|#G%c+v8?>*oO}8X2>8!9f1E|osp)esXCb2BC1hG>d+_;!6yDJeXA~ig$^Rq;!(=j=?<f-hZh^VnCd$QfMT(o<zNVC2vPO?obo}{;`5F!jP9<o`PCATXj5U{GL{4uAu9JpkyKD4+h%Q2&{(VjIl!LUTQBOuABk1+Hr6{yCW6esMieyR;K-YFlmZLm`<{2xX(iK~1v*Qjzd+@Cfp+!~j$`?K4(Pq{y>?WSL^@uXKFO{WN_b}=_F@G;gehBcTZEiVeRnY9lwuBXzg9<O|;!7C)W6}7A@6|9W7x+Si%$DN+5WVuJKptw#X<F_p^R3i^9a3k`ZsU)MeEU*8uWv~-4Rk@=qQMJRjS~eiHKeeJUxvVX<?Ufdp+Oj>g<t}cV+bj;V|2SGSeXPQ-F0rgMiLfWNj;IN#*03nF&oh*-Vk|!_#v`~L3b~A<7PKU#&LBG|Os>%^tS7>)=d|xO3aEXke6Y_jYoEir2euxqu_Ws!I<D?8v@8X&r!~qUi!vlMKdC0Ph_+iXR4_ZW@v9@ZfHS19Sh94hqM@v$buk<%z^v~sEF-KfZ7#sFZnN{RX*1Nek1$BAj;_V5jIj@`WV&B7RJ8V`GbmEA$1^*zf+q{POs>qWrZk(f#H1;)MXx`vF0AIEW}<knB{1KzGb$o3$hEJpPb=@O+O&D9EU8(dLo~Rk+9_4G*RTvVIy6WoZZqAen6X!^*|HZZwXje#S1^|@OEG-3ETLvL`>D?&@U{mp&8(6rTQEGfT`^O!n=mx2fFx}(_M}#@YphJH_^}$TnKI#&rz)(ehPlU?fv<wJU@^F==OZw*zAU7t7OCAcue9hc9<X1mpDms-y{+9Z$*%^y)hy95N3M{qi8FL4yr;jZ-7x{Qttt=?WGMV3`Kv`9yE8p9sxRWCfV$hb?4t}ZQZbCMK(uG8c&uozVX+n<5T^F3;vHkNiLW*;)uZ$+9<SW28Y`)=YOo)&t}zC<e=xhOeKNEyrm#k;kTV&!7O-!!9JB(l5ibjeh_MN<Dzi4UmN1vHlP$NHGBB&HW;Ad#QYxmbv$R<%S*Y-@p*AwK$do*<uPVH>sVMR<yRr*2L^JiPk1raqOEUi~rz~-?A}_crTc;tX!?o`#rKBvT#wv0$kgmV2_aDVGsjquAK(5p(#Ua`<AFXJm>@erG+9+DM6g3SpbucS6il*l?9w+drtucQz3OLHGTB)nChONvo+og=8wy{>VkugNFlQOO?0xzt!xvg}jM<orm3%FFVH@H`{e=Yg1!lGNUc&r;QqcL2pp|(M)ay8ekoV%2|W+zRxU#=iC7bu&mx2(}LfvOO(DXuK94K8@LNUk!lowQ*q%%Z|9Z6K$%_oZH|L@=5#4Yic5nk-eUpP>6DVy#TFnl6qj9<3lP0V9?(Ubwlm4>TvN+O@PKZ?H8iLbz42TrOp;-L+~lC^dL7#xY;Ar80}FmoRg$&8`eH7qrx*%P1JI=^X4R?5@kRjxZ>(jIfq9WGuG0|FK*tIJUZ?m#Kv;ftM^Xv$Z6!(=;}+RxhD4LAg;hF)=eO_^mFm&#8GW;xKNvMXMq!jx#T_rMBR(?6(lHajvs78>W%81gOfc*sM6P?<(sqn6W=8RVC>yakA$w^SLLpt}#2Mr?Si}`zEd=S|whmk}*>+U$$ecuelvAr!u3hQ>_#(z_lzg-Yk@%bSxsU=P9PH%rsFe8Y|?qkur!hR<8^!P9uN1+^{FIg|2id{J3PhK(@!N@UtQ?HnrlUthUJ)akd_$zN{H41h4zHKr*MXjjf$2OD*fJ_p!3Go-DMt4J+@ny|5jy8?7^}?-LWC*fc4!!?0yEqc8`ns5Z7S=&&iaa<CgVbgW7-KqW#j%_s-ABsiNi2P_$`_burs;5HAqma^3~!L;cs)UW?CyflWXd@fS5<Fq(1$gy^<&97`PJ}qFX7bt?ZC9>qI%&{t{aJnF_(XU1{dNd2N#;GW($tCwH3#C1-ohdD`ZLg^)B{XueMy5itPPut4k+hsP<*uc+DYiAN#Ux@YhBv^qG&FlJAvDXk->);OYp&C-{;QxX`6r1oKC-(db*Un@B&(OSM7WkLfh&S6uC?>A9<|W22&E#pIINJYc(gPy#<=jPxGt_PMm(Ff>arX#9j+fR*0VXaAF^_-TQ8!Wf+%n`b0<-`xVFVMR<XXdys*i%!mL%Ob|4(B1h^x#)-XRLJ2lR)2B=D|;i!c&PBgW#-nK`x=`BIHIyZN#FDRP3pRztL@+a`IqP9w_+%&Z#XDs=y*(y~hg)Xu()U->kvaPeX`Ze7$o-A4_cB{a$g{>f_r>r)vpQSZ0Nwm_lPA$kY88o~r%rp8gzO8k!7BCSU{4cbkQ?&y%RhvUHDJy%g=Pes5121K_tguuneytHTiL|G&&ND5mH>r=VX{oHO!!K#0)v%Q-w<vQQFtj48p0D4q2e$jFd#F*T3$ZmdC#+g5ezx_ZH!55+A*ryg2C>*JLbEF;Bqge^VX|tj{V@MA`zb&&Zm~nSaVd_n`Ypnqovc?aa503lh%Evq<R!?j3MvJ)leylsvazJJE4Ldb|FDm@9<rOXl&qH;YAFM%#I30-*simzMzL=()wb~?`6lx$S*G@}d8h}mHXj=_q_85eye7pQoUT?U?k(rB$uE{Eptwu5e=Iw-Rj!VsHZQI$VzUvnK(!#P5UTI9WVB;5MKA%lVlDG22ehLugCz7S0=dkr!Yn+re6CBhZnPLIkh9b$#I{7Tk1`Fghq41QS+T+>xwK2Qoh<FD%(=oWDKoaJq_Z<KnYH;a9<EC<t*K6|!?Dk)YpVY((;*?Klq^}cdo-va?xd2oS+7GX=CZ1^+pMl7LbWZh%`P-3!m-@6h^+FZ+pnA`A}*^hDXo#Cc`=(PBCd+8Uo%6oys=QO)VHB7gtG!5oiHD?j<O)G7qUaMrz^xRldxQ-H#3&7o2h6qZl|{}$s_8x7cBvoZ!AwMz@?-r<0p-%O1L;Mh^26}Cb##kytVeOFscr;9-x-27pfYsowO~jx2+B=IjPq#JF5{YucWuIv8}bVR5Z3JD6=*(xu%4z%D1YtBDRdMMy@@pd!^jAbFD?Ke6lI74YsJVtu&~m^tEa$M>Nc_aUr5GbFojay{2-ir?9dque9E+Q8J?*3c1BE$**d*DK$K<oTRd-d=Q+Yfw2&<YcdWo(=3uNHm%z)+%PP)gSPc8T(1(dPqoi89WDtUv#cYnH@EjA)wkrOrn4}qBQM;e)v;PJ4Yf2TqcN*6(=HIO%Bz8)%P!-s;;2fmg0NpO2(^bW;HsOjoV92rA1Tzdldy5E*(2047ObDPW~|Mt7_Y{VceGe2KQ30Y?X)4REwd4{fU!KZ7C1LB?W|Wcu&v#&$1dSA4!0Gn<TTVS@VJ(+O16f!DzMv};3?j)3asv>T`zO1EwJvjATz-mk*)i+JSK;#k*@-=f1&%YJuLt=JSBrM!mdTE9WOJx)UrmYi=P26HYtCkNvXuJO0cFaC$aLh<gqZT2QdaKx~&<sBriRxiX)<{*Q>>>&!+;kKCVM4U$QeY+N$ETdaP0_W2{Fu5UpJ~2e(2dNguu`gtMwBPp_&e^slujhp$4kB{0~t)UO{T`lFq%jUW3pl`gOygQ?OnKeRS505wr8Gq#bq-6^&)Nv&nHy(~K~+b<a<1GAhl<1%qDyqbz8BC#l^XRZ{uO0hq!sxn}xnXFGVjxjl~?5~El=P>=P2&@?^5~`-GYcqkg39iL0A+I;E!K#%psI_u0ny=8WzA%Ta8MB-yAs@N46{T{nPcH5+AtY6x@F-a)=Pj@w=`D*gswX$Qp|CZzYb1NFbSp}@vaRAU2QcEYO&fu*7`M?iYO-goAhHFp*Pwl?+p*U$d^8TW!>o`k$uN+v7q9fUu{HcEpt6}HeI`V#DYaLs<gul<=C4MtSS@g{moH~Ap0IW>J}h1<)-z?ZJurzXEiKhBr7jJzh#<|fDKv;KOfGh@128W%p0IGRSFm#{S}$iSa4(lNaI*TP0Wbls@2z+#cdXK+G#{U)wyeRliz^qQcd!4mk1>6*1-NJ|;I)|~hbSwlr>I{Y0J4-Sf3AKe%q#^g?k_p0F(KQnIxHh5!YSD-w765Y<}k6av@_2$1EW)*XC^~1zcgjC6)0siKQQ((>9CD3wJ$3$D=J1W0<cRbIV1rrKQiUBzph@ivaDAx54b}&8?@4qt1?cqFEg00vN3A32QINHbv1Xe-7hb#?=0oCF0kRMiKuxtbR!t8JGgBzl(bm2I4D}TEhaXufiYFAeXV4&EUjg?d8!v6Ft(d1^et|uOR*oY6|t5t&a5-A^{Qa7<1~dOf2|ZQ88Fu<jItWEh^e`?Hn2c8+#>EEf3(uAPp<8?53U3>gt@t_rYxW^JGTojoVmpnc(h9=ny!?zI<k1D?yfYli8Vbhy)DKj!MO>rRGkX0aIJf(nKT})NGNkKd9Pfk!!vy{TD7pXAv6y(9j;d}%CS4Nld3JHi8P|Fmn^;)(6isNzqTne6t34SL8;Rz@hiBjjI;o?ZmE2(60|X+*)E_i0V(dUuq|-1Mlwn&60-5KXR<Z3NGZ27HmGZ;2`m>dZm`m~#<8U<tgB46V3ZrSCo5#I*{({goU}|O5;oW|C8aB^bTG;?$*Dh?r>ayg#i!CS6ET6Z#4E|Jc{#=@yE0+0C^R>*ksRkNA~epnPcf7-EUX5&o+%VEB(SBhP^@LMX{)oXNVS}?iXyzJd#z<8M>KM)$+QTls<{;~b}V!&U9p$4Z>|xwA}+?ACNs=0{H;AEy|flE%Q6rw%(XW(-Ldd7^)V}%C9)MR{IOx8m@?uq+#<5G<*y4cqBHn1gsjo1KdzTF3ne-osx~yWVlaub+cfSd?k*LtmoXl%lrfp2s4t+g0<l3TTP0(ylP&+Qg|WaWFRm4wTrUGBULXjnJfCqhHm?D(qNb50Q7Kw8xUg_Jxg{$k7^jOb#U$0MYqPK)y|NS#T(Rvmva~O;2rQ4Tf0>aZh_#L^v#{x>crVZ}E3bF9#VqYDfGN2o4KbLkT`d2yB(&e4R4~=O1um{Ctf{&zhcJ{W7A3tX%`j)KzAm#h%`0`NTc}I3y)aL#hqNmv6D;turlevX+9+VTkt>rl>8mTO-!H1Jjx-Ih^stGskgX^&mM?gwnV{*MY_v@zNGn3N2QRd<Z=Izq&9Oe9)GMSef}laK#xTb(3#fpkZ!lr32(~^qtF%}e5UyJ(WwZgYIVBu0g)5K}EHuxvJtMfN+pQUpC^7xGXs|`40x`0tG$7P0wk;5=ShJ)wi!_Ec^)Ui8S*+!!h%dk=@TEYn6}N1!)F!mBK`uxQ;xezMMyOo0y{?QT#4xis`7hrx6|Hq4j<CqJ4X(>AIJKg(s5Q+fZL-p;WwhreH7Yu^Wi@CxNFV4XeKMz<V5HKr11>DEdn;rj#jg&yFteqs?ly_8qAKL66tXF>k|r21cB`nWTCt|J-yP03a;S?m0IjaH&#>UId@2vOH#R4#>8*~obgF}`+A4&oA+DvYEV1e=o3|G=tt5G@L#M|n!l|1qL#w^9#xU)+79jYi0I_B=f-RS_B(*WA<sJYu-mM+AU9g8Qk}fo^7$#?~au)`$W2}#(XDx@bR4PEN5V8ii>$RY)ZZRLUgs`lxku%{hcOnfegS3Y%tT3Xi_OS>tFDO1V1Tu{>pfC2Y2q`(E0=8Tx&@gqh<Rw$L87v~KXep|!A1N_4HK_QfRIrblYO#K;=qxa)rzudj6eUV58MD{2fU~}`BP~<2NUk(3B(dqRleIss&av^eVi}Dpqc!g^m9b*EysSs5HM3K)JTG#z<F10MuPg<#`z(bl|Ex$Qb}-c{k)>8Ii?pycAgjia9k*Mgz_IzWMk%MXp0ph(`zY$F&aY%BJG5glShV*kXCx9b6)||NQYM@*T&W?quQtfFyP|Ta53hML`!JO)H?+C2l&;FNQYS4mGbnj1KCiB@kg{7bHL$X(i?0tXnXxl5!Kairfi^Oz3MGg&ps{Q={<1QofTp;yjVU`URIpbnxUUVhcddgp6S%i5eX%jPU8o<lCb8hUD<O=mpDp>S=`0Yi2DI$3(z7GB1g0i5)F{ESohfTBn<ai66)i=uII`3+<1jfcJg{f2(XcJ7T`ndfG%@0>Of+RGGO{wa#V{u<#4_uxO|p+L!ZOM+Ff^5^HY$!Z8MwW;P$v*Evas|h_NK6|ZL(vr->ti}!ZJd!0kZkGl^(q+Z?wKMsI3$z`n0X6=BH4xZ>z7kxT>kIh^uZf^0b>U7c5Vwow&HS%b?z}7_@UJFs)j)#hD&8IyUXC44n-nbueR?iKj8Rj4OMmBe0({P_#xhuCq@h&Mzo7Loi6V|1_AZ%P``xNwVf3ue2yDpDz0-OR#UPur5-tsVOO{BQEYRKr^{6A*@z3`zs<c1ueF*nJE~oj4hO`w=q*KrZ4ufp19AY2C!i)w={Y-ey2IL-6{E-c`UNCNVEd8R3Krvg{gohf~+W%uC&mwX)my_t1wHhfVUK|7_}ZP9wpbLu&(8?y0#Lp|F{w=Vxq0Jg)eNdXE7@>J|t<W=(Y2va;VF#UNOKhgDN34#V}Mb#woF_{w~BRovI0@u&kjpP84FSFDMQ)HYmfi^elof;<8w})-mQN=_^<oFtUxJ4K*t&2eG9rpQ!C9Z?j;uSgy0VyfKEeTC^WDmbBWkxUyQUi?Jat=&V$%6){0AimvV^K%kSYDzH|v$*Z2KD6w&^HZ;Gk2`yJI5GvfZZZ7{L0;zc%PN>W<8ZM}^1T!M6EvyT%u`Y48SFjH_JgJni2&JbiFer8=Kdtzklda&k3$Y%x{H+zNn=)Q6-z`@xZKF%CO*B?3)U|=C=PmLy1+|SOuB)rCurl1ZNHlu1Laey4xhTPtz^n<d|E*Q3Xsoia<FcQ#$gnjso1P@DW~rYki>(wd^|tgZwX~8b9<s+Q8a0xz<g5s#xt;i>oU`<@Yq6oM+p&D6mL+5=tSI<0#F*!%113+frZ+CB4Y95&1g1POSG6>+OSZ137_}O##w?aCJ}&625;FB4xUt2oA+5f!vaK?(kS2w!uP%u+ZKUt41*Zx$GcQXf=r5iyrm*8F;I(}xc`leS4KowD05SWnv$15cAgeAhQ!ux#p)$xb4Xao+DzT8Su@EJ)Iwc+`Krr^CO|ZeWYAu*D2(+fPiM1*&kFy)MQ?l}{|E-=ax3uT4DYXqInlEKD+_dVgG&4|}*(xiqi79k2M5|=5)+KJV>n>m>ps)Y0OQ>BgqPD!OCbXw1;IuKUv$Be|8L{iLzp|0HoGwGGc_>C9`mV>ij5Gf;zpT%&W~vz}>Kc|b0<+boH=z$O+qw|AT(`(C(6n(iOR!KdJ13(j_caV6)~3L$dL)3SQ7@1vf-`q3Ik?TO6&@9;k|!}RTq(gai7eA6hAj>);<0qCgRR;!2CwR{1uurWP$o>cU@+A&ovj?QXDpj5K%m>Bb1r+dgSh*#Uo`EdTdPJdFC<5t+q3_$-z~Pck~6Ef#k0#Wh%{O@g(g=tC@+_;V76$u6SUc_H8G*L$|=9JavReyK(e#1Ot=a#_#}#?YAu(oilg`@o(OQGjj~B3*SJ=!5;A`^QMJG>GcWlkd#vN3H87F6=(TgMzaBK9G`Y05>@eZAgRl*+1T{yl)+sMEldp>}Xs8D*WVKeZ?y>c&Ik>8_m?M9(jI?tjY%F9n2&|5j&$6thT(m_nt+xxXf3J_Omp9cZp{(~ZeWpRK(X6ec7c~MGV5^&@th;?UC@^9yZ@7ZJNH(dl@TDEFx+|}rGc9GVx2t+EYqL-#LNyMz!>kmlT^M^I(6qRvuCzEdTC}sUTexhnthf%alC~=&bFX%-sH;IO^{j0&SvN|sh&2^8U$C~W0J`@o*EYtne=6~;z#*x#+c1<c8zw)RWh-{AU9dBxc`ZDssI#;!Q?^7jZZ2Z8LaNH3VXiqZ@wE`NWSR}Q1t3hX46$FS8nME&N3lP#xhY1l<}7KpO)%837cm(x;4}ra3a}TR4>L`s1F62Qa5IOsmMSNyBdjL6j~yAU-!fgVuQCj!05U5lN3eLUyQj9V(J+yzQzF!*zqQRPiY*VWfFfA4J0U%@L9Yw67@p%U1F8fq1gllCA2YQu6SIvfr>M~`wXd?Sm9VO<STrN1&Y2mxTdP8{f;6S7gtcw2EiW)Ko+&1^$*vi$t*BzKJ+vdD04$ujrKwl0K`X^9Y^{5>fVEO3fiFz8M>l&cmnS+b3a<SyIIhC92eD{0cc~?<l{03bNV7mIvZk)GSTP&4r8Ebz(kr^Ay)Z>J*&}n2T&o<c3bc19WiLCad@qQxPqeJE4Jqt0-Y|JEsvIY=$1<d~&$qRy?IWN#XESNDI<xOE;k3u1BeNAPW-z<7Cbfbu5-hbNOesgQ*sd!qRwRQgsjwz6ODrm>7qQy5bFf^l%C77+JhGL!fwQoy!M1>}k|d+CTeYG%ma{xF@*Q9@U9nOwNG#8<!5E4teJ+Ek!l<39*fgOrrL?Ox12&<i@2qby%rc!Zc`wwg7A?&%^Pu@Lh$14dd$+i*LaflT9=^-6fGzT`T09D^aJex$`>-RmbT5mz+_d?h=Q72oV=6YaUa&{2Ev-kZ^sgPSuPM$pGB9DTQLmP*bi1Ii->(g|S*_+Xm#z1$Bso#2X{L0rYbG_WBC>h1Q?PHXI<XzFuDO)5LbWrj9ks-<>#gpu>#%UNV4JM6rmV3qRvjrb!!trNvM2g7*)`ZP|E=z-j}#LrIUk5MP_mV_%%JQw1hvyKo+0VAQ?QXRtrd<auB<aCmY9+_*0}91`kDeUo3k}BZYwppgSOE)4<Z??#<%mb)|`B?W3jKWN1>^)sHjCV+a_u?sWZN`0VIgFrn~?<M<VaC!6q=Uuq6z&s5Akz$gle|a438&q%0vVfUr3&7aXyuR<8Oa1s?)8inT(mtgAAcz%gbpfunk-l_E7Rps5WfhP1piFsS4<8MQVoavZ^-j4Q+_XC<#HBdl4qO&c;dbh;fni5x*Sf4O8iCy^C0swmkn!xS;Ed9)rc$fY7U0<^s{!=WLd_q4#IJr-iPf~;|~h#DxiOEB{`QW?uKP%g-<k{IhC0X5Je5TQCaJ2mXK#~(zjJ-kFUyp?*dpf%aGg{7oAV-UExN}5hM53pFTm?HHoJhi?n+oiHJxGt<Prn7P`9<V|&BdBh(*f9~gAtH&n<2rS)sWAey`>)v|HL;+u{;WPGMJGMAyEREM{U=T|(4Wb*MHoJ=At4eq+Ztn`MzHo4mM{UV8LafMzoY`HQL*)(i5$fxA~AckxthTyCN#s4%^^jo@h>bkKNTl2Br<R^EFx|+aIr47hM7Jj#VpyR&l)(aJhmsX_8hIXH@6%%8Jk(MBDhL73Mkz+dbuI7UK5P4)i9PQb0TOd?YAj6(xw0~V6<<oBPvlfEHFzoQXGc3fU{FDX(DE_GPC5STPLO{XPoG)O0@2<rmUbTKP;3l{|cWkT_BvP+p%^i03UC%MzK|~+L0_VG_rZL&zsS%R4OU2f)(7czcM2%aw1x>SuR2}ESO?2;W64Uy_<<H{;{|;bR%fAYObj?-4B_rv!(#31su|{AszZL>8C=qO0cObL7vmFAhO!8b{m1Aow-3bRG8v13b^Pt`>V&ZoU@25F{k)B2f4$z@0{5v#JejuDji;&WV)C+?F_A@bF~XKj<g*zE-8<zKp@JyO}S_~Rx1!XCb;ps9wT?PI=ZVlexpSw61dqeMXk@X9lORkWu8p3<T)g{3LVQWWH4|q*QR4ISE~!PAEBf;0JC+s>!3TcStq_JVlu_8crpDTLaZJqt1XHmBbv=D^tR|SQ<z*Z+_Kg$-J-ymHZsDk$`PHoHLxQ&`kA0BsWpwNn5OD9;x!Jprn7r3Dy+w}XD?8(<uwQ;7oB^g{wT$(gcidvV6)vUXrRuq{J1STvMEV5^05&v8LqmmJi6sD<h5xiA-ke9_bCW6EwoKDJv0z7_9&1o0imlka=d&mprHk}Wh{U)t{H3@y0Wpa<gJ>kXsn#9iy^+bLpmHQS1fe6+cu6d";
static float MN[38],SD[38];
static float WD[13768];
static bool loaded=false;
inline void load(){
if(loaded)return;loaded=true;
// Base85 decode
unsigned char buf[27840];
int bi=0,si=0;int slen=34800;
while(si<slen){
unsigned long long v=0;
int nc=0;for(;nc<5&&si<slen;nc++,si++)v=v*85+(W85[si]-33);
if(nc<5){for(int p=nc;p<5;p++)v=v*85+84;}
for(int k=3;k>=0&&bi<27840;k--)buf[bi++]=(v>>(k*8))&0xFF;
}
// Unpack norm (float32)
for(int i=0;i<38;i++){unsigned u=buf[i*4]|(buf[i*4+1]<<8)|(buf[i*4+2]<<16)|(buf[i*4+3]<<24);float*fp=(float*)&u;MN[i]=*fp;}
for(int i=0;i<38;i++){int o=(i+38)*4;unsigned u=buf[o]|(buf[o+1]<<8)|(buf[o+2]<<16)|(buf[o+3]<<24);float*fp=(float*)&u;SD[i]=*fp;}
// Unpack weights (float16->float32)
int wo=304;
for(int i=0;i<13768;i++){
unsigned short h=buf[wo+i*2]|(buf[wo+i*2+1]<<8);
unsigned s=(h>>15)&1,e=(h>>10)&0x1F,m=h&0x3FF;
unsigned f;
if(e==0){if(m==0)f=s<<31;else{e=1;while(!(m&0x400)){m<<=1;e--;}m&=0x3FF;f=(s<<31)|((e+127-15)<<23)|(m<<13);}}
else if(e==31)f=(s<<31)|0x7F800000|(m<<13);
else f=(s<<31)|((e+127-15)<<23)|(m<<13);
float*fp=(float*)&f;WD[i]=*fp;
}
}
inline void predict(const float raw_input[38],float output[8]){
load();
int o=0;
const float*w1=WD+o;o+=38*128;
const float*b1=WD+o;o+=128;
const float*w2=WD+o;o+=128*64;
const float*b2=WD+o;o+=64;
const float*w3=WD+o;o+=64*8;
const float*b3=WD+o;
float inp[38];
for(int i=0;i<38;i++)inp[i]=(raw_input[i]-MN[i])/(SD[i]+1e-8f);
float h1[128];
for(int j=0;j<128;j++){h1[j]=b1[j];for(int i=0;i<38;i++)h1[j]+=inp[i]*w1[i*128+j];if(h1[j]<0)h1[j]=0;}
float h2[64];
for(int j=0;j<64;j++){h2[j]=b2[j];for(int i=0;i<128;i++)h2[j]+=h1[i]*w2[i*64+j];if(h2[j]<0)h2[j]=0;}
for(int j=0;j<8;j++){output[j]=b3[j];for(int i=0;i<64;i++)output[j]+=h2[i]*w3[i*8+j];}
}
inline int logits_to_thrust(float l0,float lm,float l2){
if(l0>=lm&&l0>=l2)return 0;
if(l2>=l0&&l2>=lm)return 200;
return 100;
}
} // namespace opp_nn


#include <array>
#include <memory>
#include <string>
#include <utility>
#include <vector>
namespace arena {
constexpr int THRUST_SHIELD = -1;
constexpr int THRUST_BOOST = -2;
struct BotAction {
double target_x = 0;
double target_y = 0;
int thrust = 0; 
std::string thrust_str() const {
if (thrust == THRUST_SHIELD) return "SHIELD";
if (thrust == THRUST_BOOST) return "BOOST";
return std::to_string(thrust);
}
};
class Bot {
public:
virtual ~Bot() = default;
virtual void init(
int laps,
const std::vector<std::pair<double, double>>& checkpoints) = 0;
virtual std::pair<BotAction, BotAction> get_actions(
const std::array<std::array<double, 6>, 4>& pods) = 0;
virtual std::unique_ptr<Bot> clone() const = 0;
};
} 
#include <cmath>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <vector>
namespace arena {
namespace ultimate_ga {
static constexpr double PI = 3.14159265358979323846;
static constexpr double DEG_TO_RAD = PI / 180.0;
static constexpr double RAD_TO_DEG = 180.0 / PI;
static constexpr int MAX_H = 8;
static constexpr int MAX_POP = 128;
static constexpr double CP_RADIUS_SQ = 358801.0; 
static constexpr double CP_RADIUS_SQ_EXACT = 360000.0; 
static constexpr double POD_RADIUS = 400.0;
static constexpr double FRICTION = 0.85;
static constexpr double SHIELD_MASS = 10.0;
static constexpr int SHIELD_DURATION = 4;
static constexpr int BOOST_THRUST = 650;
static constexpr int MAX_THRUST = 200;
static constexpr double MAX_TURN_DEG = 18.0;
static constexpr double MAX_TURN_RAD = MAX_TURN_DEG * DEG_TO_RAD;
static constexpr int FIRST_TURN_BUDGET_US = 1000000; 
static constexpr int TURN_BUDGET_US = 75000; 
static constexpr int IBR_BUDGET_US = 2000; 
static constexpr int MIN_PHASE2_BUDGET_US = 30000; 
static constexpr int MCTS_BUDGET_US = 1000; 
static constexpr int GA_POP_SIZE = 80; 
static constexpr int OPP_POP_SIZE = 32; 
static constexpr int GA_HORIZON = 6; 
static constexpr double CROSSOVER_PROB = 0.25; 
static constexpr double MUTATION_BASE_PROB = 0.25; 
static constexpr bool BANG_BANG_THRUST = false; 
static constexpr double WIN_BONUS = 1e9;
static constexpr double CP_CROSS_BONUS = 15000.0;
static constexpr double ACTIVATION_PENALTY = 1000.0;
static constexpr double PROGRESS_DIFF_WEIGHT = 5000.0;
static constexpr double OPP_PROGRESS_PENALTY = 10000.0;
static constexpr double OPP_BLOCKER_PROX_WEIGHT = 0.5;
static constexpr double BLOCKER_DIST_THRESHOLD = 2200.0; 
static constexpr double BLOCKER_SHIELD_DIST = 850.0;
static constexpr double BLOCKER_INTERCEPT_LEAD = 1.5; 
static constexpr double BLOCKER_CP_OFFSET = 500.0; 
static constexpr double BLOCKER_PROX_CLOSE = 800.0; 
static constexpr double BLOCKER_PROX_MED = 2000.0; 
static constexpr double BLOCKER_PROX_FAR = 5000.0; 
static constexpr double BLOCKER_PROX_WEIGHT = 3.5; 
static constexpr double BLOCKER_PERSTEP_PROX_WEIGHT = 0.5; 
static constexpr double BLOCKER_INTERPOSE_WEIGHT = 0.8; 
static constexpr double BLOCKER_SPEED_LOSS_WEIGHT = 1.5; 
static constexpr double BLOCKER_MISALIGN_WEIGHT = 0.4; 
static constexpr double HANDLING_AVG_DIST = 6500.0;
static constexpr int HANDLING_MIN_CPS = 5;
struct RNG {
unsigned s = 42;
unsigned xr() { s = (214013 * s + 2531011); return (s >> 16) & 0x7FFF; }
int ri(int a, int b) { return b <= a ? a : a + (int)(xr() % (unsigned)(b - a + 1)); }
double rd() { return (double)xr() / 32767.0; }
void seed(unsigned v) { s = v; }
};
inline int rand_thrust(RNG& rng) {
if (BANG_BANG_THRUST) return rng.ri(0, 1) * 200;
int raw = rng.ri(-100, 500);
return raw < 0 ? 0 : (raw > 200 ? 200 : raw);
}
inline int mutate_thrust(RNG& rng, int current) {
if (BANG_BANG_THRUST) return (current == 0) ? 200 : 0; 
int v = current + rng.ri(-50, 50);
return v < 0 ? 0 : (v > 200 ? 200 : v);
}
struct Pod {
double x, y;
double vx, vy;
double angle;
int next;
int lap;
int shieldtimer;
int boosted;
int isFirstTurn;
};
inline double newCollide(const Pod* p1, const Pod* p2) {
double dx = p2->x - p1->x;
double dy = p2->y - p1->y;
double pLength2 = dx * dx + dy * dy;
if (pLength2 <= 640000.0) return 0.0;
double dvx = p2->vx - p1->vx;
double dvy = p2->vy - p1->vy;
double dot_val = dx * dvx + dy * dvy;
if (dot_val > 0.0) return 10.0;
double vLength2 = dvx * dvx + dvy * dvy;
if (vLength2 == 0.0) return 10.0;
double disc = dot_val * dot_val - vLength2 * (pLength2 - 640000.0);
if (disc <= 0.0) return 10.0;
double t = (-dot_val - sqrt(disc)) / vLength2;
return t;
}
inline int cpCollide(double p1x, double p1y, double p2x, double p2y,
double cpx, double cpy) {
double dx = p2x - p1x;
double dy = p2y - p1y;
double pd2 = dx * dx + dy * dy;
double ppx = p1x, ppy = p1y;
if (pd2 != 0.0) {
double u = ((cpx - p1x) * dx + (cpy - p1y) * dy) / pd2;
if (u > 1.0) { ppx = p2x; ppy = p2y; }
else if (u > 0.0) { ppx = p1x + u * dx; ppy = p1y + u * dy; }
}
double distSQ = (ppx - cpx) * (ppx - cpx) + (ppy - cpy) * (ppy - cpy);
return distSQ < CP_RADIUS_SQ;
}
inline void bounce(Pod* a, Pod* b) {
double nx = b->x - a->x;
double ny = b->y - a->y;
double dist = sqrt(nx * nx + ny * ny);
nx /= dist; ny /= dist;
double rvx = a->vx - b->vx;
double rvy = a->vy - b->vy;
double m1 = (a->shieldtimer == 4) ? 0.1 : 1.0;
double m2 = (b->shieldtimer == 4) ? 0.1 : 1.0;
double force = (nx * rvx + ny * rvy) / (m1 + m2);
if (force < 120.0) force += 120.0;
else force += force;
a->vx -= nx * force * m1;
a->vy -= ny * force * m1;
b->vx += nx * force * m2;
b->vy += ny * force * m2;
if (dist <= 800.0) {
double ddiff = dist - 800.0;
a->x += nx * -(-ddiff / 2.0 + 0.00001);
a->y += ny * -(-ddiff / 2.0 + 0.00001);
b->x += nx * (-ddiff / 2.0 + 0.00001);
b->y += ny * (-ddiff / 2.0 + 0.00001);
}
}
inline void nextTurn(Pod pods[4], const double* CX, const double* CY,
int ncp, int laps) {
double remaining = 1.0;
double curps_x[4], curps_y[4];
for (int i = 0; i < 4; i++) { curps_x[i] = pods[i].x; curps_y[i] = pods[i].y; }
while (remaining > 0.0) {
double first = remaining;
int ci = -1, cj = -1;
for (int i = 3; i > 0; --i) {
for (int j = i - 1; j >= 0; --j) {
double t = newCollide(&pods[i], &pods[j]);
if (t <= first) { first = t; ci = i; cj = j; }
}
}
for (int i = 0; i < 4; i++) {
pods[i].x += pods[i].vx * first;
pods[i].y += pods[i].vy * first;
}
remaining -= first;
if (ci != -1) {
bounce(&pods[ci], &pods[cj]);
}
if (remaining > 0.0 && ci != -1) {
int c_idx[2] = {ci, cj};
for (int k = 0; k < 2; k++) {
int idx = c_idx[k];
if (pods[idx].lap >= laps) continue;
if (cpCollide(curps_x[idx], curps_y[idx], pods[idx].x, pods[idx].y,
CX[pods[idx].next], CY[pods[idx].next])) {
pods[idx].next++;
if (pods[idx].next >= ncp) { pods[idx].next = 0; pods[idx].lap++; }
}
}
curps_x[ci] = pods[ci].x; curps_y[ci] = pods[ci].y;
curps_x[cj] = pods[cj].x; curps_y[cj] = pods[cj].y;
}
}
for (int i = 0; i < 4; i++) {
pods[i].vx = trunc(pods[i].vx * 0.85);
pods[i].vy = trunc(pods[i].vy * 0.85);
pods[i].x = floor(pods[i].x + 0.5);
pods[i].y = floor(pods[i].y + 0.5);
if (pods[i].shieldtimer > 0) pods[i].shieldtimer--;
}
for (int i = 0; i < 4; i++) {
if (pods[i].lap >= laps) continue;
if (cpCollide(curps_x[i], curps_y[i], pods[i].x, pods[i].y,
CX[pods[i].next], CY[pods[i].next])) {
pods[i].next++;
if (pods[i].next >= ncp) { pods[i].next = 0; pods[i].lap++; }
}
}
for (int i = 0; i < 4; i++) {
if (pods[i].lap >= laps) continue;
double dx = pods[i].x - CX[pods[i].next];
double dy = pods[i].y - CY[pods[i].next];
if (dx * dx + dy * dy == CP_RADIUS_SQ_EXACT) {
pods[i].next++;
if (pods[i].next >= ncp) { pods[i].next = 0; pods[i].lap++; }
}
}
}
inline void apply_action(Pod* p, double target_x, double target_y,
int thrust, int shield, int boost) {
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
double diff = fmod(a - p->angle, 2 * PI);
if (diff >= PI) diff -= 2 * PI;
else if (diff <= -PI) diff += 2 * PI;
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
inline void apply_action_direct(Pod* p, double new_angle,
int thrust, int shield, int boost) {
if (shield) {
p->shieldtimer = 4;
if (p->isFirstTurn) p->isFirstTurn = 0;
return;
}
if (p->isFirstTurn) {
p->isFirstTurn = 0;
}
p->angle = new_angle;
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
double cc = cos(p->angle);
double cs = sin(p->angle);
if (fabs(cc - (-0.28)) < 5e-16 && fabs(fabs(cs) - 0.96) < 5e-16) {
cc = -0.28;
cs = (cs > 0) ? 0.96 : -0.96;
}
p->vx += cc * t_val;
p->vy += cs * t_val;
}
inline void apply_action_delta(Pod* p, double delta_deg, int thrust,
int shield, int boost) {
if (shield) {
p->shieldtimer = 4;
if (p->isFirstTurn) p->isFirstTurn = 0;
return;
}
double delta_rad = delta_deg * DEG_TO_RAD;
if (delta_rad > 18.0 * DEG_TO_RAD) delta_rad = 18.0 * DEG_TO_RAD;
else if (delta_rad < -18.0 * DEG_TO_RAD) delta_rad = -18.0 * DEG_TO_RAD;
if (p->isFirstTurn) {
p->isFirstTurn = 0;
}
p->angle += delta_rad;
int t_val = 0;
if (boost && p->boosted == 0) {
t_val = 650; p->boosted = 1;
} else {
t_val = thrust;
if (t_val > 200) t_val = 200;
if (t_val < 0) t_val = 0;
}
if (p->shieldtimer > 0) t_val = 0;
p->vx += cos(p->angle) * t_val;
p->vy += sin(p->angle) * t_val;
}
struct Ind {
double ra[MAX_H], ba[MAX_H];
int rt[MAX_H], bt[MAX_H];
int rs, bs;
double sc;
};
inline double evaluate(
const Ind* ind, const Pod* base, int H,
const double* CX, const double* CY, const double* EX, const double* EY,
const double* DTE, const double* RRX, const double* RRY,
int ncp, int laps, int mx,
int rp, int bp, int orp, int obp, int rb,
int fboost, int rtimeout,
double dw, double aw, double sw, double lw, double apw,
double byw, double opw, double sfw, double faw, double rw,
const double* opp_ra = nullptr, const int* opp_rt = nullptr,
const double* opp_ba = nullptr, const int* opp_bt = nullptr
) {
Pod sim[4]; memcpy(sim, base, 4 * sizeof(Pod));
int ir_cp = sim[rp].next, ir_lap = sim[rp].lap;
int ior_cp = sim[orp].next, ior_lap = sim[orp].lap;
double r_act = (double)H + 0.3, o_act = (double)H + 0.3;
double opp_init_speed = sqrt(sim[orp].vx*sim[orp].vx + sim[orp].vy*sim[orp].vy);
double blocker_prox_sum = 0.0; 
for (int t = 0; t < H; t++) {
int pre_next[4];
for (int k = 0; k < 4; k++) pre_next[k] = sim[k].next;
int rth = ind->rt[t];
int rb_flag = 0, rs_flag = 0;
if (t == 0 && fboost && sim[rp].shieldtimer == 0) rb_flag = 1;
else if (t == ind->rs && ind->rs < 3 && sim[rp].shieldtimer == 0) rs_flag = 1;
apply_action_delta(&sim[rp], ind->ra[t], rth, rs_flag, rb_flag);
int bth = ind->bt[t];
int bs_flag = 0;
if (t == ind->bs && ind->bs < 3 && sim[bp].shieldtimer == 0) bs_flag = 1;
apply_action_delta(&sim[bp], ind->ba[t], bth, bs_flag, 0);
if (opp_ra) {
apply_action_delta(&sim[orp], opp_ra[t], opp_rt[t], 0, 0);
apply_action_delta(&sim[obp], opp_ba[t], opp_bt[t], 0, 0);
} else {
apply_action(&sim[orp], EX[sim[orp].next], EY[sim[orp].next], 200, 0, 0);
double tx2 = sim[rp].x + sim[rp].vx * 1.5;
double ty2 = sim[rp].y + sim[rp].vy * 1.5;
double dx_cp = CX[sim[rp].next] - sim[rp].x;
double dy_cp = CY[sim[rp].next] - sim[rp].y;
double dcp = sqrt(dx_cp * dx_cp + dy_cp * dy_cp);
if (dcp > 0) { tx2 -= 500 * dx_cp / dcp; ty2 -= 500 * dy_cp / dcp; }
double dd = sqrt((sim[obp].x - sim[rp].x) * (sim[obp].x - sim[rp].x) +
(sim[obp].y - sim[rp].y) * (sim[obp].y - sim[rp].y));
int sh = (dd < 850 && sim[obp].shieldtimer == 0) ? 1 : 0;
apply_action(&sim[obp], tx2, ty2, 200, sh, 0);
}
nextTurn(sim, CX, CY, ncp, laps);
for (int k = 0; k < 4; k++) {
if (sim[k].next != pre_next[k]) {
if (k == rp && r_act > (double)H) r_act = (double)t + 0.5;
if (k == orp && o_act > (double)H) o_act = (double)t + 0.5;
}
}
if (!rtimeout) {
double bdx = sim[bp].x - sim[orp].x;
double bdy = sim[bp].y - sim[orp].y;
double bd2 = bdx * bdx + bdy * bdy;
if (bd2 < BLOCKER_PROX_FAR * BLOCKER_PROX_FAR) {
double bd = sqrt(bd2);
double norm = 1.0 - bd / BLOCKER_PROX_FAR;
blocker_prox_sum += norm * norm; 
}
}
}
if (sim[rp].x < -2000 || sim[rp].x > 18000 ||
sim[rp].y < -2000 || sim[rp].y > 11000)
return -1e9;
double s = 0;
for (int k = 0; k < 2; k++) {
int pi2 = (k == 0) ? rp : bp;
if (sim[pi2].x < -1000 || sim[pi2].x > 17000 ||
sim[pi2].y < -1000 || sim[pi2].y > 10000)
s -= 100000.0;
}
if (sim[rp].lap >= laps) s += WIN_BONUS;
if (sim[orp].lap >= laps) s -= WIN_BONUS;
{
int cur = sim[rp].lap * ncp + sim[rp].next;
int ini = ir_lap * ncp + ir_cp;
int crossed = cur - ini;
if (crossed > 0) s += crossed * CP_CROSS_BONUS;
}
if (sim[rp].lap < laps) {
int rl = sim[rp].lap * ncp + sim[rp].next;
if (rl < mx) {
double dx = sim[rp].x - EX[sim[rp].next];
double dy = sim[rp].y - EY[sim[rp].next];
s -= (DTE[rl] + sqrt(dx * dx + dy * dy)) * dw;
}
}
{
int our_progress = sim[rp].lap * ncp + sim[rp].next;
int opp_progress = sim[orp].lap * ncp + sim[orp].next;
int diff = our_progress - opp_progress;
s += diff * PROGRESS_DIFF_WEIGHT;
if (diff == 0) {
double our_dx = sim[rp].x - CX[sim[rp].next], our_dy = sim[rp].y - CY[sim[rp].next];
double opp_dx = sim[orp].x - CX[sim[orp].next], opp_dy = sim[orp].y - CY[sim[orp].next];
s += (sqrt(opp_dx*opp_dx+opp_dy*opp_dy) - sqrt(our_dx*our_dx+our_dy*our_dy)) * 0.5;
}
}
{
double dx_to_cp = CX[sim[rp].next] - sim[rp].x, dy_to_cp = CY[sim[rp].next] - sim[rp].y;
double dist_to_cp = sqrt(dx_to_cp*dx_to_cp + dy_to_cp*dy_to_cp);
if (dist_to_cp < 3000.0) {
int next_next = (sim[rp].next + 1) % ncp;
double nnx = CX[next_next] - sim[rp].x, nny = CY[next_next] - sim[rp].y;
double nnd = sqrt(nnx*nnx + nny*nny);
if (nnd > 0) { double alignment = (sim[rp].vx*nnx + sim[rp].vy*nny) / nnd; s += alignment * 1.5; }
}
}
{
double dx = EX[sim[rp].next] - sim[rp].x;
double dy = EY[sim[rp].next] - sim[rp].y;
double d = sqrt(dx * dx + dy * dy);
if (d > 0) {
double nx = dx / d, ny = dy / d;
s += (sim[rp].vx * nx + sim[rp].vy * ny) * aw;
double lat = sim[rp].vx * ny - sim[rp].vy * nx;
s -= fabs(lat) * lw;
double ta = atan2(dy, dx);
double ae = ta - sim[rp].angle;
while (ae > PI) ae -= 2 * PI;
while (ae < -PI) ae += 2 * PI;
s -= fabs(ae * RAD_TO_DEG) * apw;
}
s += sqrt(sim[rp].vx * sim[rp].vx + sim[rp].vy * sim[rp].vy) * sw;
}
{
double dx_cp = CX[sim[rp].next] - sim[rp].x;
double dy_cp = CY[sim[rp].next] - sim[rp].y;
double dist_cp_sq = dx_cp * dx_cp + dy_cp * dy_cp;
if (dist_cp_sq < 9000000.0) { 
int nn = (sim[rp].next + 1) % ncp;
double nnx = CX[nn] - sim[rp].x, nny = CY[nn] - sim[rp].y;
double nnd_sq = nnx * nnx + nny * nny;
if (nnd_sq > 1.0) {
double nnd = sqrt(nnd_sq);
double alignment = (sim[rp].vx * nnx + sim[rp].vy * nny) / nnd;
s += alignment * 0.3;
}
}
}
s -= ACTIVATION_PENALTY * r_act;
{
double pred_obx = sim[obp].x + sim[obp].vx * 1.85;
double pred_oby = sim[obp].y + sim[obp].vy * 1.85;
double pred_rx = sim[rp].x + sim[rp].vx * 1.85;
double pred_ry = sim[rp].y + sim[rp].vy * 1.85;
double dx = pred_obx - pred_rx, dy = pred_oby - pred_ry;
double d2 = dx*dx + dy*dy;
double threshold2 = 3000.0 * 3000.0; 
if (d2 < threshold2) {
double penalty = byw * 20.0 * (1.0 - d2 / threshold2);
double cvx = sim[obp].vx - sim[rp].vx, cvy = sim[obp].vy - sim[rp].vy;
double closing = -(dx*cvx + dy*cvy); 
if (closing > 0 && d2 > 1.0) penalty += closing * 0.5;
if (sim[obp].shieldtimer >= 3) penalty *= 1.5;
s -= penalty;
}
}
s += 10000.0 * opw * o_act;
if (sim[orp].next == ior_cp && sim[orp].lap == ior_lap) {
double dx = sim[orp].x - CX[sim[orp].next], dy = sim[orp].y - CY[sim[orp].next];
s += 10.0 * opw * sqrt(dx * dx + dy * dy);
}
if (rtimeout == 1) {
if (sim[bp].lap < laps) {
int bl = sim[bp].lap * ncp + sim[bp].next;
if (bl < mx) {
double dx = sim[bp].x - EX[sim[bp].next], dy = sim[bp].y - EY[sim[bp].next];
s -= (DTE[bl] + sqrt(dx * dx + dy * dy)) * dw;
}
}
} else if (rtimeout == 2) {
double dx_br = sim[rp].x - sim[bp].x, dy_br = sim[rp].y - sim[bp].y;
double dist_br = sqrt(dx_br*dx_br + dy_br*dy_br);
if (dist_br < 2000) {
s += (2000 - dist_br) * 2.0;
} else {
s -= (dist_br - 2000) * 1.0; 
}
double cp_dx = CX[sim[rp].next] - sim[rp].x;
double cp_dy = CY[sim[rp].next] - sim[rp].y;
double cp_d = sqrt(cp_dx*cp_dx + cp_dy*cp_dy);
if (cp_d > 1.0 && dist_br > 1.0) {
double alignment = (dx_br * cp_dx + dy_br * cp_dy) / (dist_br * cp_d);
s += alignment * 3000.0; 
}
if (dist_br > 1.0) {
double vel_toward = (sim[bp].vx * dx_br + sim[bp].vy * dy_br) / dist_br;
if (vel_toward > 0) s += vel_toward * 5.0;
}
} else {
{
double bx = sim[bp].x - sim[orp].x, by = sim[bp].y - sim[orp].y;
double cx = CX[rb] - sim[orp].x, cy = CY[rb] - sim[orp].y;
s -= sfw * atan2(fabs(bx * cy - by * cx), bx * cx + by * cy);
}
{
double dx = sim[orp].x - sim[bp].x, dy = sim[orp].y - sim[bp].y;
double ta = atan2(dy, dx);
double ae = ta - sim[bp].angle;
while (ae > PI) ae -= 2 * PI;
while (ae < -PI) ae += 2 * PI;
s -= faw * fabs(ae * RAD_TO_DEG);
}
{
double dx = sim[bp].x - RRX[rb], dy = sim[bp].y - RRY[rb];
double d = sqrt(dx * dx + dy * dy);
s -= rw * (d < 300 ? (d - 300) * 0.1 : d - 300);
}
{
double dx = sim[bp].x - sim[orp].x, dy = sim[bp].y - sim[orp].y;
double dist = sqrt(dx * dx + dy * dy);
if (dist < BLOCKER_PROX_FAR) {
double norm = (BLOCKER_PROX_FAR - dist) / (BLOCKER_PROX_FAR - BLOCKER_PROX_CLOSE);
if (norm > 1.0) norm = 1.0;
s += norm * BLOCKER_PROX_WEIGHT * 1000.0;
if (dist < BLOCKER_PROX_MED) {
double close_norm = (BLOCKER_PROX_MED - dist) / (BLOCKER_PROX_MED - BLOCKER_PROX_CLOSE);
if (close_norm > 1.0) close_norm = 1.0;
s += close_norm * close_norm * BLOCKER_PROX_WEIGHT * 2000.0;
}
}
if (dist > BLOCKER_PROX_MED) {
s -= (dist - BLOCKER_PROX_MED) * 0.5;
}
}
{
double opp_to_cp_x = CX[sim[orp].next] - sim[orp].x;
double opp_to_cp_y = CY[sim[orp].next] - sim[orp].y;
double opp_to_cp_d = sqrt(opp_to_cp_x * opp_to_cp_x + opp_to_cp_y * opp_to_cp_y);
if (opp_to_cp_d > 1.0) {
double nx = opp_to_cp_x / opp_to_cp_d;
double ny = opp_to_cp_y / opp_to_cp_d;
double bx = sim[bp].x - sim[orp].x;
double by = sim[bp].y - sim[orp].y;
double proj = bx * nx + by * ny;
double lat = fabs(bx * ny - by * nx);
if (proj > 0 && proj < opp_to_cp_d) {
double frac = proj / opp_to_cp_d;
double pos_quality = 1.0 - fabs(frac - 0.35) * 2.0;
if (pos_quality < 0) pos_quality = 0;
double lat_quality = 1.0 - lat / 1500.0;
if (lat_quality < 0) lat_quality = 0;
s += pos_quality * lat_quality * BLOCKER_INTERPOSE_WEIGHT * 1000.0;
}
}
}
s += CP_CROSS_BONUS * o_act;
{
double opp_end_speed = sqrt(sim[orp].vx*sim[orp].vx + sim[orp].vy*sim[orp].vy);
double speed_loss = opp_init_speed - opp_end_speed;
if (speed_loss > 0) s += speed_loss * BLOCKER_SPEED_LOSS_WEIGHT;
}
{
double dx = CX[sim[orp].next] - sim[orp].x;
double dy = CY[sim[orp].next] - sim[orp].y;
double d = sqrt(dx*dx + dy*dy);
if (d > 0) {
double alignment = (sim[orp].vx*dx + sim[orp].vy*dy) / d;
s -= alignment * BLOCKER_MISALIGN_WEIGHT;
}
}
s += BLOCKER_PERSTEP_PROX_WEIGHT * blocker_prox_sum * 1000.0;
}
if (ind->rs < 3) s -= 330.0;
if (ind->bs < 3) s -= 495.0;
if (ind->rs >= H && ind->rt[0] > 0) s += ind->rt[0] * 0.16;
if (ind->bs >= H && ind->bt[0] > 0) s += ind->bt[0] * 0.01;
return s;
}
inline double evaluate_opp_perspective(
const Ind* ind, const Pod* base, int H,
const double* CX, const double* CY, const double* EX, const double* EY,
const double* DTE, int ncp, int laps, int mx,
int rp, int bp, int orp, int obp,
const double* our_ra, const int* our_rt,
const double* our_ba, const int* our_bt,
double dw, double aw, double sw
) {
Pod sim[4]; memcpy(sim, base, 4 * sizeof(Pod));
int ior_cp = sim[orp].next, ior_lap = sim[orp].lap;
int ir_cp = sim[rp].next, ir_lap = sim[rp].lap;
for (int t = 0; t < H; t++) {
apply_action_delta(&sim[rp], our_ra[t], our_rt[t], 0, 0);
apply_action_delta(&sim[bp], our_ba[t], our_bt[t], 0, 0);
apply_action_delta(&sim[orp], ind->ra[t], ind->rt[t], 0, 0);
apply_action_delta(&sim[obp], ind->ba[t], ind->bt[t], 0, 0);
nextTurn(sim, CX, CY, ncp, laps);
}
if (sim[orp].x < -2000 || sim[orp].x > 18000 ||
sim[orp].y < -2000 || sim[orp].y > 11000)
return -1e9;
double s = 0;
if (sim[orp].lap >= laps) s += WIN_BONUS;
if (sim[rp].lap >= laps) s -= WIN_BONUS;
{ int cur=sim[orp].lap*ncp+sim[orp].next, ini=ior_lap*ncp+ior_cp;
if(cur-ini>0) s+=(cur-ini)*CP_CROSS_BONUS; }
if (sim[orp].lap < laps) { int rl=sim[orp].lap*ncp+sim[orp].next;
if (rl<mx) { double dx=sim[orp].x-EX[sim[orp].next],dy=sim[orp].y-EY[sim[orp].next];
s -= dw*(DTE[rl]+sqrt(dx*dx+dy*dy)); } }
{ double dx=EX[sim[orp].next]-sim[orp].x,dy=EY[sim[orp].next]-sim[orp].y,d=sqrt(dx*dx+dy*dy);
if(d>0){double nx=dx/d,ny=dy/d; s+=(sim[orp].vx*nx+sim[orp].vy*ny)*aw;}
s+=sqrt(sim[orp].vx*sim[orp].vx+sim[orp].vy*sim[orp].vy)*sw; }
{ int opp_progress=sim[orp].lap*ncp+sim[orp].next; int our_progress=sim[rp].lap*ncp+sim[rp].next;
int diff=opp_progress-our_progress; s+=diff*PROGRESS_DIFF_WEIGHT;
if(diff==0){double opp_dx=sim[orp].x-CX[sim[orp].next],opp_dy=sim[orp].y-CY[sim[orp].next];
double our_dx=sim[rp].x-CX[sim[rp].next],our_dy=sim[rp].y-CY[sim[rp].next];
s+=(sqrt(our_dx*our_dx+our_dy*our_dy)-sqrt(opp_dx*opp_dx+opp_dy*opp_dy))*0.5;} }
{ int cur=sim[rp].lap*ncp+sim[rp].next, ini=ir_lap*ncp+ir_cp;
if(cur-ini>0) s-=(cur-ini)*10000.0; }
{ double dx=sim[obp].x-sim[rp].x,dy=sim[obp].y-sim[rp].y;
s -= 0.5*sqrt(dx*dx+dy*dy); }
return s;
}
static constexpr int OPP_PS = OPP_POP_SIZE;
inline void run_opp_prediction_ga(
RNG& rng, const Pod* base, int H, int mx,
const double* CX, const double* CY, const double* EX, const double* EY,
const double* DTE, int ncp, int laps,
int rp, int bp, int orp, int obp,
const double* our_ra, const int* our_rt,
const double* our_ba, const int* our_bt,
const double* opp_prev_ra, const int* opp_prev_rt,
const double* opp_prev_ba, const int* opp_prev_bt,
int has_opp_prev,
int budget_us,
double dw, double aw, double sw,
int has_obs, double obs_r_delta, int obs_r_thrust,
double obs_b_delta, int obs_b_thrust,
double* out_ora, int* out_ort, double* out_oba, int* out_obt
) {
Ind pop[OPP_PS];
int idx = 0;
if (has_opp_prev) {
for (int t=0;t<H-1;t++){pop[0].ra[t]=opp_prev_ra[t+1];pop[0].rt[t]=opp_prev_rt[t+1];
pop[0].ba[t]=opp_prev_ba[t+1];pop[0].bt[t]=opp_prev_bt[t+1];}
pop[0].ra[H-1]=rng.rd()*36-18;pop[0].rt[H-1]=rand_thrust(rng);
pop[0].ba[H-1]=rng.rd()*36-18;pop[0].bt[H-1]=rand_thrust(rng);
pop[0].rs=H;pop[0].bs=H;idx=1;
}
if (idx < OPP_PS) {
for (int t=0;t<H;t++){pop[idx].ra[t]=0;pop[idx].rt[t]=200;pop[idx].ba[t]=0;pop[idx].bt[t]=200;}
pop[idx].rs=H;pop[idx].bs=H;idx++;
}
if (has_obs && idx < OPP_PS) {
for (int t=0;t<H;t++){
pop[idx].ra[t]=obs_r_delta;pop[idx].rt[t]=obs_r_thrust;
pop[idx].ba[t]=obs_b_delta;pop[idx].bt[t]=obs_b_thrust;
}
pop[idx].rs=H;pop[idx].bs=H;idx++;
}
for (int i=idx;i<OPP_PS;i++) {
for (int t=0;t<H;t++) {
double v=rng.ri(-400,400)/10.0;pop[i].ra[t]=v<-18?-18:(v>18?18:v);
pop[i].rt[t]=rand_thrust(rng);
v=rng.ri(-400,400)/10.0;pop[i].ba[t]=v<-18?-18:(v>18?18:v);
pop[i].bt[t]=rand_thrust(rng);
}
pop[i].rs=H;pop[i].bs=H;
}
int best=0,worst=0;
for (int i=0;i<OPP_PS;i++) {
pop[i].sc=evaluate_opp_perspective(&pop[i],base,H,CX,CY,EX,EY,DTE,ncp,laps,mx,rp,bp,orp,obp,our_ra,our_rt,our_ba,our_bt,dw,aw,sw);
if(pop[i].sc>pop[best].sc)best=i;
if(pop[i].sc<pop[worst].sc)worst=i;
}
double wsc=pop[worst].sc;
struct timespec t0,tn;clock_gettime(CLOCK_MONOTONIC,&t0);
int iters=0;Ind child;
while(1) {
iters++;if((iters&127)==0){clock_gettime(CLOCK_MONOTONIC,&tn);
long el=(tn.tv_sec-t0.tv_sec)*1000000L+(tn.tv_nsec-t0.tv_nsec)/1000L;if(el>=budget_us)break;}
int p1=rng.ri(0,OPP_PS-1),p2=rng.ri(0,OPP_PS-1);int par=(pop[p1].sc>=pop[p2].sc)?p1:p2;
if(rng.ri(0,3)==0){p1=rng.ri(0,OPP_PS-1);p2=rng.ri(0,OPP_PS-1);int par2=(pop[p1].sc>=pop[p2].sc)?p1:p2;
for(int t=0;t<H;t++){if(rng.rd()<0.5){child.ra[t]=pop[par].ra[t];child.rt[t]=pop[par].rt[t];}else{child.ra[t]=pop[par2].ra[t];child.rt[t]=pop[par2].rt[t];}
if(rng.rd()<0.5){child.ba[t]=pop[par].ba[t];child.bt[t]=pop[par].bt[t];}else{child.ba[t]=pop[par2].ba[t];child.bt[t]=pop[par2].bt[t];}}
}else{for(int t=0;t<H;t++){child.ra[t]=pop[par].ra[t];child.rt[t]=pop[par].rt[t];child.ba[t]=pop[par].ba[t];child.bt[t]=pop[par].bt[t];
if(rng.rd()<0.5){double v=rng.ri(-400,400)/10.0;child.ra[t]=v<-18?-18:(v>18?18:v);}
if(rng.rd()<0.5){child.rt[t]=rand_thrust(rng);}
if(rng.rd()<0.5){double v=rng.ri(-400,400)/10.0;child.ba[t]=v<-18?-18:(v>18?18:v);}
if(rng.rd()<0.5){child.bt[t]=rand_thrust(rng);}}}
child.rs=H;child.bs=H;
{int si=rng.ri(0,H-1);double v=child.ra[si]+rng.rd()*24-12;child.ra[si]=v<-18?-18:(v>18?18:v);}
{int si=rng.ri(0,H-1);child.rt[si]=mutate_thrust(rng,child.rt[si]);}
{int si=rng.ri(0,H-1);double v=child.ba[si]+rng.rd()*24-12;child.ba[si]=v<-18?-18:(v>18?18:v);}
{int si=rng.ri(0,H-1);child.bt[si]=mutate_thrust(rng,child.bt[si]);}
child.sc=evaluate_opp_perspective(&child,base,H,CX,CY,EX,EY,DTE,ncp,laps,mx,rp,bp,orp,obp,our_ra,our_rt,our_ba,our_bt,dw,aw,sw);
if(child.sc>pop[best].sc)best=worst;
pop[worst]=child;
if(child.sc>wsc){worst=0;wsc=pop[0].sc;for(int i=1;i<OPP_PS;i++)if(pop[i].sc<wsc){worst=i;wsc=pop[i].sc;}}
}
memcpy(out_ora,pop[best].ra,H*sizeof(double));memcpy(out_ort,pop[best].rt,H*sizeof(int));
memcpy(out_oba,pop[best].ba,H*sizeof(double));memcpy(out_obt,pop[best].bt,H*sizeof(int));
}
static constexpr int MCTS_N_ANGLES = 7;
static constexpr int MCTS_N_THRUSTS = 4;
static constexpr int MCTS_N_CANDIDATES = MCTS_N_ANGLES * MCTS_N_THRUSTS; 
static constexpr double MCTS_ANGLES[MCTS_N_ANGLES] = {-18, -12, -6, 0, 6, 12, 18};
static constexpr int MCTS_THRUSTS[MCTS_N_THRUSTS] = {0, 100, 150, 200};
static constexpr int MCTS_TOP_K = 5; 
inline int mcts_first_move_probe(
RNG& rng,
const Pod* base, int H,
const double* CX, const double* CY,
const double* EX, const double* EY,
const double* DTE, const double* RRX, const double* RRY,
int ncp, int laps, int mx,
int rp, int bp, int orp, int obp, int rb,
int fboost, int rtimeout,
double dw, double aw, double sw, double lw, double apw,
double byw, double opw, double sfw, double faw, double rw,
const double* opp_ra, const int* opp_rt,
const double* opp_ba, const int* opp_bt,
int budget_us,
Ind* out_seeds 
) {
double sum_score[MCTS_N_CANDIDATES] = {};
int visit_count[MCTS_N_CANDIDATES] = {};
int total_visits = 0;
Ind probe;
probe.rs = H; probe.bs = H;
struct timespec t0, tn;
clock_gettime(CLOCK_MONOTONIC, &t0);
int round = 0;
while (1) {
int c = round % MCTS_N_CANDIDATES;
round++;
int ai = c / MCTS_N_THRUSTS;
int ti = c % MCTS_N_THRUSTS;
probe.ra[0] = MCTS_ANGLES[ai];
probe.rt[0] = MCTS_THRUSTS[ti];
for (int t = 1; t < H; t++) {
double v = rng.ri(-400, 400) / 10.0;
probe.ra[t] = v < -18 ? -18 : (v > 18 ? 18 : v);
int raw = rng.ri(-100, 500);
probe.rt[t] = BANG_BANG_THRUST ? (rng.ri(0,1)*200) : (raw < 0 ? 0 : (raw > 200 ? 200 : raw));
}
for (int t = 0; t < H; t++) {
double v = rng.ri(-400, 400) / 10.0;
probe.ba[t] = v < -18 ? -18 : (v > 18 ? 18 : v);
int raw = rng.ri(-100, 500);
probe.bt[t] = raw < 0 ? 0 : (raw > 200 ? 200 : raw);
}
double sc = evaluate(&probe, base, H, CX, CY, EX, EY, DTE, RRX, RRY,
ncp, laps, mx, rp, bp, orp, obp, rb, fboost, rtimeout,
dw, aw, sw, lw, apw, byw, opw, sfw, faw, rw,
opp_ra, opp_rt, opp_ba, opp_bt);
sum_score[c] += sc;
visit_count[c]++;
total_visits++;
if (c == MCTS_N_CANDIDATES - 1) {
clock_gettime(CLOCK_MONOTONIC, &tn);
long el = (tn.tv_sec - t0.tv_sec) * 1000000L + (tn.tv_nsec - t0.tv_nsec) / 1000L;
if (el >= budget_us) break;
}
}
int ranked[MCTS_N_CANDIDATES];
for (int i = 0; i < MCTS_N_CANDIDATES; i++) ranked[i] = i;
for (int i = 0; i < MCTS_TOP_K; i++) {
int best_j = i;
double best_avg = sum_score[ranked[i]] / visit_count[ranked[i]];
for (int j = i + 1; j < MCTS_N_CANDIDATES; j++) {
double avg = sum_score[ranked[j]] / visit_count[ranked[j]];
if (avg > best_avg) { best_avg = avg; best_j = j; }
}
if (best_j != i) { int tmp = ranked[i]; ranked[i] = ranked[best_j]; ranked[best_j] = tmp; }
}
int seeds_created = 0;
for (int k = 0; k < MCTS_TOP_K; k++) {
int c = ranked[k];
int ai = c / MCTS_N_THRUSTS;
int ti = c % MCTS_N_THRUSTS;
Ind& seed = out_seeds[k];
seed.ra[0] = MCTS_ANGLES[ai];
seed.rt[0] = MCTS_THRUSTS[ti];
for (int t = 1; t < H; t++) {
double v = rng.ri(-400, 400) / 10.0;
seed.ra[t] = v < -18 ? -18 : (v > 18 ? 18 : v);
int raw = rng.ri(-100, 500);
seed.rt[t] = BANG_BANG_THRUST ? (rng.ri(0,1)*200) : (raw < 0 ? 0 : (raw > 200 ? 200 : raw));
}
for (int t = 0; t < H; t++) {
double v = rng.ri(-400, 400) / 10.0;
seed.ba[t] = v < -18 ? -18 : (v > 18 ? 18 : v);
int raw = rng.ri(-100, 500);
seed.bt[t] = raw < 0 ? 0 : (raw > 200 ? 200 : raw);
}
seed.rs = H; seed.bs = H;
seeds_created++;
}
return seeds_created;
}
inline void run_combined_ga(
RNG& rng,
const Pod* base, int H, int PS, int mx,
const double* CX, const double* CY,
const double* EX, const double* EY,
const double* DTE, const double* RRX, const double* RRY,
int ncp, int laps,
int rp, int bp, int orp, int obp, int rb,
int fboost, int rtimeout,
const double* prev_ra, const int* prev_rt,
const double* prev_ba, const int* prev_bt,
int prev_rs, int prev_bs, int has_prev,
int budget_us,
double dw, double aw, double sw, double lw, double apw,
double byw, double opw, double sfw, double faw, double rw,
double* out_ra, int* out_rt, double* out_ba, int* out_bt,
int* out_rs, int* out_bs,
const double* opp_ra = nullptr, const int* opp_rt = nullptr,
const double* opp_ba = nullptr, const int* opp_bt = nullptr
) {
Ind pop[MAX_POP];
int idx = 0;
if (has_prev) {
for (int t = 0; t < H - 1; t++) {
pop[0].ra[t] = prev_ra[t + 1]; pop[0].rt[t] = prev_rt[t + 1];
pop[0].ba[t] = prev_ba[t + 1]; pop[0].bt[t] = prev_bt[t + 1];
}
pop[0].ra[H - 1] = rng.rd() * 36 - 18; pop[0].rt[H - 1] = rand_thrust(rng);
pop[0].ba[H - 1] = rng.rd() * 36 - 18; pop[0].bt[H - 1] = rand_thrust(rng);
pop[0].rs = (prev_rs > 0 && prev_rs < H) ? prev_rs - 1 : H;
pop[0].bs = (prev_bs > 0 && prev_bs < H) ? prev_bs - 1 : H;
idx = 1;
for (int pv = 0; pv < 4 && idx < PS; pv++) {
memcpy(&pop[idx], &pop[0], sizeof(Ind));
for (int t = 0; t < H; t++) {
double va = pop[idx].ra[t] + rng.rd() * 12 - 6;
pop[idx].ra[t] = va < -18 ? -18 : (va > 18 ? 18 : va);
int vt = pop[idx].rt[t] + rng.ri(-30, 30);
pop[idx].rt[t] = vt < 0 ? 0 : (vt > 200 ? 200 : vt);
double vba = pop[idx].ba[t] + rng.rd() * 12 - 6;
pop[idx].ba[t] = vba < -18 ? -18 : (vba > 18 ? 18 : vba);
int vbt = pop[idx].bt[t] + rng.ri(-30, 30);
pop[idx].bt[t] = vbt < 0 ? 0 : (vbt > 200 ? 200 : vbt);
}
pop[idx].rs = pop[0].rs; pop[idx].bs = pop[0].bs;
idx++;
}
}
{
Pod hsim[4]; memcpy(hsim, base, 4 * sizeof(Pod));
for (int t = 0; t < H; t++) {
double tx = EX[hsim[rp].next], ty = EY[hsim[rp].next];
double ddx = tx - hsim[rp].x, ddy = ty - hsim[rp].y;
double dd = sqrt(ddx * ddx + ddy * ddy);
if (dd > 0) {
double ux = ddx / dd, uy = ddy / dd;
double perp = hsim[rp].vx * (-uy) + hsim[rp].vy * ux;
double cf = fabs(perp) * 3.0; if (cf > 800) cf = 800;
if (perp > 0) { tx += uy * cf; ty -= ux * cf; }
else { tx -= uy * cf; ty += ux * cf; }
if (dd < 2500) {
int nnc = (hsim[rp].next + 1) % ncp;
double bl = (2500 - dd) / 2500;
tx = tx * (1 - bl) + EX[nnc] * bl; ty = ty * (1 - bl) + EY[nnc] * bl;
}
}
double ta = atan2(ty - hsim[rp].y, tx - hsim[rp].x);
double diff = ta - hsim[rp].angle;
while (diff > PI) diff -= 2 * PI;
while (diff < -PI) diff += 2 * PI;
double diff_deg = diff * RAD_TO_DEG;
if (diff_deg > 18) diff_deg = 18;
if (diff_deg < -18) diff_deg = -18;
if (idx < PS) { pop[idx].ra[t] = diff_deg; pop[idx].rt[t] = 200; }
if (idx + 1 < PS) { pop[idx + 1].ra[t] = diff_deg; pop[idx + 1].rt[t] = 150; }
double ang_rad2 = hsim[rp].angle + diff_deg * DEG_TO_RAD;
double r_tx2 = hsim[rp].x + cos(ang_rad2) * 10000.0;
double r_ty2 = hsim[rp].y + sin(ang_rad2) * 10000.0;
apply_action(&hsim[rp], r_tx2, r_ty2, 200, 0, 0);
double btx = RRX[rb], bty = RRY[rb];
double bd = sqrt((hsim[bp].x - hsim[orp].x) * (hsim[bp].x - hsim[orp].x) +
(hsim[bp].y - hsim[orp].y) * (hsim[bp].y - hsim[orp].y));
if (bd < 3000) { btx = hsim[orp].x + hsim[orp].vx * 2; bty = hsim[orp].y + hsim[orp].vy * 2; }
double bta = atan2(bty - hsim[bp].y, btx - hsim[bp].x);
double bdiff = bta - hsim[bp].angle;
while (bdiff > PI) bdiff -= 2 * PI;
while (bdiff < -PI) bdiff += 2 * PI;
double bdiff_deg = bdiff * RAD_TO_DEG;
if (bdiff_deg > 18) bdiff_deg = 18;
if (bdiff_deg < -18) bdiff_deg = -18;
if (idx < PS) { pop[idx].ba[t] = bdiff_deg; pop[idx].bt[t] = 200; }
if (idx + 1 < PS) { pop[idx + 1].ba[t] = bdiff_deg; pop[idx + 1].bt[t] = 200; }
double bang_rad2 = hsim[bp].angle + bdiff_deg * DEG_TO_RAD;
double b_tx2 = hsim[bp].x + cos(bang_rad2) * 10000.0;
double b_ty2 = hsim[bp].y + sin(bang_rad2) * 10000.0;
apply_action(&hsim[bp], b_tx2, b_ty2, 200, 0, 0);
apply_action(&hsim[orp], EX[hsim[orp].next], EY[hsim[orp].next], 200, 0, 0);
apply_action(&hsim[obp], hsim[rp].x, hsim[rp].y, 200, 0, 0);
nextTurn(hsim, CX, CY, ncp, laps);
}
if (idx < PS) { pop[idx].rs = H; pop[idx].bs = H; idx++; }
if (idx < PS) { pop[idx].rs = H; pop[idx].bs = H; idx++; }
}
if (idx + 1 < PS) {
Pod isim[4]; memcpy(isim, base, 4 * sizeof(Pod));
for (int t = 0; t < H; t++) {
double itx = EX[isim[rp].next], ity = EY[isim[rp].next];
double ita = atan2(ity - isim[rp].y, itx - isim[rp].x);
double idiff = ita - isim[rp].angle;
while (idiff > PI) idiff -= 2 * PI;
while (idiff < -PI) idiff += 2 * PI;
double idiff_deg = idiff * RAD_TO_DEG;
if (idiff_deg > 18) idiff_deg = 18;
if (idiff_deg < -18) idiff_deg = -18;
pop[idx].ra[t] = idiff_deg; pop[idx].rt[t] = 200;
double pred_x = isim[orp].x + isim[orp].vx * 3;
double pred_y = isim[orp].y + isim[orp].vy * 3;
double ibta = atan2(pred_y - isim[bp].y, pred_x - isim[bp].x);
double ibdiff = ibta - isim[bp].angle;
while (ibdiff > PI) ibdiff -= 2 * PI;
while (ibdiff < -PI) ibdiff += 2 * PI;
double ibdiff_deg = ibdiff * RAD_TO_DEG;
if (ibdiff_deg > 18) ibdiff_deg = 18;
if (ibdiff_deg < -18) ibdiff_deg = -18;
pop[idx].ba[t] = ibdiff_deg; pop[idx].bt[t] = 200;
apply_action(&isim[rp], isim[rp].x + cos(isim[rp].angle + idiff_deg * DEG_TO_RAD) * 10000.0,
isim[rp].y + sin(isim[rp].angle + idiff_deg * DEG_TO_RAD) * 10000.0, 200, 0, 0);
apply_action(&isim[bp], isim[bp].x + cos(isim[bp].angle + ibdiff_deg * DEG_TO_RAD) * 10000.0,
isim[bp].y + sin(isim[bp].angle + ibdiff_deg * DEG_TO_RAD) * 10000.0, 200, 0, 0);
apply_action(&isim[orp], EX[isim[orp].next], EY[isim[orp].next], 200, 0, 0);
apply_action(&isim[obp], isim[rp].x, isim[rp].y, 200, 0, 0);
nextTurn(isim, CX, CY, ncp, laps);
}
pop[idx].rs = H; pop[idx].bs = H; idx++;
}
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
if (idx + 4 <= PS) {
for (int t = 0; t < H; t++) {
pop[idx].ra[t] = 0; pop[idx].rt[t] = 200; pop[idx].ba[t] = 0; pop[idx].bt[t] = 200;
pop[idx + 1].ra[t] = 0; pop[idx + 1].rt[t] = 0; pop[idx + 1].ba[t] = 0; pop[idx + 1].bt[t] = 0;
pop[idx + 2].ra[t] = -18; pop[idx + 2].rt[t] = 200; pop[idx + 2].ba[t] = -18; pop[idx + 2].bt[t] = 200;
pop[idx + 3].ra[t] = 18; pop[idx + 3].rt[t] = 200; pop[idx + 3].ba[t] = 18; pop[idx + 3].bt[t] = 200;
}
pop[idx].rs = H; pop[idx].bs = H;
pop[idx + 1].rs = H; pop[idx + 1].bs = H;
pop[idx + 2].rs = H; pop[idx + 2].bs = H;
pop[idx + 3].rs = H; pop[idx + 3].bs = H;
idx += 4;
}
struct timespec mcts_t0, mcts_t1;
clock_gettime(CLOCK_MONOTONIC, &mcts_t0);
{
Ind mcts_seeds[MCTS_TOP_K];
int n_mcts = mcts_first_move_probe(
rng, base, H, CX, CY, EX, EY, DTE, RRX, RRY,
ncp, laps, mx, rp, bp, orp, obp, rb,
fboost, rtimeout,
dw, aw, sw, lw, apw, byw, opw, sfw, faw, rw,
opp_ra, opp_rt, opp_ba, opp_bt,
MCTS_BUDGET_US, mcts_seeds);
for (int k = 0; k < n_mcts && idx < PS; k++) {
pop[idx] = mcts_seeds[k];
idx++;
}
}
clock_gettime(CLOCK_MONOTONIC, &mcts_t1);
long mcts_elapsed_us = (mcts_t1.tv_sec - mcts_t0.tv_sec) * 1000000L +
(mcts_t1.tv_nsec - mcts_t0.tv_nsec) / 1000L;
int ga_budget_us = budget_us - (int)mcts_elapsed_us;
if (ga_budget_us < MIN_PHASE2_BUDGET_US) ga_budget_us = MIN_PHASE2_BUDGET_US;
for (int i = idx; i < PS; i++) {
for (int t = 0; t < H; t++) {
double v = rng.ri(-400, 400) / 10.0;
pop[i].ra[t] = v < -18 ? -18 : (v > 18 ? 18 : v);
pop[i].rt[t] = rand_thrust(rng);
v = rng.ri(-400, 400) / 10.0;
pop[i].ba[t] = v < -18 ? -18 : (v > 18 ? 18 : v);
pop[i].bt[t] = rand_thrust(rng);
}
pop[i].rs = rng.ri(0, H + 4); pop[i].bs = rng.ri(0, H + 4);
}
int best = 0, worst = 0;
for (int i = 0; i < PS; i++) {
pop[i].sc = evaluate(&pop[i], base, H, CX, CY, EX, EY, DTE, RRX, RRY,
ncp, laps, mx, rp, bp, orp, obp, rb, fboost, rtimeout,
dw, aw, sw, lw, apw, byw, opw, sfw, faw, rw,
opp_ra, opp_rt, opp_ba, opp_bt);
if (pop[i].sc > pop[best].sc) best = i;
if (pop[i].sc < pop[worst].sc) worst = i;
}
double wsc = pop[worst].sc;
struct timespec t0, tn; clock_gettime(CLOCK_MONOTONIC, &t0);
int iters = 0; double amplitude = 1.0;
Ind child;
while (1) {
iters++;
if ((iters & 255) == 0) {
clock_gettime(CLOCK_MONOTONIC, &tn);
long el = (tn.tv_sec - t0.tv_sec) * 1000000L + (tn.tv_nsec - t0.tv_nsec) / 1000L;
if (el >= ga_budget_us) break;
amplitude = 1.0 - (double)el / (double)ga_budget_us;
if (amplitude < 0) amplitude = 0;
}
if (pop[best].sc < wsc + 0.3) {
for (int i = 0; i < PS; i++) if (i != best) pop[i].sc -= 2000;
wsc -= 2000;
}
int p1 = rng.ri(0, PS - 1), p2 = rng.ri(0, PS - 1);
int par = (pop[p1].sc >= pop[p2].sc) ? p1 : p2;
double threshold = 0.25 + amplitude;
if (rng.ri(0, 4) == 0) {
p1 = rng.ri(0, PS - 1); p2 = rng.ri(0, PS - 1);
int par2 = (pop[p1].sc >= pop[p2].sc) ? p1 : p2;
for (int t = 0; t < H; t++) {
if (rng.rd() < 0.5) { child.ra[t] = pop[par].ra[t]; child.rt[t] = pop[par].rt[t]; }
else { child.ra[t] = pop[par2].ra[t]; child.rt[t] = pop[par2].rt[t]; }
if (rng.rd() < 0.5) { child.ba[t] = pop[par].ba[t]; child.bt[t] = pop[par].bt[t]; }
else { child.ba[t] = pop[par2].ba[t]; child.bt[t] = pop[par2].bt[t]; }
}
child.rs = (rng.rd() < 0.5) ? pop[par].rs : pop[par2].rs;
child.bs = (rng.rd() < 0.5) ? pop[par].bs : pop[par2].bs;
} else {
for (int t = 0; t < H; t++) {
child.ra[t] = pop[par].ra[t]; child.rt[t] = pop[par].rt[t];
child.ba[t] = pop[par].ba[t]; child.bt[t] = pop[par].bt[t];
if (rng.rd() < threshold) { double v = rng.ri(-400, 400) / 10.0; child.ra[t] = v < -18 ? -18 : (v > 18 ? 18 : v); }
if (rng.rd() < threshold) { child.rt[t] = rand_thrust(rng); }
if (rng.rd() < threshold) { double v = rng.ri(-400, 400) / 10.0; child.ba[t] = v < -18 ? -18 : (v > 18 ? 18 : v); }
if (rng.rd() < threshold) { child.bt[t] = rand_thrust(rng); }
}
child.rs = pop[par].rs; child.bs = pop[par].bs;
if (rng.rd() < threshold) child.rs = rng.ri(0, H + 4);
if (rng.rd() < threshold) child.bs = rng.ri(0, H + 4);
}
{ int si = rng.ri(0, H - 1); double v = child.ra[si] + rng.rd() * 24 - 12; child.ra[si] = v < -18 ? -18 : (v > 18 ? 18 : v); }
{ int si = rng.ri(0, H - 1); child.rt[si] = mutate_thrust(rng, child.rt[si]); }
{ int si = rng.ri(0, H - 1); double v = child.ba[si] + rng.rd() * 24 - 12; child.ba[si] = v < -18 ? -18 : (v > 18 ? 18 : v); }
{ int si = rng.ri(0, H - 1); child.bt[si] = mutate_thrust(rng, child.bt[si]); }
child.sc = evaluate(&child, base, H, CX, CY, EX, EY, DTE, RRX, RRY,
ncp, laps, mx, rp, bp, orp, obp, rb, fboost, rtimeout,
dw, aw, sw, lw, apw, byw, opw, sfw, faw, rw,
opp_ra, opp_rt, opp_ba, opp_bt);
if (child.sc > pop[best].sc) best = worst;
pop[worst] = child;
if (child.sc > wsc) {
worst = 0; wsc = pop[0].sc;
for (int i = 1; i < PS; i++) if (pop[i].sc < wsc) { worst = i; wsc = pop[i].sc; }
}
}
memcpy(out_ra, pop[best].ra, H * sizeof(double));
memcpy(out_rt, pop[best].rt, H * sizeof(int));
memcpy(out_ba, pop[best].ba, H * sizeof(double));
memcpy(out_bt, pop[best].bt, H * sizeof(int));
*out_rs = pop[best].rs; *out_bs = pop[best].bs;
}
} 
class UltimateBot : public Bot {
public:
void init(int laps,
const std::vector<std::pair<double, double>>& checkpoints) override {
laps_ = laps + 1; 
ncp_ = static_cast<int>(checkpoints.size());
CX_.resize(ncp_); CY_.resize(ncp_);
for (int i = 0; i < ncp_; i++) {
CX_[i] = checkpoints[i].first;
CY_[i] = checkpoints[i].second;
}
compute_entry_points();
compute_dte();
compute_ram_rest_points();
compute_weights();
for (int i = 0; i < ncp_; i++) {
int n = (i + 1) % ncp_;
heading_to_next_[i] = atan2(CY_[n] - CY_[i], CX_[n] - CX_[i]);
}
turn_ = 0;
rng_.seed(42);
for (int i = 0; i < 2; i++) {
ml_[i] = 0; ol_[i] = 0;
pmc_[i] = -1; poc_[i] = -1;
msc_[i] = 0; mbu_[i] = false;
}
has_prev_ = false;
has_opp_prev_ = false;
prev_rs_ = H_; prev_bs_ = H_;
nn_angle_bias_[0] = nn_angle_bias_[1] = 0;
nn_thrust_bias_[0] = nn_thrust_bias_[1] = 0;
nn_last_pred_angle_[0] = nn_last_pred_angle_[1] = 0;
nn_last_pred_thrust_[0] = nn_last_pred_thrust_[1] = 0;
nn_obs_count_ = 0;
runner_zero_streak_ = 0;
}
std::pair<BotAction, BotAction> get_actions(
const std::array<std::array<double, 6>, 4>& pods) override {
turn_++;
using namespace ultimate_ga;
double px[4], py[4], pvx[4], pvy[4], pang[4];
int pnc[4], plp[4], psd[4];
for (int i = 0; i < 2; i++) {
px[i] = pods[i][0]; py[i] = pods[i][1];
pvx[i] = pods[i][2]; pvy[i] = pods[i][3];
pang[i] = pods[i][4]; pnc[i] = static_cast<int>(pods[i][5]);
if (pang[i] < 0) {
pang[i] = atan2(CY_[pnc[i]] - py[i], CX_[pnc[i]] - px[i]) * RAD_TO_DEG;
if (pang[i] < 0) pang[i] += 360;
}
if (pmc_[i] == -1) pmc_[i] = pnc[i];
else if (pnc[i] == 0 && pmc_[i] == ncp_ - 1) { ml_[i]++; runner_stuck_turns_[i] = 0; }
else if (pnc[i] != pmc_[i]) { runner_stuck_turns_[i] = 0; }
else { runner_stuck_turns_[i]++; }
pmc_[i] = pnc[i]; plp[i] = ml_[i]; psd[i] = msc_[i];
if (msc_[i] > 0) msc_[i]--;
}
for (int i = 0; i < 2; i++) {
px[i + 2] = pods[i + 2][0]; py[i + 2] = pods[i + 2][1];
pvx[i + 2] = pods[i + 2][2]; pvy[i + 2] = pods[i + 2][3];
pang[i + 2] = pods[i + 2][4]; pnc[i + 2] = static_cast<int>(pods[i + 2][5]);
if (pang[i + 2] < 0) {
pang[i + 2] = atan2(CY_[pnc[i + 2]] - py[i + 2], CX_[pnc[i + 2]] - px[i + 2]) * RAD_TO_DEG;
if (pang[i + 2] < 0) pang[i + 2] += 360;
}
if (poc_[i] == -1) poc_[i] = pnc[i + 2];
else if (pnc[i + 2] == 0 && poc_[i] == ncp_ - 1) ol_[i]++;
poc_[i] = pnc[i + 2]; plp[i + 2] = ol_[i]; psd[i + 2] = 0;
}
if (has_opp_obs_) {
for (int i = 0; i < 2; i++) {
double ang_now = pang[i + 2] * DEG_TO_RAD;
double ang_prev = prev_opp_ang_[i] * DEG_TO_RAD;
double delta = ang_now - ang_prev;
while (delta > PI) delta -= 2 * PI;
while (delta < -PI) delta += 2 * PI;
obs_opp_delta_[i] = delta * RAD_TO_DEG;
double ca = cos(ang_now), sa = sin(ang_now);
double dvx = pvx[i + 2] - prev_opp_vx_[i] * 0.85;
double dvy = pvy[i + 2] - prev_opp_vy_[i] * 0.85;
double thrust_est = 0;
if (ca * ca + sa * sa > 0.1) {
thrust_est = (dvx * ca + dvy * sa);
}
obs_opp_thrust_[i] = std::max(0, std::min(200, (int)(thrust_est + 0.5)));
}
}
for (int i = 0; i < 2; i++) {
prev_opp_ang_[i] = pang[i + 2];
prev_opp_x_[i] = px[i + 2]; prev_opp_y_[i] = py[i + 2];
prev_opp_vx_[i] = pvx[i + 2]; prev_opp_vy_[i] = pvy[i + 2];
}
has_opp_obs_ = true;
if (has_opp_obs_ && nn_obs_count_ > 0) {
float alpha = std::min(0.5f, 2.0f / (nn_obs_count_ + 1));
for (int i = 0; i < 2; i++) {
float angle_err = nn_last_pred_angle_[i] - (float)obs_opp_delta_[i];
float thrust_err = nn_last_pred_thrust_[i] - (float)obs_opp_thrust_[i];
nn_angle_bias_[i] = (1 - alpha) * nn_angle_bias_[i] + alpha * angle_err;
nn_thrust_bias_[i] = (1 - alpha) * nn_thrust_bias_[i] + alpha * thrust_err;
}
}
if (has_opp_obs_) nn_obs_count_++;
int budget_us = (turn_ == 1) ? ultimate_ga::FIRST_TURN_BUDGET_US : ultimate_ga::TURN_BUDGET_US;
auto rprog = [&](int idx) -> double {
int prog = plp[idx] * ncp_ + pnc[idx];
int clamped = prog < mx_ ? prog : mx_ - 1;
double dx = px[idx] - EX_[pnc[idx]], dy = py[idx] - EY_[pnc[idx]];
return DTE_[clamped] + sqrt(dx * dx + dy * dy);
};
int rp = (rprog(1) < rprog(0) - 200) ? 1 : 0;
int bp = 1 - rp;
int orp_abs = (rprog(2) < rprog(3)) ? 2 : 3;
int obp_abs = 5 - orp_abs;
{
double d2_to_our_cp = hypot(px[2] - CX_[pnc[rp]], py[2] - CY_[pnc[rp]]);
double d3_to_our_cp = hypot(px[3] - CX_[pnc[rp]], py[3] - CY_[pnc[rp]]);
double camp_r = 2000.0;
bool p2_camp = d2_to_our_cp < camp_r;
bool p3_camp = d3_to_our_cp < camp_r;
if (p2_camp && !p3_camp && orp_abs != 2) { orp_abs = 2; obp_abs = 3; }
else if (p3_camp && !p2_camp && orp_abs != 3) { orp_abs = 3; obp_abs = 2; }
}
int rb = pnc[orp_abs];
{
double od = hypot(px[orp_abs] - CX_[rb], py[orp_abs] - CY_[rb]);
double md = hypot(px[bp] - CX_[rb], py[bp] - CY_[rb]);
if (md > od + ultimate_ga::BLOCKER_DIST_THRESHOLD) {
int nb = (rb + 1) % ncp_;
double od2 = od + hypot(CX_[rb] - CX_[nb], CY_[rb] - CY_[nb]);
double md2 = hypot(px[bp] - CX_[nb], py[bp] - CY_[nb]);
if (md2 < od2 - ultimate_ga::BLOCKER_DIST_THRESHOLD) rb = nb;
else rb = (nb + 1) % ncp_;
}
}
int fboost = 0;
if (!mbu_[rp] && psd[rp] == 0) {
double dd = hypot(px[rp] - EX_[pnc[rp]], py[rp] - EY_[pnc[rp]]);
double ta = atan2(EY_[pnc[rp]] - py[rp], EX_[pnc[rp]] - px[rp]) * RAD_TO_DEG;
if (ta < 0) ta += 360;
double ae = ta - pang[rp];
while (ae > 180) ae -= 360;
while (ae < -180) ae += 360;
if (dd > 5000 && fabs(ae) < 5) fboost = 1;
}
int rtimeout = 0;
if (runner_stuck_turns_[rp] >= 30) rtimeout = 1; 
if (plp[rp] == laps_ - 1 && runner_stuck_turns_[rp] >= 20) rtimeout = 2;
Pod base[4];
for (int i = 0; i < 4; i++) {
base[i].x = px[i]; base[i].y = py[i];
base[i].vx = pvx[i]; base[i].vy = pvy[i];
base[i].angle = pang[i] * DEG_TO_RAD;
base[i].next = pnc[i]; base[i].lap = plp[i];
base[i].shieldtimer = psd[i];
base[i].boosted = (i < 2 && mbu_[i]) ? 1 : 0;
base[i].isFirstTurn = (turn_ == 1) ? 1 : 0;
}
double out_ra[MAX_H], out_ba[MAX_H];
int out_rt[MAX_H], out_bt[MAX_H];
int out_rs, out_bs;
double pred_ora[MAX_H]={}, pred_oba[MAX_H]={};
int pred_ort[MAX_H]={}, pred_obt[MAX_H]={};
bool use_ibr = has_prev_ && turn_ > 1;
int phase2_budget = budget_us;
if (turn_ > 1) {
int pod_order[4] = {rp, bp, orp_abs, obp_abs};
float nn_input[38];
int fi = 0;
for (int pi = 0; pi < 4; pi++) {
int p = pod_order[pi];
int nc = pnc[p];
double dx_cp = CX_[nc] - px[p];
double dy_cp = CY_[nc] - py[p];
double dist_cp = hypot(dx_cp, dy_cp);
double cp_angle = atan2(dy_cp, dx_cp);
double pod_angle_rad = pang[p] * DEG_TO_RAD;
double angle_to_cp = cp_angle - pod_angle_rad;
while (angle_to_cp > PI) angle_to_cp -= 2*PI;
while (angle_to_cp < -PI) angle_to_cp += 2*PI;
double speed = hypot(pvx[p], pvy[p]);
nn_input[fi++] = (float)dx_cp;
nn_input[fi++] = (float)dy_cp;
nn_input[fi++] = (float)pvx[p];
nn_input[fi++] = (float)pvy[p];
nn_input[fi++] = (float)speed;
nn_input[fi++] = (float)dist_cp;
nn_input[fi++] = (float)angle_to_cp;
}
if (has_opp_obs_) {
nn_input[fi++] = (float)obs_opp_delta_[orp_abs - 2];
nn_input[fi++] = (float)obs_opp_thrust_[orp_abs - 2];
nn_input[fi++] = (float)obs_opp_delta_[obp_abs - 2];
nn_input[fi++] = (float)obs_opp_thrust_[obp_abs - 2];
} else {
nn_input[fi++] = 0.0f; nn_input[fi++] = 100.0f;
nn_input[fi++] = 0.0f; nn_input[fi++] = 100.0f;
}
for (int oi = 0; oi < 2; oi++) {
int op = (oi == 0) ? orp_abs : obp_abs;
int nc = pnc[op];
int prev_cp = (nc - 1 + ncp_) % ncp_;
int next_cp = (nc + 1) % ncp_;
double v1x = CX_[nc] - CX_[prev_cp], v1y = CY_[nc] - CY_[prev_cp];
double v2x = CX_[next_cp] - CX_[nc], v2y = CY_[next_cp] - CY_[nc];
double l1 = hypot(v1x, v1y), l2 = hypot(v2x, v2y);
float cos_theta = 1.0f;
if (l1 > 0 && l2 > 0)
cos_theta = (float)std::max(-1.0, std::min(1.0, (v1x*v2x + v1y*v2y)/(l1*l2)));
nn_input[fi++] = cos_theta;
}
for (int oi = 0; oi < 2; oi++) {
int op = (oi == 0) ? orp_abs : obp_abs;
double d0 = hypot(px[op]-px[0], py[op]-py[0]);
double d1 = hypot(px[op]-px[1], py[op]-py[1]);
nn_input[fi++] = (float)std::min(d0, d1);
}
int our_prog = std::max(plp[rp]*ncp_+pnc[rp], plp[bp]*ncp_+pnc[bp]);
int opp_prog = std::max(plp[orp_abs]*ncp_+pnc[orp_abs], plp[obp_abs]*ncp_+pnc[obp_abs]);
nn_input[fi++] = (float)(our_prog - opp_prog);
nn_input[fi++] = (float)turn_;
float nn_output[8];
opp_nn::predict(nn_input, nn_output);
int nn_thrust_a = opp_nn::logits_to_thrust(nn_output[1], nn_output[2], nn_output[3]);
int nn_thrust_b = opp_nn::logits_to_thrust(nn_output[5], nn_output[6], nn_output[7]);
nn_last_pred_angle_[orp_abs - 2] = nn_output[0] * (float)RAD_TO_DEG;
nn_last_pred_thrust_[orp_abs - 2] = (float)nn_thrust_a;
nn_last_pred_angle_[obp_abs - 2] = nn_output[4] * (float)RAD_TO_DEG;
nn_last_pred_thrust_[obp_abs - 2] = (float)nn_thrust_b;
float corr_ora_rad = nn_output[0] - nn_angle_bias_[orp_abs - 2] * (float)DEG_TO_RAD;
float corr_oba_rad = nn_output[4] - nn_angle_bias_[obp_abs - 2] * (float)DEG_TO_RAD;
float corr_ort = (float)nn_thrust_a - nn_thrust_bias_[orp_abs - 2];
float corr_obt = (float)nn_thrust_b - nn_thrust_bias_[obp_abs - 2];
float max_turn_rad = (float)MAX_TURN_RAD;
corr_ora_rad = std::max(-max_turn_rad, std::min(max_turn_rad, corr_ora_rad));
corr_ort = std::max(0.0f, std::min(200.0f, corr_ort));
corr_oba_rad = std::max(-max_turn_rad, std::min(max_turn_rad, corr_oba_rad));
corr_obt = std::max(0.0f, std::min(200.0f, corr_obt));
double nn_ora = std::max(-18.0, std::min(18.0, (double)corr_ora_rad * RAD_TO_DEG));
int nn_ort = std::max(0, std::min(200, (int)corr_ort));
double nn_oba = std::max(-18.0, std::min(18.0, (double)corr_oba_rad * RAD_TO_DEG));
int nn_obt = std::max(0, std::min(200, (int)corr_obt));
for (int t = 0; t < H_; t++) {
pred_ora[t] = nn_ora * DEG_TO_RAD;
pred_ort[t] = nn_ort;
pred_oba[t] = nn_oba * DEG_TO_RAD;
pred_obt[t] = nn_obt;
}
}
if (use_ibr) {
int p1_budget = ultimate_ga::IBR_BUDGET_US;
double shifted_ra[ultimate_ga::MAX_H], shifted_ba[ultimate_ga::MAX_H];
int shifted_rt[ultimate_ga::MAX_H], shifted_bt[ultimate_ga::MAX_H];
for (int t = 0; t < H_ - 1; t++) {
shifted_ra[t] = prev_ra_[t+1]; shifted_rt[t] = prev_rt_[t+1];
shifted_ba[t] = prev_ba_[t+1]; shifted_bt[t] = prev_bt_[t+1];
}
shifted_ra[H_-1] = 0; shifted_rt[H_-1] = 200;
shifted_ba[H_-1] = 0; shifted_bt[H_-1] = 200;
run_opp_prediction_ga(
rng_, base, H_, mx_,
CX_.data(), CY_.data(), EX_.data(), EY_.data(),
DTE_.data(), ncp_, laps_,
rp, bp, orp_abs, obp_abs,
shifted_ra, shifted_rt, shifted_ba, shifted_bt,
opp_prev_ra_, opp_prev_rt_, opp_prev_ba_, opp_prev_bt_,
has_opp_prev_ ? 1 : 0,
p1_budget, dw_, aw_, sw_,
has_opp_obs_ ? 1 : 0,
obs_opp_delta_[orp_abs - 2], obs_opp_thrust_[orp_abs - 2],
obs_opp_delta_[obp_abs - 2], obs_opp_thrust_[obp_abs - 2],
pred_ora, pred_ort, pred_oba, pred_obt);
memcpy(opp_prev_ra_, pred_ora, H_ * sizeof(double));
memcpy(opp_prev_rt_, pred_ort, H_ * sizeof(int));
memcpy(opp_prev_ba_, pred_oba, H_ * sizeof(double));
memcpy(opp_prev_bt_, pred_obt, H_ * sizeof(int));
has_opp_prev_ = true;
phase2_budget = budget_us - p1_budget;
if (phase2_budget < ultimate_ga::MIN_PHASE2_BUDGET_US) phase2_budget = ultimate_ga::MIN_PHASE2_BUDGET_US;
}
run_combined_ga(
rng_, base, H_, PS_, mx_,
CX_.data(), CY_.data(), EX_.data(), EY_.data(),
DTE_.data(), RRX_.data(), RRY_.data(),
ncp_, laps_,
rp, bp, orp_abs, obp_abs, rb,
fboost, rtimeout,
prev_ra_, prev_rt_, prev_ba_, prev_bt_,
prev_rs_, prev_bs_, has_prev_ ? 1 : 0,
phase2_budget,
dw_, aw_, sw_, lw_, apw_, byw_, opw_, sfw_, faw_, rw_,
out_ra, out_rt, out_ba, out_bt,
&out_rs, &out_bs,
use_ibr ? pred_ora : nullptr,
use_ibr ? pred_ort : nullptr,
use_ibr ? pred_oba : nullptr,
use_ibr ? pred_obt : nullptr);
memcpy(prev_ra_, out_ra, H_ * sizeof(double));
memcpy(prev_rt_, out_rt, H_ * sizeof(int));
memcpy(prev_ba_, out_ba, H_ * sizeof(double));
memcpy(prev_bt_, out_bt, H_ * sizeof(int));
prev_rs_ = out_rs; prev_bs_ = out_bs;
has_prev_ = true;
double r_da = std::max(-18.0, std::min(18.0, out_ra[0]));
double r_ang_deg = fmod(pang[rp] + r_da, 360.0);
if (r_ang_deg < 0) r_ang_deg += 360;
double r_rad = r_ang_deg * DEG_TO_RAD;
double rtx = px[rp] + cos(r_rad) * 10000;
double rty = py[rp] + sin(r_rad) * 10000;
bool r_shield = out_rs == 0 && psd[rp] == 0;
bool r_boost = fboost && !mbu_[rp] && !r_shield;
BotAction runner_act;
runner_act.target_x = lround(rtx);
runner_act.target_y = lround(rty);
if (r_shield) {
runner_act.thrust = THRUST_SHIELD; msc_[rp] = 3;
} else if (r_boost) {
runner_act.thrust = THRUST_BOOST; mbu_[rp] = true;
} else {
runner_act.thrust = std::max(0, std::min(200, out_rt[0]));
}
double b_da = std::max(-18.0, std::min(18.0, out_ba[0]));
double b_ang_deg = fmod(pang[bp] + b_da, 360.0);
if (b_ang_deg < 0) b_ang_deg += 360;
double b_rad = b_ang_deg * DEG_TO_RAD;
double btx = px[bp] + cos(b_rad) * 10000;
double bty = py[bp] + sin(b_rad) * 10000;
bool b_shield = out_bs == 0 && psd[bp] == 0;
bool b_boost_flag = false;
if (!mbu_[bp] && !b_shield && psd[bp] == 0) {
double dd = hypot(px[bp] - px[orp_abs], py[bp] - py[orp_abs]);
if (dd > 5000) {
double ba_t = atan2(py[orp_abs] - py[bp], px[orp_abs] - px[bp]) * RAD_TO_DEG;
if (ba_t < 0) ba_t += 360;
double ae_b = ba_t - pang[bp];
while (ae_b > 180) ae_b -= 360;
while (ae_b < -180) ae_b += 360;
if (fabs(ae_b) < 10) b_boost_flag = true;
}
}
BotAction blocker_act;
blocker_act.target_x = lround(btx);
blocker_act.target_y = lround(bty);
if (b_shield) {
blocker_act.thrust = THRUST_SHIELD; msc_[bp] = 3;
} else if (b_boost_flag) {
blocker_act.thrust = THRUST_BOOST; mbu_[bp] = true;
} else {
blocker_act.thrust = std::max(0, std::min(200, out_bt[0]));
}
if (rp == 0) return {runner_act, blocker_act};
else return {blocker_act, runner_act};
}
std::unique_ptr<Bot> clone() const override {
return std::make_unique<UltimateBot>();
}
private:
static constexpr int H_ = ultimate_ga::GA_HORIZON;
static constexpr int PS_ = ultimate_ga::GA_POP_SIZE;
int laps_ = 0;
int ncp_ = 0;
int mx_ = 0;
int turn_ = 0;
ultimate_ga::RNG rng_;
std::vector<double> CX_, CY_, EX_, EY_, DTE_, RRX_, RRY_;
double heading_to_next_[20] = {}; 
int ml_[2] = {}, ol_[2] = {};
int pmc_[2] = {}, poc_[2] = {};
int msc_[2] = {};
bool mbu_[2] = {};
int runner_stuck_turns_[2] = {}; 
bool has_prev_ = false;
double prev_ra_[ultimate_ga::MAX_H] = {};
int prev_rt_[ultimate_ga::MAX_H] = {};
double prev_ba_[ultimate_ga::MAX_H] = {};
int prev_bt_[ultimate_ga::MAX_H] = {};
int prev_rs_ = 6, prev_bs_ = 6;
bool has_opp_prev_ = false;
double opp_prev_ra_[ultimate_ga::MAX_H] = {};
int opp_prev_rt_[ultimate_ga::MAX_H] = {};
double opp_prev_ba_[ultimate_ga::MAX_H] = {};
int opp_prev_bt_[ultimate_ga::MAX_H] = {};
bool has_opp_obs_ = false;
int runner_zero_streak_ = 0; 
double prev_opp_ang_[2] = {}; 
double prev_opp_x_[2] = {}, prev_opp_y_[2] = {}; 
double prev_opp_vx_[2] = {}, prev_opp_vy_[2] = {}; 
double obs_opp_delta_[2] = {}; 
int obs_opp_thrust_[2] = {}; 
float nn_angle_bias_[2] = {0, 0}; 
float nn_thrust_bias_[2] = {0, 0}; 
float nn_last_pred_angle_[2] = {0, 0}; 
float nn_last_pred_thrust_[2] = {0, 0};
int nn_obs_count_ = 0; 
double dw_, aw_, sw_, lw_, apw_, byw_, opw_, sfw_, faw_, rw_;
void compute_entry_points() {
EX_ = CX_; EY_ = CY_;
bool has_sharp = false;
for (int i = 0; i < ncp_; i++) {
int p = (i - 1 + ncp_) % ncp_, n = (i + 1) % ncp_;
double v1x = CX_[i] - CX_[p], v1y = CY_[i] - CY_[p];
double v2x = CX_[n] - CX_[i], v2y = CY_[n] - CY_[i];
double l1 = hypot(v1x, v1y), l2 = hypot(v2x, v2y);
double cos_theta = 1.0;
if (l1 > 0 && l2 > 0) {
cos_theta = std::max(-1.0, std::min(1.0, (v1x * v2x + v1y * v2y) / (l1 * l2)));
}
if (cos_theta < 0.2) has_sharp = true;
has_sharp_ = has_sharp;
double corner_cut = (ncp_ >= 5 || cos_theta < 0.2) ? 600.0 : 300.0;
double shift_dist = corner_cut * (1.0 - cos_theta) / 2.0;
double dx = CX_[p] - CX_[n], dy = CY_[p] - CY_[n];
double dd = hypot(dx, dy);
if (dd > 0) {
EX_[i] = CX_[i] + shift_dist * dx / dd;
EY_[i] = CY_[i] + shift_dist * dy / dd;
}
}
}
void compute_dte() {
mx_ = laps_ * ncp_ + 1;
DTE_.assign(mx_, 0.0);
for (int i = mx_ - 2; i >= 0; i--) {
int c = i % ncp_, nc2 = (c + 1) % ncp_;
DTE_[i] = DTE_[i + 1] + hypot(CX_[c] - CX_[nc2], CY_[c] - CY_[nc2]);
}
}
void compute_ram_rest_points() {
RRX_.resize(ncp_); RRY_.resize(ncp_);
for (int i = 0; i < ncp_; i++) {
int p = (i - 1 + ncp_) % ncp_, n = (i + 1) % ncp_;
double dx = CX_[p] + CX_[n] - 2 * CX_[i];
double dy = CY_[p] + CY_[n] - 2 * CY_[i];
double d = hypot(dx, dy);
if (d > 0) {
RRX_[i] = CX_[i] + 1000 * dx / d;
RRY_[i] = CY_[i] + 1000 * dy / d;
} else {
RRX_[i] = CX_[i]; RRY_[i] = CY_[i];
}
}
}
bool has_sharp_ = false;
void compute_weights() {
double total_dist = 0;
for (int i = 0; i < ncp_; i++) {
int n = (i + 1) % ncp_;
total_dist += hypot(CX_[i] - CX_[n], CY_[i] - CY_[n]);
}
double avg_dist = total_dist / ncp_;
bool use_handling = (ncp_ >= ultimate_ga::HANDLING_MIN_CPS || has_sharp_) && avg_dist <= ultimate_ga::HANDLING_AVG_DIST;
if (use_handling) {
dw_ = 2.9; aw_ = 3.7; sw_ = 0.2; lw_ = 1.0; apw_ = 60.0;
byw_ = 25.0; opw_ = 1.3; sfw_ = 50.0; faw_ = 25.0; rw_ = 0.15;
} else {
dw_ = 2.3; aw_ = 1.7; sw_ = 0.6; lw_ = 1.2; apw_ = 43.0;
byw_ = 15.0; opw_ = 1.2; sfw_ = 40.0; faw_ = 20.0; rw_ = 0.15;
}
}
};
}


int main(){
std::ios_base::sync_with_stdio(false);
std::cin.tie(nullptr);
int laps;std::cin>>laps;std::cin.ignore();
int nc;std::cin>>nc;std::cin.ignore();
std::vector<std::pair<double,double>>cps(nc);
for(int i=0;i<nc;i++){int x,y;std::cin>>x>>y;std::cin.ignore();cps[i]={(double)x,(double)y};}
arena::UltimateBot bot;bot.init(laps,cps);
while(true){
std::array<std::array<double,6>,4>pods;
for(int i=0;i<2;i++){int x,y,vx,vy,a,n;std::cin>>x>>y>>vx>>vy>>a>>n;std::cin.ignore();
pods[i]={(double)x,(double)y,(double)vx,(double)vy,(double)a,(double)n};}
for(int i=0;i<2;i++){int x,y,vx,vy,a,n;std::cin>>x>>y>>vx>>vy>>a>>n;std::cin.ignore();
pods[i+2]={(double)x,(double)y,(double)vx,(double)vy,(double)a,(double)n};}
auto[a0,a1]=bot.get_actions(pods);
std::cout<<(int)a0.target_x<<" "<<(int)a0.target_y<<" "<<a0.thrust_str()<<std::endl;
std::cout<<(int)a1.target_x<<" "<<(int)a1.target_y<<" "<<a1.thrust_str()<<std::endl;
}return 0;}

