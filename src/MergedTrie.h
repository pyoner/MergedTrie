/* ///////////// PENDIENTE
*/

/*
 * MergedTrie.h
 *
 *  Created on: 17/12/2015
 *      Author: Antonio Ferrandez Rodriguez antonio@dlsi.ua.es
 */

#ifndef ESQUELETO_MTRIE_H_
#define ESQUELETO_MTRIE_H_

#include <stdlib.h>
#include <cstddef>
#include <string>
#include <stdio.h>
#include <cstring>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <list>
#include <stack>
#include <dirent.h>
#include <omp.h>
#include <cmath>
#include <math.h>
#include <algorithm>
#include <vector>
#include <chrono>
#include <climits>
#include <fstream>

#include "MergedTrie_settings.h"

using namespace std;

////////////////////////////////////////// DATOS BNITRIE ///////////////////////////////
// Estructura para la cola de nodos de trie en Comprimir
struct datoCola{
	_INDICE_NMTRIE indiceSinComprimir;
	_INDICE_NMTRIE indiceNpadreComprimido;
	_INDICE_NMTRIE indiceComprimido;
	_INDICE_NMTRIE letra;

	datoCola();
	datoCola(const _INDICE_NMTRIE& kindiceSinComprimir, const _INDICE_NMTRIE& kindiceNpadreComprimido, const _INDICE_NMTRIE& kindiceComprimido, const _INDICE_NMTRIE& kletra);
};

// Estructura para poder ordenar los NL/NIT
struct datoNodo {
	_INDICE_NMTRIE clave;
	_INDICE_NMTRIE indiceNodoSinComprimir;
	_INDICE_NMTRIE indiceComprimido;
	_INDICE_NMTRIE posHash;	// Pos en TH de tamanyo BNL/BNIT

	datoNodo();
	void clear();
	void clearPosHash();
	bool operator<(datoNodo b) const;
	void ImprimirLeyenda();
	void Imprimir(const bool& esLetra);
};

// Estructura para poder copiar los nodos IT en el trie comprimido despues de haber actualizado los indices de NLs comprimidos
struct datoNodoIT {
	_INDICE_NMTRIE indNLconTHNIT_comprimido;	// Indice BNIT1 en THNL comprimida
	_INDICE_NMTRIE indTHNIT_noComprimido;	// Indice de principio de TH_NIT no comprimida
	_INDICE_NMTRIE BNIT_noComprimido;	// BNIT THNIT no comprimida

	datoNodoIT();
	void clear();
};

union nodoBloqueNL {
	struct {
		unsigned int campo1;
		unsigned int campo2;

	} NP;

	struct {
		unsigned char char1;
		unsigned char char2;
		unsigned char char3;
		unsigned char char4;
		unsigned int prim;
	} NL;

	struct {
		unsigned long long nodo;
	} todo;

	nodoBloqueNL();
	nodoBloqueNL(const nodoBloqueNL& n);
	nodoBloqueNL& operator=(const nodoBloqueNL& n);
	nodoBloqueNL& operator=(const unsigned int& n);
	bool operator==(const unsigned int& n);
	bool operator!=(const nodoBloqueNL& n);
	bool operator!=(const unsigned int& n);
	bool operator==(const nodoBloqueNL& n);
	bool EsVacio();
	void clear();
};

class MergedTrie {
	friend ostream& operator<<(ostream &os, const MergedTrie &n);
public:
	// If kB_compressed = true, then in the compaction operation, the size of the Hash Tables is adjusted to the size of the Table. Otherwise, it is set to a power of two in order to improve the temporal efficiency
	// kSizeOfTheArrayOfLN, kSizeOfTheArrayOfELN and kSizeOfTheArrayOfIT are the sizes of the arrays used in the MergedTrie. By default the sizes are set to 113000000, 12000000 and 260000000 respectively
	MergedTrie(const bool& kB_compressed = true, const _INDICE_NMTRIE& kSizeOfTheArrayOfLN = __Tam_tamBNL__, const _INDICE_NMTRIE& kSizeOfTheArrayOfELN = __Tam_tamBChar__, const _INDICE_NMTRIE& kSizeOfTheArrayOfIT = __Tam_tamBNIT__);
	MergedTrie(const string& directory);	// The MergedTrie is initialized from the one stored in "directory"
	MergedTrie(const MergedTrie& n);
	~MergedTrie();

	MergedTrie& operator= (const MergedTrie& t);

	bool find(const string& s);
	bool insert(const string& s);
	void compaction(const _INDICE_NMTRIE& numberOfExtraNodes = 0);

	void print() const;
	// prefix_print returns the number of words (until a maximum number of maxPrefixExtracted) stored in the MergedTrie que share the string "prefix"
	//	It writes them in "ostream os"
	int prefix_print(const string& prefix, ostream& os, const int& maxPrefixExtracted = 999999999);
	void print(ostream& os, const bool& print_words = true, const bool& print_statistics = true, const bool& print_MergedTrie_nodes = false) const;

