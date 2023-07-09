/*
    Title:      Tarea Convertir CSV y Buscar por RRN
    Purpose:    implement methods for file searching
    Author:     Josue De Jesus
    Date:       May 24, 2023
*/
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstring>
using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::ifstream;
using std::ofstream;
using std::vector;
using std::stringstream;
using std::setw;

class FieldData {
    private:
        string dataName;
        int dataLength;
        string dataType;
    public:
        FieldData() {}
        FieldData(string dataName, int dataLength, string dataType) {
            this->dataName = dataName;
            this->dataLength = dataLength;
            this->dataType = dataType;
        }
        ~FieldData() {}
        void setDataName(string dataName) {
            this->dataName = dataName;
        }
        string getDataName() {
            return this->dataName;
        }
        void setDataLength(int dataLength) {
            this->dataLength = dataLength;
        }
        int getDataLength() {
            return this->dataLength;
        }
        void setDataType(string dataType) {
            this->dataType = dataType;
        }
        string getDataType() {
            return this->dataType;
        }
};
/**
* Format Field
* Gets the field string 
* Returns the formated string to the length shown in the length file
*/
void formatField(std::string& field, int length) {
    field.resize(length, ' ');
}
/**
* Handle Quotes
* Gets a stringstream and trield to handle fields with quotes in order to get the data from each field correctly
* Returns the corrected field
*/
string handleQuotes(stringstream& ss) {
    string field;
    bool inQuotes = false;
    string quotedField;
    while (getline(ss, field, ',')) {
        field = field.substr(field.find_first_not_of(' '));
        field = field.substr(0, field.find_last_not_of(' ') + 1);
        /*size_t found = field.find("†");
        while (found != string::npos) {
            field = field.replace(found, 4, "...");
            found = field.find("†");
        }*/
        if (!inQuotes && !field.empty() && field.front() == '"') {
            inQuotes = true;
            if (field.length() > 1 && field[field.length() - 2] == '"') {
                quotedField = field.substr(1, field.length() - 3);
                inQuotes = false;
            } else {
                quotedField = field.substr(1);
            }
        } else if (inQuotes) {
            quotedField += "," + field;
            if (field.back() == '"' && !field.empty() && field[field.length() - 2] != '"') {
                inQuotes = false;
                return quotedField.substr(0, quotedField.length() - 1);
            }
        } else {
            return field;
        }
    }
    if (inQuotes) {
        /*size_t found = field.find("0x10");
        while (found != string::npos) {
            field = field.replace(found, 4, "...");
            found = field.find("†");
        }*/
        string remainingData;
        stringstream ss2(quotedField);
        while(getline(ss2, remainingData, ',')) {
        }
        quotedField = quotedField + '/';
        return quotedField;
    }
    return field;
}

/**
* Get Number of Records
* Gets the number of records in the file by counting each line
*/
int getNumberOfRecords(string fileName) {
    int counter = 0;
    ifstream inputFile(fileName, std::ios::binary);
    string line;
    while (getline(inputFile, line, '\n')) {
        counter++;
    }
    inputFile.close();
    return counter;
}
/**
* Get the of the File
* Opens the file loops thought the file to get the number of bytes it contains 
*/
int getFileSize(string fileName) {
    ifstream inputFile(fileName, std::ios::binary);
    inputFile.seekg(0, std::ios::end);
    int fileSize = inputFile.tellg();
    inputFile.seekg(0, std::ios::beg);
    return fileSize;
}
/**
* Trim Field
* Receives data and removes the empty space after the last character
*/
void trimField(char* field, int length) {
    int end = length - 1;
    while (end >= 0 && field[end] == ' ') {
        field[end] = '\0';
        end--;
    }
}

/**
* Get Field Data
* Opens the file and gets the data for each field and stores it inside a vector
*/
vector<FieldData*> getFieldsData(string lengthFileName) {
    ifstream inputFile(lengthFileName, std::ios::binary);
    vector<FieldData*> dataVec;
    string line, field;
    while (getline(inputFile, line, '\n')) {
        stringstream lineSS(line);
        string data1, data2, data3, data4;
        string dataName, dataLength, dataType;
        string result;
        lineSS >> data1 >> data2 >> data3;
        dataName = data1;
        dataLength = data2;
        dataType = data3;
        if (lineSS >> data4) {
            dataName = data1 + data2;
            dataLength = data3;
            dataType = data4;
        }
         else {
            dataName = data1;
            dataLength = data2;
            dataType = data3;
        }
        if (dataType.front() == '1') {
            data3 = dataType.substr(0, dataType.length() - 4);
            data4 = dataType.substr(3);
            dataLength = data3;
            dataType = data4;
        }
        dataVec.push_back(new FieldData(dataName, std::stoi(dataLength), dataType));
    }
    return dataVec;
}

