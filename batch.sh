#!/bin/sh

mkdir train
mkdir model
mkdir test
mkdir iris_grayscale

# Generate raw grayscale images from original dataset
if ! [ "$(ls -A iris_grayscale/)" ]; then
	for img in iris/* 
	do
		# echo $img
		python3 to_gray.py $img iris_grayscale/$(basename $img)
	done
fi

# Select training and testing sets
for img in iris_grayscale/*
do
	echo $img
done

for side in {L,R} 
do
	for i in {001..064}
	do
		ran=$RANDOM
		ran=$((ran%3+1))
		for j in {1..3}
		do
			if ((j==ran)); then
				# echo $i$side\_$j.png
				cp iris_grayscale/$i$side\_$j.png test/
			else
				cat iris_grayscale/$i$side\_$j.png >> train/$i$side.png
			fi
		done
	done
done

# # Generating the models, i.e., the LZW dictionaries
# if ! [ "$(ls -A model/)" ]; then
# 	for k in {9..16}
# 	do
# 		for category in train/*
# 		do
# 			./build/encode $category output/dump.txt $k w model/$(basename ${category})_$k
# 			# echo ./build/encode $category output/dump.txt $k w model/$(basename ${category})_$k
# 		done
# 	done
# fi
# 
# # Go through the test set and compress files using each model
# if [ "$(ls -A test/)" ]; then
# 	for individual in test/*
# 	do
# 		for model in model/*
# 		do
# 			./build/encode $individual output/$(basename ${individual})_$(basename $model) 8 r $model
# 			# echo ./build/encode $individual output/$(basename ${individual})_$k 8 r $model
# 		done
# 	done
# fi
