﻿#include "boost/algorithm/string/classification.hpp"
#include "boost/algorithm/string/split.hpp"
#include <fstream>
#include <iostream>
#include <iomanip> 
#include "ImpliedVolExtractor.hpp"

const std::vector<std::string> headers = { "ID", "Spot","Strike","Risk-Free Rate","Years To Expiry","Option Type","Model Type","Implied Volatility","Market Price"};


ImpliedVolExtractor::ImpliedVolExtractor(const std::string inputFilename_, const std::string outputFilename_)
{
	inputFilename = inputFilename_;
	outputFilename = outputFilename_;

	std::string str;
	std::ifstream file(inputFilename);
	if (file.is_open())
	{
		while (getline(file,str)) rowCnt++;
		file.close();
	}
	else
	{
		throw "File " + inputFilename + " cannot be found!";
	}
};

void ImpliedVolExtractor::extractImpliedVols() {
	std::vector<std::string> tokens;
	std::string str;
	std::ifstream file(inputFilename);
	getline(file, str);	// Pass Headers
	if (file.is_open()) {
		while (getline(file, str))
		{
			boost::split(tokens, str, boost::is_any_of(","), boost::token_compress_on);
			ImpliedVolatility option(tokens);
			try {
				option.solveImpliedVol();
			}
			catch(...){
				std::cout << "Brent unable to solve for trade ID: " << tokens[0];
			}
			
			finalTradeOutput.push_back(OutputObj(
				option.getTradeID(), 
				option.getSpot(), 
				option.getStrike(), 
				option.getriskFreeRate(), 
				option.getTimToMat(), 
				option.getOptionType(),
				option.getModelType(),
				option.getOptionPrice(),
				option.getImpliedVol())
			);
		}
	}
	else
	{
		throw "File " + inputFilename + " cannot be found!";
	}
}

void ImpliedVolExtractor::writeImpliedVolsToFile() {
	std::ofstream outputfile;
	outputfile.open(outputFilename);
	outputfile.precision(12);
	for (std::string str : headers) { outputfile << str << ","; } outputfile << std::endl;

	for (OutputObj& trade : finalTradeOutput)
	{
		outputfile << trade.id << "," << trade.spot << "," << trade.strike << "," << trade.rate << ",";
		outputfile << trade.timMat << "," << trade.optionType << "," << trade.modelType << ",";
		(trade.impliedVol != 0 ? outputfile << trade.impliedVol : outputfile << "nan") << ",";
		outputfile << trade.marketPrice << std::endl;
	}
	outputfile.close();
}


