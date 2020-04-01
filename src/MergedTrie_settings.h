/*
 * MergedTrie_settings.h
 *
 *  Created on: 29/09/2016
 *      Author: Antonio Ferrandez Rodriguez antonio@dlsi.ua.es
 */

#ifndef ESQUELETOMTRIE_CONFIGURACION_H_
#define ESQUELETOMTRIE_CONFIGURACION_H_





///////////////////////////////////////// INICIO CONFIGURACION EXPERIMENTACION ///////////////////////////////
// Parametros a cambiar para los experimentos de los articulos

// Si esta definida esta variable se generara la version de distribucion
#define __CONFIGURACION_DISTRIBUCION__

#ifdef __CONFIGURACION_DISTRIBUCION__
	#define __CONFIGURACION_ARTICULO__
#endif


// Si esta definida esta variable se usa la configuracion de memoria de los resultados de revista
//#define __CONFIGURACION_ARTICULO__

// Si esta definida esta variable se ejecutara en LINUX. Sino en MS Windows
//#define LINUX

// Si esta definida esta variable, entonces lee/inicializa los MergedTries desde fichero "ficheroComprimido"
//#define TRIE_PRUEBA_DE_FICHERO

#ifndef __CONFIGURACION_ARTICULO__
	// Si esta definida esta variable se ejecutara con maxima comprobacion de seguridad (excepciones)
	#define __MODO_SEGURO__
#endif

// Donde se guardara la serializacion del MergedTrie
#define __DIRECTORY_TO_STORE__ "../Indexaciones/__fich_MergedTries"

// Si esta definida significa que se trabajara con multiples Tries compartiendo SharedMergedTrie. Sino, se trabajara directamente sobre ShMergedTrie shMergedTrie
//#define __MULTIPLE_TRIE__

// Numero maximo de MergedTries que pueden trabajar conjuntamente sobre la estructura shMergedTrie.  Define la variable shMergedTrie.numBitsEOW
#define __NUM_MAX_BITRIES__ 32

#ifdef __CONFIGURACION_ARTICULO__
	// B al insertar para los nodos Letra [0..256]. Deberia ser potencia de 2 inicialmente
	#define __B_INICIAL_NL__ 8
	// B del NL raiz [0..256]. Deberia ser potencia de 2 inicialmente
	#define __B_INICIAL_NL_RAIZ__ 256

	// Valor que daria al B inicial para los nodos IT [0..16383]. Deberia ser potencia de 2 inicialmente
	#define __B_INICIAL_NIT__ 16

	// Define el incremento que se suma a la THNIT al ampliarla. Es conveniente que sea impar y no multiplo de 2
	#define __INC_BNIT__ 45
#else
	// B al insertar para los nodos Letra [0..256]. Deberia ser potencia de 2 inicialmente
	#define __B_INICIAL_NL__ 2
	// B del NL raiz [0..256]. Deberia ser potencia de 2 inicialmente
	#define __B_INICIAL_NL_RAIZ__ 2

	// Valor que daria al B inicial para los nodos IT [0..16383]. Deberia ser potencia de 2 inicialmente
	#define __B_INICIAL_NIT__ 2

	// Define el incremento que se suma a la THNIT al ampliarla. Es conveniente que sea impar y no multiplo de 2
	#define __INC_BNIT__ 0
#endif

#ifdef TRIE_PRUEBA_DE_FICHERO
	// Define el tamanyo del array de NodosLetra
	#define __Tam_tamBNL__ 300

	// Define el tamanyo del array de NodosChar
	#define __Tam_tamBChar__ 5

	// Define el tamanyo del array de NodosIT
	#define __Tam_tamBNIT__ 5
#else
	#ifdef __CONFIGURACION_DISTRIBUCION__
		// Define el tamanyo del array de NodosLetra
		#define __Tam_tamBNL__   8000000

		// Define el tamanyo del array de NodosChar
		#define __Tam_tamBChar__ 800000

		// Define el tamanyo del array de NodosIT
		#define __Tam_tamBNIT__  24000000
	#else
		#ifdef __CONFIGURACION_ARTICULO__
			// Define el tamanyo del array de NodosLetra
		//	#define __Tam_tamBNL__   1000000
			#define __Tam_tamBNL__   113000000

			// Define el tamanyo del array de NodosChar
		//	#define __Tam_tamBChar__ 1000000
			#define __Tam_tamBChar__ 12000000

			// Define el tamanyo del array de NodosIT
		//	#define __Tam_tamBNIT__   1000000
			#define __Tam_tamBNIT__  260000000
		#else
			// Define el tamanyo del array de NodosLetra
			#define __Tam_tamBNL__ 1000000
	//		#define __Tam_tamBNL__ 10

			// Define el tamanyo del array de NodosChar
			#define __Tam_tamBChar__ 1000000
	//		#define __Tam_tamBChar__ 10

			// Define el tamanyo del array de NodosIT
			#define __Tam_tamBNIT__ 1000000
	//		#define __Tam_tamBNIT__ 2
		#endif
	#endif
#endif

// Define el tamanyo del array de datos del MergedTrie
#define __Tam_tamDataMergedTrie__ __Tam_tamBNIT__

// Define el incremento por el que se multiplican los bloques de nodos
#define __INC_BN__ 2








///////////////////////////////////////// INICIO CONFIGURACION GENERAL ///////////////////////////////
// Demas aspectos de configuracion de disenyo general

#define _INDICE_NMTRIE unsigned int
#define _OFFSET_NMTRIE unsigned int
#define _NMTRIE_ nodoBloqueNL
#define _NIT_ unsigned int

// Si queremos hacer mas funciones inline, aunque el ejecutable ocupe mas
//#define __ES_INLINE__ inline
// Caso contrario
#define __ES_INLINE__

// Si queremos que se ejecute solo la parte de NL y no la de NIT (para comprobar distribucion de tiempos entre busqNL y busqNIT)
//#define __BUSQ_SOLO_NL__


// Constantes para la salida de programa por error
#define EXIT_FALTA_MEMORIA 2
#define EXIT_STEMMER 3
#define EXIT_LISTA 4
#define EXIT_TRIE_COMPRIMIDO 5
#define EXIT_INTERFAZ 6
#define EXIT_MTRIE 7


///////////////////////////////////////// FIN CONFIGURACION ///////////////////////////////

#endif

