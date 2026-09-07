#!/bin/bash
count=$#
sum=0
for num in "$@"
do
sum=$(echo "$sum + $num" | bc)
done
average=$(echo "scale=2; $sum / $count" | bc)
echo "Количество чисел: $count"
echo "Сумма: $sum"
echo "Среднее арифметическое: $average"