	void clear();	// Empty the MergedTrie without deleting its memory
	bool empty() const;	// It returns whether the MergedTrie is empty: it has no stored words
	// It returns whether the number of stored words in the MergedTrie
	size_t size() const;

	void store_in_directory(const string& directory);
	void read_from_directory(const string& directory, const bool& freeMemory = true, const bool& keepPreviousSizesOfArrays = false);

protected:
	_INDICE_NMTRIE DevolverIndiceActual();
	_INDICE_NMTRIE DevolverIndiceDatoActual();
	// The parameter kdataMT is set from the D_MergedTrie. It is set to true when the MergedTrie contains additional data as well as the words
	MergedTrie(const bool& kB_compressed, const _INDICE_NMTRIE& kSizeOfTheArrayOfLN, const _INDICE_NMTRIE& kSizeOfTheArrayOfELN, const _INDICE_NMTRIE& kSizeOfTheArrayOfIT, const bool& kdataMT);
	void SetDataMT(const _INDICE_NMTRIE& indice, const _INDICE_NMTRIE& indiceData);

private:
	void InicializarArrays(const bool& kB_compressed, const _INDICE_NMTRIE& kSizeOfTheArrayOfLN, const _INDICE_NMTRIE& kSizeOfTheArrayOfELN, const _INDICE_NMTRIE& kSizeOfTheArrayOfIT, const bool& kdataMT);
	void Copia(const MergedTrie& t);
	void InicializarTrie();
	_NMTRIE_* GetRaiz();
	void Mascaras();
	_INDICE_NMTRIE DivideStringTrie2(const string& s);
	_INDICE_NMTRIE DivideStringTrie2(const _INDICE_NMTRIE& longitud);
	void TrazaError();
#ifdef __MODO_SEGURO__
	bool EsNP(const _NMTRIE_& nodoNP) const;
	void SetNP(_NMTRIE_& nodoNP);
#endif
	void GetIndNLPadre(const _NMTRIE_& nodoNP, _INDICE_NMTRIE& kprim) const ;
	void SetIndNLPadre_Vacio(_NMTRIE_& nodoNP, const _INDICE_NMTRIE& indNPpadre);
	void SetIndNLPadre_Acero(_NMTRIE_& nodoNP);
	void SetIndNLPadre_Inicializando(_NMTRIE_& nodoNP, const _INDICE_NMTRIE& kprim);
	void GetIndTHNIT(const _NMTRIE_& nodoNP, _INDICE_NMTRIE& kprim) const ;
	void SetIndTHNIT_Vacio(_NMTRIE_& nodoNP, const _INDICE_NMTRIE& kprim);
	void SetIndTHNIT_Acero(_NMTRIE_& nodoNP);
	void SetIndTHNIT_Inicializando(_NMTRIE_& nodoNP, const _INDICE_NMTRIE& kprim);
	void SetBNIT_Vacio(_NMTRIE_& nodoNL, _NMTRIE_& nodoNP, const _INDICE_NMTRIE& kprim);
	void SetBNIT_Acero(_NMTRIE_& nodoNL, _NMTRIE_& nodoNP);
	void SetBNIT_Inicializando(_NMTRIE_& nodoNL, _NMTRIE_& nodoNP, const _INDICE_NMTRIE& kprim);
	void GetBNIT(const _NMTRIE_& nodoNL, const _NMTRIE_& nodoNP, _INDICE_NMTRIE& B) const ;
	void GetIndNPfijo_DeUnNL(const _INDICE_NMTRIE& indNL, _INDICE_NMTRIE& indNP) const;
	void GetIndNPfijo(const _NMTRIE_& nodoNP, _INDICE_NMTRIE& kprim) const ;
	void SetIndNPfijo_Vacio(_NMTRIE_& nodoNP, const _INDICE_NMTRIE& indNPfijo);
	void SetIndNPfijo_Acero(_NMTRIE_& nodoNP);
	void SetIndNPfijo_Inicializando(_NMTRIE_& nodoNP, const _INDICE_NMTRIE& kprim);
	void GetIndNLS_DeUnNL(const _NMTRIE_& nodoNL, _INDICE_NMTRIE& kprim) const ;
	void GetIndNLS(const _NMTRIE_& nodoNP, _INDICE_NMTRIE& kprim) const ;
	void SetIndNLS_Vacio(_NMTRIE_& nodoNP, const _INDICE_NMTRIE& kprim);
	void SetIndNLS_Acero(_NMTRIE_& nodoNP);
	void SetIndNLS_Inicializando(_NMTRIE_& nodoNP, const _INDICE_NMTRIE& kprim);
#ifdef __MODO_SEGURO__
	bool EsNL(const _NMTRIE_& nodoNL) const;
#endif
	bool EsNPdoble(const _NMTRIE_& nodoNL) const;
	bool EsNLS(const _NMTRIE_& nodoNL) const;
	void SetNLS(_NMTRIE_& nodoNL);
	void UnSetNLS(_NMTRIE_& nodoNL);
	bool EsTHNIT(const _NMTRIE_& nodoNL) const;
	bool EsNPmov(const _NMTRIE_& nodoNL) const;
	void SetNPmov(_NMTRIE_& nodoNL);
	void UnSetNPmov(_NMTRIE_& nodoNL);
	bool EsBNLpot2(const _NMTRIE_& nodoNL) const;
	void SetBNLpot2(_NMTRIE_& nodoNL);
	void UnSetBNLpot2(_NMTRIE_& nodoNL);
	bool EsBNITpot2(const _NMTRIE_& nodoNL) const;
	void SetBNITpot2(_NMTRIE_& nodoNL);
	void UnSetBNITpot2(_NMTRIE_& nodoNL);
	void GetPrim(const _NMTRIE_& nodoNL, _INDICE_NMTRIE& kprim) const ;
	void SetPrim_Acero(_NMTRIE_& nodoNL);
	void SetPrim_Vacio(_NMTRIE_& nodoNL, const _INDICE_NMTRIE& kprim);
	void SetPrim_Inicializando(_NMTRIE_& nodoNL, const _INDICE_NMTRIE& kprim);
	void GetColisNL(const _NMTRIE_& nodoNL, _INDICE_NMTRIE& kprim) const ;
	void SetColisNL_Vacio(_NMTRIE_& nodoNL, const _INDICE_NMTRIE& kprim);
	void SetColisNL_Acero(_NMTRIE_& nodoNL);
	void SetColisNL_Inicializando(_NMTRIE_& nodoNL, const _INDICE_NMTRIE& kprim);
	void GetBNL(const _NMTRIE_& nodoNL, _INDICE_NMTRIE& kprim) const ;
	void SetBNL_Acero(_NMTRIE_& nodoNL);
	void SetBNL_Vacio(_NMTRIE_& nodoNL, const _INDICE_NMTRIE& kprim);
	void SetBNL_Inicializando(_NMTRIE_& nodoNL, const _INDICE_NMTRIE& kprim);
	void GetLetra(const _NMTRIE_& nodoNL, _INDICE_NMTRIE& kprim) const ;
	void SetLetra(_NMTRIE_& nodoNL, const _INDICE_NMTRIE& kprim);
	void GetLetraS(const _NMTRIE_& nodoNL, unsigned char*& pChar) const ;
	void GetLetraS(const _NMTRIE_& nodoNL, _INDICE_NMTRIE& indChar) const ;
	bool GetColisNIT(const _NIT_& nodoNIT) const ;
	void SetColisNIT(_NIT_& nodoNIT);
	bool GetBitNIT_NLS(const _NIT_& nodoNIT) const ;
	void SetBitNIT_NLS(_NIT_& nodoNIT);
	void GetIT(const _NIT_& nodoNIT, _INDICE_NMTRIE& kprim) const ;
	void SetIT(_NIT_& nodoNIT, const _INDICE_NMTRIE& kprim) ;
	void ListarPalabrasNLETRAS(ostream &os) const;
	void EscribePalabra(const _INDICE_NMTRIE& imprimirPalabrasIT1, const bool& esNLS1,  const _INDICE_NMTRIE& imprimirPalabrasIT2, const bool& esNLS2, ostream &os) const;
	void EscribePalabra(list<pair <_INDICE_NMTRIE, bool>>& listaIndiceTrie, ostream &os) const;
	void ObtenerIndicePadre(_INDICE_NMTRIE& indiceActual) const;
	void ObtenerNLdeUnNP(const _INDICE_NMTRIE& indNP, _INDICE_NMTRIE& indNL) const;
	void ObtenerCadenaAscendente(_INDICE_NMTRIE indiceActual, const bool& inversa, bool esNLS, string& cadena) const;
	void ImprimirBits(const unsigned int& kk, ostream& os) const;
	void ImprimirBits(const unsigned char& kk, ostream& os) const;
	void ImprimirVNodos(vector<datoNodo>& vNodos);
	void ObtenerBitMasSignificativo(_INDICE_NMTRIE num, _INDICE_NMTRIE& bit) const;
	void ModNoPot2_ObtenerB2(const _INDICE_NMTRIE& B, _INDICE_NMTRIE& B2) const;
	void ModNoPot2(const _INDICE_NMTRIE& clave, const _INDICE_NMTRIE& B, const _INDICE_NMTRIE& B2, _INDICE_NMTRIE& modulo) const;
	void ModNoPot2(const _INDICE_NMTRIE& clave, const _INDICE_NMTRIE& B, _INDICE_NMTRIE& modulo) const;
	void ModNoPot2_256(const _INDICE_NMTRIE& clave, const _INDICE_NMTRIE& B, _INDICE_NMTRIE& modulo) const;
	void ModPot2(const _INDICE_NMTRIE& clave, const _INDICE_NMTRIE& B, _INDICE_NMTRIE& posHash);
	void ModPot2(const unsigned char& clave, const _INDICE_NMTRIE& B, _INDICE_NMTRIE& posHash);
	void ObtenerSiguienteColision(const _INDICE_NMTRIE& indPrincTH, const _INDICE_NMTRIE& indFinTH, const _INDICE_NMTRIE& indActual, const _INDICE_NMTRIE& colision, _INDICE_NMTRIE& indSiguienteColis) const;
	void ImprimirNL(const _INDICE_NMTRIE& indNL1, ostream &s) const;
	void ImprimirNP(const _NMTRIE_ NL1, const _NMTRIE_ NP1, const _INDICE_NMTRIE& indNP1, ostream &s) const;
	void ImprimirTHNIT(const _INDICE_NMTRIE& indPrincTHNIT, const _INDICE_NMTRIE& BNIT, ostream &s) const;
	void ImprimirNIT(const _NIT_& NIT, ostream &s) const;
	void ImprimirTHNL(const _NMTRIE_& NL, ostream &s) const;
	void ImprimirLetraOLetraS_NP(const _NMTRIE_& NP, _INDICE_NMTRIE indNP, const bool& esNLS, ostream &s) const;
	void ImprimirLetraOLetraS(const _NMTRIE_& NL, ostream &s) const;
	void ListarNodosIT(ostream& os) const;
	void ListarNodosChar(ostream& os) const;
	string NumerosConSeparadoresMiles(const unsigned long long& value) const;
	void prefix_print_THNL(const string& prefix, const _INDICE_NMTRIE& busq_i2, _INDICE_NMTRIE& numPrefijos, ostream& os, const string& s, const _INDICE_NMTRIE& indActual, const int& maxPrefixExtracted);
	void prefix_print_THNIT(const string& prefix, const _INDICE_NMTRIE& busq_i, _INDICE_NMTRIE& numPrefijos, ostream& os, const string& s, _INDICE_NMTRIE indActual, const int& maxPrefixExtracted);
	void RecuperarDeficheroMergedTriesAux(const string& directory, const bool& liberarMemoria, const bool& mantieneBN=false, const bool& imprimir=false);
	void RecuperarDeficheroSoloMergedTries(const string& directory, const bool& liberarMemoria);
	void RecuperarDeficheroSoloMergedTries(const int& j, const string& directory, const bool& liberarMemoria, const bool& imprimir);
	void GetNL(const _INDICE_NMTRIE& indiceNodo, _NMTRIE_*& pIndiceNodo) const;
	void GetNIT(const _INDICE_NMTRIE& indiceNodo, _NIT_*& pIndiceNodo) const;
	void GetNChar(const _INDICE_NMTRIE& indiceNodo, unsigned char*& pIndiceNodo) const;
	void AmpliarArrayChar(_INDICE_NMTRIE& numNL, _INDICE_NMTRIE& tamBloqueNL, unsigned char*& bloqueNChar_1);
	void AmpliarArrayNL(_INDICE_NMTRIE& numNL, _INDICE_NMTRIE& tamBloqueNL, _NMTRIE_*& bloqueNL_1, const string& origen);
	void AmpliarArrayNIT(_INDICE_NMTRIE& numNL, _INDICE_NMTRIE& tamBloqueNL, _NIT_*& bloqueNL_1, const string& origen);
	void GetNuevoNL(_NMTRIE_*& actual, _INDICE_NMTRIE& indAdevolver, const int& numNodosAcrear = 1);
	void GetNuevoNIT(_INDICE_NMTRIE& indAdevolver, const int& numNodosAcrear = 1);
	void GetNuevoNChar(unsigned char*& actual, _INDICE_NMTRIE& indAdevolver, const int& numNodosAcrear = 1);
	void CrearTHNL(const _INDICE_NMTRIE& B);
	bool EsNumPot2(const _INDICE_NMTRIE& i) const;
	bool EsNumNoCeroPot2(const _INDICE_NMTRIE& i) const;
	void CrearTHNIT();
	void CrearNL(_NMTRIE_& NL1, const _INDICE_NMTRIE& letra, const _INDICE_NMTRIE& indPadre);
	bool BuscarPalabraAuxTrie1(const string& s);
	bool BuscarPalabraAuxTrie2(const string& s);
	bool BuscarHijo();
	bool BuscarPalabraAuxIT();
	void GetC(const _INDICE_NMTRIE& clave, const unsigned int& B, unsigned int& c);
	void DividirNLS();
	void BuscarUltimaClaveColisionada(_INDICE_NMTRIE& indice, const _INDICE_NMTRIE& busq_B, const _INDICE_NMTRIE& busqIndicePrincipioTH);
	void RellenarTH_Colis(const bool& esTHNIT, const bool& esPot2, vector<datoNodo>& vNodosT, const _INDICE_NMTRIE& B, const _INDICE_NMTRIE& busqIndicePrincipioTHtemp, _INDICE_NMTRIE& indNuevaClave);
	bool BuscaHuecoTHNIT(_INDICE_NMTRIE indInicial, _INDICE_NMTRIE posHash, _INDICE_NMTRIE& indHueco, const _INDICE_NMTRIE& c, const _INDICE_NMTRIE& busq_B, const bool& esPot2, const _INDICE_NMTRIE& busqIndicePrincipioTH, const _INDICE_NMTRIE& busqIndiceFinTH);
	bool BuscaHuecoTHNL(const _INDICE_NMTRIE& indInicial, _INDICE_NMTRIE& indHueco, _INDICE_NMTRIE& incremento, const _INDICE_NMTRIE& busq_B, const _INDICE_NMTRIE& busqIndicePrincipioTH, const _INDICE_NMTRIE& busqIndiceFinTH);
	void CrearvNodosNIT(const _INDICE_NMTRIE& indPrincTH, const _INDICE_NMTRIE& B);
	void CrearvNodosNL(const _NMTRIE_& NL);
	bool CrearNuevaRamaCondicion();
	bool CrearNuevaRamaCondicionUltimaLetra();
	void CrearNuevaRamaIncremento();
	void CrearNuevaRamaDecremento();
	void CrearNuevaRama(const string& s);
	void AgruparActivarBloqueNuevo();
	void AgruparActivarBloqueViejo();
	void AgruparNodosAux();






