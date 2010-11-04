#!/bin/bash

#if [ $# -lt 5 ]; then
#  echo "Cantidad invalida de parametros";
#  exit -1
#fi

cnf_filename=$1
output_filename=$2
rm -rf ${output_filename}
seed=$3	

shift 3

#echo $@

#repeticiones=1
#tiempo_max=0
#iteraciones=200
#tripw=0
#repw=0
#sini=1
#fact_mut=0
#res=1
#ord=0
#rl=0
#rep_size=10
#tasa_reem=0.2
#fp_trip=0.5
#fp_rep=0.5
ITERS=10
size=5

FACT_SOC=0
FACT_COG=0
INERTIA=0

#echo "a"
while [ $# != 0 ]; do
	#echo "b"
    flag="$1"
    case "$flag" in
	-FACT_SOC) if [ $# -gt 1 ]; then
              arg="$2"
              shift
	      FACT_SOC=$arg
              #echo "You supplied an argument for the -pm flag: $arg"
            #else
              #echo "You did not provide an argument for the -pm flag"
            fi
            ;;
	-FACT_COG) if [ $# -gt 1 ]; then
              arg="$2"
              shift
	      FACT_COG=$arg
              #echo "You supplied an argument for the -pm flag: $arg"
            #else
              #echo "You did not provide an argument for the -pm flag"
            fi
            ;;
	-INERTIA) if [ $# -gt 1 ]; then
              arg="$2"
              shift
	      INERTIA=$arg
              #echo "You supplied an argument for the -pm flag: $arg"
            #else
              #echo "You did not provide an argument for the -pm flag"
            fi
            ;;

        *) echo "Unrecognized flag or argument: $flag"
            ;;
        esac
    shift
done

#linea de codigo a especificar
echo "./pso_nsp ${size} ${FACT_COG} ${FACT_SOC} ${INERTIA} ${ITERS} ${cnf_filename}"
for archivo in $( ls $cnf_filename |grep .nsp); do
    echo "./pso_nsp ${size} ${FACT_COG} ${FACT_SOC} ${INERTIA} ${ITERS} ${cnf_filename}"
   ./pso_nsp ${size} ${FACT_COG} ${FACT_SOC} ${INERTIA} ${ITERS} ${cnf_filename}
done



