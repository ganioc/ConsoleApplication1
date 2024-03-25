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
	void getFromInput(const char* msg, string& input);

	//class DSPFile {
	//private:
	//	// Data type 0~7 as defined above
	//	
	//};

	void testGetInput();
}


