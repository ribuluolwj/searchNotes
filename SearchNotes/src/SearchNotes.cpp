#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <vector>
#include <algorithm>
#include <locale>
#include <codecvt>
#ifdef linux
#include <unistd.h>
#include <dirent.h>
#endif
#ifdef WIN32
#include <Windows.h>
#include <direct.h>
#include <shellapi.h>
#include <io.h>
#endif
using namespace std;
//<pre name="code", class="cpp">
/**
 * @function: Get all files' name in directory of /cate_dir/
 * @param: cate_dir - string type
 * @result: vector<string> type
 */
// Get all files list in gaving path
vector<string> getFiles(string cate_dir)
{
    vector<string> files; // save files name
    #ifdef WIN32
        _finddata_t file;
        long lf;
        // input directory
        if ((lf=_findfirst(cate_dir.c_str(), &file)) == -1)
        {
            cout << cate_dir << "Not found!!!" << endl;
        }
        else
        {
            while(_findnext(lf, &file) == 0)
            {
                // output files name
                // cout << file.name <<endl;
                if (strcmp(file.name, ".") == 0 || strcmp(file.name, "..") ==0)
                    continue;
                files.push_back(file.name);
            }
        }
        _findclose(lf);
    #endif

    #ifdef linux
        DIR *dir;
        struct dirent *ptr;
        char base[1000];
        if((dir=opendir(cate_dir.c_str())) == NULL)
        {
            perror("Open dir error...");
            exit(1);
        }
        while((ptr==readdir(dir)) != NULL)
        {
            if(strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0) //current dir OR parrent dir
                continue;
            else if (ptr->d_type ==8) //file
                //printf("d_name:%s/%s\n", basePath,ptr->d_name);
                files.push_back(ptr->d_name);
            else if(ptr->d_type ==  10) //link file
                //printf("d_name:%s/%s\n", basePath, ptr->d_name);
                continue;
            else if (ptr->d_type == 4) //dir
            {
                files.push_back(ptr->d_name);
                /**
                 * memset(base, '\0\', sizeof(base));
                 * strcpy(base, basePath);
                 * strcat(base, '/');
                 * strcat(base, ptr->d_nSame);
                 * readFileList(base);
                 */
            }
        }
        closedir(dir);
    #endif
    // Sort
    sort(files.begin(), files.end());
    return files;
}
// Convert wide char to normal char
char* wtoc(wchar_t* wText)
{
    DWORD dwNum = WideCharToMultiByte(CP_ACP, NULL, wText, -1, NULL, 0, NULL, FALSE);
    char* psText = NULL;
    psText = new char[dwNum];
    if (!psText)
    {
        delete[]psText;
        psText = NULL;
    }
    WideCharToMultiByte(CP_ACP, NULL, wText, -1, psText, dwNum, NULL, FALSE);
    return psText;
}
// Get data from clipboard
string getPlateStr()
{
    if (OpenClipboard(NULL))
    {
        HGLOBAL hGlobal = NULL;
        hGlobal = ::GetClipboardData(CF_UNICODETEXT);
        wchar_t* pGlobal = (wchar_t*)::GlobalLock(hGlobal);
        CloseClipboard();
        return wtoc(pGlobal);
    }
}
// line = UTF8ToGB(lineR.c_str()).c_str();
string UTF8ToGB(const char* str)
{
	string result;
	WCHAR *strSrc;
	LPSTR szRes;
 
	//获得临时变量的大小
	int i = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
	strSrc = new WCHAR[i + 1];
	MultiByteToWideChar(CP_UTF8, 0, str, -1, strSrc, i);
 
	//获得临时变量的大小
	i = WideCharToMultiByte(CP_ACP, 0, strSrc, -1, NULL, 0, NULL, NULL);
	szRes = new CHAR[i + 1];
	WideCharToMultiByte(CP_ACP, 0, strSrc, -1, szRes, i, NULL, NULL);
 
	result = szRes;
	delete[]strSrc;
	delete[]szRes;
 
	return result;
}

