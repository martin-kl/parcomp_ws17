//generates different arrays with size = xxxxx (make a header file for this

int* generateIntSameNumbers(int size) {
	int result[size];
	for(int i = 0; i < size; i++) {
		result[i] = 5;
	}
	return result;
}

int* generateIntAscendingNumbers(int size) {
	int result[size];
	for(int i = 0; i < size; i++) {
		result[i] = i+1;
	}
	return result;
}

int* generateIntDescendingNumbers(int size) {
	int result[size];
	for(int i = size-1; i >= 0; i-- {
		result[i] = i+1;
	}
	return result;
}
