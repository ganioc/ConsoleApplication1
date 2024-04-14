#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <typeinfo>

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
	class DSPMemoryException : public exception {
	public:
		virtual const char* what() const throw() {
			return "DSP memory exception";
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

	using BYTE        = unsigned char ;
	using DSPFILETYPE = unsigned char ;

	const DSPFILETYPE		UNSIGNED_CHAR  = 0;    // 0
	const DSPFILETYPE		UNSIGNED_SHORT = 1;    // 1
	const DSPFILETYPE		UNSIGNED_LONG  = 2;    // 2
	const DSPFILETYPE		FLOAT          = 3;    // 3
	const DSPFILETYPE		SIGNED_CHAR    = 4;    // 4
	const DSPFILETYPE		SIGNED_SHORT   = 5;    // 5
	const DSPFILETYPE		SIGNED_LONG    = 6;    // 6
	const DSPFILETYPE		DOUBLE         = 7;    // 7
	const DSPFILETYPE		UNSIGNED_INT   = 8;    // 8
	const DSPFILETYPE		SIGNED_INT     = 9;    // 9
	const DSPFILETYPE		COMPLEX        = 10;   // 10
	const DSPFILETYPE		LONG_VECTOR    = 128;
	const DSPFILETYPE		UNKNOWN_TYPE   = 0xFF;

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

		// Get record type 
		DSPFILETYPE convType(const type_info& type);

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

		// Place pointer at beginning of data
		void seek(int len = 0, int rec = 0);
		// read a single element from file
		template<class Type>
		void readElement(Type& element);
		// read vector from file, converts from C DSP_FILE type
		// to type of vector
		template<class Type>
		void read( vector<Type>& vec);
		// WriteElement, write single element to file
		template<class Type>
		void writeElement(const Type& element);
		template <class Type>
		void write(const vector<Type>& vec);
	};

	void main();
}


