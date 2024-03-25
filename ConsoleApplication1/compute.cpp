#include "compute.h"


namespace compute {
	int add_two(int num)
	{
		return num + 2;
	}

	void getFromInput(const char* msg, string& input) {
		const int MAX_LINE_LENGTH = 80;
		char buf[MAX_LINE_LENGTH] = { 0 };

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
	}

	void testGetInput() {
		string strName;

		cout << "test getFromInput()" << "\n";

		// getFromInput("Enter your name", strName);

		// Get Parameter from user
		//do {
		//	getFromInput("Enter your name", strName);
		//} while (strName.empty());
	}
}
