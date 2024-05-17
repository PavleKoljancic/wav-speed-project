inputFiles=`ls ./media/`
algsVersions="O3 O0"
threads="12 4 1"
SSE="1 0"
for alg in $algsVersions
do
    for x in $SSE
    do
            for file in $inputFiles
                do

                    for thread_num in $threads
                    do

                                        for i in {1..5};
                                                do

                                                    #`time ./$alg -i "./media/$file"  -o "res" -s "1.75" -t "$thread_num" -x "$x"  2>&1 | grep real`
                                                    { time ./$alg -i "./media/$file" -o "res" -s "1.75" -t "$thread_num" -x "$x"; } 2>&1 | grep real >> " $alg $file $thread_num $x".txt


                                                done
                    done


                done
    done
done