int main(int argc, char* argv[]){
    vector<FieldData*> lengthVector; 
    for (int i = 0; i < argc; ++i) {
        cout << argv[i] << endl;
    }
    if (argc != 4) {
        cerr << "Error: De be ingresar las banderas que se piden." << endl;
        cerr << "El directorio del archivo que quiere abrir y la accion que desea ejecutar" << endl;
        cerr << "Ejemplo: ./data/superstore.csv -convert length-file.txt" << endl;
        getFieldsData("length-file.txt");
    }

    string fileName = argv[1];
    string flag = argv[2];
    string lengthFileName = argv[3];
    if (flag == "-convert") {
        string newFile = fileName.substr(7);
        newFile = newFile.substr(0, newFile.length() - 4) + ".fixed";
        ifstream inputLengthFile(lengthFileName);
        ifstream inputFile(fileName, std::ios::in | std::ios::binary);
        string line;
        int numberOfRecords = getNumberOfRecords(fileName);
        if (inputLengthFile.fail() || inputFile.fail()) {
            cerr << "Error: No se encontro el archivo especificado" << endl;
            return 0;
        }
        lengthVector = getFieldsData(lengthFileName);
        ofstream outputFile(newFile, std::ios::out | std::ios::binary);
        for (int i = 0; i < numberOfRecords; i++) {
            bool isInput = false;
            getline(inputFile, line, '\n');
            stringstream ss(line);
            int counter = 0;
            for (int i = 0; i <= 20; i++) {
                string field = handleQuotes(ss);
                if (field.back() == '/') {
                    isInput = true;
                    vector<string> quoteData;
                    string remainingData = field;
                    remainingData = remainingData.substr(0, remainingData.length() - 1);
                    string nextData;
                    stringstream ssQuotedField(remainingData);
                    while(getline(ssQuotedField, field, ',')) {
                        if (field.back() != '"') {
                            quoteData.push_back(field);
                            field = "";
                        } else if (field.back() == '"') {
                            quoteData.push_back(field);
                            field = "";
                        } else if (field.back() == ' '){
                            field = field;
                            i++;
                        }
                    }
                    for (size_t i = 0; i < quoteData.size() - 4; i++) {
                        field = field + quoteData[i] + ",";
                    }
                    field = field.substr(0, field.length() - 2);
                    formatField(field, lengthVector[counter]->getDataLength());
                    //outputFile << std::noskipws << field << " ";
                    outputFile.write(field.c_str(), lengthVector[counter]->getDataLength());
                    outputFile.write(" ", 1);
                    counter++;
                    field = "";
                    for (size_t i = quoteData.size() - 4; i < quoteData.size(); i++) {
                        field = field + quoteData[i] + ",";
                    }
                    field = field.substr(0, field.length() - 1);
                    stringstream remainingSS(field);
                    while(getline(remainingSS, nextData, ',')) {
                        formatField(nextData, lengthVector[counter]->getDataLength());
                        //outputFile << std::noskipws << nextData << " ";
                        outputFile.write(nextData.c_str(), lengthVector[counter]->getDataLength());
                        outputFile.write(" ", 1);
                        counter++;
                    }
                } else{
                    if (isInput == false) {
                        formatField(field, lengthVector[i]->getDataLength());
                        //outputFile << std::noskipws << field << " ";
                        outputFile.write(field.c_str(), lengthVector[counter]->getDataLength());
                        outputFile.write(" ", 1);
                        counter++;
                    }
                }
            }
            //outputFile << std::noskipws << endl;
            outputFile.write("\n", 1);
            //outputFile.close();
        }
    } else if (flag == "-retrieve"){
        lengthVector = getFieldsData("length-file.txt");
        ifstream inputFile(fileName, std::ios::in | std::ios::binary);
        stringstream ss(lengthFileName);
        int RRN;
        ss >> RRN;
        if (inputFile.fail()) {
            cerr << "Error: No se encontro el archivo espicificado" << endl;
            return 0;
        }
        else if (!ss.eof()) {
            cerr << "Error: El dato que ingreso es incorrecto." << endl;
            cerr << "Ingrese el RRN del registro que desea obtener" << endl;
            return 0;
        } else {
            if (RRN >= getNumberOfRecords(fileName)) {
                cerr << "Error: El RRN ingresado esta fuera de limites." << endl;
                return 0;
            } else {
                int recordSize = getFileSize(fileName) / getNumberOfRecords(fileName);
                int offset = recordSize * (RRN - 1) + recordSize;
                inputFile.seekg(offset);
                for (size_t i = 0; i < lengthVector.size(); i++) {
                int length = lengthVector[i]->getDataLength();
                inputFile.seekg(offset);
                char field[length];
                inputFile.read(field, length + 1);
                trimField(field, length + 1);
                cout << std::left << std::setfill(' ') << setw(14) << lengthVector[i]->getDataName();
                std::wcout << field << endl;
                offset = offset + length + 1;
                }
            }
        }
        
    } else {
        cerr << "Error: Instruccion incorrecta." << endl;
        cerr << "Debe ser -convert o -retrieve." << endl;
    }
    return 0;
}