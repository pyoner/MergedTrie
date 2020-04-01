#include <iostream>
#include <string>
#include <stdio.h>

#include "MergedTrie.h"

using namespace std;


int main() {
	// MergedTrie that stores words with no associated data
	// If kB_compressed = true, then in the compaction operation, the size of the Hash Tables is adjusted to the size of the Table. Otherwise, it is set to a power of two in order to improve the temporal efficiency
	// kSizeOfTheArrayOfLN, kSizeOfTheArrayOfELN and kSizeOfTheArrayOfIT are the sizes of the arrays used in the MergedTrie. By default the sizes are set to 113000000, 12000000 and 260000000 respectively
	//	MergedTrie(const bool& kB_compressed = true, const _INDICE_NMTRIE& kSizeOfTheArrayOfLN = __Tam_tamBNL__, const _INDICE_NMTRIE& kSizeOfTheArrayOfELN = __Tam_tamBChar__, const _INDICE_NMTRIE& kSizeOfTheArrayOfIT = __Tam_tamBNIT__);
	MergedTrie trie2;

	// MergedTrie that stores words with associated data
	// kSizeOfTheArrayOfData is the initial size of the array that stores the data of the MergedTrie
	// If kB_compressed = true, then in the compaction operation, the size of the Hash Tables is adjusted to the size of the Table. Otherwise, it is set to a power of two in order to improve the temporal efficiency
	// kSizeOfTheArrayOfLN, kSizeOfTheArrayOfELN and kSizeOfTheArrayOfIT are the sizes of the arrays used in the MergedTrie. By default the sizes are set to 113000000, 12000000 and 260000000 respectively
	//	D_MergedTrie(const _INDICE_NMTRIE& kSizeOfTheArrayOfData = __Tam_tamBNL__, const bool& kB_compressed = true, const _INDICE_NMTRIE& kSizeOfTheArrayOfLN = __Tam_tamBNL__, const _INDICE_NMTRIE& kSizeOfTheArrayOfELN = __Tam_tamBChar__, const _INDICE_NMTRIE& kSizeOfTheArrayOfIT = __Tam_tamBNIT__): MergedTrie(kB_compressed, kSizeOfTheArrayOfLN, kSizeOfTheArrayOfELN, kSizeOfTheArrayOfIT, true);
	D_MergedTrie<char> trie1;

	string word;

	cout << "\n\nExample of the usage of the MergedTrie. Please quote it as:" << endl << "\tFerrandez A, Peral J (2019) MergedTrie: Efficient textual indexing. PLoS ONE 14(4): e0215288. https://doi.org/10.1371/journal.pone.0215288" << endl << endl;

	///////////////////////////////////// MergedTrie WITH NO DATA

	///////////////////////////////////// INSERTION
	word = "hello";
	if(trie2.insert(word))
		cout << "++++ word: " << word << " inserted" << endl;
	else
		cout << "******** ERROR: word: " << word << " no inserted" << endl;
	word = "helpful";
	if(trie2.insert(word))
		cout << "++++ word: " << word << " inserted" << endl;
	else
		cout << "******** ERROR: word: " << word << " no inserted" << endl;
	word = "handle";
	if(trie2.insert(word))
		cout << "++++ word: " << word << " inserted" << endl;
	else
		cout << "******** ERROR: word: " << word << " no inserted" << endl;

	///////////////////////////////////// FIND OPERATION
	word = "hello";
	if(trie2.find(word))
		cout << "++++ word: " << word << " is stored in the MergedTrie" << endl;
	else
		cout << "******** ERROR: word: " << word << " no stored" << endl;
	word = "hellooo";
	if(trie2.find(word))
		cout << "++++ word: " << word << " is stored in the MergedTrie" << endl;
	else
		cout << "******** ERROR: word: " << word << " no stored" << endl;

	///////////////////////////////////// PRINTING THE WORDS AND STATISTICS
	cout << "\nMergedTrie trie2 with no data: " << trie2 << endl;

	//	void print(ostream& os, const bool& print_words = true, const bool& print_statistics = true, const bool& print_MergedTrie_nodes = false) const;
	cout << "\nMergedTrie Words: " << endl; trie2.print(cout, true, false, false); cout << endl;

	///////////////////////////////////// PRINTING THE WORDS WITH A PREFIX
	int maxPrefijos = 99999;
	string prefix;
	// prefix_print returns the number of words (until a maximum number of maxPrefixExtracted) stored in the MergedTrie que share the string "prefix"
	//	It writes them in "ostream os"
	//	int prefix_print(const string& prefix, ostream& os, const int& maxPrefixExtracted = 999999999);
	prefix = "he";
	cout << "Printing the stored words with the prefix: \"" << prefix << "\":" << endl;
	cout << "\nThere are " << trie2.prefix_print(prefix, cout, maxPrefijos) << " words with prefix: \"" << prefix << "\":" << endl;

	///////////////////////////////////// COMPACTION OPERATION
	// It compress the MergedTrie, eliminating the moved Nodes
	trie2.compaction();
	cout << "\nCompacted MergedTrie: " << trie2 << endl;

	///////////////////////////////////// SERIALIZATION OF THE MergedTrie
	// The "directoryToStoreTheMergedTrie" must be created previously
	trie2.store_in_directory("./directoryToStoreTheMergedTrie");

	trie2.read_from_directory("./directoryToStoreTheMergedTrie");
	cout << "\nMergedTrie restored from the directory: " << trie2 << endl;

	////////////////////////////////////////////////////////////////////////// MergedTrie WITH DATA

	////////////////////////////////////////////////////////////////////////// INSERTION
	char data;
	word = "rubber"; data = 'a';
	if(trie1.insert(word, data))
		cout << "++++ word: " << word << " inserted with data: " << data << endl;
	else
		cout << "******** ERROR: word: " << word << " no inserted" << endl;
	word = "roller"; data = 'b';
	if(trie1.insert(word, data))
		cout << "++++ word: " << word << " inserted with data: " << data << endl;
	else
		cout << "******** ERROR: word: " << word << " no inserted" << endl;
	word = "rubber"; data = 'c';
	if(trie1.insert(word, data))
		cout << "++++ word: " << word << " inserted with data: " << data << endl;
	else
		cout << "******** ERROR: word: " << word << " no inserted" << endl;
	word = "roller"; data = 'b';
	if(trie1.insert(word, data))
		cout << "++++ word: " << word << " inserted with data: " << data << endl;
	else
		cout << "******** ERROR: word: " << word << " no inserted with data: " << data << " (it was already inserted)" << endl;

	///////////////////////////////////// FIND OPERATION
	word = "rubber";
	if(trie1.find(word, data))
		cout << "++++ word: " << word << " is stored in the MergedTrie with data: " << data << endl;
	else
		cout << "******** ERROR: word: " << word << " no stored" << endl;
	word = "roller";
	if(trie1.find(word, data))
		cout << "++++ word: " << word << " is stored in the MergedTrie with data: " << data << endl;
	else
		cout << "******** ERROR: word: " << word << " no stored" << endl;

	///////////////////////////////////// PRINTING THE NUMBER OF STORED WORDS
	cout << "\nMergedTrie with data with " << trie1.size() << " words" << endl;

	///////////////////////////////////// PRINTING THE WORDS AND STATISTICS
	cout << "\nMergedTrie trie1 with data: " << trie1 << endl;

}
