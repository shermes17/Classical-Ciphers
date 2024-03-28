/*
 Sean Hermes
 Homework 1
 CPSC 4820 004 Special Topic: Cryptography
 Dr. Zhang
 */



#include <iostream>
#include <stdlib.h>
#include <string>

using namespace std;

const char ALPHABET[26] = {'A','B','C','D','E','F','G','H','I','J','K','L','M',
                            'N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};

bool IorJ = false;
bool IinTable = false;
                    
/*******************  Function Definitions  *******************/
string vigenère_encrypt(string,string);
string vigenère_decrypt(string,string);
string playfair_encrypt(string, string);
string playfair_decrypt(string, string);

int letter_to_index(char);
void makeTable(string, char table[5][5]);
void printTable(char table[5][5]);
void playfair_plaintext_helper(string &plaintext);
void toUpper(string &str);
void toLower(string &str);
pair<int,int> getCoord(char c,char table[5][5]);
string prepPlaintext(string s);

// Helper functions
void toUpper(string &str){
    int size = (int)str.size();
    for (int i = 0; i < size;i++)
        str[i] = toupper(str[i]);

}
void toLower(string &str){
     int size = (int)str.size();
    for (int i = 0; i < size;i++)
        str[i] = tolower(str[i]);
}
int letter_to_index(char c){
    c = toupper(c);
    for (int i = 0; i < 26; i++){
        if(ALPHABET[i] == c)
            return i;
    }
    // character not in alphabet
    cout << "ERROR" << endl;
    exit(1);
}
string prepPlaintext(string s){
    string temp;
    for (int i = 0; i < (int)s.size();i++){
        if(s[i] != ' ' &&s[i] != '.' &&s[i] != ',' &&s[i] != '?' &&s[i] != '!')
            temp.push_back(s[i]);
    }

    return temp;
}

/*******************  Vigenère  *******************/
string vigenère_encrypt(string key, string plaintext){
    string ciphertext;
    plaintext = prepPlaintext(plaintext);
    int size = (int)plaintext.size();
    int keySize = (int)key.size();
    
    // convert everything to uppercase
    toUpper(key);
    toUpper(plaintext);
    
    // loop through each letter using Ci = (pi +ki) % 26 formula
    for(int i = 0; i < size; i++){
        int p_i = letter_to_index(plaintext[i]);
        int k_i = letter_to_index(key[i % keySize]);
        ciphertext.push_back(ALPHABET[(p_i + k_i) % 26]);
    }
    
    return ciphertext;
     
}

string vigenère_decrypt(string key, string ciphertext){
    string plaintext;
    
    int size = (int)ciphertext.size();
    int keySize = (int)key.size();
    
    // convert everything to uppercase
    toUpper(key);
    toUpper(ciphertext);
    
    // loop through each letter using Ci = (pi - ki + 26) % 26 formula
    for(int i = 0; i < size; i++){
        int c_i = letter_to_index(ciphertext[i]);
        int k_i = letter_to_index(key[i % keySize]);
        plaintext.push_back(ALPHABET[(c_i - k_i + 26) % 26]);
    }
    
    // convert plaintext to lowercase
    toLower(plaintext);
    return plaintext;
}

/*******************  Playfair  *******************/

void makeTable(string key, char table[5][5]){
    int keySize = (int)key.size();
    if (keySize > 26)
        exit(1);
    int keyIndex = 0; 
    int i = 0;
    
    // nested loop to build 5x5 table
    for (int r = 0; r < 5; r++){
        for (int c = 0; c < 5; c++){
            if (keyIndex < keySize){ // adding key to table
                table[r][c] = key[keyIndex];
                // Account for I/J in key
                if(key[keyIndex] == 'I'){
                    IorJ = true;
                    IinTable = true;
                }
                else if(key[keyIndex] == 'J'){
                     IorJ = true;
                    IinTable = false;
                }    
                keyIndex++;
            }
            else{
                if(key.find(ALPHABET[i]) == string::npos){
                    // Account for I/J outside of key
                    if((i == 8 && IorJ == true) || (i == 9 && IorJ == true)){
                        c--;
                    } 
                    else {
                        if(i == 8)
                            IinTable = true;
                        table[r][c] = ALPHABET[i];  
                    }
                        
                }
                else
                    c--;   
               i++;
            }
        }
    }
}

void printTable(char table[5][5]){
    for (int r = 0; r < 5; r++){
        cout << "|";
        for (int c = 0; c < 5; c++){
            cout << table[r][c] << "|";
        }
        cout << endl;
    }
}

void playfair_plaintext_helper(string &plaintext){
    int size = (int)plaintext.size();
    // insert X placeholder after each double letter pair
    for(int i = 0; i < size-1; i+=2){
        if(plaintext[i] == plaintext[i+1]){
            plaintext.insert(i+1,"X");
            size++;
        }
    }

    // make size even by adding placeholder
   if(size % 2 == 1){
        plaintext.push_back('X');
    }
}

pair<int,int> getCoord(char l, char table[5][5]){
    pair<int,int> p;
   
   // deal with I/J scenario
    if(l == 'I' || l == 'J'){
        if(IinTable == true){
            l = 'I';
        }
        else{
            l = 'J';
        }
    }
    // loop through table return the coordinates of current letter
    for (int r = 0; r < 5; r++){
        for (int c = 0; c < 5; c++){
            if(l == table[r][c]){
                p.first = r;
                p.second = c;
                return p;
             
            }
        }
    }
    // char not in table
    cout << "ERROR" << endl;
    exit(1);
}

string playfair_encrypt(string key, string plaintext){

    plaintext = prepPlaintext(plaintext);
    string ciphertext;
    char table[5][5];
    toUpper(key);
    toUpper(plaintext);
    makeTable(key, table);
    playfair_plaintext_helper(plaintext);

 
    int size = (int)plaintext.size();
    for (int i = 0; i < size; i += 2){
        // get coordinates of letter pair
        pair<int,int> c1 = getCoord(plaintext[i], table);
        pair<int,int> c2 = getCoord(plaintext[i+1], table);
        
        if(c1.first == c2.first){ // equal rows, move right
            c1.second = (c1.second + 1) % 5;
            c2.second = (c2.second + 1) % 5;
            ciphertext.push_back(table[c1.first][c1.second]);
            ciphertext.push_back(table[c2.first][c2.second]);
        }
        else if (c1.second == c2.second){ // equal columns, move down
            c1.first = (c1.first + 1) % 5;
            c2.first = (c2.first + 1) % 5;
            ciphertext.push_back(table[c1.first][c1.second]);
            ciphertext.push_back(table[c2.first][c2.second]);
        }
        else{ // different rows and columns
            ciphertext.push_back(table[c1.first][c2.second]);
            ciphertext.push_back(table[c2.first][c1.second]);
        }

    }
    return ciphertext;
}

string playfair_decrypt(string key, string ciphertext){
    string plaintext;
    char table[5][5];
    toUpper(key);
    toUpper(ciphertext);
    makeTable(key, table);


    int size = (int)ciphertext.size();
    for (int i = 0; i < size; i += 2){
         // get coordinates of letter pair
        pair<int,int> c1 = getCoord(ciphertext[i], table);
        pair<int,int> c2 = getCoord(ciphertext[i+1], table);
        
        if(c1.first == c2.first){ // equal rows, move left
            c1.second = (c1.second - 1);
            c2.second = (c2.second - 1);

            if (c1.second == -1){
                c1.second = 4;
            } 
            if (c2.second == -1){
                c2.second = 4;
            }
           
            char l1 = table[c1.first][c1.second];
            char l2 = table[c2.first][c2.second];
    

            plaintext.push_back(l1);
            plaintext.push_back(l2);
        }
        else if (c1.second == c2.second){ // equal columns, move up
            c1.first = (c1.first - 1) % 5;
            c2.first = (c2.first - 1) % 5;


            if (c1.first == -1){
                c1.first = 4;
            }    
            if (c2.first == -1){
                c2.first = 4;
            }

            plaintext.push_back(table[c1.first][c1.second]);
            plaintext.push_back(table[c2.first][c2.second]);
        }
        else{ // different rows and columns
            plaintext.push_back(table[c1.first][c2.second]);
            plaintext.push_back(table[c2.first][c1.second]);
        }

    }

    // this removes placeholder X's
    string temp;
    for(int i = 0; i < size; i++){
        if (plaintext[i] != 'X'){
            temp.push_back(plaintext[i]);
        }
    }
    plaintext = temp;

    //convert plaintext to lowercase
    toLower(plaintext);
    return plaintext;
}

int main(){
    
    string plaintext = "wewereonabreak";
    string key = "dinosaur";
    string Vigenère_ciphertext = "ZMJSJEIEDJESSK";
    string Playfair_ciphertext = "VFVFAGSOUCAGCE";
    
    cout << "Vigenère Encryption: " << vigenère_encrypt(key, plaintext) << endl;
    cout << "Vigenère Decryption: " << vigenère_decrypt(key, Vigenère_ciphertext) << endl;
    cout << "Playfair Encryption: " << playfair_encrypt(key, plaintext) << endl;
    cout << "Playfair Decryption: " << playfair_decrypt(key, Playfair_ciphertext) << endl;
      
    
    return 0;
}
