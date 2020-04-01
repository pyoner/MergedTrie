/*
 * MergedTrie.cpp
 *
 *  Created on: 07/04/2015
 *      Author: Antonio
 */

#include "MergedTrie.h"
#include "MergedTrie_settings.h"


/////////// Variables calculo estadisticas que se actualizaran en las rutinas de imprimir
_INDICE_NMTRIE estadistNumNP, estadistNPmov, estadistNLvacio, estadistNLcolis, estadistNITcolis, estadistNITvacio, estadistNL, estadistNIT, estadistNTHNL, estadistNTHNIT;
_INDICE_NMTRIE estadistBNLmax, estadistBNITmax, estadistNLS;

bool imprimirNodos;	// Si esta a true se imprimira el contenido de los nodos en las rutinas de imprimir. Si es false solo se calcularan las estadisticias
bool imprimirPalabras;	// Si esta a true se imprimiran las palabras almacenadas en el trie al recorrer bloqueNL en las rutinas de imprimir, almacenando IT1 e IT2 en las siguientes dos variables
_INDICE_NMTRIE imprimirPalabrasIT1, imprimirPalabrasIT2;	// IT1 e IT2 de la palabra obtenida al estar activado imprimirPalabras
bool esNLS_IT1;	// Si IT1 en imprimirPalabrasIT1 esNLS
bool crearPilaNL = false;	// Si esta a true se ira creando la colaNL
stack<_INDICE_NMTRIE> colaNL;	// Cola para almacenar los NL que quedan pendientes por recorrer




datoCola::datoCola() {
	indiceSinComprimir = indiceNpadreComprimido = indiceComprimido = 0;
	letra = 0;
}

datoCola::datoCola(const _INDICE_NMTRIE& kindiceSinComprimir, const _INDICE_NMTRIE& kindiceNpadreComprimido, const _INDICE_NMTRIE& kindiceComprimido, const _INDICE_NMTRIE& kletra) {
	indiceSinComprimir = kindiceSinComprimir;
	indiceNpadreComprimido = kindiceNpadreComprimido;
	indiceComprimido = kindiceComprimido;
	letra = kletra;
}



datoNodo::datoNodo() {
	clear();
}

void datoNodo::clear() {
	clave = indiceNodoSinComprimir = indiceComprimido = posHash = 0;
}

void datoNodo::clearPosHash() {
	posHash = 0;
}

bool datoNodo::operator<(datoNodo b) const {
	if(posHash == b.posHash)
		return indiceNodoSinComprimir > b.indiceNodoSinComprimir;
	else
		return posHash < b.posHash;
}

void datoNodo::ImprimirLeyenda() {
	cout << "(clave, indiceNodoSinComprimir, indiceComprimido, posHash)";
}

void datoNodo::Imprimir(const bool& esLetra) {
	cout << "( {" << clave;
	if(esLetra)
		cout << " " << (char) clave;
	cout << "}, " << indiceNodoSinComprimir << ", " << indiceComprimido << ", " << posHash << ")";
}




datoNodoIT::datoNodoIT() {
	clear();
}

void datoNodoIT::clear() {
	indNLconTHNIT_comprimido = indTHNIT_noComprimido = 0;
	BNIT_noComprimido = 0;
}



nodoBloqueNL::nodoBloqueNL() {
	todo.nodo = 0;
}

nodoBloqueNL::nodoBloqueNL(const nodoBloqueNL& n) {
	todo.nodo = n.todo.nodo;
}

nodoBloqueNL& nodoBloqueNL::operator=(const nodoBloqueNL& n) {
	todo.nodo = n.todo.nodo;
	return *this;
}

nodoBloqueNL& nodoBloqueNL::operator=(const unsigned int& n) {
	todo.nodo = n;
	return *this;
}

bool nodoBloqueNL::operator==(const unsigned int& n) {
	return todo.nodo == n;
}

bool nodoBloqueNL::operator!=(const unsigned int& n) {
	return todo.nodo != n;
}

bool nodoBloqueNL::operator==(const nodoBloqueNL& n) {
	return todo.nodo == n.todo.nodo;
}

bool nodoBloqueNL::operator!=(const nodoBloqueNL& n) {
	return todo.nodo != n.todo.nodo;
}

bool nodoBloqueNL::EsVacio() {
	return todo.nodo == 0;
}

void nodoBloqueNL::clear() {
	todo.nodo = 0;
}



ostream& operator<<(ostream &os, const MergedTrie &n){
    n.print(os, false, true, false);

    return os;
}




// If kB_compressed = true, then in the compaction operation, the size of the Hash Tables is adjusted to the size of the Table. Otherwise, it is set to a power of two in order to improve the temporal efficiency
// kSizeOfTheArrayOfLN, kSizeOfTheArrayOfELN and kSizeOfTheArrayOfIT are the sizes of the arrays used in the MergedTrie. By default the sizes are set to 113000000, 12000000 and 260000000 respectively
MergedTrie::MergedTrie(const bool& kB_compressed, const _INDICE_NMTRIE& kSizeOfTheArrayOfLN, const _INDICE_NMTRIE& kSizeOfTheArrayOfELN, const _INDICE_NMTRIE& kSizeOfTheArrayOfIT) {
	InicializarArrays(kB_compressed, kSizeOfTheArrayOfLN, kSizeOfTheArrayOfELN, kSizeOfTheArrayOfIT, false);
}

// If kB_compressed = true, then in the compaction operation, the size of the Hash Tables is adjusted to the size of the Table. Otherwise, it is set to a power of two in order to improve the temporal efficiency
// kSizeOfTheArrayOfLN, kSizeOfTheArrayOfELN and kSizeOfTheArrayOfIT are the sizes of the arrays used in the MergedTrie. By default the sizes are set to 113000000, 12000000 and 260000000 respectively
MergedTrie::MergedTrie(const bool& kB_compressed, const _INDICE_NMTRIE& kSizeOfTheArrayOfLN, const _INDICE_NMTRIE& kSizeOfTheArrayOfELN, const _INDICE_NMTRIE& kSizeOfTheArrayOfIT, const bool& kdataMT) {
	InicializarArrays(kB_compressed, kSizeOfTheArrayOfLN, kSizeOfTheArrayOfELN, kSizeOfTheArrayOfIT, kdataMT);
}

void MergedTrie::InicializarArrays(const bool& kB_compressed, const _INDICE_NMTRIE& kSizeOfTheArrayOfLN, const _INDICE_NMTRIE& kSizeOfTheArrayOfELN, const _INDICE_NMTRIE& kSizeOfTheArrayOfIT, const bool& kdataMT) {
	//		cout << "entra en constructor void"<< endl;
	dataMT = kdataMT;

	tablaAmpliada = false;
	B_compressed = kB_compressed;

	tamBloqueNL = kSizeOfTheArrayOfLN;
	tamBloqueNIT = kSizeOfTheArrayOfIT;
	tamBloqueNChar = kSizeOfTheArrayOfELN;

	cant=0;
	bloqueNL = new _NMTRIE_ [tamBloqueNL];
	bloqueNIT = new _NIT_ [tamBloqueNIT];
	bloqueNChar = new unsigned char [tamBloqueNChar];

	if(dataMT) {
		bloqueDataNIT = new unsigned int [tamBloqueNIT];
		memset(bloqueDataNIT, 0, sizeof(unsigned int) * tamBloqueNIT);
		bloqueDataNIT_nuevo = NULL;
	}

	bloqueNL_nuevo = NULL;	// Para evitar que entre en temas de comprimir
	bloqueNIT_nuevo = NULL;
	bloqueNChar_nuevo = NULL;

	numNL = numNIT = numNChar = 0;

	memset(bloqueNL, 0, sizeof(_NMTRIE_) * tamBloqueNL);
	memset(bloqueNIT, 0, sizeof(_NIT_) * tamBloqueNIT);
	memset(bloqueNChar, ' ', sizeof(unsigned char) * tamBloqueNChar);

	int i;
	//		cout << "tamBloqueNL: " << endl;
	//		for(i=0; i < tamBloqueNL; ++i)
	//			cout << bloqueNL[i] << " ";
	//		cout << endl;
	//		cout << "tamBNL_IT: " << endl;
	//		for(i=0; i < tamBNL_IT; ++i)
	//			cout << bloqueNL_IT[i] << " ";
	//		cout << endl;

	numBitsEOW = __NUM_MAX_BITRIES__;
	estaComprimido = false;

	Mascaras();

	InicializarTrie();

#define _IND_NPFIJO_RAIZ 2

	busqInsertar = false;

	//    	cout << "Despues de crear los nodos raiz: "; print(cout); cout << endl;
	//    	cout << "numNL: " << numNL << " tamBloqueNL: " << tamBloqueNL << endl;
}

// The MergedTrie is initialized from the one stored in "directory"
MergedTrie::MergedTrie(const string& directory) {
	cant=0;
	read_from_directory(directory, false, false);
	busqInsertar = false;
	tablaAmpliada = false;

	Mascaras();
}

MergedTrie::MergedTrie(const MergedTrie& n) {
	tamBloqueNL = n.tamBloqueNL;
	tamBloqueNIT = n.tamBloqueNIT;
	tamBloqueNChar = n.tamBloqueNChar;

	bloqueNL = new _NMTRIE_ [tamBloqueNL];
	bloqueNChar = new unsigned char [tamBloqueNChar];
	bloqueNIT = new _NIT_ [tamBloqueNIT];

	Copia(n);

	Mascaras();
	busqInsertar = false;
	tablaAmpliada = false;
}

void MergedTrie::Copia(const MergedTrie& t) {
	if(tamBloqueNL != t.tamBloqueNL) {
		delete [] bloqueNL;
		bloqueNL = new _NMTRIE_ [t.tamBloqueNL];
		delete [] bloqueNChar;
		bloqueNChar = new unsigned char [t.tamBloqueNChar];
		delete [] bloqueNIT;
		bloqueNIT = new _NIT_ [t.tamBloqueNIT];
	}
	tamBloqueNL = t.tamBloqueNL;
	tamBloqueNIT = t.tamBloqueNIT;
	tamBloqueNChar = t.tamBloqueNChar;

	numNL = t.numNL;
	numNIT = t.numNIT;
	numNChar = t.numNChar;

	B_compressed = t.B_compressed;

	cant = t.cant;
	numBitsEOW = t.numBitsEOW;
	estaComprimido = t.estaComprimido;
	memcpy(bloqueNL, t.bloqueNL, sizeof(_NMTRIE_) * tamBloqueNL);
	memcpy(bloqueNChar, t.bloqueNChar, sizeof(unsigned char) * tamBloqueNChar);
	memcpy(bloqueNIT, t.bloqueNIT, sizeof(_NIT_) * tamBloqueNIT);
}

MergedTrie& MergedTrie::operator= (const MergedTrie& t) {
	if(this!=&t)
	{
		clear();
		// Hacemos la copia del objeto
		Copia(t);
	}
	return *this;
}

// Empty the MergedTrie without deleting its memory
void MergedTrie::clear() {
	cant = 0;
	numNL = numNIT = numNChar = 0;

	// NO se libera la memoria del bloqueNL. Solo se inicializa a cero
	memset(bloqueNL, 0, sizeof(_NMTRIE_) * tamBloqueNL);
	memset(bloqueNChar, 0, sizeof(unsigned char) * tamBloqueNChar);
	memset(bloqueNIT, 0, sizeof(_NIT_) * tamBloqueNIT);
}

MergedTrie::~MergedTrie() {
	//		cout << "entra en destructor"<< endl;
	cant = 0;
	numNL = numNIT = numNChar = 0;

	delete [] bloqueNL;
	bloqueNL = NULL;
	delete [] bloqueNChar;
	bloqueNChar = NULL;
	delete [] bloqueNIT;
	bloqueNIT = NULL;

	delete [] arrayModulo;
	arrayModulo = NULL;
}

_INDICE_NMTRIE MergedTrie::DevolverIndiceActual() {
	return busqIndiceActual;
}

_INDICE_NMTRIE MergedTrie::DevolverIndiceDatoActual() {
	return bloqueDataNIT[busqIndiceActual];
}

// Crear los primeros nodos del Trie, inicializandolos adecuadamente
// if(bloqueNL_nuevo != NULL) entonces es que se llama desde AgruparNodosAux, con lo que no se creara ni el NP ni la THNL de la raiz
void MergedTrie::InicializarTrie() {
	_NMTRIE_ *ptemp;
	_INDICE_NMTRIE indiceTemp;

	// Creacion del primer nodoIT
	GetNuevoNIT(indiceTemp);	// Este se deshecha para que IT1 siempre sea distinto de 0 y no haya ambiguedad en la deteccion de IT1 vacio

	unsigned char *ptempChar;
	_INDICE_NMTRIE indiceTempChar;
	// Creacion del primer nodoChar
	GetNuevoNChar(ptempChar, indiceTempChar);	// Este se deshecha para que dirChar siempre sea distinto de 0 y no haya ambiguedad en la deteccion de dirChar vacio

	// Creacion del primer nodo de trie1
#ifdef __MODO_SEGURO__
	SetNP(bloqueNL[0]);
#endif
	GetNuevoNL(ptemp, raizTrie1);	// El primero se deshecha para que IT2 siempre sea distinto de 0 y no haya ambiguedad en la deteccion de IT2 vacio
	if(bloqueNL_nuevo != NULL)
		GetNuevoNL(ptemp, raizTrie1, 1);	// La raiz del trie: NL (el NP y la THNL se crearan posteriormente en AgruparNodosAux)
	else
		GetNuevoNL(ptemp, raizTrie1, 1 + 1 + __B_INICIAL_NL_RAIZ__);	// La raiz del trie: NL + NP + THNL

	SetPrim_Vacio(*ptemp, 2);
	SetLetra(*ptemp, '#');
	if(bloqueNL_nuevo == NULL) {
		SetBNL_Vacio(*ptemp, __B_INICIAL_NL_RAIZ__);
		SetBNLpot2(*ptemp);

		// Inicializacion del NP de la raiz
		++ptemp;
#ifdef __MODO_SEGURO__
		SetNP(*ptemp);
#endif
	}
}

void MergedTrie::SetDataMT(const _INDICE_NMTRIE& indice, const _INDICE_NMTRIE& indiceData) {
	bloqueDataNIT[indice] = indiceData;
}

_NMTRIE_* MergedTrie::GetRaiz() {
	return bloqueNL;
}

void MergedTrie::Mascaras() {
	_INDICE_NMTRIE i, j, modulo = 1;

	estadistNLvacio = estadistNITvacio = estadistNL = estadistNIT = estadistBNLmax = estadistBNITmax = estadistNTHNL = estadistNTHNIT = 0;
	imprimirNodos = true;
	imprimirPalabras = false;

	arrayC[0] = 104681; arrayC[1] = 104683 ; arrayC[2] = 104693; arrayC[3] = 104701; arrayC[4] = 104707; arrayC[5] = 104711; arrayC[6] = 104717; arrayC[7] = 104723; arrayC[8] = 104729;
	arrayC[9] = 104743; arrayC[10] = 104759; arrayC[11] = 104761; arrayC[12] = 104773; arrayC[13] = 104779; arrayC[14] = 104789; arrayC[15] = 104801; arrayC[16] = 104803; arrayC[17] = 104827;
	arrayC[18] = 104831; arrayC[19] = 104849;

	numBitsIdEOW = ceil(log(numBitsEOW)/log(2));

	//////////////////// NP
	mascaraBitNP = (1 << 31);

#ifdef __MODO_SEGURO__
	maxIndNPfijo = (1 << 27) - 1;
#else
	maxIndNPfijo = (1 << 28) - 1;
#endif
	mascaraIndNPfijo = maxIndNPfijo;
	mascaraIndNPfijo_Acero = ~mascaraIndNPfijo;

	maxIndNLPadre = (1 << 28) - 1;
	mascaraIndNLPadre = maxIndNLPadre;
	mascaraIndNLPadre_Acero = ~mascaraIndNLPadre;

#ifdef __MODO_SEGURO__
	maxBNIT = (1 << 15) - 1;
#else
	maxBNIT = (1 << 16) - 1;
#endif

	maxBNITp3 = (1 << 4) - 1;
#ifdef __MODO_SEGURO__
	mascaraBNITp3 = (maxBNITp3 << 27);
#else
	mascaraBNITp3 = (maxBNITp3 << 28);
#endif
	mascaraBNITp3_Acero = ~mascaraBNITp3;

	maxBNITp4 = (1 << 4) - 1;
	mascaraBNITp4 = (maxBNITp4 << 28);
	mascaraBNITp4_Acero = ~mascaraBNITp4;

#ifdef __MODO_SEGURO__
	maxIndTHNIT = (1 << 27) - 1;
#else
	maxIndTHNIT = (1 << 28) - 1;
#endif
	mascaraIndTHNIT = maxIndTHNIT;
	mascaraIndTHNIT_Acero = ~mascaraIndTHNIT;

	maxIndNLS = UINT_MAX;
	mascaraIndNLS = maxIndNLS;
	mascaraIndNLS_Acero = ~mascaraIndNLS;

	//////////////////// NL
	mascaraBitNL = (1 << 7);

#ifdef __MODO_SEGURO__
	mascaraBitNLS = (1 << 6);
#else
	mascaraBitNLS = (1 << 7);
#endif
	mascaraBitNLS_Acero = ~mascaraBitNLS;

#ifdef __MODO_SEGURO__
	mascaraBitNPmov = (1 << 5);
#else
	mascaraBitNPmov = (1 << 6);
#endif
	mascaraBitNPmov_Acero = ~mascaraBitNPmov;

#ifdef __MODO_SEGURO__
	mascaraBitBNLpot2= (1 << 4);
#else
	mascaraBitBNLpot2= (1 << 5);
#endif
	mascaraBitBNLpot2_Acero = ~mascaraBitBNLpot2;

#ifdef __MODO_SEGURO__
	mascaraBitBNITpot2= (1 << 3);
#else
	mascaraBitBNITpot2= (1 << 4);
#endif
	mascaraBitBNITpot2_Acero = ~mascaraBitBNITpot2;

#ifdef __MODO_SEGURO__
	maxBNITp1 = (1 << 3) - 1;
#else
	maxBNITp1 = (1 << 4) - 1;
#endif
	mascaraBNITp1 = maxBNITp1;
	mascaraBNITp1_Acero = ~mascaraBNITp1;

	maxPrim = (1 << 28) - 1;
	mascaraPrim = maxPrim;
	mascaraPrim_Acero = ~mascaraPrim;

	maxColisNL = UCHAR_MAX;

	maxBNL = UCHAR_MAX;

	maxBNITp2 = (1 << 4) - 1;
	mascaraBNITp2 = (maxBNITp2 << 28);
	mascaraBNITp2_Acero = ~mascaraBNITp2;

	//////////////////// NIT
	mascaraColisNIT = (1 << 31);

	mascaraBitNIT_NLS = (1 << 30);

	maxIT = mascaraIT = (1 << 30) - 1;


	//////////////////// NP
	//  		cout << "mascaraBitNP:\t\t\t\t\t"; this->ImprimirBits(mascaraBitNP, cout); cout << endl;
	//
	//  		cout << "maxIndNPfijo:\t\t\t\t\t"; this->ImprimirBits(maxIndNPfijo, cout); cout << endl;
	//  		cout << "mascaraIndNPfijo:\t\t\t"; this->ImprimirBits(mascaraIndNPfijo, cout); cout << endl;
	//  		cout << "mascaraIndNPfijo_Acero:\t\t\t\t"; this->ImprimirBits(mascaraIndNPfijo_Acero, cout); cout << endl;
	//
	//  		cout << "maxIndNLPadre:\t\t"; this->ImprimirBits(maxIndNLPadre, cout); cout << endl;
	//  		cout << "mascaraIndNLPadre:\t\t"; this->ImprimirBits(mascaraIndNLPadre, cout); cout << endl;
	//  		cout << "mascaraIndNLPadre_Acero:\t\t"; this->ImprimirBits(mascaraIndNLPadre_Acero, cout); cout << endl;
	//
	//  		cout << "maxBNIT:\t\t\t\t"; this->ImprimirBits(maxBNIT, cout); cout << endl;
	//
	//  		cout << "maxBNITp3:\t\t"; this->ImprimirBits(maxBNITp3, cout); cout << endl;
	//  		cout << "mascaraBNITp3:\t\t"; this->ImprimirBits(mascaraBNITp3, cout); cout << endl;
	//  		cout << "mascaraBNITp3_Acero:\t\t"; this->ImprimirBits(mascaraBNITp3_Acero, cout); cout << endl;
	//
	//  		cout << "maxBNITp4:\t\t\t\t\t"; this->ImprimirBits(maxBNITp4, cout); cout << endl;
	//  		cout << "mascaraBNITp4:\t\t\t\t"; this->ImprimirBits(mascaraBNITp4, cout); cout << endl;
	//  		cout << "mascaraBNITp4_Acero:\t\t\t"; this->ImprimirBits(mascaraBNITp4_Acero, cout); cout << endl;
	//
	//  		cout << "maxIndTHNIT:\t\t"; this->ImprimirBits(maxIndTHNIT, cout); cout << endl;
	//  		cout << "mascaraIndTHNIT:\t\t"; this->ImprimirBits(mascaraIndTHNIT, cout); cout << endl;
	//  		cout << "mascaraIndTHNIT_Acero:\t\t\t"; this->ImprimirBits(mascaraIndTHNIT_Acero, cout); cout << endl;
	//
	//  		cout << "maxIndNLS:\t\t\t"; this->ImprimirBits(maxIndNLS, cout); cout << endl;
	//  		cout << "mascaraIndNLS:\t\t\t\t\t"; this->ImprimirBits(mascaraIndNLS, cout); cout << endl;
	//  		cout << "mascaraIndNLS_Acero:\t\t"; this->ImprimirBits(mascaraIndNLS_Acero, cout); cout << endl;
	//
	//
	//  		//////////////////// NL
	//  		cout << "mascaraBitNL:\t\t\t\t"; this->ImprimirBits(mascaraBitNL, cout); cout << endl;
	//
	//  		cout << "mascaraBitNLS:\t\t\t\t"; this->ImprimirBits(mascaraBitNLS, cout); cout << endl;
	//  		cout << "mascaraBitNLS_Acero:\t\t\t\t"; this->ImprimirBits(mascaraBitNLS_Acero, cout); cout << endl;
	//
	//  		cout << "mascaraBitNPmov:\t\t\t"; this->ImprimirBits(mascaraBitNPmov, cout); cout << endl;
	//  		cout << "mascaraBitNPmov_Acero:\t\t\t"; this->ImprimirBits(mascaraBitNPmov_Acero, cout); cout << endl;
	//
	//  		cout << "mascaraBitBNLpot2:\t\t\t"; this->ImprimirBits(mascaraBitBNLpot2, cout); cout << endl;
	//  		cout << "mascaraBitBNLpot2_Acero:\t\t\t"; this->ImprimirBits(mascaraBitBNLpot2_Acero, cout); cout << endl;
	//
	//  		cout << "mascaraBitBNITpot2:\t\t\t"; this->ImprimirBits(mascaraBitBNITpot2, cout); cout << endl;
	//  		cout << "mascaraBitBNITpot2_Acero:\t\t\t"; this->ImprimirBits(mascaraBitBNITpot2_Acero, cout); cout << endl;
	//
	//  		cout << "maxBNITp1:\t\t\t"; this->ImprimirBits(maxBNITp1, cout); cout << endl;
	//  		cout << "mascaraBNITp1:\t\t\t"; this->ImprimirBits(mascaraBNITp1, cout); cout << endl;
	//  		cout << "mascaraBNITp1_Acero:\t\t\t"; this->ImprimirBits(mascaraBNITp1_Acero, cout); cout << endl;
	//
	//  		cout << "maxPrim:\t\t\t"; this->ImprimirBits(maxPrim, cout); cout << endl;
	//  		cout << "mascaraPrim:\t\t\t"; this->ImprimirBits(mascaraPrim, cout); cout << endl;
	//  		cout << "mascaraPrim_Acero:\t"; this->ImprimirBits(mascaraPrim_Acero, cout); cout << endl;
	//
	//  		cout << "maxColisNL:\t\t\t"; this->ImprimirBits(maxColisNL, cout); cout << endl;
	//
	//  		cout << "maxBNL:\t\t\t"; this->ImprimirBits(maxBNL, cout); cout << endl;
	//
	//  		cout << "maxBNITp2:\t\t"; this->ImprimirBits(maxBNITp2, cout); cout << endl;
	//  		cout << "mascaraBNITp2:\t\t"; this->ImprimirBits(mascaraBNITp2, cout); cout << endl;
	//  		cout << "mascaraBNITp2_Acero:\t\t"; this->ImprimirBits(mascaraBNITp2_Acero, cout); cout << endl;
	//
	//
	//
	//  		//////////////////// NIT
	//  		cout << "mascaraColisNIT:\t"; this->ImprimirBits(mascaraColisNIT, cout); cout << endl;
	//
	//  		cout << "mascaraBitNIT_NLS:\t"; this->ImprimirBits(mascaraBitNIT_NLS, cout); cout << endl;
	//
	//  		cout << "mascaraIT:\t\t\t"; this->ImprimirBits(mascaraIT, cout); cout << endl;
	//  		cout << "maxIT:\t\t\t"; this->ImprimirBits(maxIT, cout); cout << endl;

	//  		exit(0);

	arrayModulo = new _INDICE_NMTRIE[maxBNIT + 1];
	for(i = 0; i < maxBNIT + 1; ++i) {
		ModNoPot2_ObtenerB2(i, arrayModulo[i]);
		//  			cout << "arrayModulo[ " << i << " ]: " << arrayModulo[i] << endl;
		//  			ImprimirBits(i, cout);
		//  			cout << endl;
		//  			ImprimirBits(arrayModulo[i], cout);
		//  			cout << endl;
		//  			cout << endl;
	}

}

// Anyade un nuevo MergedTrie a la lista
//	void MergedTrie::AnyadirMergedTrie(MergedTrieGeneral* t) {
//		listaDeMergedTries.push_back(t);
//	}

// Devuelve un nuevo identificador de MergedTrie
//	int GetIdMergedTrie() {
//		return NumeroDeHtries();
//	}

// It returns whether the MergedTrie is empty: it has no stored words
bool MergedTrie::empty() const
{
	return (cant==0);
}

// It returns whether the number of stored words in the MergedTrie
size_t MergedTrie::size() const
{
	return cant;
}

//    int NumeroDeHtries() {
//    	return listaDeMergedTries.size();
//    }

// Divide la palabra a buscar en los dos trozos que corresponden al Trie1 y al Trie2
// Devuelve la posicion de la palabra en la que comienza el string a buscar en Trie2
inline _INDICE_NMTRIE MergedTrie::DivideStringTrie2(const string& s) {
	return DivideStringTrie2(s.length());
}

// Divide la palabra a buscar en los dos trozos que corresponden al Trie1 y al Trie2
// Devuelve la posicion de la palabra en la que comienza el string a buscar en Trie2
inline _INDICE_NMTRIE MergedTrie::DivideStringTrie2(const _INDICE_NMTRIE& longitud) {
	if(longitud == 1)
		return 1;							// h devolveria 1
	else {
		return longitud * 0.5;	// hola devolveria 2, holaa devolveria 2
		//    	    return (longitud + 1) * 0.5;	// hola devolveria 2, holaa devolveria 3
	}
}

