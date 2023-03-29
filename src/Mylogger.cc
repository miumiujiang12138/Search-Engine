 ///
 /// @file    Mylogger.cc
 /// @author  lemon(haohb13@gmail.com)
 /// @date    2022-10-24 10:02:57
 ///
 
#include "../include/Mylogger.hpp"

#include <iostream>

#include <log4cpp/Priority.hh>
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/FileAppender.hh>

using std::cout;
using std::endl;
using namespace log4cpp;

namespace wd
{
//初始化只放在实现文件，不用放在头文件中，
//否则就报多次重复定义的错误
Mylogger * Mylogger::_pInstance = nullptr;
Mylogger * Mylogger::getInstance()
{
	if(nullptr == _pInstance) {
		_pInstance = new Mylogger();
	}
	return _pInstance;
}

void Mylogger::destroy()
{
	if(_pInstance) {
		delete _pInstance;
		_pInstance = nullptr;
	}
}

Mylogger::Mylogger()
: _mycat(Category::getRoot().getInstance("SE"))
{
	auto ptn1 = new PatternLayout();
	ptn1->setConversionPattern("%d %c [%p] %m%n");

	auto pfile = new FileAppender("console", "../log/SE.log");
	pfile->setLayout(ptn1);

	_mycat.setPriority(Priority::DEBUG);
	_mycat.addAppender(pfile);
}

Mylogger::~Mylogger() {
	Category::shutdown();
	cout << "~Mylogger()" << endl;
}

void Mylogger::warn(const char *msg)
{
	_mycat.warn(msg);
}

void Mylogger::error(const char *msg)
{
	_mycat.error(msg);
}

void Mylogger::debug(const char *msg)
{
	_mycat.debug(msg);
}

void Mylogger::info(const char *msg)
{
	_mycat.info(msg);
}




}//end of namespace wd
