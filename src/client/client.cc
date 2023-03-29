#include "../../lib/nlohmann/json.hpp"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>

using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::istringstream;
using std::ifstream;
using std::vector;
using std::map;
using nlohmann::json;

#define MSG_SIZE 10240

struct Message{
    int length;
    int id;
    char message[MSG_SIZE];
};
 
int Connect() 
{
	int clientfd = ::socket(AF_INET, SOCK_STREAM, 0);
	if(clientfd < 0) 
    {
		perror("socket");
		exit(0);
	}

	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(8888);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	socklen_t length = sizeof(serverAddr);

	if(::connect(clientfd,(struct sockaddr*)&serverAddr, length) < 0) 
    {
		perror("connect");
		close(clientfd);
		exit(0);
	}
	printf("conn has connected!...\n");

    return clientfd;
}

void Interaction(int clientfd){
	while(1)
    {
        int stat = 0;
		string line, word;
        char ch;
        while(!stat){
		    cout << "*****************  MENU  ****************" << endl;
            cout << "1. Keyword Query." << "\n"
                 << "2. Web Search." << "\n"
                 << "3. Exit." << "\n"
                 << "please input '1' '2' or '3':\n>> ";
            int option = 4;
            ::scanf("%d", &option);
            while((ch = ::getchar()) != EOF && ch != '\n');
            switch (option){
            case 1:
                stat = 1;
                cout << "please input keyword for Keyword Query:\n>> ";
                break;
            case 2:
                stat = 2;
                cout << "please input keyword for Web Search:\n>> ";
                break;
            case 3:
                return;
            default:
                cout << "Error, resume load!" << endl;
                break;
            }
        }
        Message mess;
		getline(cin, line);
        //istringstream iss(line);
        //iss >> word;

        mess.length = 2 * sizeof(int) + line.size();
        mess.id = stat;
        ::sprintf(mess.message, "%s", line.data());

		send(clientfd, &mess, sizeof(mess), 0);
		::memset(&mess, 0, sizeof(mess));
        recv(clientfd, &mess, sizeof(mess), 0);
        string result = mess.message;
        json j = json::parse(result.c_str());
        vector<string> words;
        if(mess.id == 100){
            cout << "\nkeyword recommendation result of " << "\033[35m" << j["keyword"] << "\033[0m:" << endl;
            for(int i = 0; i < 5; ++i){
                cout << "\033[34m" << j["result"][i] << "\033[0m, ";
            }
            cout << endl;
        }else if(mess.id == 200){
            cout << "\033[31m"<< "\nweb search result of " << j["web search"] << "\033[0m" << endl;
            for(int i = 1; i < 11; ++i){
                if(j["web search result"][i]["title"] == nullptr) break;
                cout << "\n\033[32mabstract:\033[0m " << j["web search result"][i]["abstract"] << endl;
                cout << "\033[32mtitle:\033[0m " << j["web search result"][i]["title"] << endl;
                cout << "\033[32murl:\033[0m " << "\033[4m\033[34m" << j["web search result"][i]["url"] << "\033[0m" << endl;
            }
        }else{
            cout << "\nNo results of found about " << line << endl;
        }

        cout << "\nPress any key and Enter to continue...";
        ::getchar();
        while((ch = ::getchar()) != EOF && ch != '\n');
        system("clear");
	}

} 

void UseTestFile(int clientfd, string filename, int id){
    ifstream ifs(filename);
    if(!ifs.good()){
        cout << "Failed to open testkw.txt" << endl;
        return;
    }
    string word;
    vector<string> keywords;
    map<string, vector<string>> result;
    keywords.reserve(20);
    while(ifs >> word){
        keywords.push_back(word);
    }
    ifs.close();

    if(id == 1) cout << "\n\t\t\033[32mKeyword Recommendation\033[0m" << endl;
    else cout << "\n\t\t\033[32mWeb Search\033[0m" << endl;
    Message mess;
    for(auto & keyword : keywords){
        mess.id = id;
        mess.length = 2 * sizeof(int) + keyword.size();
        ::sprintf(mess.message, "%s", keyword.c_str());
        //sleep(1);
        send(clientfd, &mess, sizeof(mess), 0);
        ::memset(&mess, 0, sizeof(mess));
        recv(clientfd, &mess, sizeof(mess), 0);
        json j = json::parse(mess.message);
        vector<string> words;
        if(mess.id == 100){
            cout << "\nkeyword recommendation result of " << "\033[35m" << j["keyword"] << "\033[0m:" << endl;
            for(int i = 0; i < 5; ++i){
                cout << "\033[34m" << j["result"][i] << "\033[0m ";
            }
            cout << endl;
        }else if(mess.id == 200){
            cout << "\033[31m"<< "\nweb search result of " << j["web search"] << "\033[0m" << endl;
            for(int i = 1; i < 11; ++i){
                if(j["web search result"][i]["title"] == nullptr) break;
                cout << "\n\033[32mabstract:\033[0m " << j["web search result"][i]["abstract"] << endl;
                cout << "\033[32mtitle:\033[0m " << j["web search result"][i]["title"] << endl;
                cout << "\033[32murl:\033[0m " << "\033[4m\033[34m" << j["web search result"][i]["url"] << "\033[0m" << endl;
            }
        }else{
            cout << "\nNo results of found about " << keyword << endl;
        }
    }
}
 
int main()
{
    int clientfd = Connect();
    while(1){
        cout << "You can choose between interactive input or open the test file to test the automated tests." << endl
            << "1. interactive input" << endl
            << "2. using test file" << endl
            << "3. exit" << endl << endl
            << "pls input your choose: ";

        int option = 4;
        char ch;
        ::scanf("%d", &option);
        while((ch = ::getchar()) != EOF && ch != '\n');
        switch (option){
        case 1:
            Interaction(clientfd);
            break;
        case 2:
            UseTestFile(clientfd, "testkw.txt", 1);
            UseTestFile(clientfd, "testwq.txt", 2);
            break;
        case 3:
            close(clientfd);
            return 0;
        default:
            cout << "error input." << endl;
        }
    }
	close(clientfd);
	return 0;
}