	_INDICE_NMTRIE tamBloqueNL;	// Tamanyo de bloqueNL
	_INDICE_NMTRIE tamBloqueNIT;	// Tamanyo de bloqueNIT
	_INDICE_NMTRIE tamBloqueNChar;	// Tamanyo de bloqueNChar

	_NMTRIE_* bloqueNL;	// Contendra los NL/NP
	_NIT_* bloqueNIT;	// Contendra los NIT
	unsigned char* bloqueNChar;	// Contendra las cadenas de caracteres comprimidas

	_INDICE_NMTRIE* arrayModulo;	// Almacena el numero todo a unos de tamanyo el bit mas significativo, para el calculo de ModNoPot2_256
	_INDICE_NMTRIE numNL;	// Ultima posicion ocupada en el ultimo bloqueNL
	_INDICE_NMTRIE numNIT;	// Ultima posicion ocupada en el ultimo bloqueNIT
	_INDICE_NMTRIE numNChar;	// Ultima posicion ocupada en el ultimo bloqueNChar

	_INDICE_NMTRIE raizTrie1;		// indiceNodo del primer nodo de trie1

	_INDICE_NMTRIE cant;			// Numero de palabras que contiene el trie

	int numBitsEOW;		// Numero de bits para la estructura EOW. Ese numero sera el maximo de MergedTries a mantener en memoria simultaneamente
	int numBitsIdEOW;	// Numero de bits para representar un id de MergedTrie. Esta en funcion de numBitsEOW = ceil(log(numBitsEOW)/log(2));

