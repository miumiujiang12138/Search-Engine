#ifndef __MYLOGGER_HPP__
#define __MYLOGGER_HPP__

#include <string>
#include <log4cpp/Category.hh>
using std::string;

//Category 
//Priority
//PatternLayout
//OstreamAppender/FileAppender

namespace wd
{

class Mylogger
{
public:
	static Mylogger * getInstance();
	static void destroy();
	void warn(const char *msg);
	void error(const char *msg);
	void debug(const char *msg);
	void info(const char *msg);

private:
	Mylogger();
	~Mylogger();

private:
	//
	log4cpp::Category & _mycat;
	static Mylogger * _pInstance;
};

}//end of namespace wd
 
// 定位信息用中括号: 【file:fuc:num】
//inline函数无法解决问题

//使用string的字符串拼接操作
#define addprefix(msg)  string("[").append(__FILE__)\
		  .append(":").append(__func__)\
		  .append(":").append(std::to_string(__LINE__))\
		  .append("] ").append(msg).c_str()
 

#define LogWarn(msg) wd::Mylogger::getInstance()->warn(addprefix(msg))
#define LogError(msg) wd::Mylogger::getInstance()->error(addprefix(msg))
#define LogInfo(msg) wd::Mylogger::getInstance()->info(addprefix(msg))
#define LogDebug(msg) wd::Mylogger::getInstance()->debug(addprefix(msg))
 
#endif
