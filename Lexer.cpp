#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <sstream>
#include <map>
#include <vector>
#include <cstdlib>
using namespace std;

int search(string a[], int length, string key) {
	for (int i = 0; i < length; i++){
		if (a[i] == key){
			return i;
		}
	}
	return -1;
}

string bracket(string s){
	return "<" + s + ">";
}

struct state_pair {
	int state;
	string input;
    state_pair(int s = -1, string i = "") {
    	state = s;
    	input = i;
    }

    friend bool operator<(const state_pair& foo, const state_pair& foo1) {
    	if (foo.state == foo1.state){
    		return foo.input < foo1.input;
    	}
    	return foo.state < foo1.state;
    }

    friend bool operator>(const state_pair& foo, const state_pair& foo1) {
    	if (foo.state == foo1.state){
    	    return foo.input > foo1.input;
    	}
    	return foo.state > foo1.state;
    }

};

int main(int argc, char* argv[]) {
	if (argc != 5 || strcmp(argv[1], "-dfa") != 0 || strcmp(argv[3], "-input") != 0){
		printf("Usage: %s -dfa <dfa_file> -input <input_file>", argv[0]);
		return 2;
	}

	ifstream dfa_file(argv[2]);

	string line;
	if (!getline(dfa_file, line)) {
		cout << "EOF reached in input file";
	}

	string valid_inputs[16] = {"a-k", "l", "o", "p", "r", "0", "1-9", ".", "+", "*", "=", "{", "}", "space", "nl", "token"};
	std::map<int, string> input_map;
	std::istringstream istr(line);
	string test;
	int i = 0, token_field = -1;
	while (istr >> test){
		if (search(valid_inputs, 16, test) != -1){
			if("token" == test) {
				token_field = i;
			}
			input_map[i] = test;
			i++;
		} else {
			cout << "Element " << bracket(test) << " was not found in list of acceptable inputs." << endl;
			return 2;
		}
	}

	int state_pairs_size = input_map.size();
	std::map<state_pair, int> state_map;
	std::map<int, string> state_token_map;
	while (getline(dfa_file, line)) {
		istr.clear();
		istr.str(line);
		int state;
		istr >> state;

		for (int i = 0; i < (int)input_map.size(); i++){
			string temp;
			istr >> temp;
			if (i == token_field){
				state_token_map[state] = temp;
			} else {
				state_pair temp_pair;
				temp_pair.state = state;
				temp_pair.input = input_map.find(i)->second;
				state_map[temp_pair] = atoi(temp.c_str());
			}
		}
	}

	state_pair s;
	s.state = 1;
	s.input = "a-k";

	int current_state = 1;

	FILE * inFile;
	char strTest[100];
	vector<string> inputs;
	vector<char> raw_inputs;
	inFile = fopen(argv[4], "r");

	if (inFile == NULL){
		printf("Error opening input file %s", bracket(argv[4]).c_str());
		return 2;
	} else {

		char buf[100];
		while (fgets(buf, 100, inFile) != NULL){
			for (int i = 0; i < (int)strlen(buf); i++) {
				int asciiChar = (int)buf[i];
				char c = buf[i];
				string pass = "";
				pass += c;
				if (asciiChar >= 97 && asciiChar <= 107) {
					pass = "a-k";
				} else if (asciiChar >= 49 && asciiChar <= 57) {
					pass = "1-9";
				} else if (buf[i] == ' ') {
					pass = "space";
				} else if (buf[i] == '\n') {
					pass = "nl";
				}

				state_pair s;
				s.state = current_state;
				s.input = pass;

				if (state_map.find(s) == state_map.end()){
					cout << "Invalid input encountered: " << bracket(pass) << endl;
					return 2;
				}
				inputs.push_back(pass);
				raw_inputs.push_back(buf[i]);
				//cout << "(" << current_state << ", " << bracket(pass) << ") -> " << state_map.find(s)->second << endl;
			}
		}
		fclose(inFile);

	}

	int gbn = 0, match_index = -1;
	string token = "";
	string current_string = "";

	if (state_token_map.find(1)->second != "NF"){
		token = state_token_map.find(1)->second;
	}


	for (int i = 0; i < raw_inputs.size(); i++){
		state_pair s;
		s.state = current_state;
		s.input = inputs[i];
		int next_state = state_map.find(s)->second;

		string next_token = "NF";
		if (next_state != 0) {
			next_token = state_token_map.find(next_state)->second;
			if (i == raw_inputs.size()-1){
				current_string += raw_inputs[i];
				if (next_token != "NF"){
					cout << bracket(next_token) << " " << current_string << endl;
				} else {
					cout << "No match for " << bracket(current_string) << endl;
				}
			}
		}

		if (next_state == 0){
			if (gbn == 0){
				cout << "No match for " << bracket(current_string) << endl;
				return 2;
			} else {

				cout << bracket(token) << " " << current_string.substr(0, gbn) << endl;

				i = match_index;
				gbn = 0;
				current_state = 1;
				token = "";
				current_string = "";
			}
		} else {
			current_state = next_state;
			if (next_token != "NF"){
				token = next_token;
				current_string += raw_inputs[i];
				gbn = current_string.size();
				match_index = i;
			} else {
				current_string += raw_inputs[i];

			}
		}
	}
	return 0;
}
