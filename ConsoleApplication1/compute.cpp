#include "compute.h"


namespace compute {
	int add_two(int num)
	{
		return num + 2;
	}

	void getInput(const char* msg, string& input) {
		const int MAX_LINE_LENGTH = 80;
		char buf[MAX_LINE_LENGTH] = { 0 };

		if (msg != NULL) {
			cout << msg << " : ";
		}
		else {
			cout << "Enter string: ";
		}

		cin.getline(buf, MAX_LINE_LENGTH);
		if (cin.eof()) {
			throw DSPInputException();
		}
		input = buf;
	}

	void testGetInput() {
		string strName;

		cout << "test getFromInput()" << "\n";

		// getFromInput("Enter your name", strName);

		// Get Parameter from user
		do {
			getInput("Enter your name", strName);
		} while (strName.empty());

		cout << "The name is: " << strName << endl;
	}

	// class DSPFile
	DSPFile::DSPFile() : m_type( DSPFILETYPE::UNKNOWN_TYPE),
		m_elementSize(0),
		m_numRecords(0),
		m_recLen(0),
		m_trailer(NULL),
		m_readOnly(true),
		m_posBeginData(0),
		m_singleElement(NULL) {

	}
	DSPFile::~DSPFile() {
		close();
		empty();
	}
	void DSPFile::empty() {
		m_type = DSPFILETYPE::UNKNOWN_TYPE;
		m_elementSize = 0;
		m_numRecords = 0;
		m_recLen = 0;
		m_readOnly = true;

		if (m_singleElement != NULL) {
			delete[] m_trailer;
			m_trailer = NULL;
		}

		if (m_fs.is_open()) {
			m_fs.close();
		}
	}
	bool DSPFile::isFound(const char* name) {
		// Validate filename
		if (name == NULL) {
			throw DSPParamException();
		}

		// Open the file
		fstream fs;
		fs.open(name, ios::in | ios::_Nocreate | ios::binary);
		if (fs.fail()) {
			return false;
		}
		fs.close();
		return true;
	}

	//
	void DSPFile::openWrite(const char* name) {
		// Validate filename
		if (name == NULL) {
			throw DSPException("No file name");
		}

		// Check the state of the file
		if (m_fs.is_open()) {
			throw DSPException("Already opened");
		}

		// Clear existing data
		empty();

		// Create a new DSP File
		m_fs.open(name, ios::out | ios::trunc | ios::binary);
		if (m_fs.fail()) {
			throw DSPException("Cannot open for writing");
		}

		// Write blank header from DSPFile structure
		if (m_recLen > USHRT_MAX) {
			// Must write vector out as a LONG_VECTOR (32-bits)
			unsigned char type = m_type | LONG_VECTOR;
		}
	}
}
