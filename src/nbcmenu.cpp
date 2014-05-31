#include "aux.hpp"
#include "io.hpp"

int main(int argc, char* argv[]) {
    setlocale(LC_CTYPE, "");
    auto lines = IO::readIn();
    IO io;
    int rval = io.takeInput(std::move(lines));
    return rval;
}
