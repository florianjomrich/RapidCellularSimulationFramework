set size 1,1
set view map;
set xlabel "X"
set ylabel "Y"
set cblabel "SINR (dB)"
unset key
plot "carrem.out" using ($1):($2):(10*log10($4)) with image
pause -1
