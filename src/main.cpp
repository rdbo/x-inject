#include "xinject/xinject.hpp"

bool CheckAccess()
{
#   if defined(MEM_WIN)
    return true; //the executable will always run as admin, so no need to check
#   elif defined(MEM_LINUX)
    if(getuid() == 0) return true; //user is root
    system("zenity --info --title=\"Error\" --text=\"Please, run as root\"");
#   endif

    return false;
}

int main(int, char**)
{
    if(CheckAccess())
        XInject().Run();
    return 0;
}