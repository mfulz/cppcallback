#include <iostream>
#include <string>

#include "callback.hh"

class Button {
public:
    Button(const NCallback<void, int>& cb)
        : notify(cb)
    {
    }

    void click(int x = 5)
    {
        notify(x);
    }

private:
    NCallback<void, int> notify;
};

class AnotherButton {
public:
    AnotherButton(const NCallback<int, int, float, std::string>& cb)
        : notify(cb)
    {
    }

    int click(int x = 5, float y = 5.5, std::string z = "Hello World")
    {
        notify(x, y, z);
        return 42;
    }

private:
    NCallback<int, int, float, std::string> notify;
};

class MemberCallback {
public:
    void buttonCB(int x) { std::cout << "Callback called with x = " << x << std::endl; }
    int anotherButtonCB(int x, float y, std::string z)
    {
        std::cout << "Callback called with x = " << x << " y = " << y << " z = " << z << std::endl;
        return 42;
    }
};

void funcCallback(int x)
{
    std::cout << "Callback called with x = " << x << std::endl;
}

int main()
{
    MemberCallback cbClass;

    Button b1(NGenerateCallback((NCallback<void, int>*)0, cbClass, &MemberCallback::buttonCB));
    AnotherButton b2(NGenerateCallback((NCallback<int, int, float, std::string>*)0, cbClass, &MemberCallback::anotherButtonCB));
    Button b3(NGenerateCallback((NCallback<void, int>*)0, &funcCallback));

    b1.click(532);
    std::cout << "With return: " << b2.click(42, 43.5, "Hello Callback") << std::endl;
    b3.click();

    return 0;
}
