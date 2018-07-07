
./main -g 01.v   -f foo  t1.ssa
./main -g 02.v   -f foo -I 200,300  t2.ssa
./main -g 03.v   -f foo -I 0,10 -I 20,50 t3.ssa
./main -g 04.v   -f foo -I -infinity,infinity  t4.ssa
./main -g 05.v   -f foo  t5.ssa
./main -g 06.v   -f foo -I -infinity,infinity  t6.ssa
./main -g 07.v   -f foo -I -10,10  t7.ssa
./main -g 08.v   -f foo -I 1,100 -I -2,2  t8.ssa
./main -g 09.v   -f foo  t9.ssa
./main -g 10.v   -f foo -I 30,50 -I 90,100  t10.ssa
pwd
