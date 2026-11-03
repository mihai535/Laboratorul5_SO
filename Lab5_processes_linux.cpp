#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
using namespace std;

bool estePrim(int n) {
    if (n < 2) return false;
    for (int d = 2; d * d <= n; d++)
        if (n % d == 0) return false;
    return true;
}

int main() {
    const int NR = 10;
    int pipeCitire[NR];
    pid_t copii[NR];

    for (int i = 0; i < NR; i++) {
        int p2c[2];   // parinte -> copil   ([0] = citire, [1] = scriere)
        int c2p[2];   // copil  -> parinte
        pipe(p2c);
        pipe(c2p);

        pid_t pid = fork();

        if (pid == 0) {
            close(p2c[1]);
            close(c2p[0]);

            int interval[2];
            read(p2c[0], interval, sizeof(interval));
            close(p2c[0]);

            for (int n = interval[0]; n <= interval[1]; n++)
                if (estePrim(n))
                    write(c2p[1], &n, sizeof(n));

            close(c2p[1]);
            return 0;
        }

        close(p2c[0]);
        close(c2p[1]);

        int interval[2] = { i * 1000 + 1, (i + 1) * 1000 };
        write(p2c[1], interval, sizeof(interval));
        close(p2c[1]);

        pipeCitire[i] = c2p[0];
        copii[i] = pid;
    }

    for (int i = 0; i < NR; i++) {
        cout << "Procesul " << i + 1 << " [" << i * 1000 + 1 << " - " << (i + 1) * 1000 << "]:\n";

        int n;
        while (read(pipeCitire[i], &n, sizeof(n)) > 0)
            cout << n << " ";
        cout << "\n\n";

        close(pipeCitire[i]);
        waitpid(copii[i], NULL, 0);
    }

    return 0;
}
