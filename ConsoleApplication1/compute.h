#pragma once

#include <iostream>
#include <string>
#include <fstream>

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
	class DSPParamException : public exception {
	public:
		virtual const char* what() const throw() {
			return "DSP params exception";
		}
	};
	class DSPException : public exception {
	private:
		string msg;
	public:
		DSPException(const char* str) :msg(string(str)) {}
		virtual const char* what() const throw() {
			return msg.c_str();
		}
	};

	//
	// getInput
	// Throws: DSPException
	//
	void getInput(const char* msg, string& input);

	//class DSPFile {
	//private:
	//	// Data type 0~7 as defined above
	//	
	//};

	template <class Type>
	void getInput(const char* msg, Type& input, Type rangeMin, Type rangeMax) {
		if (rangeMin > rangeMax) {
			throw DSPParamException();
		}

		do {
			if (msg != NULL) {
				cout << msg << " [" << rangeMin << ".." << rangeMax << "] : ";
			}
			else {
				cout << "Enter number [" << rangeMin << ".." << rangeMax << "] : ";
			}

			cin >> input;

			if (cin.eof()) {
				throw DSPInputException();
			}
		} while (input < rangeMin || input > rangeMax);

		// Read trailing newline
		cin.get();
	}

	void testGetInput();


	enum class DSPFILETYPE {
		UNSIGNED_CHAR = 0,  // 0
		UNSIGNED_SHORT = 1, // 1
		UNSIGNED_LONG,  // 2
		FLOAT,          // 3
		SIGNED_CHAR,    // 4
		SIGNED_SHORT,   // 5
		SIGNED_LONG,    // 6
		DOUBLE,         // 7
		UNSIGNED_INT,   // 8
		SIGNED_INT,     // 9
		COMPLEX,        // 10
		UNKNOWN_TYPE = 0xFF
	};

	// DSPFile
	class DSPFile {
	private:
		// Data type 0-7 as defined above
		DSPFILETYPE m_type;

		// Size of each element
		unsigned char m_elementSize;

		// Number of records
		int m_numRecords;

		// Number of elements in each record
		int m_recLen;

		// Pointer to trailer text
		char* m_trailer;

		// File data
		fstream m_fs;
		bool m_readOnly;
		streampos m_posBeginData;

		// Buffer for reading single elements
		unsigned char* m_singleElement;

		/////////////////////
		// Helper functions
		/////////////////////

		// Clear data
		void empty();

	
	public:
		DSPFile();
		~DSPFile();

		/////////////////////
		// File operations
		/////////////////////
		static bool isFound(const char* name);
		bool hasData() {
			return ((m_numRecords * m_recLen) != 0);
		}
		void openRead(const char* name);
		void openWrite(const char* name);
		void close();

		/////////////////////
		// Get file format
		/////////////////////
		DSPFILETYPE getType() {
			return m_type;
		}
		int getNumRecords() {
			return m_numRecords;
		}
		int getRecLen() {
			return m_recLen;
		}
		//////////////////////////////
		// Get and Set file contents
		//////////////////////////////
		void getTrailer(string& str);
		void setTrailer(const char* trailer);
		void seek(int len = 0, int rec = 0);

	};
}


