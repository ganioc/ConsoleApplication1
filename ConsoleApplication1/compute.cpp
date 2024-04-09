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
	DSPFile::DSPFile() : m_type( UNKNOWN_TYPE),
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
		m_type = UNKNOWN_TYPE;
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
			unsigned char type = (m_type | LONG_VECTOR);

			// Write out header
			m_fs.write((const char*)&type, sizeof(type));
			m_fs.write((const char*)&m_elementSize, sizeof(m_elementSize));

			// The m_numRecords field is combined with the m_numRecords
			// field as a 32-bit integer
			m_fs.write((const char*)&m_recLen, sizeof(m_recLen));

		}
		else {
			// Write out header
			m_fs.write((const char*)&m_type, sizeof(m_type));
			m_fs.write((const char*)&m_elementSize, sizeof(m_elementSize));

			// Two unsigned shorts specify the number
			// of records and the record length
			unsigned short s = m_numRecords;
			m_fs.write((const char*)&s, sizeof(s));
			s = m_recLen;
			m_fs.write((const char*)&s, sizeof(s));
		}

		if (m_fs.fail()) {
			throw DSPException("Writing header");
		}

		// Begin data here
		m_posBeginData = m_fs.tellp();

		// File creation succeeded
		m_readOnly = false;
	}

	void DSPFile::close() {
		if (m_fs.is_open() == false) {
			return;
		}
		if (m_readOnly == false && m_elementSize != 0) {
			if (m_recLen == 0) {
				// File created sequentially by writeElement
				m_numRecords = 1;

				// seek to the end of data
				m_fs.seekp(0L, ios::end);

				// Get current position
				m_recLen = (int)((m_fs.tellp() - m_posBeginData) / m_elementSize);
			}

			// Seek to the beginning of the header
			m_fs.seekp(0L, ios::end);

			// Update LONG_VECTOR when writing header
			if (m_recLen > USHRT_MAX) {
				// File format doesn't support matrices larger than 65355x65355
				if (m_numRecords != 1) {
					m_fs.close();
					throw DSPException("Matrix too large for file format");
				}
				// LONG_VECTOR
				BYTE type = m_type | LONG_VECTOR;

				// Write out header
				m_fs.write((const char*)&type, sizeof(type));
				m_fs.write((const char*)&m_elementSize, sizeof(m_elementSize));

				// The m_numRecords field is combined with the m_numRecords field
				// as an 32-bit integer
				m_fs.write((const char*)&m_recLen, sizeof(m_recLen));
			}
			else {
				// Write out header
				m_fs.write((const char*)&m_type, sizeof(m_type));
				m_fs.write((const char*)&m_elementSize, sizeof(m_elementSize));

				// Two unsigned shorts specify the number of recordsand the 
				// record length
				unsigned short s = m_numRecords;
				m_fs.write((const char*)&s, sizeof(s));
				s = m_recLen;
				m_fs.write((const char*)&s, sizeof(s));

			}
			if (m_fs.fail()) {
				m_fs.close();
				throw DSPException("Writing header");
			}
			// Write out trailer
			if (m_trailer != NULL) {
				streampos posTrailer = m_elementSize *( m_numRecords * m_recLen);
				m_fs.seekp(posTrailer, ios::cur);
				m_fs.write(m_trailer, strlen(m_trailer) + 1);
				if (m_fs.fail()) {
					m_fs.close();
					throw DSPException("Writing trailer");
				}
			}
		}
		m_fs.close();
		empty();
	}

	void DSPFile::seek(int len, int rec) {

	}

	void DSPFile::openRead(const char* name) {
		// validate filename
		if (name == NULL) {
			throw DSPException("No file name");
		}
		// Check state of file
		if (m_fs.is_open()) {
			throw DSPException("Already opened");
		}

		// Clear existing data
		empty();

		// Try to open the file but do not create it
		m_fs.open(name, ios::in | ios::binary | ios::_Nocreate);
		if (m_fs.fail()) {
			throw DSPException("Cannot open for reading");
		}

		// File exists -- read information into DSPFile structure
		m_fs.read((char*)&m_type, sizeof(m_type));
		m_fs.read((char*)&m_elementSize, sizeof(m_elementSize));

		// Check for long vector type
		if (m_type & LONG_VECTOR) {
			// The m_numRecords field is combined with the 
			// m_numRecords field as a 32-bit integer
			m_numRecords = 1;
			m_fs.read((char*)&m_recLen, sizeof(m_recLen));
			// Mask out LONG_VECTOR bit
			m_type &= ~LONG_VECTOR;
		}
		else {
			// Two unsigned shorts specify the number
			// of records and the record length
			unsigned short s;
			m_fs.read((char*)&s, sizeof(s));
			m_numRecords = s;
			m_fs.read((char*)&s, sizeof(s));
			m_recLen = s;
		}

		if (m_fs.fail()) {
			throw DSPException("Reading header");
		}

		// Skip over binary data to get trailer size
		m_posBeginData = m_fs.tellg();
		m_fs.seekg(0L, ios::end);
		streampos posEndTrailer = m_fs.tellg();

		// Subtract data size to get trailer length
		streamoff dataSize = m_elementSize * m_numRecords * m_recLen;

		// Validate header
		if (dataSize > posEndTrailer - m_posBeginData) {
			throw DSPException("Invalid header");
		}

		streamoff trailerSize = posEndTrailer - m_posBeginData - dataSize;

		// Allocate trailer
		if (trailerSize != 0L) {
			// Seek to beginning of trailer from end of file
			m_fs.seekg(-trailerSize, ios::end);

			// Allocate trailer
			m_trailer = new char[trailerSize + 1];
			if (m_trailer == NULL) {
				throw DSPMemoryException();
			}

			// Read trailer
			m_fs.read(m_trailer, trailerSize);
			if (m_fs.fail()) {
				throw DSPException("Reading trailer");
			}

			// Make sure that string terminates
			m_trailer[trailerSize] = 0;
		}

		// Allocate buffer for readElement
		m_singleElement = new BYTE[m_elementSize];
		if (m_singleElement == NULL) {
			throw DSPMemoryException();
		}

		// Place pointer at beginning of data
		seek();
		m_readOnly = true;
	}

	template<class Type>
	void DSPFile::readElement(Type& element) {
		// Validate state of file
		if (m_fs.is_open() == false) {
			throw DSPException("File not opened");
		}
		if (m_readOnly == false) {
			throw DSPException("Not opened for reading");
		}
		if (m_recLen == 0 || m_numRecords == 0) {
			throw DSPException("No data in file");
		}
		if (m_numRecords != 1) {
			throw DSPException("File has more than one record");
		}
		if (m_type == UNKNOWN_TYPE) {
			throw DSPException("Cannot read from unknown type");
		}
		// Read a single element
		m_fs.read(m_singleElement, m_elementSize);
		if (m_fs.fail()) {
			throw DSPException("Failure reading next element");
		}

		// Convert it to type
		convBuffer(&element, m_singleElement, m_type, 1);
	}

}
