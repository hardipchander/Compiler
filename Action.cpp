#include "pch.h"

#include "Action.h"

namespace HelperFunc { 

	bool HelperFunc::isFuncHeaderInLine(std::string& line) {
		return ((line.find("(") != -1) && (line.find(")") != -1) && (line.find("while")==-1) && (line.find("for") == -1) && (line.find(";") == -1)) ? true : false;
	}

	void breakString(std::string& line, char breaker, std::vector<std::string>& storage) {
		std::stringstream list(line);
		std::string out;
		while (std::getline(list, out, breaker)) {
			storage.push_back(out);
		}
	}
}

