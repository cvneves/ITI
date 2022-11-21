f = open("results/sizes.txt")

predicted = [[-1] * 64] * 8

pred_dict = dict()
for k in range(9, 16 + 1):
    pred_dict[k] = dict()

for line in f:
    temp = line.split(" ")
    temp[-1]=temp[-1][:-1]

    test_id = int(temp[0])
    model_id = int(temp[1])
    k = int(temp[2])
    file_size = int(temp[3])
    
    print(test_id, model_id, k, file_size)
    pred_dict[k][(test_id, model_id)] = file_size
    
f.close()

for k in range(9, 16 + 1):
    for test in range(1, 64 + 1):
        best_cat = -1
        min_size = 9999999999999

        for category in range(1, 64 + 1):
            tmp_size = pred_dict[k][(test, category)]
            if tmp_size < min_size:
                best_cat = category
                min_size = tmp_size

        predicted[k - 9][test - 1] = best_cat

# print(predicted[0])
for k in range(0,8):
    correct = [1 if predicted[k][i] - 1 == i else 0 for i in range(0, len(predicted[k]))]
    acc = 0
    for a in correct:
        acc += a / 64.0
    print("k:", k + 9, "Accuracy:", acc)
