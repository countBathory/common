read str
IFS=$'\n'
count=0
size=0
for a in $( find -type f)
do 
	if grep -q $str $a;then
		echo $a
		count=$(($count + 1))
		size=$(( $(wc -c "$a" | awk '{print $1}') + $size))
	fi
done
echo $count
echo $size
