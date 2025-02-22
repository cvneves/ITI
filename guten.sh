mkdir gutenberg/train
mkdir gutenberg/test
rm -rf gutenberg/train/*
rm -rf gutenberg/test/*

# for i in {1..10} 
# do
# 	k=$(($RANDOM%4 + 1))
# 
# 	for j in {1..4} 
# 	do
# 		filename=gutenberg/$i\_$j.txt
# 		if ((j == k));  then
# 			cp $filename gutenberg/test
# 		else 
# 			cat $filename >> gutenberg/train/$i.txt
# 		fi
# 	done
# done

for i in {1..10}
do
	class_list=$(shuf --echo $(ls gutenberg/$i\_*.txt))
	class_size=$(ls -l gutenberg/$i\_*.txt | wc -l)
	test_size=$(echo $(echo 0.2*$class_size | bc -l) | awk '{print int($1 + 0.5)}')
	it=0
	for j in ${class_list[@]}
	do
		if ((it < test_size)); then
			# echo $j $class_size macaco
			cp $j gutenberg/test
		else
			# echo $j $class_size
			cat $j >> gutenberg/train/$i.txt
		fi
		((it++))
	done
	# echo
done

# Generate models
mkdir gutenberg/model
rm -rf gutenberg/model/*

echo "Generating models"
for k in {9..16}
do
	start_time=$(date +'%s.%N')
	for category in gutenberg/train/*
	do
		./build/encode $category output/dump $k w gutenberg/model/$(basename ${category})_$k > /dev/null
	done
	end_time=$(date +'%s.%N')
	model_time[((k-9))]=$(echo $end_time-$start_time | bc)
done

echo "Compressing files from test set"
mkdir gutenberg/results/
rm -rf gutenberg/results/sizes.txt
num_tests=$(ls -l gutenberg/test/* | wc -l)
for k in {9..16}
do
	acc_size=0
	acc_used_words=0
	start_time=$(date +'%s.%N')
	for individual in gutenberg/test/*
	do
		min_compr_size=99999999999
		max_used_words=-1
		best_model_size=9999999
		best_model_used_words=9999999
		for model in gutenberg/model/*_$k
		do
			./build/encode $individual output/dump 8 r $model > tmp

			# log compressed file size and used indices
			basname=$(basename $individual)
			test_name=$(echo $basname | cut -d "_" -f 1)
			model_name=$(echo $(basename $model) | cut -d "." -f 1)
			k=$(echo $model | cut -d "_" -f 2)
			file_size=$(grep "Compressed file size: " tmp | awk '{print $4}')
			used_codewords=$(grep "Used codewords: " tmp | awk '{print $3}')

			# echo $test_name $model_name $k $file_size $used_codewords >> gutenberg/results/sizes.txt
			# echo $test_name $model_name $k $file_size $used_codewords
			if ((used_codewords > max_used_words)); then
				best_model_used_words=$model_name
				((max_used_words=used_codewords))
			fi
			if ((file_size < min_compr_size)); then
				best_model_size=$model_name
				((min_compr_size=file_size))
			fi
		done

		if ((test_name==best_model_size)); then
			acc_size=$(echo "scale=4; 1/$num_tests+$acc_size" | bc -l)
		fi
		if ((test_name==best_model_used_words)); then
			acc_used_words=$(echo "scale=4; 1/$num_tests+$acc_used_words" | bc -l)
		fi
	done
	end_time=$(date +'%s.%N')
	test_time[((k-9))]=$(echo $end_time-$start_time | bc)

	echo $k $acc_size $acc_used_words
	arr_size[((k-9))]=$acc_size
	arr_used_words[((k-9))]=$acc_used_words
done

echo ${arr_size[@]}
echo ${arr_used_words[@]}

echo #{arr_size[@]}
k=0
for ((i=0; i<${#arr_size[@]}; i++))
do
	((k=i+9))
	echo "$k" "${arr_size[$i]}" "${arr_used_words[$i]}" "${model_time[$i]}" "${test_time[$i]}" 
done > data
# echo ${#arr_size[@]}

gnuplot -p -e "set terminal postscript eps;
set output 'plot_accuracy.ps';
set title 'Acurácia x K';
set xlabel 'K';
set ylabel 'Acurácia';
set style line 100 lt 2;
set grid ls 2;
set key at graph 0.95, 0.1;
set key box lt -1 lw 2;
plot 'data' using 1:2 title \"tamanho\" with lp linewidth 5,
'data' using 1:3 title \"#índices\" with lp linewidth 5;"

gnuplot -p -e "set terminal postscript eps;
set output 'plot_time.ps';
set title 'Tempo x K';
set xlabel 'K';
set ylabel 'Tempo';
set style line 100 lt 2;
set grid ls 2;
set key at graph 0.4, 0.98;
set key box lt -1 lw 2;
plot 'data' using 1:4 title \"modelos com escrita\" with lines linewidth 5,
'data' using 1:5 title \"testes com escrita\" with lines linewidth 5,
'data' using 1:6 title \"modelos sem escrita\" with lines linewidth 5,
'data' using 1:7 title \"testes sem escrita\" with lines linewidth 5;"


rm -rf tmp
# rm -rf data
