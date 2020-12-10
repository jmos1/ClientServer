!#/bin/bash

for N in {1..40}
do
    ./client &
done
wait
