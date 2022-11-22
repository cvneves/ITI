#!/bin/sh

mkdir train
mkdir model
mkdir test
mkdir iris_grayscale
mkdir results

# Generate raw grayscale images from original dataset
if ! [ "$(ls -A iris_grayscale/)" ]; then
	for img in iris/* 
	do
		# echo $img
		python3 to_gray.py $img iris_grayscale/$(basename $img)
	done
fi

# Select training and testing sets

# for side in {L,R} 
# do
# 	for i in {001..064}
# 	do
# 		ran=$RANDOM
# 		ran=$((ran%3+1))
# 		for j in {1..3}
# 		do
# 			if ((j==ran)); then
# 				# echo $i$side\_$j.png
# 				cp iris_grayscale/$i$side\_$j.png test/
# 			else
# 				cat iris_grayscale/$i$side\_$j.png >> train/$i$side.png
# 			fi
# 		done
# 	done
# done

rm -rf test/*
rm -rf train/*
for i in {001..064}
do
	ran=$RANDOM
	ran=$((ran%6+1))
	k=0
	for side in {L,R} 
	do
		for j in {1..3}
		do
			k=$((k+1))
			if ((k==ran)); then
				# echo "cp iris_grayscale/$i$side\_$j.png test/"
				cp iris_grayscale/$i$side\_$j.png test/
			else
				# echo "cat iris_grayscale/$i$side\_$j.png >> train/$i.png"
				cat iris_grayscale/$i$side\_$j.png >> train/$i.png
			fi
		done
	done
	# echo 
done

# Generating the models, i.e., the LZW dictionaries
rm -rf model/*
if ! [ "$(ls -A model/)" ]; then
	echo "Generating models"
	for k in {16..16}
	do
		for category in train/*
		do
			./build/encode $category output/dump.txt $k w model/$(basename ${category})_$k > /dev/null
			# echo ./build/encode $category output/dump.txt $k w model/$(basename ${category})_$k
		done
	done
fi

# Go through the test set and compress files using each model
rm -rf results/sizes.txt

if [ "$(ls -A test/)" ]; then
	echo "Compressing files from test set"
	for individual in test/*
	do
		for model in model/*
		do
			./build/encode $individual output/dump.txt 8 r $model > tmp

			# log compressed file size and used indices
			basname=$(basename $individual)
			test_name=$(echo $basname | cut -d "_" -f 1 | head -c 3)
			model_name=$(echo $model | cut -d "." -f 1)
			k=$(echo $model | cut -d "_" -f 2)
			file_size=$(grep "Compressed file size: " tmp | awk '{print $4}')
			used_codewords=$(grep "Used codewords: " tmp | awk '{print $3}')

			echo $test_name $model_name $k $file_size $used_codewords >> results/sizes.txt
			# echo ./build/encode $individual output/$(basename ${individual})_$k 8 r $model
		done
	done
fi

rm -rf tmp

# # Compile results

# for file in output/*
# do
# 	basname=$(basename $file)
# 	test_name=$(echo $basname | cut -d "_" -f 1 | head -c 3)
# 	model_name=$(echo $basname | cut -d "_" -f 3 | head -c 3)
# 	k=$(echo $basname | cut -d "_" -f 4)
# 
# 	# file_size=$(wc -c $file | awk '{print $1}')
# 	echo $test_name $model_name $k >> results/sizes.txt
# done
