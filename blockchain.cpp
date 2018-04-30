
/* 
 * File:   blockchain.cpp
 * Author: Jared Gentry
 *
 * Created on December 4, 2017, 10:30 AM
 * Completed on December 6, 2017 7:30 PM
 * 
 *
 */
#include "picosha2.h"
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>

using namespace std;

// functions from test.cpp
int hexCharToInt(char a){
    if(a>='0' && a<='9')
        return(a-48);
    else if(a>='A' && a<='Z')
        return(a-55);
    else
        return(a-87);
}

string hexToString(string str){
    stringstream HexString;
    for(int i=0;i<str.length();i++){
        char a = str.at(i++);
        char b = str.at(i);
        int x = hexCharToInt(a);
        int y = hexCharToInt(b);
        HexString << (char)((16*x)+y);
    }
    return HexString.str();
}

// function to find the merkle root
string findMerkleRoot(vector<string>);

// need struct for blocks on chain
struct block
{
	string prevHash;
	string merkRoot;
	string nonce;
};

int main (int argc, char **argv)
{ 
	/* need 3 arguments in command line */
	if(argc != 3){
		cerr << "Usage: " << argv[0] << " blockchain.txt" << " transaction.txt" << endl;
		exit(1);
	}

	ifstream blocksFile, transactionsFile;
	blocksFile.open(argv[1]);
	transactionsFile.open(argv[2]);

    if (blocksFile.fail()) {
        cout << "Error openning the file " << argv[1] << " \n";
        exit(1);
	}
	if (transactionsFile.fail()) {
        cout << "Error openning the file " << argv[2] << " \n";
        exit(1);
	}

    // import each block into a vector of block structs
    vector<block> blocks;
    string prev, merk, _nonce;
    while(!blocksFile.eof()){
    	block _block;
    	blocksFile >> prev >> merk >> _nonce;
		_block.prevHash = prev;
		_block.merkRoot = merk;
		_block.nonce = _nonce;
    	blocks.push_back(_block);
    }
    blocks.erase(blocks.end());

	blocksFile.close();

	// import each transaction into a vector of strings
	vector<string> trans;
	while(!transactionsFile.eof()){
		string t;
		getline(transactionsFile, t);
		trans.push_back(t);
	}
	trans.erase(trans.end());
	
	transactionsFile.close();

	// concatenate each block's fields and hash the block
	string concatBlock, hash;
	vector<string> hashedBlocks;
	for(int i=0;i<blocks.size();i++){
		concatBlock = blocks[i].prevHash + blocks[i].merkRoot + blocks[i].nonce;
		hash = picosha2::hash256_hex_string(hexToString(concatBlock));
		hashedBlocks.push_back(hash);
	}
	
  	// cout << "Hash1: " << hashedBlocks[0] << endl;
  	// cout << "Hash2: " << hashedBlocks[1] << endl;
  	// cout << "Hash3: " << hashedBlocks[2] << endl;

  	bool isValid = true;

  	// make sure each hash starts with 0
  	for(int i=0;i<hashedBlocks.size();i++){
  		string hash = hashedBlocks[i];
  		if(hash[0] != '0'){
  			cout << "Block " << i << " is not VALID!" << endl;
  			isValid = false;
  		}
  	}

  	// check to see if chain is broken if all blocks were valid
  	if(isValid){
  		for(int i=0;i<hashedBlocks.size() - 1;i++){
	  		string hash = hashedBlocks[i];
	  		string prev = blocks[i + 1].prevHash;
	  		if(hash != prev){
	  			int j = i + 1;
	  			cout << "Blocks " << i << " and " << j << " are not chained!" << endl;
	  			isValid = false;
	  		}
  		}
  	}
  

  	string merkleRoot = findMerkleRoot(trans);
  	cout << endl;
  	
  	if(isValid){ // do part 3.
  		cout << "Blockchain is VALID" << endl;
  		block next_block;
  		next_block.prevHash = hashedBlocks[hashedBlocks.size() - 1];
  		next_block.merkRoot = merkleRoot;
  		cout << next_block.prevHash << endl;
  		cout << next_block.merkRoot << endl;

  		// find nonce
  		for(unsigned long int i = 0; i < 100000000; i++)
		{
	  		ostringstream oss;
	  		// if I had time I would add a,b,c,d,e,f to nonce here
	  		
	  		oss << setw(8) << setfill('0') << i;
	 		string testNonce = oss.str();
	 	
	  		// check if you mined correct nonce
	  		next_block.nonce = testNonce;
	  		string concat = next_block.prevHash + next_block.merkRoot + next_block.nonce;
	  		string next_blockHash = picosha2::hash256_hex_string(hexToString(concat));
	  		if(next_blockHash[0] == '0'){
	  			cout << next_block.nonce << endl;
	  			cout << endl;
	  			//cout << "I've mined a new block!!!!" << endl;
	  			cout << next_blockHash << endl;
	  			
	  			break;
	  		}
		}

  	}else{ // do part 2.
  		cout << "Blockchain is INVALID" << endl; 
  		cout << merkleRoot << endl;
  	}
  	cout << endl;
    return 0;
}

// find merkle root
string findMerkleRoot(vector<string> trans){
	if(trans.size() == 1){
		string root = picosha2::hash256_hex_string(trans[0]);
		return root;
	}else{
		string hash1, hash2, concat;
		vector<string> parents;
		for(int i=0;i<trans.size();i+=2){
			hash1 = picosha2::hash256_hex_string(trans[i]);
			hash2 = picosha2::hash256_hex_string(trans[i+1]);
			concat = hash1 + hash2;
			parents.push_back(concat);
		}
		return findMerkleRoot(parents);
	}
	
}
