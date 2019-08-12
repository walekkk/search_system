#include "./search_client.h"

void PrintHelp(std::string varsion) {
    std::cout << "******************************************************************" <<std::endl;
    std::cout << "****************   walcom to search client  **********************" <<std::endl;
    std::cout << "********************** varsion: "<< varsion <<" ******************************" <<std::endl;
    std::cout << "******************************************************************" <<std::endl;
    std::cout << "******************************************************************" <<std::endl;
}

int32_t Choose() {
    std::cout << "1. insert url" << std::endl;
    std::cout << "2. search word" << std::endl;
    std::cout << "3. close client" << std::endl;
    int result = 0;
    std::cout << ">>";
    std::cin >> result;
    std::cout << std::endl;
}

bool UseClient(int result) {
    SearchClient* pClient = new SearchClient();
    if (!pClient->Init()) {
        return false;
    }
    std::string sendData("");
    std::string response("");
    if (result == 1) {
        sendData += "insert::";
        std::cout << "url>>";
        std::string url("");
        std::cin >> url;
        sendData += url;
        if (!pClient->SendDataAndGetResult(sendData, response)) {
            LOG(ERROR, "send data error");
            DELETE_AND_SET_NULL(pClient);
            return false;
        }
    } else {
        sendData += "search::";
        std::cout << "search>>";
        std::string word("");
        std::cin >> word;
        sendData += word;
        if (!pClient->SendDataAndGetResult(sendData, response)) {
            LOG(ERROR, "send data error");
            DELETE_AND_SET_NULL(pClient);
            return false;
        }
    }
    //这里获取到了response直接打印，当然可以存储等
    std::cout << response << std::endl;
    DELETE_AND_SET_NULL(pClient);
    return true;
}

int main() {
    PrintHelp("1.1");
    while (true) {
        int result = Choose();
        if (result == 1) {
            if (!UseClient(result)) {
                return false;
            }
        } else if (result == 2) {
            if (!UseClient(result)) {
                return false;
            }
        } else if (result == 3) {
            std::cout << "client close success!" << std::endl;
            break;
        } else {
            std::cout << "dont have this choose, try again!" << std::endl;
        }
    }
    return 0;
}
