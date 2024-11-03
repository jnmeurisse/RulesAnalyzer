/*!
* This file is part of RulesAnalyzer
*
* Copyright (C) 2024 Jean-Noel Meurisse
* SPDX-License-Identifier: GPL-3.0-only
*
*/
#include "tools/yesno.h"

#include <iostream>
#include "tools/strutil.h"

namespace rat {
	static bool check_answer(const std::string& answer)
	{
		return
			(answer == "") ||
			(answer == "y") ||
			(answer == "n") ||
			(answer == "yes") ||
			(answer == "no");
	}


	bool question_yesno(const std::string& message, bool default_yes)
	{
		std::string answer;
		std::string question = default_yes ? "[Y/n] " : "[y/N] ";

		do {
			std::cout << message << "? " << question;
			std::getline(std::cin, answer);
			answer = tolower(trim(answer));
		} while (!check_answer(answer));

		return default_yes ? answer == "" || answer[0] == 'y' : answer[0] == 'y';
	}

}
