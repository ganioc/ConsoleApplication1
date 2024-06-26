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
	DSPFile::DSPFile() : m_type(UNKNOWN_TYPE),
		m_elementSize(0),
		m_numRecords(0),
		m_recLen(0),
		m_trailer(NULL),
		m_readOnly(true),
		m_posBeginData(0),
		m_singleElement(NULL) {

	}

	// DSPFile destructor
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
			delete[] m_singleElement;
			m_singleElement = NULL;
		}

		if (m_trailer != NULL) {
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

	DSPFILETYPE DSPFile::convType(const type_info& type) {
		if (typeid(unsigned char) == type) {
			return UNSIGNED_CHAR;
		}
		if (typeid(short int) == type) {
			return SIGNED_SHORT;
		}
		// ToDo: add more types
		if (typeid(float) == type) {
			return FLOAT;
		}

		return UNKNOWN_TYPE;
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

		// Begin data here, tellp()�����
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

				// Get current position, correct computation
				m_recLen = (int)((m_fs.tellp() - m_posBeginData) / m_elementSize);
			}

			// Seek to the beginning of the header
			m_fs.seekp(0L, ios::beg);

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

				// Two unsigned shorts specify the number of records and the 
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
				// 
				int pos = m_elementSize * (m_numRecords * m_recLen);
				streampos posTrailer = (streampos)pos;
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
	// Place pointer at beginning of data
	void DSPFile::seek(int len, int rec) {
		m_fs.clear();
		m_fs.seekg(m_posBeginData, ios::beg);
	}

	// Open a file for reading
	// When the file is opened, the header and trailer 
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
		int sz = m_elementSize * m_numRecords * m_recLen;
		streamoff dataSize = (streamoff)sz;

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
		// ? problems here
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

	template<class Type>
	void DSPFile::convBuffer(
		vector<Type>& vec,
		BYTE* rowData,
		DSPFILETYPE type,
		int numEle) {
		// put row Data into the vector
		copy(&rowData[0], &rowData[numEle], back_inserter(vec));
	}

	// Read vector from file. 
	template<class Type>
	void DSPFile::read(vector<Type>& vec) {
		int lenTemp;

		// Validate state of file
		if (m_fs.is_open() == false) {
			throw DSPException("Not opened");
		}

		if (m_readOnly == false) {
			throw DSPException("Not opened for reading");
		}

		// Get current file pointer position
		int recoff = (m_fs.tellg() - m_posBeginData) / m_elementSize;
		if (recoff >= m_recLen * m_numRecords) {
			throw DSPException("Read past end of data");
		}

		// Calculate elements to read until next record starts
		recoff = m_recLen - (recoff % m_recLen);

		// Allocate vector for data
		/*cout << "recoff: " << recoff << endl;*/

		// resize the vector, not as reserve()
		vec.resize(recoff);

		/*	int lenType =  sizeof(Type);*/

		if (m_type == convType(typeid(Type))) {
			// Read data directly into vector buffer without conversion
			// m_fs.read((BYTE*)vec.data(), sizeof(Type) * recoff);
			// m_fs.read(reinterpret_cast<char*>(vec.data()), sizeof(Type) * m_recLen);
			m_fs.read((char*)(&vec[0]), sizeof(Type) * recoff);
			// How to read from fstream into a vector?



			//cout << "print out vec:\n";
			//cout << vec[0] << ", " << vec[1] << " ," << vec[2];

			if (m_fs.fail()) {
				throw DSPException("Reading vector");
			}
		}
		else
		{
			// Read data into temporary buffer then convert
			lenTemp = m_elementSize * recoff;
			BYTE* rowData = new BYTE[lenTemp];
			if (rowData == NULL) {
				throw DSPMemoryException();
			}

			// Read Row in
			m_fs.read(reinterpret_cast<char*>(rowData), lenTemp);
			if (m_fs.fail()) {
				delete[] rowData;
				throw DSPException("Reading vector");
			}

			// Convert it to vector type
			try {
				this->convBuffer(vec, rowData, m_type, recoff);
			}
			catch (DSPException& e) {
				delete[] rowData;
				throw e;
			}
			delete[] rowData;
		}
	}
	template<class Type>
	void DSPFile::writeElement(const Type& element) {
		// Validate state of file
		if (m_fs.is_open() == false) {
			throw DSPException("Not opened");
		}

		if (m_readOnly) {
			throw DSPException("Not opened for writing");
		}

		DSPFILETYPE dft = convType(typeid(Type));
		if (dft == UNKNOWN_TYPE) {
			throw DSPException("Writing unknown type");
		}
		else if (m_type == UNKNOWN_TYPE) {
			// Set type to data coming in
			m_type = dft;
			m_elementSize = sizeof(Type);

		}
		else if (m_type != dft) {
			throw DSPException("Can only write one type");
		}

		if (m_numRecords > 1) {
			throw DSPException("Can only write to single record");
		}

		// We can only write vectors when using writeElement
		m_numRecords = 1;

		// Write single element
		m_fs.write((BYTE*)&element, m_elementSize);

		if (m_fs.fail()) {
			throw DSPException("Writing single data element");
		}
	}

	template <class Type>
	void DSPFile::write(const vector<Type>& vec) {
		// Validate state of file
		if (m_fs.is_open() == false) {
			throw DSPException("opened");
		}

		if (m_readOnly) {
			throw DSPException("Not opened for writing");
		}

		// Validate input
		if (vec.empty()) {
			throw DSPException("Writing empty vector");
		}

		DSPFILETYPE type = convType(typeid(Type));

		if (m_numRecords == 0) {
			// First data written to file
			m_type = type;
			m_elementSize = sizeof(Type);
			m_numRecords = 0;
			m_recLen = (int)vec.size();
		}
		else {
			// Data already written to file(check type and length)
			if (m_type != type) {
				throw DSPException("Vector of differnt type than file");
			}
			if (vec.size() != m_recLen) {
				throw DSPException("Vector of different length");
			}
		}
		// Write out data
		// m_fs.write((BYTE*)vec., sizeof(Type) * m_recLen);
		m_fs.write(reinterpret_cast<const char*>(vec.data()), sizeof(Type) * m_recLen);

		if (m_fs.fail()) {
			throw DSPException("Writing vector");
		}
		m_numRecords++;
	}

	void DSPFile::getTrailer(string& str) {
		str.clear();
		if (m_fs.is_open() == false) {
			throw DSPException("Not opened");
		}
		if (m_readOnly == false) {
			throw DSPException("Not opened for reading");
		}

		str = m_trailer;
	}

	void DSPFile::setTrailer(const char* trailer) {
		if (m_fs.is_open() == false) {
			throw DSPException("Not opened");
		}
		if (m_readOnly == true) {
			throw DSPException("Not opened for writing");
		}
		if (m_trailer != NULL) {
			delete[] m_trailer;
			m_trailer = NULL;
		}
		if (trailer != NULL) {
			// Allocate new trailer
			m_trailer = new char[strlen(trailer) + 1];
			if (m_trailer == NULL) {
				throw DSPMemoryException();
			}
			// Copy trailer
			strcpy(m_trailer, trailer);
		}
	}

	void wrrecs() {
		cout << "Test DSPFile class\r\n";
		try {

			// File to open
			DSPFile dspfile;

			// open file "output.dat" for writing
			dspfile.openWrite("output.dat");

			// Vector to write to the file
			vector<short int> dataOut(1000);

			// Fill vector with data
			for (int i = 0; i < dataOut.size(); i++) {
				dataOut[i] = abs(i - 500);
			}

			// write first record to file
			dspfile.write(dataOut);

			// change data in vector
			for (int i = 0; i < dataOut.size(); i++) {
				dataOut[i] = i + 1;
			}

			// write second record to file
			dspfile.write(dataOut);

			// close file
			dspfile.close();

			cout << "File \"output.data\" contains two records\n";
			cout << endl;

		}
		catch (exception& e) {
			// Display the 
			cerr << e.what() << endl;
		}

	}
	void rdrecs() {
		try {
			// File to open
			DSPFile dspfile;

			// Declare vector variable to load
			vector<short int> r1;
			vector<short int> r2;

			// open file "output.dat" for reading
			dspfile.openRead("output.dat");
			if (dspfile.getType() != SIGNED_SHORT) {
				throw DSPException("Wrong data type in file");
			}
			else {
				cout << "data file type OK\r\n";
			}

			// Read first and second records of file
			dspfile.read(r1);
			dspfile.read(r2);

			// Verify length
			cout << "\nr1 length: " << r1.size() << "\n";
			for (int i = 0; i < 10; i++) {
				cout << r1[i] << ", ";
			}
			cout << "\n";

			cout << "r2 length: " << r2.size() << "\n";
			for (int i = 0; i < 10; i++) {
				cout << r2[i] << ", ";
			}
			cout << "\n";


			cout << endl;

			if (r1.size() != 1000 || r2.size() != 1000) {
				throw DSPException("Wrong record length in data file");
			}

			dspfile.close();

			cout << "Read two records from file \"output.dat\"\n";
		}
		catch (exception& e) {
			cerr << e.what() << endl;
		}
	}
	void rdfrec() {
		try {
			DSPFile dspfile;
			string strName;

			// Get parameters from user
			do {
				getInput("Enter file name:", strName);
			} while (strName.empty());

			// Declar float vector to red
			vector<float> floatVec;
			// Open file for reading
			dspfile.openRead(strName.c_str());

			// Not that this file doesn't contain floats
			if (dspfile.getType() != FLOAT) {
				cout << "File \""
					<< strName
					<< "\" doesn't contain float data.\n";
			}
			// Read first record of file as a float vector
			// The read() member function converts the data to float
			dspfile.read(floatVec);

			// close file
			dspfile.close();

			// Display data in a column
			cout.flags(ios::showpoint);
			for (int i = 0; i < floatVec.size(); i++) {
				cout << i << " " << floatVec[i] << endl;
			}
		}
		catch (exception& e) {
			cerr << e.what() << endl;
		}
	}
	void rdtrail() {
		try {
			DSPFile dspfile;
			string strName;
			string strTrailer;

			do {
				getInput("Enter file name:", strName);
			} while (strName.empty());

			dspfile.openRead(strName.c_str());

			dspfile.getTrailer(strTrailer);

			dspfile.close();

			if (strTrailer.empty()) {
				cout << "No trailer in file \"" << strName << "\"\n";
			}
			else {
				cout << "Trailer:\n" << strTrailer << endl;
			}
		}
		catch (exception& e) {
			cerr << e.what() << endl;
		}
	}
	void wrtrail() {
		try {
			DSPFile dspfile;
			string strTrailer;

			dspfile.openWrite("output1.dat");

			vector<short int>dataOut(1000);

			// Fill vector with data
			for (int i = 0; i < dataOut.size(); i++) {
				dataOut[i] = abs(i - 500);
			}

			// Write first record to file
			dspfile.write(dataOut);

			// Change data in vector
			for (int i = 0; i < dataOut.size(); i++) {
				dataOut[i] = i;
			}

			// Write second record to file
			dspfile.write(dataOut);

			// create trailer
			strTrailer = "File \"output1.dat\" contains two records:\n";
			strTrailer += "First record is an inverted triangle\n";
			strTrailer += "Second record is a ramp\n";
			cout << strTrailer;

			// Set files trailer
			dspfile.setTrailer(strTrailer.c_str());

			// close file
			dspfile.close();
		}
		catch (exception &e) {
			cerr << e.what() << endl;
		}
	}
	void makeWave(vector<float>& vecWave, const vector<float>& vecFreq) {
		const double TWO_PI = 8.0 * atan(1.0);

		// For each frequency
		for (int i = 0; i < vecFreq.size(); i++) {
			// Add the sinusoid to the waveform
			double arg = TWO_PI * vecFreq[i];
			for (int j = 0; j < vecWave.size(); j++) {
				vecWave[j] += (float)cos(j * arg);
			}
		}
		// Normalize amplitude by the number of frequencies
		for (int i = 0; i < vecWave.size(); i++) {
			vecWave[i] /= vecFreq.size();
		}
	}
	void mkwave() {
		try {
			int samples = 0;
			getInput("Enter number of samples", samples, 2, 10000);

			vector<float> vecWave(samples);
			vecWave = { 0.0f };

			int freqs = 0;
			getInput("Enter number of frequencies", freqs, 1, 20);

			vector<float> vecFreqs(freqs);
			for (int i = 0; i < freqs; i++) {
				char format[80] = { 0 };
				sprintf(format, "Frequency %d", i);
				getInput(format, vecFreqs[i], 0.0f, 0.5f);
			}

			makeWave(vecWave, vecFreqs);

			// Get filename
			string strName;
			do {
				getInput("Enter file name to create", strName);
			} while (strName.empty());

			// Open file for writing
			DSPFile dspfile;
			dspfile.openWrite(strName.c_str());

			// Store data in DSPFile
			dspfile.write(vecWave);

			// Write trailer
			static char buf[300];
			sprintf(buf,
				"\nSignal of length %d equal to the sum of %d\n"
				"cosine waves at the following frequencies:\n",
				vecWave.size(),
				vecFreqs.size());
			string str = buf;
			for (int i = 0; i < vecFreqs.size(); i++) {
				sprintf(buf, "f/fs = %f\n", vecFreqs[i]);
				str += buf;
			}

			// Write trailer to display
			cout << str;

			// Add trailer to file
			dspfile.setTrailer(str.c_str());

			dspfile.close();
		}
		catch (exception& e) {
			cerr << e.what() << endl;
		}
	}
}
