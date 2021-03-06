/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.1.0-dev
             DATE     : 02/2018
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MALT_HELPERS_HPP
#define MALT_HELPERS_HPP

/********************  HEADERS  *********************/
//STL C++
#include <ostream>

/*******************  NAMESPACE  ********************/
namespace MALT
{

/********************  STRUCT  **********************/
/**
 * Struct to aggregate all global helper functions (might be better to be a namespace).
 * 
 * @brief Simple container to aggregate some helper functions.
**/
struct Helpers
{
	static void printValue(std::ostream & out,double value, const char* unit = "");
	static int getFileId(void);
};

}

#endif //MALT_HELPERS_HPP