	bool estaComprimido;	// Si true significa que esta comprimido y no modificado (o sea que no hay nodos letra despues de nodos IT o ITdato, por lo que la busqueda se finaliza tras explorar los nodos letra seguidos en el ABB completo)
	bool B_compressed;	// If true, then in the compaction operation, the size of the Hash Tables is adjusted to the size of the Table. Otherwise, it is set to a power of two in order to improve the temporal efficiency

	bool dataMT;	// If true, then the MergedTrie also contains data, in which case bloqueDataNIT will contain the indices of the data
	unsigned int* bloqueDataNIT;	// Contendra la direccion del dato en la clase D_MergedTrie



	//////////////////// NP
	unsigned int mascaraBitNP;

	unsigned int maxIndNPfijo;
	unsigned int mascaraIndNPfijo;
	unsigned int mascaraIndNPfijo_Acero;

	unsigned int maxIndNLPadre;
	unsigned int mascaraIndNLPadre;
	unsigned int mascaraIndNLPadre_Acero;

	unsigned int maxBNIT;	// Max. (BNITp4 + BNITp3 + BNITp2 + BNITp1)

	unsigned int maxBNITp3;
	unsigned int mascaraBNITp3;
	unsigned int mascaraBNITp3_Acero;

	unsigned int maxBNITp4;
	unsigned int mascaraBNITp4;
	unsigned int mascaraBNITp4_Acero;

