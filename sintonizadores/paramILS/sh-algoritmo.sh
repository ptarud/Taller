#!/bin/bash

#if [ $# -lt 5 ]; then
#  echo "Cantidad invalida de parametros";
#  exit -1
#fi

cnf_filename=$1
instance_specifics=$2
cutoff_time=$3
cutoff_length=$4
seed=$5

shift 5

#echo $@

output_filename="resultados.txt"
rm -rf ${output_filename}

iteraciones=200
res=1
size=100

FACT_SOC=0
FACT_COG=0
INERTIA=0


while [ $# != 0 ]; do
    flag="$1"
    case "$flag" in
        -FACT_SOC) if [ $# -gt 1 ]; then
              arg="$2"
              shift
              cr=$arg
              #echo "You supplied an argument for the -pm flag: $arg"
            #else
              #echo "You did not provide an argument for the -pm flag"
            fi
            ;;
        -FACT_COG) if [ $# -gt 1 ]; then
              arg="$2"
              shift
              mr=$arg
              #echo "You supplied an argument for the -pm flag: $arg"
            #else
              #echo "You did not provide an argument for the -pm flag"
            fi
            ;;
	-INERTIA) if [ $# -gt 1 ]; then
              arg="$2"
              shift
              mr=$arg
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
#for archivo in $( ls $cnf_filename |grep .nsp); do
    echo "./taller tamaño cognitivo social inercia iteraciones archivo"
   ./taller ${size} ${FACT_COG} ${FACT_SOC} ${INERTIA} ${iteraciones} ${cnf_filename} ${output_filename} ${res} ${seed}

#done


solved="SAT"
runlength=`cat ${output_filename}`
runtime=0
best_sol=0


echo "Result for ParamILS: ${solved}, ${runtime}, ${runlength}, ${best_sol}, ${seed}"
