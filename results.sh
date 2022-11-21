mkdir results

rm -rf results/sizes.txt

for file in output/*
do
	basname=$(basename $file)
	test_name=$(echo $basname | cut -d "_" -f 1 | head -c 3)
	model_name=$(echo $basname | cut -d "_" -f 3 | head -c 3)
	k=$(echo $basname | cut -d "_" -f 4)
	echo $test_name $model_name $k $(wc -c $file | awk '{print $1}') >> results/sizes.txt
done