	unsigned int maxIndTHNIT;
	unsigned int mascaraIndTHNIT;
	unsigned int mascaraIndTHNIT_Acero;

	unsigned int maxIndNLS;
	unsigned int mascaraIndNLS;
	unsigned int mascaraIndNLS_Acero;

	//////////////////// NL
	unsigned char mascaraBitNL;

	unsigned char mascaraBitNLS;
	unsigned char mascaraBitNLS_Acero;

	unsigned char mascaraBitNPmov;
	unsigned char mascaraBitNPmov_Acero;

	unsigned char mascaraBitBNLpot2;
	unsigned char mascaraBitBNLpot2_Acero;

	unsigned char mascaraBitBNITpot2;
	unsigned char mascaraBitBNITpot2_Acero;

	unsigned int maxBNITp1;
	unsigned char mascaraBNITp1;
	unsigned char mascaraBNITp1_Acero;

	unsigned int maxPrim;
	unsigned int mascaraPrim;
	unsigned int mascaraPrim_Acero;

	unsigned int maxColisNL;

	unsigned int maxBNL;

	unsigned int maxBNITp2;
	unsigned int mascaraBNITp2;
	unsigned int mascaraBNITp2_Acero;


	//////////////////// NIT
	_NIT_ mascaraColisNIT;

