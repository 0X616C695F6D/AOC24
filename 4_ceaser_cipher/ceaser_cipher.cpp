#include <stdlib.h>
#include <iostream>
#include <fstream>

/*
 * Ceaser Cipher (ROTX) encryption and decryption.
 */

using namespace std;

/*
 * Function to encrypt or decrypt a given string.
 * 
 * Inputs:
 * plain -> text to transform
 * key   -> rotation amount
 * crypt -> transform type
 */
string crypt(string& plain, int key, string& crypt) {
	string text = plain;
	if (crypt == "enc") {
		for (int i = 0; i < (int)plain.size(); i++) {
			if (text[i] != ' ') {
				text[i] = int(plain[i]) + key;
			}
		}
	} else {
		for (int i = 0; i < (int)plain.size(); i++) {
			if (text[i] != ' ') {
				text[i] = int(plain[i]) - key;
			}
		}
	}

	return text;
}

int main (int argc, char* argv[]) {
	if (argc != 4) {
		cout << "Usage: " << argv[0] << " <key> <enc/dec> <file>" << endl;
		cout << "Example: " << argv[0] << " 3 enc plain.txt" << endl;
	}

	int key = atoi(argv[1]);
	string transform = argv[2];
	if (transform != "enc" && transform != "dec") {
		cerr << "Incorrect transform type\n";
		return 1;
	}

	ifstream f(argv[3]);
	if (!f.is_open()) {
		cerr << "Cant open file " << argv[3] << "\n";
		return 1;
	}

	string plaintext;
	string text;
	while (getline(f, plaintext)) {
		cout << "p: " << plaintext << endl;
		text = crypt(plaintext, key, transform);
		cout << "c: " << text << "\n" << endl;
	}

	return 0;
}
