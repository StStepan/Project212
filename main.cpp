#include "common.h"
#include <windows.h>
#include <string>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

#include "ThreadPool.h"
//array of string
typedef std::vector<std::string> ArrayOfVector;

//Line by line into the ArrayOfVector
std::vector<ArrayOfVector*>* readFileToArrayOfVectors(std::string filePath, int arrayOfVectorCount) {
	if (arrayOfVectorCount < 1) throw;

	std::ifstream in(filePath);
	if (in.bad()) throw;

	auto* ArraysssOfVector = new std::vector<ArrayOfVector*>();//auto type defenition
	//Create array of array of string
	for (int i = 0; i < arrayOfVectorCount; i++) ArraysssOfVector->push_back(new ArrayOfVector());

	int currentArrayOfVector = 0;
	std::string line;
	//currentArrayOfVector - ArrayOfVector to put string, 
	while (std::getline(in, line)) {
		ArraysssOfVector->at(currentArrayOfVector)->push_back(line);//put at the end of array of array
		currentArrayOfVector = (currentArrayOfVector + 1) % arrayOfVectorCount;
	}

	in.close();
	return ArraysssOfVector;
}

void writeArrayOfVectorToFile(std::string filePath, ArrayOfVector* ArrayOfVector) {
	std::ofstream out(filePath);
	if (out.bad()) throw;
	for (std::string line : *ArrayOfVector) out << line << std::endl;
	out.close();
}

//merge of 2 array of string
ArrayOfVector* merge(ArrayOfVector* ArrayOfVectorA, ArrayOfVector* ArrayOfVectorB) {
	if (ArrayOfVectorA->empty()) return ArrayOfVectorB;
	if (ArrayOfVectorB->empty()) return ArrayOfVectorA;

	auto* result = new ArrayOfVector(ArrayOfVectorA->size() + ArrayOfVectorB->size());
	std::merge(ArrayOfVectorA->begin(), ArrayOfVectorA->end(), ArrayOfVectorB->begin(), ArrayOfVectorB->end(),result->begin());
	return result;
}

//merge of array of array of string
ArrayOfVector* merge(std::vector<ArrayOfVector*>* arayssOfVector) {
	auto* result = new ArrayOfVector();
	for (auto* ArrayOfVector : *arayssOfVector) {
		result = merge(result, ArrayOfVector);
	}
	return result;
}

int main(int argc, const char* argv[]) {

	const std::string INPUT_PATH = "C:\\Users\\Степан\\Desktop\\trash\\input.txt";
	const std::string OUTPUT_PATH = "C:/Users/Степан/Desktop/trash_out/output.txt";
	const int THREAD_COUNT = 5; //changeble 
	const int ARRAYSOFVECTOR_COUNT = 5; //tied

	std::cout << "Input file reading is started" << std::endl;
	std::vector<ArrayOfVector*>* arrayofvectors = readFileToArrayOfVectors(INPUT_PATH, ARRAYSOFVECTOR_COUNT);
	std::cout << "File is successfully read" << std::endl;

	std::cout << "Sorting is started" << std::endl;

	ThreadPool threadPool(THREAD_COUNT);
	std::vector<Future*> futures;

	for (ArrayOfVector* arrayofvector : *arrayofvectors) {
		Future* future = threadPool.schedule([arrayofvector]() -> void
			{
				std::sort(arrayofvector->begin(), arrayofvector->end());
			});
		//save into the array
		futures.push_back(future);
	}

	//waiting for all calculation complete
	for (Future* future : futures) future->wait();

	std::cout << "Merging is started" << std::endl;
	ArrayOfVector* result = merge(arrayofvectors);
	std::cout << "Merging is successfully finished" << std::endl
		<< "Sorting is successfully finished" << std::endl;

	std::cout << "Result writing is started" << std::endl;
	writeArrayOfVectorToFile(OUTPUT_PATH, result);
	std::cout << "Result is successfully written" << std::endl;
	return 0;
}
