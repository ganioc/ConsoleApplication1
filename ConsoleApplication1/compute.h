#pragma once

#include <iostream>
#include <string>

using namespace std;

namespace compute {
	int add_two(int num);


	// Define a custom exception 
	class DSPInputException : public exception {
	public:
		virtual const char* what() const throw() {
			return "DSP input exception";
		}
	};

	//
	// getInput
	// Throws: DSPException
	//
	//void getFromInput(const char* msg, string& input) {
	//	const int MAX_LINE_LENGTH = 80;
	//	char buf[MAX_LINE_LENGTH] = { 0 };

	//	if (msg != NULL) {
	//		cout << msg << " : ";
	//	}
	//	else {
	//		cout << "Enter string: ";
	//	}

	//	cin.getline(buf, MAX_LINE_LENGTH);
	//	if (cin.eof()) {
	//		throw DSPInputException();
	//	}
	//	input = buf;
	//}

	//class DSPFile {
	//private:
	//	// Data type 0~7 as defined above
	//	
	//};

	void testGetInput();
}