	_NIT_ mascaraBitNIT_NLS;

	_NIT_ mascaraIT;
	_NIT_ maxIT;

//	list<MergedTrieGeneral*> listaDeMergedTries;		// Listado de los MergedTries que trabajan sobre esta estructura de Trie comprimido

   	bool tablaAmpliada;	// En caso que se amplie alguna tabla, entonces se activara esta variable a true. Sino, siempre a false




   	/////////////////////////////////////////////////////// INICIO Variables temporales para la busqueda/insercion
	_INDICE_NMTRIE busqIndiceActual, busqIndiceTemp, busqIndicePadre, busqIndicePrincipioTH, busqIndiceFinTH, busqIndiceNP;
	_INDICE_NMTRIE busqIndiceTrie2, busqTrie2Inicio;
	_INDICE_NMTRIE busqIndiceNChar;	// Indice en bloqueNChar[busqIndiceNChar] que no coincide con cadena[busq_i] en NLS
	_INDICE_NMTRIE busqSLength;
	_INDICE_NMTRIE busqLetraTemp, busqLetra;
	_INDICE_NMTRIE busqITTemp, busqIT;
	_INDICE_NMTRIE busqIndiceRaiz_IT;	// Indice asociado a busqRaiz_IT
	_INDICE_NMTRIE busqIndiceNLS;	// Indice que se usara en DividirNLS
	_INDICE_NMTRIE busqC;

	_NMTRIE_ *busqRaiz_IT;		// NL que contiene el IT1
	_NMTRIE_ *busqActual, *busqPadre, *busqPrincipioTH, *busqNP, *busqTrie2;

	bool busqFalloNLS;		// Si es true el fallo en la busqueda se habra producido en la cadena almacenada en el NLS
	bool busqUltimaLetraEnPrefijo;	// Sera true cuando la ultima letra de la cad a insertar pertenezca al prefijo comun de un NLS
	bool busqITesNLS;		// Sera true si indiceTrie2 es NLS para almacenarlo en el nuevo IT almacenado

	bool busqEsBNLpot2;		// Sera true si BNL de THNL actual es pot2
	bool busqEsBNITpot2;	// Sera true si BNIT de THNIT actual es pot2

	bool busqError;		// Se activara cuando se quiera detectar un error de ejecucion

	_NIT_ *busqActualNIT;

	_INDICE_NMTRIE busqPosHash, busq_i, busq_iB, busqNumTrieFallo;

	bool busqInsertar;	// si se ejecuta desde insert estara a true. Sino a false (por defecto)

	_INDICE_NMTRIE busq_B;	// Se utiliza solo en BuscarHijo/IT para comunicarse con CrearNuevaRama para saber el bf de la nueva tabla anidada a crear

	unsigned char* busqNChar;	// Para manejo de los NLS en bloqueNChar

   	/////////////////////////////////////////////////////// INICIO Variables temporales para la Compresion
	_INDICE_NMTRIE *indices_nuevo;  	// Contiene los nuevos indices de en los que se almacenan los nodos de bloqueNL dentro de bloqueNL_nuevo: bloqueNL[indAntiguo] = indDelNodoEnBloqueNLnuevo

	_INDICE_NMTRIE raizTrie1_viejo;

	_INDICE_NMTRIE tamBloqueNL_nuevo, tamBloqueNIT_nuevo, tamBloqueNChar_nuevo;
	_INDICE_NMTRIE tamBloqueNL_viejo, tamBloqueNIT_viejo, tamBloqueNChar_viejo;

