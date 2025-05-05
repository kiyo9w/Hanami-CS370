#include <iostream>
#include <string>
#include <vector>

// Garden: SimpleGarden (namespace omitted for simplicity)
struct Rose {
    public:
        void sayHello() {
            std::cout << std::string("Hello from Hanami Rose!\n");
            return;
        }

    private:
        int secretNumber = 42;
    protected:
        bool isFriendly = true;
};

int main() {
    std::string userName;
    std::cout << std::string("What's your name?");
    std::cin >> userName;
    Rose g;
    g.sayHello();
    if ((userName == std::string("Rose"))) {
        std::cout << std::string("You have a lovely name!\n");
    }
    else if ((userName == std::string("Lily"))) {
        std::cout << std::string("Another beautiful flower name!\n");
    }
    else {
        std::cout << std::string("Nice to meet you,") << userName << std::string("!\n");
    }
    return 0;
}

