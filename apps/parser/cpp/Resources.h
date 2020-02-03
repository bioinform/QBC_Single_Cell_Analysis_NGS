#pragma once
#include <string>
#include <map>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include "../StringAnylizerGUI/kernel.h"
using boost::multi_index::multi_index_container;
using boost::multi_index::ordered_non_unique;
using boost::multi_index::ordered_unique;
using boost::multi_index::indexed_by;
using boost::multi_index::member;
using boost::multi_index::composite_key;
using boost::multi_index::nth_index;
using boost::multi_index::get;
using boost::multi_index::composite_key_compare;

typedef multi_index_container <
    Cell,
    indexed_by <

    ordered_unique<
    composite_key<
    Cell,
    member<Cell, string, &Cell::barcode>,
    member<Cell, string, &Cell::ahca>,
    member<Cell, string, &Cell::random>
    >
    >
    ,

    ordered_non_unique<
    composite_key<
    Cell,
    member<Cell, string, &Cell::barcode>,
    member<Cell, string, &Cell::ahca>,
    member<Cell, int, &Cell::num>,
    member<Cell, string, &Cell::random>
    >
    ,
    composite_key_compare<
    std::less<std::string>, // barcode sorted as by default
    std::less<std::string>, // ahca sorted as by default
    std::greater<int>, // sort large num first
    std::less<std::string>
    >
    >,

    ordered_non_unique<
    member<	Cell, string, &Cell::random>
    >,

    ordered_non_unique<
    member<Cell, string, &Cell::removeMe>
    >,

    ordered_non_unique<
    boost::multi_index::const_mem_fun< Cell, const std::string&, &Cell::uidsc3>
    >
    >
> CellHashList;


enum {
	COMPOSED_VIEW = 0,
	PRINT_VIEW,
	RANDOM_VIEW,
	SC3_VIEW,
	UIDSC3_VIEW,
};

typedef nth_index<CellHashList, COMPOSED_VIEW>::type composed_view;
typedef nth_index<CellHashList, PRINT_VIEW>::type print_view;
typedef nth_index<CellHashList, RANDOM_VIEW>::type random_view;
typedef nth_index<CellHashList, SC3_VIEW>::type SC3_view;

// a bunch of legacy landmine declarations from legacy MyMainClass.cpp
struct LegacyLandMines {
    // Time elapsed
    // seems to be unused
    // float readTime = 0;

    // seems to be unused
    //int mutations_Allowed = 1;
    std::string whereatthis = "";
    std::string countingme = "";
    /* seems to be not needed
    void ResetStatistics()
    {
        readTime = 0;
    }
    */


};