	_NMTRIE_ *bloqueNL_nuevo;
	_NIT_ *bloqueNIT_nuevo;
	unsigned int *bloqueDataNIT_nuevo;
	unsigned char *bloqueNChar_nuevo;

	_NMTRIE_* bloqueNL_viejo;
	_NIT_ *bloqueNIT_viejo;
	unsigned int *bloqueDataNIT_viejo;
	unsigned char* bloqueNChar_viejo;

	_INDICE_NMTRIE numNL_nuevo, numNIT_nuevo, numNChar_nuevo;	// Por donde vamos rellenando el nuevo vector
	_INDICE_NMTRIE numNL_viejo, numNIT_viejo, numNChar_viejo;

	stack<datoCola> colaNL_compresion;


   	/////////////////////////////////////////////////////// INICIO Demas variables temporales
   	unsigned int arrayC[20];	// Array de numeros primos (para que busque siempre en toda la tabla) para definir los valores de c en las colisiones

   	ldiv_t busqDivresult;

//	bool vD[16384];	// vector de bool utilizado en CalcularNLD y NITD para detectar claves colisionadas

	vector<datoNodo> vNodos;
	_INDICE_NMTRIE vNodosTam;
	datoNodo datoTemp;

	/////////// FIN Variables temporales

};

template<class T>
class D_MergedTrie: public MergedTrie
{
	friend ostream& operator<<(ostream &os, const D_MergedTrie &n){
	    n.print(os, false, true, false);

	    return os;
	}
public:
	// kSizeOfTheArrayOfData is the initial size of the array that stores the data of the MergedTrie
	// If kB_compressed = true, then in the compaction operation, the size of the Hash Tables is adjusted to the size of the Table. Otherwise, it is set to a power of two in order to improve the temporal efficiency
	// kSizeOfTheArrayOfLN, kSizeOfTheArrayOfELN and kSizeOfTheArrayOfIT are the sizes of the arrays used in the MergedTrie. By default the sizes are set to 113000000, 12000000 and 260000000 respectively
	D_MergedTrie(const _INDICE_NMTRIE& kSizeOfTheArrayOfData = __Tam_tamBNL__, const bool& kB_compressed = true, const _INDICE_NMTRIE& kSizeOfTheArrayOfLN = __Tam_tamBNL__, const _INDICE_NMTRIE& kSizeOfTheArrayOfELN = __Tam_tamBChar__, const _INDICE_NMTRIE& kSizeOfTheArrayOfIT = __Tam_tamBNIT__): MergedTrie(kB_compressed, kSizeOfTheArrayOfLN, kSizeOfTheArrayOfELN, kSizeOfTheArrayOfIT, true) {
		BN0 = __Tam_tamDataMergedTrie__;
    	bloque0conDatos = new T [BN0];
//		cantBloqueDatos = 0;
	}

    D_MergedTrie(const D_MergedTrie& n) {
//        cantBloqueDatos = n.cantBloqueDatos;
		BN0 = n.BN0;
    	bloque0conDatos = new T [BN0];

		Copia(n);
	}

	// The MergedTrie is initialized from the one stored in "directory"
    D_MergedTrie(const string& directory): MergedTrie(directory) {
    	read_from_directory(directory, false, false);
    }

	~D_MergedTrie() {
		clear();
        if(bloque0conDatos != NULL) {
        	delete [] bloque0conDatos;
        	bloque0conDatos = NULL;
        }
	}

	// Empty the MergedTrie without deleting its memory
	void clear() {
//        cantBloqueDatos = 0;
        MergedTrie::clear();
	}

	D_MergedTrie& operator=(const D_MergedTrie& t)
    {
        if(this!=&t)
        {
            clear();
            // Hacemos la copia del objeto
            Copia(t);
        }
        return *this;
    }

    void Copia(const D_MergedTrie& t) {
//        cantBloqueDatos = t.cantBloqueDatos;
		BN0 = t.BN0;

		if(BN0 != t.BN0) {
			delete [] bloque0conDatos;
			bloque0conDatos = new T [BN0];
		}
		memcpy(bloque0conDatos, t.bloque0conDatos, sizeof(T) * BN0);
	}

