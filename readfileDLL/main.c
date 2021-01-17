#include "main.h"

#include <stdlib.h>
#include <stdio.h>

/** \brief
 *  //функция чтения
 * \param filename char* - имя файла
 * \return char* - считанный текст или NULL, если сломалось
 *
 */
DLL_EXPORT char * readFile(char * filename)
{
    FILE * file = fopen (filename, "rb");//rt
    if (file == NULL) return NULL;
    fseek(file,0, SEEK_END);
    size_t sizeBuf = ftell(file);
    fseek(file,0, SEEK_SET);

    char * text = (char *)malloc(sizeBuf+1);
    if (text == NULL) return NULL;
    fread(text,1,sizeBuf,file);
    text[sizeBuf]='\0';
    fclose(file);
    return text;
}

// a sample exported function
void DLL_EXPORT SomeFunction(const LPCSTR sometext)
{
    MessageBoxA(0, sometext, "DLL Message", MB_OK | MB_ICONINFORMATION);
}



DLL_EXPORT BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            // attach to process
            // return FALSE to fail DLL load
            break;

        case DLL_PROCESS_DETACH:
            // detach from process
            break;

        case DLL_THREAD_ATTACH:
            // attach to thread
            break;

        case DLL_THREAD_DETACH:
            // detach from thread
            break;
    }
    return TRUE; // succesful
}