void MergedTrie::TrazaError() {
	cout << "encontrado" << endl;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ NP
#ifdef __MODO_SEGURO__
inline bool MergedTrie::EsNP(const _NMTRIE_& nodoNP) const {
	return (nodoNP.NP.campo1 & mascaraBitNP);
}

inline void MergedTrie::SetNP(_NMTRIE_& nodoNP) {
	nodoNP.NP.campo1 |= mascaraBitNP;
}
#endif


inline void MergedTrie::GetIndNLPadre(const _NMTRIE_& nodoNP, _INDICE_NMTRIE& kprim) const  {
#ifdef __MODO_SEGURO__
	if(!EsNP(nodoNP)) {
		cerr << "ERROR_GetIndNLPadre. NO recibe un NP" << endl;
		exit(EXIT_MTRIE);
	}
#endif
	kprim = (nodoNP.NP.campo2 & mascaraIndNLPadre);

#ifdef __MODO_SEGURO__
	if(!EsNL(bloqueNL[kprim])) {
		cerr << "ERROR_GetIndNLPadre. NO Devuelve un NL" << endl;
		exit(EXIT_MTRIE);
	}
#endif
}

inline void MergedTrie::SetIndNLPadre_Vacio(_NMTRIE_& nodoNP, const _INDICE_NMTRIE& indNPpadre) {
#ifdef __MODO_SEGURO__
	_INDICE_NMTRIE temp;
	GetIndNLPadre(nodoNP, temp);
	if(temp != 0) {
		cerr << "ERROR_SetIndNLPadre_Vacio. NO es vacio previamente" << endl;
		exit(EXIT_MTRIE);
	}
	if(!EsNP(nodoNP)) {
		cerr << "ERROR_SetIndNLPadre_Vacio. NO recibe un NP" << endl;
		exit(EXIT_MTRIE);
	}
	if(!EsNL(bloqueNL[indNPpadre])) {
		cerr << "ERROR_SetIndNLPadre_Vacio. indNPpadre no es un NL" << endl;
		exit(EXIT_MTRIE);
	}
#endif
	if(indNPpadre > maxIndNLPadre) {
		cerr << "ERROR_SetIndNLPadre_Vacio. Se ha superado el prim maximo en " << indNPpadre << endl;
		exit(EXIT_MTRIE);
	}
	nodoNP.NP.campo2 |= indNPpadre;
}

inline void MergedTrie::SetIndNLPadre_Acero(_NMTRIE_& nodoNP) {
	nodoNP.NP.campo2 &= mascaraIndNLPadre_Acero;
}

inline void MergedTrie::SetIndNLPadre_Inicializando(_NMTRIE_& nodoNP, const _INDICE_NMTRIE& kprim) {
	SetIndNLPadre_Acero(nodoNP);
	SetIndNLPadre_Vacio(nodoNP, kprim);
}



inline void MergedTrie::GetIndTHNIT(const _NMTRIE_& nodoNP, _INDICE_NMTRIE& kprim) const  {
#ifdef __MODO_SEGURO__
	if(!EsNP(nodoNP)) {
		cerr << "ERROR_GetIndTHNIT. NO recibe un NP" << endl;
		exit(EXIT_MTRIE);
	}
#endif
	kprim = (nodoNP.NP.campo1 & mascaraIndTHNIT);
}

inline void MergedTrie::SetIndTHNIT_Vacio(_NMTRIE_& nodoNP, const _INDICE_NMTRIE& kprim) {
#ifdef __MODO_SEGURO__
	_INDICE_NMTRIE temp;
	GetIndTHNIT(nodoNP, temp);
	if(temp != 0) {
		cerr << "ERROR_SetIndTHNIT_Vacio. NO es vacio previamente" << endl;
		exit(EXIT_MTRIE);
	}
	if(!EsNP(nodoNP)) {
		cerr << "ERROR_SetIndTHNIT_Vacio. NO recibe un NP" << endl;
		exit(EXIT_MTRIE);
	}
#endif
	if(kprim > maxIndTHNIT) {
		cerr << "ERROR_SetIndTHNIT_Vacio. Se ha superado el prim maximo en " << kprim << endl;
		exit(EXIT_MTRIE);
	}
	nodoNP.NP.campo1 |= kprim;
}

inline void MergedTrie::SetIndTHNIT_Acero(_NMTRIE_& nodoNP) {
	nodoNP.NP.campo1 &= mascaraIndTHNIT_Acero;
}

inline void MergedTrie::SetIndTHNIT_Inicializando(_NMTRIE_& nodoNP, const _INDICE_NMTRIE& kprim) {
	SetIndTHNIT_Acero(nodoNP);
	SetIndTHNIT_Vacio(nodoNP, kprim);
}


inline void MergedTrie::SetBNIT_Vacio(_NMTRIE_& nodoNL, _NMTRIE_& nodoNP, const _INDICE_NMTRIE& kprim) {
#ifdef __MODO_SEGURO__
	_INDICE_NMTRIE temp;
	GetBNIT(nodoNL, nodoNP, temp);
	if(temp != 0) {
		cerr << "ERROR_SetBNIT_Vacio. NO es vacio previamente" << endl;
		exit(EXIT_MTRIE);
	}
	if(!EsNP(nodoNP)) {
		cerr << "ERROR_SetBNIT_Vacio. NO recibe un NP" << endl;
		exit(EXIT_MTRIE);
	}
	if(!EsNL(nodoNL)) {
		cerr << "ERROR_SetBNIT_Vacio. NO recibe un NL" << endl;
		exit(EXIT_MTRIE);
	}
#endif
	if(kprim > maxBNIT) {
		cerr << "ERROR_SetBNIT_Vacio. Se ha superado el prim maximo en " << kprim << endl;
		exit(EXIT_MTRIE);
	}
	nodoNL.NL.char1 |= (kprim & maxBNITp1);

	if(kprim > maxBNITp1) {
#ifdef __MODO_SEGURO__
		nodoNL.NL.prim |= ((kprim << 25)  & mascaraBNITp2);
#else
		nodoNL.NL.prim |= ((kprim << 24)  & mascaraBNITp2);
#endif

#ifdef __MODO_SEGURO__
		nodoNP.NP.campo1 |= ((kprim << 20)  & mascaraBNITp3);
#else
		nodoNP.NP.campo1 |= ((kprim << 20)  & mascaraBNITp3);
#endif

#ifdef __MODO_SEGURO__
		nodoNP.NP.campo2 |= ((kprim << 17)  & mascaraBNITp4);
#else
		nodoNP.NP.campo2 |= ((kprim << 16)  & mascaraBNITp4);
#endif
	}

	//		cout << "kprim:  "; ImprimirBits(kprim, cout); cout << endl;
	//		cout << "nodoNL.char1: "; ImprimirBits(nodoNL.NL.char1, cout); cout << endl;
	//		cout << "nodoNL.prim: "; ImprimirBits(nodoNL.NL.prim, cout); cout << endl;
	//		cout << "nodoNP.campo1: "; ImprimirBits(nodoNP.NP.campo1, cout); cout << endl;
	//		cout << "nodoNP.campo2: "; ImprimirBits(nodoNP.NP.campo2, cout); cout << endl;
}

inline void MergedTrie::SetBNIT_Acero(_NMTRIE_& nodoNL, _NMTRIE_& nodoNP) {
	nodoNL.NL.char1 &= mascaraBNITp1_Acero;
	nodoNL.NL.prim &= mascaraBNITp2_Acero;
	nodoNP.NP.campo1 &= mascaraBNITp3_Acero;
	nodoNP.NP.campo2 &= mascaraBNITp4_Acero;

	//		cout << "\tAcero.nodoNL.char1: "; ImprimirBits(nodoNL.NL.char1, cout); cout << endl;
	//		cout << "\tAcero.nodoNL.prim: "; ImprimirBits(nodoNL.NL.prim, cout); cout << endl;
	//		cout << "\tAcero.nodoNP.campo1: "; ImprimirBits(nodoNP.NP.campo1, cout); cout << endl;
	//		cout << "\tAcero.nodoNP.campo2: "; ImprimirBits(nodoNP.NP.campo2, cout); cout << endl;
}

inline void MergedTrie::SetBNIT_Inicializando(_NMTRIE_& nodoNL, _NMTRIE_& nodoNP, const _INDICE_NMTRIE& kprim) {
	SetBNIT_Acero(nodoNL, nodoNP);
	SetBNIT_Vacio(nodoNL, nodoNP, kprim);
}

inline void MergedTrie::GetBNIT(const _NMTRIE_& nodoNL, const _NMTRIE_& nodoNP, _INDICE_NMTRIE& B) const  {
#ifdef __MODO_SEGURO__
	if(!EsNP(nodoNP)) {
		cerr << "ERROR_GetBNIT. NO recibe un NP" << endl;
		exit(EXIT_MTRIE);
	}
	if(!EsNL(nodoNL)) {
		cerr << "ERROR_GetBNIT. NO recibe un NL" << endl;
		exit(EXIT_MTRIE);
	}
#endif
	B = (nodoNL.NL.char1 & mascaraBNITp1);

#ifdef __MODO_SEGURO__
	B |= ((nodoNL.NL.prim & mascaraBNITp2) >> 25);
#else
	B |= ((nodoNL.NL.prim & mascaraBNITp2) >> 24);
#endif

#ifdef __MODO_SEGURO__
	B |= ((nodoNP.NP.campo1 & mascaraBNITp3) >> 20);
#else
	B |= ((nodoNP.NP.campo1 & mascaraBNITp3) >> 20);
#endif

#ifdef __MODO_SEGURO__
	B |= ((nodoNP.NP.campo2 & mascaraBNITp4) >> 17);
#else
	B |= ((nodoNP.NP.campo2 & mascaraBNITp4) >> 16);
#endif
}

inline void MergedTrie::GetIndNPfijo_DeUnNL(const _INDICE_NMTRIE& indNL, _INDICE_NMTRIE& indNP) const {
#ifdef __MODO_SEGURO__
	if(!EsNL(bloqueNL[indNL])) {
		cerr << "ERROR_GetIndNPfijo_DeUnNL. No recibe un NL" << endl;
		exit(EXIT_MTRIE);
	}
#endif
	GetPrim(bloqueNL[indNL], indNP);
	if(EsNPmov(bloqueNL[indNL])) {
		GetIndNPfijo(bloqueNL[indNP + 1], indNP);
	}
}

inline void MergedTrie::GetIndNPfijo(const _NMTRIE_& nodoNP, _INDICE_NMTRIE& kprim) const  {
#ifdef __MODO_SEGURO__
	if(!EsNP(nodoNP)) {
		cerr << "ERROR_GetIndNPfijo. No recibe un NP" << endl;
		exit(EXIT_MTRIE);
	}
#endif
	kprim = (nodoNP.NP.campo1 & mascaraIndNPfijo);
}

inline void MergedTrie::SetIndNPfijo_Vacio(_NMTRIE_& nodoNP, const _INDICE_NMTRIE& indNPfijo) {
#ifdef __MODO_SEGURO__
	_INDICE_NMTRIE temp;
	GetIndNPfijo(nodoNP, temp);
	if(temp != 0) {
		cerr << "ERROR_SetIndNPfijo_Vacio. NO es vacio previamente" << endl;
		exit(EXIT_MTRIE);
	}
	if(!EsNP(nodoNP)) {
		cerr << "ERROR_SetIndNPfijo_Vacio. No recibe un NP" << endl;
		exit(EXIT_MTRIE);
	}
	if(!EsNP(bloqueNL[indNPfijo])) {
		cerr << "ERROR_SetIndNPfijo_Vacio. indNPfijo no es un NP" << endl;
		exit(EXIT_MTRIE);
	}
#endif
	if(indNPfijo > maxIndNPfijo) {
		cerr << "ERROR_SetIndNPfijo_Vacio. Se ha superado el prim maximo en " << indNPfijo << endl;
		exit(EXIT_MTRIE);
	}
	nodoNP.NP.campo1 |= indNPfijo;
}

inline void MergedTrie::SetIndNPfijo_Acero(_NMTRIE_& nodoNP) {
	nodoNP.NP.campo1 &= mascaraIndNPfijo_Acero;
}

inline void MergedTrie::SetIndNPfijo_Inicializando(_NMTRIE_& nodoNP, const _INDICE_NMTRIE& kprim) {
	SetIndNPfijo_Acero(nodoNP);
	SetIndNPfijo_Vacio(nodoNP, kprim);
}


inline void MergedTrie::GetIndNLS_DeUnNL(const _NMTRIE_& nodoNL, _INDICE_NMTRIE& kprim) const  {
#ifdef __MODO_SEGURO__
	if(!EsNL(nodoNL)) {
		cerr << "ERROR_GetIndNLS_DeUnNL. No recibe un NL" << endl;
		exit(EXIT_MTRIE);
	}
	if(!EsNLS(nodoNL)) {
		cerr << "ERROR_GetIndNLS_DeUnNL. No recibe un NLS" << endl;
		exit(EXIT_MTRIE);
	}
#endif
	GetPrim(nodoNL, kprim);
#ifdef __MODO_SEGURO__
	_NMTRIE_* ptemp;
	GetNL(kprim + 1, ptemp);
#endif
	GetIndNLS(bloqueNL[kprim + 1], kprim);
}

inline void MergedTrie::GetIndNLS(const _NMTRIE_& nodoNP, _INDICE_NMTRIE& kprim) const  {
#ifdef __MODO_SEGURO__
	if(!EsNP(nodoNP)) {
		cerr << "ERROR_GetIndNLS. No recibe un NP" << endl;
		exit(EXIT_MTRIE);
	}
#endif
	//    	kprim = (nodoNP.NP.campo2 & mascaraIndNLS);
	kprim = nodoNP.NP.campo2;
}

inline void MergedTrie::SetIndNLS_Vacio(_NMTRIE_& nodoNP, const _INDICE_NMTRIE& kprim) {
#ifdef __MODO_SEGURO__
	_INDICE_NMTRIE temp;
	GetIndNLS(nodoNP, temp);
	//    	if(temp != 0) {
	//    		cerr << "ERROR_SetIndNLS_Vacio. NO es vacio previamente" << endl;
	//    		exit(EXIT_MTRIE);
	//    	}
	if(!EsNP(nodoNP)) {
		cerr << "ERROR_SetIndNLS_Vacio. No recibe un NP" << endl;
		exit(EXIT_MTRIE);
	}
#endif
	if(kprim > maxIndNLS) {
		cerr << "ERROR_SetIndNLS_Vacio. Se ha superado el prim maximo en " << kprim << endl;
		exit(EXIT_MTRIE);
	}
	//		nodoNP.NP.campo2 |= kprim;
	nodoNP.NP.campo2 = kprim;
}

inline void MergedTrie::SetIndNLS_Acero(_NMTRIE_& nodoNP) {
	//    	nodoNP.NP.campo2 &= mascaraIndNLS_Acero;
	nodoNP.NP.campo2 = 0;
}

inline void MergedTrie::SetIndNLS_Inicializando(_NMTRIE_& nodoNP, const _INDICE_NMTRIE& kprim) {
	//    	SetIndNLS_Acero(nodoNP);
	SetIndNLS_Vacio(nodoNP, kprim);
}



//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ NL
#ifdef __MODO_SEGURO__
inline bool MergedTrie::EsNL(const _NMTRIE_& nodoNL) const {
	return !(nodoNL.NL.char1 & mascaraBitNP);
}
#endif

inline bool MergedTrie::EsNPdoble(const _NMTRIE_& nodoNL) const {
#ifdef __MODO_SEGURO__
	if(!EsNL(nodoNL)) {
		cerr << "ERROR_EsNPdoble. No recibe un NL" << endl;
		exit(EXIT_MTRIE);
	}
#endif
	return (EsNPmov(nodoNL) || EsNLS(nodoNL));
}

inline bool MergedTrie::EsNLS(const _NMTRIE_& nodoNL) const {
#ifdef __MODO_SEGURO__
	if(!EsNL(nodoNL)) {
		cerr << "ERROR_EsNLS. No recibe un NL" << endl;
		exit(EXIT_MTRIE);
	}
#endif
	return (nodoNL.NL.char1 & mascaraBitNLS);
}

inline void MergedTrie::SetNLS(_NMTRIE_& nodoNL) {
#ifdef __MODO_SEGURO__
	if(!EsNL(nodoNL)) {
		cerr << "ERROR_SetNLS. No recibe un NL" << endl;
		exit(EXIT_MTRIE);
	}
#endif
	nodoNL.NL.char1 |= mascaraBitNLS;
}

inline void MergedTrie::UnSetNLS(_NMTRIE_& nodoNL) {
#ifdef __MODO_SEGURO__
	if(!EsNL(nodoNL)) {
		cerr << "ERROR_UnSetNLS. No recibe un NL" << endl;
		exit(EXIT_MTRIE);
	}
#endif
	nodoNL.NL.char1 &= mascaraBitNLS_Acero;
}


inline bool MergedTrie::EsTHNIT(const _NMTRIE_& nodoNL) const {
#ifdef __MODO_SEGURO__
	if(!EsNL(nodoNL)) {
		cerr << "ERROR_EsTHNIT. No recibe un NL" << endl;
		exit(EXIT_MTRIE);
	}
#endif
	_INDICE_NMTRIE indTemp;
	GetPrim(nodoNL, indTemp);
	GetIndTHNIT(bloqueNL[indTemp], indTemp);

	return (indTemp != 0);
}


inline bool MergedTrie::EsNPmov(const _NMTRIE_& nodoNL) const {
#ifdef __MODO_SEGURO__
	if(!EsNL(nodoNL)) {
		cerr << "ERROR_EsNPmov. No recibe un NL" << endl;
		exit(EXIT_MTRIE);
	}
#endif
	return (nodoNL.NL.char1 & mascaraBitNPmov);
}

inline void MergedTrie::SetNPmov(_NMTRIE_& nodoNL) {
#ifdef __MODO_SEGURO__
	if(!EsNL(nodoNL)) {
		cerr << "ERROR_SetNPmov. No recibe un NL" << endl;
		exit(EXIT_MTRIE);
	}
#endif
	nodoNL.NL.char1 |= mascaraBitNPmov;
}

inline void MergedTrie::UnSetNPmov(_NMTRIE_& nodoNL) {
#ifdef __MODO_SEGURO__
	if(!EsNL(nodoNL)) {
		cerr << "ERROR_UnSetNPmov. No recibe un NL" << endl;
		exit(EXIT_MTRIE);
	}
#endif
	nodoNL.NL.char1 &= mascaraBitNPmov_Acero;
}


inline bool MergedTrie::EsBNLpot2(const _NMTRIE_& nodoNL) const {
#ifdef __MODO_SEGURO__
	if(!EsNL(nodoNL)) {
		cerr << "ERROR_EsBNLpot2. No recibe un NL" << endl;
		exit(EXIT_MTRIE);
	}
#endif
	return (nodoNL.NL.char1 & mascaraBitBNLpot2);
}

inline void MergedTrie::SetBNLpot2(_NMTRIE_& nodoNL) {
#ifdef __MODO_SEGURO__
	if(!EsNL(nodoNL)) {
		cerr << "ERROR_SetBNLpot2. No recibe un NL" << endl;
		exit(EXIT_MTRIE);
	}
#endif
	nodoNL.NL.char1 |= mascaraBitBNLpot2;
}

inline void MergedTrie::UnSetBNLpot2(_NMTRIE_& nodoNL) {
#ifdef __MODO_SEGURO__
	if(!EsNL(nodoNL)) {
		cerr << "ERROR_UnSetBNLpot2. No recibe un NL" << endl;
		exit(EXIT_MTRIE);
	}
#endif
	nodoNL.NL.char1 &= mascaraBitBNLpot2_Acero;
}



inline bool MergedTrie::EsBNITpot2(const _NMTRIE_& nodoNL) const {
#ifdef __MODO_SEGURO__
	if(!EsNL(nodoNL)) {
		cerr << "ERROR_EsBNITpot2. No recibe un NL" << endl;
		exit(EXIT_MTRIE);
	}
#endif
	return (nodoNL.NL.char1 & mascaraBitBNITpot2);
}

inline void MergedTrie::SetBNITpot2(_NMTRIE_& nodoNL) {
#ifdef __MODO_SEGURO__
	if(!EsNL(nodoNL)) {
		cerr << "ERROR_SetBNITpot2. No recibe un NL" << endl;
		exit(EXIT_MTRIE);
	}
#endif
	nodoNL.NL.char1 |= mascaraBitBNITpot2;
}

inline void MergedTrie::UnSetBNITpot2(_NMTRIE_& nodoNL) {
#ifdef __MODO_SEGURO__
	if(!EsNL(nodoNL)) {
		cerr << "ERROR_UnSetBNITpot2. No recibe un NL" << endl;
		exit(EXIT_MTRIE);
	}
#endif
	nodoNL.NL.char1 &= mascaraBitBNITpot2_Acero;
}



// Devuelve en kprim el indPrim
inline void MergedTrie::GetPrim(const _NMTRIE_& nodoNL, _INDICE_NMTRIE& kprim) const  {
#ifdef __MODO_SEGURO__
	if(!EsNL(nodoNL)) {
		cerr << "ERROR_GetPrim. No recibe un NL" << endl;
		exit(EXIT_MTRIE);
	}
#endif
	kprim = (nodoNL.NL.prim & mascaraPrim);
}

inline void MergedTrie::SetPrim_Acero(_NMTRIE_& nodoNL) {
	nodoNL.NL.prim &= mascaraPrim_Acero;
}

inline void MergedTrie::SetPrim_Vacio(_NMTRIE_& nodoNL, const _INDICE_NMTRIE& kprim) {
#ifdef __MODO_SEGURO__
	_INDICE_NMTRIE temp;
	GetPrim(nodoNL, temp);
	if(temp != 0) {
		cerr << "ERROR_SetPrim_Vacio. NO es vacio previamente" << endl;
		exit(EXIT_MTRIE);
	}
	if(!EsNL(nodoNL)) {
		cerr << "ERROR_SetPrim_Vacio. No recibe un NL" << endl;
		exit(EXIT_MTRIE);
	}
#endif
	if(kprim > maxPrim) {
		cerr << "ERROR_SetPrim_Vacio. Se ha superado el prim maximo en " << kprim << endl;
		exit(EXIT_MTRIE);
	}
	nodoNL.NL.prim |= kprim;
}

inline void MergedTrie::SetPrim_Inicializando(_NMTRIE_& nodoNL, const _INDICE_NMTRIE& kprim) {
	SetPrim_Acero(nodoNL);
	SetPrim_Vacio(nodoNL, kprim);
}

inline void MergedTrie::GetColisNL(const _NMTRIE_& nodoNL, _INDICE_NMTRIE& kprim) const  {
#ifdef __MODO_SEGURO__
	if(!EsNL(nodoNL)) {
		cerr << "ERRO_GetColisNL. No recibe un NL" << endl;
		exit(EXIT_MTRIE);
	}
#endif
	kprim = nodoNL.NL.char2;
}

inline void MergedTrie::SetColisNL_Vacio(_NMTRIE_& nodoNL, const _INDICE_NMTRIE& kprim) {
#ifdef __MODO_SEGURO__
	if(!EsNL(nodoNL)) {
		cerr << "ERROR_SetColisNL_Vacio. No recibe un NL" << endl;
		exit(EXIT_MTRIE);
	}
#endif
	if(kprim > maxColisNL) {
		cerr << "ERROR_SetColisNL_Vacio. Se ha superado el prim maximo en " << kprim << endl;
		exit(EXIT_MTRIE);
	}
	nodoNL.NL.char2 = kprim;
}

inline void MergedTrie::SetColisNL_Acero(_NMTRIE_& nodoNL) {
#ifdef __MODO_SEGURO__
	if(!EsNL(nodoNL)) {
		cerr << "ERROR_SetColisNL_Acero. No recibe un NL" << endl;
		exit(EXIT_MTRIE);
	}
#endif
	nodoNL.NL.char2 = 0;
}

inline void MergedTrie::SetColisNL_Inicializando(_NMTRIE_& nodoNL, const _INDICE_NMTRIE& kprim) {
	//    	SetColisNL_Acero(nodoNL);
	SetColisNL_Vacio(nodoNL, kprim);
}


inline void MergedTrie::GetBNL(const _NMTRIE_& nodoNL, _INDICE_NMTRIE& kprim) const  {
#ifdef __MODO_SEGURO__
	if(!EsNL(nodoNL)) {
		cerr << "ERROR_GetBNL. No recibe un NL" << endl;
		exit(EXIT_MTRIE);
	}
#endif
	if(nodoNL.NL.char3 == 255)
		kprim = 256;
	else
		kprim = nodoNL.NL.char3;
}

inline void MergedTrie::SetBNL_Acero(_NMTRIE_& nodoNL) {
#ifdef __MODO_SEGURO__
	if(!EsNL(nodoNL)) {
		cerr << "ERROR_SetBNL_Acero. No recibe un NL" << endl;
		exit(EXIT_MTRIE);
	}
#endif
	nodoNL.NL.char3 = 0;
}

inline void MergedTrie::SetBNL_Vacio(_NMTRIE_& nodoNL, const _INDICE_NMTRIE& kprim) {
#ifdef __MODO_SEGURO__
	if(!EsNL(nodoNL)) {
		cerr << "ERROR_SetBNL_Vacio. No recibe un NL" << endl;
		exit(EXIT_MTRIE);
	}
#endif
	if(kprim > maxBNL + 1) {
		cerr << "ERROR_SetBNL_Vacio. Se ha superado el prim maximo en " << kprim << endl;
		exit(EXIT_MTRIE);
	}
	if(kprim == 256)
		nodoNL.NL.char3 = 255;
	else
		nodoNL.NL.char3 = kprim;
}

inline void MergedTrie::SetBNL_Inicializando(_NMTRIE_& nodoNL, const _INDICE_NMTRIE& kprim) {
	//    	SetBNL_Acero(nodoNL);
	SetBNL_Vacio(nodoNL, kprim);
}


inline void MergedTrie::GetLetra(const _NMTRIE_& nodoNL, _INDICE_NMTRIE& kprim) const  {
#ifdef __MODO_SEGURO__
	if(!EsNL(nodoNL)) {
		cerr << "ERROR_GetLetra. No recibe un NL" << endl;
		exit(EXIT_MTRIE);
	}
#endif
	kprim = nodoNL.NL.char4;
}

inline void MergedTrie::SetLetra(_NMTRIE_& nodoNL, const _INDICE_NMTRIE& kprim) {
#ifdef __MODO_SEGURO__
	if(!EsNL(nodoNL)) {
		cerr << "ERROR_SetLetra. No recibe un NL" << endl;
		exit(EXIT_MTRIE);
	}
#endif
	nodoNL.NL.char4 = kprim;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ NLS
inline void MergedTrie::GetLetraS(const _NMTRIE_& nodoNL, unsigned char*& pChar) const  {
#ifdef __MODO_SEGURO__
	if(!EsNL(nodoNL)) {
		cerr << "ERROR_GetLetraS. NO Recibe un NL" << endl;
		exit(EXIT_MTRIE);
	}
	if(!EsNLS(nodoNL)) {
		cerr << "ERROR_GetLetraS. NO Recibe un NLS" << endl;
		exit(EXIT_MTRIE);
	}
#endif
	_INDICE_NMTRIE indTemp;
	GetPrim(nodoNL, indTemp);
#ifdef __MODO_SEGURO__
	if(indTemp == 0) {
		cerr << "ERROR_GetLetraS. IndPrim == 0" << endl;
		exit(EXIT_MTRIE);
	}
	_NMTRIE_* pIndiceNodo;
	GetNL(indTemp + 1, pIndiceNodo);
#endif
	GetIndNLS(bloqueNL[indTemp + 1], indTemp);
	pChar = &(bloqueNChar[indTemp]);
#ifdef __MODO_SEGURO__
	if(indTemp == 0) {
		cerr << "ERROR_GetLetraS. IndNChar == 0" << endl;
		exit(EXIT_MTRIE);
	}
#endif
}

inline void MergedTrie::GetLetraS(const _NMTRIE_& nodoNL, _INDICE_NMTRIE& indChar) const  {
#ifdef __MODO_SEGURO__
	if(!EsNL(nodoNL)) {
		cerr << "ERROR_GetLetraS. NO Recibe un NL" << endl;
		exit(EXIT_MTRIE);
	}
	if(!EsNLS(nodoNL)) {
		cerr << "ERROR_GetLetraS. NO Recibe un NLS" << endl;
		exit(EXIT_MTRIE);
	}
#endif
	GetPrim(nodoNL, indChar);
#ifdef __MODO_SEGURO__
	if(indChar == 0) {
		cerr << "ERROR_GetLetraS. IndPrim == 0" << endl;
		exit(EXIT_MTRIE);
	}
#endif
	GetIndNLS(bloqueNL[indChar + 1], indChar);
#ifdef __MODO_SEGURO__
	if(indChar == 0) {
		cerr << "ERROR_GetLetraS. IndNChar == 0" << endl;
		exit(EXIT_MTRIE);
	}
#endif
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ NIT
inline bool MergedTrie::GetColisNIT(const _NIT_& nodoNIT) const  {
	return (nodoNIT & mascaraColisNIT);
}

inline void MergedTrie::SetColisNIT(_NIT_& nodoNIT) {
	nodoNIT |= mascaraColisNIT;
}

inline bool MergedTrie::GetBitNIT_NLS(const _NIT_& nodoNIT) const  {
	return (nodoNIT & mascaraBitNIT_NLS);
}

inline void MergedTrie::SetBitNIT_NLS(_NIT_& nodoNIT) {
	nodoNIT |= mascaraBitNIT_NLS;
}

inline void MergedTrie::GetIT(const _NIT_& nodoNIT, _INDICE_NMTRIE& kprim) const  {
	kprim = (nodoNIT & mascaraIT);
}

inline void MergedTrie::SetIT(_NIT_& nodoNIT, const _INDICE_NMTRIE& kprim)  {
#ifdef __MODO_SEGURO__
	_INDICE_NMTRIE temp;
	GetIT(nodoNIT, temp);
	if(temp != 0 && temp != kprim) {
		cerr << "ERROR_SetIT. NO es vacio previamente" << endl;
		exit(EXIT_MTRIE);
	}
#endif
	nodoNIT |= kprim;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ FUNCIONES DE LISTAR PALABRAS
// Lista todas las palabras almacenadas recorriendo bloqueNL
// OJO: No se podra poner llamadas a imprimirXXX ni en esta rutina, ni en las que se llaman desde esta!!!!!
void MergedTrie::ListarPalabrasNLETRAS(ostream &os) const {
	//#define _TRAZA_LISTAR_PALABRAS_NLETRAS_
	_INDICE_NMTRIE prim, indTemp;
	_NMTRIE_ *actual;
	_INDICE_NMTRIE indiceTrie1, indiceTrie2, indiceTrie3, i, j, k, temp, tam;
	_INDICE_NMTRIE b = 0, bf;
	list<_INDICE_NMTRIE> listaIndiceTrie;
	int v = 0;

#ifdef __MODO_SEGURO__
	_NMTRIE_ *pnodo;
#endif

#ifndef __CONFIGURACION_DISTRIBUCION__
	os << "\n\nWords stored in the MergedTrie:" << endl;
	os << 	  "###############################" << endl;
#endif

	imprimirNodos = false;
	imprimirPalabras = true;
	crearPilaNL = true;
	imprimirPalabrasIT1 = imprimirPalabrasIT2 = 0;

	colaNL.push(raizTrie1);
	while(!colaNL.empty()) {
		indTemp = colaNL.top();	// Caso de considerarla como stack
#ifdef __MODO_SEGURO__
		GetNL(indTemp, pnodo);
#endif
		colaNL.pop();
		ImprimirNL(indTemp, os);
	}

	imprimirNodos = true;
	imprimirPalabras = false;
	crearPilaNL = false;
}

void MergedTrie::EscribePalabra(const _INDICE_NMTRIE& imprimirPalabrasIT1, const bool& esNLS1,  const _INDICE_NMTRIE& imprimirPalabrasIT2, const bool& esNLS2, ostream &os) const {
	//#define _TRAZA_ESCRIBE_PALABRA_
	list<_INDICE_NMTRIE>::iterator p;
	bool primero = true;
	string cadena = "";

#ifdef _TRAZA_ESCRIBE_PALABRA_
	cout << "\tEscribePalabra de imprimirPalabrasIT1: " << imprimirPalabrasIT1 << " imprimirPalabrasIT2: " << imprimirPalabrasIT2 << endl;
#endif
	ObtenerCadenaAscendente(imprimirPalabrasIT1, false, esNLS1, cadena);
#ifdef _TRAZA_ESCRIBE_PALABRA_
	cout << "\tEscribePalabra sale de imprimirPalabrasIT1: " << imprimirPalabrasIT1 << ". cadena: " << cadena << endl;
#endif
	ObtenerCadenaAscendente(imprimirPalabrasIT2, true, esNLS2, cadena);
#ifdef _TRAZA_ESCRIBE_PALABRA_
	cout << "\tEscribePalabra sale de imprimirPalabrasIT2: " << imprimirPalabrasIT2 << ". cadena: " << cadena << endl;
#endif

	os << cadena << endl;
}

void MergedTrie::EscribePalabra(list<pair <_INDICE_NMTRIE, bool>>& listaIndiceTrie, ostream &os) const {
	list<pair <_INDICE_NMTRIE, bool>>::iterator p;
	bool primero = true;
	string cadena = "";

#ifdef _TRAZA_ESCRIBE_PALABRA_
	cout << "\tEscribePalabra de listaIndiceTrie: ";
	for(p = listaIndiceTrie.begin(); p!=listaIndiceTrie.end(); ++p) {
		cout << "<" << p->first << ", " << p->second << "> ";
	}
#endif
	//		os << endl;
	for(p = listaIndiceTrie.begin(); p!=listaIndiceTrie.end(); ++p) {
		ObtenerCadenaAscendente(p->first, !(p == listaIndiceTrie.begin()), p->second, cadena);
	}
	os << cadena << endl;
}

// Recibe indiceActual que apunta a un NL
// Devuelve indiceActual que apunta al NL padre del indiceActual recibido
void MergedTrie::ObtenerIndicePadre(_INDICE_NMTRIE& indiceActual) const {
	//#define _TRAZA_OBTENER_INDICE_PADRE_
#ifdef _TRAZA_OBTENER_INDICE_PADRE_
	cout << "\tObtenerIndicePadre. indiceActual recibido: " << indiceActual << endl;
#endif
#ifdef __MODO_SEGURO__
	if(!EsNL(bloqueNL[indiceActual])) {
		cerr << "ERROR_ObtenerIndicePadre. NO Recibe un NL" << endl;
		exit(EXIT_MTRIE);
	}
#endif

	GetPrim(bloqueNL[indiceActual], indiceActual);
	GetIndNLPadre(bloqueNL[indiceActual], indiceActual);

#ifdef _TRAZA_OBTENER_INDICE_PADRE_
	cout << "\tObtenerIndicePadre. NodoLPadre: indiceActual: " << indiceActual << endl;
#endif
}

// Recibe un indNP, que sera un indNPfijo
// Devuelve el indNL que se corresponde a ese indNP, mediante retroceso al NLpadre y recorrido de la THNL de descendientes hasta que coincida con indNP
void MergedTrie::ObtenerNLdeUnNP(const _INDICE_NMTRIE& indNP, _INDICE_NMTRIE& indNL) const {
	//#define _TRAZA_OBTENER_NL_DE_UN_NP_
#ifdef _TRAZA_OBTENER_NL_DE_UN_NP_
	cout << "\tObtenerNLdeUnNP. indNP recibido: " << indNP << endl;
#endif
#ifdef __MODO_SEGURO__
	if(!EsNP(bloqueNL[indNP])) {
		cerr << "ERROR_ObtenerNLdeUnNP. NO recibe un NP en indNP: " << indNP << endl;
		exit(EXIT_MTRIE);
	}
#endif
	_INDICE_NMTRIE B, i, indTemp;

	GetIndNLPadre(bloqueNL[indNP], indNL);
	GetBNL(bloqueNL[indNL], B);
#ifdef _TRAZA_OBTENER_NL_DE_UN_NP_
	cout << "\tObtenerNLdeUnNP. indNLpadre: " << indNL << " B: " << B << endl;
#endif

	if(EsNPdoble(bloqueNL[indNL])) {
		GetPrim(bloqueNL[indNL], indNL);
		indNL += 2;
	}
	else {
		GetPrim(bloqueNL[indNL], indNL);
		++indNL;
	}
#ifdef _TRAZA_OBTENER_NL_DE_UN_NP_
	cout << "\tObtenerNLdeUnNP. Comienzo de la TH de descendientes de indNLpadre: " << indNL << endl;
#endif

	for(i = 0; i < B; ++i, ++indNL) {
		if(bloqueNL[indNL] != 0) {
			GetIndNPfijo_DeUnNL(indNL, indTemp);

			if(indTemp == indNP) {
#ifdef _TRAZA_OBTENER_NL_DE_UN_NP_
				cout << "\tObtenerNLdeUnNP. Encuentra indNL: " << indNL << endl;
#endif
				return;
			}
#ifdef _TRAZA_OBTENER_NL_DE_UN_NP_
			else {
				cout << "\tObtenerNLdeUnNP. NO lo encuentra indNL: " << indNL << endl;
			}
#endif
		}
	}
#ifdef _TRAZA_OBTENER_NL_DE_UN_NP_
	cerr << "\tObtenerNLdeUnNP. FINAL y no lo encuentra!!!!" << endl;
	exit(EXIT_MTRIE);
#endif
}

// Recibe 	esNLS que sera true si indiceActual es un NLS
//			Si inversa indiceActual es un NP. Sino es un NL
// Devuelve cadena. Si inversa se anyadira a cadena por el final. Sino por el principio
void MergedTrie::ObtenerCadenaAscendente(_INDICE_NMTRIE indiceActual, const bool& inversa, bool esNLS, string& cadena) const {
	//#define _TRAZA_OBTENER_CADENA_ASCENDENTE_
	_NMTRIE_ *actual;
	_INDICE_NMTRIE indiceTemp, letra;

#ifdef __MODO_SEGURO__
	if(inversa) {
		if(!EsNP(bloqueNL[indiceActual])) {
			cerr << "ERROR_ObtenerCadenaAscendente. NO recibe un NP en inversa && indiceActual: " << indiceActual << endl;
			exit(EXIT_MTRIE);
		}
	}
	else {
		if(!EsNL(bloqueNL[indiceActual])) {
			cerr << "ERROR_ObtenerCadenaAscendente. NO recibe un NL en !inversa && indiceActual: " << indiceActual << endl;
			exit(EXIT_MTRIE);
		}
	}
#endif

	if(esNLS) {
		int i;
		string cadenaT;

		if(inversa)
			GetIndNLS(bloqueNL[indiceActual + 1], indiceTemp);
		else
			GetIndNLS_DeUnNL(bloqueNL[indiceActual], indiceTemp);

		if(indiceTemp != 0) {
			letra = bloqueNChar[indiceTemp];
			while(letra != '\0') {
				cadenaT += (char) (letra);
				letra = bloqueNChar[++indiceTemp];
			}

			if(inversa) {
				//					cadenaT += '\0';
				i = cadenaT.size() - 1;
				while(i >= 0) {
					cadena += (char) (cadenaT[i]);
					--i;
				}
			}
			else
				cadena = cadenaT + cadena;
		}
	}

	if(inversa) {
		if(indiceActual == _IND_NPFIJO_RAIZ)
			indiceActual = raizTrie1;
		else {
			ObtenerNLdeUnNP(indiceActual, indiceTemp);
			indiceActual = indiceTemp;
		}
	}

	while(indiceActual != raizTrie1) {
#ifdef __MODO_SEGURO__
		if(!EsNL(bloqueNL[indiceActual])) {
			cerr << "ERROR_ObtenerCadenaAscendente. indiceActual: " << indiceActual << " no es NL" << endl;
			exit(EXIT_MTRIE);
		}
#endif
#ifdef _TRAZA_OBTENER_CADENA_ASCENDENTE_
		cout << "\tObtenerCadenaAscendente de trie2: " << inversa << " de indiceActual: " << indiceActual << " cadena: {" << cadena << "}" << endl;
#endif
		GetLetra(bloqueNL[indiceActual], letra);
		if(inversa)
			cadena = cadena + (char) (letra);
		else
			cadena = (char) (letra) + cadena;

#ifdef _TRAZA_OBTENER_CADENA_ASCENDENTE_
		cout << "\t\tObtenerCadenaAscendente. cadenaActual: {" << cadena << "}" << endl;
#endif
		ObtenerIndicePadre(indiceActual);
	}
#ifdef _TRAZA_OBTENER_CADENA_ASCENDENTE_
	cout << "\t\tObtenerCadenaAscendente. cadena devuelta: {" << cadena << "}" << endl;
#endif
}

void MergedTrie::ImprimirBits(const unsigned int& kk, ostream& os) const {
	for(int i= (sizeof(kk)*8)-1; i != 0 ; --i) {
		os << ((kk >> i) & 1);
	}
	os << (kk & 1);
}

void MergedTrie::ImprimirBits(const unsigned char& kk, ostream& os) const {
	for(int i= (sizeof(kk)*8)-1; i != 0 ; --i) {
		os << ((kk >> i) & 1);
	}
	os << (kk & 1);
}

void MergedTrie::ImprimirVNodos(vector<datoNodo>& vNodos) {

	if(vNodos.size() != 0) {
		cout << "\t\t"; vNodos[0].ImprimirLeyenda(); cout << ": ";
		for(int i = 0; i < vNodos.size(); ++i) {
			cout << "<" << i << "> "; vNodos[i].Imprimir((busqNumTrieFallo != 3)); cout << ", ";
		}
		//			cout << endl;
	}
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ FUNCIONES DE CALCULO DE MODULOS Y COLISIONES:
inline void MergedTrie::ObtenerBitMasSignificativo(_INDICE_NMTRIE num, _INDICE_NMTRIE& bit) const {
	if (!num)
		bit = 0;

	bit = 1;

	while (num >>= 1)
		bit <<= 1;
}

// Obtiene B2 de B tal que es el numero con todo unos a partir del bit mas significativo de B
inline void MergedTrie::ModNoPot2_ObtenerB2(const _INDICE_NMTRIE& B, _INDICE_NMTRIE& B2) const {
	if(B < 2)
		B2 = 0;
	else if(EsNumNoCeroPot2(B))
		B2 = B - 1;
	else {
		ObtenerBitMasSignificativo(B, B2);
		B2 <<= 1;
		B2 -= 1;
	}
}

// Obtiene el modulo de un B que puede no ser potencia de 2
// Lo hace de manera eficiente usando la operacion & y comprobando que no se salga de rango (en lugar de %) a partir de B2
inline void MergedTrie::ModNoPot2(const _INDICE_NMTRIE& clave, const _INDICE_NMTRIE& B, const _INDICE_NMTRIE& B2, _INDICE_NMTRIE& modulo) const {
	//    	modulo = clave % B;

	modulo = clave & B2;
	if(modulo >= B)
		modulo = modulo - B;
}

// Obtiene el modulo de un B que puede no ser potencia de 2
// Lo hace de manera eficiente usando la operacion & y comprobando que no se salga de rango (en lugar de %) a partir de B2
// Calcula B2 mediante ModNoPot2_ObtenerB2
inline void MergedTrie::ModNoPot2(const _INDICE_NMTRIE& clave, const _INDICE_NMTRIE& B, _INDICE_NMTRIE& modulo) const {
	//    	modulo = clave % B;

	//    	ModNoPot2_ObtenerB2(B, modulo);
	modulo = arrayModulo[B];

	modulo = clave & modulo;
	if(modulo >= B)
		modulo = modulo - B;
}

// Obtiene el modulo de un B (hasta 63) que puede no ser potencia de 2
// Lo hace de manera eficiente usando la operacion & y comprobando que no se salga de rango (en lugar de %) a partir de B2
// Calcula B2 mediante arrayModulo[B]
inline void MergedTrie::ModNoPot2_256(const _INDICE_NMTRIE& clave, const _INDICE_NMTRIE& B, _INDICE_NMTRIE& modulo) const {
#ifdef __MODO_SEGURO__
	if(B > 255) {
		cerr << "ERROR_ModNoPot2_256. B: " << B << " > 255" << endl;
		exit(EXIT_MTRIE);
	}
#endif
	//    	modulo = clave % B;

	modulo = arrayModulo[B];

	//    	modulo = (clave >> 4) & modulo;
	modulo = clave & modulo;
	if(modulo >= B)
		modulo = modulo - B;
}

inline void MergedTrie::ModPot2(const _INDICE_NMTRIE& clave, const _INDICE_NMTRIE& B, _INDICE_NMTRIE& posHash) {
	posHash = (clave & (B - 1));
}

inline void MergedTrie::ModPot2(const unsigned char& clave, const _INDICE_NMTRIE& B, _INDICE_NMTRIE& posHash) {
	posHash = (clave & (B - 1));
}

// Calcula indSiguienteColis a partir de (indeActual + colision) % B, a partir de indPrincTH y indFinTH
void MergedTrie::ObtenerSiguienteColision(const _INDICE_NMTRIE& indPrincTH, const _INDICE_NMTRIE& indFinTH, const _INDICE_NMTRIE& indActual, const _INDICE_NMTRIE& colision, _INDICE_NMTRIE& indSiguienteColis) const {
#ifdef __MODO_SEGURO__
	if(colision > (indFinTH - indPrincTH + 1)) {
		cerr << "ERROR_ObtenerSiguienteColision. colision: " << colision << " > B: " << (indFinTH - indPrincTH + 1) << endl;
		exit(EXIT_MTRIE);
	}
#endif
	indSiguienteColis = indActual + colision;
	if(indSiguienteColis > indFinTH)
		indSiguienteColis = indPrincTH + (indSiguienteColis - indFinTH - 1);
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ FUNCIONES DE IMPRIMIR:
//	Se actualizaran las variables globales de estadist
//	Se imprimira siempre que imprimirNodos == true
//	Se imprimiran las palabras almacenadas en el trie si imprimirPalabras == true al recorrer bloqueNL en las rutinas de imprimir, almacenando IT1 e IT2 en (imprimirPalabrasIT1, imprimirPalabrasIT2)
//	Se actualizara colaNL, la cola para almacenar los NL que quedan pendientes por recorrer, si crearPilaNL == true
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void MergedTrie::ImprimirNL(const _INDICE_NMTRIE& indNL1, ostream &s) const {
	_INDICE_NMTRIE indTemp, indTemp2, BNL;
	_NMTRIE_* pNL1, *pnodo2;
	_INDICE_NMTRIE letra;

	GetNL(indNL1, pNL1);
#ifdef __MODO_SEGURO__
	if(!EsNL(*pNL1)) {
		cerr << "ERROR_ImprimirNL de un nodo que no es NL en indNL1: " << indNL1 << endl;
		exit(EXIT_MTRIE);
	}
#endif

	if(imprimirNodos)
		s << " [NL." << indNL1 << "]";
	if(*pNL1 == 0) {
		if(imprimirNodos)
			s << " VACIO";
		++estadistNLvacio;
	}
	else {
		++estadistNumNP;	// Porque por cada NL hay un NP
		if(EsNPdoble(*pNL1))
			++estadistNumNP;
		GetLetra(*pNL1, letra);
		++estadistNL;
		if(imprimirNodos)
			s << " NL." << "\"" << (char) letra << "\"";

		if(EsNLS(*pNL1)) {
			GetLetraS(*pNL1, indTemp);
			if(imprimirNodos)
				s << " NLS. \"" << (char*) &(bloqueNChar[indTemp]) << "\"" << " IndLetraS: " << indTemp;
			++estadistNLS;
		}

		GetColisNL(*pNL1, BNL);
		if(imprimirNodos) {
			s << " ColisNL: " << BNL;
		}
		if(BNL != 0)
			++estadistNLcolis;

		GetBNL(*pNL1, BNL);
		GetPrim(*pNL1, indTemp);
		if(imprimirNodos) {
			if(indTemp == 0) {
				cerr << "ERROR_ImprimirNL de un NL con IndPrim == 0" << endl;
				exit(EXIT_MTRIE);
			}
			else {
				if(imprimirNodos) {
					if(BNL != 0) {
						if(EsNPdoble(*pNL1))
							s << " Prim: " << (indTemp + 2);
						else
							s << " Prim: " << (indTemp + 1);
					}
					else
						s << " Prim: SinDescendientes";
				}
			}
		}

		GetNL(indTemp, pnodo2);	// Apuntando a prim

		if(imprimirPalabras) {
			imprimirPalabrasIT1 = indNL1;
			esNLS_IT1 = EsNLS(*pNL1);
			//    			cout << "ListarPalabrasNLETRAS. imprimirPalabrasIT1 (nuevo): " << imprimirPalabrasIT1 << " imprimirPalabrasIT2: " << imprimirPalabrasIT2 << endl;
		}

		ImprimirNP(*pNL1, *pnodo2, indTemp, s);

		if(imprimirNodos)
			s << endl << "    ";

		ImprimirTHNL(*pNL1, s);
	}
	if(imprimirNodos)
		s << endl;
}

void MergedTrie::ImprimirNP(const _NMTRIE_ NL1, const _NMTRIE_ NP1, const _INDICE_NMTRIE& indNP1, ostream &s) const {
	_INDICE_NMTRIE indTemp, indTemp2, indTemp3;
	_NMTRIE_* pnodo2;

#ifdef __MODO_SEGURO__
	if(!EsNL(NL1)) {
		cerr << "ERROR_ImprimirNP de un nodo que no es NL" << endl;
		exit(EXIT_MTRIE);
	}
	if(!EsNP(NP1)) {
		cerr << "ERROR_ImprimirNP: NP1 no es de tipo NP" << endl;
		exit(EXIT_MTRIE);
	}
#endif
	GetPrim(NL1, indTemp);
	if(indTemp != indNP1) {
		cerr << "ERROR_ImprimirNP: prim de NL1: " << indTemp << " != indNP1: " << indNP1 << endl;
		exit(EXIT_MTRIE);
	}
	GetNL(indTemp, pnodo2);
	if(*pnodo2 != NP1) {
		cerr << "ERROR_ImprimirNP: el supuesto NP no coincide con NP1" << endl;
		exit(EXIT_MTRIE);
	}

	if(imprimirNodos)
		s << " [NP." << indNP1 << "]";

	if(EsNPmov(NL1))
		++estadistNPmov;

	if(imprimirNodos) {
		if(EsNPmov(NL1)) {
#ifdef __MODO_SEGURO__
			if(!EsNP(bloqueNL[indNP1 + 1])) {
				cerr << "ERROR_ImprimirNP: EsNPmov y NP2 no es de tipo NP" << endl;
				exit(EXIT_MTRIE);
			}
#endif
			if(imprimirNodos)
				s << " NPmov";
			GetIndNPfijo(bloqueNL[indNP1 + 1], indTemp);
		}
		else {
			indTemp = indNP1;
			if(imprimirNodos)
				s << " NPnoMov";
		}

		s << " IndNPfijo: " << indTemp;
		//    		s << " Letra_NPfijo: " << (char) indTemp2;
		if(EsNLS(NL1)) {
			GetIndNLS(bloqueNL[indTemp + 1], indTemp2);
			if(indTemp2 == 0)
				s << " IndNLS_NPfijo: VACIO";
			else
				s << " IndNLS_NPfijo: " << indTemp2;
		}

	}

	if(!EsNLS(NL1)) {
		if(imprimirNodos)
			s << " IndNLS: VACIO";
	}
	else {
		GetNL(indNP1 + 1, pnodo2);
		GetIndNLS(bloqueNL[indTemp + 1], indTemp2);
		GetIndNLS(*pnodo2, indTemp);

		if(indTemp != indTemp2) {
			cerr << "ERROR_ImprimirNP: IndNPfijo.indNLS != NP.indNLS" << endl;
			exit(EXIT_MTRIE);
		}

		if(imprimirNodos) {
			if(indTemp == 0)
				s << " IndNLS: VACIO";
			else
				s << " IndNLS: " << indTemp;
		}
	}

	GetIndNLPadre(NP1, indTemp);
	if(imprimirNodos) {
		if(indTemp == raizTrie1)
			s << " IndNLPadre: NRAIZ";
		else {
			s << " IndNLPadre: " << indTemp;
		}
	}

	if(!EsTHNIT(NL1)) {
		if(imprimirNodos)
			s << " THNIT: VACIO";
	}
	else {
		GetIndTHNIT(NP1, indTemp);

		if(imprimirNodos)
			s << " IndTHNIT: " << indTemp;
		++estadistNTHNIT;

		GetBNIT(NL1, NP1, indTemp2);
		if(indTemp2 > estadistBNITmax)
			estadistBNITmax = indTemp2;

		if(indTemp == 0) {
			cerr << "ERROR_ImprimirNP con IndTHNIT == 0" << endl;
			exit(EXIT_MTRIE);
		}

		if(indTemp2 == 0) {
			cerr << "ERROR_ImprimirNP con BNIT == 0" << endl;
			exit(EXIT_MTRIE);
		}

		if(imprimirNodos) {
			if(EsBNITpot2(NL1))
				s << " BNITpot2";
			else
				s << " BNITnoPot2";
			s << endl;
		}
		ImprimirTHNIT(indTemp, indTemp2, s);
	}
}

void MergedTrie::ImprimirTHNIT(const _INDICE_NMTRIE& indPrincTHNIT, const _INDICE_NMTRIE& BNIT, ostream &s) const {
	_NIT_ *pNIT;
	_INDICE_NMTRIE indTemp, indTemp2, indTHNIT = indPrincTHNIT, indFinTH;

	int i, j;
	if(imprimirNodos)
		s << " THNIT con BNIT: " << BNIT << endl;

	indFinTH = indPrincTHNIT + BNIT - 1;
	for(i = 0; i < BNIT; ++i, ++indTHNIT) {
		GetNIT(indTHNIT, pNIT);
		if(imprimirNodos)
			s << "\t\t[NIT." << indTHNIT << "](" << i << ") =";

		if(*pNIT == 0) {
			if(imprimirNodos)
				s << " VACIO";
			++estadistNITvacio;
		}
		else {
			++estadistNIT;

			ImprimirNIT(*pNIT, s);

			if(imprimirPalabras) {
				GetIT(*pNIT, indTemp);
				imprimirPalabrasIT2 = indTemp;
				//     			cout << "ListarPalabrasNLETRAS. imprimirPalabrasIT1: " << imprimirPalabrasIT1 << " imprimirPalabrasIT2 (nuevo): " << imprimirPalabrasIT2 << endl;
				EscribePalabra(imprimirPalabrasIT1, esNLS_IT1, imprimirPalabrasIT2, GetBitNIT_NLS(*pNIT), s);
			}
		}
		if(imprimirNodos)
			s << endl;
	}
}

void MergedTrie::ImprimirNIT(const _NIT_& NIT, ostream &s) const {
	_INDICE_NMTRIE indTemp;
	_NMTRIE_ *pNP;

	if(imprimirNodos) {
		GetIT(NIT, indTemp);
		if(indTemp == _IND_NPFIJO_RAIZ)
			s << " IndiceIT_a_NPfijo: NRAIZ";
		else {
			s << " IndiceIT_a_NPfijo: " << indTemp;
			GetNL(indTemp, pNP);
			ImprimirLetraOLetraS_NP(*pNP, indTemp, GetBitNIT_NLS(NIT), s);
		}
	}

	if(GetColisNIT(NIT)) {
		++estadistNITcolis;
		if(imprimirNodos)
			s << " ColisNIT";
	}
	else {
		if(imprimirNodos)
			s << " NO_ColisNIT";
	}

	if(imprimirNodos) {
		if(GetBitNIT_NLS(NIT))
			s << " BitNIT_NLS";
		else
			s << " NO_BitNIT_NLS";
	}
}

// Imprime la THNL de descendicentes de NL
void MergedTrie::ImprimirTHNL(const _NMTRIE_& NL, ostream &s) const {
	_NMTRIE_ *pnodo;
	_INDICE_NMTRIE indPrincTH, indFinTH, indPrincTH0, indFinTH0, indTemp, indTemp2, indTemp3, indSiguienteColis, B, B2;
	int i, j;

#ifdef __MODO_SEGURO__
	if(!EsNL(NL)) {
		cerr << "ERROR_ImprimirTHNL de un nodo que no es NL" << endl;
		exit(EXIT_MTRIE);
	}
#endif
	GetBNL(NL, B);
	if(imprimirNodos && B > 0) {
		s << " BNL: " << B;
		if(EsBNLpot2(NL))
			s << " BNLpot2";
		else
			s << " BNLnoPot2";
		s << endl;
	}
	if(B > estadistBNLmax)
		estadistBNLmax = B;

	if(B > 0) {
		++estadistNTHNL;
		GetPrim(NL, indTemp);
		if(indTemp == 0) {
			cerr << "ERROR_ImprimirTHNL de un NL/NLMov con IndPrim == 0" << endl;
			exit(EXIT_MTRIE);
		}
		++indTemp;	// Saltamos el NP
		if(EsNPdoble(NL))
			++indTemp;	// Saltamos el NP

		GetNL(indTemp, pnodo);
		indPrincTH0 = indTemp;
		indFinTH0 = indPrincTH0 + B - 1;


		for(i = 0; i < B; ++i, ++indTemp) {
			GetNL(indTemp, pnodo);
			if(imprimirNodos)
				s << "\t\t[NL." << indTemp << "](" << i << ") =";
			if(*pnodo == 0) {
				if(imprimirNodos)
					s << " NL VACIO";
				++estadistNLvacio;
			}
			else {
				if(bloqueNL_nuevo == NULL)
					ImprimirLetraOLetraS(*pnodo, s);
				else
					s << " Letra: \"" << (char) pnodo->NL.char4 << "\"";
				if(crearPilaNL)
					colaNL.push(indTemp);

				GetColisNL(*pnodo, indTemp2);
				if(indTemp2 == 0) {
					if(imprimirNodos)
						s << " ColisNL: 0";
				}
				else {
					if(imprimirNodos) {
						s << " ColisNL: " << indTemp2;
						ObtenerSiguienteColision(indPrincTH0, indFinTH0, indTemp, indTemp2, indTemp2);
						s << " Sigte. NL colisionado en: " << indTemp2 << endl;
					}
				}
			}
			if(imprimirNodos)
				s << endl;
		}

		if(imprimirNodos)
			s << endl;
	}
}

void MergedTrie::ImprimirLetraOLetraS_NP(const _NMTRIE_& NP, _INDICE_NMTRIE indNP, const bool& esNLS, ostream &s) const {
	_INDICE_NMTRIE indTemp;
	_INDICE_NMTRIE letra;
	_NMTRIE_* pNP;

#ifdef __MODO_SEGURO__
	if(!EsNP(NP)) {
		cerr << "ERROR_ImprimirLetraOLetraS de un nodo que no es NP" << endl;
		exit(EXIT_MTRIE);
	}
#endif
	ObtenerNLdeUnNP(indNP, indTemp);
	GetLetra(bloqueNL[indTemp], letra);
	if(imprimirNodos)
		s << " Letra: \"" << (char) letra << "\"";

	if(esNLS) {
		GetNL(indNP + 1, pNP);
		GetIndNLS(*pNP, indTemp);
		if(imprimirNodos && indTemp != 0)
			s << " LetraS: \"" << (char*) &(bloqueNChar[indTemp]) << "\"";
	}
}

void MergedTrie::ImprimirLetraOLetraS(const _NMTRIE_& NL, ostream &s) const {
	_INDICE_NMTRIE indTemp;
	_INDICE_NMTRIE letra;
	_NMTRIE_* pNL;

#ifdef __MODO_SEGURO__
	if(!EsNL(NL)) {
		cerr << "ERROR_ImprimirLetraOLetraS de un nodo que no es NL" << endl;
		exit(EXIT_MTRIE);
	}
#endif
	GetLetra(NL, letra);
	if(imprimirNodos)
		s << " Letra: \"" << (char) letra << "\"";

	if(EsNLS(NL)) {
		GetLetraS(NL, indTemp);
		if(imprimirNodos)
			s << " LetraS: \"" << (char*) &(bloqueNChar[indTemp]) << "\"";
	}
}

// Lista todos los nodos de manera recursiva
void MergedTrie::print() const
{
	print(cout);
}

void MergedTrie::ListarNodosIT(ostream& os) const {
	_INDICE_NMTRIE i=0, j=1, dir1;
	_NIT_* pnodo;

	os << endl << endl;
	os << "**********" << endl;
	os << "ARRAY NIT:" << endl;
	os << "**********" << endl;
	for(i=1; i < numNIT; ++i) {
		GetNIT(i, pnodo);
		os << "\t[NIT." << i << "] = "; ImprimirNIT(*pnodo, os); os << endl;
	}
	os << endl << endl;
}

void MergedTrie::ListarNodosChar(ostream& os) const {
	_INDICE_NMTRIE i=0, j=1, dir1;
	_NMTRIE_* pnodo;

	os << "\n\n";
	os << "##########" << endl;
	os << "ARRAY CHAR" << endl;
	os << "##########" << endl;

	for(i=0; i < tamBloqueNChar && i < numNChar; ) {	// El primero se deshecha para que para que IT2 siempre sea distinto de 0 y no haya ambiguedad en la deteccion de IT2 vacio
		os << "\t[NCHAR." << i << "]=" << bloqueNChar[i];
		os << endl;
	}
	os << endl << endl;
}

// Devuelve el entero value formateado por los miles: 123,456,789
string MergedTrie::NumerosConSeparadoresMiles(const unsigned long long& value) const {
//#ifdef LINUX
	string numWithCommas = to_string(value);
//#else
//	char buffer [64];
//	itoa (value,buffer,10);
//	//	sprintf (buffer, "%l", value);
//	string numWithCommas(buffer);
//#endif
	int insertPosition = numWithCommas.length() - 3;
	while (insertPosition > 0) {
		numWithCommas.insert(insertPosition, ",");
		insertPosition-=3;
	}
	return numWithCommas;
}

// prefix_print returns the number of words (until a maximum number of maxPrefixExtracted) stored in the MergedTrie que share the string "prefix"
//	It writes them in "ostream os"
int MergedTrie::prefix_print(const string& prefix, ostream& os, const int& maxPrefixExtracted) {
	//#define _TRAZA_BUSCAR_PREFIJO_
#ifdef _TRAZA_BUSCAR_PREFIJO_
	cout << endl << "\n\n!!!!!!!!!!!!\tprefix_print de prefijo: \"" << prefix << "\" de longitud: " << prefix.length() << endl;
#endif
	string s, cadena;
	_INDICE_NMTRIE numPrefijos = 0, busq_i2;
	bool falloEsNLS = false;

	busqTrie2Inicio = prefix.length();
	if(!BuscarPalabraAuxTrie1(prefix)) {
#ifdef _TRAZA_BUSCAR_PREFIJO_
		cout << "\tprefix_print. Fallo en BuscarPalabraAuxTrie1. busqIndiceActual: " << busqIndiceActual << " busqIndicePadre: " << busqIndicePadre << " busq_i: " << busq_i << " busqLetraTemp: " << (char) busqLetraTemp << " actual: "; ImprimirNL(busqIndiceActual, cout); cout << endl;
#endif
		s = prefix.substr(0, busq_i);
		if(*busqActual == 0) {
#ifdef _TRAZA_BUSCAR_PREFIJO_
			cout << "\tprefix_print. Fallo en BuscarPalabraAuxTrie1. *busqActual == 0. Se retrocede al busqIndicePadre" << endl;
#endif
			busqIndiceActual = busqIndicePadre;
			busqActual = busqPadre;
		}
		else if(busqFalloNLS) {
			if(busq_i < busqTrie2Inicio) {
#ifdef _TRAZA_BUSCAR_PREFIJO_
				cout << "\tprefix_print. Fallo en BuscarPalabraAuxTrie1. EsNLS por lo que no se revisara su THNIT" << endl;
#endif
				if(bloqueNChar[busqIndiceNChar] != '\0')		// Si no se consume todo el NLS
					falloEsNLS = true;
			}
			else {
				for(; bloqueNChar[busqIndiceNChar] != '\0'; ++busqIndiceNChar) {
					s += (char) bloqueNChar[busqIndiceNChar];
				}
#ifdef _TRAZA_BUSCAR_PREFIJO_
				cout << "\tprefix_print. Fallo en BuscarPalabraAuxTrie1. EsNLS y se ha consumido todo el prefijo. Se copia el resto del NLS en s" << endl;
#endif
			}
		}
		else if(*busqActual != 0) {
#ifdef _TRAZA_BUSCAR_PREFIJO_
			cout << "\tprefix_print. Fallo en BuscarPalabraAuxTrie1. Caso en que *busqActual != 0 y no coincide con busqLetraTemp: retrocedemos al nodoPadre para ver si tiene THNIT" << endl;
#endif
			busqIndiceActual = busqIndicePadre;
			busqActual = busqPadre;
		}
		else {
			cerr << "\tprefix_print. Caso de fallo en BuscarPalabraAuxTrie1 no controlado" << endl;
			exit(0);
		}
	}
	else {
		s = prefix.substr(0, busq_i);
#ifdef _TRAZA_BUSCAR_PREFIJO_
		cout << "\tprefix_print. Exito en BuscarPalabraAuxTrie1. s: " << s << " busqIndiceActual: " << busqIndiceActual << " busqIndicePadre: " << busqIndicePadre << " busq_i: " << busq_i << " busqLetraTemp: " << (char) busqLetraTemp << " actual: "; ImprimirNL(busqIndiceActual, cout); cout << endl;
#endif
	}

	if(busqIndiceActual == raizTrie1) {
#ifdef _TRAZA_BUSCAR_PREFIJO_
		cout << "\tprefix_print. Fallo en BuscarPalabraAuxTrie1. No hay prefijos porque no ha encontrado la primera letra en el MergedTrie" << endl;
#endif
		os << "\tThere are no words with prefix: \"" << prefix << "\"" << endl;
		return numPrefijos;
	}

	if(!falloEsNLS) {
#ifdef _TRAZA_BUSCAR_PREFIJO_
		cout << "((((((((((((((((((((((((((((\tprefix_print. Sale de BuscarPalabraAuxTrie1. A recorrer la THNIT de busqIndiceActual: " << busqIndiceActual << " busq_i: " << busq_i << " s: " << s << endl;
#endif
		prefix_print_THNIT(prefix, busq_i, numPrefijos, os, s, busqIndiceActual, maxPrefixExtracted);
	}

	if(busq_i == busqTrie2Inicio) {
#ifdef _TRAZA_BUSCAR_PREFIJO_
		cout << "::::::::::::::::::::::::::::\tprefix_print. Ha recorrido todo prefix. Entonces los hijos de busqIndiceActual: " << busqIndiceActual << " tambien seran prefijos" << endl;
#endif
		prefix_print_THNL(prefix, busq_i, numPrefijos, os, s, busqIndiceActual, maxPrefixExtracted);
	}

	if(busq_i != 1) {
#ifdef _TRAZA_BUSCAR_PREFIJO_
		cout << "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\tprefix_print. Buscando desde primera letra hasta busq_i (ultima letra del prefijo encontrada en BuscarPalabraAuxTrie1): " << busq_i << " posibles prefijos que cumplan la condicion" << endl;
#endif
		busqIndicePadre = raizTrie1;
		busqPadre = &(bloqueNL[raizTrie1]);
		for(busq_i2 = busq_i - 1, busq_i = 0; busq_i < busq_i2 && numPrefijos < maxPrefixExtracted;) {
			busqLetraTemp = (unsigned char) prefix[busq_i];
			if(!BuscarHijo()) {
#ifdef _TRAZA_BUSCAR_PREFIJO_
				cerr << "\tprefix_print: Falla en BuscarHijo. busq_i: " << busq_i << " busqIndiceActual: " << busqIndiceActual << " actual: "; ImprimirNL(busqIndiceActual, cerr); cout << endl;
#endif
				exit(0);
			}
#ifdef _TRAZA_BUSCAR_PREFIJO_
			else {
				cout << "\tprefix_print: Exito en BuscarHijo. busq_i: " << busq_i << " busqIndiceActual: " << busqIndiceActual << " actual: "; ImprimirNL(busqIndiceActual, cout); cout << endl;
			}
#endif
			++busq_i;
			s = prefix.substr(0, busq_i);
			//			prefix_print_THNL(prefix, busq_i, numPrefijos, os, s, busqIndiceActual);
			prefix_print_THNIT(prefix, busq_i, numPrefijos, os, s, busqIndiceActual, maxPrefixExtracted);

			busqIndicePadre = busqIndiceActual;
			busqPadre = busqActual;

			if(EsNLS(bloqueNL[busqIndiceActual]))
				break;
		}
	}

#ifndef __CONFIGURACION_DISTRIBUCION__
	if(numPrefijos == 0)
		os << "There are no words with prefix: \"" << prefix << "\"" << endl;
	else
		os << "Total number of words with prefix: \"" << prefix << "\": " << numPrefijos << endl;
#endif
	return numPrefijos;
}

// RECIBE:
//	* indActual NL desde el que recorrer su THNL
//	* numPrefijos: numero de prefijos encontrados hasta el momento
//	* s: prefijo obtenido hasta este punto
//	* busqTrie2Inicio == longitud del prefix
//	* busq_i2: prefix[busq_i2] por el que avanza la busqueda
// DEVUELVE:
//	* numPrefijos: numero de prefijos encontrados
//	* ostream os donde escribe los prefijos encontrados
void MergedTrie::prefix_print_THNL(const string& prefix, const _INDICE_NMTRIE& busq_i2, _INDICE_NMTRIE& numPrefijos, ostream& os, const string& s, const _INDICE_NMTRIE& indActual, const int& maxPrefixExtracted) {
#ifdef _TRAZA_BUSCAR_PREFIJO_
	cout << "\tprefix_print_THNL. Entra en prefix_print_THNL. s: \"" << s << "\" busq_i2: " << busq_i2 << " indActual: " << indActual << " actual: "; ImprimirNL(indActual, cout); cout << endl;
#endif
	if(numPrefijos >= maxPrefixExtracted)
		return;

	_INDICE_NMTRIE indTemp, indTemp2, i, j, B, letra;
	string cadena;

	GetPrim(bloqueNL[indActual], indTemp);
	GetBNL(bloqueNL[indActual], B);
	if(EsNPdoble(bloqueNL[indActual]))
		indTemp += 2;
	else
		++indTemp;

	for(i = 0; i < B; ++i, ++indTemp) {
		if(numPrefijos >= maxPrefixExtracted)
			return;

		if(bloqueNL[indTemp] != 0) {
			cadena = s;

			GetLetra(bloqueNL[indTemp], letra);
			if(busq_i2 != busqTrie2Inicio && (char) letra != prefix[busq_i2]) {
#ifdef _TRAZA_BUSCAR_PREFIJO_
				cout << "\t\t\tprefix_print_THNL. No coincide letra: " << (char) letra << " con prefix[" << busq_i2 << "] = " << (char) prefix[busq_i2] << endl;
#endif
				continue;
			}
			cadena += letra;

			if(EsNLS(bloqueNL[indTemp])) {
				GetPrim(bloqueNL[indTemp], indTemp2);
				GetIndNLS(bloqueNL[indTemp2 + 1], indTemp2);

				if(indTemp2 != 0) {
					letra = bloqueNChar[indTemp2];
					while(letra != '\0') {
						cadena += (char) (letra);
						letra = bloqueNChar[++indTemp2];
					}
				}
			}

			prefix_print_THNIT(prefix, busq_i2, numPrefijos, os, cadena, indTemp, maxPrefixExtracted);
			prefix_print_THNL(prefix, busq_i2, numPrefijos, os, cadena, indTemp, maxPrefixExtracted);
		}
	}
}

// RECIBE:
//	* indActual NL desde el que recorrer su THNIT
//	* prefix: string prefijo de la busqueda
//	* busqTrie2Inicio == longitud del prefix
//	* busq_i: prefix[busq_i] por el que avanza la busqueda
//	* numPrefijos: numero de prefijos encontrados hasta el momento
//	* s: prefijo obtenido hasta este punto
// DEVUELVE:
//	* numPrefijos: numero de prefijos encontrados
//	* ostream os donde escribe los prefijos encontrados
void MergedTrie::prefix_print_THNIT(const string& prefix, const _INDICE_NMTRIE& busq_i, _INDICE_NMTRIE& numPrefijos, ostream& os, const string& s, _INDICE_NMTRIE indActual, const int& maxPrefixExtracted) {
#ifdef _TRAZA_BUSCAR_PREFIJO_
	cout << "\tprefix_print_THNIT. Entra en prefix_print_THNIT. s: \"" << s << "\" indActual: " << indActual << " busq_i: " << busq_i << " busqLetraTemp: " << (char) busqLetraTemp << " actual: "; ImprimirNL(indActual, cout); cout << endl;
#endif
	if(numPrefijos >= maxPrefixExtracted)
		return;

	_INDICE_NMTRIE indTemp, indTemp2, i, busq_i2, B, IT, letra;
	string cadena, cadenaT;

	GetPrim(bloqueNL[indActual], indTemp);
	GetBNIT(bloqueNL[indActual], bloqueNL[indTemp], B);
	GetIndTHNIT(bloqueNL[indTemp], indTemp);
	if(indTemp == 0) {
#ifdef _TRAZA_BUSCAR_PREFIJO_
		cout << "\tprefix_print_THNIT. No hay prefijos porque no hay THNIT" << endl;
#endif
	}
	else {
#ifdef _TRAZA_BUSCAR_PREFIJO_
		cout << "\tprefix_print_THNIT. Empieza el recorrido de la THNIT que empieza en: " << indTemp << " B: " << B << " cadena obtenida hasta ahora: " << s << endl;
#endif

		bool continuar;
		for(i = 0; i < B; ++i, ++indTemp) {
			if(numPrefijos >= maxPrefixExtracted)
				return;

			if(bloqueNIT[indTemp] != 0) {
				continuar = true;
				GetIT(bloqueNIT[indTemp], indActual);
#ifdef _TRAZA_BUSCAR_PREFIJO_
				cout << "\tprefix_print_THNIT. Analizando el IT de indice: " << indTemp << " cadena obtenida hasta ahora: " << s << endl;
#endif
				cadena = s;
				busq_i2 = busq_i;

				if(GetBitNIT_NLS(bloqueNIT[indTemp])) {
					int i;
					cadenaT = "";

					GetIndNLS(bloqueNL[indActual + 1], indTemp2);

					if(indTemp2 != 0) {
						letra = bloqueNChar[indTemp2];
						while(letra != '\0') {
							cadenaT += (char) (letra);
							letra = bloqueNChar[++indTemp2];
						}

						i = cadenaT.size() - 1;
						while(i >= 0) {
							if(busq_i2 != busqTrie2Inicio && (char) cadenaT[i] != prefix[busq_i2]) {
#ifdef _TRAZA_BUSCAR_PREFIJO_
								cout << "\t\t\tprefix_print_THNIT. No coincide letra del NLS: " << (char) cadenaT[i] << " con prefix[" << busq_i2 << "] = " << (char) prefix[busq_i2] << endl;
#endif
								continuar = false;
								break;
							}
							else if(busq_i2 != busqTrie2Inicio)
								++busq_i2;
							cadena += (char) (cadenaT[i]);
							--i;
						}
					}
				}

				if(indActual == _IND_NPFIJO_RAIZ)
					indActual = raizTrie1;
				else {
					ObtenerNLdeUnNP(indActual, indTemp2);
					indActual = indTemp2;
				}

				while(indActual != raizTrie1 && continuar) {
#ifdef _TRAZA_BUSCAR_PREFIJO_
					cout << "\t\tprefix_print_THNIT. Bucle hasta indActual != raizTrie1. NL indActual: " << indActual << " cadena: {" << cadena << "}" << " actual: "; ImprimirNL(indActual, cout); cout << endl;
#endif
					GetLetra(bloqueNL[indActual], letra);

					if(busq_i2 != busqTrie2Inicio && (char) letra != prefix[busq_i2]) {
#ifdef _TRAZA_BUSCAR_PREFIJO_
						cout << "\t\t\tprefix_print_THNIT. No coincide letra: " << (char) letra << " con prefix[" << busq_i2 << "] = " << (char) prefix[busq_i2] << endl;
#endif
						continuar = false;
						break;
					}
					else if(busq_i2 != busqTrie2Inicio)
						++busq_i2;
					cadena = cadena + (char) (letra);

#ifdef _TRAZA_BUSCAR_PREFIJO_
					cout << "\t\t\t\tprefix_print_THNIT. cadenaActual: {" << cadena << "}" << endl;
#endif
					ObtenerIndicePadre(indActual);
				}

				if(numPrefijos < maxPrefixExtracted && busq_i2 == busqTrie2Inicio && indActual == raizTrie1 && continuar) {
#ifdef _TRAZA_BUSCAR_PREFIJO_
					cout << "\t\t\t\t\t===================== PREFIJO DEVUELTO: ";
#endif
					++numPrefijos;
					os << cadena << endl;
				}
			}
		}
	}
}

void MergedTrie::print(ostream& os, const bool& print_words, const bool& print_statistics, const bool& print_MergedTrie_nodes) const
{
	_INDICE_NMTRIE temp;
	_NMTRIE_* pnodo;
	_INDICE_NMTRIE indTemp;

	imprimirNodos = print_MergedTrie_nodes;
	crearPilaNL = true;

	estadistNumNP = estadistNPmov = estadistNLvacio = estadistNITvacio = estadistNL = estadistNLcolis = estadistNITcolis = estadistNLS = estadistNIT = estadistBNLmax = estadistBNITmax = estadistNTHNL = estadistNTHNIT = 0;

	//    	colaNL.~stack();

	//		os << endl << "NL Raiz: " << raizTrie1 << endl;
	colaNL.push(raizTrie1);

	while(!colaNL.empty()) {
		indTemp = colaNL.top();	// Caso de considerarla como stack
		GetNL(indTemp, pnodo);
		colaNL.pop();
		ImprimirNL(indTemp, os);
	}

	imprimirNodos = true;
	crearPilaNL = false;

	//		ListarNodosIT(os);
	//		ListarNodosChar(os);

	if(print_statistics) {
#ifndef __CONFIGURACION_DISTRIBUCION__
		os << "\n\nMERGED-TRIE STATISTICS: " << endl;
		os << numNL << " " << estadistNL << " " << estadistNLvacio << " " << estadistNLcolis;
		os << " " << estadistNumNP << " " << estadistNPmov << " " << estadistNTHNL;
		os << " " << estadistBNLmax;

		os << " " << numNChar << " " << estadistNLS;

		os << " " << numNIT << " " << estadistNIT << " " << estadistNITvacio << " " << estadistNITcolis;
		os << " " << estadistNTHNIT << " " << estadistBNITmax;

		os << " " << __B_INICIAL_NL__ << " " << __B_INICIAL_NL_RAIZ__;
		os << " " << __B_INICIAL_NIT__;
		os << " " << __INC_BN__ << " ";
		if(B_compressed)
			os << "__B_COMPRESSED__";
		else
			os << "__B_NO_COMPRESSED__";
		os << " " << __INC_BNIT__;
		os << " " << (numNL * 8 + numNIT * 4 + numNChar) / 1024;
		os << " " << ((estadistNL + estadistNL + estadistNLS) * 8 + estadistNIT * 4 + numNChar) / 1024;
		os << " " << ((estadistNL + estadistNL + estadistNLS + estadistNLvacio) * 8 + (estadistNIT + estadistNITvacio) * 4 + numNChar) / 1024;
		os << endl << endl;
#endif


		os << "\n\n\n+++++ GENERAL. MergedTrie: ";
		os << "#words: " << NumerosConSeparadoresMiles(cant);
		os << " Store Additional Data for words: ";
		if(dataMT)
			os << "T";
		else
			os << "F";

		os << "\n\t+++++ LETTER NODES (LN): ";
		os << " #PositionsInArrayOfLN: " << NumerosConSeparadoresMiles(numNL);
		os << " #LetterNodes: " << NumerosConSeparadoresMiles(estadistNL);
		os << " #EmptyLN: " << NumerosConSeparadoresMiles(estadistNLvacio);
		os << " #LN_collided: " << NumerosConSeparadoresMiles(estadistNLcolis);
		os << " #HashTablesOfLN: " << NumerosConSeparadoresMiles(estadistNTHNL);
		os << " BmaxOfLN: " << estadistBNLmax;

		os << "\n\t+++++ EXTENDED LETTER NODES (ELN): ";
		os << " #PositionsInArrayOfELN: " << NumerosConSeparadoresMiles(numNChar);
		os << " #ExtendedLetterNodes: " << NumerosConSeparadoresMiles(estadistNLS);

		os << "\n\t+++++ PARENT NODES (PN): ";
		os << " #ParentNodes: " << NumerosConSeparadoresMiles(estadistNumNP);
		os << " #MovedParentNodes: " << NumerosConSeparadoresMiles(estadistNPmov);

		os << "\n\t+++++ INTER-TRIE NODES (IT): ";
		os << " #PositionsInArrayOfIT: " << NumerosConSeparadoresMiles(numNIT);
		os << " #InterTrieNodes: " << NumerosConSeparadoresMiles(estadistNIT);
		os << " #EmptyIT: " << NumerosConSeparadoresMiles(estadistNITvacio);
		os << " #IT_collided: " << NumerosConSeparadoresMiles(estadistNITcolis);
		os << " #HashTablesOfIT: " << NumerosConSeparadoresMiles(estadistNTHNIT);
		os << " BmaxOfIT: " << NumerosConSeparadoresMiles(estadistBNITmax);

		os << "\n\t+++++ CONFIGURATION: ";
		os << " SizeOfTheArrayOfLN: " << NumerosConSeparadoresMiles(tamBloqueNL);
		os << " SizeOfTheArrayOfELN: " << NumerosConSeparadoresMiles(tamBloqueNChar);
		os << " SizeOfTheArrayOfIT: " << NumerosConSeparadoresMiles(tamBloqueNIT);
		os << " MergedTrie is compacted: ";
		if(estaComprimido)
			os << "T";
		else
			os << "F";
		os << " Default MergedTrie Storing Directory: " << __DIRECTORY_TO_STORE__;
		os << " TRIE_PRUEBA_DE_FICHERO: ";
#ifdef TRIE_PRUEBA_DE_FICHERO
		os << "T";
#else
		os << "F";
#endif
		os << " __MULTIPLE_TRIE__: ";
#ifdef __MULTIPLE_TRIE__
		os << "T";
#else
		os << "F";
#endif
		os << " __B_INICIAL_NL__: " << __B_INICIAL_NL__;
		os << " __B_INICIAL_NL_RAIZ__: " << __B_INICIAL_NL_RAIZ__;
		os << " __B_INICIAL_NIT__: " << __B_INICIAL_NIT__;
		os << " __MODO_SEGURO__: ";
#ifdef __MODO_SEGURO__
		os << "T";
#else
		os << "F";
#endif
		os << " __CONFIGURACION_ARTICULO__: ";
#ifdef __CONFIGURACION_ARTICULO__
		os << "T";
#else
		os << "F";
#endif
		//			os << " __INC_BN__ (*tamBloqueNodos al ampliar array): " << __INC_BN__;
		os << " __B_COMPRESSED__: ";
		if(B_compressed)
			os << "T";
		else
			os << "F";
		//			os << " __INC_BNIT__ (+B al ampliar THNIT): " << __INC_BNIT__;

		if(dataMT)
			os << "\n\n\t+++++ MEMORY CONSUMED BY THE MergedTrie ((#PositionsInArrayOfLN * 8 + #PositionsInArrayOfIT * 8 + #PositionsInArrayOfELN * 1) / 1024): " << NumerosConSeparadoresMiles((numNL * 8 + numNIT * 8 + numNChar) / 1024);
		else
			os << "\n\n\t+++++ MEMORY CONSUMED BY THE MergedTrie ((#PositionsInArrayOfLN * 8 + #PositionsInArrayOfIT * 4 + #PositionsInArrayOfELN * 1) / 1024): " << NumerosConSeparadoresMiles((numNL * 8 + numNIT * 4 + numNChar) / 1024);
		os << " KBytes";

		os << endl << endl;
	}

	if(print_words)
		ListarPalabrasNLETRAS(os);
}

// Lista todos los MergedTrie que trabajan sobre esta estructura
//	void MergedTrie::ListarMergedTries(const bool& imprTexto = true) {
//		if(imprTexto) {
//			cout << endl << "LISTA DE MergedTrieS: " << endl;
//			cout <<         "****************" << endl;
//		}
//		list<MergedTrieGeneral*>::iterator p;
//		for(p = listaDeMergedTries.begin();p!=listaDeMergedTries.end();++p) {
//			cout << endl; (*p)->ImprimirMergedTrie(); cout << endl;
//		}
//	}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ FUNCIONES DE SERIALIZACION
// Guarda los MergedTrie actualmente activos en fichero: directorio  __fich_MergedTries y ficheros MergedTrieSettings.txt y MergedTrie_Numero.txt
void MergedTrie::store_in_directory(const string& directory) {
	string nomFich0, nomFich;
	DIR* dir = opendir(directory.c_str());
	bool ret=false;
	ofstream f;
	int i;
	if (dir)
	{
		// Existia el directorio, lo cerramos y elminamos
		closedir(dir);
	}
	else if (ENOENT == errno)
	{
		// No existia el dir, lo creamos
//#ifdef LINUX
//		string s="mkdir "+directory;
//#else
//		string s="md "+directory;
//#endif
//		int temp = system(s.c_str());
		cerr << "ERROR_store_in_directory: the directory: " << directory << " must be created previously" << endl;
		exit(EXIT_MTRIE);
	}

	cout << endl << "STORING THE MergedTrie in the directory: \"" << directory << "\"" << endl;
	cout <<         "**************************************************************************" << endl;

	nomFich0 = nomFich = directory+"/MergedTrieSettings.txt";
	f.open(nomFich.c_str());
	if(!f.fail())
	{
		f << numNL << " " << numNIT << " " << numNChar << " " << cant << " " << tamBloqueNL << " " << tamBloqueNIT << " " << tamBloqueNChar << " " << numBitsEOW << " " << raizTrie1 << " " << estaComprimido << " " << B_compressed << " ";
		f << "\n";
		int j=1, k=0;
		i=0;

		f.close();
		nomFich = nomFich0 + ".bin";
		f.open(nomFich.c_str(), ios::out | ios::binary);
		if(f.fail())
		{
			cerr << "ERROR_store_in_directory while creating the file: " << nomFich << endl;
			exit(EXIT_MTRIE);
		}
		_INDICE_NMTRIE numNodosT;
		if(numNL > tamBloqueNL)
			numNodosT = tamBloqueNL;
		else
			numNodosT = numNL;
		f.write(reinterpret_cast<char *> (bloqueNL), sizeof(_NMTRIE_) * numNodosT);

		f.close();
		nomFich = nomFich0 + "2.bin";
		f.open(nomFich.c_str(), ios::out | ios::binary);
		if(f.fail())
		{
			cerr << "ERROR_store_in_directory while creating the file: " << nomFich << endl;
			exit(EXIT_MTRIE);
		}
		if(numNChar > tamBloqueNChar)
			numNodosT = tamBloqueNChar;
		else
			numNodosT = numNChar;
		f.write(reinterpret_cast<char *> (bloqueNChar), sizeof(unsigned char) * numNodosT);

		f.close();
		nomFich = nomFich0 + "3.bin";
		f.open(nomFich.c_str(), ios::out | ios::binary);
		if(f.fail())
		{
			cerr << "ERROR_store_in_directory while creating the file: " << nomFich << endl;
			exit(EXIT_MTRIE);
		}
		if(numNIT > tamBloqueNIT)
			numNodosT = tamBloqueNIT;
		else
			numNodosT = numNIT;
		f.write(reinterpret_cast<char *> (bloqueNIT), sizeof(_NIT_) * numNodosT);
	}
	else {
		cerr << "ERROR_store_in_directory while creating the file: " << nomFich << endl;
		exit(EXIT_MTRIE);
	}
	f.close();

	//		cout << endl << "GUARDANDO DATOS DE LOS MergedTrieS: " << endl;
	//		cout <<         "******************************" << endl;
	//
	//		list<MergedTrieGeneral*>::iterator pG;
	//		for(pG = listaDeMergedTries.begin();pG!=listaDeMergedTries.end();++pG) {
	//			i = (*pG)->GetNumeroTrie();
	////			cout << "VALOR: " << i << endl;
	//			nomFich = directory + "/MergedTrie_" + devuelveStringNumero(i) + ".txt";
	//			f.open(nomFich.c_str());
	//			if(!f.fail())
	//			{
	//				(*pG)->ImprimirMergedTrie(f);
	//			}
	//			else {
	//				cerr << "ERROR_store_in_directory while creating the file: " << nomFich << endl;
	//				exit(EXIT_MTRIE);
	//			}
	//			f.close();
	//		}
}

// Recupera los MergedTrie que estan guardados en fichero.
// Debe coincidir con la estructura de MergedTries sobre la que se genero el fichero
// Si freeMemory se libera previamente la memoria que tuviese asignada. Lo normal es que este parametro este siempre a true
// Si recuperaMergedTries entonces se recupera tambien la informacion almacenada en disco de las maps
// Si keepPreviousSizesOfArrays entonces si el nuevo numNL es inferior a tamBloqueNL entonces se mantiene el tamBloqueNL anterior y no el nuevo tamBloqueNL leido del fichero
void MergedTrie::read_from_directory(const string& directory, const bool& freeMemory, const bool& keepPreviousSizesOfArrays) {
	//	void MergedTrie::read_from_directory(const string& directory, const bool& freeMemory, const bool& recuperaMergedTries, const bool& keepPreviousSizesOfArrays=false) {
	DIR* dir = opendir(directory.c_str());
	bool ret=false;
	if (dir)
	{
		// Existia el directorio, lo cerramos y elminamos
		closedir(dir);
	}
	else if (ENOENT == errno)
	{
		// No existia el dir
		cerr << "ERROR_read_from_directory: the directory does not exist: " << directory << endl;
		exit(EXIT_MTRIE);
	}

	cout << endl << "RESTORING THE MergedTrie from the directory: \"" << directory << "\"" << endl;
	cout <<         "******************************************************************************" << endl;
	//#pragma omp parallel for
	//	  	for(string::size_type i = 0; i < 2; ++i)
	//	  	{
	//	  		if(i==0)
	RecuperarDeficheroMergedTriesAux(directory, freeMemory, keepPreviousSizesOfArrays);
	//	  		if(
	//	  				i ==1 &&
	//	  				recuperaMergedTries) {
	//	  			RecuperarDeficheroSoloMergedTries(directory, freeMemory);
	//	  		}
	//	  	}
}

void MergedTrie::RecuperarDeficheroMergedTriesAux(const string& directory, const bool& liberarMemoria, const bool& mantieneBN, const bool& imprimir) {
	string nomFich0, nomFich;
	ifstream f;
	int i, j;
	_INDICE_NMTRIE ktamBNL, kBN, ktamBNL_IT, ktamBNL_IT_MB, ktamBNL_MB;

	if(imprimir)
		cout << "\n\n************************************* RECUPERANDO ESQUELETO DE MergedTrieS" << endl;

	nomFich0 = nomFich = directory+"/MergedTrieSettings.txt";
	f.open(nomFich.c_str());
	string linea;
	if(!f.fail())
	{
		this->clear();
		f >> numNL >> numNIT >> numNChar >> cant >> tamBloqueNL >> tamBloqueNIT >> tamBloqueNChar >> numBitsEOW >> raizTrie1 >> estaComprimido >> B_compressed;

		if(mantieneBN && numNL < tamBloqueNL) {

		}
		else {
			i=0;
			if(liberarMemoria) {
				delete [] bloqueNL;
				delete [] bloqueNChar;
				delete [] bloqueNIT;
			}
			bloqueNL = new _NMTRIE_[numNL];
			bloqueNChar = new unsigned char[numNL];
			bloqueNIT = new _NIT_[numNIT];

			tamBloqueNL = numNL;
			tamBloqueNChar = numNChar;
			tamBloqueNIT = numNIT;
		}
		if(numNL > 0) {
			f.close();
			nomFich = nomFich0 + ".bin";
			f.open(nomFich.c_str(), ios::in | ios::binary);
			f.read(reinterpret_cast<char *>(bloqueNL), sizeof(_NMTRIE_)* numNL);

			f.close();
			nomFich = nomFich0 + "2.bin";
			f.open(nomFich.c_str(), ios::in | ios::binary);
			f.read(reinterpret_cast<char *>(bloqueNChar), sizeof(unsigned char)* numNChar);

			f.close();
			nomFich = nomFich0 + "3.bin";
			f.open(nomFich.c_str(), ios::in | ios::binary);
			f.read(reinterpret_cast<char *>(bloqueNIT), sizeof(_NIT_)* numNIT);
		}
	}
	else {
		cerr << "ERROR_RecuperarDeficheroMergedTriesAux. The file: " << nomFich << " does not exist" << endl;
		exit(EXIT_MTRIE);
	}
	f.close();
	if(imprimir)
		cout << "\n\n************************************* ESQUELETO DE MergedTrieS RECUPERADO" << endl;
}

// Recupera los MergedTrie que estan guardados en fichero.
// Debe coincidir con la estructura de MergedTries sobre la que se genero el fichero
void MergedTrie::RecuperarDeficheroSoloMergedTries(const string& directory, const bool& liberarMemoria) {
	string nomFich;
	DIR* dir = opendir(directory.c_str());
	bool ret=false;
	ifstream f;
	int i;
	if (dir)
	{
		// Existia el directorio, lo cerramos y elminamos
		closedir(dir);
	}
	else if (ENOENT == errno)
	{
		// No existia el dir
		cerr << "ERROR_RecuperarDeficheroSoloMergedTries no existe el directory: " << directory << endl;
		exit(EXIT_MTRIE);
	}

	//		cout << endl << "RECUPERANDO DATOS DE LOS MergedTrieS: " << endl;
	//		cout <<         "********************************" << endl;
	//
	//	  	for(list<MergedTrieGeneral*>::iterator pG = listaDeMergedTries.begin();pG!=listaDeMergedTries.end();++pG) {
	////	  		RecuperarDeficheroSoloMergedTriesAux(pG,  directory, liberarMemoria);
	//
	//	  		i = (*pG)->GetNumeroTrie();
	//	  		if(liberarMemoria)
	//	  			(*pG)->clear();
	//	  		//			cout << "GetNumeroTrie: " << i << endl << endl;
	//	  		nomFich = directory + "/MergedTrie_" + devuelveStringNumero(i) + ".txt";
	//	  		f.open(nomFich.c_str());
	//	  		if(!f.fail())
	//	  		{
	//	  			(*pG)->InsertarMergedTrie(f);
	//	  		}
	//	  		else {
	//	  			cerr << "ERROR_RecuperarDeficheroMergedTries no existe el archivo: " << nomFich << endl;
	//	  			exit(EXIT_MTRIE);
	//	  		}
	//	  		f.close();
	//	  	}
}

// Recupera los MergedTrie que estan guardados en fichero.
// Debe coincidir con la estructura de MergedTries sobre la que se genero el fichero
void MergedTrie::RecuperarDeficheroSoloMergedTries(const int& j, const string& directory, const bool& liberarMemoria, const bool& imprimir) {
	if(imprimir)
		cout << "\n\n************************************* RECUPERANDO DATOS DEL MergedTrie: " << j << endl;

	string nomFich;
	bool ret=false;
	ifstream f;
	int i, k=1;
	//	  	for(list<MergedTrieGeneral*>::iterator pG = listaDeMergedTries.begin();pG!=listaDeMergedTries.end();++pG) {
	//	  		if(k == j) {
	//	  			i = (*pG)->GetNumeroTrie();
	//	  			if(liberarMemoria)
	//	  				(*pG)->clear();
	//	  			//			cout << "GetNumeroTrie: " << i << endl << endl;
	//	  			nomFich = directory + "/MergedTrie_" + devuelveStringNumero(i) + ".txt";
	//	  			f.open(nomFich.c_str());
	//	  			if(!f.fail())
	//	  			{
	//	  				(*pG)->InsertarMergedTrie(f);
	//	  			}
	//	  			else {
	//	  				cerr << "ERROR_RecuperarDeficheroMergedTries no existe el archivo: " << nomFich << endl;
	//	  				exit(EXIT_MTRIE);
	//	  			}
	//	  			f.close();
	//	  			break;
	//	  		}
	//	  		else
	//	  			++k;
	//	  	}
	if(imprimir)
		cout << "\n\n************************************* MergedTrie: " << j << " RECUPERADO" << endl;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ FUNCIONES DE ACCESO A NODOS
void MergedTrie::GetNL(const _INDICE_NMTRIE& indiceNodo, _NMTRIE_*& pIndiceNodo) const {
#ifdef __MODO_SEGURO__
	if(indiceNodo >= tamBloqueNL) {
		cerr << "ERROR_GetNL incorrecto con indiceNodo: " << indiceNodo << " tamBloqueNL: " << tamBloqueNL << endl;
		exit(EXIT_MTRIE);
	}
#endif
	pIndiceNodo = &(bloqueNL[indiceNodo]);
}

void MergedTrie::GetNIT(const _INDICE_NMTRIE& indiceNodo, _NIT_*& pIndiceNodo) const {
#ifdef __MODO_SEGURO__
	if(indiceNodo >= tamBloqueNIT) {
		cerr << "ERROR_GetNIT incorrecto con indiceNodo: " << indiceNodo << " tamBloqueNIT: " << tamBloqueNIT << endl;
		exit(EXIT_MTRIE);
	}
#endif
	pIndiceNodo = &(bloqueNIT[indiceNodo]);
}

void MergedTrie::GetNChar(const _INDICE_NMTRIE& indiceNodo, unsigned char*& pIndiceNodo) const {
#ifdef __MODO_SEGURO__
	if(indiceNodo >= tamBloqueNChar) {
		cerr << "ERROR_GetChar incorrecto con indiceNodo: " << indiceNodo << " tamBloqueNChar: " << tamBloqueNChar << endl;
		exit(EXIT_MTRIE);
	}
#endif
	pIndiceNodo = &(bloqueNChar[indiceNodo]);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ FUNCIONES DE AMPLIAR BLOQUES DE NODOS
void MergedTrie::AmpliarArrayChar(_INDICE_NMTRIE& numNL, _INDICE_NMTRIE& tamBloqueNL, unsigned char*& bloqueNChar_1) {
	unsigned char *bloqueNChartemp;
	while(numNL > tamBloqueNL) {
		tablaAmpliada = true;
		_INDICE_NMTRIE tamBNLtemp = tamBloqueNL * __INC_BN__;
		if(numNL >= tamBNLtemp)
			tamBNLtemp = numNL * __INC_BN__;
		cout << "WARNING Resizing the array of Extended Letter Nodes (ELN) in the MergedTrie from " << tamBloqueNL << " to " << tamBNLtemp << "..." << endl;

		bloqueNChartemp = new unsigned char[tamBNLtemp];
		if(!bloqueNChartemp) {
			cerr << "ERROR: Additional memory is required for the resizing of the array of Extended Letter Nodes (ELN) in the MergedTrie" << endl;
			exit(EXIT_MTRIE);
		}
		memcpy(bloqueNChartemp, bloqueNChar_1, sizeof(unsigned char) * tamBloqueNL);
		memset(&(bloqueNChartemp[tamBloqueNL]), 0, sizeof(unsigned char) * (tamBNLtemp - tamBloqueNL));
		delete [] bloqueNChar_1;
		bloqueNChar_1 = bloqueNChartemp;
		bloqueNChartemp = NULL;

		tamBloqueNL = tamBNLtemp;

		cout << "\t\t\tResizing is concluded" << endl;
	}
}

void MergedTrie::AmpliarArrayNL(_INDICE_NMTRIE& numNL, _INDICE_NMTRIE& tamBloqueNL, _NMTRIE_*& bloqueNL_1, const string& origen) {
	_NMTRIE_ *bloqueNLtemp;
	while(numNL > tamBloqueNL) {
		tablaAmpliada = true;
		//    		cout << "\n\n*************************************   ESQUELETO MergedTrie ANTES DE_AmpliarArray: " << *this << endl;
		_INDICE_NMTRIE tamBNLtemp = tamBloqueNL * __INC_BN__;
		while(numNL >= tamBNLtemp)
			tamBNLtemp *= __INC_BN__;
		cout << "WARNING Resizing the array of Letter Nodes (LN) in the MergedTrie from " << tamBloqueNL << " to " << tamBNLtemp << "..." << endl;

		bloqueNLtemp = new _NMTRIE_[tamBNLtemp];
		if(!bloqueNLtemp) {
			cerr << "ERROR: Additional memory is required for the resizing of the array of Letter Nodes (LN) in the MergedTrie" << endl;
			exit(EXIT_MTRIE);
		}
		memcpy(bloqueNLtemp, bloqueNL_1, sizeof(_NMTRIE_) * tamBloqueNL);
		memset(&(bloqueNLtemp[tamBloqueNL]), 0, sizeof(_NMTRIE_) * (tamBNLtemp - tamBloqueNL));
		delete [] bloqueNL_1;
		bloqueNL_1 = bloqueNLtemp;
		bloqueNLtemp = NULL;

		tamBloqueNL = tamBNLtemp;

		cout << "\t\t\tResizing is concluded" << endl;
	}
}

void MergedTrie::AmpliarArrayNIT(_INDICE_NMTRIE& numNL, _INDICE_NMTRIE& tamBloqueNL, _NIT_*& bloqueNL_1, const string& origen) {
	_NIT_ *bloqueNLtemp;
	unsigned int *bloqueNLtempDatos;

	while(numNL > tamBloqueNL) {
		tablaAmpliada = true;
		//    		cout << "\n\n*************************************   ESQUELETO MergedTrie ANTES DE_AmpliarArray: " << *this << endl;
		_INDICE_NMTRIE tamBNLtemp = tamBloqueNL * __INC_BN__;
		while(numNL >= tamBNLtemp)
			tamBNLtemp *= __INC_BN__;
		cout << "WARNING Resizing the array of Extended InterTrie Nodes (IT) in the MergedTrie from " << tamBloqueNL << " to " << tamBNLtemp << "..." << endl;

		bloqueNLtemp = new _NIT_[tamBNLtemp];
		if(dataMT)
			bloqueNLtempDatos = new unsigned int[tamBNLtemp];
		else
			bloqueNLtempDatos = NULL;

		if(!bloqueNLtemp) {
			cerr << "ERROR: Additional memory is required for the resizing of the array of Extended InterTrie Nodes (IT) in the MergedTrie" << endl;
			exit(EXIT_MTRIE);
		}
		memcpy(bloqueNLtemp, bloqueNL_1, sizeof(_NIT_) * tamBloqueNL);
		memset(&(bloqueNLtemp[tamBloqueNL]), 0, sizeof(_NIT_) * (tamBNLtemp - tamBloqueNL));

		delete [] bloqueNL_1;
		bloqueNL_1 = bloqueNLtemp;
		bloqueNLtemp = NULL;

		if(dataMT) {
			memcpy(bloqueNLtempDatos, bloqueDataNIT, sizeof(unsigned int) * tamBloqueNL);
			memset(&(bloqueNLtempDatos[tamBloqueNL]), 0, sizeof(unsigned int) * (tamBNLtemp - tamBloqueNL));
			delete [] bloqueDataNIT;
			bloqueDataNIT = bloqueNLtempDatos;
			bloqueNLtempDatos = NULL;
		}

		tamBloqueNL = tamBNLtemp;

		cout << "\t\t\tResizing is concluded" << endl;
	}
}

// Crea un conjunto de numNodosAcrear consecutivos
// Devuelve la direccion punteroN al primer nodoB creado de ese conjunto
// Devuelve por parametro el puntero a esa casilla
void MergedTrie::GetNuevoNL(_NMTRIE_*& actual, _INDICE_NMTRIE& indAdevolver, const int& numNodosAcrear) {
	indAdevolver = numNL;
	numNL = numNL + numNodosAcrear;
	if(numNL > tamBloqueNL) {
		AmpliarArrayNL(numNL, tamBloqueNL, bloqueNL, "NL");
	}

	if(tablaAmpliada) {
		GetNL(busqIndiceActual, busqActual);
		GetNL(busqIndicePadre, busqPadre);
		GetNL(busqIndicePrincipioTH, busqPrincipioTH);
		GetNL(busqIndiceRaiz_IT, busqRaiz_IT);
		tablaAmpliada = false;
	}

	// Devolviendo la posicion de la nueva casilla
	GetNL(indAdevolver, actual);
	//    	cout << "\tNuevo actual: " << *actual << " en posicion indAdevolver: " << indAdevolver << endl;
}

// Crea un conjunto de numNodosAcrear consecutivos
// Devuelve en indAdevolver el indice a esa casilla
void MergedTrie::GetNuevoNIT(_INDICE_NMTRIE& indAdevolver, const int& numNodosAcrear) {
	indAdevolver = numNIT;
	numNIT = numNIT + numNodosAcrear;
	if(numNIT > tamBloqueNIT) {
		AmpliarArrayNIT(numNIT, tamBloqueNIT, bloqueNIT, "NIT");
	}
}

// Crea un conjunto de numNodosAcrear consecutivos
// Devuelve la direccion punteroN al primer nodoB creado de ese conjunto
// Devuelve por parametro el puntero a esa casilla
void MergedTrie::GetNuevoNChar(unsigned char*& actual, _INDICE_NMTRIE& indAdevolver, const int& numNodosAcrear) {
	indAdevolver = numNChar;
	numNChar = numNChar + numNodosAcrear;
	if(numNChar > tamBloqueNChar) {
		AmpliarArrayChar(numNChar, tamBloqueNChar, bloqueNChar);
	}
	// Devolviendo la posicion de la nueva casilla
	GetNChar(indAdevolver, actual);
	//    	cout << "\tNuevo actual: " << *actual << " en posicion indAdevolver: " << indAdevolver << endl;
}

// Crea THNL de tamanyo B
// RECIBE: B, tamanyo de la tabla a crear
//		(busqIndiceActual, busqActual) indice del NL/NLMov donde crear la TH (el cual podria tener una TH previa)
// DEVUELVE: (busqPrincipioTH, busqIndicePrincipioTH) puntero a primera posicion tabla creada (su NP)
//		busq_B, B de la TH creada
//		Actualiza: 	busqIndiceActual.prim al enlace con busqIndicePrincipioTH
//					busqIndiceActual.B = B
//					busqIndiceActual.prim.NP
//					busqIndiceActual.prim.NP.indPadre
//		Dejara el mismo NP que tenia antes con indicePadre, BNIT e indTHNIT
void MergedTrie::CrearTHNL(const _INDICE_NMTRIE& B) {
#ifdef __MODO_SEGURO__
	if(EsNLS(*busqActual)) {
		cerr << "ERROR_CrearTHNL. NL es NLS" << endl;
		exit(EXIT_MTRIE);
	}
	if(busqActual != &(bloqueNL[busqIndiceActual])) {
		cerr << "ERROR_CrearTHNL. busqActual != busqIndiceActual" << endl;
		exit(EXIT_MTRIE);
	}
#endif
	busq_B = B;
	if(EsNumNoCeroPot2(B))
		SetBNLpot2(*busqActual);
	else
		UnSetBNLpot2(*busqActual);

	//		if(EsNPdoble(*busqActual))
	GetNuevoNL(busqPrincipioTH, busqIndicePrincipioTH, busq_B + 2);	// El +2 por el NP1/2. Siempre se generaran 2 NP porque hay que guardar el indNPfijo
	//		else
	//			GetNuevoNL(busqPrincipioTH, busqIndicePrincipioTH, busq_B + 1);	// El +1 por el NP

	_INDICE_NMTRIE indTemp, indTemp2;
	GetPrim(*busqActual, indTemp);
	(*busqPrincipioTH) = bloqueNL[indTemp];
	if(EsNPdoble(*busqActual))
		bloqueNL[busqIndicePrincipioTH + 1] = bloqueNL[indTemp + 1];
#ifdef __MODO_SEGURO__
	else
		SetNP(bloqueNL[busqIndicePrincipioTH + 1]);
#endif

	if(!EsNPmov(*busqActual)) {
		SetNPmov(*busqActual);
#ifdef __MODO_SEGURO__
		SetNP(bloqueNL[busqIndicePrincipioTH + 1]);
#endif
		SetIndNPfijo_Inicializando(bloqueNL[busqIndicePrincipioTH + 1], indTemp);
	}

	SetPrim_Inicializando(*busqActual, busqIndicePrincipioTH);

	SetBNL_Inicializando(*busqActual, B);
}

bool MergedTrie::EsNumPot2(const _INDICE_NMTRIE& i) const {
	return (i != 0 && (i & (i-1)) == 0);
}

bool MergedTrie::EsNumNoCeroPot2(const _INDICE_NMTRIE& i) const {
	return ((i & (i-1)) == 0);
}

// Crea THNIT de tamanyo busq_B
// RECIBE: busq_B, tamanyo de la tabla a crear
//		(busqIndiceActual, busqActual) indice del NL de la THNIT a crear
// DEVUELVE: (busqIndicePrincipioTH) indice a primera posicion tabla creada
//		Actualiza: 	NP(busqIndiceActual).indTHNIT al enlace con indicePrimeraPosHash
//					NP(busqIndiceActual).BNIT = B
void MergedTrie::CrearTHNIT() {
#ifdef __MODO_SEGURO__
	if(busqActual != &(bloqueNL[busqIndiceActual])) {
		cerr << "ERROR_CrearTHNIT. busqActual != busqIndiceActual" << endl;
		exit(EXIT_MTRIE);
	}
#endif
	_INDICE_NMTRIE busqIndiceActualNP;

	GetNuevoNIT(busqIndicePrincipioTH, busq_B);
	GetPrim(*busqActual, busqIndiceActualNP);

	if(EsNumNoCeroPot2(busq_B))
		SetBNITpot2(*busqActual);
	else
		UnSetBNITpot2(*busqActual);

	SetBNIT_Inicializando(*busqActual, bloqueNL[busqIndiceActualNP], busq_B);
	SetIndTHNIT_Inicializando(bloqueNL[busqIndiceActualNP], busqIndicePrincipioTH);
}

// Crea NL en NL1 que puede estar inicialmente NO vacio (con letra) creando el NP1 e inicializando Prim a ese NP, e indPadre
void MergedTrie::CrearNL(_NMTRIE_& NL1, const _INDICE_NMTRIE& letra, const _INDICE_NMTRIE& indPadre) {
	_INDICE_NMTRIE indTemp;
	_NMTRIE_* pTemp;

	//		cout << "\t\t\tCrearNL. ANTES: indNL1: " << indNL1 << " indPadre: " << indPadre << " Nodo creado: "; ImprimirNL(pNL1, pNL2, indNL1, cout); cout << endl;
	SetLetra(NL1, letra);

	GetNuevoNL(pTemp, indTemp);
#ifdef __MODO_SEGURO__
	SetNP(*pTemp);
#endif
	SetIndNLPadre_Vacio(*pTemp, indPadre);

	SetPrim_Vacio(NL1, indTemp);
	//		_INDICE_NMTRIE indPrim; GetPrim(NL1, indPrim); cout << "\t\t\tCrearNL. DESPUES: letra: " << letra << " indPadre: " << indPadre << " indTemp: " << indTemp << " indPrim: " << indPrim << endl;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ FUNCIONES DE BUSQUEDA
// Busca una palabra de forma descendente. NO busca datos
// DEVUELVE (variables locales de la clase para que se usen en la insercion sin causar ineficiencia de paso de parametros a funcion):
//	* true si existe toda la cadena s en los trozos correspondientes, y los nodosIT correspondiente
//		- busqIndiceActual = indice que apunta al ultimo nodoIT. Habria que comprobar POSTERIORMENTE en find(const string& s, const int& numMergedTrie, const bool& buscarDato, _INDICE_NMTRIE& dirDato) si tiene el bit de fin de palabra a 1 para trie numMergedTrie
//		- busqActual = puntero al nodo que apunta busqIndiceActual
// 	* false si falla en la cadena s o la de nodosIT, en cuyo caso devolveria
//		- busqNumTrieFallo = 1 si falla en busq. trie1, 2 si en trie2, 3 si en idTrie1 de nodoIT
//		- busq_i = numero de caracter que falla de s
//		- busqIndicePadre = indice que apunta al padre de busqIndiceActual, es decir, caracter (busq_i - 1) que si se encuentra (busq_i falla)
//		- busqPadre = puntero al nodo que apunta busqIndicePadre
//		- busqIndiceActual = indice que apunta al ultimo nodo tratado, desde el que habria que continuar la insercion
//			+ si es nodo vacio, se corresponderia a la posicion donde insertar el nodo
//			+ si no es nodo vacio:
//				_ y coincide con busqIndicePadre, entonces es que prim de busqIndicePadre == 0
//				_ y NO coincide con busqIndicePadre, entonces es que hay que ampliar la tabla
//		- busqActual = puntero al nodo que apunta busqIndiceActual
bool MergedTrie::find(const string& s) {
	//#define _TRAZA_BUSCARPAL_
#ifdef _TRAZA_BUSCARPAL_
	cout << endl << "\n\n************ find: " << s << endl;
	//		cout << "Esqueleto MergedTrie antes de buscar: " << *this << endl;
#endif
	busqSLength = s.size();
	busqTrie2Inicio = DivideStringTrie2(busqSLength);	// En el ultimo caracter de trie1 no hay que profundizar, sino buscar un nodoIT

	////////////////////////////////////////////////////////////// Recorrido del trie1
	if(!BuscarPalabraAuxTrie1(s))
		return false;

	////////////////////////////////////////////////////////////// Recorrido del trie2
	if(!BuscarPalabraAuxTrie2(s))
		return false;

	////////////////////////////////////////////////////////////// Tratamiento nodos IT
	return BuscarPalabraAuxIT();
}

// DEVUELVE:
//	  - (busqIndicePadre, busqPadre) NL padre de (busqIndiceActual, busqActual)
//    - true si encuentra la subcadena de s en el trie1. Devolviendo (busqIndiceActual, busqActual) (busqIndiceRaiz_IT, busqRaiz_IT) apuntando al NL final de la subcadena, en cuya THNIT habra que buscar el IT2 correspondiente
//	  - false si no lo encuentra. Devolviendo:
//			(busqIndiceActual, busqActual) el ultimo NL visitado o vacio si no existe el NL donde debe estar la letra
//			En caso que EsNLS(*busqActual), devolvera s[busq_i] el caracter que falla con el que no coincide en bloqueNChar[busqIndiceNChar]
bool MergedTrie::BuscarPalabraAuxTrie1(const string& s) {
#ifdef _TRAZA_BUSCARPAL_
	cout << "################### \tBuscarPalabraAuxTrie1. s: " << s << " busq_i: " << busq_i << " busqTrie2Inicio: " << busqTrie2Inicio << " ###################" << endl;
#endif
	busqNumTrieFallo = 1;
	busqIndicePadre = busqIndiceActual = raizTrie1;
	busqActual = bloqueNL + busqIndiceActual;	// La raiz siempre va estar en bloqueNL por eso no se usa GetNL
	busqPadre = busqActual;
	busqFalloNLS = false;

	for(busq_i = 0; busq_i < busqTrie2Inicio; ++busq_i) {
		busqLetraTemp = (unsigned char) s[busq_i];
		if(!BuscarHijo()) {
#ifdef _TRAZA_BUSCARPAL_
			cout << "\tBuscarPalabraAuxTrie1: Falla en trie1. busq_i: " << busq_i << " busqTrie2Inicio: " << busqTrie2Inicio << endl;
#endif
			return false;
		}

		if(EsNLS(*busqActual)) {
			GetIndNLS_DeUnNL(*busqActual, busqIndiceNChar);
#ifdef _TRAZA_BUSCARPAL_
			cout << "\tBuscarPalabraAuxTrie1. Es NLS. INICIO. bloqueNChar[" << busqIndiceNChar << "] = " << (char) bloqueNChar[busqIndiceNChar] << " vs. " << " s[" << busq_i << "] = " << (char) s[busq_i] << endl;
#endif
			for(++busq_i, busqLetraTemp = (unsigned char) s[busq_i]; busq_i < busqTrie2Inicio && bloqueNChar[busqIndiceNChar] == busqLetraTemp && bloqueNChar[busqIndiceNChar] != '\0'; ++busq_i, busqLetraTemp = (unsigned char) s[busq_i], ++busqIndiceNChar) {
#ifdef _TRAZA_BUSCARPAL_
				cout << "\tBuscarPalabraAuxTrie1: Exito en trie1.NLS. bloqueNChar[" << busqIndiceNChar << "] = " << (char) bloqueNChar[busqIndiceNChar] << " == " << " s[" << busq_i << "] = " << (char) s[busq_i] << endl;
#endif
			}
			if(bloqueNChar[busqIndiceNChar] != '\0') {		// Si no se consume todo el NLS
#ifdef _TRAZA_BUSCARPAL_
				cout << "\tBuscarPalabraAuxTrie1: Falla en trie1.NLS porque bloqueNChar[" << busqIndiceNChar << "] = " << bloqueNChar[busqIndiceNChar] << " != EOS. s[" << busq_i << "] = " << (char) s[busq_i] << endl;
#endif
				busqFalloNLS = true;
				return false;
			}
			if(busq_i < busqTrie2Inicio) {		// O no se consume todo el string de busqueda
#ifdef _TRAZA_BUSCARPAL_
				cout << "\tBuscarPalabraAuxTrie1: Falla en trie1.NLS porque busq_i: " << busq_i << " < busqTrie2Inicio: " << busqTrie2Inicio << endl;
#endif
				busqFalloNLS = true;
				return false;
			}
			--busq_i;	// Porque for(...; ++busq_i)
			busqLetraTemp = (unsigned char) s[busq_i];
		}
#ifdef _TRAZA_BUSCARPAL_
		else {
			cout << "\tBuscarPalabraAuxTrie1: Exito en trie1. busqTrie2Inicio: " << busqTrie2Inicio << " busqIndiceActual: " << busqIndiceActual << " busqActual: "; ImprimirNL(busqIndiceActual, cout); cout << endl;
		}
#endif
		busqIndicePadre = busqIndiceActual;
		busqPadre = busqActual;
	}	// Fin del for de busqueda de la cadena

	busqRaiz_IT = busqActual;
	busqIndiceRaiz_IT = busqIndiceActual;

	//		if(!busqInsertar && !EsTHNIT(*busqRaiz_IT))  {
	//#ifdef _TRAZA_BUSCARPAL_
	//			cout << "\tBuscarPalabraAuxTrie1: Falla en trie1 porque busqIndiceRaiz_IT: " << busqIndiceRaiz_IT << " No EsTHNIT. busqTrie2Inicio: " << busqTrie2Inicio << " busqIndiceActual: " << busqIndiceActual << " busqActual: "; ImprimirNL(busqIndiceActual, cout); cout << endl;
	//#endif
	//			return false;
	//		}

#ifdef __MODO_SEGURO__
	if(busqActual != &(bloqueNL[busqIndiceActual])) {
		cerr << "ERROR_BuscarPalabraAuxTrie1. busqActual != busqIndiceActual" << endl;
		exit(EXIT_MTRIE);
	}
#endif
	return true;
}

// DEVUELVE:
//	  - (busqIndicePadre, busqPadre) NL padre de (busqIndiceActual, busqActual)
//    - true si encuentra la subcadena de s en el trie2. Devolviendo (busqIndiceActual, busqActual) (busqIndiceTrie2) apuntando al NL final de la subcadena, el cual habra que buscar en THNIT
//	  - false si no lo encuentra. Devolviendo:
//			(busqIndicePrincipioTH) al ppio de la TH donde falla/acierta
//			(busq_B) como B de la TH donde falla/acierta
//			(busqIndiceActual, busqActual) el ultimo NL visitado o vacio si no existe el NL donde debe estar la letra
//			En caso que EsNLS(*busqActual), devolvera s[busq_i] el caracter que falla con el que no coincide en bloqueNChar[busqIndiceNChar]
bool MergedTrie::BuscarPalabraAuxTrie2(const string& s) {
#ifdef _TRAZA_BUSCARPAL_
	cout << "################### \tBuscarPalabraAuxTrie2. s: " << s << " busq_i: " << busq_i << " busqTrie2Inicio: " << busqTrie2Inicio << " ###################" << endl;
#endif
	busqNumTrieFallo = 2;
	busqIndicePadre = busqIndiceActual = raizTrie1;
	busqFalloNLS = false;
	busqActual = bloqueNL + busqIndiceActual;	// La raiz siempre va estar en bloqueNL por eso no se usa GetNL
	busqPadre = busqActual;

	if(busqSLength != 1) {	// Sino indiceTrie2 == raiz
		for(busq_i = busqSLength-1; busq_i >= busqTrie2Inicio; --busq_i) {
			busqLetraTemp = (unsigned char) s[busq_i];
			if(!BuscarHijo()) {
#ifdef _TRAZA_BUSCARPAL_
				cout << "\tBuscarPalabraAuxTrie2: Falla en trie2. busq_i: " << busq_i << " busqTrie2Inicio: " << busqTrie2Inicio << endl;
#endif
				return false;
			}

			if(EsNLS(*busqActual)) {
				GetIndNLS_DeUnNL(*busqActual, busqIndiceNChar);
#ifdef _TRAZA_BUSCARPAL_
				cout << "\tBuscarPalabraAuxTrie2. Es NLS. INICIO. bloqueNChar[" << busqIndiceNChar << "] = " << (char) bloqueNChar[busqIndiceNChar] << " vs. " << " s[" << busq_i << "] = " << (char) s[busq_i] << endl;
#endif
				for(--busq_i, busqLetraTemp = (unsigned char) s[busq_i]; busq_i >= busqTrie2Inicio && bloqueNChar[busqIndiceNChar] == busqLetraTemp && bloqueNChar[busqIndiceNChar] != '\0'; --busq_i, busqLetraTemp = (unsigned char) s[busq_i], ++busqIndiceNChar) {
#ifdef _TRAZA_BUSCARPAL_
					cout << "\tBuscarPalabraAuxTrie2: Exito en trie2.NLS. bloqueNChar[" << busqIndiceNChar << "] = " << (char) bloqueNChar[busqIndiceNChar] << " == " << " s[" << busq_i << "] = " << (char) s[busq_i] << endl;
#endif
				}
				if(bloqueNChar[busqIndiceNChar] != '\0') {		// Si no se consume todo el NLS
#ifdef _TRAZA_BUSCARPAL_
					cout << "\tBuscarPalabraAuxTrie2: Falla en trie2.NLS porque bloqueNChar[" << busqIndiceNChar << "] != EOS. s[" << busq_i << "] = " << (char) s[busq_i] << endl;
#endif
					busqFalloNLS = true;
					return false;
				}
				if(busq_i >= busqTrie2Inicio) {		// O no se consume todo el string de busqueda
#ifdef _TRAZA_BUSCARPAL_
					cout << "\tBuscarPalabraAuxTrie2: Falla en trie2.NLS porque busq_i: " << busq_i << " >= busqTrie2Inicio: " << busqTrie2Inicio << endl;
#endif
					busqFalloNLS = true;
					return false;
				}
				++busq_i;	// Porque for(...; --busq_i)
				busqLetraTemp = (unsigned char) s[busq_i];
			}
#ifdef _TRAZA_BUSCARPAL_
			else {
				cout << "\tBuscarPalabraAuxTrie2: Exito en trie2. busqTrie2Inicio: " << busqTrie2Inicio << " busqIndiceActual: " << busqIndiceActual << " busqActual: "; ImprimirNL(busqIndiceActual, cout); cout << endl;
			}
#endif
			busqIndicePadre = busqIndiceActual;
			busqPadre = busqActual;
		}	// Fin del for de busqueda de la cadena
		busqIndiceTrie2 = busqIndiceActual;
	}
	else {
		busqActual = bloqueNL + busqIndiceActual;	// La raiz siempre va estar en bloqueNL por eso no se usa GetNL
		busqIndiceTrie2 = busqIndiceActual;
	}

#ifdef __MODO_SEGURO__
	if(busqActual != &(bloqueNL[busqIndiceActual])) {
		cerr << "ERROR_BuscarPalabraAuxTrie2. busqActual != busqIndiceActual" << endl;
		exit(EXIT_MTRIE);
	}
#endif
	return true;
}

// Recorre la tabla hash de descendientes NL de busqIndicePadre
// RECIBE:
//  * busqLetraTemp letra a buscar en busq_i
//  * busqIndicePadre/busqPadre nodo padre a partir del cual hay que hacer la busqueda
//	* busqNumTrieFallo que indica el tipo de busqueda que se esta realizando (nodosIT o nodosLetra) segun lo explicado en find
// DEVUELVE:
//			(busqIndicePrincipioTH) al ppio de la TH donde falla/acierta
//			(busq_B) como B de la TH donde falla/acierta (ya sea de NBucket/NL/NIT)
//			(busqIndiceActual, busqActual) el ultimo NL visitado o vacio si no existe el NL donde debe estar la letra
//  * true si encuentra el nodo con la letra busqLetraTemp (correspondiente a busq_i de s o busq_stringNIT) en la tabla de descendientes de busqIndicePadre/busqPadre
//	       	busqIndiceActual/busqActual apuntando al NL que contiene
//			Si EsNLS(*busqActual) se comprobara en BuscarPalabraAuxTrie1/2 si es true/false la busqueda
//  * false si no lo encuentra, devolviendo:
//    	- busqIndiceActual == busqIndicePadre si busqIndicePadre.B == 0
//    	- busqIndiceActual != busqIndicePadre && (*busqActual) == 0 si la casilla donde debe estar es vacia
//											  && (*busqActual) != 0 si no coincide con busqLetraTemp
bool MergedTrie::BuscarHijo() {
	//#define _TRAZA_BUSCARHIJO
#ifdef _TRAZA_BUSCARHIJO
	cout << "***************BuscarHijo. letra numero busq_i: " << busq_i << " letra: " << (char) busqLetraTemp;
	cout << " busqNumTrieFallo: " << busqNumTrieFallo;
	cout << " Nodo padre. busqIndicePadre: " << busqIndicePadre << " busqPadre: "; ImprimirNL(busqIndicePadre, cout); cout << endl;
#endif

#ifdef __MODO_SEGURO__
	if(busqActual != &(bloqueNL[busqIndiceActual])) {
		cerr << "ERROR_BuscarHijo. busqActual != busqIndiceActual" << endl;
		exit(EXIT_MTRIE);
	}
	if(busqPadre != &(bloqueNL[busqIndicePadre])) {
		cerr << "ERROR_BuscarHijo. busqPadre != busqIndicePadre" << endl;
		exit(EXIT_MTRIE);
	}
#endif
	GetBNL(*busqPadre, busq_B);

	if(busq_B == 0) {
#ifdef _TRAZA_BUSCARHIJO
		cout << "\t\tBuscarHijo. No lo encuentra. busqIndicePadre.prim == 0" << endl;
#endif
		busqActual = busqPadre;	// Para que busqActual != 0 y forzar que se amplie la tabla en CrearNuevaRama
		busqIndiceActual = busqIndicePadre;
		return false;
	}

	GetPrim(*busqPadre, busqIndicePrincipioTH);
	if(EsNPdoble(*busqPadre)) {
		busqIndicePrincipioTH += 2;
		busqPrincipioTH += 2;	// Por el NP inicial
	}
	else {
		++busqIndicePrincipioTH; ++busqPrincipioTH;	// Por el NP inicial
	}

	if(EsBNLpot2(*busqPadre))
		ModPot2(busqLetraTemp, busq_B, busqPosHash);
	else
		ModNoPot2_256(busqLetraTemp, busq_B, busqPosHash);
	busqIndiceActual = busqIndicePrincipioTH + busqPosHash;
	GetNL(busqIndiceActual, busqActual);
#ifdef _TRAZA_BUSCARHIJO
	cout << "\tBuscarHijo. Empieza la busqueda de busqLetraTemp: " << (char) busqLetraTemp << " en THNL de busq_B: " << busq_B << " que empieza en busqIndicePrincipioTH: " << busqIndicePrincipioTH << " busqPosHash: " << busqPosHash << " busqIndiceActual: " << busqIndiceActual << ": " << endl;
	if(!EsNLS(*busqActual))
		ImprimirNL(busqIndiceActual, cout);
#endif

	if((*busqActual) == 0) {
#ifdef _TRAZA_BUSCARHIJO
		cout << "\tBuscarHijo. No lo encuentra porque busqIndiceActual: " << busqIndiceActual << " es una casilla vacia" << endl;
#endif
		return false;
	}
	else {
#ifdef __MODO_SEGURO__
		if(!EsNL(*busqActual)) {
			cerr << "ERROR_BuscarHijo. !EsNL(*busqActual). busqIndiceActual: " << busqIndiceActual << endl;
			exit(EXIT_MTRIE);
		}
#endif
		GetLetra(*busqActual, busqLetra);

		if(busqLetraTemp == busqLetra) {
#ifdef _TRAZA_BUSCARHIJO
			cout << "\tBuscarHijo. Encuentra busqLetraTemp: " << (char) busqLetraTemp << " en busqIndiceActual: " << busqIndiceActual << " busqActual: "; ImprimirNL(busqIndiceActual, cout); cout << endl;
#endif
			return true;
		}
		else {
#ifdef _TRAZA_BUSCARHIJO
			cout << "\tBuscarHijo. No coincide la letra del NL: " << (char) busqLetra << " con la letra buscada: " << (char) busqLetraTemp << " en busqIndiceActual: " << busqIndiceActual << " busqActual: "; ImprimirNL(busqIndiceActual, cout); cout << endl;
#endif
			GetColisNL(*busqActual, busqIndiceTemp);

			if(busqIndiceTemp == 0) {
#ifdef _TRAZA_BUSCARHIJO
				cout << "\tBuscarHijo. No lo encuentra porque NO Hay colisiones" << endl;
#endif
				return false;
			}
			else {
				while(true) {
					busqIndiceFinTH = busqIndicePrincipioTH + busq_B - 1;
					ObtenerSiguienteColision(busqIndicePrincipioTH, busqIndiceFinTH, busqIndiceActual, busqIndiceTemp, busqIndiceActual);
					GetNL(busqIndiceActual, busqActual);
#ifdef _TRAZA_BUSCARHIJO
					cout << "\tBuscarHijo. Hay colisiones sin BNL2, por lo que sigue buscando en posicion: " << busqIndiceActual << " busqIndicePrincipioTH: " << busqIndicePrincipioTH << " busqIndiceFinTH: " << busqIndiceFinTH << endl;
#endif
					if(*busqActual == 0) {
#ifdef _TRAZA_BUSCARHIJO
						cout << "\tBuscarHijo. No lo encuentra porque la posicion busqIndiceActual: " << busqIndiceActual << " esta vacia" << endl;
#endif
						return false;
					}

					GetLetra(*busqActual, busqLetra);
					if(busqLetraTemp == busqLetra) {
#ifdef _TRAZA_BUSCARHIJO
						cout << "\tBuscarHijo. Encuentra busqLetraTemp: " << (char) busqLetraTemp << " en busqIndiceActual: " << busqIndiceActual << " busqActual: "; ImprimirNL(busqIndiceActual, cout); cout << endl;
#endif
						return true;
					}
#ifdef _TRAZA_BUSCARHIJO
					cout << "\tBuscarHijo. No coincide la letra del NL: " << (char) busqLetra << " con la letra buscada: " << (char) busqLetraTemp << " en busqIndiceActual: " << busqIndiceActual << " busqActual: "; ImprimirNL(busqIndiceActual, cout); cout << endl;
#endif
					GetColisNL(*busqActual, busqIndiceTemp);

					if(busqIndiceTemp == 0) {
#ifdef _TRAZA_BUSCARHIJO
						cout << "\tBuscarHijo. No lo encuentra porque NO Hay colisiones" << endl;
#endif
						return false;
					}
				}
			}
		}
	}
}

// Busca (busqIndiceTrie2) (el indice del NP de trie2) en la THNL del final de trie1 (busqIndiceRaiz_IT, busqRaiz_IT)
// RECIBE:
//  * (busqIndiceTrie2) a buscar
//  * (busqIndiceRaiz_IT, busqRaiz_IT) nodo que contiene la THNIT en la que hay que hacer la busqueda
// DEVUELVE:
//			(busqIndicePrincipioTH) al ppio de la TH donde falla/acierta
//			(busq_B) como B de la TH donde falla/acierta
//			(busqIndiceActual, busqActual) el ultimo NL visitado o vacio si no existe el NL donde debe estar la letra
//  * true si encuentra el NIT
//	       	busqIndiceActual/busqActual apuntando al NIT
//  * false si no lo encuentra, devolviendo:
//    	- busqIndiceActual == busqIndicePadre si no existe THNIT
//    	- busqIndiceActual != busqIndicePadre && (*busqActual) == 0 si la casilla donde debe estar es vacia
//											  && (*busqActual) != 0 si no coincide con NIT. Si BNIT == 1 && EsNIT(*busqPrincipioTH) entonces busqTrieFalloBes1 = true (habria que crear la TH de NBucket)
//																						    Sino busqTrieFalloBes1 = false (habria que ampliar la TH de D)
bool MergedTrie::BuscarPalabraAuxIT() {
	//#define _TRAZA_BUSCARAUXIT
#ifdef __BUSQ_SOLO_NL__
	return true;
#endif

	busqNumTrieFallo = 3;
#ifdef __MODO_SEGURO__
	if(busqNumTrieFallo == 3 && (busqIndiceTrie2 == 0 || busqIndiceRaiz_IT == 0)) {
		cerr << "ERROR_BuscarPalabraAuxIT. busqIndiceTrie2 = " << busqIndiceTrie2 << " == 0 || busqIndiceRaiz_IT = " << busqIndiceRaiz_IT << " == 0" << endl;
		exit(EXIT_MTRIE);
	}
	if(busqRaiz_IT != &(bloqueNL[busqIndiceRaiz_IT])) {
		cerr << "ERROR_BuscarPalabraAuxIT. busqRaiz_IT != busqIndiceRaiz_IT" << endl;
		exit(EXIT_MTRIE);
	}
#endif

#ifdef _TRAZA_BUSCARAUXIT
	cout << "\n\n////////////////// BuscarPalabraAuxIT. busqInsertar: " << busqInsertar << " busqIndiceTrie2: " << busqIndiceTrie2 << ": "; ImprimirNL(busqIndiceTrie2, cout); cout << " busqIndiceRaiz_IT: " << busqIndiceRaiz_IT << " busqRaiz_IT: "; ImprimirNL(busqIndiceRaiz_IT, cout); cout << endl;
#endif
	GetNL(busqIndiceTrie2, busqTrie2);
	busqITesNLS = EsNLS(*busqTrie2);
	GetPrim(*busqTrie2, busqIndiceTrie2);
	if(EsNPmov(*busqTrie2))
		GetIndNPfijo(bloqueNL[busqIndiceTrie2 + 1], busqIndiceTrie2);

#ifdef _TRAZA_BUSCARAUXIT
	cout << "\tBuscarPalabraAuxIT. busqIndiceTrie2.indNPfijo: " << busqIndiceTrie2 << " busqIndiceTrie2.busqITesNLS: " << busqITesNLS << " BF_IT: " << busq_B << " busqIndicePrincipioTH: " << busqIndicePrincipioTH << endl;
#endif

	GetPrim(*busqRaiz_IT, busqIndiceNP);
	GetNL(busqIndiceNP, busqNP);
	GetIndTHNIT(*busqNP, busqIndicePrincipioTH);
	if(busqIndicePrincipioTH == 0) {
#ifdef _TRAZA_BUSCARAUXIT
		cout << "\t\tBuscarPalabraAuxIT. No lo encuentra. busqRaiz_IT no tiene THNIT" << endl;
#endif
		busqIndiceActual = busqIndicePadre = busqIndiceRaiz_IT;		// Asi se detecta en CrearNuevaRama para que se cree la THNIT
		busqActual = busqPadre = busqRaiz_IT;
		return false;
	}

	GetBNIT(*busqRaiz_IT, *busqNP, busq_B);

	busqEsBNITpot2 = EsBNITpot2(*busqRaiz_IT);
	if(busqEsBNITpot2) {
		ModPot2(busqIndiceTrie2, busq_B, busqPosHash);
		GetC(busqIndiceTrie2, busq_B, busqC);
	}
	else {
		ModNoPot2(busqIndiceTrie2, busq_B, busqPosHash);
		busqC = 1;
	}
	busqIndiceActual = busqIndicePrincipioTH + busqPosHash;
	busqIndiceFinTH = busqIndicePrincipioTH + busq_B - 1;

	busqIndicePadre = 0;	// Para que no coincida con busqIndiceActual, para que no entre en el caso de crear la THNIT
	busqPadre = bloqueNL;

	busq_iB = 1;
	busqIndiceTemp = 1;
	while(true) {
#ifdef __MODO_SEGURO__
		GetNIT(busqIndiceActual, busqActualNIT);
#endif
		if(bloqueNIT[busqIndiceActual] == 0) {
#ifdef _TRAZA_BUSCARAUXIT
			cout << "\tBuscarPalabraAuxIT. No lo encuentra porque la posicion busqIndiceActual: " << busqIndiceActual << " esta vacia" << endl;
#endif
			return false;
		}

		GetIT(bloqueNIT[busqIndiceActual], busqIT);
		if(busqIndiceTrie2 == busqIT) {
#ifdef _TRAZA_BUSCARAUXIT
			cout << "\tBuscarPalabraAuxIT. Encuentra busqIndiceTrie2: " << busqIndiceTrie2 << " en busqIndiceActual: " << busqIndiceActual << " busqActual: "; ImprimirNIT(bloqueNIT[busqIndiceActual], cout); cout << endl;
#endif
			return true;
		}
#ifdef _TRAZA_BUSCARAUXIT
		cout << "\tBuscarPalabraAuxIT. No coincide el NIT: " << busqIT << " con el IT buscado: " << busqIndiceTrie2 << " en busqIndiceActual: " << busqIndiceActual << " busqActual: "; ImprimirNIT(bloqueNIT[busqIndiceActual], cout); cout << endl;
#endif

		if(!GetColisNIT(bloqueNIT[busqIndiceActual])) {
#ifdef _TRAZA_BUSCARAUXIT
			cout << "\tBuscarPalabraAuxIT. No lo encuentra porque NO Hay colisiones" << endl;
#endif
			return false;
		}

		++busq_iB;
		if(busq_iB > busq_B) {
#ifdef _TRAZA_BUSCARAUXIT
			cout << "\tBuscarPalabraAuxIT. No lo encuentra porque busq_iB > busq_B" << endl;
#endif
			return false;
		}

		if(busqEsBNITpot2) {
			busqPosHash += busqC;
			ModPot2(busqPosHash, busq_B, busqIndiceActual);
			busqIndiceActual += busqIndicePrincipioTH;
		}
		else
			//	    		ModNoPot2(busqPosHash, busq_B, busqIndiceActual);
			ObtenerSiguienteColision(busqIndicePrincipioTH, busqIndiceFinTH, busqIndiceActual, busqC, busqIndiceActual);
	}
}

void MergedTrie::GetC(const _INDICE_NMTRIE& clave, const unsigned int& B, unsigned int& c) {
	//#define _TRAZA_GETC_

	if(B & 1) {	// B es impar
		c = arrayC[clave & 3];
#ifdef _TRAZA_GETC_
		cout << " B: " << B << " impar. clave: " << clave << " clave % 4: " << (clave % 4) << " c: " << c << endl;
#endif
	}
	else {
		//			c = arrayC[4 + (clave % 16)];
		c = arrayC[4 + (clave & 15)];
#ifdef _TRAZA_GETC_
		cout << " B: " << B << " par. clave: " << clave << " clave % 16: " << (clave % 16) << " c: " << c << endl;
#endif
	}
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ FUNCIONES DE INSERTAR
// Inserta una palabra. NO INSERTA datos
// DEVUELVE (variables locales de la clase para que se usen en la insercion sin causar ineficiencia de paso de parametros a funcion)::
//	* true si no existia previamente la cadena s en los trozos correspondientes, y la cadena de nodosIT correspondiente
//		- busqIndiceActual = indice que apunta al ultimo nodoIT. Habria que comprobar POSTERIORMENTE en insert(const string& s, const int& numMergedTrie, const bool& buscarDato, _INDICE_NMTRIE& dirDato) si tiene el bit de fin de palabra a 1 para trie numMergedTrie
//		- busqActual = puntero al nodo que apunta busqIndiceActual
// 	* false si no
bool MergedTrie::insert(const string& s)
{
	//#define _TRAZA_INSERTARPAL_
#ifdef _TRAZA_INSERTARPAL_
	cout << endl << "\n\n###########\tInsertarPalabra: " << s << endl;
	//		cout << "Esqueleto MergedTrie antes de insertar: " << *this << endl;
#endif

	//#ifdef __MODO_SEGURO__
	//    	if(s == "00160083") {
	//    		busqError = true;
	//    		cout << "encuentra s: " << s << endl;
	//    	}
	//    	else
	//    		busqError = false;
	//#endif

	busqInsertar = true;
	if(find(s)) {
#ifdef _TRAZA_INSERTARPAL_
		cout << "\t###########\tInsertarPalabra. NO se inserta. La palabra existia previamente" << endl;
#endif
		busqInsertar = false;
		return false;
	}
	estaComprimido = false;

#ifdef _TRAZA_INSERTARPAL_
	cout << "\t###########\tInsertarPalabra. Fallo en la busqueda. busqNumTrieFallo: " << busqNumTrieFallo << endl;
#endif
	if(busqNumTrieFallo == 3) {	// Sgca. que el error se produce al no existir indiceTrie1 + indiceTrie2
		CrearNuevaRama(s);
		++cant;
		busqInsertar = false;
		return true;
	}
	else {	// Sgca. que el error se produce al insertar trie1 o trie2
		CrearNuevaRama(s);
	}
#ifdef _TRAZA_INSERTARPAL_
	cout << "\t###########\tInsertarPalabra. Despues de crear la rama en busqNumTrieFallo: " << busqNumTrieFallo << endl;
#endif
	++busqNumTrieFallo;

	if(busqNumTrieFallo == 2) {	// El fallo de insercion se produjo en trie1
		busqRaiz_IT = busqActual;
		busqIndiceRaiz_IT = busqIndiceActual;

		if(!BuscarPalabraAuxTrie2(s)) {
			CrearNuevaRama(s);
			busqIndiceTrie2 = busqIndiceActual;
		}
#ifdef _TRAZA_INSERTARPAL_
		cout << "\t###########\tInsertarPalabra. Despues de crear la rama en busqNumTrieFallo: " << busqNumTrieFallo;
		cout << endl;
#endif
		++busqNumTrieFallo;
	}
	else	// El fallo de insercion se produjo en trie2
		busqIndiceTrie2 = busqIndiceActual;

	if(busqNumTrieFallo == 3) {
		if(!BuscarPalabraAuxIT()) {
			CrearNuevaRama(s);
		}
	}

#ifdef _TRAZA_INSERTARPAL_
	cout << "\t###########\tInsertarPalabra. PALABRA INSERTADA: " << s << " busqNumTrieFallo: " << busqNumTrieFallo;
	cout << endl;
#endif
	busqInsertar = false;
	++cant;

#ifdef __MODO_SEGURO__
	//		if(busqError) {
	//			cout << "\tInsertarPalabra. find s: " << s << ": " << find(s) << endl;
	//		}
#endif

	return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ FUNCIONES DE CrearNuevaRama

// Divide la cadena almacenada en el NLS segun el proceso de insercion
// RECIBE:
//	- busqIndiceActual/busqActual apunta al NLS y NLMov a dividir
//	- busqIndicePadre/busqPadre nodo padre del NLS
//	- (busq_i, busqIndiceNChar):  busq_i es el caracter que falla en la insercion con el que busqIT no coincide en bloqueNChar[busqIndiceNChar]
// DEVUELVE:
//	- busqIndiceActual/busqActual apuntando al NL donde hacer la insercion de busqIT
//	- Actualiza busqIndicePadre/busqPadre
//	- Usa vNodos
void MergedTrie::DividirNLS() {
	//		Dividir NLS: (prefComun + primLetraSufijoSobrante + sufijoSobrante), (prefComun + primLetraSufijoCadAinsertar + sufijoCadAinsertar)
	//			1) Si hay prefComun y primLetraSufijoCadAinsertar == 0 y sufijoCadAinsertar == 0:
	//					A prefComun se le quitara el ultimo caracter dandoselo a primLetraSufijoSobrante
	//					busq_i se decrementara (ese ultimo caracter de prefComun se dara a primLetraSufijoCadAinsertar
	//			2) Si hay prefComun:
	//					El NLS pasa a NL
	//					Crear tantos NL como letras tenga prefComun.
	//					El ultimo NL creado se hace que apunte a una nueva TH con 2 NL: primLetraSufijoSobrante y primLetraSufijoCadAinsertar
	//			   Sino,
	//					el NLS pasa a NL que apuntará a la nueva TH con 2 NL: primLetraSufijoSobrante y primLetraSufijoCadAinsertar
	//			3) NL.primLetraSufijoSobrante.prim apuntará al nuevo NLS con sufijoSobrante
	//			4) Si sufijoSobrante == 1, el NL final se convierte a NL con solo la última letra.
	//			   Sino se incrementa el indArrayChar hasta el inicio de sufijoSobrante
	//			5) Si sufijoCadAinsertar == 1, entonces crear NL con esa letra, al cual apuntará el NL.sufijoCadAinsertar.prim
	//			6) Si sufijoCadAinsertar > 1, entonces crear NLS con ese sufijo, al cual apuntará el NL.sufijoCadAinsertar.prim
	//			7) Si sufijoCadAinsertar == 0, entonces este nodo será el final de la cadena a insertar (busqUltimaLetraEnPrefijo = true)

	//#define _TRAZA_DIVIDIRNLS_
	bool seDivideIndiceRaiz_IT = false;

	if(busqIndiceActual == busqIndiceRaiz_IT) {
		seDivideIndiceRaiz_IT = true;
#ifdef _TRAZA_DIVIDIRNLS_
		cout << "\t\tDividirNLS. seDivideIndiceRaiz_IT = true ya que busqIndiceRaiz_IT: " << busqIndiceRaiz_IT << " == busqIndiceActual: " << busqIndiceActual << endl;
#endif
	}

	busqUltimaLetraEnPrefijo = false;

#ifdef _TRAZA_DIVIDIRNLS_
	_INDICE_NMTRIE indNLStraza = busqIndiceActual;
	cout << "\t\tDividirNLS. busqIndicePadre: " << busqIndicePadre << " busqIndiceActual: " << busqIndiceActual << " NLS: "; ImprimirNL(busqIndiceActual, cout); cout << endl;
#endif
#ifdef __MODO_SEGURO__
	if(!EsNLS(*busqActual)) {
		cerr << "ERROR_DividirNLS. !EsNLS en busqIndiceActual: " << busqIndiceActual << endl;
		exit(EXIT_MTRIE);
	}
	if(busqActual != &(bloqueNL[busqIndiceActual])) {
		cerr << "ERROR_DividirNLS. busqActual != busqIndiceActual" << endl;
		exit(EXIT_MTRIE);
	}
	if(busqPadre != &(bloqueNL[busqIndicePadre])) {
		cerr << "ERROR_DividirNLS. busqPadre != busqIndicePadre" << endl;
		exit(EXIT_MTRIE);
	}
#endif

	//		_INDICE_NMTRIE indTemp, indNLSnoMov, indNPdelNLS, BNITdelNLS, indTHNITdelNLS, indNLSchar, i, j, indAinsertar_busqUltimaLetraEnPrefijo;
	_INDICE_NMTRIE indTemp, indNPdelNLS, BNITdelNLS, indNLSchar, i, j, indAinsertar_busqUltimaLetraEnPrefijo;
	_NMTRIE_ *pTemp;
	unsigned char *pchar;
	bool tieneTHNIT = EsTHNIT(*busqActual);
	bool esBNITpot2 = EsBNITpot2(*busqActual);

	GetIndNLS_DeUnNL(*busqActual, indNLSchar);
	//		GetPrim(*busqActual, indNLSnoMov);	// Sera el IT almacenado previamente. Ahi se alojara el final del NLS
#ifdef _TRAZA_DIVIDIRNLS_
	//		cout << "\t\tDividirNLS. indNLSnoMov: " << indNLSnoMov << " busq_i: " << busq_i << " char: " << (char) busqLetraTemp << " busqIndiceNChar que falla: " << busqIndiceNChar << " char: " << (char) bloqueNChar[busqIndiceNChar] << " indNLSchar: " << indNLSchar << endl;
	cout << "\t\tDividirNLS. busq_i: " << busq_i << " char: " << (char) busqLetraTemp << " busqIndiceNChar que falla: " << busqIndiceNChar << " char: " << (char) bloqueNChar[busqIndiceNChar] << " indNLSchar: " << indNLSchar << endl;
#endif
	if(!CrearNuevaRamaCondicion()) {
		busqUltimaLetraEnPrefijo = true;
		//			--busqIndiceNChar;
		//			CrearNuevaRamaDecremento();
#ifdef _TRAZA_DIVIDIRNLS_
		cout << "\t\tDividirNLS. Caso en que el prefijo comun contiene la ultima letra de la cad a insertar. Por ello --busqIndiceNChar: " << busqIndiceNChar << " y --busq_i: " << busq_i << endl;
#endif
	}

	GetPrim(*busqActual, indNPdelNLS);	// Para recuperar la THNIT y el BNIT
	GetBNIT(*busqActual, bloqueNL[indNPdelNLS], BNITdelNLS);
	//		GetIndTHNIT(bloqueNL[indNPdelNLS], indTHNITdelNLS);

	UnSetNLS(*busqActual);	// El NLS quedara como NL pero tal cual estaba menos su THNIT y BNIT que se almacenara en el busqIndiceNLS (ultimo NLMov creado al dividir el NLS)

	GetNuevoNL(pTemp, indTemp);
	(*pTemp) = bloqueNL[indNPdelNLS];
	UnSetNPmov(*busqActual);
	SetPrim_Inicializando(*busqActual, indTemp);
	if(tieneTHNIT) {
		SetBNIT_Acero(*busqActual, *pTemp);	// Pierde la THNIT y el BNIT y el indNLS
		UnSetBNITpot2(*busqActual);
		SetIndTHNIT_Acero(*pTemp);
	}

	if(busqIndiceNChar != indNLSchar) {
#ifdef _TRAZA_DIVIDIRNLS_
		cout << "\t\tDividirNLS. Hay prefijo comun" << endl;
#endif
		for(; indNLSchar != busqIndiceNChar ; ++indNLSchar) {
			if((indNLSchar + 1) == busqIndiceNChar && __B_INICIAL_NL__ < 2)	// Para la ultima letra la TH albergara a primLetraSufijoSobrante y primLetraSufijoCadAinsertar
				busq_B = 2;
			else
				busq_B = __B_INICIAL_NL__;
			CrearTHNL(busq_B);
			if(EsNPdoble(*busqActual)) {
				busqIndicePrincipioTH += 2;
				busqPrincipioTH += 2;
			}
			else {
				++busqIndicePrincipioTH; ++busqPrincipioTH;
			}
			busqIndicePadre = busqIndiceActual;
			busqPadre = busqActual;

			if(EsBNLpot2(*busqActual))
				ModPot2(bloqueNChar[indNLSchar], busq_B, indTemp);
			else
				ModNoPot2_256(bloqueNChar[indNLSchar], busq_B, indTemp);

			busqIndiceActual = busqIndicePrincipioTH + indTemp;
			GetNL(busqIndiceActual, busqActual);

			CrearNL(*busqActual, bloqueNChar[indNLSchar], busqIndicePadre);

#ifdef _TRAZA_DIVIDIRNLS_
			cout << "\t\tDividirNLS. NL de Prefijo comun creado en busqIndiceActual: " << busqIndiceActual << ": "; ImprimirNL(busqIndiceActual, cout); cout << endl;
#endif
		}

		if(bloqueNChar[busqIndiceNChar] != '\0') {	// Si el NLS no se consume totalmente
			if(__B_INICIAL_NL__ < 2)
				busq_B = 2;
			else
				busq_B = __B_INICIAL_NL__;
			CrearTHNL(busq_B);
			if(EsNPdoble(*busqActual)) {
				busqIndicePrincipioTH += 2;
				busqPrincipioTH += 2;
			}
			else {
				++busqIndicePrincipioTH; ++busqPrincipioTH;
			}

			busqIndicePadre = busqIndiceActual;
			busqPadre = busqActual;
		}
	}
	else {
		if(__B_INICIAL_NL__ < 2)
			busq_B = 2;
		else
			busq_B = __B_INICIAL_NL__;
		CrearTHNL(busq_B);
		if(EsNPdoble(*busqActual)) {
			busqIndicePrincipioTH += 2;
			busqPrincipioTH += 2;
		}
		else {
			++busqIndicePrincipioTH; ++busqPrincipioTH;
		}

		busqIndicePadre = busqIndiceActual;
		busqPadre = busqActual;

#ifdef _TRAZA_DIVIDIRNLS_
		cout << "\t\tDividirNLS. No hay prefijo comun" << endl;
#endif
	}

	vNodos.clear();

	bool consumeNLS = false;
	if(bloqueNChar[busqIndiceNChar] == '\0') {
#ifdef _TRAZA_DIVIDIRNLS_
		cout << "\t\tDividirNLS. Caso en que el NLS se consume totalmente. Ultimo NL del NLS seria busqIndiceActual: " << busqIndiceActual << ": "; ImprimirNL(busqIndiceActual, cout); cout << endl;
#endif
		consumeNLS = true;
		busqIndiceNLS = busqIndiceActual;
	}
	else {
		if(busqUltimaLetraEnPrefijo) {	// Solo se almacena la letra del prefijo que no coincide (primLetraSufijoSobrante)
			indAinsertar_busqUltimaLetraEnPrefijo = busqIndiceActual;
			if(EsBNLpot2(*busqActual))
				ModPot2(bloqueNChar[busqIndiceNChar], busq_B, indTemp);
			else
				ModNoPot2_256(bloqueNChar[busqIndiceNChar], busq_B, indTemp);

			busqIndiceActual = busqIndicePrincipioTH + indTemp;
			GetNL(busqIndiceActual, busqActual);
			busqIndiceNLS = busqIndiceActual;

			CrearNL(*busqActual, bloqueNChar[busqIndiceNChar], busqIndicePadre);
		}
		else {
			datoTemp.clave = bloqueNChar[busqIndiceNChar];	// Letra del prefijo que no coincide (primLetraSufijoSobrante)
			datoTemp.indiceNodoSinComprimir = raizTrie1;
			vNodos.push_back(datoTemp);
			datoTemp.clear();

			datoTemp.clave = busqIT;	// Letra de la cadena a insertar (primLetraSufijoCadAinsertar)
			datoTemp.indiceNodoSinComprimir = 0;
			vNodos.push_back(datoTemp);
			datoTemp.clear();

			RellenarTH_Colis(false, EsBNLpot2(*busqActual), vNodos, busq_B, busqIndicePrincipioTH, busqIndiceActual);
			GetNL(busqIndiceActual, busqActual);
		}

		++busqIndiceNChar;
	}

	// NL final del NLS original. Contendra la THNIT y BNIT del NLS original
	//		SetPrim_Inicializando(bloqueNL[busqIndiceNLS], indNLSnoMov);
	//		SetBNIT_Vacio(bloqueNL[busqIndiceNLS], bloqueNL[indNLSnoMov], BNITdelNLS);	// Para recuperar la THNIT y BNIT
	//		SetIndTHNIT_Vacio(bloqueNL[indNLSnoMov], indTHNITdelNLS);
	SetPrim_Inicializando(bloqueNL[busqIndiceNLS], indNPdelNLS);

	if(tieneTHNIT) {
		SetBNIT_Inicializando(bloqueNL[busqIndiceNLS], bloqueNL[indNPdelNLS], BNITdelNLS);	// Para recuperar la THNIT y BNIT
		if(esBNITpot2)
			SetBNITpot2(bloqueNL[busqIndiceNLS]);
	}

	// Actualizacion del indNLSnoMov
	//		GetPrim(bloqueNL[busqIndicePadre], indTemp);
	//		GetIndNPfijo(bloqueNL[indTemp], indTemp);
	SetIndNLPadre_Inicializando(bloqueNL[indNPdelNLS], busqIndicePadre);

#ifdef _TRAZA_DIVIDIRNLS_
	cout << "\t\tDividirNLS. La Letra del prefijo que no coincide (primLetraSufijoSobrante) almacenada en busqIndiceNLS: " << busqIndiceNLS << ": "; ImprimirNL(busqIndiceNLS, cout); cout << endl;
#endif

	if(seDivideIndiceRaiz_IT) {
		busqIndiceRaiz_IT = busqIndiceNLS;
		busqRaiz_IT = &(bloqueNL[busqIndiceNLS]);
#ifdef _TRAZA_DIVIDIRNLS_
		cout << "\t\tDividirNLS. Ya que seDivideIndiceRaiz_IT = true se actualiza busqIndiceRaiz_IT: " << busqIndiceRaiz_IT << " a busqIndiceNLS: " << busqIndiceNLS << endl;
#endif
	}

	if(bloqueNChar[busqIndiceNChar] != '\0') {
		//			SetNLS(bloqueNL[busqIndiceNLS]);
		//			SetNLS(bloqueNL[indNLSnoMov]);
		//			SetIndNLS_Inicializando(bloqueNL[indNLSnoMov + 2], busqIndiceNChar);
		SetNLS(bloqueNL[busqIndiceNLS]);
		SetIndNLS_Inicializando(bloqueNL[indNPdelNLS + 1], busqIndiceNChar);
#ifdef _TRAZA_DIVIDIRNLS_
		cout << "\t\tDividirNLS. Hay sufijoSobrante (busqIndiceNChar: " << busqIndiceNChar << "). La Letra del prefijo que no coincide (primLetraSufijoSobrante) almacenada en busqIndiceNLS: " << busqIndiceNLS << " se convierte a NLS : "; ImprimirNL(busqIndiceNLS, cout); cout << endl;
		//			cout << "\t\tDividirNLS. indNLSnoMov: " << indNLSnoMov << ": "; ImprimirNL(indNLSnoMov, cout); cout << endl;
#endif
	}
	else {
		//			UnSetNLS(bloqueNL[indNLSnoMov]);
		SetIndNLS_Inicializando(bloqueNL[indNPdelNLS + 1], 0);
#ifdef _TRAZA_DIVIDIRNLS_
		cout << "\t\tDividirNLS. NO Hay sufijoSobrante (busqIndiceNChar: " << busqIndiceNChar << "). La Letra del prefijo que no coincide (primLetraSufijoSobrante) almacenada en busqIndiceNLS: " << busqIndiceNLS << " NO sera NLS : "; ImprimirNL(busqIndiceNLS, cout); cout << endl;
		//			cout << "\t\tDividirNLS. indNLSnoMov: " << indNLSnoMov << ": "; ImprimirNL(indNLSnoMov, cout); cout << endl;
#endif
	}

	if(consumeNLS) {
		busqIndiceActual = busqIndiceNLS;
		GetNL(busqIndiceActual, busqActual);

		busq_B = __B_INICIAL_NL__;
		CrearTHNL(busq_B);
		if(EsNPdoble(*busqActual)) {
			busqIndicePrincipioTH += 2;
			busqPrincipioTH += 2;
		}
		else {
			++busqIndicePrincipioTH; ++busqPrincipioTH;
		}
		busqIndicePadre = busqIndiceActual;
		busqPadre = busqActual;

		if(EsBNLpot2(*busqActual))
			ModPot2(busqIT, busq_B, indTemp);
		else
			ModNoPot2_256(busqIT, busq_B, indTemp);

		busqIndiceActual = busqIndicePrincipioTH + indTemp;
		GetNL(busqIndiceActual, busqActual);

#ifdef _TRAZA_DIVIDIRNLS_
		cout << "\t\tDividirNLS. consumeNLS. El nuevo NL se almacenara en busqIndiceActual: " << busqIndiceActual << " busqIndiceNLS: "; ImprimirNL(busqIndiceNLS, cout); cout << endl;
#endif
	}

	if(busqUltimaLetraEnPrefijo)
		busqIndiceActual = indAinsertar_busqUltimaLetraEnPrefijo;

	GetNL(busqIndiceActual, busqActual);

#ifdef _TRAZA_DIVIDIRNLS_
	cout << "\t\tDividirNLS. FINAL. Indice donde almacenar la nueva clave a insertar(busqIndiceActual): " << busqIndiceActual << " busqIndicePadre: " << busqIndicePadre << " El NLS original se ha transformado a: "; ImprimirNL(indNLStraza, cout); cout << endl;
#endif
}

// Devuelve el ultimo nodo de la lista de colisiones
// RECIBE:
//	- busq_B: tamanyo de la nueva THNIT
//	- busqIndicePrincipioTH: indice principio de la nueva THNIT
//	- cTHNIT: si cTHNIT != 0 entonces es THNIT y se corresponde a c(claveAbuscar). Sino THNL2
//	- indice de partida de la lista de colisiones
// DEVUELVE:
//	- indice con el ultimo nodo de la lista de colisiones (su colision == 0)
void MergedTrie::BuscarUltimaClaveColisionada(_INDICE_NMTRIE& indice, const _INDICE_NMTRIE& busq_B, const _INDICE_NMTRIE& busqIndicePrincipioTH) {
	_INDICE_NMTRIE colisionTemp;
	while(true) {
		GetColisNL(bloqueNL[indice], colisionTemp);

		if(colisionTemp == 0)
			break;
		else {
			ObtenerSiguienteColision(busqIndicePrincipioTH, busqIndicePrincipioTH + busq_B - 1, indice, colisionTemp, indice);
		}
	}
}

// Calcula las posiciones de las claves dentro de una TH con colisiones dentro de la misma tabla
// RECIBE:
//	- vNodosT: claves de la THNIT
//			El vNodosT[i].indiceNodoSinComprimir == 0	Es la clave a insertar
//			El vNodosT[i].indiceNodoSinComprimir == raizTrie1	Es la clave que viene de dividirNLS que habria que crear el NL completo y no un NLMov
//	- busqIndicePadre de la TH a crear. Solo se utilizara cuando vNodos[i].indiceNodoSinComprimir == raizTrie1
//	- B: tamanyo de la nueva THNIT
//	- busqIndicePrincipioTHtemp: indice principio de la nueva THNIT
//	- esTHNIT: si es true entonces es THNIT. Sino THNL2
//	- bloqueNL_nuevo, porque cuando bloqueNL_nuevo != NULL, significa que esta funcion se esta llamando desde AgruparNodosAux, con lo que indiceNodoSinComprimir se ejecutara sobre bloqueNL/NIT_viejo
// DEVUELVE:
//	- vNodosT actualizando: posHash, indiceComprimido
//	- La TH rellena (bloqueNL/bloqueNIT segun corresponda)
//	- indNuevaClave donde se almacenara la nueva clave
//	- busqIndiceNLS con el indice de donde se haya almacenado vNodosT[i].indiceNodoSinComprimir == raizTrie1
void MergedTrie::RellenarTH_Colis(const bool& esTHNIT, const bool& esPot2, vector<datoNodo>& vNodosT, const _INDICE_NMTRIE& B, const _INDICE_NMTRIE& busqIndicePrincipioTHtemp, _INDICE_NMTRIE& indNuevaClave) {
	//#define _TRAZA_RELLENARTH_COLIS_
	_INDICE_NMTRIE tamV = vNodosT.size();
	_INDICE_NMTRIE i, colision, colisionTemp, posHashTemp, valor, indComprimidoTemp, c, indUltClaveColis, incremento, indTemp;

	for(i = 0; i < tamV; ++i) {		// Calculando posHash
		if(esTHNIT) {
			if(esPot2)
				ModPot2(vNodosT[i].clave, B, vNodosT[i].posHash);
			else
				ModNoPot2(vNodosT[i].clave, B, vNodosT[i].posHash);
		}
		else {
			if(esPot2)
				ModPot2(vNodosT[i].clave, B, vNodosT[i].posHash);
			else
				ModNoPot2_256(vNodosT[i].clave, B, vNodosT[i].posHash);
		}
	}

	std::sort(vNodosT.begin(), vNodosT.end());
	//		if(busqError) {
#ifdef _TRAZA_RELLENARTH_COLIS_
	cout << "\t\tRellenarTH_Colis. bloqueNL_nuevo: " << bloqueNL_nuevo << " esPot2: " << esPot2 << " vNodosT despues de posHash: "; ImprimirVNodos(vNodosT); cout << endl;
#endif
	//		}

	// Almacenando indiceComprimido de los primeros nodos de cada posHash (sin colisiones)
	valor = vNodosT[0].posHash;
	for(i = 0; i < tamV; ++i) {
		if(i == 0 || valor != vNodosT[i].posHash) {
			vNodosT[i].indiceComprimido = busqIndicePrincipioTHtemp + vNodosT[i].posHash;
			valor = vNodosT[i].posHash;

			if(vNodosT[i].indiceNodoSinComprimir == 0) {	// Es la clave a insertar
				indNuevaClave = vNodosT[i].indiceComprimido;
				if(esTHNIT) {
					SetIT(bloqueNIT[vNodosT[i].indiceComprimido], vNodosT[i].clave);
				}
				else {
					SetLetra(bloqueNL[vNodosT[i].indiceComprimido], vNodosT[i].clave);	// El indPrim, indPadre, ... se calculara una vez rellenada toda la THNL
				}
			}
			else if(!esTHNIT && vNodosT[i].indiceNodoSinComprimir == raizTrie1 && bloqueNL_nuevo == NULL) {	// Es la clave que viene de dividirNLS que habria que crear el NL completo y no un NLMov
				CrearNL(bloqueNL[vNodosT[i].indiceComprimido], vNodosT[i].clave, busqIndicePadre);
				busqIndiceNLS = vNodosT[i].indiceComprimido;
			}
			else {
				if(esTHNIT) {
					if(bloqueNL_nuevo != NULL) {
						SetIT(bloqueNIT[vNodosT[i].indiceComprimido], vNodosT[i].clave);
						if(GetBitNIT_NLS(bloqueNIT_viejo[vNodosT[i].indiceNodoSinComprimir])) {
							GetIT(bloqueNIT_viejo[vNodosT[i].indiceNodoSinComprimir], indTemp);
							GetIndNLS(bloqueNL_viejo[indTemp + 1], indTemp);
							if(indTemp != 0)
								SetBitNIT_NLS(bloqueNIT[vNodosT[i].indiceComprimido]);
						}

						if(dataMT)
							bloqueDataNIT[vNodosT[i].indiceComprimido] = bloqueDataNIT_viejo[vNodosT[i].indiceNodoSinComprimir];
					}
					else {
						SetIT(bloqueNIT[vNodosT[i].indiceComprimido], vNodosT[i].clave);
						if(GetBitNIT_NLS(bloqueNIT[vNodosT[i].indiceNodoSinComprimir]))
							SetBitNIT_NLS(bloqueNIT[vNodosT[i].indiceComprimido]);

						if(dataMT)
							bloqueDataNIT[vNodosT[i].indiceComprimido] = bloqueDataNIT[vNodosT[i].indiceNodoSinComprimir];
					}
				}
				else {
					if(bloqueNL_nuevo != NULL) {
						bloqueNL[vNodosT[i].indiceComprimido] = bloqueNL_viejo[vNodosT[i].indiceNodoSinComprimir];
						colaNL_compresion.push(datoCola(vNodosT[i].indiceNodoSinComprimir, 0, vNodosT[i].indiceComprimido, vNodosT[i].clave));
					}
					else
						bloqueNL[vNodosT[i].indiceComprimido] = bloqueNL[vNodosT[i].indiceNodoSinComprimir];
					SetColisNL_Acero(bloqueNL[vNodosT[i].indiceComprimido]);

					if(busqNumTrieFallo == 2 && vNodosT[i].indiceNodoSinComprimir == busqIndiceRaiz_IT) {
						busqIndiceRaiz_IT = vNodosT[i].indiceComprimido;
						busqRaiz_IT = &(bloqueNL[vNodosT[i].indiceComprimido]);
					}
				}
			}
		}
	}

#ifdef _TRAZA_RELLENARTH_COLIS_
	cout << "\t\tRellenarTH_Colis. TH rellenada con claves no colisionadas en busqIndicePrincipioTHtemp: " << busqIndicePrincipioTHtemp << endl;
	for(i = 0, indComprimidoTemp = busqIndicePrincipioTHtemp; i < B; ++i, ++indComprimidoTemp) {
		if(esTHNIT) {
			cout << "\t[NIT." << indComprimidoTemp << "](" << i << ")  =";
			if(bloqueNIT[indComprimidoTemp] == 0)
				cout << " VACIO";
			else {
				GetIT(bloqueNIT[indComprimidoTemp], colisionTemp);
				cout << " IT: " << colisionTemp;
				cout << " Colision: " << GetColisNIT(bloqueNIT[indComprimidoTemp]);
			}
		}
		else {
			cout << "\t[NL." << indComprimidoTemp << "](" << i << ")  =";
			if(bloqueNL[indComprimidoTemp] == 0)
				cout << " VACIO";
			else {
				GetLetra(bloqueNL[indComprimidoTemp], colisionTemp);
				cout << " Letra: " << (char) colisionTemp;
				GetColisNL(bloqueNL[indComprimidoTemp], colisionTemp);
				cout << " Colision: " << colisionTemp;
			}
		}
		cout << endl;
	}
#endif

	// Almacenando indiceComprimido de los demas nodos de cada posHash (colisionados)
	for(i = 0; i < tamV; ++i) {
		if(i == 0 || valor != vNodosT[i].posHash) {
			valor = vNodosT[i].posHash;
		}
		else {
			indUltClaveColis = busqIndicePrincipioTHtemp + vNodosT[i].posHash;
			if(esTHNIT) {
				if(esPot2)
					GetC(vNodosT[i].clave, B, c);
				else
					c = 1;
				BuscaHuecoTHNIT(indUltClaveColis, indUltClaveColis - busqIndicePrincipioTHtemp, indComprimidoTemp, c, B, esPot2, busqIndicePrincipioTHtemp, busqIndicePrincipioTHtemp + B - 1);
				//					SetColisNIT(bloqueNIT[indUltClaveColis]);
			}
			else {
				BuscarUltimaClaveColisionada(indUltClaveColis, B, busqIndicePrincipioTHtemp);
				BuscaHuecoTHNL(indUltClaveColis, indComprimidoTemp, incremento, B, busqIndicePrincipioTHtemp, busqIndicePrincipioTHtemp + B - 1);
				SetColisNL_Vacio(bloqueNL[indUltClaveColis], incremento);
			}
#ifdef _TRAZA_RELLENARTH_COLIS_
			cout << "\t\t\tRellenarTH_Colis. Se va a almacenar el nodo i: " << i << " CON colisiones EN indComprimidoTemp: " << indComprimidoTemp << " enlazandolo con indUltClaveColis: " << indUltClaveColis << endl;
#endif
			if(vNodosT[i].indiceNodoSinComprimir == 0) {	// Es la clave a insertar
				indNuevaClave = indComprimidoTemp;
				//					indNuevaClave = busqIndicePrincipioTHtemp + posHashTemp;
				if(esTHNIT) {
					SetIT(bloqueNIT[indComprimidoTemp], vNodosT[i].clave);
				}
				else {
					SetLetra(bloqueNL[indComprimidoTemp], vNodosT[i].clave);	// El indPrim, indPadre, ... se calculara una vez rellenada toda la THNL
				}
			}
			else if(!esTHNIT && vNodosT[i].indiceNodoSinComprimir == raizTrie1 && bloqueNL_nuevo == NULL) {	// Es la clave que viene de dividirNLS que habria que crear el NL completo y no un NLMov
				CrearNL(bloqueNL[indComprimidoTemp], vNodosT[i].clave, busqIndicePadre);
				busqIndiceNLS = indComprimidoTemp;
			}
			else {
				if(esTHNIT) {
					if(bloqueNL_nuevo != NULL) {
						SetIT(bloqueNIT[indComprimidoTemp], vNodosT[i].clave);
						if(GetBitNIT_NLS(bloqueNIT_viejo[vNodosT[i].indiceNodoSinComprimir])) {
							GetIT(bloqueNIT_viejo[vNodosT[i].indiceNodoSinComprimir], indTemp);
							GetIndNLS(bloqueNL_viejo[indTemp + 1], indTemp);
							if(indTemp != 0)
								SetBitNIT_NLS(bloqueNIT[indComprimidoTemp]);
						}

						if(dataMT)
							bloqueDataNIT[vNodosT[i].indiceComprimido] = bloqueDataNIT_viejo[vNodosT[i].indiceNodoSinComprimir];
					}
					else {
						SetIT(bloqueNIT[indComprimidoTemp], vNodosT[i].clave);
						if(GetBitNIT_NLS(bloqueNIT[vNodosT[i].indiceNodoSinComprimir]))
							SetBitNIT_NLS(bloqueNIT[indComprimidoTemp]);

						if(dataMT)
							bloqueDataNIT[vNodosT[i].indiceComprimido] = bloqueDataNIT[vNodosT[i].indiceNodoSinComprimir];
					}
				}
				else {
					if(bloqueNL_nuevo != NULL) {
						bloqueNL[indComprimidoTemp] = bloqueNL_viejo[vNodosT[i].indiceNodoSinComprimir];
						colaNL_compresion.push(datoCola(vNodosT[i].indiceNodoSinComprimir, 0, indComprimidoTemp, vNodosT[i].clave));
					}
					else
						bloqueNL[indComprimidoTemp] = bloqueNL[vNodosT[i].indiceNodoSinComprimir];

					SetColisNL_Acero(bloqueNL[indComprimidoTemp]);

					if(busqNumTrieFallo == 2 && vNodosT[i].indiceNodoSinComprimir == busqIndiceRaiz_IT) {
						busqIndiceRaiz_IT = vNodosT[i].indiceComprimido;
						busqRaiz_IT = &(bloqueNL[vNodosT[i].indiceComprimido]);
					}
				}
			}
			vNodosT[i].indiceComprimido = indComprimidoTemp;
		}
	}

	//		if(busqError) {
#ifdef _TRAZA_RELLENARTH_COLIS_
	cout << "\t\tRellenarTH_Colis. vNodosT despues de posHash y calcular indiceComprimido: "; ImprimirVNodos(vNodosT); cout << endl;

	if(bloqueNL_nuevo != NULL)
		cout << "\t\tRellenarTH_Colis. FIN. TH rellenada en busqIndicePrincipioTHtemp: " << busqIndicePrincipioTHtemp << endl;
	else
		cout << "\t\tRellenarTH_Colis. FIN. indNuevaClave: " << indNuevaClave << " TH rellenada en busqIndicePrincipioTHtemp: " << busqIndicePrincipioTHtemp << endl;
	for(i = 0, indComprimidoTemp = busqIndicePrincipioTHtemp; i < B; ++i, ++indComprimidoTemp) {
		if(esTHNIT) {
			cout << "\t[NIT." << indComprimidoTemp << "](" << i << ")  =";
			if(bloqueNIT[indComprimidoTemp] == 0)
				cout << " VACIO";
			else {
				GetIT(bloqueNIT[indComprimidoTemp], colisionTemp);
				cout << " IT: " << colisionTemp;
				cout << " Colision: " << GetColisNIT(bloqueNIT[indComprimidoTemp]);
			}
		}
		else {
			cout << "\t[NL." << indComprimidoTemp << "](" << i << ")  =";
			if(bloqueNL[indComprimidoTemp] == 0)
				cout << " VACIO";
			else {
				GetLetra(bloqueNL[indComprimidoTemp], colisionTemp);
				cout << " Letra: " << (char) colisionTemp;
				GetColisNL(bloqueNL[indComprimidoTemp], colisionTemp);
				cout << " Colision: " << colisionTemp;
			}
		}
		cout << endl;
	}
#endif
	//		}
}

// RECIBE:
//	* indInicial posicion de partida en la TH
//	* busq_B tamanyo de la TH
//	* busqIndicePrincipioTH principio de la TH
//	* busqIndiceFinTH fin de la TH
//	* c: incremento en la TH para buscar el siguiente hueco
// DEVUELVE:
//	* true si hay hueco libre en la TH EN LAS 15 POSICIONES A CONTINUACION DE busqIndiceActual (ya que porque colisNIT tiene 4 bits)
//	  false en caso contrario
//	* indHueco indice del hueco libre
bool MergedTrie::BuscaHuecoTHNIT(_INDICE_NMTRIE indInicial, _INDICE_NMTRIE posHash, _INDICE_NMTRIE& indHueco, const _INDICE_NMTRIE& c, const _INDICE_NMTRIE& busq_B, const bool& esPot2, const _INDICE_NMTRIE& busqIndicePrincipioTH, const _INDICE_NMTRIE& busqIndiceFinTH) {
	//#define _TRAZA_BUSCAHUECOTHNIT_
	_INDICE_NMTRIE incremento;
#ifdef _TRAZA_BUSCAHUECOTHNIT_
	cout << "\tBuscaHuecoTHNIT. INICIO. indInicial: " << indInicial << " busqIndicePrincipioTH: " << busqIndicePrincipioTH << " busqIndiceFinTH: " << busqIndiceFinTH << " c: " << c << endl;
#endif
	for(incremento = 1; incremento < busq_B; ++incremento) {
		SetColisNIT(bloqueNIT[indInicial]);
		if(esPot2) {
			posHash += c;
			ModPot2(posHash, busq_B, indHueco);
			indHueco += busqIndicePrincipioTH;
		}
		else
			//	    		ModNoPot2(posHash, busq_B, indHueco);
			ObtenerSiguienteColision(busqIndicePrincipioTH, busqIndiceFinTH, indInicial, c, indHueco);

		if(bloqueNIT[indHueco] == 0) {
#ifdef _TRAZA_BUSCAHUECOTHNIT_
			cout << "\tBuscaHuecoTHNIT. Encuentra hueco libre en indHueco: " << indHueco << endl;
#endif
			return true;
		}
#ifdef _TRAZA_BUSCAHUECOTHNIT_
		else
			cout << "\tBuscaHuecoTHNIT. NO Encuentra hueco libre en indHueco: " << indHueco << endl;
#endif
		indInicial = indHueco;
	}

	return false;
}

// RECIBE:
//	* indInicial posicion de partida en la TH
//	* busq_B tamanyo de la TH
//	* busqIndicePrincipioTH principio de la TH
//	* busqIndiceFinTH fin de la TH
// DEVUELVE:
//	* true si hay hueco libre en la TH EN LAS 255 POSICIONES A CONTINUACION DE busqIndiceActual (ya que porque colisNL tiene 8 bits)
//	  false en caso contrario
//	* indHueco indice del hueco libre
//	* incremento que seria lo que habria que almacenar en el campo colis
bool MergedTrie::BuscaHuecoTHNL(const _INDICE_NMTRIE& indInicial, _INDICE_NMTRIE& indHueco, _INDICE_NMTRIE& incremento, const _INDICE_NMTRIE& busq_B, const _INDICE_NMTRIE& busqIndicePrincipioTH, const _INDICE_NMTRIE& busqIndiceFinTH) {
	//#define _TRAZA_BUSCAHUECOTHNL_
#ifdef _TRAZA_BUSCAHUECOTHNL_
	cout << "\tBuscaHuecoTHNL. Inicio. indInicial: " << indInicial << " busq_B: " << busq_B << " busqIndicePrincipioTH: " << busqIndicePrincipioTH << " busqIndiceFinTH: " << busqIndiceFinTH << endl;
#endif

	//		for(incremento = 1, indHueco = busqIndiceActual + 1; incremento < 256 && incremento < busq_B; ++incremento, ++indHueco) {	// incremento < 256 porque colisNL tiene 8 bits
	for(incremento = 1, indHueco = indInicial + 1; incremento < busq_B; ++incremento, ++indHueco) {	// incremento < 256 porque colisNL tiene 8 bits
		if(indHueco > busqIndiceFinTH)
			indHueco = busqIndicePrincipioTH;

		if(bloqueNL[indHueco] == 0) {
#ifdef _TRAZA_BUSCAHUECOTHNL_
			cout << "\tBuscaHuecoTHNL. Encuentra hueco libre en indHueco: " << indHueco << " incremento: " << incremento << endl;
#endif
			return true;
		}
#ifdef _TRAZA_BUSCAHUECOTHNL_
		else
			cout << "\tBuscaHuecoTHNL. NO Encuentra hueco libre en indHueco: " << indHueco << endl;
#endif
	}

	return false;
}

// Devuelve vNodos con los contenidos en la THNIT de inicio
// RECIBE:
//		* vNodos que contendra la clave a insertar
//		* B con el tamanyo de la THNIT que empieza en indPrincTH
// DEVUELVE:
//		* vNodos leidos
void MergedTrie::CrearvNodosNIT(const _INDICE_NMTRIE& indPrincTH, const _INDICE_NMTRIE& B) {
	//#define _TRAZA_CREARVNODOSNIT_
	_INDICE_NMTRIE indTemp = indPrincTH, indTemp2, i;
#ifdef _TRAZA_CREARVNODOSNIT_
	cout << "\t\tCrearvNodosNIT. ENTRADA. busq_B: " << busq_B << " indPrincTH: " << indPrincTH << " vNodos (con solo la clave a insertar): "; ImprimirVNodos(vNodos); cout << endl;
#endif
	for(i = 0; i < B; ++i, ++indTemp) {
		if(bloqueNIT[indTemp] != 0) {
			GetIT(bloqueNIT[indTemp], indTemp2);
			if(indTemp2 != 0) {
				datoTemp.clave = indTemp2;
				datoTemp.indiceNodoSinComprimir = indTemp;
				vNodos.push_back(datoTemp);
				datoTemp.clear();
			}
			else {
				cerr << "ERROR_CrearvNodosNIT con IT = 0" << endl;
				exit(EXIT_MTRIE);
			}
		}
	}

#ifdef _TRAZA_CREARVNODOSNIT_
	cout << "\t\tCrearvNodosNIT. FINAL. vNodos (anyadiendo todos los nodos de la TH): "; ImprimirVNodos(vNodos); cout << endl;
#endif
}

// Devuelve vNodos con los contenidos en la THNL de inicio
// RECIBE:
//		* vNodos que contendra la clave a insertar
//		* NL nodoPadre cuya THNL de descendientes se va a almacenar en vNodos
// DEVUELVE:
//		* vNodos leidos
void MergedTrie::CrearvNodosNL(const _NMTRIE_& NL) {
	//#define _TRAZA_CREARVNODOSNL_

#ifdef _TRAZA_CREARVNODOSNL_
	cout << "\t\tCrearvNodosNL. ENTRADA. vNodos (con SOLO la clave a insertar): "; ImprimirVNodos(vNodos); cout << endl;
#endif
	_NMTRIE_ *pnodo;
	_INDICE_NMTRIE indPrincTH, indTemp, indTemp2, indTemp3, B, B2;
	int i, j;

#ifdef __MODO_SEGURO__
	if(!EsNL(NL)) {
		cerr << "ERROR_ImprimirTHNL de un nodo que no es NL" << endl;
		exit(EXIT_MTRIE);
	}
#endif
	if(EsNLS(NL)) {
		cerr << "ERROR_ImprimirTHNL de un NL que es NLS" << endl;
		exit(EXIT_MTRIE);
	}

	GetBNL(NL, B);

	if(B > 0) {
		GetPrim(NL, indTemp);
		if(EsNPdoble(NL))
			indTemp += 2;	// Saltamos el NP1/2
		else
			++indTemp;	// Saltamos el NP1
		GetNL(indTemp, pnodo);

		for(i = 0; i < B; ++i, ++indTemp) {
			GetNL(indTemp, pnodo);
			if(*pnodo != 0) {
				GetLetra(*pnodo, indTemp2);

				datoTemp.clave = indTemp2;
				datoTemp.indiceNodoSinComprimir = indTemp;
				vNodos.push_back(datoTemp);
				datoTemp.clear();
#ifdef _TRAZA_CREARVNODOSNL_
				cout << "\t\tCrearvNodosNL. Anyade letra: " << (char) indTemp2 << " de indice: " << indTemp << endl;
#endif
			}
		}
	}

#ifdef _TRAZA_CREARVNODOSNL_
	cout << "\t\tCrearvNodosNL. FINAL. vNodos (anyadiendo todos los nodos de la TH): "; ImprimirVNodos(vNodos); cout << endl;
#endif
}

bool MergedTrie::CrearNuevaRamaCondicion() {
	if(busqNumTrieFallo == 1)
		return busq_i < busqTrie2Inicio;
	else
		//    	else if(busqNumTrieFallo == 2)
		return busq_i >= busqTrie2Inicio;
}

// Devolvera true cuando busq_i sea la ultima letra a tratar
bool MergedTrie::CrearNuevaRamaCondicionUltimaLetra() {
	CrearNuevaRamaIncremento();
	if(CrearNuevaRamaCondicion()) {
		CrearNuevaRamaDecremento();
		return true;
	}
	else {
		CrearNuevaRamaDecremento();
		return false;
	}
}

void MergedTrie::CrearNuevaRamaIncremento() {
	if(busqNumTrieFallo == 1)
		++busq_i;
	else
		//    	else if(busqNumTrieFallo == 2)
		--busq_i;
}

void MergedTrie::CrearNuevaRamaDecremento() {
	if(busqNumTrieFallo == 1)
		--busq_i;
	else
		//    	else if(busqNumTrieFallo == 2)
		++busq_i;
}

// Crea nueva rama en el trie de NLETRA y tabla Hash de IT a partir de la salida de find (si se cambia algo, cambiarlo tambien en find)
// RECIBE:
//		- busqNumTrieFallo = 1 si falla en busq. trie1, 2 si en trie2, 3 si en idTrie1 de nodoIT
//		- busq_i = numero de caracter que falla ya sea de s o de cadena de nodoIT
//		- busqIndiceTrie2 que se esta buscando con busqNumTrieFallo == 3
//		- busqLetraTemp que se esta buscando con busqNumTrieFallo != 3
//
//		- (busq_indB, busq_B) = B de la TH donde se ha producido el fallo
//		- (busqIndicePadre, busqPadre) indice que apunta al padre de busqIndiceActual, es decir, caracter (busq_i - 1) que si se encuentra (busq_i falla)
//		- (busqIndiceActual, busqActual) = indice que apunta al ultimo nodo tratado, desde el que habria que continuar la insercion
//		- (busqIndiceRaiz_IT, busqRaiz_IT) = apunta al nodo de trie1 en cuya THNIT se tiene que insertar busqIndiceTrie2
//
//		- Si busqIndiceActual == busqIndicePadre entonces hay que crear la tabla de NLETRA/NIT (prim de busqIndicePadre == 0)
//		  Sino. Si busqIndiceActual es nodo vacio, se corresponderia a la posicion donde insertar el nodo
//			    Si no es nodo vacio: hay que ampliar y mover la tabla
void MergedTrie::CrearNuevaRama(const string& s) {
	//#define _TRAZA_CREARNUEVARAMA_
	_INDICE_NMTRIE indTemp, indTemp2, i, j, B;
	_NMTRIE_ *pTemp, *pTemp2;
	busqUltimaLetraEnPrefijo = false;

	if(busqNumTrieFallo == 3)
		busqIT = busqIndiceTrie2;	// Clave a buscar/insertar
	else
		busqIT = busqLetraTemp;		// Clave a buscar/insertar

#ifdef _TRAZA_CREARNUEVARAMA_
	cout << "\tCrearNuevaRama. s: " << s << ". busqNumTrieFallo: " << busqNumTrieFallo;
	if(busqNumTrieFallo != 3) {
		cout << " desde busq_i: " << busq_i << " busqLetraTemp: " << (char) busqLetraTemp << " hasta busqTrie2Inicio: " << busqTrie2Inicio << " ";
	}
	else {
		cout << " de busqIndiceTrie2: " << busqIndiceTrie2;
	}
	if(busqNumTrieFallo != 1) {
		cout << endl << "\tCrearNuevaRama. busqIndiceRaiz_IT: " << busqIndiceRaiz_IT << " busqRaiz_IT: "; ImprimirNL(busqIndiceRaiz_IT, cout);
	}
	if(busqNumTrieFallo != 3) {
		cout << endl << "\tCrearNuevaRama. busqIndicePadre: " << busqIndicePadre << " busqPadre: "; ImprimirNL(busqIndicePadre, cout);
	}
	cout << endl << "\tCrearNuevaRama. busqIndiceActual: " << busqIndiceActual << " busq_B: " << busq_B << endl;
#endif

	//    	if(busqIndiceActual == 357) {
	//    		TrazaError();
	//    	}

#ifdef __MODO_SEGURO__
	if(busqNumTrieFallo == 3 && (busqIndiceTrie2 == 0 || busqIndiceRaiz_IT == 0)) {
		cerr << "ERROR_CrearNuevaRama. busqIndiceTrie2 = " << busqIndiceTrie2 << " == 0 || busqIndiceRaiz_IT = " << busqIndiceRaiz_IT << " == 0" << endl;
		exit(EXIT_MTRIE);
	}
	if(busqNumTrieFallo != 3 && (!EsNL(*busqActual) || !EsNL(*busqPadre))) {
		cerr << "ERROR_CrearNuevaRama. busqIndiceActual o busqIndicePadre No es nodo Letra" << endl;
		exit(EXIT_MTRIE);
	}
	if(busqNumTrieFallo != 3 && busqActual != &(bloqueNL[busqIndiceActual])) {
		cerr << "ERROR_CrearNuevaRama. busqActual != busqIndiceActual" << endl;
		exit(EXIT_MTRIE);
	}
	if(busqNumTrieFallo != 3 && busqPadre != &(bloqueNL[busqIndicePadre])) {
		cerr << "ERROR_CrearNuevaRama. busqPadre != busqIndicePadre" << endl;
		exit(EXIT_MTRIE);
	}
	if(busqNumTrieFallo == 3 && busqRaiz_IT != &(bloqueNL[busqIndiceRaiz_IT])) {
		cerr << "ERROR_CrearNuevaRama. busqPadre != busqIndicePadre" << endl;
		exit(EXIT_MTRIE);
	}
#endif
	// INICIO prim de busqIndicePadre == 0. Hay que crear TH de descendientes
	if(busqIndiceActual == busqIndicePadre) {
#ifdef _TRAZA_CREARNUEVARAMA_
		cout << "\t\tCrearNuevaRama. busqIndiceActual == busqIndicePadre: " << busqIndiceActual << ". Crear TH" << endl;
#endif
		if(busqNumTrieFallo == 3) {
#ifdef __MODO_SEGURO__
			if(busqRaiz_IT != &(bloqueNL[busqIndiceRaiz_IT])) {
				cerr << "ERROR_CrearNuevaRama. busqPadre != busqIndicePadre" << endl;
				exit(EXIT_MTRIE);
			}
#endif
			busq_B = __B_INICIAL_NIT__;
			busqIndiceActual = busqIndiceRaiz_IT;
			busqActual = busqRaiz_IT;
			CrearTHNIT();

			if(EsBNITpot2(*busqActual))
				ModPot2(busqIT, busq_B, busqPosHash);
			else
				ModNoPot2(busqIT, busq_B, busqPosHash);
		}
		else {
			if(busqIndicePadre == raizTrie1)
				busq_B = __B_INICIAL_NL_RAIZ__;
			else
				busq_B = __B_INICIAL_NL__;

			busqIndiceActual = busqIndicePadre;
			GetNL(busqIndiceActual, busqActual);
			GetNL(busqIndicePadre, busqPadre);

			CrearTHNL(busq_B);
			if(EsNPdoble(*busqActual)) {
				busqIndicePrincipioTH += 2;
				busqPrincipioTH += 2;
			}
			else {
				++busqIndicePrincipioTH; ++busqPrincipioTH;
			}

			if(EsBNLpot2(*busqActual))
				ModPot2(busqIT, busq_B, busqPosHash);
			else
				ModNoPot2_256(busqIT, busq_B, busqPosHash);
		}

#ifdef _TRAZA_CREARNUEVARAMA_
		cout << "\t\tCrearNuevaRama. Se ha creado la tabla en busqIndicePrincipioTH: " << busqIndicePrincipioTH << " busqIndiceActual: "; ImprimirNL(busqIndiceActual, cout); cout << endl;
#endif
		busqIndiceActual = busqIndicePrincipioTH + busqPosHash;
		if(busqNumTrieFallo != 3) {
			GetNL(busqIndiceActual, busqActual);
		}

		if(busqNumTrieFallo == 3)
			SetIT(bloqueNIT[busqIndiceActual], busqIT);
		else
			SetLetra(bloqueNL[busqIndiceActual], busqIT);
#ifdef _TRAZA_CREARNUEVARAMA_
		cout << "\t\tCrearNuevaRama. Se ha creado la tabla en busqIndicePrincipioTH: " << busqIndicePrincipioTH << " posicion donde insertar busqIndiceActual: " << busqIndiceActual << endl;
#endif
	}	// FIN prim de busqIndicePadre == 0. Hay que crear tabla hash de descendientes y pasar de NodoLetraHoja a NodoLetraNoHoja
	else if(busqNumTrieFallo != 3 && busqFalloNLS) {	// INICIO busqFalloNLS
#ifdef _TRAZA_CREARNUEVARAMA_
		cout << "\t\tCrearNuevaRama. busqFalloNLS. DividirNLS" << endl;
#endif
		DividirNLS();

		if(busqUltimaLetraEnPrefijo) {
#ifdef _TRAZA_CREARNUEVARAMA_
			cout << "\t\tCrearNuevaRama. Despues de DividirNLS. Se ha dado el caso busqUltimaLetraEnPrefijo, por lo que finaliza CrearNuevaRama" << endl;
#endif
			if(busqNumTrieFallo == 1) {
				busqRaiz_IT = busqActual;
				busqIndiceRaiz_IT = busqIndiceActual;
			}
			else if(busqNumTrieFallo == 2){
				busqIndiceTrie2 = busqIndiceActual;
			}
			return;
		}
	}	// FIN busqFalloNLS
	else if((busqNumTrieFallo != 3 && *busqActual == 0) || (busqNumTrieFallo == 3 && bloqueNIT[busqIndiceActual] == 0)) {
		if(busqNumTrieFallo == 3)
			SetIT(bloqueNIT[busqIndiceActual], busqIT);
		else
			SetLetra(bloqueNL[busqIndiceActual], busqIT);

	}
	else {	// INICIO *busqActual != 0. La tabla ya estaba creada. Comprobar si hay hueco para la nueva clave, sino AMPLIA
		if(busqNumTrieFallo == 3) {
#ifdef _TRAZA_CREARNUEVARAMA_
			cout << "\t\tCrearNuevaRama. THNIT. *busqActual != 0. La tabla ya estaba creada. Comprobar si hay hueco para la nueva clave, sino AMPLIA" << endl;
#endif
			busqIndiceFinTH = busqIndicePrincipioTH + busq_B - 1;

			_INDICE_NMTRIE c;
			if(busqEsBNITpot2)
				GetC(busqIT, busq_B, c);
			else
				c = 1;
			if(BuscaHuecoTHNIT(busqIndiceActual, busqIndiceActual - busqIndicePrincipioTH, busqIndiceTemp, c, busq_B, busqEsBNITpot2, busqIndicePrincipioTH, busqIndiceFinTH)) {
				SetColisNIT(bloqueNIT[busqIndiceActual]);
				busqIndiceActual = busqIndiceTemp;

				if(busqNumTrieFallo == 3)
					SetIT(bloqueNIT[busqIndiceActual], busqIT);
				else
					SetLetra(bloqueNL[busqIndiceActual], busqIT);
#ifdef _TRAZA_CREARNUEVARAMA_
				cout << "\t\tCrearNuevaRama. THNIT. *busqActual != 0. Hay hueco para la nueva clave en busqIndiceActual: " << busqIndiceActual << endl;
#endif
			}
			else {
				vNodos.clear();
				datoTemp.clave = busqIT;		// Clave a anyadir. Tendra su datoTemp.indiceNodoSinComprimir = 0
				vNodos.push_back(datoTemp);
				datoTemp.clear();

				CrearvNodosNIT(busqIndicePrincipioTH, busq_B);
#ifdef __MODO_SEGURO__
				_INDICE_NMTRIE claveInsertada;
				if(vNodos.size() < busq_B) {
					cerr << "ERROR_CrearNuevaRama. THNIT. *busqActual != 0. NO Hay hueco para la nueva clave pero vNodos.size(): " <<  vNodos.size() << " < busq_B: " << busq_B << endl;
					exit(EXIT_MTRIE);
				}
#endif

				//    				busq_B = vNodos.size() + __INC_BNIT__;
				//    				if(busq_B < 8000)
				busq_B = busq_B * 2;
				//    				else
				//    					busq_B = vNodos.size() + __INC_BNIT__;
				busqIndiceActual = busqIndiceRaiz_IT;
				busqActual = busqRaiz_IT;
				CrearTHNIT();
#ifdef _TRAZA_CREARNUEVARAMA_
				cout << "\t\tCrearNuevaRama. *busqActual != 0 pero no hay hueco: Ampliar THNIT de busqIndiceRaiz_IT: " << busqIndiceRaiz_IT << " a busq_B: " << busq_B << endl;
#endif
				RellenarTH_Colis(true, EsBNITpot2(*busqActual), vNodos, busq_B, busqIndicePrincipioTH, busqIndiceActual);
#ifdef _TRAZA_CREARNUEVARAMA_
				cout << "\t\tCrearNuevaRama. *busqActual != 0 pero no hay hueco: THNIT creada: "; ImprimirNL(busqIndiceRaiz_IT, cout); cout << endl;
#endif
			}
		}
		else {	// INICIO busqNumTrieFallo != 3. *busqActual != 0.
#ifdef _TRAZA_CREARNUEVARAMA_
			cout << "\t\tCrearNuevaRama. THNL. *busqActual != 0. La tabla ya estaba creada. Comprobar si hay hueco para la nueva clave, sino AMPLIA" << endl;
#endif
			busqIndiceFinTH = busqIndicePrincipioTH + busq_B - 1;
			if(BuscaHuecoTHNL(busqIndiceActual, busqIndiceTemp, i, busq_B, busqIndicePrincipioTH, busqIndiceFinTH)) {
				SetColisNL_Vacio(*busqActual, i);
				busqIndiceActual = busqIndiceTemp;
				GetNL(busqIndiceActual, busqActual);
#ifdef _TRAZA_CREARNUEVARAMA_
				cout << "\t\tCrearNuevaRama. THNL. *busqActual != 0. Hay hueco para la nueva clave en busqIndiceActual: " << busqIndiceActual << " colision: " << i << endl;
#endif
			}
			else {	// INICIO Ampliar la THNL
				vNodos.clear();
				datoTemp.clave = busqIT;		// Clave a anyadir. Tendra su datoTemp.indiceNodoSinComprimir = 0
				vNodos.push_back(datoTemp);
				datoTemp.clear();

				CrearvNodosNL(*busqPadre);

				GetBNL(*busqPadre, busq_B);
				if(busq_B < 256)
					busq_B *= 2;

				busqIndiceActual = busqIndicePadre;
				busqActual = busqPadre;

				CrearTHNL(busq_B);
				if(EsNPdoble(*busqActual)) {
					busqIndicePrincipioTH += 2;
					busqPrincipioTH += 2;
				}
				else {
					++busqIndicePrincipioTH; ++busqPrincipioTH;
				}
				busqIndicePadre = busqIndiceActual;
				busqPadre = busqActual;
#ifdef _TRAZA_CREARNUEVARAMA_
				cout << "\t\tCrearNuevaRama. *busqActual != 0 pero no hay hueco: Ampliar tabla a busq_B: " << busq_B << endl;
#endif
				RellenarTH_Colis(false, EsBNLpot2(*busqActual), vNodos, busq_B, busqIndicePrincipioTH, busqIndiceActual);
				GetNL(busqIndiceActual, busqActual);

				for(i = 0; i < vNodos.size(); ++i) {
					if(vNodos[i].indiceNodoSinComprimir != 0) {
						GetBNL(bloqueNL[vNodos[i].indiceComprimido], B);
						if(B > 0) {
#ifdef _TRAZA_CREARNUEVARAMA_
							cout << "\t\tCrearNuevaRama. *busqActual != 0 pero no hay hueco. Ya se ha ampliado tabla. Actualizando NLpadre de vNodos[" << i << "] a indNL: " << vNodos[i].indiceComprimido << " Inicialmente: "; ImprimirNL(vNodos[i].indiceComprimido, cout); cout << endl;
#endif
							GetPrim(bloqueNL[vNodos[i].indiceComprimido], indTemp);
							++indTemp;	// Saltamos el NP
							if(EsNPdoble(bloqueNL[vNodos[i].indiceComprimido]))
								++indTemp;	// Saltamos el NP

							for(j = 0; j < B; ++j, ++indTemp) {
								if(bloqueNL[indTemp] != 0) {
									GetPrim(bloqueNL[indTemp], indTemp2);
									SetIndNLPadre_Inicializando(bloqueNL[indTemp2], vNodos[i].indiceComprimido);
								}
							}
#ifdef _TRAZA_CREARNUEVARAMA_
							cout << "\t\tCrearNuevaRama. *busqActual != 0 pero no hay hueco. Ya se ha ampliado tabla. ACTUALIZADO NLpadre de vNodos[" << i << "] a indNL: " << vNodos[i].indiceComprimido << " ACTUALIZADO: "; ImprimirNL(vNodos[i].indiceComprimido, cout); cout << endl;
#endif
						}
					}
				}
			}	// FIN Ampliar la THNL
		}	// FIN busqNumTrieFallo != 3. *busqActual != 0.

#ifdef _TRAZA_CREARNUEVARAMA_
		cout << "\t\tCrearNuevaRama. *busqActual != 0. nueva TH: " << endl;
		if(busqNumTrieFallo == 3)
			ImprimirTHNIT(busqIndicePrincipioTH, busq_B, cout);
		else
			ImprimirNL(busqIndicePadre, cout);
		//    			ImprimirTHNL(bloqueNL[busqIndicePadre], cout);
#endif
	}	// FIN *busqActual != 0. La tabla ya estaba creada. Comprobar si hay hueco para la nueva clave, sino AMPLIA

	//    	if(busqIndiceActual == 357) {
	//    		TrazaError();
	//    	}

#ifdef __MODO_SEGURO__
	_INDICE_NMTRIE claveInsertada;
	if(busqNumTrieFallo == 3)
		GetIT(bloqueNIT[busqIndiceActual], claveInsertada);
	else
		GetLetra(bloqueNL[busqIndiceActual], claveInsertada);

	if(claveInsertada != busqIT && claveInsertada != 0) {
		cerr << "ERROR_CrearNuevaRama. busqActual no almacena busqIT: " << busqIT << " sino: " << claveInsertada << ". busqIndiceActual: " << busqIndiceActual << endl;
		exit(EXIT_MTRIE);
	}
#endif

	if(busqNumTrieFallo == 3) {
		SetIT(bloqueNIT[busqIndiceActual], busqIT);
		if(busqITesNLS)
			SetBitNIT_NLS(bloqueNIT[busqIndiceActual]);

		if(dataMT) {
			bloqueDataNIT[busqIndiceActual] = cant;
		}

		//    		if(busqError) {
		//    			cout << "\tCrearNuevaRama. NIT. "; ImprimirNL(busqIndiceRaiz_IT, cout); cout << endl;
#ifdef _TRAZA_CREARNUEVARAMA_
		cout << "\t\tCrearNuevaRama. Despues de encontrar nodo vacio y actualizar nodoIT. busqIndiceActual: " << busqIndiceActual << " busqActual: "; ImprimirNIT(bloqueNIT[busqIndiceActual], cout); cout << endl;
#endif
		//    		}
	}
	else {
		// Se creara la rama en busqActual que debe apuntar a un nodo vacio desde s[busq_i] en busqIndiceActual
		if(busqNumTrieFallo == 1)
			indTemp = busqTrie2Inicio - busq_i;
		else
			indTemp = busq_i + 1 - busqTrie2Inicio;

#ifdef _TRAZA_CREARNUEVARAMA_
		cout << "\t\tCrearNuevaRama. Posicion donde insertar busqLetraTemp: " << (char) busqLetraTemp << " en busqIndiceActual: " << busqIndiceActual << " busqIndicePadre: " << busqIndicePadre << " Num. letras pendientes de insertar: " << indTemp << " busqNumTrieFallo: " << busqNumTrieFallo << " busq_B: " << busq_B << endl;
#endif

		CrearNuevaRamaIncremento();
		--indTemp;
#ifdef _TRAZA_CREARNUEVARAMA_
		cout << "\t\tCrearNuevaRama. Despues de crear el NL dentro de la TH de NL. Num. letras pendientes de insertar: " << indTemp << endl;
#endif

#ifdef __MODO_SEGURO__
		if(busqActual != &(bloqueNL[busqIndiceActual])) {
			cerr << "ERROR_CrearNuevaRama. busqActual != &(bloqueNL[busqIndiceActual])" << endl;
			exit(EXIT_MTRIE);
		}
#endif
		CrearNL(*busqActual, busqIT, busqIndicePadre);
		if(indTemp > 0) {
			SetNLS(*busqActual);

			GetNuevoNL(pTemp2, indTemp2);	// Creando el NP2
#ifdef __MODO_SEGURO__
			SetNP(*pTemp2);
#endif
			GetNuevoNChar(busqNChar, busqIndiceNChar, indTemp + 1);
			SetIndNLS_Vacio(*pTemp2, busqIndiceNChar);

			for(; CrearNuevaRamaCondicion(); CrearNuevaRamaIncremento(), ++busqNChar) {
				(*busqNChar) = (unsigned char) s[busq_i];
#ifdef _TRAZA_CREARNUEVARAMA_
				cout << "\t\tCrearNuevaRama. Creando la letra: " << (char) s[busq_i] << " dentro del NLS" << endl;
#endif
			}
			(*busqNChar) = '\0';
#ifdef _TRAZA_CREARNUEVARAMA_
			cout << "\t\tCrearNuevaRama. NL final creado. Despues de CrearNLS en busqIndiceNChar: " << busqIndiceNChar << ": "; ImprimirNL(busqIndiceActual, cout); cout << endl;
#endif
		}
		else {
			busqIndicePadre = busqIndiceActual;
			busqPadre = busqActual;
		}

		if(busqNumTrieFallo == 1) {
			busqRaiz_IT = busqActual;
			busqIndiceRaiz_IT = busqIndiceActual;
		}
		else if(busqNumTrieFallo == 2){
			busqIndiceTrie2 = busqIndiceActual;
		}
		//    		if(busqError) {
		//    			cout << "\tCrearNuevaRama. NL. busqNumTrieFallo: " << busqNumTrieFallo << " busqIndicePadre: "; ImprimirNL(busqIndicePadre, cout); cout << endl;
#ifdef _TRAZA_CREARNUEVARAMA_
		cout << "\t\tCrearNuevaRama. NL final creado: "; ImprimirNL(busqIndiceActual, cout); cout << endl;
#endif
		//    		}
	}
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ FUNCIONES DE COMPRESION

void MergedTrie::AgruparActivarBloqueNuevo() {
	//#define _TRAZA_AGRUPAR_ACTIVAR_BLOQUES_
#ifdef _TRAZA_AGRUPAR_ACTIVAR_BLOQUES_
	cout << "\tAgruparActivarBloqueNuevo!!!!" << endl;
#endif
	tamBloqueNL = tamBloqueNL_nuevo;
	tamBloqueNIT = tamBloqueNIT_nuevo;
	tamBloqueNChar = tamBloqueNChar_nuevo;

	bloqueNL = bloqueNL_nuevo;
	bloqueNIT = bloqueNIT_nuevo;
	bloqueNChar = bloqueNChar_nuevo;
	bloqueDataNIT = bloqueDataNIT_nuevo;

	numNL = numNL_nuevo;
	numNIT = numNIT_nuevo;
	numNChar = numNChar_nuevo;
}

void MergedTrie::AgruparActivarBloqueViejo() {
#ifdef _TRAZA_AGRUPAR_ACTIVAR_BLOQUES_
	cout << "\tAgruparActivarBloqueViejo!!!!" << endl;
#endif
	tamBloqueNL = tamBloqueNL_viejo;
	tamBloqueNIT = tamBloqueNIT_viejo;
	tamBloqueNChar = tamBloqueNChar_viejo;

	bloqueNL = bloqueNL_viejo;
	bloqueNIT = bloqueNIT_viejo;
	bloqueNChar = bloqueNChar_viejo;
	bloqueDataNIT = bloqueDataNIT_viejo;

	numNL_nuevo = numNL;
	numNIT_nuevo = numNIT;
	numNChar_nuevo = numNChar;

	numNL = numNL_viejo;
	numNIT = numNIT_viejo;
	numNChar = numNChar_viejo;
}

// Agrupa los nodos, juntando los nodos hermanos para disminuir el numero de nodos del tipo sig
void MergedTrie::AgruparNodosAux() {
	//#define _TRAZA_AGRUPARAUX_
#ifdef _TRAZA_AGRUPARAUX_
	cout << "\n\n\nAgruparNodosAux. Principio. raizTrie1: " << raizTrie1 << " tamBloqueNL_nuevo: " << tamBloqueNL_nuevo << " tamBloqueNIT_nuevo: " << tamBloqueNIT_nuevo << endl << endl << endl;
#endif

	// Datos de estadisticas
	unsigned int tamanyoVectorMaxNletra = 0, tamanyoVectorMaxNit = 0, tamanyoVectorNumeroNletra = 0, tamanyoVectorNumeroNit = 0, tamanyoVectorNumeroNL1hijo = 0;
	float tamanyoVectorMedioNletra = 0, tamanyoVectorMedioNit = 0;

	datoCola datoTempCola;

	_NMTRIE_ *actual, *actualComprimido, *pNP1;
	_INDICE_NMTRIE indNP1, indActual, indActualComprimido, indPadre, indTemp, BNL, BNIT, indTHNIT, longChar, indChar, i, j, bInt, indNPfijo;
	double tempExp;
	unsigned char *pChar, *pChar2;
	vector<datoNodoIT> vNodosIT_compresion;
	datoNodoIT datoNodoIT_temp;

	AgruparActivarBloqueNuevo();

	InicializarTrie();

	while (!colaNL_compresion.empty())
	{
		colaNL_compresion.pop();
	}
	colaNL_compresion.push(datoCola(raizTrie1, 0, raizTrie1, 0));
	indices_nuevo[raizTrie1] = raizTrie1;

	busqNumTrieFallo = 1;

	// INICIO recorrido colaNL_compresion de NL
	while(!colaNL_compresion.empty()) {
		// Y tambien hay que cambiar todo queue por stack segun toque
		//			datoTempCola = colaNL_compresion.front();	// Caso de considerarla como queue
		datoTempCola = colaNL_compresion.top();	// Caso de considerarla como stack
		colaNL_compresion.pop();
		indActual = datoTempCola.indiceSinComprimir;
		//			indiceNpadreComprimido = datoTempCola.indiceNpadreComprimido;
		indActualComprimido = datoTempCola.indiceComprimido;

		AgruparActivarBloqueViejo();
#ifdef _TRAZA_AGRUPARAUX_
		//			cout << "\tAgruparNodosAux. Nodo colaNL_compresion extraido. indActual: " << indActual << " indiceNpadreComprimido: " << indiceNpadreComprimido << " indActualComprimido: " << indActualComprimido << " actual: "; ImprimirNL(indActual, cout); cout << endl;
		cout << "\n##########\tAgruparNodosAux. Nodo colaNL_compresion extraido. indActual: " << indActual << " indActualComprimido: " << indActualComprimido << " actual: "; ImprimirNL(indActual, cout); cout << endl;
#endif
		GetNL(indActual, actual);
		GetPrim(*actual, indNP1);
		GetNL(indNP1, pNP1);
		GetIndNLPadre(*pNP1, indPadre);
		GetIndNPfijo_DeUnNL(indActual, indNPfijo);

		GetBNL(*actual, BNL);

		if(EsNLS(*actual))
			GetLetraS(*actual, pChar);
		else
			pChar = NULL;

		GetIndTHNIT(*pNP1, indTHNIT);
		if(indTHNIT != 0) {
			GetBNIT(*actual, *pNP1, datoNodoIT_temp.BNIT_noComprimido);
			datoNodoIT_temp.indNLconTHNIT_comprimido = indActualComprimido;
			datoNodoIT_temp.indTHNIT_noComprimido = indTHNIT;
#ifdef _TRAZA_AGRUPARAUX_
			cout << "\tAgruparNodosAux. vNodosIT_compresion anyadido. BNIT_noComprimido: " << datoNodoIT_temp.BNIT_noComprimido << " indNLconTHNIT_comprimido: " << datoNodoIT_temp.indNLconTHNIT_comprimido << " indTHNIT_noComprimido: " << datoNodoIT_temp.indTHNIT_noComprimido << endl;
#endif
			vNodosIT_compresion.push_back(datoNodoIT_temp);
			datoNodoIT_temp.clear();
		}

		if(BNL > 0) {
			vNodos.clear();
			CrearvNodosNL(*actual);	    // Leyendo THNL antigua
#ifdef _TRAZA_AGRUPARAUX_
			cout << "\t\tAgruparNodosAux. vNodosNL creado: "; ImprimirVNodos(vNodos); cout << endl;
#endif
		}

		AgruparActivarBloqueNuevo();
		GetNL(indActualComprimido, actualComprimido);
		actualComprimido->NL.char1 = 0;

		if(pChar != NULL)
			GetNuevoNL(busqPrincipioTH, busqIndicePrincipioTH, 2);	// El +2 por el NP1/2
		else
			GetNuevoNL(busqPrincipioTH, busqIndicePrincipioTH);	// El +1 por el NP

#ifdef _TRAZA_AGRUPARAUX_
		cout << "\t\tAgruparNodosAux. indices_nuevo[" << indNPfijo << "] = " << busqIndicePrincipioTH << endl;
#endif
		indices_nuevo[indNPfijo] = busqIndicePrincipioTH;
		indices_nuevo[indActual] = indActualComprimido;
		//			UnSetNPmov(*actualComprimido);

		SetPrim_Inicializando(*actualComprimido, busqIndicePrincipioTH);
#ifdef __MODO_SEGURO__
		SetNP(*busqPrincipioTH);
		if(indPadre != 0 && indices_nuevo[indPadre] == 0) {
			cerr << "ERROR_AgruparNodosAux. indices_nuevo[" << indPadre << "] == 0" << endl;
			exit(EXIT_MTRIE);
		}
#endif
		SetIndNLPadre_Inicializando(*busqPrincipioTH, indices_nuevo[indPadre]);

		if(pChar != NULL) {
			++busqPrincipioTH;
#ifdef __MODO_SEGURO__
			SetNP(*busqPrincipioTH);
#endif

			longChar = strlen((char*) pChar);
			++longChar;
			GetNuevoNChar(pChar2, indChar, longChar);
			for(i = 0; i < longChar; ++i, ++pChar2, ++pChar)
				(*pChar2) = (*pChar);

			SetNLS(*actualComprimido);
			SetIndNLS_Inicializando(*busqPrincipioTH, indChar);
		}

		if(BNL > 0) {
			// Estadisticas de tamanyos de vectores en todo el trie
			++tamanyoVectorNumeroNletra;
			tamanyoVectorMedioNletra += BNL;
			if(BNL > tamanyoVectorMaxNletra)
				tamanyoVectorMaxNletra = BNL;
			if(BNL == 1)
				++tamanyoVectorNumeroNL1hijo;

			busqIndiceActual = indActualComprimido;
			busqActual = actualComprimido;

			busq_B = vNodos.size();
			if(!B_compressed) {
				tempExp = (log(busq_B)/log(2));	// Con tamanyos de array de potencias de 2 se gana en eficiencia
				bInt = (int) tempExp;
				if((double) bInt != tempExp)	// Si hay algun decimal hay que incrementarlo para que quepan todos los nodos
					++bInt;
				//				if(bInt != 0)
				//					--bInt;
				busq_B = pow(2, bInt);
			}
			if(EsNumNoCeroPot2(busq_B))
				SetBNLpot2(*actualComprimido);
			//				else
			//					UnSetBNLpot2(*actualComprimido);

			GetNuevoNL(busqPrincipioTH, busqIndicePrincipioTH, busq_B);

			SetBNL_Inicializando(*actualComprimido, busq_B);

			busqIndicePadre = busqIndiceActual;
			busqPadre = busqActual;

			RellenarTH_Colis(false, EsBNLpot2(*busqActual), vNodos, busq_B, busqIndicePrincipioTH, indTemp);
		}
#ifdef _TRAZA_AGRUPARAUX_
		cout << "\tAgruparNodosAux. Nodo FINAL comprimido: "; ImprimirNL(indActualComprimido, cout); cout << endl;
#endif
	}
	// FIN recorrido colaNL_compresion de NL

#ifdef _TRAZA_AGRUPARAUX_
	cout << "\n=============\tAgruparNodosAux. Actualizando NodoITs a nuevo indiceIT comprimido:" << endl;
#endif
	busqNumTrieFallo = 3;

	// INICIO recorrido vector de IT para copiarlos en array comprimido con los nuevosIndicesComprimidos
	for (i=0; i < vNodosIT_compresion.size(); ++i) {
#ifdef _TRAZA_AGRUPARAUX_
		cout << "/////////////////// \tAgruparNodosAux. Tratando nodoIT de vNodosIT_compresion[" << i << "]. BNIT_noComprimido: " << vNodosIT_compresion[i].BNIT_noComprimido << " indNLconTHNIT_comprimido: " << vNodosIT_compresion[i].indNLconTHNIT_comprimido << " indTHNIT_noComprimido: " << vNodosIT_compresion[i].indTHNIT_noComprimido << endl;
#endif
		AgruparActivarBloqueViejo();
		vNodos.clear();
		CrearvNodosNIT(vNodosIT_compresion[i].indTHNIT_noComprimido, vNodosIT_compresion[i].BNIT_noComprimido);	    // Leyendo THNIT antigua
#ifdef _TRAZA_AGRUPARAUX_
		cout << "\t\tAgruparNodosAux. vNodosNIT creado: "; ImprimirVNodos(vNodos); cout << endl;
#endif
		for(j = 0; j < vNodos.size(); ++j) {
			vNodos[j].clave = indices_nuevo[vNodos[j].clave];
		}
#ifdef _TRAZA_AGRUPARAUX_
		cout << "\t\tAgruparNodosAux. vNodosNIT actualizado a nuevos_indices: "; ImprimirVNodos(vNodos); cout << endl;
#endif

		AgruparActivarBloqueNuevo();
		busqIndiceActual = vNodosIT_compresion[i].indNLconTHNIT_comprimido;
		GetNL(busqIndiceActual, busqActual);

		busq_B = vNodos.size();
		// Estadisticas de tamanyos de vectores en todo el trie
		if(busq_B > 0) {
			++tamanyoVectorNumeroNit;
			tamanyoVectorMedioNit += busq_B;
			if(busq_B > tamanyoVectorMaxNit)
				tamanyoVectorMaxNit = busq_B;
		}

		if(!B_compressed) {
			tempExp = (log(busq_B)/log(2));	// Con tamanyos de array de potencias de 2 se gana en eficiencia
			bInt = (int) tempExp;
			if((double) bInt != tempExp)	// Si hay algun decimal hay que incrementarlo para que quepan todos los nodos
				++bInt;
			//				if(bInt != 0)
			//					--bInt;
			busq_B = pow(2, bInt);
		}

		CrearTHNIT();
		RellenarTH_Colis(true, EsBNITpot2(*busqActual), vNodos, busq_B, busqIndicePrincipioTH, busqIndiceActual);
#ifdef _TRAZA_AGRUPARAUX_
		cout << "\n\t\tAgruparNodosAux. THNIT creada: "; ImprimirNL(busqIndiceActual, cout); cout << endl;
#endif
	}
	// FIN recorrido vector de IT para copiarlos en array comprimido con los nuevosIndicesComprimidos

#ifndef __CONFIGURACION_DISTRIBUCION__
	cout << "\tAgruparNodosAux. ESTADISTICAS:" << endl;
	cout << "\t\ttamanyoVectorMaxNletra: " << tamanyoVectorMaxNletra << " tamanyoVectorNumeroNletra: " << tamanyoVectorNumeroNletra << " tamanyoVectorMedioNletra: " << tamanyoVectorMedioNletra / (float) tamanyoVectorNumeroNletra << " tamanyoVectorNumeroNL1hijo: " << tamanyoVectorNumeroNL1hijo << endl;
	cout << "\t\ttamanyoVectorMaxNit: " << tamanyoVectorMaxNit << " tamanyoVectorNumeroNit: " << tamanyoVectorNumeroNit << " tamanyoVectorMedioNit: " << tamanyoVectorMedioNit / (float) tamanyoVectorNumeroNit << endl;
#endif

	AgruparActivarBloqueViejo();
}

// Agrupa los nodos, juntando los nodos hermanos para disminuir el numero de nodos del tipo sig y quitando los nodos multibyte
void MergedTrie::compaction(const _INDICE_NMTRIE& numberOfExtraNodes) {
	if(!this->empty()) {
		//	        cout << endl << "Estado actual ANTES de la reordenacion: " << *this << endl;
#ifndef __CONFIGURACION_DISTRIBUCION__
	#define __COMPRIMIR_TPOS__
#endif

#ifdef __COMPRIMIR_TPOS__
		time_t inicio, fin;
		time(&inicio);
		struct tm * timeinfo;
#endif
		numNL_nuevo = numNIT_nuevo = numNChar_nuevo = 0;	// Por donde vamos rellenando el nuevo vector

		numNL_viejo = numNL;
		numNIT_viejo = numNIT;
		numNChar_viejo = numNChar;

		if(numberOfExtraNodes == 0) {
			tamBloqueNL_nuevo = this->numNL;
			tamBloqueNIT_nuevo = this->numNIT;
			tamBloqueNChar_nuevo = this->numNChar;
		}
		else {
			tamBloqueNL_nuevo = numberOfExtraNodes + 100;
			tamBloqueNIT_nuevo = numberOfExtraNodes + 100;
			tamBloqueNChar_nuevo = numberOfExtraNodes + 100;
		}

		tamBloqueNL_viejo = tamBloqueNL;
		tamBloqueNIT_viejo = tamBloqueNIT;
		tamBloqueNChar_viejo = tamBloqueNChar;

		bloqueNL_nuevo = new _NMTRIE_ [tamBloqueNL_nuevo];
		bloqueNIT_nuevo = new _NIT_ [tamBloqueNIT_nuevo];
		bloqueNChar_nuevo = new unsigned char [tamBloqueNL_nuevo];

		if(dataMT) {
			bloqueDataNIT_viejo = bloqueDataNIT;
			bloqueDataNIT_nuevo = new unsigned int[tamBloqueNIT_nuevo];

			if(!bloqueDataNIT_nuevo) {
				cerr << "ERROR_AgruparNodos no consigue suficiente memoria en el new de bloqueDataNIT_nuevo" << endl;
				exit(EXIT_MTRIE);
			}

			memset(bloqueDataNIT_nuevo, 0, sizeof(unsigned int) * tamBloqueNIT_nuevo);
		}

		if(!bloqueNL_nuevo || !bloqueNChar_nuevo || !bloqueNIT_nuevo) {
			cerr << "ERROR_AgruparNodos no consigue suficiente memoria en el new de bloqueN_nuevo" << endl;
			exit(EXIT_MTRIE);
		}

		memset(bloqueNL_nuevo, 0, sizeof(_NMTRIE_) * tamBloqueNL_nuevo);
		memset(bloqueNChar_nuevo, 0, sizeof(unsigned char) * tamBloqueNL_nuevo);
		memset(bloqueNIT_nuevo, 0, sizeof(_NIT_) * tamBloqueNIT_nuevo);

		bloqueNL_viejo = bloqueNL;
		bloqueNIT_viejo = bloqueNIT;
		bloqueNChar_viejo = bloqueNChar;

		//			list<MergedTrieGeneral*>::iterator p;

		indices_nuevo = new _INDICE_NMTRIE [numNL];
		if(!indices_nuevo) {
			cerr << "ERROR_AgruparNodos no consigue suficiente memoria en el new de indices_nuevo" << endl;
			exit(EXIT_MTRIE);
		}

		memset(indices_nuevo, 0, sizeof(_INDICE_NMTRIE) * numNL);

		//			for(p = listaDeMergedTries.begin();p!=listaDeMergedTries.end();++p) {
		//				(*p)->AbreFichTemporalO();
		//			}

		//	        cout << "Sale de abrir ficheros temporales" << endl;

		unsigned int bITmax, bLetrasMax;
		bITmax = bLetrasMax = 0;

		///////////////////////////////////////////// Compactar trie
		AgruparNodosAux();

		delete [] bloqueNL;
		delete [] bloqueNChar;
		delete [] bloqueNIT;
		delete [] indices_nuevo;

		if(dataMT) {
			delete [] bloqueDataNIT;
			bloqueDataNIT = bloqueDataNIT_nuevo;
			bloqueDataNIT_nuevo = NULL;
		}

		bloqueNL = bloqueNL_nuevo;
		bloqueNChar = bloqueNChar_nuevo;
		bloqueNIT = bloqueNIT_nuevo;

		bloqueNL_nuevo = NULL;	// Para evitar que entre en temas de comprimir
		bloqueNIT_nuevo = NULL;
		bloqueNChar_nuevo = NULL;
		indices_nuevo = NULL;

		tamBloqueNL = tamBloqueNL_nuevo;
		tamBloqueNIT = tamBloqueNIT_nuevo;
		tamBloqueNChar = tamBloqueNChar_nuevo;

		numNL = numNL_nuevo;
		numNIT = numNIT_nuevo;
		numNChar = numNChar_nuevo;

		estaComprimido = true;
#ifdef __COMPRIMIR_TPOS__
time(&fin);

timeinfo = localtime ( &inicio );
cout << "\n\tTiempo inicio: " << asctime(timeinfo);
timeinfo = localtime ( &fin );
cout <<   "\tTiempo fin:    " << asctime(timeinfo) << endl;
cout << "\t\tSegundos transcurridos time: " << (int) difftime(fin,inicio) << endl;
#endif

//	        cout << endl << "Resultado FINAL de la reordenacion: " << endl;
//			cout << "\tEstructura de nodos: " << *this << endl;
//	        ListarMergedTries();
	}
}