	void read_from_directory(const string& directory, const bool& freeMemory = true, const bool& keepPreviousSizesOfArrays = false) {
		MergedTrie::read_from_directory(directory, freeMemory, keepPreviousSizesOfArrays);
//		cout << "Despues de leer el MT: "; MergedTrie::print();

	  	bool imprimir=false;

		string nomFich0, nomFich;
		ifstream f;
		int i, j;
		_INDICE_NMTRIE kcantEsteTrie;

		if(imprimir)
			cout << "\n\n************************************* RECUPERANDO ESQUELETO DE D_MergedTrieS" << endl;

		nomFich0 = nomFich = directory+"/D_MergedTrieSettings.txt";
		f.open(nomFich.c_str());
		string linea;
		if(!f.fail())
		{
			f >> kcantEsteTrie;

			if(keepPreviousSizesOfArrays && kcantEsteTrie < BN0) {

			}
			else {
				i=0;
				if(freeMemory) {
					delete [] bloque0conDatos;
				}
				bloque0conDatos = new T[kcantEsteTrie];

				BN0 = kcantEsteTrie;
			}
			cout << "BN0: " << BN0 << endl;

			if(BN0 > 0) {
				f.close();
				nomFich = nomFich0 + ".bin";
				f.open(nomFich.c_str(), ios::in | ios::binary);
				f.read(reinterpret_cast<char *>(bloque0conDatos), sizeof(T)* BN0);
			}
		}
		else {
			cerr << "ERROR_read_from_directory. The file: " << nomFich << " does not exist" << endl;
			exit(EXIT_MTRIE);
		}
		f.close();
		if(imprimir)
			cout << "\n\n************************************* ESQUELETO DE D_MergedTrie RECUPERADO" << endl;
	}

	void store_in_directory(const string& directory) {
		MergedTrie::store_in_directory(directory);

	  	string nomFich0, nomFich;
	  	bool ret=false;
	  	ofstream f;
	  	int i;

		nomFich0 = nomFich = directory+"/D_MergedTrieSettings.txt";
		f.open(nomFich.c_str());
		if(!f.fail())
		{
			f << MergedTrie::size() << " ";
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
			f.write(reinterpret_cast<char *> (bloque0conDatos), sizeof(T) * BN0);
		}
		else {
			cerr << "ERROR_store_in_directory while creating the file: " << nomFich << endl;
			exit(EXIT_MTRIE);
		}
    	f.close();
	}

    void AmpliarArrayDatos(const _INDICE_NMTRIE& numNL, _INDICE_NMTRIE& tamBloqueNL, T*& bloqueNChar_1) {
		T *bloqueNChartemp;
		while(numNL >= tamBloqueNL) {
			_INDICE_NMTRIE tamBNLtemp = tamBloqueNL * __INC_BN__;
			if(numNL >= tamBNLtemp)
				tamBNLtemp = numNL * __INC_BN__;
			cout << "WARNING Resizing the array of data in the MergedTrie from " << tamBloqueNL << " to " << tamBNLtemp << "..." << endl;

			bloqueNChartemp = new T[tamBNLtemp];
	        if(!bloqueNChartemp) {
				cerr << "ERROR: Additional memory is required for the resizing of the array of data in the MergedTrie" << endl;
				exit(EXIT_MTRIE);
	        }
			memcpy(bloqueNChartemp, bloqueNChar_1, sizeof(T) * tamBloqueNL);
			memset(&(bloqueNChartemp[tamBloqueNL]), 0, sizeof(T) * (tamBNLtemp - tamBloqueNL));
			delete [] bloqueNChar_1;
			bloqueNChar_1 = bloqueNChartemp;
			bloqueNChartemp = NULL;

			tamBloqueNL = tamBNLtemp;

			cout << "\t\t\tResizing is concluded" << endl;
		}
	}

    bool insert(const string& s, const T& data)
    {
    	if(MergedTrie::insert(s)) {
    		if(MergedTrie::size() > BN0) {
    			AmpliarArrayDatos(MergedTrie::size(), BN0, bloque0conDatos);
    		}
    		bloque0conDatos[MergedTrie::size() - 1] = data;
    		return true;
    	}
    	else {
    		if(data != bloque0conDatos[DevolverIndiceDatoActual()]) {
    			bloque0conDatos[DevolverIndiceDatoActual()] = data;
    			return true;
    		}
    		else {
        		cerr << "The word \"" << s << "\" was already stored in the D_MergedTrie with data \"" << data << "\"" << endl;
    			return false;
    		}
    	}
    }

    bool insert(const string& s)
    {
    	cerr << "The word \"" << s << "\" has to be inserted with a data in the D_MergedTrie" << endl;
    	return false;
    }

    bool find(const string& s, T& data)
    {
    	if(MergedTrie::find(s)) {
    		data = bloque0conDatos[DevolverIndiceDatoActual()];
    		return true;
    	}
    	else {
    		return false;
    	}
    }

    bool find(const string& s)
    {
    	return MergedTrie::find(s);
    }

private:
    T* bloque0conDatos;	// Bloque 0 de nodos. Que contendra el grueso de los datos
    _INDICE_NMTRIE BN0;	// Tamaño del primer bloque de nodos dato. Este sera mayor para mejorar eficiencia
//	_INDICE_NMTRIE cantBloqueDatos;			// Numero de palabras que contiene el trie
};

#endif

