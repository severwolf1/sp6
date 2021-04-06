#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <map>
#include <thread>
#include <stdio.h>
#include <ctype.h>
#include <mutex>


#define NUM_OF_THREADS 2

using namespace std;

string str = "world";

void validate_args(int argc, char const* argv[]);
int fileSize(string path_file);
void run_process(
	string path_file,
	int s_size,
	int idx
	);

vector<string> vector_s(const string& s, char delimiter);
void prepare_line(string& line);
void process(
	string path_file,
	int offset,
	int s_size,
	string output_file_path
	);
vector<int> substring_positions;

map<string, int> bigrams;
mutex bigram_mutex;

int main(int argc, char const* argv[])
{
	string path_file = "C:/test.txt";
	cout << "Path to file: " << path_file << endl;

	int file_size = fileSize(path_file);
	cout << "Size of file: " << file_size << endl;

	int s_size = file_size / NUM_OF_THREADS;
	cout << "Size of s: " << s_size << endl;


	vector<thread> th_handlers;

	for (int i = 0; i < NUM_OF_THREADS; ++i)
		th_handlers.push_back(
			thread(
				run_process,
				path_file,
				s_size,
				i
				)
			);

	for (int i = 0; i < NUM_OF_THREADS; ++i)
		th_handlers[i].join();


	return 0;
}

void validate_args(int argc, char const* argv[]) {
	if (argc != 2) {
		cout << "Usage: " << argv[0] << " <path_file>" << endl;
		exit(1);
	}
}


void run_process(
	string path_file,
	int s_size,
	int idx
	) {
	int offset = idx * s_size;

	string output_filename = "output_" + to_string(idx) + ".txt";
	bigram_mutex.lock();
	cout << "Run " << idx + 1 << " process " << endl;
	bigram_mutex.unlock();
	process(
		path_file,
		offset,
		s_size,
		output_filename
		);
	bigram_mutex.lock();
	cout << "Run " << idx + 1 << " finished " << endl;
	bigram_mutex.unlock();
}


int fileSize(string path_file) {
	ifstream file(path_file, ios::binary | ios::ate);
	return file.tellg();
}


void process(
	string path_file,
	int offset,
	int s_size,
	string output_file_path
	) {


	fstream input_file;
	input_file.open(path_file, ios::in);
	input_file.seekg(offset);

	char* s = new char[s_size];
	input_file.read(s, s_size);

	//поиск слова в s
	int index = -1;
	for (int i = 0; i < s_size; i++)
	{
		bool succes = true;
		
		for (int j = 0; j < str.length(); j++)
		{
			if (str[j] != s[i + j])
				succes = false;
		}
		if (succes)
			index = i;
		
	}
	
	if (index != -1)
	{
		bigram_mutex.lock();
		cout << "\nThe substring is present in the string: " << offset + index << endl;
		bigram_mutex.unlock();
	}
	else {
		cout << "There is no substring in the string";
	}
	index = 0;
}


vector<string> vector_s(const string& s, char delimiter) {
	vector<string> tokens;
	string token;
	istringstream tokenStream(s);
	while (getline(tokenStream, token, delimiter))
		tokens.push_back(token);

	return tokens;
}

void prepare_line(string& line) {
	for (int i = 0; i < line.length(); ++i) {
		if (!(
			('a' <= line[i] && line[i] <= 'z') ||
			('A' <= line[i] && line[i] <= 'Z')
			))
			line[i] = ' ';
		else
			line[i] = tolower(line[i]);
	}
}