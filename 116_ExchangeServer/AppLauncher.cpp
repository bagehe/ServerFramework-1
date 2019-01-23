#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/file.h>
#include <stdlib.h>

#include "ExchangeApp.hpp"
#include "SignalUtility.hpp"
#include "NowTime.hpp"
#include "FileUtility.hpp"
#include "StringUtility.hpp"
#include "AppUtility.hpp"

using namespace ServerLib;

int main(int argc, char* *argv)
{
    bool bResume;
    int iWorldID;
    int iInstanceID;

    CAppUtility::AppLaunch(argc, argv, CExchangeApp::SetAppCmd, bResume, iWorldID, iInstanceID);

	CExchangeApp* pApp = new CExchangeApp;
    int iRet = pApp->Initialize(bResume);
    if (iRet)
    {
        exit(4);
    }

    pApp->Run();

    return 0;
}