// Main program
int main(void)
{
    // Set 
    #ifdef WIN32
        SetConsoleOutputCP(65001);
//    system("chcp 936 && cls");
    #endif
    std::locale::global(std::locale("zh_CN.UTF-8"));
    // Get current directory path
    char current_address[256];
    //---init
    memset(current_address, 0, 256);
    //---get path
    _getcwd(current_address, 256); 
    //---show path
    std::cout << current_address << endl;
    strcat_s(current_address, "\\*");
    // Get files in path
    vector<string> pathFiles = getFiles((string)current_address);
    for (int i = 0; i < pathFiles.size(); i++)
    {
        std::cout << "--" << pathFiles[i] << endl;
    }
    locale china("zh_CN");
    wcin.imbue(china);
    wstring wnote;
    wstring note;
    while (1)
    {
        vector<wstring> noteMod;
        vector<int>noteTag;
        vector<int>modStart;
        wofstream outFile("SearchResaults.txt");
        // Read note to find 
        std::cout << "Searching: " << endl;
        getline(std::wcin,note);
        if (note.size() != 0)
        {
           std::wcout << note << endl;
           int modNum = 0;
           int position = 0;
           std::wcout << L"=====================================================" << endl;
           for (int i = 0; i < pathFiles.size(); i++)
           {
               if (pathFiles[i].substr(pathFiles[i].length() - 4) == ".txt" && pathFiles[i] != "SearchResaults.txt")
               {
                   std::cout << "Searching " << pathFiles[i] <<  "..." << endl;
                   wifstream fileInput(pathFiles[i]); // open file stream
                   wstring line;
                   while (getline(fileInput,line)) // read line, tell diff ulineInputg "\n", end with EOF
                   {
                       // Judge if line in files contain the note to search
                       if (!((line.find(L"//") != wstring::npos) || (line.find(L"----") != wstring::npos) || (line.size() == 0)))
                       {
                           if (noteMod.size() == 0)
                           {
                               noteMod.push_back(line);
                               position++;
                               if (line.find(L"页: ") != wstring::npos)
                               {
                                   modNum++;
                                   modStart.push_back(position);
                               }
               			    if (line.find(note) != wstring::npos)
                               {
                                   noteTag.push_back(modNum);
                               }
                           }
                           else
                           {
                               if (line != noteMod.back())
                               {
                                   noteMod.push_back(line);
                                   position++;
                                   if (line.find(L"页: ") != wstring::npos)
                                   {
                                       modNum++;
                                       modStart.push_back(position);
                                   }
               					if (line.find(note) != wstring::npos)
                                   {
                                       noteTag.push_back(modNum);
                                   }
                               }
                           }
                       }
                   }
                   fileInput.close();
               }
           }
           std::wcout << L"=====================================================" << endl;
           std::cout << noteTag.size() << endl;
           for (int i = 0; i < noteTag.size(); i++)
           {
               std::wcout << "Note model number: " << noteTag[i] - 1 << "\t" << "Model start position: " << modStart[noteTag[i] -1] << endl;
               for (int j = 0; j < (modStart[noteTag[i]] - modStart[noteTag[i] - 1]); j++)
               {
                   std::wcout << noteMod[modStart[noteTag[i] - 1] + j -1] << endl;
               }
               std::wcout << "\n----------------------------------------------------------------------------" << endl;
               outFile << "Note model number: " << noteTag[i] - 1 << "\t" << "Model start position: " << modStart[noteTag[i] - 1] << "\n";
               for (int j = 0; j < (modStart[noteTag[i]] - modStart[noteTag[i] - 1]); j++)
               {
                   outFile << noteMod[modStart[noteTag[i] - 1] + j -1] << "\n";
               }
               outFile << "\n----------------------------------------------------------------------------\n";
           }
           std::wcout << endl;
           std::wcout << "Note number: " << modNum << endl;
           std::wcout << "Line number: " << position << endl;
           std::wcout << "End searching...\n";
           outFile.close();
           // Open resluts file ulineInputg notepad
           WinExec("notepad.exe SearchResaults.txt", SW_SHOW);
        }
        system("pause");
    }
    return 0;
}