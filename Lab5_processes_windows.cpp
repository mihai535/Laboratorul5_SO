#include <windows.h>
#include <iostream>
#include <string>
using namespace std;

bool estePrim(int n) {
    if (n < 2) return false;
    for (int d = 2; d * d <= n; d++)
        if (n % d == 0) return false;
    return true;
}

int main(int argc, char* argv[]) {

    if (argc > 1 && string(argv[1]) == "copil") {
        int a, b;
        cin >> a >> b;
        for (int i = a; i <= b; i++)
            if (estePrim(i)) cout << i << " ";
        cout.flush();
        return 0;
    }

    const int NR = 10;
    HANDLE pipeCitire[NR];
    HANDLE proces[NR];

    char exe[MAX_PATH];
    GetModuleFileNameA(NULL, exe, MAX_PATH);

    SECURITY_ATTRIBUTES sa = { sizeof(sa), NULL, TRUE };

    for (int i = 0; i < NR; i++) {
        HANDLE inR, inW, outR, outW;
        CreatePipe(&inR, &inW, &sa, 0);
        CreatePipe(&outR, &outW, &sa, 0);

        SetHandleInformation(inW, HANDLE_FLAG_INHERIT, 0);
        SetHandleInformation(outR, HANDLE_FLAG_INHERIT, 0);

        STARTUPINFOA si = { sizeof(si) };
        si.dwFlags = STARTF_USESTDHANDLES;
        si.hStdInput = inR;
        si.hStdOutput = outW;
        si.hStdError = GetStdHandle(STD_ERROR_HANDLE);

        PROCESS_INFORMATION pi;
        string cmd = "\"" + string(exe) + "\" copil";

        if (!CreateProcessA(NULL, &cmd[0], NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
            cout << "Eroare la crearea procesului " << i + 1 << "\n";
            return 1;
        }

        CloseHandle(inR);
        CloseHandle(outW);
        CloseHandle(pi.hThread);

        string interval = to_string(i * 1000 + 1) + " " + to_string((i + 1) * 1000) + "\n";
        DWORD scris;
        WriteFile(inW, interval.c_str(), (DWORD)interval.size(), &scris, NULL);
        CloseHandle(inW);

        pipeCitire[i] = outR;
        proces[i] = pi.hProcess;
    }

    for (int i = 0; i < NR; i++) {
        cout << "Procesul " << i + 1 << " [" << i * 1000 + 1 << " - " << (i + 1) * 1000 << "]:\n";

        char buf[256];
        DWORD citit;
        while (ReadFile(pipeCitire[i], buf, sizeof(buf) - 1, &citit, NULL) && citit > 0) {
            buf[citit] = '\0';
            cout << buf;
        }
        cout << "\n\n";

        WaitForSingleObject(proces[i], INFINITE);
        CloseHandle(pipeCitire[i]);
        CloseHandle(proces[i]);
    }

    return 0;
}