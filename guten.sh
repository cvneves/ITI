mkdir gutenberg/train
mkdir gutenberg/test
rm -rf gutenberg/train/*
rm -rf gutenberg/test/*

for i in {1..10} 
do
	k=$(($RANDOM%4 + 1))

	for j in {1..4} 
	do
		filename=gutenberg/$i\_$j.txt
		if ((j == k));  then
			cp $filename gutenberg/test
		else 
			cat $filename >> gutenberg/train/$i.txt
		fi
	done
done

# Generate models
mkdir gutenberg/model
rm -rf gutenberg/model/*

echo "Generating models"
for k in {16..16}
do
	for category in gutenberg/train/*
	do
		./build/encode $category output/dump $k w gutenberg/model/$(basename ${category})_$k > /dev/null
	done
done

echo "Compressing files from test set"
mkdir gutenberg/results/
rm -rf gutenberg/results/sizes.txt
acc=0
for individual in gutenberg/test/*
do
	min_compr_size=99999999999
	best_model=9999999
	for model in gutenberg/model/*
	do
		./build/encode $individual output/dump 8 r $model > tmp

		# log compressed file size and used indices
		basname=$(basename $individual)
		test_name=$(echo $basname | cut -d "_" -f 1)
		model_name=$(echo $(basename $model) | cut -d "." -f 1)
		k=$(echo $model | cut -d "_" -f 2)
		file_size=$(grep "Compressed file size: " tmp | awk '{print $4}')
		used_codewords=$(grep "Used codewords: " tmp | awk '{print $3}')

		echo $test_name $model_name $k $file_size $used_codewords >> gutenberg/results/sizes.txt
		# echo $test_name $model_name $k $file_size $used_codewords
		if ((-used_codewords < min_compr_size)); then
			best_model=$model_name
			((min_compr_size=-used_codewords))
			# echo $min_compr_size
		fi
	done

	if ((test_name==best_model)); then
		acc=$(echo 0.1 + $acc | bc -l)
	fi
done

echo Accuracy: $acc

rm -rf tmp
