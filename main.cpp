#include "ConsoleUI.h"
#include "ConsoleColors.h"

int main() {
    Console::enableVirtualTerminalOutput(); // no-op on non-Windows platforms
    ConsoleUI ui;
    ui.run();
    return 0;
}